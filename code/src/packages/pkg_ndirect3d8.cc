//----------------------------------------------------------
// pkg_ndirect3d8.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nd3d8server (nClass *, nKernelServer *);
extern "C" void n_fini_nd3d8server (void);
extern "C" void *n_new_nd3d8server (void);
extern "C" char *n_version_nd3d8server (void);
extern "C" bool n_init_nd3d8texture (nClass *, nKernelServer *);
extern "C" void n_fini_nd3d8texture (void);
extern "C" void *n_new_nd3d8texture (void);
extern "C" char *n_version_nd3d8texture (void);
extern "C" bool n_init_nd3d8indexbuffer (nClass *, nKernelServer *);
extern "C" void n_fini_nd3d8indexbuffer (void);
extern "C" void *n_new_nd3d8indexbuffer (void);
extern "C" char *n_version_nd3d8indexbuffer (void);
extern "C" bool n_init_nd3d8pixelshader (nClass *, nKernelServer *);
extern "C" void n_fini_nd3d8pixelshader (void);
extern "C" void *n_new_nd3d8pixelshader (void);
extern "C" char *n_version_nd3d8pixelshader (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT ndirect3d8_addmodules(nKernelServer *);
extern "C" void N_EXPORT ndirect3d8_remmodules(nKernelServer *);
void N_EXPORT ndirect3d8_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nd3d8server",n_init_nd3d8server,n_fini_nd3d8server,n_new_nd3d8server);
    ks->AddModule("nd3d8texture",n_init_nd3d8texture,n_fini_nd3d8texture,n_new_nd3d8texture);
    ks->AddModule("nd3d8indexbuffer",n_init_nd3d8indexbuffer,n_fini_nd3d8indexbuffer,n_new_nd3d8indexbuffer);
    ks->AddModule("nd3d8pixelshader",n_init_nd3d8pixelshader,n_fini_nd3d8pixelshader,n_new_nd3d8pixelshader);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT ndirect3d8_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
