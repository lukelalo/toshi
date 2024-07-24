//----------------------------------------------------------
// pkg_ndsound.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_ndsoundserver2 (nClass *, nKernelServer *);
extern "C" void n_fini_ndsoundserver2 (void);
extern "C" void *n_new_ndsoundserver2 (void);
extern "C" char *n_version_ndsoundserver2 (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT ndsound_addmodules(nKernelServer *);
extern "C" void N_EXPORT ndsound_remmodules(nKernelServer *);
void N_EXPORT ndsound_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("ndsoundserver2",n_init_ndsoundserver2,n_fini_ndsoundserver2,n_new_ndsoundserver2);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT ndsound_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
