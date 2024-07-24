#define N_IMPLEMENTS nSound
//--------------------------------------------------------------------
//  nsound_init.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "audio/nsound.h"

nClass *nSound::local_cl  = NULL;
nKernelServer *nSound::ks = NULL;

extern char *nSound_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nSound::local_cl = cl;
    nSound::ks       = ks;
    ks->AddClass("nroot",cl);
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nSound::ks->RemClass(nSound::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nSound;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nSound_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
