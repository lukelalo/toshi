#define N_IMPLEMENTS nWorld
//------------------------------------------------------------------------------
//  (C) 2002	ling
//------------------------------------------------------------------------------
#include "toshi/nworld.h"
#include "kernel/npersistserver.h"

static void n_setmap(void* slf, nCmd* cmd);
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
    clazz->AddCmd("v_setmap_s", 'SMAP', n_setmap);
    clazz->AddCmd("v_setterrain_s", 'STRN', n_setterrain);
    clazz->EndCmds();
}

static
void
n_setmap(void* slf, nCmd* cmd)
{
    nWorld* self = (nWorld*)slf;
    self->SetMap(cmd->In()->GetS());
}

static
void
n_setterrain(void* slf, nCmd* cmd)
{
    nWorld* self = (nWorld*)slf;
    self->SetTerrain(cmd->In()->GetS());
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


