#define N_IMPLEMENTS nObserver
//-------------------------------------------------------------------
//  nobserver.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "misc/nobserver.h"

nClass *nObserver::local_cl  = NULL;
nKernelServer *nObserver::ks = NULL;

extern void n_initcmds(nClass *);
extern char *nObserver_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nObserver::local_cl = cl;
    nObserver::ks       = ks;
    
    // mit Superklasse verketten
    nClass *super = ks->OpenClass("nroot");
    if (super) super->AddSubClass(nObserver::local_cl);
    else n_error("Could not open superclass 'nroot'.\n");
    
    n_initcmds(cl);
	
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    // von Superklasse abkoppeln
    nClass *super = nObserver::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nObserver::local_cl);
        nObserver::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nObserver;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nObserver_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
