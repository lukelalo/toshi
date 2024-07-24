//----------------------------------------------------------
// ndinput8_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT ndinput8_addmodules(nKernelServer *);
extern "C" void N_EXPORT ndinput8_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    ndinput8_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    ndinput8_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
