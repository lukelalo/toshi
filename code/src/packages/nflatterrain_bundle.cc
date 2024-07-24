//----------------------------------------------------------
// nflatterrain_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT nflatterrain_addmodules(nKernelServer *);
extern "C" void N_EXPORT nflatterrain_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    nflatterrain_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    nflatterrain_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
