#ifndef N_RSRCPATH_H
#define N_RSRCPATH_H
//-------------------------------------------------------------------
/**
    @class nRsrcPath

    @brief smart resource file locator

    Encapsulates a path name to a resource which may or may
    not be relative to the current working directory or may
    contain a fileserver assign. The pathname is absolutized
    and stored in the object.
*/
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_PERSISTSERVER_H
#include "kernel/npersistserver.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#ifndef N_FILESERVER2_H
#include "kernel/nfileserver2.h"
#endif

#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------------------------
class nRsrcPath 
{
public:
    /// clear paths
    void Clear(void); 
    /// set paths
    void Set(nFileServer2* fs, const char *_name, const char *cwd);
    /// get original path
    const char *GetPath(void) const;
    /// get absolute path
    const char *GetAbsPath(void) const;

private:
    nString origName;   // original filename
    nString absName;    // absolutized filename
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nRsrcPath::Clear()
{
    this->origName.Set(0);
    this->absName.Set(0);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nRsrcPath::Set(nFileServer2* fs, const char *name, const char * /* unused: cwd */) 
{
    if (name)
    {
        this->origName = name;

        char mangledPath[N_MAXPATH];
        char absolutePath[N_MAXPATH];
        fs->ManglePath(name, mangledPath, sizeof(mangledPath));
        fs->MakeAbsolute(mangledPath, absolutePath, sizeof(absolutePath));
        this->absName = absolutePath;
    }
    else
    {
        this->origName.Set(0);
        this->absName.Set(0);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nRsrcPath::GetPath() const
{
    return this->origName.IsEmpty() ? 0 : this->origName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char* 
nRsrcPath::GetAbsPath() const
{
    return this->absName.IsEmpty() ? 0 : this->absName.Get();
}

//-------------------------------------------------------------------
#endif

