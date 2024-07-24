#define N_IMPLEMENTS nLenseFlare2

//==============================================================================
//  node/nLenseFlare2_init.cc
//  author: Jeremy Bishop
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

#include "node/nlenseflare2.h"
#include "kernel/nkernelserver.h"

nClass *nLenseFlare2::local_cl  = 0;
nKernelServer *nLenseFlare2::ks = 0;

extern void n_initcmds(nClass *);
extern char *nLenseFlare2_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
bool
N_EXPORT
n_init(nClass *cl, nKernelServer *ks)
{
    nLenseFlare2::local_cl = cl;
    nLenseFlare2::ks       = ks;
    ks->AddClass("nvisnode", cl);
    n_initcmds(cl);
    return true;
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
void
N_EXPORT
n_fini(void)
{
    nLenseFlare2::ks->RemClass(nLenseFlare2::local_cl);
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
void*
N_EXPORT
n_create(void)
{
    return new nLenseFlare2;
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
char*
N_EXPORT
n_version(void)
{
    return nLenseFlare2_version;
}
//------------------------------------------------------------------------------
