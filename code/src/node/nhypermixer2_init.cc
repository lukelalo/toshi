#define N_IMPLEMENTS nHyperMixer2
//------------------------------------------------------------------------------
//  (C) 2001 Radon Labs GmbH

#include "node/nhypermixer2.h"
#include "kernel/nkernelserver.h"

nClass* nHyperMixer2::clazz = 0;
nKernelServer* nHyperMixer2::ks = 0;

extern void n_initcmds(nClass* clazz);
extern char* nHyperMixer2_version;

extern "C" bool N_EXPORT n_init(nClass* clazz, nKernelServer* ks);
extern "C" void N_EXPORT n_fini();
extern "C" N_EXPORT void* n_create();
extern "C" N_EXPORT char* n_version();

//------------------------------------------------------------------------------
/**
*/
N_EXPORT
bool
n_init(nClass* cl, nKernelServer* ks)
{
    nHyperMixer2::clazz = cl;
    nHyperMixer2::ks    = ks;
    ks->AddClass("nvisnode", cl);
    n_initcmds(cl);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
N_EXPORT
void
n_fini()
{
    nHyperMixer2::ks->RemClass(nHyperMixer2::clazz);
}

//------------------------------------------------------------------------------
/**
*/
N_EXPORT
void*
n_create()
{
    return new nHyperMixer2();
}

//------------------------------------------------------------------------------
/**
*/
N_EXPORT
char*
n_version()
{
    return nHyperMixer2_version;
}

//------------------------------------------------------------------------------
