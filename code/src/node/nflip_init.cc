#define N_IMPLEMENTS nFlipFlop
//--------------------------------------------------------------------
//  IMPLEMENTATION
//      nFlipFlop -- 1-aus-N-Selektor
//
//  (C) 1998 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "node/nflipflop.h"

nClass *nFlipFlop::local_cl  = NULL;
nKernelServer *nFlipFlop::ks = NULL;

extern void n_initcmds(nClass *);
extern char *nFlipFlop_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nFlipFlop::local_cl = cl;
    nFlipFlop::ks       = ks;
    
    // mit Superklasse verketten
    nClass *super = ks->OpenClass("nanimnode");
    if (super) super->AddSubClass(nFlipFlop::local_cl);
    else n_error("could not open superclass nanimnode!");
    n_initcmds(cl);
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    // von Superklasse abkoppeln
    nClass *super = nFlipFlop::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nFlipFlop::local_cl);
        nFlipFlop::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nFlipFlop;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nFlipFlop_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

