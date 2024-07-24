//----------------------------------------------------------
// ndirect3d8_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT ndirect3d8_addmodules(nKernelServer *);
extern "C" void N_EXPORT ndirect3d8_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    ndirect3d8_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    ndirect3d8_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
