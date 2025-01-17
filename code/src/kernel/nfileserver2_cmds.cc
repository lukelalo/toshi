#define N_IMPLEMENTS nFileServer2
#define N_KERNEL
//------------------------------------------------------------------------------
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nfileserver2.h"
#include "kernel/npersistserver.h"

static void n_setassign(void* slf, nCmd* cmd);
static void n_getassign(void* slf, nCmd* cmd);
static void n_manglepath(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nfileserver2
    
    @superclass
    nroot

    @classinfo
    da nu fileserver 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setassign_ss", 'SASS', n_setassign);
    clazz->AddCmd("s_getassign_s", 'GASS', n_getassign);
    clazz->AddCmd("s_manglepath_s", 'MNGP', n_manglepath);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setassign

    @input
    s assign name
    s path

    @output
    v

    @info
    defines an assign with the specified name and links it to the specified
	path
*/
static
void
n_setassign(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SetAssign(s0, s1);
}
//------------------------------------------------------------------------------
/**
    @cmd
    getassign

    @input
    s assign name

    @output
	s path

    @info
	siehe setassign
*/
static
void
n_getassign(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    cmd->Out()->SetS(self->GetAssign(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    manglepath

    @input
    s(UnmangledPath)

    @output
    s(MangledPath)

    @info
    Convert a path with assigns into a native absolute path.
*/
static
void
n_manglepath(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    char buf[N_MAXPATH];
    cmd->Out()->SetS(self->ManglePath(cmd->In()->GetS(), buf, sizeof(buf)));
}
