//----------------------------------------------------------
// ia_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT ia_addmodules(nKernelServer *);
extern "C" void N_EXPORT ia_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    ia_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    ia_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
