//----------------------------------------------------------
// npython_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT npython_addmodules(nKernelServer *);
extern "C" void N_EXPORT npython_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    npython_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    npython_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
