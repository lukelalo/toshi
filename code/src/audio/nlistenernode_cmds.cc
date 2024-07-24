#define N_IMPLEMENTS nListenerNode
//-------------------------------------------------------------------
//  nlistenernode_cmds.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "audio/nlistenernode.h"

static void n_setvelocitychannel(void*, nCmd*);
static void n_getvelocitychannel(void*, nCmd*);
static void n_setdopplerfactor(void *, nCmd *);
static void n_getdopplerfactor(void *, nCmd *);
static void n_setrollofffactor(void *, nCmd *);
static void n_getrollofffactor(void *, nCmd *);
static void n_setvelocitycap(void*, nCmd*);
static void n_getvelocitycap(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nlistenernode

    @superclass
    nvisnode

    @classinfo
    Define an audio listener in 3d space.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setvelocitychannel_s",    'SVCH', n_setvelocitychannel);
    cl->AddCmd("s_getvelocitychannel_v",    'GVCH', n_getvelocitychannel);
    cl->AddCmd("v_setdopplerfactor_f",      'SDPF', n_setdopplerfactor);
    cl->AddCmd("f_getdopplerfactor_v",      'GDPF', n_getdopplerfactor);
    cl->AddCmd("v_setrollofffactor_f",      'SRLF', n_setrollofffactor);
    cl->AddCmd("f_getrollofffactor_v",      'GRLF', n_getrollofffactor);
    cl->AddCmd("v_setvelocitycap_f",        'SVCP', n_setvelocitycap);
    cl->AddCmd("f_getvelocitycap_v",        'GVCP', n_getvelocitycap);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvelocitychannel
    
    @input
    s (ChannelName)
    
    @output
    v
    
    @info
    Set name of 3D-channel which delivers the listeners velocity
    in global space. The default name is "lvelocity".
*/
static void n_setvelocitychannel(void* slf, nCmd* cmd)
{
    nListenerNode* self = (nListenerNode*) slf;
    self->SetVelocityChannel(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvelocitychannel
    
    @input
    v
    
    @output
    s (ChannelName)
    
    @info
    Get name of channel which contains listeners velocity.
*/
static void n_getvelocitychannel(void* slf, nCmd* cmd)
{
    nListenerNode* self = (nListenerNode*) slf;
    cmd->Out()->SetS(self->GetVelocityChannel());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setdopplerfactor

    @input
    f (DopplerFactor)

    @output
    v

    @info
    Set the doppler factor.
*/
static void n_setdopplerfactor(void* slf, nCmd* cmd)
{
    nListenerNode* self = (nListenerNode*) slf;
    self->SetDopplerFactor(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getdopplerfactor

    @input
    v

    @output
    f (DopplerFactor)

    @info
    Get the doppler factor.
*/
static void n_getdopplerfactor(void *o, nCmd *cmd)
{
    nListenerNode *self = (nListenerNode *) o;
    cmd->Out()->SetF(self->GetDopplerFactor());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setrollofffactor

    @input
    f (RolloffFactor)

    @output
    v

    @info
    Set the rolloff factor.
*/
static void n_setrollofffactor(void *o, nCmd *cmd)
{
    nListenerNode *self = (nListenerNode *) o;
    self->SetRolloffFactor(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrollofffactor

    @input
    v

    @output
    f (RolloffFactor)

    @info
    Get the rolloff factor.
*/
static void n_getrollofffactor(void *o, nCmd *cmd)
{
    nListenerNode *self = (nListenerNode *) o;
    cmd->Out()->SetF(self->GetRolloffFactor());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvelocitycap
    
    @input
    f (VelocityCap)
    
    @output
    v
    
    @info
    Set a maximum value against which listener velocity will be capped.
*/
static
void
n_setvelocitycap(void* slf, nCmd* cmd)
{
    nListenerNode* self = (nListenerNode*) slf;
    self->SetVelocityCap(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvelocitycap
    
    @input
    v
    
    @output
    f (VelocityCap)

    @info
    Get the velicity cap value.
*/
static
void
n_getvelocitycap(void* slf, nCmd* cmd)
{
    nListenerNode* self = (nListenerNode*) slf;
    cmd->Out()->SetF(self->GetVelocityCap());
}

//------------------------------------------------------------------------------
/**
*/
bool 
nListenerNode::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) 
    {
        nCmd *cmd;

        //--- setvelocitychannel ---
        cmd = fs->GetCmd(this, 'SVCH');
        cmd->In()->SetS(this->GetVelocityChannel());
        fs->PutCmd(cmd);

        //--- setdopplerfactor ---
        cmd = fs->GetCmd(this, 'SDPF');
        cmd->In()->SetF(this->GetDopplerFactor());
        fs->PutCmd(cmd);

        //--- setrollofffactor ---
        cmd = fs->GetCmd(this, 'SRLF');
        cmd->In()->SetF(this->GetRolloffFactor());
        fs->PutCmd(cmd);

        //--- setvelocitycap ---
        cmd = fs->GetCmd(this, 'SVCP');
        cmd->In()->SetF(this->GetVelocityCap());
        fs->PutCmd(cmd);

        retval = true;
    }
    return retval;
}


//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
