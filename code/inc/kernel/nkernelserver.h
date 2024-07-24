#ifndef N_KERNELSERVER_H
#define N_KERNELSERVER_H
//--------------------------------------------------------------------
/**
    @defgroup NebulaKernelUtilities Kernel Utilities
    @ingroup NebulaKernelModule
*/
/**
    @defgroup KernelServices Kernel Services
    @ingroup NebulaKernelModule
*/
/** 
    @class nKernelServer 
    @ingroup KernelServices
    @brief The heart of Nebula.
    First Nebula object to be created.
    
    Manages:
    
     - object creation
     - resolving object names into object pointers
     - the list of loaded class modules and packages
     - some other lesser services

    <h2>Kernel Server Initialization</h2>

    The kernel server must be the first Nebula system to be initialized:

@code
    nKernelServer * kernelServer;

    kernelServer = new nKernelServer;
@endcode

    The kernel server creates three other servers
       - nTimeServer @c /sys/servers/time
       - nPersistServer @c /sys/servers/persist
       - nFileServer2 @c /sys/servers/file2

    The kernel server then reads the package @c .toc (Table Of Contents)
    files to find out what classes are available and which DLL files
    they are in. The @c .toc files are automatically generated during the
    build process.

    @todo Talk more about the process of loading classes and how they
    interact with shared libraries.

    <h2>Kernel Server Shutdown</h2>

    Once everything but the kernel server has been shutdown, the
    kernel server can be deleted:

@code
    delete kernelServer;
    kernelServer = NULL;
@endcode

*/
//--------------------------------------------------------------------
#include <stdarg.h>
#include <stdio.h>

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_CLASS_H
#include "kernel/nclass.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_LINEBUFFER_H
#include "util/nlinebuffer.h"
#endif

#ifndef N_STACK_H
#include "util/nstack.h"
#endif

#undef N_DEFINES
#define N_DEFINES nKernelServer
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nPckgTocEntry : public nHashNode {
    char pckg_name[N_MAXNAMELEN];
public:
    nPckgTocEntry(const char *cl, const char *pk)
        : nHashNode(cl)
    {
        n_strncpy2(pckg_name,pk,sizeof(pckg_name));
    };
    const char *GetPackageName(void) {
        return pckg_name;
    };
};

//------------------------------------------------------------------------------
class nTimeServer;
class nPersistServer;
class nFileServer2;
class nHardRefServer;
class nRemoteServer;
class nEnv;
class nLogHandler;

class N_PUBLIC nKernelServer {
    /// A list of all loaded classes
    nHashList class_list;
    nHashList toc_list;
    nRoot *root;
    nRoot *cwd;
    nStack<nRoot *> cwd_stack;
    nEnv *var_memalloc;
    nEnv *var_memused;
    nEnv *var_memnumalloc;
    nEnv *var_numobjects;

public:
    /// Pointer to the nKernelServer object
    static nKernelServer *ks;
    /// Pointer to the nTimeServer
    nTimeServer *ts;
    nHardRefServer *hardRefServer;
    int num_objects;    

    /// Class constructor
    nKernelServer();
    /// Class destructor
    virtual ~nKernelServer();
    
    // class objects
    /// Link new class module into the kernel.
    virtual void      AddClass(const char *super, nClass *cl);
    /// Removes a class from the nebula kernel
    virtual void      RemClass(nClass *cl);
    virtual nClass    *OpenClass(const char *cl_name);
    virtual void      CloseClass(nClass *cl);
    /// Returns a pointer to the class given in cl_name
    virtual nClass    *FindClass(const char *cl_name);
    virtual nHashList *GetClassList(void);

    // create, load, lookup objects by name
    virtual nRoot *New(const char *cl_name, const char *path);
    virtual nRoot *NewNoFail(const char *cl_name, const char *path);
    virtual nRoot *Load(const char *path);
    virtual nRoot *Lookup(const char *path);

    /// add a log handler to the chain (kernel does not take ownership)
    void AddLogHandler(nLogHandler* logHandler);
    /// removes a log handler (kernel does not take ownership)
    void RemoveLogHandler(nLogHandler* logHandler);
    /// get pointer to current log handler object
    nLogHandler* GetDefaultLogHandler() const;
    /// print a log message
    void Print(const char*, ...);
    void VPrint(const char*, va_list);
    /// show an important user message which may require user ack
    void Message(const char*, ...);
    void VMessage(const char*, va_list);
    /// show an error message which may require user ack
    void Error(const char*, ...);
    void VError(const char*, va_list);


    // current working object
    virtual nRoot *SetCwd(nRoot *);
    virtual nRoot *GetCwd(void);
    virtual void PushCwd(nRoot *);
    virtual nRoot *PopCwd(void);

    // class packages
    virtual void ReadTocFiles(void);
    virtual void AddModule(const char *,
                           bool (*)(nClass *, nKernelServer *),
                           void (*)(void),
                           void *(*)(void));

    /// Optionally call to update memory usage variables
    virtual void Trigger(void);
    
    nPersistServer* GetPersistServer() const;
    nFileServer2*   GetFileServer2()   const;
    nRemoteServer* GetRemoteServer() const;
    
private:
    /// Checks if the given path is an absolute path
    inline bool _isAbsPath(const char *path) { return (path[0]=='/'); }; 
    /// Create a new object and create all missing objects in the hierarchy.
    nRoot *_checkCreatePath(const char *, const char *, bool);
    /// Lookup an object by the given path and return a pointer to it.
    nRoot *_lookupPath(const char *);
    /// Loads a class and return a pointer to it.
    nClass *_openClass(const char *);
    nRoot *_newUnnamedObject(const char *);
    bool loadPackage(const char *);
    bool readTocFile(const char *);
    
    //This is done in this fashion as an aim towards the future
    /// Pointer to the nPersistServer object
    nPersistServer* persistServer;
    /// Pointer to the nFileServer2 object
    nFileServer2* fileServer;
    nRemoteServer* remoteServer;
    nList* curLogHandlers;       // the attached log handlers
};    
//--------------------------------------------------------------------
#endif
