#define N_IMPLEMENTS nOggStream
//------------------------------------------------------------------------------
//  noggstream_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio/noggstream.h"
#include "kernel/nfileserver2.h"

nNebulaClass(nOggStream, "nstream");

//------------------------------------------------------------------------------
/**
    Static callback function. Ogg Vorbis calls this when it needs to
    read data from an ogg file.
*/
size_t
nOggStream::readFunc(void* ptr, size_t size, size_t nmemb, void* datasource)
{
    nFile* file = (nFile*) datasource;
    n_assert(file);
    return file->Read(ptr, size * nmemb);
}

//------------------------------------------------------------------------------
/**
    Static callback function. Ogg Vorbis calls this when it needs to
    seek into an ogg file.
*/
int
nOggStream::seekFunc(void* datasource, ogg_int64_t offset, int whence)
{
    nFile* file = (nFile*) datasource;
    n_assert(file);

    nFile::nSeekType seekType;
    switch (whence)
    {
        case SEEK_CUR: seekType = nFile::CURRENT; break;
        case SEEK_END: seekType = nFile::END; break;
        case SEEK_SET: seekType = nFile::START; break;
        default:       seekType = nFile::START; break; 
    }
    return file->Seek((int) offset, seekType) ? 1 : 0;
}

//------------------------------------------------------------------------------
/**
    Static callback function. Ogg Vorbis calls this when it needs to
    close a file.
*/
int
nOggStream::closeFunc(void* datasource)
{
    nFile* file = (nFile*) datasource;
    n_assert(file);
    file->Close();
    return 0;
}

//------------------------------------------------------------------------------
/**
    Static callback function. Ogg Vorbis calls this when it needs to
    tell the current position in a file.
*/
long
nOggStream::tellFunc(void* datasource)
{
    nFile* file = (nFile*) datasource;
    n_assert(file);
    return file->Tell();
}


//------------------------------------------------------------------------------
/**
*/
nOggStream::nOggStream() :
    file(0),
    numSamples(0)
{
    memset(&(this->oggFile), 0, sizeof(this->oggFile));
}

//------------------------------------------------------------------------------
/**
    Open an ogg file for reading.
*/
bool
nOggStream::Open(const char* name, int openFlags)
{
    n_assert(!this->isOpen);
    n_assert(name);
    n_assert(0 == this->file);

    // open the ogg file
    this->file = this->refFileServer->NewFileObject();
    n_assert(this->file);
    if (this->file->Open(name, "rb"))
    {
        // setup custom stream callbacks (redirects through nFile)
        ov_callbacks oggCallbacks;
        oggCallbacks.read_func  = nOggStream::readFunc;
        oggCallbacks.seek_func  = nOggStream::seekFunc;
        oggCallbacks.close_func = nOggStream::closeFunc;
        oggCallbacks.tell_func  = nOggStream::tellFunc;

        // open the ogg stream
        if (0 > ov_open_callbacks((void*)this->file, &(this->oggFile), 0, 0, oggCallbacks))
        {
            // failed!
            n_printf("nOggStream: could not open '%s' as ogg file!\n", name);
            this->file->Close();
            goto fail;
        }

        // get info about ogg file
        vorbis_info* oggInfo = ov_info(&(this->oggFile), -1);
        n_assert(oggInfo);

        this->SetBaseFreq(oggInfo->rate);
        this->SetNumChannels(oggInfo->channels);
        this->numSamples   = (int)ov_pcm_total(&(this->oggFile), -1);
        if (this->numSamples == OV_EINVAL)
        {
            n_printf("nOggStream: Error in '%s':  bitstream is unseekable\n", name);
            this->file->Close();
            goto fail;
        }
        return nStream::Open(name, openFlags);
    }

fail:
    // fail
    delete this->file;
    this->file = 0;
    return false;
}

//------------------------------------------------------------------------------
/**
    Close the ogg file and associated stuff.
*/
void
nOggStream::Close()
{
    n_assert(this->isOpen);
    n_assert(this->file);

    ov_clear(&(this->oggFile));
    
    // do NOT close file, ogg vorbis does this for us
    delete this->file;
    this->file = 0;
    nStream::Close();
}

//------------------------------------------------------------------------------
/**
    Stream data from ogg file.
*/
int
nOggStream::Read(char* buf, int num)
{
    n_assert(this->isOpen);
    n_assert(this->file);
    short* xFadeBuf  = (short*) buf;
    int xFadeNum     = num >> 1;
    int allBytesRead = 0;

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
        int curSample = (int)(ov_pcm_tell(&(this->oggFile)));
        int bitstream = 0;
        int bytesLeft = num;
        while (bytesLeft > 0)
        {
            int actuallyRead = ov_read(&(this->oggFile),        // pointer to ogg vorbis file handle
                                       buf,                     // output buffer
                                       bytesLeft,               // number of bytes to read
                                       0,                       // little endian format
                                       2,                       // 2 bytes per sample
                                       1,                       // sample data is signed
                                       &bitstream);
            n_assert(actuallyRead >= 0);  // make sure ov_read was successful
            int leftToRead = (this->numSamples - curSample)*2;
            if (actuallyRead >= leftToRead)
            {
                actuallyRead = leftToRead;
                if (this->isLooping)
                {
                    // in loop mode seek back to beginning of data
                    ov_pcm_seek_page(&(this->oggFile), 0);
                    curSample = 0;
                }
                else
                {
                    // in oneshot mode, fill the rest of the buffer with zero's
                    memset(buf, 0, num - allBytesRead);
                    this->isEof = true;
                    this->ModulateXFadeBuffer(xFadeBuf, xFadeNum);
                    return allBytesRead;
                }
            }
            else curSample += actuallyRead/2;

            allBytesRead += actuallyRead;
            bytesLeft    -= actuallyRead;
            buf          += actuallyRead;
        }
    }

    // cross fade: modulate content of cross fade buffer into new data
    this->ModulateXFadeBuffer(xFadeBuf, xFadeNum);
    return num;
}

//------------------------------------------------------------------------------
/**
    @brief tell current position in OGG file
    returns uncompressed audio sample position within the OGG file.
    (i.e.  not bytes!)
*/
int 
nOggStream::Tell(void)
{
    n_assert(this->isOpen);
    n_assert(this->file);
    int pos = (int)(ov_pcm_tell(&(this->oggFile)));
    if (pos >= this->numSamples) 
    {
        if (this->isLooping) 
        {
            pos = 0;
        }
        else
        {
            pos = numSamples;
        }
    }
    return pos;
}

//------------------------------------------------------------------------------
/**
    @brief seek to "pos" in OGG file
    pos = uncompressed audio sample position within the OGG file.
    (i.e.  not bytes!)
*/
void 
nOggStream::Seek(int pos)
{
    n_assert(this->isOpen);
    n_assert(this->file);
    n_assert((pos >= 0) && (pos <= this->numSamples));

    if ((pos == this->numSamples) && (!this->isLooping)) 
    {
        this->isEof = true;
    } 
    else 
    {
        this->isEof = false;
    }
    if (ov_pcm_tell(&(this->oggFile)) != pos) 
    {
        ov_pcm_seek(&(this->oggFile), pos);
    }
}

//------------------------------------------------------------------------------
