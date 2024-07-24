#define N_IMPLEMENTS nIACLIPS

#include "toshi/niaCLIPS.h"
#include "kernel/npersistserver.h"

static void n_getclipsscript(void *slf, nCmd *cmd);
static void n_setclipsscript(void *slf, nCmd *cmd);

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
    clazz->AddCmd("v_setclipsscript_s", 'SCSS', n_setclipsscript);
	clazz->AddCmd("s_getclipsscript_v", 'GCSS', n_getclipsscript);
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

//------------------------------------------------------------------------------
/**
    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/
static void n_setclipsscript(void *slf, nCmd *cmd) {
	nIACLIPS *self = (nIACLIPS*)slf;
	self->SetCLIPSScript(cmd->In()->GetS());
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
static void n_getclipsscript(void *slf, nCmd *cmd) {
	nIACLIPS *self = (nIACLIPS*)slf;
	cmd->Out()->SetS(self->GetCLIPSScript());
}

bool
nIACLIPS::SaveCmds(nPersistServer* ps)
{
	if (!nIA::SaveCmds(ps))
		return false;

	return true;
}
