#define N_IMPLEMENTS nPrimitiveServer
//------------------------------------------------------------------------------
//  (C) 2003  Leaf Garland & Vadim Macagon
//
//  nPrimitiveServer is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "gfx/nprimitiveserver.h"
#include "kernel/npersistserver.h"


//------------------------------------------------------------------------------
/**
    @scriptclass
    nprimitiveserver
    
    @superclass
    nroot

    @classinfo
    A detailed description of what the class does (written for script programmers!) 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nPrimitiveServer::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

