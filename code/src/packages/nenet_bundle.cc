//----------------------------------------------------------
// nenet_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT nenet_addmodules(nKernelServer *);
extern "C" void N_EXPORT nenet_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    nenet_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    nenet_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
