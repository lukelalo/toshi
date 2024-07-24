//----------------------------------------------------------
// camera_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT camera_addmodules(nKernelServer *);
extern "C" void N_EXPORT camera_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    camera_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    camera_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
