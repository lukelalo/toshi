#define N_IMPLEMENTS nCamera
//------------------------------------------------------------------------------
//  (C) 2002	ling
//------------------------------------------------------------------------------
#include "laberintosultan/ncamera.h"
#include "kernel/npersistserver.h"

static void n_setworld(void* slf, nCmd* cmd);
static void n_setnode(void* slf, nCmd* cmd);
static void n_settarget(void* slf, nCmd* cmd);
static void n_setposition(void* slf, nCmd* cmd);
static void n_setstyle(void* slf, nCmd* cmd);
static void n_settargetposition(void* slf, nCmd* cmd);
static void n_setgiroderecha(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ncamera
    
    @superclass
    nroot

    @classinfo
    A detailed description of what the class does (written for script programmers!) 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setworld_s", 'TWRD', n_setworld);
    clazz->AddCmd("v_setnode_s", 'TNOD', n_setnode);
    clazz->AddCmd("v_settarget_s", 'TGTE', n_settarget);
    clazz->AddCmd("v_setposition_fff", 'SPOS', n_setposition);
    clazz->AddCmd("v_setstyle_s", 'SSTY', n_setstyle);
    clazz->AddCmd("v_settargetposition_fff", 'STPS', n_settargetposition);
    clazz->AddCmd("v_setgiroderecha_f", 'SGDC', n_setgiroderecha);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setworld

    @input
    s(nWorld path)

    @output
    v

    @info
    Sets the world...
*/
static
void
n_setworld(void* slf, nCmd* cmd)
{
    nCamera* self = (nCamera*)slf;
    self->SetWorld(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setnode

    @input
    s(n3DNode path)

    @output
    v

    @info
    Sets associated n3dnode to update, mainly for ensuring the audio listener
    is in the appropriate place so things don't sound funny.
*/
static
void
n_setnode(void* slf, nCmd* cmd)
{
    nCamera* self = (nCamera*)slf;
    self->SetNode(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    settarget

    @input
    s(nEntity path)

    @output
    v

    @info
    Sets the target entity to track.
*/
static
void
n_settarget(void* slf, nCmd* cmd)
{
    nCamera* self = (nCamera*)slf;
    self->SetTarget(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setposition

    @input
    f, f, f

    @output
    v

    @info
    Sets camera position.
*/
static
void
n_setposition(void* slf, nCmd* cmd)
{
    nCamera* self = (nCamera*)slf;
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->SetPosition(x, y, z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstyle

    @input
    s

    @output
    v

    @info
    Set the camera style.

        stationary
        locked_chase
        chase
		totalstationary
*/
static
void
n_setstyle(void* slf, nCmd* cmd)
{
    nCamera* self = (nCamera*)slf;

    nCamera::Style style = nCamera::STATIONARY;
    const char *str = cmd->In()->GetS();
    if (strcmp(str, "stationary") == 0)         style = nCamera::STATIONARY;
    else if (strcmp(str, "locked_chase") == 0)  style = nCamera::LOCKED_CHASE;
    else if (strcmp(str, "chase") == 0)         style = nCamera::CHASE;
	else if (strcmp(str, "totalstationary") == 0) style = nCamera::TOTAL_STATIONARY;
    else {
        n_printf("Unknown camera style '%s', valid is 'stationary|locked_chase|chase|totalstationary'!\n",str);
    }
    self->SetStyle(style);
}

//------------------------------------------------------------------------------
/**
    @cmd
    settargetposition

    @input
    f, f, f

    @output
    v

    @info
    Sets target camera position, final result depends on style.
*/
static
void
n_settargetposition(void* slf, nCmd* cmd)
{
    nCamera* self = (nCamera*)slf;
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->SetTargetPosition(x, y, z);
}


static
void
n_setgiroderecha(void* slf, nCmd* cmd)
{
    nCamera* self = (nCamera*)slf;
	self->setGiroDerecha(cmd->In()->GetF());
}


//------------------------------------------------------------------------------
/**
    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nCamera::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        //nCmd* cmd = ps->GetCmd(this, 'XXXX');
        //ps->PutCmd(cmd);

        return true;
    }
    return false;
}

