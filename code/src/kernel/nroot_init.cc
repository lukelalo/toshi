#define N_IMPLEMENTS nRoot
#define N_KERNEL
//--------------------------------------------------------------------
//  IMPLEMENTATION
//      nRoot
//
//  (C) 1998 by Andre Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nroot.h"

//--------------------------------------------------------------------
// static nRoot Members 
//--------------------------------------------------------------------
nClass *nRoot::local_cl = NULL;
nKernelServer *nRoot::ks = NULL;

extern void nroot_initcmds(nClass *);
extern char *nkernel_version;
extern "C" bool root_init(nClass *, nKernelServer *);
extern "C" void root_fini(void);
extern "C" void *root_new(void);
extern "C" char *root_version(void);

//--------------------------------------------------------------------
//  root_init()
//  Class Construktor
//--------------------------------------------------------------------
bool  root_init(nClass *cl, nKernelServer *ks)
{
    nRoot::local_cl = cl;
    nRoot::ks       = ks;
    
    // nRoot Klasse hat keine Superklasse
    
    // Scriptinterface definieren    
    nroot_initcmds(cl);

    return TRUE;
}

//--------------------------------------------------------------------
//  root_fini()
//  Class Destruktor
//--------------------------------------------------------------------
void root_fini(void)
{
    // nicht notwendig bei Root-Klasse: von Superclass abkoppeln
}

//--------------------------------------------------------------------
//  root_new()
//  Instantiate Object
//--------------------------------------------------------------------
void *root_new(void)
{
    return n_new nRoot;
}

//--------------------------------------------------------------------
//  root_version()
//  Instantiate Object
//--------------------------------------------------------------------
char *root_version(void)
{
    return nkernel_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
