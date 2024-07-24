//----------------------------------------------------------
// pkg_camera.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_ncamera (nClass *, nKernelServer *);
extern "C" void n_fini_ncamera (void);
extern "C" void *n_new_ncamera (void);
extern "C" char *n_version_ncamera (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT camera_addmodules(nKernelServer *);
extern "C" void N_EXPORT camera_remmodules(nKernelServer *);
void N_EXPORT camera_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("ncamera",n_init_ncamera,n_fini_ncamera,n_new_ncamera);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT camera_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
