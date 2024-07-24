#define N_IMPLEMENTS nZipDirectory
//------------------------------------------------------------------------------
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "file/nzipdirectory.h"

//------------------------------------------------------------------------------
/**
      

    history:
    - 04-Feb-2002   peter    created
*/
nZipDirectory::nZipDirectory(nZipFileServer* server) : nDirectory(server)
{
	search = NULL;
}

//------------------------------------------------------------------------------
/**

    history:
    - 04-Feb-2002   peter    created
*/
nZipDirectory::~nZipDirectory()
{
}

//------------------------------------------------------------------------------
/**
    opens the specified directory

	@param dirName   the name of the directory to open
    @return          success

	history:
    - 06-Feb-2002   peter    created
*/
bool 
nZipDirectory::Open(const char* dirName)
{
	n_assert(!this->IsOpen());

    n_assert(dirName);
    n_assert(strlen(dirName)>0);

	this->iszip = false;

	char buf[N_MAXPATH];
	this->fs->ManglePath(dirName, buf, sizeof(buf));

    DWORD attr;
    ulong last_chr_index;
    char last_chr;
	bool retval;

    // Der Filename muss "absolutisiert" werden, wenn er es
    // nicht schon ist. Ein Filename gilt als absolut, wenn
    // er mit einem "/", einem "\" anfaengt, oder mit einem
    // Laufwerksbuchstabe anfaengt (2.Buchstabe ein ":").
    this->fs->MakeAbsolute(buf, this->path, sizeof(this->path));
	this->handle = NULL;
    
    // evtl. angehaengte Slashes entfernen...
    last_chr_index = strlen(this->path)-1;    
    last_chr = this->path[last_chr_index];
	if ((last_chr == '/') || (last_chr == '\\')) this->path[last_chr_index]=0;

    // testen, ob File existiert und ein Dir ist...
    attr = GetFileAttributes(this->path);
    if ((attr != 0xffffffff) && (attr & FILE_ATTRIBUTE_DIRECTORY)) 
		retval=true;
    else                                                           
	{
		this->iszip = true;
		if(((nZipFileServer*)this->fs)->FindDirectory(this->path))
			retval = true;
		else
			retval = false;
	}


	if(!retval)
		this->path[0] = 0;

	if(retval)
		this->empty = !(this->SetToFirstEntry());

	return retval;
}

//------------------------------------------------------------------------------
/**
	sets search index to first entry in directory
  
	@return          success

	history:
    - 06-Feb-2002   peter    created
*/
bool 
nZipDirectory::SetToFirstEntry()
{
	n_assert(this->IsOpen());

	this->ix = -1;

	if(this->iszip)
		return this->SetToFirstZipEntry();

	this->searchState = DISK;

	if(this->handle)
		FindClose(this->handle);

	char tmpName[N_MAXPATH];
    strcpy(tmpName,this->path);
    strcat(tmpName,"/*.*");
	this->handle = FindFirstFile(tmpName,&(this->findData));
	
	if (this->handle == INVALID_HANDLE_VALUE) 
		return this->SetToFirstZipEntry();

	while ((strcmp(this->findData.cFileName, "..") ==0) || (strcmp(this->findData.cFileName, ".") ==0))
		if(!FindNextFile(this->handle, &this->findData))
			return this->SetToFirstZipEntry();


	this->ix = 0;
	
	return true;
}

//------------------------------------------------------------------------------
/**
	sets search index to first virtual entry in directory
  
	@return          success

	history:
    - 06-Feb-2002   peter    created
*/
bool 
nZipDirectory::SetToFirstZipEntry()
{
	n_assert(this->IsOpen());

	this->ix = -1;
	this->searchState = ZIPDIR;
	this->search = ((nZipFileServer*)this->fs)->dtoc;

	return this->SetToNextZipEntry();
}

//------------------------------------------------------------------------------
/**
	selects next directory entry
  
	@return          success

	history:
    - 06-Feb-2002   peter    created
*/
bool 
nZipDirectory::SetToNextEntry()
{
	n_assert(this->IsOpen());


	switch (this->searchState) {
	case DISK:
		if(nDirectory::SetToNextEntry())
			return true;
		this->searchState = ZIPDIR;
		this->search = ((nZipFileServer*)this->fs)->dtoc;
		return this->SetToNextZipEntry();
	case ZIPDIR:
		if(this->search)
			if(this->search->next)
			{
				this->search = this->search->next;
				return this->SetToNextZipEntry();
			}
		this->searchState = ZIPFILE;
		this->search = ((nZipFileServer*)this->fs)->toc;
	case ZIPFILE:
		if(this->search)
			if(this->search->next)
			{
				this->search = this->search->next;
				return this->SetToNextZipEntry();
			}
		return false;
	};

	return false;
}

//------------------------------------------------------------------------------
/**
	gets type of actual directory entry
  
	@return          FILE or DIRECTORY

	history:
    - 06-Feb-2002   peter    created
*/
nDirectory::nEntryType 
nZipDirectory::GetEntryType()
{
	n_assert(this->IsOpen());
	
	switch (this->searchState) {
	case DISK:
		return nDirectory::GetEntryType();
	case ZIPDIR:
		return DIRECTORY;
	case ZIPFILE:
		return FILE;
	};
	return FILE;
}

//------------------------------------------------------------------------------
/**
	gets name of actual directory entry
  
	@return          the name

	history:
    - 06-Feb-2002   peter    created
*/
const char* 
nZipDirectory::GetEntryName()
{
	n_assert(this->IsOpen());

	switch(this->searchState) {
	case DISK:
		return nDirectory::GetEntryName();
	case ZIPFILE:
	case ZIPDIR:
		n_assert(this->search);
		return this->search->name;
	};
	return 0;
}

//------------------------------------------------------------------------------
/**
	determines wether entry is in my directory
  
	@param entry	the entry to test
	@return          

	history:
    - 06-Feb-2002   peter    created
*/
bool
nZipDirectory::IsInDirectory(nZipEntry* entry)
{
	n_assert(this->IsOpen());

	char* n = strstr(entry->name, this->path);
	if(n != entry->name)
		return false;
	n += strlen(this->path);
	if(strlen(n) == 0)
		return false;
	n++;
	if((strchr(n, '/') == NULL) && (strchr(n, '\\') == NULL))
		return true;
	return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nZipDirectory::SetToNextZipEntry()
{
	switch(this->searchState) {
	case ZIPDIR:
		while(this->search)
		{
			if(IsInDirectory(this->search))
			{
				this->ix++;
				return true;
			}
			this->search = this->search->next;
		}
		this->searchState = ZIPFILE;
		this->search = ((nZipFileServer*)this->fs)->toc;
	case ZIPFILE:
		while(this->search)
		{
			if(IsInDirectory(this->search))
			{
				this->ix++;
				return true;
			}
			this->search = this->search->next;
		}
		return false;
	};
	return false;
}
