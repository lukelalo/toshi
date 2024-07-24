#ifndef N_STREAM_H
#define N_STREAM_H
//------------------------------------------------------------------------------
/**
    @class nStream
    @ingroup NebulaAudioModule
    @brief base class for all audio streamers

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#undef N_DEFINES
#define N_DEFINES nStream
#include "kernel/ndefdllclass.h"
//------------------------------------------------------------------------------
class nFileServer2;
class N_PUBLIC nStream : public nRoot 
{
public:
    /// constructor
    nStream();
    /// destructor
    virtual ~nStream();
    /// open stream file
    virtual bool Open(const char* name, int openFlags);
    /// read data from stream
    virtual int Read(char* buf, int numBytes);
    /// tell current stream data position
    virtual int Tell();
    /// seek to stream data position
    virtual void Seek(int seekPos);
    /// close stream
    virtual void Close();
    /// begin a cross fade, subsequent Read() will be modulated with target data
    void BeginXFade();
    /// finish a cross fade
    void EndXFade();
    /// end of stream data reached?
    bool EndOfStream();
    /// get the base frequency
    int GetBaseFreq() const;
    /// get looping flag
    bool GetLooping() const;
    /// get number of channels
    int GetNumChannels();

    /// open flags
    enum 
    {
        LOOPING = (1<<0)
    };

    /// initialize static data
    static void InitStaticData();
    static nKernelServer *kernelServer;

protected:
    /// set number of channels
    void SetNumChannels(int n);
    /// set the base frequency
    void SetBaseFreq(int f);
    /// write data to the current xfade buffer write position
    int UpdateXFadeBuffer(short* src, int numSamples);
    /// modulate data source data with contents of xfade buffer at current read pos
    int ModulateXFadeBuffer(short* src, int numSamples);

    enum
    {
        XFADESIZE = 256,                    // number of samples in crossfade buffer
    };
    nAutoRef<nFileServer2> refFileServer;
    nString fileName;
    bool isOpen;
    bool isLooping;
    bool isEof;
    bool xFadeEnabled;
    int baseFreq;
    int numChannels;

    static float xFadeTable[XFADESIZE];     // cross fade modulate table
    int xFadeWriteIndex;                    // current cross fade buffer write index
    int xFadeReadIndex;                     // current cross fade buffer read index
    short xFadeBuffer[XFADESIZE];           // cross fade buffer
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nStream::SetBaseFreq(int f)
{
    this->baseFreq = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nStream::GetBaseFreq() const
{
    return this->baseFreq;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nStream::GetLooping() const
{
    return this->isLooping;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStream::SetNumChannels(int n)
{
    this->numChannels = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nStream::GetNumChannels()
{
    return this->numChannels;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nStream::EndOfStream()
{
    return this->isEof;
}

//------------------------------------------------------------------------------
#endif
