#define N_IMPLEMENTS nMeshCluster
//--------------------------------------------------------------------
//  nmeshcluster_init.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "node/nmeshcluster.h"

nClass *nMeshCluster::local_cl  = NULL;
nKernelServer *nMeshCluster::ks = NULL;

extern char *nMeshCluster_version;
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
    nMeshCluster::local_cl = cl;
    nMeshCluster::ks       = ks;
    ks->AddClass("nvisnode",cl);
    n_initcmds(cl);
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nMeshCluster::ks->RemClass(nMeshCluster::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nMeshCluster;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nMeshCluster_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
