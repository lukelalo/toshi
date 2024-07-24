#define N_IMPLEMENTS nPersistServer
#define N_KERNEL
//--------------------------------------------------------------------
//  IMPLEMENTATION
//  nPersistServer
//
//  (C) 1998 Andre Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/npersistserver.h"

nKernelServer* nPersistServer::kernelServer = 0;

extern void npserv_initcmds(nClass *);
extern char *nkernel_version;
extern "C" bool npersistserver_init(nClass *, nKernelServer *);
extern "C" void npersistserver_fini(void);
extern "C" void *npersistserver_new(void);
extern "C" char *npersistserver_version(void);

//--------------------------------------------------------------------
//  n_init()
//  03-Nov-98   floh    created
//  11-Nov-98   floh    getrootdir/setrootdir rausgeflogen
//  10-Jan-99   floh    + parsefile
//  21-Jun-99   floh    + 'parsefile' und 'loadobject' gekillt
//--------------------------------------------------------------------
bool npersistserver_init(nClass *cl, nKernelServer *ks)
{
    nPersistServer::kernelServer = ks;
    nPersistServer::local_cl = cl;
    
    // mit Superklasse verketten
    nClass *super = ks->OpenClass("nroot");
    if (super) super->AddSubClass(nPersistServer::local_cl);
    else n_error("nPersistServer::n_init(): unknown superclass nroot");
    
    // Scriptkommandos
    npserv_initcmds(cl);
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//  03-Nov-98   floh    created
//--------------------------------------------------------------------
void npersistserver_fini(void)
{
    // von Superklasse abkoppeln
    nClass *super = nPersistServer::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nPersistServer::local_cl);
        nPersistServer::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
void *npersistserver_new(void)
{
    return n_new nPersistServer;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
char *npersistserver_version(void)
{
   return nkernel_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
