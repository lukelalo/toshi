#define N_IMPLEMENTS nWavStream
//-------------------------------------------------------------------
//  nwavstream_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "audio/nwavstream.h"
#include "kernel/nfileserver2.h"

#define swapul(a) (ulong)(((ulong)a>>24)+(((ulong)a&0xff0000)>>8)+(((ulong)a&0xff00)<<8)+(((ulong)a&0xff)<<24))

nNebulaClass(nWavStream, "nstream");

//------------------------------------------------------------------------------
/**
    28-May-00   floh    created
*/
nWavStream::nWavStream() :
    file(0),
    bitsPerSample(0),
    dataStart(0),
    dataLen(0)
{
    // empty
}

//-------------------------------------------------------------------
/**
    16-May-00   floh    created
*/
ulong 
nWavStream::GetFourCC()
{
    n_assert(this->file);
    ulong i;
    this->file->Read(&i, sizeof(i));
    return swapul(i);
}

//-------------------------------------------------------------------
/**
    16-May-00   floh    created
*/
ulong 
nWavStream::GetULong()
{
    n_assert(this->file);
    ulong i;
    this->file->Read(&i, sizeof(i));
    return i;
}

//-------------------------------------------------------------------
/**
    16-May-00   floh    created
*/
ushort 
nWavStream::GetUShort()
{
    n_assert(this->file);
    ushort i;
    this->file->Read(&i, sizeof(i));
    return i;
}

//-------------------------------------------------------------------
/**
    @brief Open wav file for reading, store relevant information and
    position at beginning of data chunk.

    28-May-00   floh    created
*/
bool 
nWavStream::Open(const char *name, int openFlags)
{
    n_assert(name);
    n_assert(0 == this->file);

    this->file = this->refFileServer->NewFileObject();
    n_assert(file);
    if (this->file->Open(name, "rb"))
    {
        ulong l;
        ushort s;
        
        // read header
        l = this->GetFourCC();
        if (l != 'RIFF') 
        {
            n_printf("nWavStream::Open(%s): not a wav file.\n", name);
            goto fail;
        }

        // skip length field
        l = this->GetULong();
        
        // read wave fourcc
        l = this->GetFourCC();
        if (l != 'WAVE') 
        {
            n_printf("nWavStream::Open(%s): not a wav file.\n", name);
            goto fail;
        }
        
        // read format fourcc
        l = this->GetFourCC();
        if (l != 'fmt ') 
        {
            n_printf("nWavStream::Open(%s): not a wav file.\n", name);
            goto fail;
        }
        // read len of format chunk and compute position of its end
        l = this->GetULong();
        int endOfChunk = this->file->Tell() + l;
        
        // skip format specifier
        s = this->GetUShort();

        // read number of channels
        s = this->GetUShort();
        if (s > 2) 
        {
            n_printf("nWavStream::Open(%s): not a mono or stereo file!\n", name);
            goto fail;
        }
        this->SetNumChannels(s);

        // read sample rate (hz)
        l = this->GetULong();
        this->SetBaseFreq(l);

        // skip bytes per second, bytes per sample
        l = this->GetULong();
        s = this->GetUShort();

        // read bits per channel
        s = this->GetUShort();
        if (s != 16) 
        {
            n_printf("nWavStream::Open(%s): not a 16 bit file!\n", name);
            goto fail;
        }
        this->bitsPerSample = s;

        // find data chunk 
        ulong cc;
        ulong len = 0;
        do 
        {
            this->file->Seek(endOfChunk, nFile::START);
            cc = this->GetFourCC();
            len = this->GetULong();
            endOfChunk = this->file->Tell() + len;
        } while (cc != 'data');

        // keep position and len of data chunk
        this->dataStart = this->file->Tell();
        this->dataLen   = len;

        return nStream::Open(name, openFlags);
    }

fail:
    if (this->file) 
    {
        delete this->file;
        this->file = 0;
    }
    return false;
}

//-------------------------------------------------------------------
/**
    28-May-00   floh    created
*/
void 
nWavStream::Close(void)
{
    n_assert(this->isOpen);
    n_assert(this->file);
    this->file->Close();
    delete this->file;
    this->file = 0;
    nStream::Close();
}

//-------------------------------------------------------------------
/**
    28-May-00   floh    created
*/
int 
nWavStream::Read(char *buf, int num)
{
    n_assert(this->isOpen);
    n_assert(this->file);
    short* xFadeBuf = (short*) buf;
    int xFadeNum     = num >> 1;

    // cross fade: copy current buffer contents
    this->UpdateXFadeBuffer(xFadeBuf, xFadeNum);

    if (this->isEof) 
    {
        // file is exhausted, just fill with zero's AND RETURN 
        // ACTUAL NUMBER OF BYTES READ!
        memset(buf, 0, num);
        num = 0;
    } 
    else 
    {
        // file not exhausted, stream from file
        int bytesLeft = num;
        while (bytesLeft > 0) 
        {
            // check if this read operation would go past the end of this file
            int readBytes = bytesLeft;
            int pos = this->file->Tell() - this->dataStart;
            if ((pos + readBytes) > this->dataLen)
            {
                readBytes = this->dataLen - pos;
            }
            int actuallyRead = this->file->Read(buf, readBytes);
            n_assert(actuallyRead == readBytes);
            if (bytesLeft != readBytes) 
            {
                if (!this->isLooping) 
                {
                    // end of file reached and not in loop mode
                    // fill rest with 0's, AND RETURN ACTUAL NUMBER
                    // OF BYTES READ!
                    memset(buf + readBytes, 0, num - readBytes);
                    this->isEof = true;
                    this->ModulateXFadeBuffer(xFadeBuf, xFadeNum);
                    return readBytes;
                } 
                else 
                {
                    // seek back to beginning of data
                    this->file->Seek(this->dataStart, nFile::START);
                }
            }
            bytesLeft -= readBytes;
            buf       += readBytes;
        }
    }

    // cross fade: modulate content of cross fade buffer into new data
    this->ModulateXFadeBuffer(xFadeBuf, xFadeNum);

    return num;
}

//-------------------------------------------------------------------
/**
    28-May-00   floh    created
*/
int 
nWavStream::Tell(void)
{
    n_assert(this->isOpen);
    n_assert(this->file);
    int pos = this->file->Tell() - this->dataStart;
    if (pos >= this->dataLen) 
    {
        if (this->isLooping) 
        {
            pos = 0;
        }
        else
        {
            pos = dataLen;
        }
    }
    return pos;
}

//-------------------------------------------------------------------
/**
    18-May-00   floh    created
    29-May-00   floh    + set the exhausted flag based on the
                          filepos and looping status
*/
void 
nWavStream::Seek(int pos)
{
    n_assert(this->isOpen);
    n_assert(this->file);
    n_assert((pos >= 0) && (pos <= this->dataLen));

    if ((pos == this->dataLen) && (!this->isLooping)) 
    {
        this->isEof = true;
    } 
    else 
    {
        this->isEof = false;
    }
    int realPos = this->dataStart + pos;
    if (this->file->Tell() != realPos) 
    {
        this->file->Seek(realPos, nFile::START);
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
