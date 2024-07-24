//----------------------------------------------------------
// sprite_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT sprite_addmodules(nKernelServer *);
extern "C" void N_EXPORT sprite_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    sprite_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    sprite_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
