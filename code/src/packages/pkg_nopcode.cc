//----------------------------------------------------------
// pkg_nopcode.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nopcodeserver (nClass *, nKernelServer *);
extern "C" void n_fini_nopcodeserver (void);
extern "C" void *n_new_nopcodeserver (void);
extern "C" char *n_version_nopcodeserver (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT nopcode_addmodules(nKernelServer *);
extern "C" void N_EXPORT nopcode_remmodules(nKernelServer *);
void N_EXPORT nopcode_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nopcodeserver",n_init_nopcodeserver,n_fini_nopcodeserver,n_new_nopcodeserver);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT nopcode_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
