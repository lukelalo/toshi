#define N_IMPLEMENTS nZipFile
//------------------------------------------------------------------------------
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "file/nzipfile.h"
#include "file/nzipfileserver.h"


//------------------------------------------------------------------------------
/**

	history:
    - 04-Feb-2002   peter    created
*/
nZipFile::nZipFile(nZipFileServer* server) : nFile(server)
{
	file = 0;
}

//------------------------------------------------------------------------------
/**

	history:
    - 04-Feb-2002   peter    created
*/
nZipFile::~nZipFile()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @return                      true if file is open, false if closed.
*/
bool
nZipFile::IsOpen()
{
    if (this->iszip)
    {
        return (file != 0);
    }
    else
    {
        return nFile::IsOpen();
    }
}

//------------------------------------------------------------------------------
/**
    opens the specified file
	  
	@param dirName		the name of the file to open
    @param accessMode   the access mode ("(r|w|a)[+]")
    @return				success

	history:
    - 04-Feb-2002   peter    created
*/
bool 
nZipFile::Open(const char* fileName, const char* accessMode)
{
	this->buflen = 0;
	this->buf2real = 0;
	this->virt2buf = 0;
	if(nFile::Open(fileName, accessMode))
	{
		this->iszip = false;
		return true;
	}

	char buf[N_MAXPATH];
	char buf2[N_MAXPATH];
	this->fs->ManglePath(fileName, buf2, sizeof(buf2));
	this->fs->MakeAbsolute(buf2, buf, sizeof(buf));
	
	this->iszip = true;
	
	//n_assert(stricmp(accessMode, "r") == 0);

	nZipEntry* entry = ((nZipFileServer*) this->fs)->LookFor(buf);
	if(entry == NULL)
		return false;

	this->file = unzOpen(entry->file);
	if(!this->file)
		return false;

	char* internalName = entry->name + strlen(((nZipFileServer*)this->fs)->rootPath) + 1;
	
	if(unzLocateFile(file, internalName,2) != UNZ_OK)
	{
		unzClose(this->file);
		this->file = 0;
		return false;
	}
	if(unzOpenCurrentFile(file) != UNZ_OK)
	{
		unzClose(this->file);
		this->file = 0;
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------
/**
	closes the file

	history:
    - 04-Feb-2002   peter    created
*/
void
nZipFile::Close()
{
	if(!this->iszip)
	{
		nFile::Close();
		return;
	}

	n_assert(this->IsOpen());

	unzCloseCurrentFile(this->file);
	unzClose(this->file);
	this->file = 0;
}

//------------------------------------------------------------------------------
/**
    reads a number of bytes from the file
	  
	@param buffer		buffer for data
	@param numBytes		number of bytes to read
	@return				number of bytes read

	history:
    - 06-Feb-2002   peter    created
*/
int 
nZipFile::Read(void* buffer, int numBytes)
{
	n_assert(this->IsOpen());

	if(!this->iszip)
		return nFile::Read(buffer, numBytes);

	int read = 0;
	unsigned char* pb = (unsigned char*) buffer;
	while (numBytes > 0)
	{
		int cpy = numBytes;
        if (numBytes > int(this->buflen - this->virt2buf))
        {
            cpy = int(this->buflen - this->virt2buf);
        }

		memcpy(pb, this->buffer + this->virt2buf, cpy);
		pb += cpy;
		read += cpy;
		numBytes -= cpy;
		this->virt2buf += cpy;
		if(this->virt2buf == this->buflen)
			this->FillBuffer();
		if(this->virt2buf == this->buflen)
			return read;
	}
	return read;

}

int
nZipFile::Write(const void* buffer, int numBytes)
{
	if(!this->iszip)
		return nFile::Write(buffer, numBytes);
	n_error("Writing not supported for zip-files!");
	return 0;
}

//------------------------------------------------------------------------------
/**
	fills the buffer

	history:
    - 06-Feb-2002   peter    created
*/
void 
nZipFile::FillBuffer()
{
	n_assert(this->IsOpen());

	int bufsize = sizeof(this->buffer);
	int preserve = (ZIP_BUFFER_PRESERVE < this->virt2buf) ? ZIP_BUFFER_PRESERVE : this->virt2buf;

	// remaining bytes in buffer that shall not be lost
	int remain = this->buflen - this->virt2buf + preserve;
	// move them to the beginning of the buffer
	memmove(this->buffer, this->buffer + this->virt2buf - preserve, remain);
	// increase relative position of buffer to file
	this->buf2real += this->virt2buf - preserve;
	// fill the buffer
	int read = unzReadCurrentFile(this->file, this->buffer + this->virt2buf + preserve, bufsize - this->virt2buf - preserve);
	// adjust buffer length
	this->buflen = read + remain;

	this->virt2buf = preserve;
}

//------------------------------------------------------------------------------
/**
	gets current position of file pointer
  
	@return          position of pointer

	history:
    - 06-Feb-2002   peter    created
*/
int 
nZipFile::Tell()
{
	n_assert(this->IsOpen());

	if(!this->iszip)
		return nFile::Tell();

	return this->virt2buf + this->buf2real;
}

//------------------------------------------------------------------------------
/**
	sets the file pointer to given absolute or relative position

    @param byteOffset		the offset
	@param origin			position from which to count
  
	@return          success

	history:
    - 06-Feb-2002   peter    created
*/
bool
nZipFile::Seek(int byteOffset, nSeekType origin)
{
	n_assert(this->IsOpen());

	if(!this->iszip)
		return nFile::Seek(byteOffset, origin);

	int absolut = 0;
	switch (origin) {
	case START:
		absolut = byteOffset;
		break;
	case CURRENT:
		absolut = byteOffset + this->virt2buf + this->buf2real;
		break;
	case END:
		absolut = this->GetFileSize() + byteOffset;
		break;
	};

	if((unsigned)absolut < this->buf2real)
	{
		unzCloseCurrentFile(this->file);
		int success = unzOpenCurrentFile(this->file);
		n_assert(success == UNZ_OK);
		int bufsize = sizeof(this->buffer);
		int chunks = absolut / bufsize;
		for(int i = 0; i < chunks; i++)
			if(unzReadCurrentFile(this->file, this->buffer, bufsize) != bufsize)
				return false;
		int addi = absolut % bufsize;
		if(unzReadCurrentFile(this->file, this->buffer, addi) != addi)
			return false;
		this->buflen = 0;
		this->buf2real = absolut;
		this->virt2buf = 0;
		return true;
	}
	int relative = absolut - this->buf2real;
	while ((unsigned)relative >= this->buflen)
	{
		int bufsize = sizeof(this->buffer);
		int read = unzReadCurrentFile(this->file, this->buffer, bufsize);
		if(read == 0)
			return false;
		this->buf2real += read;
		this->buflen = read;
		relative -= read;
	}
	this->virt2buf = relative;
	return true;
}

//------------------------------------------------------------------------------
/**
	gets uncompressed size of current file
  
	@return          size

	history:
    - 06-Feb-2002   peter    created
*/
int 
nZipFile::GetFileSize()
{
	n_assert(this->IsOpen());

	n_assert(this->iszip);

	unz_file_info info;
	unzGetCurrentFileInfo(this->file, &info, NULL, 0, NULL, 0, NULL, 0);

	return info.uncompressed_size;

}
