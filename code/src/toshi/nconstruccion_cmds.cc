#define N_IMPLEMENTS nConstruccion
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nconstruccion.h"
#include "kernel/npersistserver.h"

static void n_setid(void *slf, nCmd *cmd);

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
	clazz->AddCmd("v_setid_i",'SID ',n_setid);
    clazz->EndCmds();
}


static void n_setid(void *slf, nCmd *cmd) {
	nConstruccion*self=(nConstruccion*)slf;
	self->SetId(cmd->In()->GetI());
}

bool
nConstruccion::SaveCmds(nPersistServer* ps)
{
	if (nEdificio::SaveCmds(ps))
    {
        //nCmd* cmd;
        return true;
    }
    return false;
}