//----------------------------------------------------------
// juego_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT juego_addmodules(nKernelServer *);
extern "C" void N_EXPORT juego_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    juego_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    juego_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
