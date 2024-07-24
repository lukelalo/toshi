//----------------------------------------------------------
// pkg_nMLoader.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nMDLRepository (nClass *, nKernelServer *);
extern "C" void n_fini_nMDLRepository (void);
extern "C" void *n_new_nMDLRepository (void);
extern "C" char *n_version_nMDLRepository (void);
extern "C" bool n_init_nMLoader (nClass *, nKernelServer *);
extern "C" void n_fini_nMLoader (void);
extern "C" void *n_new_nMLoader (void);
extern "C" char *n_version_nMLoader (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT nMLoader_addmodules(nKernelServer *);
extern "C" void N_EXPORT nMLoader_remmodules(nKernelServer *);
void N_EXPORT nMLoader_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nMDLRepository",n_init_nMDLRepository,n_fini_nMDLRepository,n_new_nMDLRepository);
    ks->AddModule("nMLoader",n_init_nMLoader,n_fini_nMLoader,n_new_nMLoader);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT nMLoader_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
