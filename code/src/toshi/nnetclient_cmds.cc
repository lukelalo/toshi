

#define N_IMPLEMENTS nNetClient

#include "toshi/nnetclient.h"
#include "kernel/npersistserver.h"


static void n_setpeer(void *slf, nCmd *cmd);
static void n_getpeer(void *slf, nCmd *cmd);
static void n_setchannel(void *slf, nCmd *cmd);
static void n_getchannel(void *slf, nCmd *cmd);

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
	clazz->AddCmd("v_setpeer_s", 'SPRP', n_setpeer);
	clazz->AddCmd("s_getpeer_v", 'GPRP', n_getpeer);
	clazz->AddCmd("v_setchannel_i", 'SCHP', n_setchannel);
	clazz->AddCmd("i_getchannel_v", 'GCHP', n_getchannel);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------


static void n_setpeer(void *slf, nCmd *cmd) {
	nNetClient *self = (nNetClient*)slf;
	self->SetPeer(cmd->In()->GetS());
}

static void n_getpeer(void *slf, nCmd *cmd) {
	nNetClient *self = (nNetClient*)slf;
	cmd->Out()->SetS(self->GetPeer());
}

//------------------------------------------------------------------------------


static void n_setchannel(void *slf, nCmd *cmd) {
	nNetClient *self = (nNetClient*)slf;
	self->SetChannel(cmd->In()->GetI());
}

static void n_getchannel(void *slf, nCmd *cmd) {
	nNetClient *self = (nNetClient*)slf;
	cmd->Out()->SetI(self->GetChannel());
}

//------------------------------------------------------------------------------
/**
    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/

bool
nNetClient::SaveCmds(nPersistServer* ps)
{
	
	if (!nRoot::SaveCmds(ps))
		return false;

	return true;
}

