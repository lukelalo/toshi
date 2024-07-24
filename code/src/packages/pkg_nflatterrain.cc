//----------------------------------------------------------
// pkg_nflatterrain.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nflatterrainnode (nClass *, nKernelServer *);
extern "C" void n_fini_nflatterrainnode (void);
extern "C" void *n_new_nflatterrainnode (void);
extern "C" char *n_version_nflatterrainnode (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT nflatterrain_addmodules(nKernelServer *);
extern "C" void N_EXPORT nflatterrain_remmodules(nKernelServer *);
void N_EXPORT nflatterrain_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nflatterrainnode",n_init_nflatterrainnode,n_fini_nflatterrainnode,n_new_nflatterrainnode);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT nflatterrain_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
