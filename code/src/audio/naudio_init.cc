#define N_IMPLEMENTS nAudioServer
//--------------------------------------------------------------------
//  naudio_init.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "audio/naudioserver.h"

nClass *nAudioServer::local_cl  = NULL;
nKernelServer *nAudioServer::ks = NULL;

extern char *nAudioServer_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);
extern void n_initcmds(nClass *);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nAudioServer::local_cl = cl;
    nAudioServer::ks       = ks;
    ks->AddClass("nroot",cl);
    n_initcmds(cl);    
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nAudioServer::ks->RemClass(nAudioServer::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nAudioServer;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nAudioServer_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
