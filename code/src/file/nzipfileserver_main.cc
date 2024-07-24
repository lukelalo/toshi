#define N_IMPLEMENTS nZipFileServer
//------------------------------------------------------------------------------
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"

#include "file/nzipfileserver.h"

nNebulaScriptClass(nZipFileServer, "nfileserver2");

//------------------------------------------------------------------------------
/**
*/
nZipFileServer::nZipFileServer() : nFileServer2()
{
	toc = NULL;
	dtoc = NULL;
	rootPath[0] = 0;
}

//------------------------------------------------------------------------------
/**
*/
nZipFileServer::~nZipFileServer()
{
	if(toc)
		delete toc;
}

//------------------------------------------------------------------------------
/**
	creates a new nZipFile object
  
	@return          the nFile object

	history:
    - 04-Feb-2002   peter    created
*/
nFile*
nZipFileServer::NewFileObject()
{
	return new nZipFile(this);
}

//------------------------------------------------------------------------------
/**
	creates a new nZipDirectory object
  
	@return          the nDirectory object

	history:
    - 06-Feb-2002   peter    created
*/
nDirectory*
nZipFileServer::NewDirectoryObject()
{
	return new nZipDirectory(this);
}
//------------------------------------------------------------------------------
/**
*/
nZipEntry::nZipEntry(const char* _name, const char* _file, nZipEntry* suc) : next(suc)
{
	strcpy(this->name, _name);
	strcpy(this->file, _file);
}

//------------------------------------------------------------------------------
/**
*/
nZipEntry::~nZipEntry()
{
	if(this->next)
		delete this->next;
}

//------------------------------------------------------------------------------
/**
	sets the directory in which to look for zip-files
	 
	@param dir		name of the directory

	history:
    - 04-Feb-2002   peter    created
*/
void
nZipFileServer::SetRootDirectory(const char* dir)
{
	char buf[N_MAXPATH];
	char buf2[N_MAXPATH];
	this->ManglePath(dir, buf2, sizeof(buf2));
	this->MakeAbsolute(buf2, buf, sizeof(buf));

	strcpy(this->rootPath, buf);

	const char* entryName;
	if(toc)
	{
		delete toc;
		toc = NULL;
	}
	if(dtoc)
	{
		delete dtoc;
		dtoc = NULL;
	}
	nDirectory* folder = nFileServer2::NewDirectoryObject();
	if(!folder->Open(buf))
	{
		delete folder;
		return;
	}
	if(!folder->IsEmpty())
	{
		do {
			entryName = folder->GetEntryName();
			if((strlen(entryName) > 3) && (folder->GetEntryType() == nDirectory::FILE))
			{
				const char* ext = entryName + strlen(entryName) - 4;
				if (n_stricmp(ext, ".zip") == 0)
                {
					this->Scan(entryName, buf);
                }
			}
		} while (folder->SetToNextEntry());
	}
	folder->Close();
	delete folder;
}

//------------------------------------------------------------------------------
/**
	search table of content for given file
	 
	@param name		name of the file to find

	history:
    - 04-Feb-2002   peter    created
*/
nZipEntry*
nZipFileServer::LookFor(const char* name)
{
	nZipEntry* entry = this->toc;
	while (entry)
	{
		if (n_stricmp(entry->name, name) == 0)
			return entry;
		entry = entry->next;
	}
	return NULL;
}

//------------------------------------------------------------------------------
/**
	search table of content for given directory
	 
	@param name		name of the directory to find
	@return			true if directory was found

	history:
    - 06-Feb-2002   peter    created
*/
bool
nZipFileServer::FindDirectory(const char* name)
{
    nZipEntry* entry = this->dtoc;
    for (; entry; entry = entry->next)
    {
        if (n_stricmp(entry->name, name) == 0)
        {
            return true;
        }
    }
    return false;
}


//------------------------------------------------------------------------------
/**
	scan a zip-file and remember all entries
	 
	@param dir		name of the zip-file

	history:
    - 04-Feb-2002   peter    created
*/
void
nZipFileServer::Scan(const char* name, const char* rootDir)
{
	unzFile file = unzOpen(name);
	n_assert(file);

	unz_file_info info;
	char entryName[N_MAXPATH];
	char entryPath[N_MAXPATH];

	if(unzGoToFirstFile(file) == UNZ_OK)
	{
		do {
			unzGetCurrentFileInfo (file, &info, entryName, sizeof(entryName),
					     NULL, 0, NULL, 0);
			strcpy(entryPath, rootDir);
			strcat(entryPath, "/");
			strcat(entryPath, entryName);
			this->toc = new nZipEntry(entryPath, name, this->toc);
			this->ExtractDirectory(entryName, name);

		} while (unzGoToNextFile(file) == UNZ_OK);
	}
	unzClose(file);
}

//------------------------------------------------------------------------------
/**
*/
void
nZipFileServer::ExtractDirectory(char* name, const char* file)
{
	char buf[N_MAXPATH];
    char* lastForwardSlash  = strrchr(name, '/');
    char* lastBackSlash     = strrchr(name, '\\');
    char* lastSlash = (lastForwardSlash > lastBackSlash) ? lastForwardSlash : lastBackSlash;
	if (lastSlash == NULL)
    {
		return;
    }
	(*lastSlash) = 0;
	strcpy(buf, this->rootPath);
	strcat(buf, "/");
	strcat(buf, name);
	if(!this->FindDirectory(buf))
	{
		this->dtoc = new nZipEntry(buf, file, this->dtoc);
		this->ExtractDirectory(name, file);
	}

}
