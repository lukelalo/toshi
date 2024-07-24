//----------------------------------------------------------
// pkg_ia.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nia (nClass *, nKernelServer *);
extern "C" void n_fini_nia (void);
extern "C" void *n_new_nia (void);
extern "C" char *n_version_nia (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT ia_addmodules(nKernelServer *);
extern "C" void N_EXPORT ia_remmodules(nKernelServer *);
void N_EXPORT ia_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nia",n_init_nia,n_fini_nia,n_new_nia);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT ia_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
