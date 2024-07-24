//----------------------------------------------------------
// ncollide_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT ncollide_addmodules(nKernelServer *);
extern "C" void N_EXPORT ncollide_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    ncollide_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    ncollide_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
