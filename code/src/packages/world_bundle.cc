//----------------------------------------------------------
// world_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT world_addmodules(nKernelServer *);
extern "C" void N_EXPORT world_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    world_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    world_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
