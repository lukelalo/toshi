#define N_IMPLEMENTS nPRender
//-------------------------------------------------------------------
//  nprender_cmds.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "particle/nprender.h"

static void n_setstretch(void *, nCmd *);
static void n_getstretch(void *, nCmd *);
static void n_setspin(void *, nCmd *);
static void n_getspin(void *, nCmd *);
static void n_setspinaccel(void *, nCmd *);
static void n_getspinaccel(void *, nCmd *);
static void n_setemitter(void *, nCmd *);
static void n_getemitter(void *, nCmd *);
static void n_beginkeys(void *, nCmd *);
static void n_setkey(void *, nCmd *);
static void n_getkey(void *, nCmd *);
static void n_endkeys(void *, nCmd *);
static void n_getnumkeys(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nprender

    @superclass
    nanimnode

    @classinfo
    Superclass of all particle renderer classes. Renders particles
    emitted by an nPEmitter object. The emitter object must be
    attached with the 'setemitter' command and must be a direct
    subobject of the nprender object.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setstretch_b",            'SSTR', n_setstretch);
    cl->AddCmd("b_getstretch_v",            'GSTR', n_getstretch);
    cl->AddCmd("v_setspin_f",               'SSPN', n_setspin);
    cl->AddCmd("f_getspin_v",               'GSPN', n_getspin);
    cl->AddCmd("v_setspinaccel_f",          'SSPA', n_setspinaccel);
    cl->AddCmd("f_getspinaccel_v",          'GSPA', n_getspinaccel);
    cl->AddCmd("v_setemitter_s",            'SEMT', n_setemitter);
    cl->AddCmd("s_getemitter_v",            'GEMT', n_getemitter);
    cl->AddCmd("v_beginkeys_i",             'BKEY', n_beginkeys);
    cl->AddCmd("v_setkey_iffffff",          'SKEY', n_setkey);
    cl->AddCmd("v_endkeys_v",               'EKEY', n_endkeys);
    cl->AddCmd("i_getnumkeys_v",            'GNKS', n_getnumkeys);
    cl->AddCmd("ffffff_getkey_i",           'GKEY', n_getkey);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstretch

    @input
    b (Stretch)

    @output
    v

    @info
    Set to true if the particle renderer should attempt to stretch
    the particle from its previous to its current position.
*/
static void n_setstretch(void *o, nCmd *cmd)
{
    nPRender *self = (nPRender *) o;
    self->SetStretch(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstretch

    @input

    @output
    b (Stretch)

    @info
    Get the current state of the stretch flag.
*/
static void n_getstretch(void *o, nCmd *cmd)
{
    nPRender *self = (nPRender *) o;
    cmd->Out()->SetB(self->GetStretch());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setspin

    @input
    f (Spin)

    @output
    v

    @info
    Set the spinning speed for particles in degrees per second.
*/
static void n_setspin(void *o, nCmd *cmd)
{
    nPRender *self = (nPRender *) o;
    self->SetSpin(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getspin

    @input
    v

    @output
    f (Spread)

    @info
    Get the spinning speed for particles in degrees per second.
*/
static void n_getspin(void *o, nCmd *cmd)
{
    nPRender *self = (nPRender *) o;
    cmd->Out()->SetF(self->GetSpin());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setspinaccel

    @input
    f (SpinAccel)

    @output
    v

    @info
    Set the spinning acceleration for particles in
    degrees per second^2.
*/
static void n_setspinaccel(void *o, nCmd *cmd)
{
    nPRender *self = (nPRender *) o;
    self->SetSpinAccel(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getspinaccel

    @input
    v

    @output
    f (SpinAccel)

    @info
    Get the spinning acceleration for particles in
    degrees per second^2.
*/
static void n_getspinaccel(void *o, nCmd *cmd)
{
    nPRender *self = (nPRender *) o;
    cmd->Out()->SetF(self->GetSpinAccel());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setemitter

    @input
    s (EmitterObjectName)

    @output
    v

    @info
    Set name of emitter object. Must be direct subobject of
    this object and of class or subclass of nPEmitter.
*/
static void n_setemitter(void *o, nCmd *cmd)
{
    nPRender *self = (nPRender *) o;
    self->SetEmitter(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getemitter

    @input
    v

    @output
    s (EmitterObjectName)

    @info
    Return the name of the current emitter object.
*/
static void n_getemitter(void *o, nCmd *cmd)
{
    nPRender *self = (nPRender *) o;
    char buf[N_MAXPATH];
    cmd->Out()->SetS(self->GetEmitter(buf,sizeof(buf)));
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginkeys

    @input
    i (NumKeys)

    @output
    v

    @info
    Start per-particle key frame definition. Takes number of keyframes
    as argument.
*/
static void n_beginkeys(void *o, nCmd *cmd)
{
    nPRender *self = (nPRender *) o;
    self->BeginKeys(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setkey

    @input
    i (KeyIndex), f (Size), f (Bounce), f (Red), f (Green), f (Blue), f (Alpha)

    @output
    v

    @info
    Set a per-particle keyframe. 
*/
static void n_setkey(void *o, nCmd *cmd)
{
    nPRender *self = (nPRender *) o;
    int i = cmd->In()->GetI();
    float size   = cmd->In()->GetF();
    float bounce = cmd->In()->GetF();
    float r      = cmd->In()->GetF();
    float g      = cmd->In()->GetF();
    float b      = cmd->In()->GetF();
    float a      = cmd->In()->GetF();
    self->SetKey(i,size,bounce,r,g,b,a);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endkeys

    @input
    v

    @output
    v

    @info
    End the per-particle keyframe definitions.
*/
static void n_endkeys(void *o, nCmd *)
{
    nPRender *self = (nPRender *) o;
    self->EndKeys();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumkeys

    @input
    v

    @output
    i (NumKeys)

    @info
    Return number of per-particle keyframes.
*/
static void n_getnumkeys(void *o, nCmd *cmd)
{
    nPRender *self = (nPRender *) o;
    cmd->Out()->SetI(self->GetNumKeys());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkey

    @input
    i (KeyIndex)

    @output
    f (Size), f (Bounce), f (Red), f (Green), f (Blue), f (Alpha)

    @info
    Return definition of given keyframe.
*/
static void n_getkey(void *o, nCmd *cmd)
{
    nPRender *self = (nPRender *) o;
    int i = cmd->In()->GetI();
    float size,bounce,r,g,b,a;
    self->GetKey(i,size,bounce,r,g,b,a);
    cmd->Out()->SetF(size);
    cmd->Out()->SetF(bounce);
    cmd->Out()->SetF(r);
    cmd->Out()->SetF(g);
    cmd->Out()->SetF(b);
    cmd->Out()->SetF(a);
}

//-------------------------------------------------------------------
//  SaveCmds()
//  20-Mar-00   floh    created
//  15-Oct-00   floh    + 'setstretch'
//-------------------------------------------------------------------
bool nPRender::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nAnimNode::SaveCmds(fs)) {
        nCmd *cmd;

        //--- setstretch ---
        cmd = fs->GetCmd(this,'SSTR');
        cmd->In()->SetB(this->GetStretch());
        fs->PutCmd(cmd);

        //--- setspin ---
        cmd = fs->GetCmd(this,'SSPN');
        cmd->In()->SetF(this->GetSpin());
        fs->PutCmd(cmd);

        //--- setspinaccel ---
        cmd = fs->GetCmd(this,'SSPA');
        cmd->In()->SetF(this->GetSpinAccel());
        fs->PutCmd(cmd);

        //--- setemitter ---
        cmd = fs->GetCmd(this,'SEMT');
        char buf[N_MAXPATH];
        cmd->In()->SetS(this->GetEmitter(buf,sizeof(buf)));
        fs->PutCmd(cmd);

        //--- keyframes ---
        int keys;
        if ((keys = this->GetNumKeys()) > 0) {
            cmd = fs->GetCmd(this,'BKEY');
            cmd->In()->SetI(keys);
            fs->PutCmd(cmd);

            int i;
            for (i=0; i<keys; i++) {
                cmd = fs->GetCmd(this,'SKEY');
                float size,bounce,r,g,b,a;
                this->GetKey(i,size,bounce,r,g,b,a);
                cmd->In()->SetI(i);
                cmd->In()->SetF(size);
                cmd->In()->SetF(bounce);
                cmd->In()->SetF(r);
                cmd->In()->SetF(g);
                cmd->In()->SetF(b);
                cmd->In()->SetF(a);
                fs->PutCmd(cmd);
            }
            cmd = fs->GetCmd(this,'EKEY');
            fs->PutCmd(cmd);
        }
        retval = true;
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
