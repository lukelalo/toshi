//----------------------------------------------------------
// pkg_nenet.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nenethost (nClass *, nKernelServer *);
extern "C" void n_fini_nenethost (void);
extern "C" void *n_new_nenethost (void);
extern "C" char *n_version_nenethost (void);
extern "C" bool n_init_nenetpeer (nClass *, nKernelServer *);
extern "C" void n_fini_nenetpeer (void);
extern "C" void *n_new_nenetpeer (void);
extern "C" char *n_version_nenetpeer (void);
extern "C" bool n_init_nenetserver (nClass *, nKernelServer *);
extern "C" void n_fini_nenetserver (void);
extern "C" void *n_new_nenetserver (void);
extern "C" char *n_version_nenetserver (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT nenet_addmodules(nKernelServer *);
extern "C" void N_EXPORT nenet_remmodules(nKernelServer *);
void N_EXPORT nenet_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nenethost",n_init_nenethost,n_fini_nenethost,n_new_nenethost);
    ks->AddModule("nenetpeer",n_init_nenetpeer,n_fini_nenetpeer,n_new_nenetpeer);
    ks->AddModule("nenetserver",n_init_nenetserver,n_fini_nenetserver,n_new_nenetserver);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT nenet_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
