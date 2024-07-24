#ifndef N_WAVSTREAM_H
#define N_WAVSTREAM_H
//------------------------------------------------------------------------------
/**
    @class nWavStream
    @ingroup NebulaAudioModule
    @brief WAV file streamer

    Simple wav file streamer. Limited to PCM 16 bit mono wavs.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_STREAM_H
#include "audio/nstream.h"
#endif

#undef N_DEFINES
#define N_DEFINES nWavStream
#include "kernel/ndefdllclass.h"
//------------------------------------------------------------------------------
class nFile;
class nWavStream : public nStream 
{
public:
    /// constructor
    nWavStream();
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

    static nKernelServer *kernelServer;

private:
    /// read a four cc code from file
    ulong GetFourCC();
    /// read a 32 bit unsigned int from file
    ulong GetULong();
    /// read a 16 bit unsigned short from file
    ushort GetUShort();

    nFile* file;
    int bitsPerSample;
    int dataStart;
    int dataLen;
};
//--------------------------------------------------------------------
#endif
