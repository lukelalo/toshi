#define N_IMPLEMENTS nFileServer2
#define N_KERNEL
//------------------------------------------------------------------------------
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------

#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "kernel/nfileserver2.h"

#if defined(__MACOSX__)
#include <Carbon/carbon.h>
#endif

#undef n_init
#undef n_fini
#undef n_create
#define n_init   nfileserver2_init
#define n_fini   nfileserver2_fini
#define n_create nfileserver2_create

nNebulaScriptClass(nFileServer2, "nroot");

//------------------------------------------------------------------------------
/**

	history:
    - 30-Jan-2002   peter    created
*/
nFileServer2::nFileServer2() :
    assignDir(this),
    bytesRead(0),
    bytesWritten(0),
    numSeeks(0)
{
    this->assignDir = kernelServer->Lookup("/sys/share/assigns");
    if (!this->assignDir.isvalid())
    {
        this->assignDir = kernelServer->New("nroot","/sys/share/assigns");
    }
    this->initHomeAssign();
    this->initBinAssign();
}

//------------------------------------------------------------------------------
/**

	history:
    - 30-Jan-2002   peter    created
*/
nFileServer2::~nFileServer2()
{
    // note: keep assigns valid
}

//------------------------------------------------------------------------------
/**
    Reset statistics.
*/
void
nFileServer2::ResetStatistics()
{
    this->bytesRead = 0;
    this->bytesWritten = 0;
    this->numSeeks = 0;
}

//------------------------------------------------------------------------------
/**
    Add read bytes to statistics.
*/
void
nFileServer2::AddBytesRead(int b)
{
    this->bytesRead += b;
}

//------------------------------------------------------------------------------
/**
    Add read bytes written to statistics.
*/
void
nFileServer2::AddBytesWritten(int b)
{
    this->bytesWritten += b;
}

//------------------------------------------------------------------------------
/**
    Add a seek op to stats.
*/
void
nFileServer2::AddSeek()
{
    this->numSeeks++;
}

//------------------------------------------------------------------------------
/**
    Get bytes read since last ResetStatistics()
*/
int
nFileServer2::GetBytesRead() const
{
    return this->bytesRead;
}

//------------------------------------------------------------------------------
/**
    Get bytes written since last ResetStatistics()
*/
int
nFileServer2::GetBytesWritten() const
{
    return this->bytesWritten;
}

//------------------------------------------------------------------------------
/**
    Get number of seeks since last ResetStatistics().
*/
int
nFileServer2::GetNumSeeks() const
{
    return this->numSeeks;
}

//------------------------------------------------------------------------------
/**
	creates new or modifies existing assign under /sys/share/assigns

    @param assignName   the name of the assign
    @param pathName		the path to which the assign links

	history:
    - 30-Jan-2002   peter    created
*/
bool
nFileServer2::SetAssign(const char* assignName, const char* pathName)
{
	if (pathName[strlen(pathName)-1] != '/') {
        n_printf("path must end with a '/'\n");
        return false;
    }
        
    // ex. das Assign schon?
    kernelServer->PushCwd(this->assignDir.get());
    nEnv *env = (nEnv *) this->assignDir->Find(assignName);
    if (!env) {
        env = (nEnv *) kernelServer->New("nenv", assignName);
        if (!env) n_error("Could not create assign object!");
    }
    env->SetS(pathName);
    kernelServer->PopCwd();
    return true;

}

//------------------------------------------------------------------------------
/**
    queries existing assign under /sys/share/assigns

    @param assignName   the name of the assign
    @return				the path to which the assign links

	history:
    - 30-Jan-2002   peter    created
*/
const char*
nFileServer2::GetAssign(const char* assignName)
{
    nEnv *env = (nEnv *) this->assignDir->Find(assignName);
    if (env) 
		return env->GetS();
    else 
	{
        n_printf("Assign '%s' not defined!\n",assignName);
        return NULL;
    }
}

//------------------------------------------------------------------------------
/**
    Cleanup the path name in place (replace any backslashes with slashes),
    and removes a trailing slash if exists.
*/
void
nFileServer2::CleanupPathName(char* path)
{
    n_assert(path);

    char* ptr = path;
    char c;

    // replace backslashes with slashes
    while ((c = *ptr))
    {
        if (c == '\\')
        {
            *ptr = '/';
        }
        ptr++;
    }

    // remove trailing slash
    if ((ptr > path) && (*(--ptr) == '/'))
    {
        *ptr = 0;
    }
}

//------------------------------------------------------------------------------
/**
	makes an absolute path out of a relative one

    @param buf		buffer to store absolute path in
	@param path		the relative path (may already be absolute)
	
	history:
    - 06-Feb-2002   peter    created
*/
void
nFileServer2::MakeAbsolute(const char* path, char* buf, int buflen)
{
	if ((path[0]=='/')||(path[0]=='\\')||(path[1]==':'))
    {
        // ist bereits ein absoluter Pfad...
    	n_strncpy2(buf, path, buflen);
    } 
    else 
    {
        // Pfad ist relativ zum cwd...
        n_strncpy2(buf, this->GetCwd(), buflen);
        strcat(buf, "/");
        strcat(buf, path);
    }
    this->CleanupPathName(buf);
}

//------------------------------------------------------------------------------
/**
    Expands assign in path to full absolute path, replaces any backslashes
    by slashes, and returns any trainling slash, and makes the path absolute.
	  
	@param pathName		the path to expand
	@param buf			buffer for result
	@param bufSize		size of the buffer
    @return				result buffer
*/
const char* 
nFileServer2::ManglePath(const char* pathName, char* buf, int bufSize)
{
    char *pathBuf;
    char *colon;
    pathBuf = (char*)n_malloc(bufSize);
    n_strncpy2(pathBuf,pathName, bufSize);
    buf[0] = 0;

    // check for assigns
    while ((colon = strchr(pathBuf,':')))
	{
        *colon++ = 0;
        if (strlen(pathBuf) > 1)
		{
            const char *replace = this->GetAssign(pathBuf);
            if (replace)
            {
                n_strncpy2(buf, replace, bufSize);
                n_strcat(buf, colon, bufSize);
                n_strncpy2(pathBuf, buf, bufSize);
            }
        }
    }
    n_free(pathBuf);
    
    // no assigns, just do a copy.
    if (0 == buf[0])
    {
        n_strncpy2(buf, pathName, bufSize);
    }

    this->CleanupPathName(buf);
    return buf;
}

//------------------------------------------------------------------------------
/**
	creates a new nDirectory object

    @return          the nDirectory object

	history:
    - 30-Jan-2002   peter    created
*/
nDirectory* 
nFileServer2::NewDirectoryObject()
{
	return new nDirectory(this);
}

//------------------------------------------------------------------------------
/**
	creates a new nFile object
  
	@return          the nFile object

	history:
    - 30-Jan-2002   peter    created
*/
nFile*
nFileServer2::NewFileObject()
{
	return new nFile(this);
}

//------------------------------------------------------------------------------
/**
    Change current working dir.

    @param  newDir      path to new working dir, may contain assigns
    @return             true or false
*/
bool
nFileServer2::ChangeDir(const char* newDir)
{
    // change to working dir
    int result = chdir(newDir);
    return (result == 0);
}

//------------------------------------------------------------------------------
/**
    Return path to current working dir.

    @return     path to current working dir
*/
const char*
nFileServer2::GetCwd()
{
    char buf[N_MAXPATH];
    getcwd(buf, sizeof(buf));
    this->CleanupPathName(buf);
    this->cwd = buf;
    return this->cwd.Get();
}

//-------------------------------------------------------------------
//  initHomeAssign()
//  Initialize Nebula's home directory assign ("home:").
//  10-Aug-99   floh    created
//-------------------------------------------------------------------
void nFileServer2::initHomeAssign(void)
{
    char buf[N_MAXPATH];
#ifdef __WIN32__
    
    // Win32: try to find the nkernel.dll module handle's filename
    // and cut off the last 2 directories
    HMODULE hmod = GetModuleHandle("nkernel.dll");
    DWORD res = GetModuleFileName(hmod,buf,sizeof(buf));
    if (res == 0) {
        n_printf("nFileServer2::initHomeAssign(): GetModuleFileName() failed!\n");
    }

    // "x\y\bin\win32\xxx.exe" -> "x\y\"
    int i;
    char c;
    char *p;
    for (i=0; i<3; i++) 
    {
        p = strrchr(buf,'\\');
        n_assert(p);
        p[0] = 0;
    }
    
    // convert all backslashes to slashes
    p = buf;
    while ((c = *p)) 
    {
        if (c == '\\') *p = '/';
        p++;
    }
    strcat(buf,"/");
    
#elif defined(__LINUX__)
    
    // under Linux, the NEBULADIR environment variable must be set,
    // otherwise the current working directory will be used
    char *s = getenv("NEBULADIR");
    if (s) 
    {
        n_strncpy2(buf,s,sizeof(buf));
    } else {
        n_printf("Env variable NEBULADIR not set, assuming cwd as nebula home directory!\n");
        if (!getcwd(buf,sizeof(buf))) {
            n_error("Arrgh, getcwd() failed... bailing out!\n");
        }
    }
    // if last char is not a /, append one
    if (buf[strlen(buf)] != '/') strcat(buf,"/");

#elif defined(__MACOSX__)
    // isn't there an easer way?
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef bundleURL = CFBundleCopyBundleURL( mainBundle );
    FSRef bundleFSRef;
    CFURLGetFSRef( bundleURL, &bundleFSRef );
    FSRefMakePath( &bundleFSRef, (unsigned char*)buf, N_MAXPATH );	
    strcat( buf, "/Contents/Resources/" );
#else
#error nFileServer2::initHomeAssign() not implemented!
    // n_error("nPersistServer::initHomeAssign not implemented!\n");
#endif
    
    // finally, set the assign
    this->SetAssign("home",buf);
}

//-------------------------------------------------------------------
//  initBinAssign()
//  10-Aug-99   floh    created
//-------------------------------------------------------------------
void nFileServer2::initBinAssign(void)
{
    char buf[N_MAXPATH];
    const char *home_dir = this->GetAssign("home");
    n_assert(home_dir);
    n_strncpy2(buf,home_dir,sizeof(buf));

#ifdef __WIN32__
    strcat(buf,"bin/win32");
#elif defined(__LINUX__)
    strcat(buf,"bin/linux");
#elif defined(__MACOSX__)
    strcat(buf,"bin/macosx");
#else
#error nFileServer2::initBinAssign() not implemented!
    n_error("nFileServer2::initBinAssign() not implemented!\n");
#endif

    // append 'd' to bin: in debug builds.
#ifdef _DEBUG
    strcat(buf, "d/");
#else
    strcat(buf,"/");
#endif

    this->SetAssign("bin",buf);
}
//------------------------------------------------------------------------------
