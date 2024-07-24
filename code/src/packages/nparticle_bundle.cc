//----------------------------------------------------------
// nparticle_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT nparticle_addmodules(nKernelServer *);
extern "C" void N_EXPORT nparticle_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    nparticle_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    nparticle_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
