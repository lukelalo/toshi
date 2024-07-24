#define N_IMPLEMENTS nJointAnim
//--------------------------------------------------------------------
//  njointanim.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "node/njointanim.h"

nClass *nJointAnim::local_cl  = NULL;
nKernelServer *nJointAnim::ks = NULL;

extern void n_initcmds(nClass *);
extern char *nJointAnim_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nJointAnim::local_cl = cl;
    nJointAnim::ks       = ks;
    ks->AddClass("nanimnode", cl);
    n_initcmds(cl);
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nJointAnim::ks->RemClass(nJointAnim::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nJointAnim;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nJointAnim_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

