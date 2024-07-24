#define N_IMPLEMENTS nStream
//-------------------------------------------------------------------
//  nstream_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "audio/nstream.h"
#include "math/nmath.h"

nNebulaClassStaticInit(nStream, "nroot", nStream::InitStaticData);

float nStream::xFadeTable[nStream::XFADESIZE];

//------------------------------------------------------------------------------
/**
    Initialize the static 
*/
void
nStream::InitStaticData()
{
    // fill the cross fade modulation table
    float curRad = 0.0f;
    float diffRad = N_PI / float(XFADESIZE);
    int i;
    for (i = 0; i < XFADESIZE; i++, curRad += diffRad)
    {
        xFadeTable[i] = (n_cos(curRad) + 1.0f) * 0.5f;
    }
}

//------------------------------------------------------------------------------
/**
*/
nStream::nStream() :
    refFileServer(kernelServer, this),
    isOpen(false),
    isLooping(false),
    isEof(false),
    xFadeEnabled(false),
    baseFreq(0),
    xFadeWriteIndex(0),
    xFadeReadIndex(0)
{
    this->refFileServer = "/sys/servers/file2";
}

//------------------------------------------------------------------------------
/**
*/
nStream::~nStream()
{
    if (this->isOpen)
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Open the stream.

    @param  name        filename of stream file
    @param  openFlags   nStream::LOOPING if this is a looping file
*/
bool 
nStream::Open(const char *name, int openFlags)
{
    n_assert(name);
    n_assert(this->fileName.IsEmpty());
    n_assert(!this->isOpen);
    
    this->fileName = name;
    this->isLooping = (openFlags & LOOPING) ? true : false;
    this->isEof = false;
    this->isOpen = true;
    this->xFadeEnabled = false;
    return true;
}

//------------------------------------------------------------------------------
/**
    28-May-00   floh    created
*/
void 
nStream::Close(void)
{
    n_assert(this->isOpen);
    this->fileName.Set(0);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    28-May-00   floh    created
*/
int 
nStream::Read(char *, int)
{
    n_error("Pure virtual function called!\n");
    return 0;
}

//------------------------------------------------------------------------------
/**
    28-May-00   floh    created
*/
int 
nStream::Tell(void)
{
    n_error("Pure virtual function called!\n");
    return 0;
}

//------------------------------------------------------------------------------
/**
    28-May-00   floh    created
*/
void 
nStream::Seek(int)
{
    n_error("Pure virtual function called!\n");
}

//------------------------------------------------------------------------------
/**
    Go into cross fade mode. The following Read()'s will be modulated with the
    current contents of the stream buffer to generate a smooth cross
    fade should help to prevent crackles...

    NOTE: CROSS FADING WILL ONLY WORK 100% CORRECT WITH 1-CHANNEL SAMPLES,
    HOWEVER, MULTICHANNEL SAMPLES SHOULD WORK TOO, BUT THE SAMPLE DATA
    WILL BE TREATED AS 1-DIMENSIONAL.
*/
void
nStream::BeginXFade()
{
    this->xFadeEnabled = true;
    this->xFadeReadIndex = 0;
    this->xFadeWriteIndex = 0;
}

//------------------------------------------------------------------------------
/**
    Leave cross fade mode. Data in the stream buffer will be overwritten.
*/
void
nStream::EndXFade()
{
    n_assert(this->xFadeEnabled);
    this->xFadeEnabled = false;
}

//------------------------------------------------------------------------------
/**
    Write samples into the crossfade buffer at the current write position, but
    only until the write buffer is full. Returns actual number of samples written.
    If the xFadeBuffer is already full, nothing will be written and the
    method does essentially nothing. If crossfading is disabled the method
    will do nothing as well.

    @param  src             points to 16 bit samples to copy into crossfade buffer
    @param  numSamples      number of sample to copy
    @return                 number of samples actually copied
*/
int
nStream::UpdateXFadeBuffer(short* src, int numSamples)
{
    // cross fading enabled at all?
    if (!this->xFadeEnabled)
    {
        return 0;
    }

    // clamp number of samples if necessary
    if ((this->xFadeWriteIndex + numSamples) >= XFADESIZE)
    {
        numSamples = XFADESIZE - this->xFadeWriteIndex;
    }

    // copy samples and update write index
    if (numSamples > 0)
    {
        memcpy(&(this->xFadeBuffer[this->xFadeWriteIndex]), src, numSamples * sizeof(short));
        this->xFadeWriteIndex += numSamples;
    }
    return numSamples;
}

//------------------------------------------------------------------------------
/**
    Modulate samples pointed to by src with the contents of the xFadeBuffer
    at the current read position of the xFadeBuffer, and write the
    modulated value to the dst pointer. src and dst may point to the
    same location. The method stops if the read index of the
    crossfade buffer reaches the end of the buffer. If cross fading is
    not enabled, the method will do nothing.

    @param  src         points to source data which will be modified (16 bit samples)
    @param  numSamples  number of samples to process
    @return             number of samples actually processed
*/
int
nStream::ModulateXFadeBuffer(short* src, int numSamples)
{
    // cross fading enabled at all?
    if (!this->xFadeEnabled)
    {
        return 0;
    }

    // clamp number of samples if necessary
    if ((this->xFadeReadIndex + numSamples) >= XFADESIZE)
    {
        numSamples = XFADESIZE - this->xFadeReadIndex;
    }

    if (numSamples > 0)
    {
        // modulate data
        int i;
        for (i = 0; i < numSamples; i++)
        {
            int readIndex = this->xFadeReadIndex + i;
            float srcSample = (float) src[i];
            float bufSample = (float) this->xFadeBuffer[readIndex];
            float mix       = xFadeTable[readIndex];
            float val = (bufSample * mix) + (srcSample * (1.0f - mix));
            src[i] = (short) val;
        }

        // update read index
        this->xFadeReadIndex += numSamples;
    }
    return numSamples;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
