//----------------------------------------------------------
// nterrain_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT nterrain_addmodules(nKernelServer *);
extern "C" void N_EXPORT nterrain_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    nterrain_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    nterrain_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
