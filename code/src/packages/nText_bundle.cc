//----------------------------------------------------------
// nText_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT nText_addmodules(nKernelServer *);
extern "C" void N_EXPORT nText_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    nText_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    nText_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
