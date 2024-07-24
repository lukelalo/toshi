//----------------------------------------------------------
// pkg_mundo.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nworld (nClass *, nKernelServer *);
extern "C" void n_fini_nworld (void);
extern "C" void *n_new_nworld (void);
extern "C" char *n_version_nworld (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT mundo_addmodules(nKernelServer *);
extern "C" void N_EXPORT mundo_remmodules(nKernelServer *);
void N_EXPORT mundo_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nworld",n_init_nworld,n_fini_nworld,n_new_nworld);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT mundo_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
