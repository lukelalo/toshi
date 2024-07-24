//------------------------------------------------------------------------------
//  wavinfo.cc
//  Put information about a wav file to stdout.
//
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "tools/wftools.h"

#define swapul(a) (ulong)(((ulong)a>>24)+(((ulong)a&0xff0000)>>8)+(((ulong)a&0xff00)<<8)+(((ulong)a&0xff)<<24))

//------------------------------------------------------------------------------
static
int
getfourcc(nFile* file)
{
    int i;
    file->GetInt(i);
    return swapul(i);
}

//------------------------------------------------------------------------------
static
int
getint(nFile* file)
{
    int i;
    file->GetInt(i);
    return i;
}

//------------------------------------------------------------------------------
static
short
getshort(nFile* file)
{
    short i;
    file->GetShort(i);
    return i;
}

//------------------------------------------------------------------------------
int
main(int argc, char* argv[])
{
    bool help;
    const char* filename;
    int retval = 0;
    int i32, riff, wave, fmt, endOfChunk;
    short type, channels, blockAlign, bitsPerSample;
    int sampleRate, bytesPerSec;
    int fourcc, len;
    int dataStart, dataLen;

    nKernelServer* kernelServer = new nKernelServer;
    nFileServer2* fileServer = (nFileServer2*) kernelServer->New("nfileserver2", "/sys/servers/file2");

    // get args
    help = wf_getboolarg(argc, argv, "-help");
    filename = wf_getstrarg(argc, argv, "-file", "sound.wav");
    if (help)
    {
        printf("wavinfo [-help] [-file filename]\n"
               "(C) 2002 RadonLabs GmbH\n"
               "Prints wav file information.\n"
               "-help -- show help\n"
               "-file -- wav filename\n");
        return 5;
    }

    // open wav file
    nFile* file = fileServer->NewFileObject();
    n_assert(file);
    if (!file->Open(filename, "rb"))
    {
        printf("wavinfo: Could not open file %s!\n", filename);
        retval = 10;
        goto fail;
    }

    // read wav header
    riff = getfourcc(file);
    if (riff != 'RIFF')
    {
        printf("wavinfo: %s is not a wav file (no RIFF header)!\n", filename);
        retval = 10;
        goto fail;
    }

    // skip length field
    i32 = getint(file);

    // read header
    wave = getfourcc(file);
    if (wave != 'WAVE')
    {
        printf("wavinfo: %s is not a wav file (no WAVE chunk)!\n", filename);
        retval = 10;
        goto fail;
    }

    // read fmt header
    fmt = getfourcc(file);
    if (fmt != 'fmt ')
    {
        printf("wavinfo: %s is not a wav file (no fmt chunk)!\n", filename);
        retval = 10;
        goto fail;
    }

    // read length of fmt chunk
    i32 = getint(file);
    endOfChunk = file->Tell() + i32;

    type = getshort(file);
    channels = getshort(file);
    sampleRate = getint(file);
    bytesPerSec = getint(file);
    blockAlign = getshort(file);
    bitsPerSample = getshort(file);

    // seek to end of fmt chunk
    do
    {
        file->Seek(endOfChunk, nFile::START);
        fourcc = getfourcc(file);
        len = getint(file);
        endOfChunk = file->Tell() + len;
    }
    while (fourcc != 'data');

    dataStart = file->Tell();
    dataLen = len;

    // print result
    printf("filename %s type %d channels %d samplerate %d bytespersec %d blockalign %d bitspersample %d datastart %d datalen %d", 
        filename,
        type,
        channels,
        sampleRate,
        bytesPerSec,
        blockAlign,
        bitsPerSample,
        dataStart,
        dataLen);
fail:
    // close wav file
    if (file->IsOpen())
    {
        file->Close();
    }
    delete file;
    delete kernelServer;

    return retval;
}

