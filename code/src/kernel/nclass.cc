#define N_IMPLEMENTS nClass
#define N_KERNEL
//--------------------------------------------------------------------
//  nclass.cc
//  (C) 1998..2000 Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>

#include "kernel/nclass.h"
#include "kernel/ncmdprotonative.h"
#include "util/nhashlist.h"
#include "kernel/nroot.h"
#include "util/nkeyarray.h"

extern "C" bool root_init(nClass *, nKernelServer *);
extern "C" void root_fini(void);
extern "C" void *root_new(void);

extern "C" bool npersistserver_init(nClass *, nKernelServer *);
extern "C" void npersistserver_fini(void);
extern "C" void *npersistserver_new(void);

extern "C" bool nfileserver2_init(nClass *, nKernelServer *);
extern "C" void nfileserver2_fini(void);
extern "C" void *nfileserver2_create(void);

extern "C" bool nscriptserver_init(nClass *, nKernelServer *);
extern "C" void nscriptserver_fini(void);
extern "C" void *nscriptserver_new(void);

extern "C" bool ntimeserver_init(nClass *, nKernelServer *);
extern "C" void ntimeserver_fini(void);
extern "C" void *ntimeserver_new(void);

extern "C" bool nremoteserver_init(nClass *, nKernelServer *);
extern "C" void nremoteserver_fini(void);
extern "C" void *nremoteserver_create(void);

extern "C" bool nenv_init(nClass *, nKernelServer *);
extern "C" void nenv_fini(void);
extern "C" void *nenv_new(void);

/**
  @param name Name of the class
  @param csrv Pointer to the kernel sever

  loads the class from the dll
*/
//--------------------------------------------------------------------
//  load_class()
//--------------------------------------------------------------------
bool nClass::load_class(const char *name, nKernelServer *csrv)
{
    bool retval = false;
    bool image_ok = false;

    // FIXME: catch special case kernel classes
    if (strcmp(name,"nroot") == 0) 
    {
        this->dll_image = NULL;
        this->n_init_ptr    = root_init;
        this->n_fini_ptr    = root_fini;
        this->n_new_ptr     = root_new;
        image_ok = true; 
    } 
    else if (strcmp(name,"npersistserver") == 0) 
    {
        this->dll_image     = NULL;
        this->n_init_ptr    = npersistserver_init;
        this->n_fini_ptr    = npersistserver_fini;
        this->n_new_ptr     = npersistserver_new;
        image_ok = true;
    } 
    else if (strcmp(name,"nfileserver2") == 0) 
    {
        this->dll_image     = 0;
        this->n_init_ptr    = nfileserver2_init;
        this->n_fini_ptr    = nfileserver2_fini;
        this->n_new_ptr     = nfileserver2_create;
        image_ok = true;
    } 
    else if (strcmp(name,"nscriptserver") == 0) 
    {
        this->dll_image = NULL;
        this->n_init_ptr    = nscriptserver_init;
        this->n_fini_ptr    = nscriptserver_fini;
        this->n_new_ptr     = nscriptserver_new;
        image_ok = true;
    } 
    else if (strcmp(name,"ntimeserver") == 0) 
    {
        this->dll_image = NULL;
        this->n_init_ptr    = ntimeserver_init;
        this->n_fini_ptr    = ntimeserver_fini;
        this->n_new_ptr     = ntimeserver_new;
        image_ok = true;
    } 
    else if (strcmp(name,"nenv") == 0) 
    {
        this->dll_image = NULL;
        this->n_init_ptr    = nenv_init;
        this->n_fini_ptr    = nenv_fini;
        this->n_new_ptr     = nenv_new;
        image_ok = true;
    }
    else if (strcmp(name,"nremoteserver") == 0)
    {
        this->dll_image = NULL;
        this->n_init_ptr    = nremoteserver_init;
        this->n_fini_ptr    = nremoteserver_fini;
        this->n_new_ptr     = nremoteserver_create;
        image_ok = true;
    }
    else 
    {
        // normal case, try to open class from dll
        this->dll_image = n_dllopen(name);
        if (this->dll_image) 
        {
            this->n_init_ptr    = (bool (*) (nClass *,nKernelServer *)) 
                              n_dllsymbol(this->dll_image,"n_init");
            this->n_fini_ptr    = (void (*) (void)) 
                              n_dllsymbol(this->dll_image,"n_fini");
            this->n_new_ptr     = (void * (*)(void)) 
                              n_dllsymbol(this->dll_image,"n_create");
            image_ok = true;
        }
    }
    if (image_ok) 
    { 
        if (this->n_init_ptr(this,csrv))
        {
            retval = true;
        } 
        else
        {
            n_error("nClass::load_class(), calling n_init() failed!");
        }
    }
    return retval;
}

/**
  Calls class n_fini method to cleanup the class and unloads the dll
*/
//--------------------------------------------------------------------
//  unload_class()
//--------------------------------------------------------------------
void nClass::unload_class(void)
{
    // call class destructor and unload dll
    if (this->n_fini_ptr) this->n_fini_ptr();
    if (this->dll_image) n_dllclose(this->dll_image);
    this->dll_image = NULL;
    this->n_init_ptr = NULL;
    this->n_fini_ptr = NULL;
    this->n_new_ptr  = NULL;
}            
    
/**
  @param name The name of the class
  @param kserv Pointer to the kernel server

  - 08-Aug-99   floh    Header
  - 24-Oct-99   floh    no longer load class code here, to fix
                        problems when class can not be loaded
  - 18-Feb-00   floh    + cmd_table is now a nKeyArray
*/
nClass::nClass(const char *name, nKernelServer *kserv)
         :nHashNode(name),
          cmd_list(64),
          script_cmd_list(0)
{
    n_assert(name);
    n_assert(kserv);
    this->ks            = kserv;
    this->opened        = false;
    this->in_begin_cmds = false;
    this->cmd_table  = NULL;
    this->superclass = NULL;
    this->ref_count  = 0;
    this->dll_image  = NULL;
    this->n_init_ptr = NULL;
    this->n_fini_ptr = NULL;
    this->n_new_ptr  = NULL;
}

/**
  @param name The name of the class
  @param kserv Pointer to the kernel server
  @param _init Pointer to the n_init function of this class
  @param _fini Pointer to the n_fini function
  @param _new Pointer to class constructor

  - 08-Aug-99   floh    Header
  - 24-Oct-99   floh    don't actually initialize code module
  - 18-Feb-00   floh    + cmd_table is now a nKeyArray
*/
nClass::nClass(const char *name,
               nKernelServer *kserv,
               bool (*_init)(nClass *, nKernelServer *),
               void (*_fini)(void),
               void *(*_new)(void))
               : nHashNode(name),
                 cmd_list(64),
                 script_cmd_list(0)
{
    n_assert(name);
    n_assert(kserv);
    n_assert(_init);
    n_assert(_fini);
    n_assert(_new);
    this->ks            = kserv;
    this->opened        = false; 
    this->in_begin_cmds = false;
    this->cmd_table  = NULL;
    this->superclass = NULL;
    this->ref_count  = 0;
    this->dll_image  = NULL;
    this->n_init_ptr     = _init;
    this->n_fini_ptr     = _fini;
    this->n_new_ptr      = _new;
}

/**

  - 04-Aug-99   floh    boeser Bug: nCmdProto-Liste wurde als nCmd 
                        Objekte freigegeben...

  - 18-Feb-00   floh    + cmd_table is now a nKeyArray
*/
nClass::~nClass(void)
{
    nCmdProto *cp;
    if (this->opened) this->Close();
    n_assert(this->superclass == NULL);
    n_assert(this->ref_count == 0);
    while ((cp = (nCmdProto *) this->cmd_list.RemHead())) 
    {
        n_delete cp;
    }
    if (this->cmd_table) 
    {
        n_delete this->cmd_table;
    }
    if ( this->script_cmd_list )
    {
      while ((cp = (nCmdProto *) this->script_cmd_list->RemHead()))
          n_delete cp;
      n_delete this->script_cmd_list;
    }
}

/**
  Loads the class from a package if this class is a package class else
  the clas is loaded from a dll

  - 24-Oct-99   floh    created
*/
bool nClass::Open(void)
{
    n_assert(!this->opened);
    const char *n = this->GetName();
    if (!this->n_init_ptr) 
    {
        // not a packaged class, try to load directly from dll
        if (this->load_class(n,this->ks)) 
        {
            this->opened = true;
        } 
        else 
        {
            n_printf("Could not open class '%s'\n",n);
            return false;
        }
    } 
    else 
    {
        // a packaged class, just call class constructor
        if (this->n_init_ptr(this,ks)) 
        {
            this->opened = true;
        } 
        else 
        {
            n_printf("n_init_ptr() failed on packaged class '%s'\n",n);
        }
    }
    return this->opened;
}

/**
  Calls unload_class to unload the class

  - 24-Oct-99   floh    created
*/
void nClass::Close(void)
{
    n_assert(this->opened);
    this->unload_class();
}

/**

  - 08-Aug-99   floh    Header
*/
nRoot *nClass::NewObject(void)
{
    nRoot *obj = (nRoot *) this->n_new_ptr();
    if (obj) 
    {
        obj->SetClass(this);
        this->AddRef();  
    }
    return obj;
}

/**
  @param obj Pointer to the object to be deleted

  - 08-Aug-99   floh    Header
*/
void nClass::DelObject(nRoot *obj)
{
    n_assert(obj);
    this->RemRef();
}

/**

  - 08-Aug-99   floh    created
*/
void nClass::BeginCmds(void)
{
    n_assert(!this->in_begin_cmds);
    n_assert(NULL == this->cmd_table);
    this->in_begin_cmds = true;
}

/**
  @param proto_def The command string
  @param id The command id

  - 08-Aug-99   floh    Header
*/
void nClass::AddCmd(const char *proto_def, ulong id)
{
    n_assert(this->in_begin_cmds);
    n_assert(proto_def);
    n_assert(id);
    nCmdProtoNative *cp = n_new nCmdProtoNative(proto_def,id,NULL);
    if (cp) 
    {
        this->cmd_list.AddTail(cp);
    }
}

/**
  @param proto_def The command string
  @param id The command id
  @param cmd_proc

  - 08-Aug-99   floh    new version with cmd_proc support
*/
void nClass::AddCmd(const char *proto_def, ulong id, void (*cmd_proc)(void *, nCmd *))
{
    n_assert(this->in_begin_cmds);
    n_assert(proto_def);
    n_assert(id);
    n_assert(cmd_proc);
    nCmdProtoNative *cp = n_new nCmdProtoNative(proto_def,id,cmd_proc);
    if (cp) 
    {
        this->cmd_list.AddTail(cp);
    }
}

/**
  Build sorted array of attached cmds for a bsearch() by ID. 
  TODO: check duplicate cmd IDs!

  - 08-Aug-99   floh    created
  - 24-Oct-99   floh    checks for identical cmd ids and throws
                        an error 
  - 18-Feb-00   floh    cmd_table now a nKeyArray
*/
void nClass::EndCmds(void)
{
    n_assert(this->in_begin_cmds);
    n_assert(NULL == this->cmd_table);
    this->in_begin_cmds = false;
    nClass *cl;

    // count commands
    int num_cmds = 0;
    cl = this;
    do 
    {
        nHashNode *node;
        for (node = cl->cmd_list.GetHead();
             node;
             node = node->GetSucc())
        {
            num_cmds++;
        }
    } while ((cl=cl->GetSuperClass()));

    // create and fill command table
    this->cmd_table = n_new nKeyArray<nCmdProtoNative *>(num_cmds);
    cl = this;
    do 
    {
        nCmdProtoNative *cp;
        for (cp = (nCmdProtoNative *) cl->cmd_list.GetHead();
             cp;
             cp = (nCmdProtoNative *) cp->GetSucc())
        {
            this->cmd_table->Add(cp->GetID(),cp);
        }
    } while ((cl=cl->GetSuperClass()));

    // check for identical cmd ids
    int i;
    for (i=0; i<(num_cmds-1); i++) 
    {
        if (this->cmd_table->GetKeyAt(i) == this->cmd_table->GetKeyAt(i+1)) 
        {
            nCmdProto *cp0 = (nCmdProto *) this->cmd_table->GetElementAt(i);
            nCmdProto *cp1 = (nCmdProto *) this->cmd_table->GetElementAt(i+1);
            n_error("ERROR: Command id collission in class '%s'\n"
                     "cmd '%s' and cmd '%s' both have id '0x%lx'\n",
                     this->GetName(),
                     cp0->GetName(),
                     cp1->GetName(),
                     this->cmd_table->GetKeyAt(i));
        }
    }
}

/**
  @param numCmds The number of script cmds that will be defined.
*/
void nClass::BeginScriptCmds(int numCmds)
{
    n_assert(!this->script_cmd_list);
    this->script_cmd_list = n_new nHashList(numCmds);
}

/**
  Can only be called between Begin/EndScriptCmds().
  @param cmdProto A cmd proto created by a script server.
  @note Script server cmd protos can't be looked up by their 4cc,
        since they don't have a 4cc :).
*/
void nClass::AddScriptCmd(nCmdProto* cmdProto)
{
    n_assert(this->script_cmd_list);
    n_assert(this->opened);
    n_assert(cmdProto);
    this->script_cmd_list->AddTail(cmdProto);
}

/**
*/
void nClass::EndScriptCmds()
{
    // empty
}

/**
  @param name The name of the command to be found

  - 08-Aug-99   floh    Header
*/
nCmdProto *nClass::FindCmdByName(const char *name)
{
    n_assert(name);
    nCmdProto *cp;
    // try the native cmd list first
    cp = (nCmdProto *) this->cmd_list.Find(name);
    // if that fails try the script cmd list
    if (this->script_cmd_list && !cp)
        cp = (nCmdProto *) this->script_cmd_list->Find(name);
    // if not found, recursively hand up to parent class
    if ((!cp) && (this->superclass)) 
    {
        cp = this->superclass->FindCmdByName(name);
    }
    return cp;
}

/**
  @param name The name of the command to be found
*/
nCmdProtoNative *nClass::FindNativeCmdByName(const char *name)
{
    n_assert(name);
    nCmdProtoNative *cp;
    cp = (nCmdProtoNative *) this->cmd_list.Find(name);
    // if not found, recursively hand up to parent class
    if ((!cp) && (this->superclass)) 
    {
        cp = this->superclass->FindNativeCmdByName(name);
    }
    return cp;
}

/**
  @param name The name of the command to be found
*/
nCmdProto *nClass::FindScriptCmdByName(const char *name)
{
    n_assert(name);
    nCmdProto *cp;
    if (this->script_cmd_list)
        cp = (nCmdProto *) this->script_cmd_list->Find(name);
    // if not found, recursively hand up to parent class
    if ((!cp) && (this->superclass)) 
    {
        cp = this->superclass->FindScriptCmdByName(name);
    }
    return cp;
}

/**
  @param id The id of the command to be found

  - 08-Aug-99   floh    Header
  - 08-Aug-99   floh    + macht jetzt einen Binary-Search auf die
                          Cmd-Table, anstatt sich linear durch die
                          Cmd-Liste zu hangeln.
  - 25-Jan-99   floh    + bsearch() slightly optimized
  - 18-Feb-00   floh    + cmd_table now nKeyArray
*/
nCmdProto *nClass::FindCmdById(ulong id)
{
    // HACK: if the class has no commands, hand the request
    // to the parent class.
    if (!this->cmd_table) 
    {
        nClass *cl = this->GetSuperClass();
        if (cl) return cl->FindCmdById(id);
        else    return NULL;
    } 
    else 
    {
        nCmdProtoNative *cp = NULL;
        if (this->cmd_table->Find((int)id,cp)) return (nCmdProto *) cp;
        else                                   return NULL;
    }
}    

/**
  @param cl Pointer to the class to be added as subclass

  - 08-Aug-99   floh    Header
*/
void nClass::AddSubClass(nClass *cl)
{
    this->AddRef();
    cl->superclass = this;
}

/**
  @param cl Pointer to the class to be removed from subclasses

  - 08-Aug-99   floh    Header
*/
void nClass::RemSubClass(nClass *cl)
{
    this->RemRef();
    cl->superclass = NULL;
}        
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------






