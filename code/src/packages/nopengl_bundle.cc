//----------------------------------------------------------
// nopengl_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT nopengl_addmodules(nKernelServer *);
extern "C" void N_EXPORT nopengl_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    nopengl_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    nopengl_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
