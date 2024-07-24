#define N_IMPLEMENTS nAnimNode
//-------------------------------------------------------------------
//  node/nanim_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/nanimnode.h"

static void n_setreptype(void *, nCmd *);
static void n_getreptype(void *, nCmd *);
static void n_setchannel(void *, nCmd *);
static void n_getchannel(void *, nCmd *);
static void n_setscale(void *, nCmd *);
static void n_getscale(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nanimnode

    @superclass
    nvisnode

    @classinfo
    The animnode class administers some attributes
    shared by all animating node classes which these
    are then logically derive from nAnimNode.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setreptype_s",'SRPT',n_setreptype);
    cl->AddCmd("s_getreptype_v",'GRPT',n_getreptype);
    cl->AddCmd("v_setchannel_s",'SCHN',n_setchannel);
    cl->AddCmd("s_getchannel_v",'GCHN',n_getchannel);
    cl->AddCmd("v_setscale_f",'SSCL',n_setscale);
    cl->AddCmd("f_getscale_v",'GSCL',n_getscale);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setreptype

    @input
    s (RepType = [oneshot loop]

    @output
    v

    @info
    Sets the loop mode for the object, at the moment
    there is 'oneshot' and 'loop' defined.
*/
static void n_setreptype(void *o, nCmd *cmd)
{
    nAnimNode *self = (nAnimNode *) o;
    const char *rs = cmd->In()->GetS();
    if      (strcmp("oneshot",rs)==0) self->SetRepType(N_REPTYPE_ONESHOT);
    else if (strcmp("loop",rs)==0)    self->SetRepType(N_REPTYPE_LOOP);
    else {
        n_printf("reptype not valid, must be [oneshot loop]\n");
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    getreptype

    @input
    v

    @output
    s (RepType = [oneshot loop]

    @info
    Returns the actual RepType of the object.
*/
static void n_getreptype(void *o, nCmd *cmd)
{
    nAnimNode *self = (nAnimNode *) o;
    nRepType rt = self->GetRepType();
    const char *rs;
    switch (rt) {
        case N_REPTYPE_ONESHOT: rs="oneshot"; break;
        case N_REPTYPE_LOOP:    rs="loop"; break;
        default:                rs="<error>"; break;
    }
    cmd->Out()->SetS(rs);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setchannel

    @input
    s (ChannelName)

    @output
    v

    @info
    Links the object to a global Anim-Channel.
    The defaultchannel is 'time'.
*/
static void n_setchannel(void *o, nCmd *cmd)
{
    nAnimNode *self = (nAnimNode *) o;
    self->SetChannel(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getchannel

    @input
    v

    @output
    s (ChannelName)

    @info
    Returns the channel the object is linked to.
*/
static void n_getchannel(void *o, nCmd *cmd)
{
    nAnimNode *self = (nAnimNode *) o;
    cmd->Out()->SetS(self->GetChannel());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setscale

    @input
    f (Scale)

    @output
    v

    @info
    Defines a scale value for multiplication of the time values
    in the keyframes before they are compared to the time.
*/
static void n_setscale(void *o, nCmd *cmd)
{
    nAnimNode *self = (nAnimNode *) o;
    self->SetScale(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getscale

    @input
    v

    @output
    f (Scale)

    @info
    Returns the with 'setscale' defined value.
*/
static void n_getscale(void *o, nCmd *cmd)
{
    nAnimNode *self = (nAnimNode *) o;
    cmd->Out()->SetF(self->GetScale());
}

//-------------------------------------------------------------------
//  SaveCmds()
//  02-Jan-99   floh    machine generated
//-------------------------------------------------------------------
bool nAnimNode::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;

        //--- setreptype ---
        cmd = fs->GetCmd(this, 'SRPT');
        switch (this->GetRepType()) {
            case N_REPTYPE_ONESHOT: cmd->In()->SetS("oneshot"); break;
            case N_REPTYPE_LOOP:    cmd->In()->SetS("loop"); break;
            default:                cmd->In()->SetS("<error>"); break;
        }
        fs->PutCmd(cmd);

        //--- setchannel ---
        cmd = fs->GetCmd(this, 'SCHN');
        cmd->In()->SetS(this->GetChannel());
        fs->PutCmd(cmd);

        //--- setscale ---
        cmd = fs->GetCmd(this, 'SSCL');
        cmd->In()->SetF(this->GetScale());
        fs->PutCmd(cmd);

        retval = true;
    }
    return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
