//----------------------------------------------------------
// nmap_bundle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/nkernelserver.h"

extern "C" void N_EXPORT nmap_addmodules(nKernelServer *);
extern "C" void N_EXPORT nmap_remmodules(nKernelServer *);

extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);

void N_EXPORT n_addmodules(nKernelServer *ks)
{
    nmap_addmodules(ks);
}

void N_EXPORT n_remmodules(nKernelServer *ks)
{
    nmap_remmodules(ks);
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
