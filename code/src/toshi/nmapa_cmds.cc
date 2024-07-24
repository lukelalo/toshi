#define N_IMPLEMENTS nMapa
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nMapa.h"
#include "kernel/npersistserver.h"

static void n_setmap(void* slf, nCmd* cmd);
static void n_getmap(void* slf, nCmd* cmd);
static void n_mostrardebug(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nMapa
    
    @superclass
    nroot

    @classinfo
    A detailed description of what the class does (written for script programmers!) 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setmap_i", 'SMAP', n_setmap);
	clazz->AddCmd("s_getmap_v", 'GMAP', n_getmap);
	clazz->AddCmd("v_mostrardebug_b", 'MDBG', n_mostrardebug);
    clazz->EndCmds();
}

static
void
n_setmap(void* slf, nCmd* cmd)
{
    nMapa* self = (nMapa*) slf;
	int n_jug=cmd->In()->GetI();
    self->SetMap(n_jug);
}

static
void
n_getmap(void* slf, nCmd* cmd)
{
    nMapa* self = (nMapa*) slf;
    cmd->Out()->SetS(self->GetMap());
}

static
void
n_mostrardebug(void* slf, nCmd* cmd)
{
    nMapa* self = (nMapa*) slf;
	self->PintarDebug(cmd->In()->GetB());
}

bool
nMapa::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        //nCmd* cmd;
        return true;
    }
    return false;
}
