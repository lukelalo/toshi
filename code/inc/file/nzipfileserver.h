#ifndef N_ZIPFILESERVER_H
#define N_ZIPFILESERVER_H
//------------------------------------------------------------------------------
/**
    @defgroup ZipFileServer Zip File Server
    @ingroup FileServices

    @deprecated The nZipFileServer has been deprecated in favor of
    the nNpkFileServer.  The nNpkFileServer performs better with
    larger number of files and offers all of the advantages of the
    nZipFileServer, except for data compression.
*/
/**
    @class nZipFileServer
    @ingroup ZipFileServer

    @brief new fileserver for handling zip files

    The nZipFileServer provides functions for creating file and
    directory objects and assigns within zip files.

    @deprecated The nZipFileServer has been deprecated in favor of
    the nNpkFileServer.  The nNpkFileServer performs better with
    larger number of files and offers all of the advantages of the
    nZipFileServer, except for data compression.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_ZIPFILE_H
#include "file/nzipfile.h"
#endif

#ifndef N_ZIPDIRECTORY_H
#include "file/nzipdirectory.h"
#endif

#ifndef N_FILESERVER2_H
#include "kernel/nfileserver2.h"
#endif

#undef N_DEFINES
#define N_DEFINES nZipFileServer
#include "kernel/ndefdllclass.h"

class nZipDirectory;
class nZipFile;

class nZipEntry
{
	friend class nZipFileServer;
	friend class nZipFile;
	friend class nZipDirectory;
protected:
	/// constructor
	nZipEntry(const char* _name, const char* _file, nZipEntry* suc);
	/// destructor
	virtual ~nZipEntry();

	char name[N_MAXPATH];
	char file[N_MAXPATH];

	nZipEntry* next;
};

//------------------------------------------------------------------------------
class N_PUBLIC nZipFileServer : public nFileServer2
{
	friend class nZipFile;
	friend class nZipDirectory;
public:
    /// constructor
    nZipFileServer();
    /// destructor
    virtual ~nZipFileServer();
    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

	/// sets directory in which to look for zip-files
	virtual void SetRootDirectory(const char* dir);
	/// creates a new nZipFile object
    virtual nFile* NewFileObject();
	/// creates a new nZipDirectory object
    virtual nDirectory* NewDirectoryObject();
	/// gets path of root-directory
	virtual const char* GetRootDirectory() { return rootPath; }

protected:
	/// table of content
	nZipEntry* toc;
	/// directory list
	nZipEntry* dtoc;

	/// scan zipfile
	virtual void Scan(const char* name, const char* rootDir);
	/// look for file in table of content
	virtual nZipEntry* LookFor(const char* name);
	/// look for directory in table of content
	virtual bool FindDirectory(const char* name);
	/// extract directory names from zip-file
	virtual void ExtractDirectory(char* name, const char* file);
	/// root path
	char rootPath[N_MAXPATH];
};
//------------------------------------------------------------------------------
#endif
