#define N_IMPLEMENTS nProfileServer
//------------------------------------------------------------------------------
//  nprofileserver_init.cc
//  (C) 2001 RadonLabs GmbH
//
//  NOTE
//  This stuff will sooner or later go into a macro, so don't use too much
//  time prettyfying it.
//------------------------------------------------------------------------------
#include "misc/nprofileserver.h"
#include "kernel/nkernelserver.h"

nClass* nProfileServer::clazz = 0;
nKernelServer* nProfileServer::kernelServer = 0;

extern void n_initcmds(nClass* clazz);
extern char* nProfileServer_version;

extern "C" bool N_EXPORT n_init(nClass* clazz, nKernelServer* kernelServer);
extern "C" void N_EXPORT n_fini();
extern "C" N_EXPORT void* n_create();
extern "C" N_EXPORT char* n_version();

//------------------------------------------------------------------------------
/**
*/
N_EXPORT
bool
n_init(nClass* clazz, nKernelServer* kernelServer)
{
    nProfileServer::clazz = clazz;
    nProfileServer::kernelServer = kernelServer;
    kernelServer->AddClass("nroot", clazz);
    n_initcmds(clazz);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
N_EXPORT
void
n_fini()
{
    nProfileServer::kernelServer->RemClass(nProfileServer::clazz);
}

//------------------------------------------------------------------------------
/**
*/
N_EXPORT
void*
n_create()
{
    return n_new nProfileServer();
}

//------------------------------------------------------------------------------
/**
*/
N_EXPORT
char*
n_version()
{
    return nProfileServer_version;
}
