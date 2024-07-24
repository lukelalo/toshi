//----------------------------------------------------------
// pkg_nparticle.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nparticleserver (nClass *, nKernelServer *);
extern "C" void n_fini_nparticleserver (void);
extern "C" void *n_new_nparticleserver (void);
extern "C" char *n_version_nparticleserver (void);
extern "C" bool n_init_npemitter (nClass *, nKernelServer *);
extern "C" void n_fini_npemitter (void);
extern "C" void *n_new_npemitter (void);
extern "C" char *n_version_npemitter (void);
extern "C" bool n_init_npservemitter (nClass *, nKernelServer *);
extern "C" void n_fini_npservemitter (void);
extern "C" void *n_new_npservemitter (void);
extern "C" char *n_version_npservemitter (void);
extern "C" bool n_init_npointemitter (nClass *, nKernelServer *);
extern "C" void n_fini_npointemitter (void);
extern "C" void *n_new_npointemitter (void);
extern "C" char *n_version_npointemitter (void);
extern "C" bool n_init_nareaemitter (nClass *, nKernelServer *);
extern "C" void n_fini_nareaemitter (void);
extern "C" void *n_new_nareaemitter (void);
extern "C" char *n_version_nareaemitter (void);
extern "C" bool n_init_nmeshemitter (nClass *, nKernelServer *);
extern "C" void n_fini_nmeshemitter (void);
extern "C" void *n_new_nmeshemitter (void);
extern "C" char *n_version_nmeshemitter (void);
extern "C" bool n_init_nstaticmeshemitter (nClass *, nKernelServer *);
extern "C" void n_fini_nstaticmeshemitter (void);
extern "C" void *n_new_nstaticmeshemitter (void);
extern "C" char *n_version_nstaticmeshemitter (void);
extern "C" bool n_init_nprender (nClass *, nKernelServer *);
extern "C" void n_fini_nprender (void);
extern "C" void *n_new_nprender (void);
extern "C" char *n_version_nprender (void);
extern "C" bool n_init_nspriterender (nClass *, nKernelServer *);
extern "C" void n_fini_nspriterender (void);
extern "C" void *n_new_nspriterender (void);
extern "C" char *n_version_nspriterender (void);
extern "C" bool n_init_ntrailrender (nClass *, nKernelServer *);
extern "C" void n_fini_ntrailrender (void);
extern "C" void *n_new_ntrailrender (void);
extern "C" char *n_version_ntrailrender (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT nparticle_addmodules(nKernelServer *);
extern "C" void N_EXPORT nparticle_remmodules(nKernelServer *);
void N_EXPORT nparticle_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nparticleserver",n_init_nparticleserver,n_fini_nparticleserver,n_new_nparticleserver);
    ks->AddModule("npemitter",n_init_npemitter,n_fini_npemitter,n_new_npemitter);
    ks->AddModule("npservemitter",n_init_npservemitter,n_fini_npservemitter,n_new_npservemitter);
    ks->AddModule("npointemitter",n_init_npointemitter,n_fini_npointemitter,n_new_npointemitter);
    ks->AddModule("nareaemitter",n_init_nareaemitter,n_fini_nareaemitter,n_new_nareaemitter);
    ks->AddModule("nmeshemitter",n_init_nmeshemitter,n_fini_nmeshemitter,n_new_nmeshemitter);
    ks->AddModule("nstaticmeshemitter",n_init_nstaticmeshemitter,n_fini_nstaticmeshemitter,n_new_nstaticmeshemitter);
    ks->AddModule("nprender",n_init_nprender,n_fini_nprender,n_new_nprender);
    ks->AddModule("nspriterender",n_init_nspriterender,n_fini_nspriterender,n_new_nspriterender);
    ks->AddModule("ntrailrender",n_init_ntrailrender,n_fini_ntrailrender,n_new_ntrailrender);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT nparticle_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
