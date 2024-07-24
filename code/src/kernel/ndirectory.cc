#define N_IMPLEMENTS nDirectory
#define N_KERNEL
//------------------------------------------------------------------------------
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/ndirectory.h"

//------------------------------------------------------------------------------
/**
      

    history:
    - 30-Jan-2002   peter    created
    - 15-Feb-2003   cubejk   linux support added
*/
nDirectory::nDirectory(nFileServer2* server) : fs(server)
{
    // mark directory as closed
    path[0] = 0;
    empty = true;
    this->numEntries=-1;
    #if defined(__LINUX__) || defined(__MACOSX__)
    this->dir = NULL;
    this->d_ent = NULL;
    #endif
}

//------------------------------------------------------------------------------
/**
*/
nDirectory::~nDirectory()
{
    if(this->IsOpen())
    {
        n_printf("Warning: Directory destroyed before closing\n");
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    opens the specified directory

    @param dirName   the name of the directory to open
    @return          success

    history:
    - 30-Jan-2002   peter    created
    - 15-Feb-2003   cubejk   linux support added
*/
bool
nDirectory::Open(const char* dirName)
{
    n_assert(!this->IsOpen());

    n_assert(dirName);
    n_assert(strlen(dirName)>0);

    // the filename must be made 'absolute', if it not already is.
    // a filename is deemed to be 'absolute' if it beginns with '/', '\'
    // or a driveletter. (the 2nd char is a ':')
    char buf[N_MAXPATH];
    this->fs->ManglePath(dirName, buf, N_MAXPATH);
    this->fs->MakeAbsolute(buf, this->path, sizeof(this->path));

    // remove possible trailing slashes...
    char last_chr;
    int last_chr_index;
    last_chr_index = strlen(this->path)-1;
    last_chr = this->path[last_chr_index];
    if ((last_chr == '/') || (last_chr == '\\')) this->path[last_chr_index]=0;

#ifdef __WIN32__
    DWORD attr;
    bool retval;
    this->handle = NULL;

    // test the file existence and that it is a directory...
    attr = GetFileAttributes(this->path);
    if ((attr != 0xffffffff) && (attr & FILE_ATTRIBUTE_DIRECTORY))
    {
        this->empty = !(this->SetToFirstEntry());
        retval=true;
    }
    else
    {
        this->path[0] = 0;
        retval=false;
    }

    return retval;
#else
    bool retval = false;
    struct stat attr;

    // test the file existence and that it is a directory...
    if (stat(this->path,&attr) == 0)
    {
        if (S_ISDIR(attr.st_mode))
        {    
            this->dir = opendir(this->path);
            if (this->dir != NULL)
            {
        this->empty = !(this->SetToFirstEntry());
                retval = true;
            }
        }
        else
        {
            this->path[0] = 0;
        }
    }
    return retval;
#endif
}

//------------------------------------------------------------------------------
/**
    closes the directory

    history:
    - 30-Jan-2002   peter    created
    - 15-Feb-2003   cubejk   linux support added
*/
void
nDirectory::Close()
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    if (this->handle) {
        FindClose(this->handle);
        this->handle = NULL;
    }
#else
    if (this->dir != NULL)
        if (closedir(this->dir) == 0)
        {
            this->dir = NULL;
            this->d_ent = NULL; //pointer cleanup
        }
        else
        {
            char buf[255];
            sprintf(buf, "Can't close Directory: %s!",this->path);
            n_warn (buf);
        }
#endif
    this->numEntries = -1;
    this->path[0] = 0;
}

//------------------------------------------------------------------------------
/**
    asks if directory is empty
  
    @return          true if empty

    history:
    - 30-Jan-2002   peter    created
    - 15-Feb-2003   cubejk   linux support added
*/
bool 
nDirectory::IsEmpty()
{
    n_assert(this->IsOpen());

    return this->empty;
}

//------------------------------------------------------------------------------
/**
    sets search index to first entry in directory
    and selects the first entry
  
    @return          success

    history:
    - 30-Jan-2002   peter    created
    - 15-Feb-2003   cubejk   linux support added
*/
bool 
nDirectory::SetToFirstEntry()
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    this->ix = -1;

    if(this->handle)
        FindClose(this->handle);

    char tmpName[N_MAXPATH];
    strcpy(tmpName,this->path);
    strcat(tmpName,"/*.*");
    this->handle = FindFirstFile(tmpName,&(this->findData));

    if (this->handle == INVALID_HANDLE_VALUE)
        return false;
    while ((strcmp(this->findData.cFileName, "..") ==0) || (strcmp(this->findData.cFileName, ".") ==0))
        if(!FindNextFile(this->handle, &this->findData))
            return false;

    this->ix = 0;

    return true;
#else
    n_assert (this->dir != NULL)
    rewinddir(this->dir);
    //skip '.' and '..'
    do
    {
        
        this->d_ent = readdir(this->dir);
        if (this->d_ent == NULL)
        {
            //break if there is no first entry
            this->ix = -1;
    return false;
        }
    }
    while (this->d_ent->d_name[0] == '.' 
       || (this->d_ent->d_name[0]=='.' && this->d_ent->d_name[1]=='.')
    );
    
    this->ix = 0;
    return true;
#endif
}

//------------------------------------------------------------------------------
/**
    selects next directory entry
  
    @return          success

    history:
    - 30-Jan-2002   peter    created
    - 15-Feb-2003   cubejk   linux support added
*/
bool 
nDirectory::SetToNextEntry()
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    n_assert(this->handle);
    n_assert(this->handle != INVALID_HANDLE_VALUE);

    bool suc = (FindNextFile(this->handle,&(this->findData)) != 0) ? true : false;
    if(suc)
        this->ix++;

    return suc;
#else
    n_assert (this->dir != NULL)
    this->d_ent = readdir(this->dir);
    if (this->d_ent != NULL)
    {
        this->ix++;
        return true;
    }
    return false;
#endif
}

//------------------------------------------------------------------------------
/**
    gets name of actual directory entry
  
    @return          the name

    history:
    - 30-Jan-2002   peter    created
    - 15-Feb-2003   cubejk   linux support added
*/
const char* 
nDirectory::GetEntryName()
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    n_assert(this->handle);
    n_assert(this->handle != INVALID_HANDLE_VALUE);

    strcpy(this->apath, this->path);
    strcat(this->apath, "/");
    strcat(this->apath, this->findData.cFileName);
    return this->apath;
#else
    n_assert(this->dir != NULL);
    n_assert(this->d_ent != NULL);
    
    strcpy(this->apath, this->path);
    strcat(this->apath, "/");
    strcat(this->apath, this->d_ent->d_name);
    
    return this->apath;
#endif
}

//------------------------------------------------------------------------------
/**
    gets type of actual directory entry
  
    @return          FILE or DIRECTORY

    history:
    - 30-Jan-2002   peter    created
    - 15-Feb-2003   cubejk   linux support added
*/
nDirectory::nEntryType 
nDirectory::GetEntryType()
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    n_assert(this->handle);
    n_assert(this->handle != INVALID_HANDLE_VALUE);

    if(this->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return DIRECTORY;
    else
        return FILE;
#else
    struct stat attr;
    
    stat(this->GetEntryName(),&attr);
    
    if (S_ISREG(attr.st_mode))
        return FILE;
    if (S_ISDIR(attr.st_mode))
        return DIRECTORY;
    return INVALID;
#endif
}

//------------------------------------------------------------------------------
/**
    gets number of entries in directory

    @return          number of entries

    history:
    - 30-Jan-2002   peter    created
    - 15-Feb-2003   cubejk   don't change the actual seek position
                             cache the result by the first run
*/
int
nDirectory::GetNumEntries()
{
    
    n_assert(this->IsOpen());

    if(this->numEntries >=0)
    {
        //use the cached count
        return this->numEntries;
    }
    else
    {
        int temp_ix = this->ix;
        
        this->numEntries = 0;

    if(this->SetToFirstEntry())
    {
            this->numEntries++;
        while(this->SetToNextEntry())
                this->numEntries++;
        }
        
        //go back to old position
        if(this->SetToFirstEntry())
        {
            while (this->ix < temp_ix)
                this->SetToNextEntry();
    }

        return this->numEntries;
    }
}

//------------------------------------------------------------------------------
/**
    gets name of entry at specified position in directory

    @param index    number of entry
    @return         name of entry

    history:
    - 30-Jan-2002   peter    created
*/
const char*
nDirectory::GetEntryName(int index)
{
    n_assert(this->IsOpen());
    n_assert(!this->empty);

    if (index < this->ix)
        if(!this->SetToFirstEntry())
            return 0;

    while (this->ix < index)
        if (!this->SetToNextEntry())
            return 0;

    return this->GetEntryName();
}

//------------------------------------------------------------------------------
/**
    gets type of entry at specified position in directory

    @param index   number of entry
    @return        FILE or DIRECTORY or INVALID if there is no entry with number index

    history:
    - 30-Jan-2002   peter    created
*/
nDirectory::nEntryType
nDirectory::GetEntryType(int index)
{
    n_assert(this->IsOpen());
    n_assert(!this->empty);

    if (index < this->ix)
        if(!this->SetToFirstEntry())
            return INVALID;

    while (this->ix < index)
        if (!this->SetToNextEntry())
            return INVALID;

    return this->GetEntryType();
}
