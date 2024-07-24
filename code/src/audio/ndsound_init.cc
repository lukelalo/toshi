#define N_IMPLEMENTS nDSoundServer
//--------------------------------------------------------------------
//  ndsound_init.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "audio/ndsoundserver.h"

nClass *nDSoundServer::local_cl  = NULL;
nKernelServer *nDSoundServer::ks = NULL;

extern char *nDSoundServer_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nDSoundServer::local_cl = cl;
    nDSoundServer::ks       = ks;
    ks->AddClass("naudioserver",cl);
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nDSoundServer::ks->RemClass(nDSoundServer::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nDSoundServer;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nDSoundServer_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
