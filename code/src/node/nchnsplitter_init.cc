#define N_IMPLEMENTS nChnSplitter
//--------------------------------------------------------------------
//  nchnsplitter.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "node/nchnsplitter.h"

nClass *nChnSplitter::local_cl  = NULL;
nKernelServer *nChnSplitter::ks = NULL;

extern void n_initcmds(nClass *);
extern char *nChnSplitter_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nChnSplitter::local_cl = cl;
    nChnSplitter::ks       = ks;
    ks->AddClass("nanimnode", cl);
    n_initcmds(cl);
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nChnSplitter::ks->RemClass(nChnSplitter::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nChnSplitter;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nChnSplitter_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

