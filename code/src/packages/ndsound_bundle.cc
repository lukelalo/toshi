//----------------------------------------------------------
// ndsound_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT ndsound_addmodules(nKernelServer *);
extern "C" void N_EXPORT ndsound_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    ndsound_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    ndsound_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
