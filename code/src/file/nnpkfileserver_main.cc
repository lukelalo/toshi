#define N_IMPLEMENTS nNpkFileServer
//------------------------------------------------------------------------------
//  nnpkfileserver_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "file/nnpkfileserver.h"
#include "file/nnpkfilewrapper.h"
#include "file/nnpktocentry.h"
#include "file/nnpkfile.h"
#include "file/nnpkdirectory.h"

nNebulaScriptClass(nNpkFileServer, "nfileserver2");

//------------------------------------------------------------------------------
/**
*/
nNpkFileServer::nNpkFileServer() :
    isNpkCwd(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nNpkFileServer::~nNpkFileServer()
{
    // delete npk file wrappers
    nNpkFileWrapper* cur;
    while ((cur = (nNpkFileWrapper*) this->npkFiles.RemHead()))
    {
        n_delete cur;
    }
}

//------------------------------------------------------------------------------
/**
*/
nFile*
nNpkFileServer::NewFileObject()
{
    return n_new nNpkFile(this);
}

//------------------------------------------------------------------------------
/**
*/
nDirectory*
nNpkFileServer::NewDirectoryObject()
{
    return n_new nNpkDirectory(this);
}

//------------------------------------------------------------------------------
/**
    Check the extension of a pathname.
*/
bool
nNpkFileServer::CheckExtension(const char* path, const char* ext)
{
    n_assert(path);
    const char* dot = strrchr(path, '.');
    if (dot)
    {
        if (strcmp(dot + 1, ext) == 0)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Scan through a directory for npk files and create a nNpkFileWrapper
    object for each npk file found, and open the npk files. Returns
    the number of npk files found.
*/
bool
nNpkFileServer::ParseFile(const char* pathName)
{
    // absolutize directory name
    char mangledPath[N_MAXPATH];
    char absPath[N_MAXPATH];
    char rootPath[N_MAXPATH];
    this->ManglePath(pathName, mangledPath, sizeof(mangledPath));
    this->MakeAbsolute(mangledPath, absPath, sizeof(absPath));

    // generate root path
    strcpy(rootPath, absPath);
    char* slash = strrchr(rootPath, '/');
    if (slash)
    {
        *slash = 0;
    }
    else
    {
        n_printf("nNpkFileServer::ParseFile(): '%s' not a valid absolute file name\n", pathName);
        return false;
    }

    // read file...
    if (this->ParseNpkFile(rootPath, absPath))
    {
        return true;
    }
    else
    {
        n_printf("nNpkFileServer::ParseFile(): could not open file '%s'\n", pathName);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Parse a single npk file, creates a nNpkFileWrapper object, adds it the
    the npkFiles list and opens it.
*/
bool
nNpkFileServer::ParseNpkFile(const char* rootPath, const char* absFilename)
{
    nNpkFileWrapper* fileWrapper = n_new nNpkFileWrapper();
    n_assert(fileWrapper);
    if (fileWrapper->Open(this, rootPath, absFilename))
    {
        this->npkFiles.AddTail(fileWrapper);
        this->CheckDupes(fileWrapper);
        return true;
    }
    else
    {
        n_delete fileWrapper;
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Find a nTocEntry by filename. The filename must be absolute.
*/
nNpkTocEntry*
nNpkFileServer::FindFirstTocEntry(const char* absPath)
{
    nNpkFileWrapper* curWrapper;
    for (curWrapper = (nNpkFileWrapper*) this->npkFiles.GetHead(); 
         curWrapper; 
         curWrapper = (nNpkFileWrapper*) curWrapper->GetSucc())
    {
        nNpkToc& toc = curWrapper->GetTocObject();
        nNpkTocEntry* tocEntry = toc.FindEntry(absPath);
        if (tocEntry)
        {
            return tocEntry;
        }
    }
    // not found
    return 0;
}

//------------------------------------------------------------------------------
/**
    Parse the file wrapper objects which are less recent then the ones
    in the given nNpkTocEntry for a match.
*/
nNpkTocEntry*
nNpkFileServer::FindNextTocEntry(nNpkTocEntry* curEntry, const char* absPath)
{
    nNpkFileWrapper* headWrapper = curEntry->GetFileWrapper();
    nNpkFileWrapper* curWrapper;
    for (curWrapper = (nNpkFileWrapper*) headWrapper->GetSucc();
         curWrapper;
         curWrapper = (nNpkFileWrapper*) curWrapper->GetSucc())
    {
        nNpkToc& toc = curWrapper->GetTocObject();
        nNpkTocEntry* tocEntry = toc.FindEntry(absPath);
        if (tocEntry)
        {
            return tocEntry;
        }
    }
    // not found
    return 0;
}

//------------------------------------------------------------------------------
/**
    Set the current working directory. First check if the directory is 
    a valid filesystem directory. If not, check each filewrapper object
    if the directory is inside.
*/
bool
nNpkFileServer::ChangeDir(const char* newDir)
{
    n_assert(newDir);

    // first try to change to filesystem-dir
    if (nFileServer2::ChangeDir(newDir))
    {
        // this is a valid filesystem working dir
        this->isNpkCwd = false;
        return true;
    }

    // otherwise check npk file wrappers if they contain the path
    char mangledPath[N_MAXPATH];
    char absPath[N_MAXPATH];
    this->ManglePath(newDir, mangledPath, sizeof(mangledPath));
    this->MakeAbsolute(mangledPath, absPath, sizeof(absPath));

    nNpkTocEntry* entry = this->FindFirstTocEntry(absPath);
    if (entry && (nNpkTocEntry::DIR == entry->GetType()))
    {
        this->isNpkCwd = true;
        this->npkCwd = absPath;
        return true;
    }

    // neither a valid filesystem dir, nor a dir in a npk file
    return false;
}

//------------------------------------------------------------------------------
/**
    Get the current working directory.
*/
const char*
nNpkFileServer::GetCwd()
{
    if (this->isNpkCwd)
    {
        return this->npkCwd.Get();
    }
    else
    {
        return nFileServer2::GetCwd();
    }
}

//------------------------------------------------------------------------------
/**
    Recursively go through nNpkTocEntry tree, and search for duplicates
    in more recent npk files. Mark duplicates as such.
*/
void
nNpkFileServer::RecurseCheckDupes(nNpkFileWrapper* curWrapper, nNpkTocEntry* curTocEntry)
{
    char tocEntryName[N_MAXPATH];
    curTocEntry->GetFullName(tocEntryName, sizeof(tocEntryName));
    curTocEntry->SetDuplicate(false);

    nNpkFileWrapper* recentWrapper;
    for (recentWrapper = (nNpkFileWrapper*) this->npkFiles.GetHead();
         recentWrapper != curWrapper;
         recentWrapper = (nNpkFileWrapper*) recentWrapper->GetSucc())
    {
        if (recentWrapper->GetTocObject().FindEntry(tocEntryName))
        {
            // DUPE!
            curTocEntry->SetDuplicate(true);
n_printf("DUPE: %s\n", tocEntryName);

            // recurse
            if (curTocEntry->GetType() == nNpkTocEntry::DIR)
            {
                nNpkTocEntry* childTocEntry = curTocEntry->GetFirstEntry();
                if (childTocEntry) do
                {
                    this->RecurseCheckDupes(curWrapper, childTocEntry);
                }
                while ((childTocEntry = curTocEntry->GetNextEntry(childTocEntry)));
            }
        }
    }
}
    

//------------------------------------------------------------------------------
/**
    Check the given nNpkFileWrapper for duplicate entries in existing
    nNpkFileWrapper objects, and mark them as dupes.
*/
void
nNpkFileServer::CheckDupes(nNpkFileWrapper* fileWrapper)
{
    n_assert(fileWrapper);
    nNpkToc& curToc = fileWrapper->GetTocObject();
    this->RecurseCheckDupes(fileWrapper, curToc.GetRootEntry());
}

//------------------------------------------------------------------------------

