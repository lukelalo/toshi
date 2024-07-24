//----------------------------------------------------------
// pkg_nText.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nLabel (nClass *, nKernelServer *);
extern "C" void n_fini_nLabel (void);
extern "C" void *n_new_nLabel (void);
extern "C" char *n_version_nLabel (void);
extern "C" bool n_init_nFontServer (nClass *, nKernelServer *);
extern "C" void n_fini_nFontServer (void);
extern "C" void *n_new_nFontServer (void);
extern "C" char *n_version_nFontServer (void);
extern "C" bool n_init_nFont (nClass *, nKernelServer *);
extern "C" void n_fini_nFont (void);
extern "C" void *n_new_nFont (void);
extern "C" char *n_version_nFont (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT nText_addmodules(nKernelServer *);
extern "C" void N_EXPORT nText_remmodules(nKernelServer *);
void N_EXPORT nText_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nLabel",n_init_nLabel,n_fini_nLabel,n_new_nLabel);
    ks->AddModule("nFontServer",n_init_nFontServer,n_fini_nFontServer,n_new_nFontServer);
    ks->AddModule("nFont",n_init_nFont,n_fini_nFont,n_new_nFont);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT nText_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
