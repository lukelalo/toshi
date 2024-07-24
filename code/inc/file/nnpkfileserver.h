#ifndef N_NPKFILESERVER_H
#define N_NPKFILESERVER_H
//------------------------------------------------------------------------------
/**
    @defgroup NpkFileServer NPK File Server
    @ingroup FileServices
*/
/**
    @class nNpkFileServer
    @ingroup NpkFileServer
    
    @brief Implement reading from npk files.

    nNpkFileServer implements the nFileServer2 interface, but works with
    npk files instead of the regular file system.  Npk files combine
    many individual files into a single file, which may be faster for
    loading many small files, especially on Windows.

    The implementation of nNpkFileServer relies upon nNpkFile and
    nNpkDirectory to provide the implementation of file and directory
    objects respectively.

    Some tools are provided for working with npk files as well. See
    @ref npktools for further information.

    The npk file format is:

    @code
    # header
    block HEADER {
        uint32 'NPK0'       // magic number
        uint32 blockLen     // number of following bytes in block
        uint32 dataOffset   // byte offset of data block from beginning of file
    }

    # table of contents, sequence of blocks of 3 types:
    block DIR {
        uint32 'DIR_'               // magic number of directory block
        uint32 blockLen             // number of following bytes in block
        uint16 dirNameLength        // length of the following name
        char[] dirName              // name of directory
    }

    block FILE {
        uint32 'FILE'               // magic number of file block
        uint32 blockLen             // number of following bytes in block
        uint32 fileOffset           // start of file data inside data block
        uint32 fileLength           // length of file data in bytes
        uint16 fileNameLength       // length of the following name
        char[] fileName             // name of file
    }

    block ENDOFDIR {
        uint32 'DEND'               // magic number of end of dir block
        uint32 blockLen             // number of following bytes in block (0)
    }

    # the data block
    block DATA {
        uint32 'DATA'               // magic number of data block
        uint32 blockLen             // number of following bytes in block
        char[] data                 // the data
    }
    @endcode

    (C) 2002 RadonLabs GmbH
*/

#ifndef N_FILESERVER2_H
#include "kernel/nfileserver2.h"
#endif

#undef N_DEFINES
#define N_DEFINES nNpkFileServer
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nNpkTocEntry;
class nNpkFileWrapper;
class nNpkFileServer : public nFileServer2
{
    friend class nNpkFile;
    friend class nNpkDirectory;

public:
    /// constructor
    nNpkFileServer();
    /// destructor
    virtual ~nNpkFileServer();

    /// create new nNpkFile object
    virtual nFile* NewFileObject();
    /// create new nNpkDirectory object
    virtual nDirectory* NewDirectoryObject();
    /// parse an npk file
    virtual bool ParseFile(const char* fileName);
    /// set current working dir (may be inside npk file)
    virtual bool ChangeDir(const char* newDir);
    /// get current working dir
    virtual const char* GetCwd();

    /// pointer to kernel server
    static nKernelServer* kernelServer;

private:
    /// parse the root dir (hardwired to "home:") for npk files
    bool ParseNpkFile(const char* rootPath, const char* filename);
    /// check extension of a filename
    bool CheckExtension(const char* path, const char* ext);
    /// find first matching toc entry by name in the included npk file wrappers
    nNpkTocEntry* FindFirstTocEntry(const char* absPath);
    /// find next (as in "next npk file") matching toc entry
    nNpkTocEntry* FindNextTocEntry(nNpkTocEntry* curEntry, const char* absPath);
    /// check for duplicates in nNpkFileWrappers
    void CheckDupes(nNpkFileWrapper* fileWrapper);
    /// recursing helper method for CheckDupes()
    void RecurseCheckDupes(nNpkFileWrapper* curWrapper, nNpkTocEntry* curTocEntry);
    
    nList npkFiles;         // list of nNpkFileWrapper objects
    bool isNpkCwd;          // true if current cwd is inside a npk file
    nString npkCwd;         // the current cwd into npk file
};

//------------------------------------------------------------------------------
#endif
