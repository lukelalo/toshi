#define N_IMPLEMENTS nSkyDome

// (C) 2000 Forged Reality

#include "nemesis/nskydome.h"
#include "kernel/nkernelserver.h"

nClass* nSkyDome::pClass = 0;
nKernelServer* nSkyDome::pKernelServer = 0;

extern void n_initcmds(nClass*);
extern char* nskydome_version;
extern "C" bool N_EXPORT n_init(nClass*, nKernelServer*);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void* n_create(void);
extern "C" N_EXPORT char* n_version(void);

N_EXPORT bool n_init(nClass* pClass, nKernelServer* pKernelServer)
{
    nSkyDome::pClass = pClass;
    nSkyDome::pKernelServer = pKernelServer;

    pKernelServer->AddClass("nvisnode", pClass);

    n_initcmds(pClass);
    return true;
}

N_EXPORT void n_fini()
{
    nSkyDome::pKernelServer->RemClass(nSkyDome::pClass);
}

N_EXPORT void* n_create()
{
    return new nSkyDome;
}

N_EXPORT char* n_version()
{
    return nskydome_version;
}
