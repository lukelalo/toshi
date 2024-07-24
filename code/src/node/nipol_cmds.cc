#define N_IMPLEMENTS nIpol
//-------------------------------------------------------------------
//  node/nipol_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include "kernel/npersistserver.h"
#include "node/nipol.h"

static void n_addkey1f(void *, nCmd *);
static void n_addkey2f(void *, nCmd *);
static void n_addkey3f(void *, nCmd *);
static void n_addkey4f(void *, nCmd *);
static void n_getval1f(void *, nCmd *);
static void n_getval2f(void *, nCmd *);
static void n_getval3f(void *, nCmd *);
static void n_getval4f(void *, nCmd *);
static void n_beginkeys(void *, nCmd *);
static void n_setkey1f(void *, nCmd *);
static void n_setkey2f(void *, nCmd *);
static void n_setkey3f(void *, nCmd *);
static void n_setkey4f(void *, nCmd *);
static void n_endkeys(void *, nCmd *);
static void n_getkeyinfo(void *, nCmd *);
static void n_getkey1f(void *, nCmd *);
static void n_getkey2f(void *, nCmd *);
static void n_getkey3f(void *, nCmd *);
static void n_getkey4f(void *, nCmd *);
static void n_connect(void *, nCmd *);
static void n_getconnect(void *, nCmd *);
// static void n_disconnect(void *, nCmd *);
static void n_setipoltype(void *, nCmd *);
static void n_getipoltype(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nipol

    @superclass
    nanimnode

    @classinfo
    A nipol (short for interpolator) object is able to animate
    properties of the parent object. Basically it goes like this:
    (1) You create one, or more, nipol objects as children of the
        object to be animated. The class of the object to be animated
        must derive from a nVisNode class.
    (2) You 'connect' the nipol object to a 'command' of its parent
        object. For the nipol class a channel is a script command which
        accepts 1..4 float arguments (for example n3dnode::t[xyz], 
        n3dnode::r[xyz], n3dnode::s[xyz], but also n3dlight::setcolor, 
        etc...).
    (3) You select the channel that will drive the nipol, this is by 
        default the 'time' channel.
    (4) You start to attach 1,2,3 or 4 dimensional keyframes to the
        nipol object (depending on what the channel needs). 
    (5) Select the type of interpolation:
          Linear     - Does a linear interpolation between keyframes.
          Step       - Steps between keyframes, no interpolation.
          Quaternion - Spherical quaternion interpolation. Only makes 
                       sense when connected to a command that takes 
                       quaternions.
          Cubic      - Cubic interpolation.
          Spline     - Catmull-Rom spline interpolation.
            
    The nipol node will attach to its parent as a depend node. The nipol
    will compute the actual interpolation values and generate a dispatch()
    call to the parent object.
    Consider also that the object does not animate the parent object
    (the to be animated object) BEFORE the first keyframe or AFTER the
    last keyframe. So you can have several interpolators working
    on the same channel but working one after the other.
    In the loop mode the timer switches from the last keyframe back
    to the start of the local timeline (t = 0). However the first keyframe
    does not have to start at (t = 0) but can be anytime later.
    the interpolator is completely inactive up to the point where the
    first keyframe is reached.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("b_connect_s",       'CNCT', n_connect);
    cl->AddCmd("s_getconnect_v",    'GCNT', n_getconnect);
//    cl->AddCmd("v_disconnect_v",    'DSCN', n_disconnect);
    cl->AddCmd("v_setipoltype_s",   'SIPT', n_setipoltype);
    cl->AddCmd("s_getipoltype_v",   'GIPT', n_getipoltype);
    cl->AddCmd("v_addkey1f_ff",     'AK1F', n_addkey1f);
    cl->AddCmd("v_addkey2f_fff",    'AK2F', n_addkey2f);
    cl->AddCmd("v_addkey3f_ffff",   'AK3F', n_addkey3f);
    cl->AddCmd("v_addkey4f_fffff",  'AK4F', n_addkey4f);
    cl->AddCmd("f_getval1f_v",      'GVL1', n_getval1f);
    cl->AddCmd("ff_getval2f_v",     'GVL2', n_getval2f);
    cl->AddCmd("fff_getval3f_v",    'GVL3', n_getval3f);
    cl->AddCmd("ffff_getval4f_v",   'GVL4', n_getval4f);
    cl->AddCmd("v_beginkeys_ii",    'BGKS', n_beginkeys);
    cl->AddCmd("v_setkey1f_iff",    'SK1F', n_setkey1f);
    cl->AddCmd("v_setkey2f_ifff",   'SK2F', n_setkey2f);
    cl->AddCmd("v_setkey3f_iffff",  'SK3F', n_setkey3f);
    cl->AddCmd("v_setkey4f_ifffff", 'SK4F', n_setkey4f);
    cl->AddCmd("v_endkeys_v",       'ENKS', n_endkeys);
    cl->AddCmd("ii_getkeyinfo_v",   'GKIN', n_getkeyinfo);
    cl->AddCmd("ff_getkey1f_i",     'GK1F', n_getkey1f);
    cl->AddCmd("fff_getkey2f_i",    'GK2F', n_getkey2f);
    cl->AddCmd("ffff_getkey3f_i",   'GK3F', n_getkey3f);
    cl->AddCmd("fffff_getkey4f_i",  'GK4F', n_getkey4f);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    connect

    @input
    s (Channelname)

    @output
    b (Success)

    @info
    Connects the nipol object with the interpolation channel
    of the parent object. An interpolation channel is a script 
    command which accepts 1..4 float arguments (for example 
    n3dnode::t[xyz], n3dnode::r[xyz], n3dnode::s[xyz], but also 
    n3dlight::setcolor, etc...) as parameters. On each triggering
    of the nipol object it calculates the actual interpolation 
    values and uses the indicated script command on the parent
    object.
*/
static void n_connect(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    cmd->Out()->SetB(self->Connect(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    disconnect

    @input
    v

    @output
    v

    @info
    Disconnect the nipol object from its current connection.
*/
/*
static void n_disconnect(void *o, nCmd *)
{
    nIpol *self = (nIpol *) o;
    self->Disconnect();
}
*/

//------------------------------------------------------------------------------
/**
    @cmd
    getconnect

    @input
    v

    @output
    s (Connection)

    @info
    Get the current connection (can be 'null' if disconnected).
*/
static void n_getconnect(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    cmd->Out()->SetS(self->GetConnect());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setipoltype

    @input
    s (IpolType = [step | linear | quaternion | cubic | spline])

    @output
    v

    @info
    Define how interpolation actually should take place:
    step:       no interpolation
    linear:     linear interpolation (default)
    quaternion: use spherical quaternion interpolation
    cubic:      use cubic interpolation
    spline:     catmull-rom spline interpolation

*/
static void n_setipoltype(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    const char *s = cmd->In()->GetS();
    nIpol::nIpolType t;
    if      (strcmp(s,"step")==0)       t=nIpol::N_STEP;
    else if (strcmp(s,"linear")==0)     t=nIpol::N_LINEAR;
    else if (strcmp(s,"quaternion")==0) t=nIpol::N_QUATERNION;
    else if (strcmp(s,"cubic")==0)      t=nIpol::N_CUBIC;
    else if (strcmp(s,"spline")==0)     t=nIpol::N_SPLINE;
    else {
        n_error("unknown interpolation type '%s', valid is (step | linear | quaternion)\n", s);
    }
    self->SetIpolType(t);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getipoltype

    @input
    v

    @output
    s (IpolType = [step | linear | quaternion | cubic | spline])

    @info
    Return current interpolation type.
*/
static void n_getipoltype(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    nIpol::nIpolType t = self->GetIpolType();
    const char *s;
    switch (t) {
        case nIpol::N_STEP:        s="step"; break;
        case nIpol::N_QUATERNION:  s="quaternion"; break;
        case nIpol::N_CUBIC:       s="cubic"; break;
        case nIpol::N_SPLINE:      s="spline"; break;
        case nIpol::N_LINEAR:      
        default:                   s="linear"; break;
    }
    cmd->Out()->SetS(s);
}

//------------------------------------------------------------------------------
/**
    @cmd
    addkey1f            

    @input
    f (Time), f (Value0)

    @output
    v

    @info
    Attaches a 1 dimensional keyframe to the object. Time
    is a point in time on the timeline in seconds 
*/
static void n_addkey1f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        float t = cmd->In()->GetF();
        float x = cmd->In()->GetF();
        self->AddKey1f(t,x);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    addkey2f            

    @input
    f (Time), f (Value0), f (Value1)

    @output
    v

    @info
    Attaches a 2 dimensional keyframe to the object.
*/
static void n_addkey2f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        float t = cmd->In()->GetF();
        float x = cmd->In()->GetF();
        float y = cmd->In()->GetF();
        self->AddKey2f(t,x,y);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    addkey3f            

    @input
    f (Time), f (Value0), f (Value1), f (Value2)

    @output
    v

    @info
    Attaches a 3 dimensional keyframe to the object.
*/
static void n_addkey3f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        float t = cmd->In()->GetF();
        float x = cmd->In()->GetF();
        float y = cmd->In()->GetF();
        float z = cmd->In()->GetF();
        self->AddKey3f(t,x,y,z);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    addkey4f            

    @input
    f (Time), f (Value0), f (Value1), f (Value2), f (Value3) 

    @output
    v

    @info
    Attaches a 4 dimensional keyframe to the object. 
*/
static void n_addkey4f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        float t = cmd->In()->GetF();
        float x = cmd->In()->GetF();
        float y = cmd->In()->GetF();
        float z = cmd->In()->GetF();
        float w = cmd->In()->GetF();
        self->AddKey4f(t,x,y,z,w);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    getval1f

    @input
    v

    @output
    f (Value0)

    @info
    Queries the actual state of the interpolator as a 1 
    dimensional value. If the interpolator is not connected
    you can hereby investigate its actual state. This only
    works if the parent object energizes the interpolator
    nevertheless, since the interpolator registers at the
    parent object as a "DependNode" only at a "connect".
    If this does not occur then the interpolator is passive
    and does NOT manipulate its parent object. In case of this
    the parent object itself must query actively (e.g. with
    the 'getval*f' commands).
    Works for example with the nmixer class.
*/
static void n_getval1f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    cmd->Out()->SetF(self->curval.x);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getval2f

    @input
    v

    @output
    f (Value0), f (Value1)

    @info
    Queries the actual state of the interpolator as a 2D-value.
*/
static void n_getval2f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    cmd->Out()->SetF(self->curval.x);
    cmd->Out()->SetF(self->curval.y);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getval3f

    @input
    v

    @output
    f (Value0), f (Value1), f (Value2)

    @info
    Queries the actual state of the interpolator as a 3D-value.
*/
static void n_getval3f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    cmd->Out()->SetF(self->curval.x);
    cmd->Out()->SetF(self->curval.y);
    cmd->Out()->SetF(self->curval.z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getval4f

    @input
    v

    @output
    f (Value0), f (Value1), f (Value2), f (Value3)

    @info
    Queries the actual state of the interpolator as a 4D-value.
*/
static void n_getval4f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    cmd->Out()->SetF(self->curval.x);
    cmd->Out()->SetF(self->curval.y);
    cmd->Out()->SetF(self->curval.z);
    cmd->Out()->SetF(self->curval.w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginkeys

    @input
    i (NumKeys), i (KeySize)

    @output
    v

    @info
    Starts a keyframe definition. 'NumKeys' is the number
    of keyframes you want to define, 'KeySize' the dimension 
    of the keyframes (1,2,3 or 4).
    'beginkeys' reallocates the internal keyarray, all
    formerly defined keys are lost. After the 'beginkeys' there
    must follow exactly 'NumKeys' times 'setkeyXf' calls, where
    'X' is the 'KeySize'. Thereafter comes a closing 'endkeys'.
*/
static void n_beginkeys(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        int n = cmd->In()->GetI();
        int s = cmd->In()->GetI();
        self->BeginKeys(n,s);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    setkey1f

    @input
    i (KeyIndex)
    f (Time)
    f (Value0)

    @output
    v

    @info
    Defines a 1-dimensional keyframe.
*/
static void n_setkey1f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        int i   = cmd->In()->GetI();
        float t = cmd->In()->GetF();
        float x = cmd->In()->GetF();
        self->SetKey1f(i,t,x);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    setkey2f

    @input
    i (KeyIndex)
    f (Time)
    f (Value0)
    f (Value1)

    @output
    v

    @info
    Defines a 2-dimensional keyframe.
*/
static void n_setkey2f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        int i   = cmd->In()->GetI();
        float t = cmd->In()->GetF();
        float x = cmd->In()->GetF();
        float y = cmd->In()->GetF();
        self->SetKey2f(i,t,x,y);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    setkey3f

    @input
    i(KeyIndex)
    f(Time)
    f(Value0)
    f(Value1)
    f(Value2)

    @output
    v

    @info
    Defines a 3-dimensional keyframe.
*/
static void n_setkey3f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        int i   = cmd->In()->GetI();
        float t = cmd->In()->GetF();
        float x = cmd->In()->GetF();
        float y = cmd->In()->GetF();
        float z = cmd->In()->GetF();
        self->SetKey3f(i,t,x,y,z);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    setkey4f

    @input
    i (KeyIndex)
    f (Time)
    f (Value0)
    f (Value1)
    f (Value2)
    f (Value3)

    @output
    v

    @info
    Defines a 4-dimensional keyframe.
*/
static void n_setkey4f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        int i   = cmd->In()->GetI();
        float t = cmd->In()->GetF();
        float x = cmd->In()->GetF();
        float y = cmd->In()->GetF();
        float z = cmd->In()->GetF();
        float w = cmd->In()->GetF();
        self->SetKey4f(i,t,x,y,z,w);
    }
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
    Closes a with 'beginkeys' started key definition.
*/
static void n_endkeys(void *o, nCmd *)
{
    nIpol *self = (nIpol *) o;
    self->EndKeys();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkeyinfo

    @input
    v

    @output
    i (NumKeys), i (KeySize)

    @info
    Returns the number of keys ('NumKeys') and their
    dimension ('KeySize') in the KeyArray.
*/
static void n_getkeyinfo(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        int n, s;
        self->GetKeyInfo(n,s);
        cmd->Out()->SetI(n);
        cmd->Out()->SetI(s);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkey1f

    @input
    i (KeyIndex)

    @output
    f (Time), f (Value0)

    @info
    Returns the 1-dimensional content of the keyframe with
    index 'KeyIndex'.
*/
static void n_getkey1f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        float t,x;
        int i = cmd->In()->GetI();
        self->GetKey1f(i,t,x);
        cmd->Out()->SetF(t);
        cmd->Out()->SetF(x);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkey2f

    @input
    i (KeyIndex)

    @output
    f (Time), f (Value0), f (Value1)

    @info
    Returns the 2-dimensional content of the keyframe with
    index 'KeyIndex'.
*/
static void n_getkey2f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        float t,x,y;
        int i = cmd->In()->GetI();
        self->GetKey2f(i,t,x,y);
        cmd->Out()->SetF(t);
        cmd->Out()->SetF(x);
        cmd->Out()->SetF(y);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkey3f

    @input
    i (KeyIndex)

    @output
    f (Time), f (Value0), f (Value1), f (Value2)

    @info
    Returns the 3-dimensional content of the keyframe with
    index 'KeyIndex'.
*/
static void n_getkey3f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        float t,x,y,z;
        int i = cmd->In()->GetI();
        self->GetKey3f(i,t,x,y,z);
        cmd->Out()->SetF(t);
        cmd->Out()->SetF(x);
        cmd->Out()->SetF(y);
        cmd->Out()->SetF(z);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkey4f

    @input
    i (KeyIndex)

    @output
    f (Time), f (Value0), f (Value1), f (Value2), f (Value3)

    @info
    Returns the 4-dimensional content of the keyframe with
    index 'KeyIndex'.
*/
static void n_getkey4f(void *o, nCmd *cmd)
{
    nIpol *self = (nIpol *) o;
    {
        float t,x,y,z,w;
        int i = cmd->In()->GetI();
        self->GetKey4f(i,t,x,y,z,w);
        cmd->Out()->SetF(t);
        cmd->Out()->SetF(x);
        cmd->Out()->SetF(y);
        cmd->Out()->SetF(z);
        cmd->Out()->SetF(w);
    }
}

//-------------------------------------------------------------------
//  SaveCmds()
//  02-Jan-99   floh    machine generated
//  07-Dec-00   floh    + setipoltype
//-------------------------------------------------------------------
bool nIpol::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nAnimNode::SaveCmds(fs)) {
        nCmd *cmd;

        //--- connect ---
        if (this->cmd_proto) {
            cmd = fs->GetCmd(this,'CNCT');
            cmd->In()->SetS(this->cmd_proto->GetName());
            fs->PutCmd(cmd);
        }

        //--- setipoltype ---
        nIpolType t = this->GetIpolType();
        if (t != N_LINEAR) {
            cmd = fs->GetCmd(this,'SIPT');
            const char *s;
            switch (t) {
                case N_STEP:        s="step"; break;
                case N_QUATERNION:  s="quaternion"; break;
                case N_CUBIC:       s="cubic"; break;
                case N_SPLINE:      s="spline"; break;
                case N_LINEAR:      
                default:            s="linear"; break;
            }
            cmd->In()->SetS(s);
            fs->PutCmd(cmd);
        }

        // key definitions...
        if (this->keyArray.Size() > 0) 
        {
            //--- beginkeys ---
            cmd = fs->GetCmd(this, 'BGKS');
            cmd->In()->SetI(this->keyArray.Size());
            cmd->In()->SetI(this->key_size);
            fs->PutCmd(cmd);

            //--- die Keys themselves ---
            int i;
            for (i=0; i < this->keyArray.Size(); i++) 
            {
                nIpolKey& k = this->keyArray.At(i);
                ulong id;
                switch (this->key_size) {
                    case 1: id='SK1F'; break;
                    case 2: id='SK2F'; break;
                    case 3: id='SK3F'; break;
                    case 4: id='SK4F'; break;
                    default: id=0; break;
                }
                if (id) 
                {
                    cmd = fs->GetCmd(this, id);
                    cmd->In()->SetI(i);
                    cmd->In()->SetF(k.t);
                    if (this->key_size>0) cmd->In()->SetF(k.f.x);
                    if (this->key_size>1) cmd->In()->SetF(k.f.y);
                    if (this->key_size>2) cmd->In()->SetF(k.f.z);
                    if (this->key_size>3) cmd->In()->SetF(k.f.w);
                    fs->PutCmd(cmd);
                }
            }

            //--- endkeys ---
            cmd = fs->GetCmd(this,'ENKS');
            fs->PutCmd(cmd);
        }
        retval = true;
    }
    return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
