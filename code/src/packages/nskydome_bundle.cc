//----------------------------------------------------------
// nskydome_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT nskydome_addmodules(nKernelServer *);
extern "C" void N_EXPORT nskydome_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    nskydome_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    nskydome_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
