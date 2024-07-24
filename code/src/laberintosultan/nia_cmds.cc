#define N_IMPLEMENTS nIA

#include "laberintosultan/nia.h"
#include "kernel/npersistserver.h"

static void n_getiascript(void *slf, nCmd *cmd);
static void n_setiascript(void *slf, nCmd *cmd);

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
    clazz->AddCmd("v_setiascript_s", 'SIAS', n_setiascript);
	clazz->AddCmd("s_getiascript_v", 'GIAS', n_getiascript);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setiascript

    @input
    s

    @output
    v

    @info
    Establece el script a ejecutar en cada ciclo de IA.
*/
static void n_setiascript(void *slf, nCmd *cmd) {
	nIA *self = (nIA*)slf;
	self->SetIAScript(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getiascript

    @input
    v

    @output
    s

    @info
    Devuelve el script de IA que se ejecuta.
*/
static void n_getiascript(void *slf, nCmd *cmd) {
	nIA *self = (nIA*)slf;
	cmd->Out()->SetS(self->GetIAScript());
}

//------------------------------------------------------------------------------
/**
    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nIA::SaveCmds(nPersistServer* ps)
{
	
	if (!nRoot::SaveCmds(ps))
		return false;

	if (strlen(iaScript) > 0) {
		nCmd *cmd;
		cmd = ps->GetCmd(this, 'SIAS');
		if (!cmd)
			return false;

		cmd->In()->SetS(iaScript);
		ps->PutCmd(cmd);
	}

	return true;
}
