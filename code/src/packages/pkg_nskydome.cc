//----------------------------------------------------------
// pkg_nskydome.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nskydome (nClass *, nKernelServer *);
extern "C" void n_fini_nskydome (void);
extern "C" void *n_new_nskydome (void);
extern "C" char *n_version_nskydome (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT nskydome_addmodules(nKernelServer *);
extern "C" void N_EXPORT nskydome_remmodules(nKernelServer *);
void N_EXPORT nskydome_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nskydome",n_init_nskydome,n_fini_nskydome,n_new_nskydome);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT nskydome_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
