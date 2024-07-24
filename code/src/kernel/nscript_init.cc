#define N_IMPLEMENTS nScriptServer
#define N_KERNEL
//--------------------------------------------------------------------
//  IMPLEMENTATION
//      nScriptServer -- Superklasse fuer konkrete ScriptServer
//
//  (C) 1998 Andre Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"

nClass *nScriptServer::local_cl  = NULL;
nKernelServer *nScriptServer::kernelServer = NULL;

extern char *nkernel_version;
extern "C" bool N_EXPORT nscriptserver_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT nscriptserver_fini(void);
extern "C" N_EXPORT void *nscriptserver_new(void);
extern "C" N_EXPORT char *nscriptserver_version(void);

//--------------------------------------------------------------------
//  n_init()
//  09-Oct-98   floh    created
//--------------------------------------------------------------------
bool N_EXPORT nscriptserver_init(nClass *cl, nKernelServer *ks)
{
    nScriptServer::local_cl = cl;
    nScriptServer::kernelServer = ks;
    ks->AddClass("nroot",cl);
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//  09-Oct-98   floh    created
//--------------------------------------------------------------------
void N_EXPORT nscriptserver_fini(void)
{
    nScriptServer::kernelServer->RemClass(nScriptServer::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//  09-Oct-98   floh    created
//--------------------------------------------------------------------
N_EXPORT void *nscriptserver_new(void)
{
    return n_new nScriptServer;
}

//--------------------------------------------------------------------
//  n_version()
//  09-Oct-98   floh    created
//--------------------------------------------------------------------
N_EXPORT char *nscriptserver_version(void)
{
    return nkernel_version;
}

//--------------------------------------------------------------------
//  ~nScriptServer()
//  31-Oct-98   floh    created
//  21-Jul-99   floh    korrektes Aufraeumen, wenn im Server oder
//                      Client-Modus
//--------------------------------------------------------------------
nScriptServer::~nScriptServer()
{
}


nScriptServer::nScriptServer() :
    quitRequested(false),
    failOnError(false)
{
    SetSelectMethod(SELCOMMAND);
}
//--------------------------------------------------------------------
//  Run()
//  10-Oct-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::Run(const char *, const char*&)
{
    n_error("pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  RunScript()
//  10-Oct-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::RunScript(const char *, const char*&)
{
    n_error("pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  BeginWrite()
//--------------------------------------------------------------------
nFile*
nScriptServer::BeginWrite(const char * /*filename */, nRoot * /* obj */)
{
    n_error("pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  WriteBeginNewObject()
//  04-Nov-98   floh    created
//  06-Mar-00   floh    + new name
//--------------------------------------------------------------------
bool nScriptServer::WriteBeginNewObject(nFile *, nRoot *, nRoot *)
{
    n_error("pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  WriteBeginNewObjectCmd()
//  04-Nov-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::WriteBeginNewObjectCmd(nFile *, nRoot *, nRoot *, nCmd *)
{
    n_error("pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  WriteBeginSelObject()
//  04-Nov-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::WriteBeginSelObject(nFile *, nRoot *, nRoot *)
{
    n_error("pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  WriteCmd()
//  04-Nov-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::WriteCmd(nFile *, nCmd *)
{
    n_error("pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  WriteEndObject()
//  04-Nov-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::WriteEndObject(nFile *, nRoot *, nRoot *)
{
    n_error("pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  WriteEnd()
//  04-Nov-98   floh    created
//--------------------------------------------------------------------
bool
nScriptServer::EndWrite(nFile *)
{
    n_error("pure virtual function called!");
    return false;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

