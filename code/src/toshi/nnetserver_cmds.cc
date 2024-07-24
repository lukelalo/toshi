#define N_IMPLEMENTS nNetServer

#include "toshi/nnetServer.h"
#include "kernel/npersistserver.h"


static void n_sethost(void *slf, nCmd *cmd);
static void n_gethost(void *slf, nCmd *cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nnet
    
    @superclass
    nroot

    @classinfo
    A detailed description of what the class does (written for script programmers!) 
*/

void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
	clazz->AddCmd("v_sethost_s", 'SHTP', n_sethost);
	clazz->AddCmd("s_gethost_v", 'GHTP', n_gethost);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------


static void n_sethost(void *slf, nCmd *cmd) {
	nNetServer *self = (nNetServer*)slf;
	self->SetHost(cmd->In()->GetS());
}

static void n_gethost(void *slf, nCmd *cmd) {
	nNetServer *self = (nNetServer*)slf;
	cmd->Out()->SetS(self->GetHost());
}

//------------------------------------------------------------------------------
/**
    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/

bool
nNetServer::SaveCmds(nPersistServer* ps)
{
	
	if (!nRoot::SaveCmds(ps))
		return false;

	return true;
}
