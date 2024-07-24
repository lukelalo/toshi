#define N_IMPLEMENTS nFogNode
//--------------------------------------------------------------------
//  nfog_init.cc
//  (C) 2000 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "node/nfognode.h"

nClass *nFogNode::local_cl  = NULL;
nKernelServer *nFogNode::ks = NULL;

extern char *nFogNode_version;
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
    nFogNode::local_cl = cl;
    nFogNode::ks       = ks;
    nClass *super = ks->OpenClass("nvisnode");
    if (super) super->AddSubClass(nFogNode::local_cl);
    else n_error("Could not open superclass 'nvisnode'\n");
    n_initcmds(cl);
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nClass *super = nFogNode::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nFogNode::local_cl);
        nFogNode::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nFogNode;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nFogNode_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
