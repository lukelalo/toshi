#define N_IMPLEMENTS nFogNode
//-------------------------------------------------------------------
//  nfog_cmds.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/nfognode.h"

static void n_setmode(void *, nCmd *);
static void n_getmode(void *, nCmd *);
static void n_setrange(void *, nCmd *);
static void n_getrange(void *, nCmd *);
static void n_setdensity(void *, nCmd *);
static void n_getdensity(void *, nCmd *);
static void n_setcolor(void *, nCmd *);
static void n_getcolor(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nfognode

    @superclass
    nvisnode

    @classinfo
    Define an ambient fog source. A fog node defines fog
    attributes for the entire scene. Several fog nodes in the
    same scene overwrite each other (like ambient light sources).
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setmode_s",       'SMOD', n_setmode);
    cl->AddCmd("s_getmode_v",       'GMOD', n_getmode);
    cl->AddCmd("v_setrange_ff",     'SRNG', n_setrange);
    cl->AddCmd("ff_getrange_v",     'GRNG', n_getrange);
    cl->AddCmd("v_setdensity_f",    'SDNS', n_setdensity);
    cl->AddCmd("f_getdensity_v",    'GDNS', n_getdensity);
    cl->AddCmd("v_setcolor_ffff",   'SCOL', n_setcolor);
    cl->AddCmd("ffff_getcolor_v",   'GCOL', n_getcolor);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmode

    @input
    s (FogMode=linear,exp,exp2)

    @output
    v

    @info
    Define one of several fog computation modes. Default mode is
    linear fog:
    Equation for linear fog is: f = (end - dist) / (end - start)
    Equation for exp fog is:    f = e ^ (-density * dist)
    Equation for exp2 fog is:   f = e ^ ((-density * dist)^2)
*/
static void n_setmode(void *o, nCmd *cmd)
{
    nFogNode *self = (nFogNode *) o;
    const char *s = cmd->In()->GetS();
    nFogMode fm;
    if      (strcmp(s,"linear")==0) fm=N_FOGMODE_LINEAR;
    else if (strcmp(s,"exp")==0)    fm=N_FOGMODE_EXP;
    else if (strcmp(s,"exp2")==0)   fm=N_FOGMODE_EXP2;
    else {
        n_error("Unsupported fog mode '%s', valid modes are 'linear','exp','exp2'\n",s);
    }
    self->SetMode(fm);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmode

    @input
    v

    @output
    s (FogMode=linear,exp,exp2)

    @info
    Return the current fog mode.
*/
static void n_getmode(void *o, nCmd *cmd)
{
    nFogNode *self = (nFogNode *) o;
    switch (self->GetMode()) {
        case N_FOGMODE_LINEAR:  cmd->Out()->SetS("linear"); break;
        case N_FOGMODE_EXP:     cmd->Out()->SetS("exp"); break;
        case N_FOGMODE_EXP2:    cmd->Out()->SetS("exp2"); break;
        default: break;
    };
}

//------------------------------------------------------------------------------
/**
    @cmd
    setrange

    @input
    f (FogStart), f (FogEnd)

    @output
    v

    @info
    Set starting and ending distance for linear fog mode.
    The default values are FogStart=0.0, FogEnd=1.0.
    These values are not used for exp and exp2 fog.
*/
static void n_setrange(void *o, nCmd *cmd)
{
    nFogNode *self = (nFogNode *) o;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    self->SetRange(f0,f1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrange

    @input
    v

    @output
    f (FogStart), f (FogEnd)

    @info
    Get linear fog range defined with 'setrange'.
*/
static void n_getrange(void *o, nCmd *cmd)
{
    nFogNode *self = (nFogNode *) o;
    float f0,f1;
    self->GetRange(f0,f1);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setdensity

    @input
    f (FogDensity)

    @output
    v

    @info
    Set the density value used in the exp and exp2 fog formula.
    The default value is 1.0.
    This value is not used for linear fog.
*/
static void n_setdensity(void *o, nCmd *cmd)
{
    nFogNode *self = (nFogNode *) o;
    self->SetDensity(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getdensity

    @input
    v

    @output
    f (FogDensity)

    @info
    Get the fog density defined by 'setdensity'.
*/
static void n_getdensity(void *o, nCmd *cmd)
{
    nFogNode *self = (nFogNode *) o;
    cmd->Out()->SetF(self->GetDensity());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcolor

    @input
    f (Red), f (Green), f (Blue), f (Alpha)

    @output
    v

    @info
    Set the fog color.
*/
static void n_setcolor(void *o, nCmd *cmd)
{
    nFogNode *self = (nFogNode *) o;
    float r = cmd->In()->GetF();
    float g = cmd->In()->GetF();
    float b = cmd->In()->GetF();
    float a = cmd->In()->GetF();
    self->SetColor(r,g,b,a);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcolor

    @input
    v

    @output
    f (Red), f (Green), f (Blue), f (Alpha)

    @info
    Get the fog color.
*/
static void n_getcolor(void *o, nCmd *cmd)
{
    nFogNode *self = (nFogNode *) o;
    float r,g,b,a;
    self->GetColor(r,g,b,a);
    cmd->Out()->SetF(r);
    cmd->Out()->SetF(g);
    cmd->Out()->SetF(b);
    cmd->Out()->SetF(a);
}

//-------------------------------------------------------------------
//  SaveCmds()
//  23-Mar-00   floh    created
//-------------------------------------------------------------------
bool nFogNode::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;

        //--- setmode ---
        if ((cmd = fs->GetCmd(this,'SMOD'))) {
            switch (this->GetMode()) {
                case N_FOGMODE_LINEAR:  cmd->In()->SetS("linear"); break;
                case N_FOGMODE_EXP:     cmd->In()->SetS("exp"); break;
                case N_FOGMODE_EXP2:    cmd->In()->SetS("exp2"); break;
                default: break;
            };
            fs->PutCmd(cmd);
        }

        //--- setrange ---
        if ((cmd = fs->GetCmd(this,'SRNG'))) {
            float f0,f1;
            this->GetRange(f0,f1);
            cmd->In()->SetF(f0);
            cmd->In()->SetF(f1);
            fs->PutCmd(cmd);
        }

        //--- setdensity ---
        if ((cmd = fs->GetCmd(this,'SDNS'))) {
            cmd->In()->SetF(this->GetDensity());
            fs->PutCmd(cmd);
        }

        //--- setcolor ---
        if ((cmd = fs->GetCmd(this,'SCOL'))) {
            float r,g,b,a;
            this->GetColor(r,g,b,a);
            cmd->In()->SetF(r);
            cmd->In()->SetF(g);
            cmd->In()->SetF(b);
            cmd->In()->SetF(a);
            fs->PutCmd(cmd);
        }
        retval = true;
    }
    return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
