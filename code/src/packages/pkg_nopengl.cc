//----------------------------------------------------------
// pkg_nopengl.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nglserver (nClass *, nKernelServer *);
extern "C" void n_fini_nglserver (void);
extern "C" void *n_new_nglserver (void);
extern "C" char *n_version_nglserver (void);
extern "C" bool n_init_ngltexture (nClass *, nKernelServer *);
extern "C" void n_fini_ngltexture (void);
extern "C" void *n_new_ngltexture (void);
extern "C" char *n_version_ngltexture (void);
extern "C" bool n_init_nglpixelshader (nClass *, nKernelServer *);
extern "C" void n_fini_nglpixelshader (void);
extern "C" void *n_new_nglpixelshader (void);
extern "C" char *n_version_nglpixelshader (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT nopengl_addmodules(nKernelServer *);
extern "C" void N_EXPORT nopengl_remmodules(nKernelServer *);
void N_EXPORT nopengl_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nglserver",n_init_nglserver,n_fini_nglserver,n_new_nglserver);
    ks->AddModule("ngltexture",n_init_ngltexture,n_fini_ngltexture,n_new_ngltexture);
    ks->AddModule("nglpixelshader",n_init_nglpixelshader,n_fini_nglpixelshader,n_new_nglpixelshader);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT nopengl_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
