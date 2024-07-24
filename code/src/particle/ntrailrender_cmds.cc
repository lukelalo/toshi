#define N_IMPLEMENTS nTrailRender

//------------------------------------------------------------------------------
//  © 2001 Radon Labs GmbH

#include "particle/ntrailrender.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    ntrailrender

    @superclass
    nprender

    @classinfo
    Render a trail of connected particles.
*/

//------------------------------------------------------------------------------
/**
    @param clazz    used to associate the registered commands with the class.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    fills an nCmd object with the appropriate persistent data and passes the
    nCmd object on to the nPersistServer for output to a file.

    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nTrailRender::SaveCmds(nPersistServer* fileServer)
{
    if (nPRender::SaveCmds(fileServer))
    {
        return true;
    }
    else
    {
        return false;
    }
}
