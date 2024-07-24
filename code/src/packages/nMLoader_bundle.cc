//----------------------------------------------------------
// nMLoader_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT nMLoader_addmodules(nKernelServer *);
extern "C" void N_EXPORT nMLoader_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    nMLoader_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    nMLoader_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
