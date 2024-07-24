#define N_IMPLEMENTS nJoint
//--------------------------------------------------------------------
//  njoint.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "node/njoint.h"

nClass *nJoint::local_cl  = NULL;
nKernelServer *nJoint::ks = NULL;

extern void n_initcmds(nClass *);
extern char *nJoint_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nJoint::local_cl = cl;
    nJoint::ks       = ks;
    ks->AddClass("n3dnode",cl);
    n_initcmds(cl);
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nJoint::ks->RemClass(nJoint::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nJoint;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nJoint_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
