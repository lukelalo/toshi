#ifndef N_FILESERVER2_H
#define N_FILESERVER2_H
//------------------------------------------------------------------------------
/**
    @class nFileServer2
    @ingroup FileServices

    @brief new fileserver, provides functions for creating file and
    directory objects and assigns

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_FILE_H
#include "kernel/nfile.h"
#endif

#ifndef N_DIRECTORY_H
#include "kernel/ndirectory.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#undef N_DEFINES
#define N_DEFINES nFileServer2
#include "kernel/ndefdllclass.h"

#ifdef __WIN32__
#   include <direct.h>
#    ifndef getcwd
#        define getcwd      _getcwd
#    endif
#else
#   include <unistd.h>
#endif


class nDirectory;
//------------------------------------------------------------------------------
class N_PUBLIC nFileServer2 : public nRoot
{
public:
    /// constructor
    nFileServer2();
    /// destructor
    virtual ~nFileServer2();

    /** @name Assigns and Path management
 
        See @ref fs_assigns for more information on assigns and how
        they are used.
    */
    //@{
    /// sets a path alias
    virtual bool SetAssign(const char* assignName, const char* pathName);
    /// gets a path alias
    virtual const char* GetAssign(const char* assignName);
    /// expand path alias to real path
    virtual const char* ManglePath(const char* pathName, char* buf, int bufSize);
    /// makes a path absolute
    virtual void MakeAbsolute(const char* path, char* buf, int bufSize);
    /// set current working dir
    virtual bool ChangeDir(const char* newDir);
    /// get current working dir
    virtual const char* GetCwd();
    //@}

    /// creates a new nDirectory object
    virtual nDirectory* NewDirectoryObject();
    /// creates a new nFile object
    virtual nFile* NewFileObject();

    /** @name Statistics
        Statistics gathering for tracking how much I/O is going on.
    */
    //@{
    /// reset statistics
    void ResetStatistics();
    /// add read bytes 
    void AddBytesRead(int b);
    /// add written bytes
    void AddBytesWritten(int b);
    /// add seek operation to stats
    void AddSeek();
    /// get bytes read since last reset
    int GetBytesRead() const;
    /// get bytes written since last reset
    int GetBytesWritten() const;
    /// get number of seeks
    int GetNumSeeks() const;
    //@}

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

protected:
    /// cleanup a path name in place
    void CleanupPathName(char* path);

	/// moved here from nfileserver
    void initHomeAssign();
    void initBinAssign();

    /// pointer to nebula-directory with assigns
    nRef<nRoot> assignDir;
    /// current working directory
    nString cwd;
    /// statistics: bytes read
    int bytesRead;
    /// statistics: bytes written
    int bytesWritten;
    /// statistics: number of seeks
    int numSeeks;
};
//------------------------------------------------------------------------------
#endif
