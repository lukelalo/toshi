#define N_IMPLEMENTS nWeightTree
//--------------------------------------------------------------------
//  nweighttree.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "node/nweighttree.h"

nClass *nWeightTree::local_cl  = NULL;
nKernelServer *nWeightTree::ks = NULL;

extern char *nWeightTree_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_new(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nWeightTree::local_cl = cl;
    nWeightTree::ks       = ks;
    nClass *super = ks->OpenClass("nvisnode");
    if (super) super->AddSubClass(nWeightTree::local_cl);
    else n_error("Could not open superclass '%s'\n");
    cl->BeginCmds();
    cl->AddCmd("b_addleaf_s",   nID('A','L','E','F'));
    cl->AddCmd("b_addnode_sss", nID('A','N','O','D'));
    cl->EndCmds();
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    // von Superklasse abkoppeln
    nClass *super = nWeightTree::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nWeightTree::local_cl);
        nWeightTree::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_new()
//--------------------------------------------------------------------
N_EXPORT void *n_new(void)
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


