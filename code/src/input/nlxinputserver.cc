#define N_IMPLEMENTS nLXInputServer
//--------------------------------------------------------------------
//  nlxinputserver.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "input/nlxinputserver.h"

nClass *nLXInputServer::local_cl  = NULL;
nKernelServer *nLXInputServer::ks = NULL;

extern char *nLXInputServer_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nLXInputServer::local_cl = cl;
    nLXInputServer::ks       = ks;
    
    // mit Superklasse verketten
    nClass *super = ks->OpenClass("ninputserver");
    if (super) super->AddSubClass(nLXInputServer::local_cl);
    else n_error("Could not open superclass ninputserver!");
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    // von Superklasse abkoppeln
    nClass *super = nLXInputServer::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nLXInputServer::local_cl);
        nLXInputServer::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_new()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nLXInputServer;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nLXInputServer_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------


