#define N_IMPLEMENTS nChnReader
//------------------------------------------------------------------------------
//  nchnreader_init.cc
//  (C) 2001 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "node/nchnreader.h"
#include "kernel/nkernelserver.h"

nClass* nChnReader::clazz = 0;
nKernelServer* nChnReader::kernelServer = 0;

extern void n_initcmds(nClass* clazz);
extern char* nChnReader_version;

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
    nChnReader::clazz = clazz;
    nChnReader::kernelServer = kernelServer;
    kernelServer->AddClass("nvisnode", clazz);
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
    nChnReader::kernelServer->RemClass(nChnReader::clazz);
}

//------------------------------------------------------------------------------
/**
*/
N_EXPORT
void*
n_create()
{
    return new nChnReader();
}

//------------------------------------------------------------------------------
/**
*/
N_EXPORT
char*
n_version()
{
    return nChnReader_version;
}

//------------------------------------------------------------------------------
