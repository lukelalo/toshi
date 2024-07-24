//----------------------------------------------------------
// pkg_ndinput8.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_ndi8server (nClass *, nKernelServer *);
extern "C" void n_fini_ndi8server (void);
extern "C" void *n_new_ndi8server (void);
extern "C" char *n_version_ndi8server (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT ndinput8_addmodules(nKernelServer *);
extern "C" void N_EXPORT ndinput8_remmodules(nKernelServer *);
void N_EXPORT ndinput8_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("ndi8server",n_init_ndi8server,n_fini_ndi8server,n_new_ndi8server);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT ndinput8_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
