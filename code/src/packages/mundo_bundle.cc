//----------------------------------------------------------
// mundo_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT mundo_addmodules(nKernelServer *);
extern "C" void N_EXPORT mundo_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    mundo_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    mundo_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
