//----------------------------------------------------------
// nnebula_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT nnebula_addmodules(nKernelServer *);
extern "C" void N_EXPORT nnebula_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    nnebula_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    nnebula_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
