//----------------------------------------------------------
// entity_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT entity_addmodules(nKernelServer *);
extern "C" void N_EXPORT entity_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    entity_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    entity_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------