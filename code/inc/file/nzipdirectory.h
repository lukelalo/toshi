#ifndef N_ZIPDIRECTORY_H
#define N_ZIPDIRECTORY_H
//------------------------------------------------------------------------------
/**
    @class nZipDirectory
    @ingroup ZipFileServer

    @brief wrapper for directory functions within zip files

    provides functions for searching directories within zip files

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ZIPFILESERVER_H
#include "file/nzipfileserver.h"
#endif

#ifndef N_DIRECTORY_H
#include "kernel/ndirectory.h"
#endif

#undef N_DEFINES
#define N_DEFINES nZipDirectory
#include "kernel/ndefdllclass.h"

class nZipFileServer;
class nZipEntry;
//------------------------------------------------------------------------------
class nZipDirectory : public nDirectory
{
public:
    /// constructor
    nZipDirectory(nZipFileServer* server);
    /// destructor
    virtual ~nZipDirectory();
	/// opens a directory
	virtual bool Open(const char* dirName);
	/// sets pointer to first entry of directory
    virtual bool SetToFirstEntry();
	/// moves pointer to next entry of directory
    virtual bool SetToNextEntry();

	/// gets name of actual entry
    virtual const char* GetEntryName();
	/// gets type of actual entry
    virtual nEntryType GetEntryType();


protected:
	/// state of searching through directory
	enum SearchState {
		DISK,
		ZIPDIR,
		ZIPFILE,
	} searchState;
	/// is it a zip-only directory?
	bool iszip;
	/// pointer to zip-file's table of content
	nZipEntry* search;

	/// sets pointer to first zip-entry
	virtual bool SetToFirstZipEntry();
	/// determines wether object is in my directory
	virtual bool IsInDirectory(nZipEntry* entry);
	/// moves pointer to next zip-entry
	virtual bool SetToNextZipEntry();
};
//------------------------------------------------------------------------------
#endif
