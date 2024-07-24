#define N_IMPLEMENTS nWorld
//------------------------------------------------------------------------------
//  (C) 2002	ling
//------------------------------------------------------------------------------
#include "laberintosultan/nworld.h"
#include "kernel/npersistserver.h"

static void n_setterrain(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nworld
    
    @superclass
    nroot

    @classinfo
    A detailed description of what the class does (written for script programmers!) 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setterrain_s", 'STRN', n_setterrain);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nWorld::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps)) {
        return true;
    }
    return false;
}


static
void
n_setterrain(void* slf, nCmd* cmd)
{
    nWorld* self = (nWorld*)slf;
    self->SetTerrain(cmd->In()->GetS());
}
