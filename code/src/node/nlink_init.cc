#define N_IMPLEMENTS nLinkNode
//--------------------------------------------------------------------
//  nlinknode.cc
//  (C) 1999 Andre Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "node/nlinknode.h"

nClass *nLinkNode::local_cl  = NULL;
nKernelServer *nLinkNode::ks = NULL;

extern void n_initcmds(nClass *);
extern char *nLinkNode_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nLinkNode::local_cl = cl;
    nLinkNode::ks       = ks;
    
    // mit Superklasse verketten
    nClass *super = ks->OpenClass("nvisnode");
    if (super) super->AddSubClass(nLinkNode::local_cl);
    else n_error("Could not open superclass 'nvisnode'.\n");
    n_initcmds(cl);
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nClass *super = nLinkNode::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nLinkNode::local_cl);
        nLinkNode::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nLinkNode;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nLinkNode_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
