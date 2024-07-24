//----------------------------------------------------------
// pkg_npython.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_npythonserver (nClass *, nKernelServer *);
extern "C" void n_fini_npythonserver (void);
extern "C" void *n_new_npythonserver (void);
extern "C" char *n_version_npythonserver (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT npython_addmodules(nKernelServer *);
extern "C" void N_EXPORT npython_remmodules(nKernelServer *);
void N_EXPORT npython_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("npythonserver",n_init_npythonserver,n_fini_npythonserver,n_new_npythonserver);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT npython_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
