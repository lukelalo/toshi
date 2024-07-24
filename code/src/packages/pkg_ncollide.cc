//----------------------------------------------------------
// pkg_ncollide.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_ncollideserver (nClass *, nKernelServer *);
extern "C" void n_fini_ncollideserver (void);
extern "C" void *n_new_ncollideserver (void);
extern "C" char *n_version_ncollideserver (void);
extern "C" bool n_init_ncollidenode (nClass *, nKernelServer *);
extern "C" void n_fini_ncollidenode (void);
extern "C" void *n_new_ncollidenode (void);
extern "C" char *n_version_ncollidenode (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT ncollide_addmodules(nKernelServer *);
extern "C" void N_EXPORT ncollide_remmodules(nKernelServer *);
void N_EXPORT ncollide_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("ncollideserver",n_init_ncollideserver,n_fini_ncollideserver,n_new_ncollideserver);
    ks->AddModule("ncollidenode",n_init_ncollidenode,n_fini_ncollidenode,n_new_ncollidenode);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT ncollide_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
