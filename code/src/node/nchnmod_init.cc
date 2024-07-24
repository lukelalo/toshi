#define N_IMPLEMENTS nChnModulator
//--------------------------------------------------------------------
//  nchnmodulator.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "node/nchnmodulator.h"

nClass *nChnModulator::local_cl  = NULL;
nKernelServer *nChnModulator::ks = NULL;

extern void n_initcmds(nClass *);
extern char *nChnModulator_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nChnModulator::local_cl = cl;
    nChnModulator::ks       = ks;
    ks->AddClass("nvisnode", cl);
    n_initcmds(cl);
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nChnModulator::ks->RemClass(nChnModulator::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nChnModulator;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nChnModulator_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

