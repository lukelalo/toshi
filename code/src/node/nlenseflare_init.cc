#define N_IMPLEMENTS nLenseFlare

//==============================================================================
//  node/nlenseflare_init.cc
//  author: Jeremy Bishop
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

#include "node/nlenseflare.h"
#include "kernel/nkernelserver.h"

nClass *nLenseFlare::local_cl  = 0;
nKernelServer *nLenseFlare::ks = 0;

extern void n_initcmds(nClass *);
extern char *nLenseFlare_version;
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
    nLenseFlare::local_cl = cl;
    nLenseFlare::ks       = ks;
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
    nLenseFlare::ks->RemClass(nLenseFlare::local_cl);
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
N_EXPORT
void* n_create(void)
{
    return new nLenseFlare;
}
//==============================================================================
//  yyyy.mm.dd  init    change remarks
//------------------------------------------------------------------------------
N_EXPORT
char* n_version(void)
{
    return nLenseFlare_version;
}
//------------------------------------------------------------------------------
