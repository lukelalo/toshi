#ifndef N_OGGSTREAM_H
#define N_OGGSTREAM_H
//------------------------------------------------------------------------------
/**
    Ogg Vorbis file streamer. 
    Uses the Ogg Vorbis SDK from http://www.vorbis.com
    Many thanks to Alexander Denisov <denisov@darkwing.uoregon.edu> for
    implementing the original nOggStream class.
    
    (C) 2002 RadonLabs GmbH
*/
#ifndef N_STREAM_H
#include "audio/nstream.h"
#endif

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#undef N_DEFINES
#define N_DEFINES nOggStream
#include "kernel/ndefdllclass.h"
//------------------------------------------------------------------------------
class nFile;
class nOggStream : public nStream
{
public:
    /// constructor
    nOggStream();
    /// open ogg stream file
    virtual bool Open(const char* name, int openFlags);
    /// read data from stream
    virtual int Read(char* buf, int numBytes);
    /// tell current stream position
    virtual int Tell();
    /// seek to stream data position
    virtual void Seek(int seekPos);
    /// close stream
    virtual void Close();

    static nKernelServer* kernelServer;

    /// ogg vorbis read function callback
    static size_t readFunc(void* ptr, size_t size, size_t nmemb, void* datasource);
    /// ogg vorbis seek function callback
    static int seekFunc(void* datasource, ogg_int64_t offset, int whence);
    /// ogg vorbis close function callback
    static int closeFunc(void* datasource);
    /// ogg vorbis tell function callback
    static long tellFunc(void* datasource);

private:
    nFile* file;                   // pointer to Nebula file object
    OggVorbis_File oggFile;        // Ogg Vorbis file handle
    int numSamples;                // total number of samples in OGG file
};

//------------------------------------------------------------------------------
#endif
