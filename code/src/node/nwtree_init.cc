#define N_IMPLEMENTS nWeightTree
//--------------------------------------------------------------------
//  nweighttree.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "node/nweighttree.h"

nClass *nWeightTree::local_cl  = NULL;
nKernelServer *nWeightTree::ks = NULL;

extern void n_initcmds(nClass *);
extern char *nWeightTree_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nWeightTree::local_cl = cl;
    nWeightTree::ks       = ks;
    ks->AddClass("nvisnode", cl);
    n_initcmds(cl);
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nWeightTree::ks->RemClass(nWeightTree::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nWeightTree;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nWeightTree_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

