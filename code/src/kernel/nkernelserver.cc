#define N_IMPLEMENTS nKernelServer
#define N_KERNEL
/*!
  \file
*/

//--------------------------------------------------------------------
//  nkernelserver.cc
//  (C) 1998 Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>

#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "kernel/npersistserver.h"
#include "kernel/ntimeserver.h"
#include "kernel/nhardrefserver.h"
#include "kernel/nremoteserver.h"
#include "kernel/ndefaultloghandler.h"
#include "kernel/nfileserver2.h"

nKernelServer *nKernelServer::ks = NULL;
extern char *nkernel_version;

// from nmemory.cc
#ifdef __NEBULA_MEM_MANAGER__
extern int n_memallocated;
extern int n_memused;
extern int n_memnumalloc;
#endif

/**
  Loads a class and return a pointer to it.
  First it is checked if the class is already loaded, if so the pointer to 
  this class is returned, otherwise first a loading from package is tried
  than loading from the dll itself.

  @param cl_name Name of the class to be opened

  - 08-Oct-98    floh    created
  - 04-Oct-98    floh    char * -> const char *
  - 10-Aug-99    floh    + if class not loaded, now first looks into
                           package tocs before trying to load directly
                           from dll
  - 24-Oct-99    floh    returns zero if class could not be loaded
                      
*/
nClass *nKernelServer::_openClass(const char *cl_name)
{
    // class already loaded?
    nClass *cl = (nClass *) this->class_list.Find(cl_name);
    if (cl) return cl;
    
    // try loading from package
    nPckgTocEntry *pte = (nPckgTocEntry *) this->toc_list.Find(cl_name);
    if (pte) {
        if (this->loadPackage(pte->GetPackageName())) {
            cl = (nClass *) this->class_list.Find(cl_name);
            if (cl) return cl;
        }
    }
        
    // try loading from dll
    cl = n_new nClass(cl_name,this);
    if (cl->Open()) {
        this->class_list.AddTail(cl);
    } else {
        n_delete cl;
        cl = NULL;
    }
    return cl;
}

/**
  _newUnnamedObject()

  @param lc_name Name of the object

  - 08-Oct-98   floh    created
  - 04-Oct-98   floh    char * -> const char *
*/
nRoot *nKernelServer::_newUnnamedObject(const char *cl_name)
{
    nClass *cl;
    if ((cl = this->_openClass(cl_name))) return cl->NewObject();
    else                                  return NULL;
}

/**
  Lookup an object by the given path and return a pointer to it.
  Check whether object described by path exists, returns
  pointer or NULL.

  @param path The path of the object to be found.

  - 08-Oct-98   floh    created
  - 04-Oct-98   floh    char * -> const char *
  - 11-Dec-01   floh    bugfix: returned 'cwd' if "" string given,
                        now return 0
*/
nRoot *nKernelServer::_lookupPath(const char *path)
{
    nRoot *cur;
    char *next_str;
    char str_buf[512];

    // check for empty string
    if (0 == path[0])
    {
        return 0;
    }

    char *str = str_buf;
    strncpy(str_buf,path,sizeof(str_buf)); 
    str_buf[sizeof(str_buf)-1]=0;
    if (_isAbsPath(str)) cur = this->root;
    else                 cur = this->cwd;
    while ((next_str = strtok(str,"/")) && cur) {
        if (str) str=NULL;
        cur = cur->Find(next_str);
    }
    return (nRoot *) cur;
}                    

/**
  Create a new object and create all missing objects in the hierarchy.
  Create object described by path, fill up missing path components
  with nRoot objects.

  @param cl_name The name of the object
  @param path Path where the object should be created
  @param die_on_error Flag on true creates a n_error message "Aborting\n", on false doesn't

  - 08-Oct-98   floh    created
  - 04-Oct-98   floh    char * -> const char *
  - 01-Dec-98   floh    if object exists, increase ref count
  - 24-Oct-99   floh    don't break on problems, instead return NULL
*/
nRoot *nKernelServer::_checkCreatePath(const char *cl_name, const char *path, bool die_on_error)
{
    nRoot *parent = NULL;
    nRoot *child  = NULL;
    char *cur_str, *next_str;
    char str_buf[512];
    char *str = str_buf;
    strncpy(str_buf,path,sizeof(str_buf)); 
    str_buf[sizeof(str_buf)-1]=0;
    
    if (_isAbsPath(str)) parent = this->root;
    else                 parent = this->cwd;
    cur_str = strtok(str,"/");
    if (cur_str) {
        // for each directory path component
        while ((next_str = strtok(NULL,"/"))) {
            child = parent->Find(cur_str);
            if (!child) {
                // subdir doesn't exist, fill up
                child = this->_newUnnamedObject("nroot");
                if (child) {
                    child->SetName(cur_str);
                    parent->AddTail(child);
                    child->Initialize();
                } else {
                    n_printf("ERROR: Couldn't create object '%s' in path '%s'.\n",
                             cur_str, path);
                    if (die_on_error) n_error("Aborting.\n");
                    return NULL;
                }
            }
            parent = child;
            cur_str = next_str;
        }

        // cur_str is now name of last path component
        child = parent->Find(cur_str);
        if (!child) {
            // create and link object
            child = (nRoot *) this->_newUnnamedObject(cl_name);
            if (child) {
                child->SetName(cur_str);
                parent->AddTail(child);
                child->Initialize();
            } else {
                n_printf("ERROR: Couldn't create object '%s' of class '%s'.\n",
                         path,cl_name);
                if (die_on_error) n_error("Aborting.\n");
                return NULL;
            }
        }
    } else {
        n_warn("Path component is empty!");
    }
    return child;
}

/**
  The class constructor

  - 08-Oct-98   floh    created
  - 04-Nov-98   floh    + nFileServer Object
  - 07-Dec-98   floh    + nTimeServer Object
                        + nFileServer Object is now called 
                          "/sys/servers/file"
  - 15-Jan-99   floh    + there is no nRoot::Init() anymore.
  - 22-Feb-99   floh    + char_buf initialising
  - 26-Feb-99   floh    + initialising MemHandler
  - 11-May-99   floh    + loads by default the Nebula-Package-DLL
  - 25-May-99   floh    + logmsg redirection
  - 26-May-99   floh    + num_objects, var_memnumalloc
  - 10-Aug-99   floh    + HashTable size for Classlist and toc_list 
                          set to 128
                        + doesn't load the nclasses package overall 
                          but reads in TOC-Files
  - 20-Jan-00   floh    + no SetScriptServer() anymore
*/
nKernelServer::nKernelServer(void)
             : class_list(128),
               toc_list(128),
               num_objects(0)
{
    ks = this;

    //Create the log handler - we now have n_printf
    this->curLogHandlers = n_new nList;
    nNode *node = n_new nNode;
    node->SetPtr(n_new nDefaultLogHandler);
    this->curLogHandlers->AddHead(node);

    // create hard ref server
    this->hardRefServer = n_new nHardRefServer;
    n_assert(this->hardRefServer);

    // create root object
    this->root = (nRoot *) this->_newUnnamedObject("nroot");
    if (this->root) {
        this->root->SetName("/");
    } else {
        n_error("Could not create root object!");    
    }
    this->cwd = this->root;

    // create time server
    this->ts = (nTimeServer *) this->New("ntimeserver","/sys/servers/time");
    n_assert(this->ts);
    
    // create a default fileserver2 object
    this->fileServer = (nFileServer2*) this->New("nfileserver2","/sys/servers/file2");
    n_assert(this->fileServer);

    // create persist server
    this->persistServer = (nPersistServer*) this->New("npersistserver","/sys/servers/persist");
    n_assert(this->persistServer);
    
    //create remote server
    this->remoteServer  = (nRemoteServer*)  this->New("nremoteserver",  "/sys/servers/remote");
    n_assert(this->remoteServer);

    // read package toc files
    this->ReadTocFiles();

    // create status variables
    this->var_memalloc = (nEnv *) this->New("nenv","/sys/var/mem_alloc");
    this->var_memused  = (nEnv *) this->New("nenv","/sys/var/mem_used");
    this->var_memnumalloc = (nEnv *) this->New("nenv","/sys/var/mem_num");
    this->var_numobjects  = (nEnv *) this->New("nenv","/sys/var/obj_num");
}

/**
  The class destructor

  - 08-Oct-98   floh    created
  - 07-Dec-98   floh    + TimeServer
  - 26-Feb-99   floh    + kills MemHandler
  - 25-May-99   floh    + Stdout redirection
  - 07-Feb-00   floh    + more detailed shutdown messages
*/
nKernelServer::~nKernelServer(void)
{
    n_printf("-> shutting down kernel...\n");
    
    // kill time and file server
    n_printf("  time server killed...\n");
    if (this->ts) {
        this->ts->Release();
        this->ts = NULL;
    }
    n_printf("  time server killed...\n");

    if (this->persistServer) {
        this->persistServer->Release();
        this->persistServer = NULL;
     }
    n_printf("  persist server killed...\n");
    
    if (this->fileServer) {
        this->fileServer->Release();
        this->fileServer = NULL;
     }
    n_printf("  file server killed...\n");
    
    // kill object hierarchie
    this->root->Release();
    n_printf("  root killed...\n");

    // kill class list
    // ===============
    // Class objects must be released in inheritance order,
    // subclasses first, then parent class. Do multiple
    // runs over the class list and release classes
    // with a ref count of 0, until list is empty or
    // no classes with a ref count of 0 zero exists
    // (that's a fatal error, there are still objects
    // around of that class).
    bool is_empty;
    long num_zero_refs = 1;
    while ((!(is_empty = this->class_list.IsEmpty())) && (num_zero_refs > 0)) {
        num_zero_refs = 0;
        nClass *act_cl = (nClass *) this->class_list.GetHead();
        nClass *next_cl;
        do {
            next_cl = (nClass *) act_cl->GetSucc();
            if (act_cl->GetRef() == 0) {
                num_zero_refs++;
                act_cl->Remove();
                n_delete act_cl;
            }
            act_cl = next_cl;
        } while (act_cl);
    }
    if (!is_empty) {
        n_printf("~nKernelServer(): ref_count error cleaning up class list!\n");
        n_printf("Offending classes:\n");
        nClass* actCl;
        for (actCl = (nClass*) this->class_list.GetHead();
             actCl;
             actCl = (nClass*) actCl->GetSucc())
        {
            n_printf("%s: refcount %d\n", actCl->GetName(), actCl->GetRef());
        }
        n_error("Aborting!");
    }
    n_printf("  class list killed...\n");

    // kill the nHardRefServer
    n_delete this->hardRefServer;
    this->hardRefServer = 0;
    
    // kill package toc list
    nPckgTocEntry *pe;
    while ((pe = (nPckgTocEntry *) this->toc_list.RemHead())) n_delete pe;
    n_printf("<- kernel is down...\n");
    
    // NO n_printf() BEYOND THIS POINT!
    nNode* node = this->curLogHandlers->GetHead();
    n_delete (nDefaultLogHandler*)node->GetPtr();
    while ((node = this->curLogHandlers->RemHead()))
    {
        n_delete node;
    }
    n_delete this->curLogHandlers;

    ks = 0;
}

/**
  @brief Link new class module into the kernel.

  @param super super class for the new class
  @param cl the new class

  - 17-May-00   floh    created
*/
void nKernelServer::AddClass(const char *super, nClass *cl)
{
    nClass *s = this->OpenClass(super);
    if (s) s->AddSubClass(cl);
    else {
        n_error("nKernelServer::AddClass(): Could not open super class '%s'\n",super);
    }
}

/**
  @brief Remove class module from kernel.

  @param cl Pointer to class to be removed

  - 17-May-00   floh    created
*/
void nKernelServer::RemClass(nClass *cl)
{
    nClass *super = cl->GetSuperClass();
    n_assert(super);
    super->RemSubClass(cl);
    this->CloseClass(super);
}

/**
  @brief Returns a pointer to the class given in cl_name

  @param cl_name Name of the class

  - 08-Oct-98   floh    created
  - 04-Oct-98   floh    char * -> const char *
*/
nClass *nKernelServer::FindClass(const char *cl_name)
{
    n_assert(cl_name);
    nClass *cl = (nClass *) this->class_list.Find(cl_name);
    return cl;
}

/**
  Loads a class and return a pointer to it.
  First it is checked if the class is already loaded, if so the pointer to 
  this class is returned, otherwise first a loading from package is tried
  than loading from the dll itself.

  @param cl_name Name of the class to be opened

  - 08-Oct-98   floh    created
  - 04-Oct-98   floh    char * -> const char *
*/
nClass *nKernelServer::OpenClass(const char *cl_name)
{
    n_assert(cl_name);
    nClass *cl = this->_openClass(cl_name);
    if (cl) cl->AddRef();
    return cl;
}

/**
  @param cl class to be closed.

  - 08-Oct-98   floh    created
*/
void nKernelServer::CloseClass(nClass *cl)
{
    n_assert(cl);
    cl->RemRef();
}

/**
  @brief Return the list of all classes.

  - 08-Oct-98   floh    created
  - 17-May-99   floh    returns now nHashList 
*/
nHashList *nKernelServer::GetClassList(void)
{
    return &(this->class_list);
}

/**
  @brief Create an instance of the class @c cl_name, located at @c
  path in the hierarchy.

  An error will result in the application asserting and aborting.

  From C++:

@code
    nKernelServer * kernelServer;

    ...

    // Relative path:
    n3DNode * light1 = kernelServer->New("n3dnode", "light");
    // Absolute path:
    n3DNode * light2 = kernelServer->New("n3dnode", "/usr/scene/light");
@endcode

  From TCL:

@verbatim
    new n3dnode light             # Relative path
    new n3dnode /usr/scene/light  # Absolute path
@endverbatim

  From Python:

@verbatim
    light = new('n3dnode', 'light')              # Relative path
    light = new('n3dnode', '/usr/scene/light')   # Absolute path
@endverbatim


  @param cl_name Name of the object.  This is usually an all lowercase
         classname.
  @param path Path where to create the new object in the hierarchy. This
         may be either an absolute or relative path.
  @return The newly created object.

  - 08-Oct-98   floh    created
  - 04-Oct-98   floh    char * -> const char *
  - 15-Jul-99   floh    uses Link() on object
  - 29-Jul-99   floh    Link() killed
  - 24-Oct-99   floh    throws a fatal error if object could not 
                        be created
  - 04-Oct-00   floh    + keep pointer to last created object
*/
nRoot *nKernelServer::New(const char *cl_name, const char *path)
{
    n_assert(cl_name);
    n_assert(path);
    nRoot *o = _checkCreatePath(cl_name,path,true);
    n_assert(o);
    return o;
}

/**
  @brief Create an instance of the class @c cl_name, located at @c path
  in the hierarchy.
  
  Don't barf if object could not be created, silently return NULL instead.

  @param cl_name Name of the object.  This is usually an all lowercase
         classname.
  @param path Path where to create the new object in the hierarchy

  - 24-Oct-99   floh    created
  - 04-Oct-00   floh    + keep pointer to last created object
*/
nRoot *nKernelServer::NewNoFail(const char *cl_name, const char *path)
{
    n_assert(cl_name);
    n_assert(path);
    nRoot *o = _checkCreatePath(cl_name,path,false);
    return o;
}

/*
  @param path A path

  - 08-Oct-98   floh    created
  - 04-Oct-98   floh    char * -> const char *
  - 11-Nov-98   floh    implementiert
*/
nRoot *nKernelServer::Load(const char *path)
{
    n_assert(path);
    return this->persistServer->LoadObject(path);
}

/**
  @brief Look up a path and return the corresponding nRoot object.

  @param path A path
  @return the nRoot object for the specified @c path.

  - 08-Oct-98   floh    created
  - 04-Oct-98   floh    char * -> const char *
*/
nRoot *nKernelServer::Lookup(const char *path)
{
    n_assert(path);
    nRoot *o = _lookupPath(path);
    return o;
}

/**
  @brief Set the current working directory within the hiearchy to a
  particular (given) object.

  @param o Pointer to nRoot object

  - 08-Oct-98   floh    created
  - 13-May-99   floh    + if NULL pointer given, set root object
*/
nRoot *nKernelServer::SetCwd(nRoot *o)
{
    if (o) this->cwd = o;
    else   this->cwd = this->root;
    return this->cwd;
}

/**
  @brief Get the nRoot object for the current working directory within
  the hiearchy.

  - 08-Oct-98   floh    created
*/
nRoot *nKernelServer::GetCwd(void)
{
    return this->cwd;
}    

/**
  @brief Push the current working directory onto a stack and change
  the current working directory to the given nRoot object. Used in
  conjunction with PopCwd().

  @param o Pointer to nRoot object for new current working directory.

  - 28-Sep-00   floh    created
*/
void nKernelServer::PushCwd(nRoot *o)
{
    n_assert(o);
    this->cwd_stack.Push(this->cwd);
    this->cwd = o;
}

/**
  @brief Pop the previous directory that was pushed onto the stack
  and change the current working directory to that object.

  @return Returns the new working directory as an nRoot object.

  - 28-Sep-00   floh    created
*/
nRoot *nKernelServer::PopCwd(void)
{
    nRoot *prev_cwd = this->cwd;
    this->cwd = cwd_stack.Pop();
    return prev_cwd;
}

/**
  Status update for mem manager.
  Update mem status variables.

  - 13-May-99   floh    created
*/
void nKernelServer::Trigger(void)
{
    #ifdef __NEBULA_MEM_MANAGER__
    this->var_memalloc->SetI(n_memallocated);
    this->var_memused->SetI(n_memused);
    this->var_memnumalloc->SetI(n_memnumalloc);
    #else
    this->var_memalloc->SetS("nebula mem manager not active!");
    this->var_memused->SetS("nebula mem manager not active!");
    this->var_memnumalloc->SetS("nebula mem manager not active!");
    #endif
}

/**
    @brief Returns the current nRemoteServer object.
*/
nRemoteServer* nKernelServer::GetRemoteServer() const
{
    return this->remoteServer;
}

/**
    @brief Returns the nPersistServer object.
*/
nPersistServer* nKernelServer::GetPersistServer() const
{
    return this->persistServer;
}

/**
    @brief Returns the nFileServer2 object.
*/
nFileServer2* nKernelServer::GetFileServer2() const
{
    return this->fileServer;
}

//------------------------------------------------------------------------------
/**
    Get pointer to the default log handler object. This method can be used
    to obtain a pointer to the log line buffer object of the log handler.
    Please note that support for a line buffer is optional!
*/
nLogHandler*
nKernelServer::GetDefaultLogHandler() const
{
    return (nLogHandler*)this->curLogHandlers->GetHead()->GetPtr();
}


void
nKernelServer::VPrint(const char* str, va_list argList)
{
    nNode* node = this->curLogHandlers->GetHead();
    nLogHandler* logger;
    do 
    {
        logger = (nLogHandler*)node->GetPtr();
        logger->Print(str, argList);
    }
    while ((node = node->GetSucc()));
}

//------------------------------------------------------------------------------
/**
    Print a new text message to the log. This always works like a normal
    printf, it will never halt the program for user interaction.
*/
void
nKernelServer::Print(const char* str, ...)
{
    //n_assert(this->curLogHandler);
    va_list argList;
    va_start(argList, str);
    this->VPrint(str, argList);
    va_end(argList);    
}

void
nKernelServer::VMessage(const char* str, va_list argList)
{
    nNode* node = this->curLogHandlers->GetHead();
    nLogHandler* logger;
    do 
    {
        logger = (nLogHandler*)node->GetPtr();
        logger->Message(str, argList);
    }
    while ((node = node->GetSucc()));
}
//------------------------------------------------------------------------------
/**
    Show an important message which may require user interaction. The
    actual behaviour of this method depends on the actual log handler
    class (under Win32, a message box could be shown to the user).
*/
void
nKernelServer::Message(const char* str, ...)
{
    //n_assert(this->curLogHandler);
    va_list argList;
    va_start(argList, str);
    this->VMessage(str, argList);
    va_end(argList);    
}

void
nKernelServer::VError(const char* str, va_list argList)
{
    nNode* node = this->curLogHandlers->GetHead();
    nLogHandler* logger;
    do 
    {
        logger = (nLogHandler*)node->GetPtr();
        logger->Error(str, argList);
    }
    while ((node = node->GetSucc()));
}
//------------------------------------------------------------------------------
/**
    Show an error message which may require user interaction. The
    actual behaviour of this method depends on the actual log handler
    class (under Win32, a message box could be shown to the user).
*/
void
nKernelServer::Error(const char* str, ...)
{
    //n_assert(this->curLogHandler);
    va_list argList;
    va_start(argList, str);
    this->VError(str, argList);
    va_end(argList);    
}

//------------------------------------------------------------------------------
/**
    Set new log handler. NOTE: the nKernelServer does not gain ownership of
    the log handler, it just uses it. Whoever sets a log handler is responsible
    to cleanly release it, and must make sure that the kernel server is
    not left with an invalid pointer by set a zero log handler.

    @param  logHandler  pointer to a new nLogHandler object, or 0 to 
                        restore the default log handler
*/
void
nKernelServer::AddLogHandler( nLogHandler* log )
{
    if(log)
    {
       nNode* node = n_new nNode(log);
       this->curLogHandlers->AddTail(node);
    }
}

//------------------------------------------------------------------------------
/**
    Removes a log handler from the list of handlers	

    @param  logHandler  The pointer to the log handler to remove
*/
void
nKernelServer::RemoveLogHandler( nLogHandler* log )
{
    nNode* node = this->curLogHandlers->GetHead();
    while ((node = node->GetSucc()))
    {
        if (node->GetPtr() == log)
        {
            node->Remove();
            n_delete node;
            break;
        }
    }
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
