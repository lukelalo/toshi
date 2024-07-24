//----------------------------------------------------------
// pkg_sprite.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nCSprite (nClass *, nKernelServer *);
extern "C" void n_fini_nCSprite (void);
extern "C" void *n_new_nCSprite (void);
extern "C" char *n_version_nCSprite (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT sprite_addmodules(nKernelServer *);
extern "C" void N_EXPORT sprite_remmodules(nKernelServer *);
void N_EXPORT sprite_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nCSprite",n_init_nCSprite,n_fini_nCSprite,n_new_nCSprite);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT sprite_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
