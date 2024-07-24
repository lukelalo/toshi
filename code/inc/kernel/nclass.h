#ifndef N_CLASS_H
#define N_CLASS_H
//--------------------------------------------------------------------
/**
    @defgroup NebulaObjectSystem The Nebula Object System
    @ingroup NebulaKernelModule
    @brief The Nebula object system provides higher level capabilities
    than the C++ object model, allowing for demand-loaded classes from
    shared libraries, persistence, easy scripting support, and a
    hierarchical name space.

    Nebula's C++ classes live in DLLs, which are loaded on demand when the
    first object of a class is created.  When Nebula is initialized,
    it reads in all of the @c .toc files in the @c bin: directory via
    nKernelServer::ReadTocFiles().  This initializes the list of
    classes (instances of nClass) that are available along with which
    DLL those classes are implemented by.

    Nebula objects live in a hierarchical tree of named nodes similar to a
    file system hierarchy. An object name can be converted into a C++ pointer
    and a C++ pointer can be converted into a name at any time. Having named
    objects is very convenient when referencing objects through the
    script interface, or in their persistent state. Runtime efficiency doesn't
    suffer from this, since normally, references by name are resolved into
    references by pointer only once at load/initialization time. During an
    object's lifetime, references to other objects are normally kept as
    cached C++ pointers. (See @ref SmartPointers for more information.)

    Nebula objects survive on disk as ascii scripts that contain exactly
    the sequence of script commands that would put a default object of that
    class into an exact clone of the object which created the script. Since
    the object persistency mechanism reuses the script interface there is
    no conceptual difference between a "Nebula script" and a "Nebula data
    file".  (See nPersistServer for information about serialization.)
*/
/**
    @class nClass
    @ingroup NebulaObjectSystem

    @brief represent a dynamically loaded class

    All currently loaded and initialised classes are registered in 
    a global list of class objects in the kernel server . 
    When an object is created through the nKernelServer, it will first 
    search the class list whether the object's class is already loaded
    and initialized. If not, the DLL loader will attempt to load the
    class module by trying to open a DLL with the same name as the
    class that is being looked for. Once a module is loaded, it is
    initialized by calling the n_init() function of the module.
    Inside n_init() the class will register itself with the kernel
    server and initialize any class specific data (like the list
    of script commands that instances of that class accept).

    Objects are created through the class modules n_new()
    function, which may only be called by the kernel server.
    The public interface for allocating instances of
    classes is nKernelServer::New() which can be used both
    from C++ and the script interface.

    When the kernel server decided that a class should be
    unloaded, it will call the module's n_fini() function
    before the dll loader will actually unload the module.
  
    (C) 1999 A.Weissflog
*/
//--------------------------------------------------------------------

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_HASHLIST_H
#include "util/nhashlist.h"
#endif

#ifndef N_HASHNODE_H
#include "util/nhashnode.h"
#endif

#ifndef N_CMDPROTO_H
#include "kernel/ncmdproto.h"
#endif

#ifndef N_KEYARRAY_H
#include "util/nkeyarray.h"
#endif

#undef N_DEFINES
#define N_DEFINES nClass
#include "kernel/ndefdllclass.h"

/**
  @class nIdCmdAssoc
  @brief 


*/
//--------------------------------------------------------------------
struct nIdCmdAssoc {
    long id;        // four cc command id
    nCmdProto *cp;
};

class nCmdProtoNative;
class nKernelServer;
class nRoot;
class N_PUBLIC nClass : public nHashNode {

    /// Reference to the kernel server
    nKernelServer *ks;
    bool in_begin_cmds;
    /// Returns true if the class is opened
    bool opened;
    /// The superclass of this class
    nClass *superclass;
    /// The reference count for this class
    long ref_count;
        
    /// The hashed native commandlist of this class
    nHashList cmd_list;             // optimized for Find By Name
    /// The native commandlist in a keyarray
    nKeyArray<nCmdProtoNative *> *cmd_table;   // optimized for Find By ID
    /// The hashed script commandlist of this class
    nHashList* script_cmd_list;
    
    /// Pointer to the dll the class comes from
    void *dll_image;
    /// The n_init function of this class
    bool (*n_init_ptr)(nClass *, nKernelServer *);
    /// The n_fini function of this class
    void (*n_fini_ptr)(void);
    /// The constructor function for dll classes 
    void *(*n_new_ptr)(void);
    
public:
    /// Class constructor
    nClass(const char *name, nKernelServer *ks);
    /// Class constructor
    nClass(const char *name,
           nKernelServer *ks,
           bool (*)(nClass *, nKernelServer *),
           void (*)(void),
           void *(*)(void));
    /// Class destructor
    virtual ~nClass(void);

    /// Opens the dll class
    bool Open(void);
    /// Closes the dll class
    void Close(void);
    /// create a new object
    nRoot *NewObject(void);
    /// delete a object
    void DelObject(nRoot *);

    // reference counting
    /// Adds a reference to this class
    long AddRef(void) { return ++this->ref_count; };
    /// Removes a reference from this class
    long RemRef(void) { n_assert(this->ref_count!=0); return --this->ref_count; };
    /// Returns the reference count for this class
    long GetRef(void) { return this->ref_count; };
    /// start defining commands
    void BeginCmds(void);
    /// Add a command for this class
    void AddCmd(const char *proto_def, ulong id);
    /// Add a command for this class
    void AddCmd(const char *proto_def, ulong id, void (*)(void *, nCmd *));
    /// finish defining commands
    void EndCmds(void);
    /// Start defining script-side commands
    void BeginScriptCmds(int numCmds);
    /// Add a script-side command for this class
    void AddScriptCmd(nCmdProto*);
    /// Finish defining script-side commands
    void EndScriptCmds(void);
    /// Looks up a command (searches both native & script-side)
    nCmdProto *FindCmdByName(const char *name);
    /// Looks up a native command by name
    nCmdProtoNative *FindNativeCmdByName(const char *name);
    /// Looks up a script-side command by name
    nCmdProto *FindScriptCmdByName(const char *name);
    /// Looks up a native command
    nCmdProto *FindCmdById(ulong id);
    /// Returns the command list for this class
    nHashList *GetCmdList(void) { return &(this->cmd_list); };
    /// add class
    void AddClass(const char *super, nClass *cl);
    /// remove class
    void RemClass(nClass *cl);

    // *** OBSOLETE LOW LEVEL CLASS FUNCTIONS ***
    /// Returns the superclass of this class
    nClass *GetSuperClass(void) { return this->superclass; };
    /// Add a subclass to this class
    void AddSubClass(nClass *cl);
    /// Remove a subclass from this class
    void RemSubClass(nClass *cl);
                
private:
    bool load_class(const char *name, nKernelServer *csrv);
    void unload_class(void);
};
//--------------------------------------------------------------------
#endif
