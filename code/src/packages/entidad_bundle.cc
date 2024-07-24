//----------------------------------------------------------
// entidad_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT entidad_addmodules(nKernelServer *);
extern "C" void N_EXPORT entidad_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    entidad_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    entidad_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
