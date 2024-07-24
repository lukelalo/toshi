#define N_IMPLEMENTS nOctree
//--------------------------------------------------------------------
//  IMPLEMENTATION
//  noctree.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "misc/noctree.h"

nClass *nOctree::local_cl  = NULL;
nKernelServer *nOctree::ks = NULL;

extern void n_initcmds(nClass *);
extern char *nOctree_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nOctree::local_cl = cl;
    nOctree::ks       = ks;

    // mit Superklasse verketten
    nClass *super = ks->OpenClass("nroot");
    if (super) super->AddSubClass(nOctree::local_cl);
    else n_error("Could not open superclass 'nroot'\n");

    n_initcmds(cl);

    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    // von Superklasse abkoppeln
    nClass *super = nOctree::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nOctree::local_cl);
        nOctree::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nOctree;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nOctree_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

