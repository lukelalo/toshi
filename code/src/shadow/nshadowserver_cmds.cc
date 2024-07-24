#define N_IMPLEMENTS nShadowServer
//------------------------------------------------------------------------------
//  nshadowserver_cmds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shadow/nshadowserver.h"
#include "kernel/npersistserver.h"

static void n_setcastshadows(void* slf, nCmd* cmd);
static void n_getcastshadows(void* slf, nCmd* cmd);
static void n_setshadowcolor(void* slf, nCmd* cmd);
static void n_getshadowcolor(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nshadowserver
  
    @superclass
    nroot

    @classinfo
    Super class for shadow casting subsystem.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setcastshadows_b",    'SCSH', n_setcastshadows);
    cl->AddCmd("b_getcastshadows_v",    'GCSH', n_getcastshadows);
    cl->AddCmd("v_setshadowcolor_ffff", 'SSCL', n_setshadowcolor);
    cl->AddCmd("ffff_getshadowcolor_v", 'GSCL', n_getshadowcolor);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcastshadows

    @input
    b (CastShadowsFlag)

    @output
    v

    @info
    Turn shadow casting globally on/off.
*/
static
void
n_setcastshadows(void* slf, nCmd* cmd)
{
    nShadowServer* self = (nShadowServer*) slf;
    self->SetCastShadows(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcastshadows

    @input
    v

    @output
    b (CastShadowsFlag)

    @info
    Get global shadow casting state.
*/
static
void
n_getcastshadows(void* slf, nCmd* cmd)
{
    nShadowServer* self = (nShadowServer*) slf;
    cmd->Out()->SetB(self->GetCastShadows());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setshadowcolor

    @input
    f(Red), f(Green), f(Blue), f(Alpha)

    @output
    v

    @info
    Set the current shadow color. Alpha defines shadow transparency.
*/
static
void
n_setshadowcolor(void* slf, nCmd* cmd)
{
    nShadowServer* self = (nShadowServer*) slf;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    float f3 = cmd->In()->GetF();
    self->SetShadowColor(f0, f1, f2, f3);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getshadowcolor

    @input
    v

    @output
    f(Red), f(Green), f(Blue), f(Alpha)
    
    @info
    Get the current shadow color.
*/
static
void
n_getshadowcolor(void* slf, nCmd* cmd)
{
    nShadowServer* self = (nShadowServer*) slf;
    float f0, f1, f2, f3;
    self->GetShadowColor(f0, f1, f2, f3);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
    cmd->Out()->SetF(f2);
    cmd->Out()->SetF(f3);
}

//------------------------------------------------------------------------------
