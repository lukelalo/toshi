#define N_IMPLEMENTS nPEmitter
//-------------------------------------------------------------------
//  npemit_cmds.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "particle/npemitter.h"

static void n_settimes(void *, nCmd *);
static void n_gettimes(void *, nCmd *);
static void n_setlifetime(void *, nCmd *);
static void n_getlifetime(void *, nCmd *);
static void n_setfreq(void *, nCmd *);
static void n_getfreq(void *, nCmd *);
static void n_setspeed(void *, nCmd *);
static void n_getspeed(void *, nCmd *);
static void n_setaccel(void *, nCmd *);
static void n_getaccel(void *, nCmd *);
static void n_setinnercone(void *, nCmd *);
static void n_getinnercone(void *, nCmd *);
static void n_setoutercone(void *, nCmd *);
static void n_getoutercone(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    npemitter

    @superclass
    nanimnode

    @classinfo
    Superclass of all particle emitter classes. Particle emitter
    objects are used in conjunction with particle renderers.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_settimes_fff",    'STMS', n_settimes);
    cl->AddCmd("fff_gettimes_v",    'GTMS', n_gettimes);
    cl->AddCmd("v_setlifetime_f",   'SLFT', n_setlifetime);
    cl->AddCmd("f_getlifetime_v",   'GLFT', n_getlifetime);
    cl->AddCmd("v_setfreq_f",       'SFRQ', n_setfreq);
    cl->AddCmd("f_getfreq_v",       'GFRQ', n_getfreq);
    cl->AddCmd("v_setspeed_f",      'SSPD', n_setspeed);
    cl->AddCmd("f_getspeed_v",      'GSPD', n_getspeed);
    cl->AddCmd("v_setaccel_fff",    'SACC', n_setaccel);
    cl->AddCmd("fff_getaccel_v",    'GACC', n_getaccel);
    cl->AddCmd("v_setinnercone_f",  'SICN', n_setinnercone);
    cl->AddCmd("f_getinnercone_v",  'GICN', n_getinnercone);
    cl->AddCmd("v_setoutercone_f",  'SOCN', n_setoutercone);
    cl->AddCmd("f_getoutercone_v",  'GOCN', n_getoutercone);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    settimes

    @input
    f (Start), f (Stop), f (Repeat)

    @output
    v

    @info
    Set timestamps for emitter activation. Start is the timestamp
    at which the emitter will start producing particles, Stop is
    the timestamp where the emitter will stop, Repeat is a
    timestamp after Stop, after which the emitter will rewind
    its internal time to 0.0.
*/
static void n_settimes(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    self->SetTimes(f0,f1,f2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettimes

    @input
    v

    @output
    f (Start), f (Stop), f (Repeat)

    @info
    Retrieve the values that have been set with 'settimes'
*/
static void n_gettimes(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    float f0,f1,f2;
    self->GetTimes(f0,f1,f2);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
    cmd->Out()->SetF(f2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlifetime

    @input
    f (Lifetime)

    @output
    v

    @info
    Set the lifetime for particles in seconds.
*/
static void n_setlifetime(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    self->SetLifetime(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlifetime

    @input
    v

    @output
    f (Lifetime)

    @info
    Get the lifetime for particles in seconds.
*/
static void n_getlifetime(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    cmd->Out()->SetF(self->GetLifetime());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfreq

    @input
    f (Frequency)

    @output
    v

    @info
    Set the emission frequency in particles per second.
*/
static void n_setfreq(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    self->SetFreq(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfreq

    @input
    v

    @output
    f (Frequency)

    @info
    Get the emission frequency in particles per second.
*/
static void n_getfreq(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    cmd->Out()->SetF(self->GetFreq());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setspeed

    @input
    f (Frequency)

    @output
    v

    @info
    Set the initial speed of particles in coords per second.
*/
static void n_setspeed(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    self->SetSpeed(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getspeed

    @input
    v

    @output
    f (Speed)

    @info
    Get the initial speed of particles in coords per second.
*/
static void n_getspeed(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    cmd->Out()->SetF(self->GetSpeed());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setaccel

    @input
    f (AccelX), f (AccelY), f (AccelZ)

    @output
    v

    @info
    Set the acceleration vector for particles in coords per second^2.
    The acceleration vector is defined in world space.
*/
static void n_setaccel(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    self->SetAccel(f0,f1,f2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getaccel

    @input
    v

    @output
    f (AccelX), f (AccelY), f (AccelZ)

    @info
    Get the acceleration vector of particles in coords per second^2.
*/
static void n_getaccel(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    float f0,f1,f2;
    self->GetAccel(f0,f1,f2);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
    cmd->Out()->SetF(f2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setinnercone

    @input
    f (InnerCone)

    @output
    v

    @info
    Set the inner cone angle in degrees that will limit the emission
    direction of particles.
*/
static void n_setinnercone(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    self->SetInnerCone(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getinnercone

    @input
    v

    @output
    f (InnerCone)

    @info
    Get the current inner cone angle.
*/
static void n_getinnercone(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    cmd->Out()->SetF(self->GetInnerCone());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setoutercone

    @input
    f (OuterCone)

    @output
    v

    @info
    Set the outer cone angle in degrees that will limit the emission
    direction of particles.
*/
static void n_setoutercone(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    self->SetOuterCone(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getoutercone

    @input
    v

    @output
    f (OuterCone)

    @info
    Get the current outer cone angle.
*/
static void n_getoutercone(void *o, nCmd *cmd)
{
    nPEmitter *self = (nPEmitter *) o;
    cmd->Out()->SetF(self->GetOuterCone());
}

//-------------------------------------------------------------------
//  SaveCmds()
//  17-Mar-00   floh    created
//-------------------------------------------------------------------
bool nPEmitter::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nAnimNode::SaveCmds(fs)) {
        nCmd *cmd;

        //--- settimes ---
        if ((cmd = fs->GetCmd(this,'STMS'))) {
            float f0,f1,f2;
            this->GetTimes(f0,f1,f2);
            cmd->In()->SetF(f0);
            cmd->In()->SetF(f1);
            cmd->In()->SetF(f2);
            fs->PutCmd(cmd);
        }

        //--- setlifetime ---
        if ((cmd = fs->GetCmd(this,'SLFT'))) {
            cmd->In()->SetF(this->GetLifetime());
            fs->PutCmd(cmd);
        }

        //--- setfreq ---
        if ((cmd = fs->GetCmd(this,'SFRQ'))) {
            cmd->In()->SetF(this->GetFreq());
            fs->PutCmd(cmd);
        }

        //--- setspeed ---
        if ((cmd = fs->GetCmd(this,'SSPD'))) {
            cmd->In()->SetF(this->GetSpeed());
            fs->PutCmd(cmd);
        }

        //--- setaccel ---
        if ((cmd = fs->GetCmd(this,'SACC'))) {
            float f0,f1,f2;
            this->GetAccel(f0,f1,f2);
            cmd->In()->SetF(f0);
            cmd->In()->SetF(f1);
            cmd->In()->SetF(f2);
            fs->PutCmd(cmd);
        }

        //--- setinnercone ---
        if ((cmd = fs->GetCmd(this,'SICN'))) {
            cmd->In()->SetF(this->GetInnerCone());
            fs->PutCmd(cmd);
        }

        //--- setoutercone ---
        if ((cmd = fs->GetCmd(this,'SOCN'))) {
            cmd->In()->SetF(this->GetOuterCone());
            fs->PutCmd(cmd);
        }

        retval = true;
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------




