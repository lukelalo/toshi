//----------------------------------------------------------
// pkg_nmap.cc
// MACHINE GENERATED, DON'T EDIT!
//----------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
extern "C" bool n_init_nmap (nClass *, nKernelServer *);
extern "C" void n_fini_nmap (void);
extern "C" void *n_new_nmap (void);
extern "C" char *n_version_nmap (void);
extern "C" bool n_init_nmapnode (nClass *, nKernelServer *);
extern "C" void n_fini_nmapnode (void);
extern "C" void *n_new_nmapnode (void);
extern "C" char *n_version_nmapnode (void);
#ifndef __MACOSX__
extern "C" void N_EXPORT n_addmodules(nKernelServer *);
extern "C" void N_EXPORT n_remmodules(nKernelServer *);
void N_EXPORT n_addmodules(nKernelServer *ks)
#else
extern "C" void N_EXPORT nmap_addmodules(nKernelServer *);
extern "C" void N_EXPORT nmap_remmodules(nKernelServer *);
void N_EXPORT nmap_addmodules(nKernelServer *ks)
#endif
{
    ks->AddModule("nmap",n_init_nmap,n_fini_nmap,n_new_nmap);
    ks->AddModule("nmapnode",n_init_nmapnode,n_fini_nmapnode,n_new_nmapnode);
}
#ifndef __MACOSX__
void N_EXPORT n_remmodules(nKernelServer *)
#else
void N_EXPORT nmap_remmodules(nKernelServer *)
#endif
{
}
//----------------------------------------------------------
// EOF
//----------------------------------------------------------
