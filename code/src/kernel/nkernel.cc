#define N_IMPLEMENTS nKernelServer
#define N_KERNEL

/*!
  \file nkernel.cc
*/
//--------------------------------------------------------------------
//  nkernel.cc -- Startup module of the nebula kernel
//
//  (C) 1998 Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
#include "kernel/nloghandler.h"

#ifdef __WIN32__
#include <windows.h>
#include <direct.h>
#endif

#ifdef __LINUX__
#include <time.h>
#include <sys/time.h>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

#ifdef __MACOSX__
#include <Carbon/carbon.h>
#include <string>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/stat.h>
#endif


/**
  @fn N_EXPORT void n_barf(const char *exp, const char *file, int line)
  @ingroup NebulaUtilityFunctions
  @brief Flushes the nebula logfile and aborts the process.

  @param exp
  @param file
  @param line

  \n

  - 13-Dec-99   floh    created
  - 07-Feb-00   floh    should now also flush log channel 
                        correctly if not on stdout
*/
N_EXPORT void n_barf(const char *exp, const char *file, int line)
{
    #ifndef __STANDALONE__
		if (nKernelServer::ks)
		{
			nKernelServer::ks->Error("*** NEBULA ASSERTION ***\nexpression: %s\nfile: %s\nline: %d\n", exp, file, line);
		}
    #else
        printf("*** NEBULA ASSERTION ***\nexpression: %s\nfile: %s\nline: %d\n", exp, file, line);
    #endif
	n_sleep(1.0f); //from N1 n_barf
    abort();
}

/**
  @fn N_EXPORT void n_error(const char *s, ...)
  @ingroup NebulaUtilityFunctions
  @brief An n_printf compatible function that is called when a
  serious situation is encountered which requires abortion of the
  program.

  \param s
  \param ...

  - 27-Nov-98   floh    created
*/
N_EXPORT void n_error(const char* msg, ...)
{
    va_list argList;
    va_start(argList, msg);
    #ifndef __STANDALONE__
        if (nKernelServer::ks)
        {
            //nKernelServer::ks->GetLogHandler()->Error(msg, argList);
			nKernelServer::ks->VError(msg, argList);
        }
    #else
        vprintf(s, arglist);
    #endif
    abort();
};        

/**
  @fn N_EXPORT void n_message(const char *s, ...)
  @ingroup NebulaUtilityFunctions
  @brief An n_printf compatible function that is called when an
  important message should be displayed to the user, but which does
  not abort the program.

  \param s
  \param ...

  - 27-Nov-98   floh    created
*/
N_EXPORT void n_message(const char* msg, ...)
{
    va_list argList;
    va_start(argList, msg);
    #ifndef __STANDALONE__
        if (nKernelServer::ks)
        {
            //nKernelServer::ks->GetLogHandler()->Message(msg, argList);
			nKernelServer::ks->VMessage(msg, argList);
        }
    #else
        vprintf(s, arglist);
    #endif
};        

/**
  @fn N_EXPORT void n_printf(const char *s, ...)
  @ingroup NebulaUtilityFunctions
  @brief A standard printf function, but will print to the console and
  the log file.

  \param s
  \param ...

  - 27-Nov-98   floh    created
*/
N_EXPORT void n_printf(const char *s, ...)
{
    va_list arglist;
    va_start(arglist,s);
    #ifndef __STANDALONE__
        if (nKernelServer::ks)
        {
            //nKernelServer::ks->GetLogHandler()->Print(s, arglist);
			nKernelServer::ks->VPrint(s, arglist);
        }
    #else
        vprintf(s, arglist);
    #endif
    va_end(arglist);
}

/**
  @fn N_EXPORT void n_sleep(double sec)
  @ingroup NebulaUtilityFunctions
  @brief Sleep function for portability

  @param sec Time in seconds

  - 21-Dec-98   floh    created
*/
N_EXPORT void n_sleep(double sec)
{
#   ifdef __WIN32__
    int milli_sec = (int) (sec * 1000.0);
    Sleep(milli_sec);
#   elif defined(__LINUX__)
    struct timespec t;
    #define N_NANOSEC_INT 1000000000
    #define N_NANOSEC_FLT 1000000000.0
    long long int nano_sec = (long long int) (sec * N_NANOSEC_FLT);
    t.tv_sec  = nano_sec / N_NANOSEC_INT;
    t.tv_nsec = nano_sec % N_NANOSEC_INT;
    nanosleep(&t,NULL);
#   else
#   warning n_sleep not yet implemented for this platform.
#   endif
}

/**
  @fn N_EXPORT char *n_strdup(const char *from)
  @ingroup NebulaUtilityFunctions
  @brief A string copy function with memory allocation

  @param from

  - 17-Jan-99   floh    created
*/
N_EXPORT char *n_strdup(const char *from)
{
    char *to = (char *) n_malloc(strlen(from)+1);
    if (to) strcpy(to,from);
    return to;
}

/**
  @fn N_EXPORT char *n_strncpy2(char *dest, const char *src, size_t size)
  @ingroup NebulaUtilityFunctions
  @brief A strncopy function which sets a 0 at the end of the copied string

  @param dest A char pointer where the string will be copied to.
  @param src  A char pointer to a 0 terminated character string
  @param size The length of the source string without trailing 0

  Works like @c strncpy(), but makes sure that the copied string
  is terminated correcly (that is, in ALL cases, a terminating
  0 is appended without violating the 'size' boundary. 

  - 19-Feb-99   floh    created
*/
N_EXPORT char *n_strncpy2(char *dest, const char *src, size_t size)
{
    strncpy(dest,src,size);
    dest[size-1] = 0;
    return dest;
}

/**
  @fn N_EXPORT void n_strcat(char *dest, const char *src, size_t dest_size)
  @ingroup NebulaUtilityFunctions
  @brief A safe strcat function, makes sure that 'dest' is never overwritten.

  @param dest A char pointer where the string will be concaten  

  Throws an assertion if dest would be overwritten. Fairly slow
  though.

  - 06-Mar-00   floh    created
*/
N_EXPORT void n_strcat(char *dest, const char *src, size_t dest_size)
{
    unsigned int l = strlen(dest) + strlen(src) + 1;
    n_assert(l < dest_size);
    strcat(dest,src);
}

/**
  @fn N_EXPORT int n_mkdir(const char * pathname)
  @ingroup NebulaUtilityFunctions
  @brief A portable function for creating a directory.

  @param pathname A char pointer to a string with the path name of the
                  directory to be created.  This can be absolute or relative
                  to the current working directory.
*/
N_EXPORT int n_mkdir(const char * pathname)
{
#if defined(__WIN32__)
    return mkdir(pathname);
#else
    return mkdir(pathname, S_IRWXU|S_IRWXG);
#endif
}

/*!
  \fn void *n_dllopen(const char *name)
  \brief A portable dll openening function for Windows, Linux, and Mac OS X.

  \param name A char pointer to a string with the dll name, 
                for linux without leading 'lib' and without trailing 
                'so', for windows systems the full dll name is required

  - 10-May-99   floh    created
  - 02-Aug-01   leaf    + error description in win32
  - 03-Aug-01   leaf    + error description in linux, 
                          thanks to Warren Baird
*/
#if defined(__LINUX__)
static void *_dlopen_wrapper(const char *name, bool prefix)
{
    char dll_name[N_MAXPATH];
    if (prefix) {
        strcpy(dll_name,"lib");
        strcat(dll_name,name);
    } else {
        strcpy(dll_name,name);
    }
    strcat(dll_name,".so");
    void *dll = dlopen(dll_name,RTLD_NOW|RTLD_GLOBAL);
    // Caller will handle printing error
    return dll;
}
void *n_dllopen(const char *name)
{
    void *dll = _dlopen_wrapper(name, true);
    if (!dll) {
        char *err1 = n_strdup(dlerror());
        dll = _dlopen_wrapper(name, false);
	if (!dll) {
	    const char *err2 = dlerror();
            n_printf("Could not load dll for '%s'\n", name);
            n_printf("Error was:\n");
            n_printf("%s\n", err1);
            n_printf("%s\n", err2);
	    n_free(err1);
        }
    }
    return dll;
}
#elif defined(__WIN32__)
void *n_dllopen(const char *name)
{
    HINSTANCE dll;
    dll = LoadLibrary((LPCSTR) name);
    if (!dll) {
        // Find out why we failed
        LPVOID lpMsgBuf;
        FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0,
            NULL 
            );
        
        // Display the string.
        n_printf("Could not load dll '%s'\nError was:\n%s\n", name, lpMsgBuf);

        // Free the buffer.
        LocalFree( lpMsgBuf );
    }
    return (void *) dll;
}
#elif defined(__MACOSX__)
void *n_dllopen(const char *name)
{
    void* result = 0;

    const int kBundleAnyType  = 0;
    const int kBundleNoSubdir = 0;

    CFBundleRef mainBundle = CFBundleGetMainBundle();

    // put the name of the bundle we want in a CFString
    std::string bundleName( name );
    bundleName += ".bundle";
    CFStringRef cfBundleName = CFStringCreateWithCString( NULL,
                                                          bundleName.c_str(),
                                                          kCFStringEncodingASCII );

    CFURLRef bundleURL = CFBundleCopyResourceURL( mainBundle,
                                                  cfBundleName, 
                                                  kBundleAnyType, 
                                                  kBundleNoSubdir );

    if ( bundleURL )
    {
        CFBundleRef myBundle = CFBundleCreate( kCFAllocatorDefault, bundleURL );
        Boolean loaded = CFBundleLoadExecutable( myBundle );
        if ( loaded )
            result = myBundle;
    }
    return result;
}
#else
void *n_dllopen(const char *name)
{
    n_error("nClass::dll_load() not implemented!");
    return NULL;
}
#endif

/*!
  \fn void n_dllclose(void *dll)
  \brief A portable dll closing function for Windows, Linux and Mac OS X
  systems.

  \param dll Void pointer to the dll to be closed

  - 10-May-99   floh    created
*/
#ifdef __LINUX__
void n_dllclose(void *dll)
{
    dlclose(dll);
}
#elif defined(__WIN32__)
void n_dllclose(void *dll)
{
    FreeLibrary((HMODULE)dll);
}
#elif defined(__MACOSX__)
void n_dllclose(void *dll)
{
    CFBundleUnloadExecutable( (CFBundleRef)dll );
}
#else
void n_dllclose(void *dll)
{
    n_error("nClass::dll_unload() not implemented!");
}
#endif

/*!
  \fn void *n_dllsymbol(void *dll, const char *sym)
  \brief Look up a symbol within a DLL.

  \param dll
  \param sym

  - 10-May-99   floh    created

*/
#ifdef __LINUX__
void *n_dllsymbol(void *dll, const char *sym)
{
    void *h = dlsym(dll,sym);
    if (!h)
    {
        n_printf("%s\n", dlerror());
        n_error("nClass::dll_symbol() failed!\n");    
    }
    return h;
}
#elif defined(__WIN32__)
void *n_dllsymbol(void *dll, const char *sym)
{
    FARPROC h = GetProcAddress((HMODULE)dll, (LPCSTR)sym);
    if (!h) n_error("nClass::dll_symbol() failed!\n");
    return (void *) h;
}
#elif defined(__MACOSX__)
void *n_dllsymbol(void *dll, const char *sym)
{
    CFStringRef cfSym = CFStringCreateWithCString( NULL, sym,
                                                   kCFStringEncodingASCII );
    void *h = (void*)CFBundleGetFunctionPointerForName( (CFBundleRef)dll,
                                                        cfSym );
    if ( !h )
    {
        n_error( "nClass::dll_symbol() failed!\n" );
    }
    return h;
}
#else
void *n_dllsymbol(void *dll, const char *sym)
{
    n_error("nClass::dll_symbol() not implemented!");
}
#endif

/**
  @fn bool n_strmatch(const char *str, const char *pat)
  @ingroup NebulaUtilityFunctions
  @brief A stringmatch function for win32 systems.

  @param str A char pointer to the string to be searched
  @param pat A char pointer containing the pattern to be found

  This is a replacement for posix @c 'fnmatch()' because win32 systems don't 
  possess a universal pattern matching routine. This routine coresponds to 
  Tcl8.0's @c TclStringMatch()

  - 13-May-99   floh    created
*/
bool n_strmatch(const char *str, const char *pat)
{
    char c2;

    while (true) {
        if (*pat==0) {
            if (*str==0) return true;
            else         return false;
        }
        if ((*str==0) && (*pat!='*')) return false;
        if (*pat=='*') {
            pat++;
            if (*pat==0) return true;
            while (true) {
                if (n_strmatch(str, pat)) return true;
                if (*str==0) return false;
                str++;
            }
        }
        if (*pat=='?') goto match;
        if (*pat=='[') {
            pat++;
            while (true) {
                if ((*pat==']') || (*pat==0)) return false;
                if (*pat==*str) break;
                if (pat[1] == '-') {
                    c2 = pat[2];
                    if (c2==0) return false;
                    if ((*pat<=*str) && (c2>=*str)) break;
                    if ((*pat>=*str) && (c2<=*str)) break;
                    pat+=2;
                }
                pat++;
            }
            while (*pat!=']') {
                if (*pat==0) {
                    pat--;
                    break;
                }
                pat++;
            }
            goto match;
        }
    
        if (*pat=='\\') {
            pat++;
            if (*pat==0) return false;
        }
        if (*pat!=*str) return false;

match:
        pat++;
        str++;
    }
}

//--------------------------------------------------------------------
// EOF
//--------------------------------------------------------------------
