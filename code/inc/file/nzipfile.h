#ifndef N_ZIPFILE_H
#define N_ZIPFILE_H
//------------------------------------------------------------------------------
/**
    @class nZipFile
    @ingroup ZipFileServer

    @brief wrapper for file routines within zip files

    provides functions for reading and writing files within zip files

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_FILE_H
#include "kernel/nfile.h"
#endif

#include "file/unzip.h"

#undef N_DEFINES
#define N_DEFINES nZipFile
#include "kernel/ndefdllclass.h"

#define ZIP_BUFFER_LENGTH 4096
#define ZIP_BUFFER_PRESERVE 1024

class nZipFileServer;
//------------------------------------------------------------------------------
class nZipFile : public nFile
{
public:
    nZipFile(nZipFileServer* server);
    /// destructor
    virtual ~nZipFile();
	/// opens a file
	virtual bool Open(const char* fileName, const char* accessMode);
	/// closes the file
    virtual void Close();
	/// reads some bytes from the file
    virtual int Read(void* buffer, int numBytes);
	/// gets position in file
    virtual int Tell();
	/// sets position in file
    virtual bool Seek(int byteOffset, nSeekType origin);
	/// writes some bytes to the file
    virtual int Write(const void* buffer, int numBytes);
	/// determines wether the file is open
	virtual bool IsOpen();

protected:
	/// is the file within a zip-file?
	bool iszip;
	/// handle of zip-file
	unzFile file;
	/// fills the buffer
	virtual void FillBuffer();
	/// gets size of file
	virtual int GetFileSize();
	/// the buffer
	unsigned char buffer[ZIP_BUFFER_LENGTH];
	/// number of valid bytes in buffer
	unsigned int buflen;
	/// location of buffer in real uncompressed file space
	unsigned int buf2real;
	/// offset within buffer
	unsigned int virt2buf;
};

//------------------------------------------------------------------------------
#endif
