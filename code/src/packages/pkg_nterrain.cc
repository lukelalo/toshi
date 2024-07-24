//----------------------------------------------------------
// pkg_nterrain.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nterrainnode (nClass *, nKernelServer *);
extern "C" void n_fini_nterrainnode (void);
extern "C" void *n_new_nterrainnode (void);
extern "C" char *n_version_nterrainnode (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT nterrain_addmodules(nKernelServer *);
extern "C" void N_EXPORT nterrain_remmodules(nKernelServer *);
void N_EXPORT nterrain_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nterrainnode",n_init_nterrainnode,n_fini_nterrainnode,n_new_nterrainnode);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT nterrain_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
