//----------------------------------------------------------
// nopcode_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT nopcode_addmodules(nKernelServer *);
extern "C" void N_EXPORT nopcode_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    nopcode_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    nopcode_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
