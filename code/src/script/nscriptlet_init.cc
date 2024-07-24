#define N_IMPLEMENTS nScriptlet

//------------------------------------------------------------------------------
//  © 2001 Radon Labs GmbH

#include "script/nscriptlet.h"
#include "kernel/nkernelserver.h"

///. used for identifying the runtime type of this object.
nClass* nScriptlet::clazz = 0;
///. used to link/unlink this class with nKernelServer instance.
nKernelServer* nScriptlet::kernelServer = 0;

/// nKernelServer callback to register commands and its 4CC command id.
extern void n_initcmds(nClass* clazz);
/// version string for the associated class.
extern char* nScriptlet_version;

/// nKernelServer initialization callback.
extern "C" bool N_EXPORT n_init(nClass* clazz, nKernelServer* kernelServer);
/// nKernelServer cleanup callback.
extern "C" void N_EXPORT n_fini();
/// nKernelServer creation callback.
extern "C" N_EXPORT void* n_create();
/// nKernelServer version callback.
extern "C" N_EXPORT char* n_version();

//------------------------------------------------------------------------------
/**
    links associated class to nKernelServer,  initalizes the associated class'
    dependency to the nKernelServer and its nClass.

    @param clazz            refers to the associated class' type.
    @param kernelServer     refers to the associated class' nKernelServer
                            dependency.
*/
N_EXPORT
bool
n_init(nClass* clazz, nKernelServer* kernelServer)
{
    nScriptlet::clazz = clazz;
    nScriptlet::kernelServer = kernelServer;
    kernelServer->AddClass("nroot", clazz);
    n_initcmds(clazz);
    return true;
}

//------------------------------------------------------------------------------
/**
    unlinks associated class from nKernelServer.
*/
N_EXPORT
void
n_fini()
{
    nScriptlet::kernelServer->RemClass(nScriptlet::clazz);
}

//------------------------------------------------------------------------------
/**
    allows the nKernelServer to create instances of the associated class.
*/
N_EXPORT
void*
n_create()
{
    return new nScriptlet();
}

//------------------------------------------------------------------------------
/**
    allows the nKernelServer to access the version information for the
    associated class.
*/
N_EXPORT
char*
n_version()
{
    return nScriptlet_version;
}
