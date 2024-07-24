#define N_IMPLEMENTS nMathServer
//--------------------------------------------------------------------
//  nmathserver.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "misc/nmathserver.h"

nClass *nMathServer::local_cl  = NULL;
nKernelServer *nMathServer::ks = NULL;

extern char *nMathServer_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nMathServer::local_cl = cl;
    nMathServer::ks       = ks;
    
    // mit Superklasse verketten
    nClass *super = ks->OpenClass("nroot");
    if (super) super->AddSubClass(nMathServer::local_cl);
    else n_error("could not open superclass 'nroot'\n");
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    // von Superklasse abkoppeln
    nClass *super = nMathServer::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nMathServer::local_cl);
        nMathServer::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_new()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nMathServer;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nMathServer_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

