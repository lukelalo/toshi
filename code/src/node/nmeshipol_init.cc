#define N_IMPLEMENTS nMeshIpol
//--------------------------------------------------------------------
//  nmeshipol_init.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "node/nmeshipol.h"

nClass *nMeshIpol::local_cl  = NULL;
nKernelServer *nMeshIpol::ks = NULL;

extern char *nMeshIpol_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);
extern void n_initcmds(nClass *);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nMeshIpol::local_cl = cl;
    nMeshIpol::ks       = ks;
    ks->AddClass("nanimnode",cl);
    n_initcmds(cl);
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nMeshIpol::ks->RemClass(nMeshIpol::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nMeshIpol;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nMeshIpol_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
