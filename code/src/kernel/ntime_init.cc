#define N_IMPLEMENTS nTimeServer
#define N_KERNEL
//--------------------------------------------------------------------
//  IMPLEMENTATION
//      nTimeServer -- Keeper Of The Time
//
//  (C) 1998 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"

nClass *nTimeServer::local_cl  = NULL;
nKernelServer *nTimeServer::ks = NULL;

extern void ntime_initcmds(nClass *);
extern char *nkernel_version;
extern "C" bool N_EXPORT ntimeserver_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT ntimeserver_fini(void);
extern "C" N_EXPORT void *ntimeserver_new(void);
extern "C" N_EXPORT char *ntimeserver_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT ntimeserver_init(nClass *cl, nKernelServer *ks)
{
    nTimeServer::local_cl = cl;
    nTimeServer::ks       = ks;
    
    // mit Superklasse verketten
    nClass *super = ks->OpenClass("nroot");
    if (super) super->AddSubClass(nTimeServer::local_cl);
    else n_error("Could not open superclass nroot!");

    ntime_initcmds(cl);
    
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT ntimeserver_fini(void)
{
    // von Superklasse abkoppeln
    nClass *super = nTimeServer::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nTimeServer::local_cl);
        nTimeServer::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *ntimeserver_new(void)
{
    return n_new nTimeServer;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *ntimeserver_version(void)
{
    return nkernel_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
