#ifndef N_NPKDIRECTORY_H
#define N_NPKDIRECTORY_H
//------------------------------------------------------------------------------
/**
    @class nNpkDirectory
    @ingroup NpkFileServer
    
    @brief npk File Directory Wrapper.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_DIRECTORY_H
#include "kernel/ndirectory.h"
#endif

#undef N_DEFINES
#define N_DEFINES nNpkDirectory
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nNpkFileServer;
class nNpkTocEntry;
class nNpkDirectory : public nDirectory
{
public:
    /// constructor
    nNpkDirectory(nFileServer2* fs);
    /// destructor
    virtual ~nNpkDirectory();
    /// open directory
    virtual bool Open(const char* dirName);
    /// close directory
    virtual void Close();
    /// check if directory is empty
    virtual bool IsEmpty();
    /// set first entry as current
    virtual bool SetToFirstEntry();
	/// set next entry as current
    virtual bool SetToNextEntry();
	/// get name of current entry
    virtual const char* GetEntryName();
	/// get type of current entry
    virtual nEntryType GetEntryType();

private:
    /// determines wether the directory is opened
    virtual bool IsOpen() const;
    /// set to first npk entry
    bool SetToFirstNpkEntry();
    /// set to next npk entry
    bool SetToNextNpkEntry();
    /// handle npk file during nDirectory entry enumeration
    void CheckNpkEntryOverride();

    nNpkFileServer* npkFileServer;      // pointer to nNpkFileServer interface 
    bool isNpkDir;                      // true if clean npk dir, otherwise may be mixed filesys/npk dir
    bool npkEntryOverride;              // true if npk file entered during filesystem dir enum
    nNpkTocEntry* tocEntry;             // pointer to current npk entry, or 0 if current entry is a filesys entry
    nNpkTocEntry* curSearchEntry;       // current search entry inside npk file
    char npkEntryAbsPath[N_MAXPATH];    // storage buffer for current entry name
};

//------------------------------------------------------------------------------
#endif
