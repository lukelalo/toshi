#define N_IMPLEMENTS nConServer
//--------------------------------------------------------------------
//  nconserver.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "misc/nconserver.h"

nClass *nConServer::local_cl  = NULL;
nKernelServer *nConServer::ks = NULL;

extern void n_initcmds(nClass *);
extern char *nConServer_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nConServer::local_cl = cl;
    nConServer::ks       = ks;
    
    // mit Superklasse verketten
    nClass *super = ks->OpenClass("nroot");
    if (super) super->AddSubClass(nConServer::local_cl);
    else n_error("Could not open superclass 'nroot'!\n");
    
    // hier die Scriptkommandos definieren
    n_initcmds(cl);
    	
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nClass *super = nConServer::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nConServer::local_cl);
        nConServer::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nConServer;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nConServer_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
