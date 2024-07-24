#ifndef N_DIRECTORY_H
#define N_DIRECTORY_H
//------------------------------------------------------------------------------
/**
    @class nDirectory
    @ingroup FileServices

    @brief wrapper for directory functions

    nDirectory provides functions for searching directories.  For usage
    information, see nFileServer2.

    @todo This needs to be ported to Mac OS X as it is
    currently Windows and Linux only.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_FILESERVER2_H
#include "kernel/nfileserver2.h"
#endif

#if defined(__LINUX__) || defined(__MACOSX__)
    //linux includes
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <dirent.h>
#endif

#undef N_DEFINES
#define N_DEFINES nDirectory
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nDirectory
{
public:
    enum nEntryType
    {
        FILE,
        DIRECTORY,
        INVALID,
    };

    /// constructor
    nDirectory(nFileServer2* server);
    /// destructor
    virtual ~nDirectory();

    /**
        @name Basic Operations
    */
    //@{
    /// opens a directory
    virtual bool Open(const char* dirName);
    /// closes the directory
    virtual void Close();
    /// get the full path name of the directory itself
    const char* GetPathName() const;
    /// check if the directory is empty
    virtual bool IsEmpty();
    //@}

    /**
        @name Serial Access
    */
    //@{
    /// sets the pointer to the first entry in the directory
    virtual bool SetToFirstEntry();
    /// moves the pointer to the next entry in the directory
    virtual bool SetToNextEntry();
    /// gets the name of the actual entry
    virtual const char* GetEntryName();
    /// gets the type of the actual entry
    virtual nEntryType GetEntryType();
    //@}

    /**
        @name Random Access
    */
    //@{
    /// gets number of entries in directory
    virtual int GetNumEntries();
    /// gets name of entry at given position
    virtual const char* GetEntryName(int index);
    /// gets type of entry at given position
    virtual nEntryType GetEntryType(int index);
    //@}

protected:
    /// determines wether the directory is opened
    bool IsOpen() const;
    /// pointer to fileserver
    nFileServer2* fs;
    /// path of the directory
    char path[N_MAXPATH];
    /// is the directory empty?
    bool empty;

    /// the cached number of entrys (saved by the first run of GetNumEntries())
    int numEntries;
    /// number of actual selected entry
    int ix;
    /// absolute path of actual entry
    char apath[N_MAXPATH];

#ifdef __WIN32__
    /// win32 directory search handle
    HANDLE handle;
    /// win32 search result
    WIN32_FIND_DATA findData;
#elif defined(__LINUX__) || defined(__MACOSX__)
    /// linux directory handler
    DIR *dir;
    /// current directory entry
    struct dirent *d_ent;
#endif
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nDirectory::IsOpen() const
{
    return ((this->path[0] == 0) ? false : true);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nDirectory::GetPathName() const
{
    n_assert(this->IsOpen());
    return this->path;
}

//------------------------------------------------------------------------------
#endif
