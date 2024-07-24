#define N_IMPLEMENTS nOpcodeServer
//------------------------------------------------------------------------------
//  nopcodeserver_init.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "opcode/nopcodeserver.h"
#include "kernel/nkernelserver.h"

nClass* nOpcodeServer::clazz = 0;
nKernelServer* nOpcodeServer::kernelServer = 0;

extern char* nOpcodeServer_version;

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
    nOpcodeServer::clazz = clazz;
    nOpcodeServer::kernelServer = kernelServer;
    kernelServer->AddClass("ncollideserver", clazz);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
N_EXPORT
void
n_fini()
{
    nOpcodeServer::kernelServer->RemClass(nOpcodeServer::clazz);
}

//------------------------------------------------------------------------------
/**
*/
N_EXPORT
void*
n_create()
{
    return n_new nOpcodeServer();
}

//------------------------------------------------------------------------------
/**
*/
N_EXPORT
char*
n_version()
{
    return nOpcodeServer_version;
}

//------------------------------------------------------------------------------
