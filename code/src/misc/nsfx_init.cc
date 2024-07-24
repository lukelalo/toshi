#define N_IMPLEMENTS nSpecialFxServer
//--------------------------------------------------------------------
//  nsfx_init.cc
//  (C) 2000 RadonLabs GmbH -- Andre Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "misc/nspecialfxserver.h"

nClass *nSpecialFxServer::local_cl  = NULL;
nKernelServer *nSpecialFxServer::ks = NULL;

extern char *nSpecialFxServer_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nSpecialFxServer::local_cl = cl;
    nSpecialFxServer::ks       = ks;
    nClass *super = ks->OpenClass("nroot");
    if (super) super->AddSubClass(nSpecialFxServer::local_cl);
    else n_error("Could not open superclass 'nroot'!\n");
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nClass *super = nSpecialFxServer::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nSpecialFxServer::local_cl);
        nSpecialFxServer::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nSpecialFxServer;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nSpecialFxServer_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
