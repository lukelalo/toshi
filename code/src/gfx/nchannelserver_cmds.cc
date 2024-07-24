#define N_IMPLEMENTS nChannelServer
//------------------------------------------------------------------------------
//  nchannelserver_cmds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/nchannelserver.h"

static void n_setchannel1f(void*, nCmd*);
static void n_setchannel2f(void*, nCmd*);
static void n_setchannel3f(void*, nCmd*);
static void n_setchannel4f(void*, nCmd*);
static void n_setchannelstring(void*, nCmd*);
static void n_getchannel1f(void*, nCmd*);
static void n_getchannel2f(void*, nCmd*);
static void n_getchannel3f(void*, nCmd*);
static void n_getchannel4f(void*, nCmd*);
static void n_getchannelstring(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nchannelserver

    @superclass
    nroot

    @classinfo
    Manages the animation channels.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setchannel1f_sf",    'SCH1', n_setchannel1f);
    cl->AddCmd("v_setchannel2f_sff",   'SCH2', n_setchannel2f);
    cl->AddCmd("v_setchannel3f_sfff",  'SCH3', n_setchannel3f);
    cl->AddCmd("v_setchannel4f_sffff", 'SCH4', n_setchannel4f);
    cl->AddCmd("v_setchannelstring_ss",'SCHS', n_setchannelstring);
    cl->AddCmd("f_getchannel1f_s",     'GCH1', n_getchannel1f);
    cl->AddCmd("ff_getchannel2f_s",    'GCH2', n_getchannel2f);
    cl->AddCmd("fff_getchannel3f_s",   'GCH3', n_getchannel3f);
    cl->AddCmd("ffff_getchannel4f_s",  'GCH4', n_getchannel4f);
    cl->AddCmd("s_getchannelstring_s", 'GCHS', n_getchannelstring);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setchannel1f

    @input
    s (ChannelName), f (Value)

    @output
    v

    @info
    Set channel to a float value.
*/
static void
n_setchannel1f(void* slf, nCmd* cmd)
{
    nChannelServer* self = (nChannelServer*) slf;
    int chnIndex = self->GenChannel(cmd->In()->GetS());
    float f0 = cmd->In()->GetF();
    self->SetChannel1f(chnIndex, f0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setchannel2f

    @input
    s(ChannelName), f(Val0), f(Val1)

    @output
    v

    @info
    Set channel to a 2d float value.
*/
static void
n_setchannel2f(void* slf, nCmd* cmd)
{
    nChannelServer* self = (nChannelServer*) slf;
    int chnIndex = self->GenChannel(cmd->In()->GetS());
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    self->SetChannel2f(chnIndex, f0, f1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setchannel3f

    @input
    s(ChannelName), f(Val0), f(Val1), f(Val2)

    @output
    v

    @info
    Set channel to a 3d float value.
*/
static void
n_setchannel3f(void* slf, nCmd* cmd)
{
    nChannelServer* self = (nChannelServer*) slf;
    int chnIndex = self->GenChannel(cmd->In()->GetS());
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    self->SetChannel3f(chnIndex, f0, f1, f2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setchannel4f

    @input
    s(ChannelName), f(Val0), f(Val1), f(Val2), f(Val3)

    @output
    v

    @info
    Set channel to a 4d float value.
*/
static void
n_setchannel4f(void* slf, nCmd* cmd)
{
    nChannelServer* self = (nChannelServer*) slf;
    int chnIndex = self->GenChannel(cmd->In()->GetS());
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    float f4 = cmd->In()->GetF();
    self->SetChannel4f(chnIndex, f0, f1, f2, f4);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setchannelstring
    @input
    s(ChannelName), s(ChannelContents)
    @output
    v
    @info
    Set channel to a string.
*/
static void
n_setchannelstring(void* slf, nCmd* cmd)
{
    nChannelServer* self = (nChannelServer*) slf;
    int chnIndex = self->GenChannel(cmd->In()->GetS());
    self->SetChannelStringCopy(chnIndex, cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getchannel1f

    @input
    s(ChannelName)

    @output
    f(Value)

    @info
    Get channel as float value.
*/
static void
n_getchannel1f(void* slf, nCmd* cmd)
{
    nChannelServer* self = (nChannelServer*) slf;
    int chnIndex = self->GenChannel(cmd->In()->GetS());
    float f0;
    f0 = self->GetChannel1f(chnIndex);
    cmd->Out()->SetF(f0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getchannel2f

    @input
    s(ChannelName)

    @output
    f(Val0), f(Val1)

    @info
    Get channel as 2d float value.
*/
static void
n_getchannel2f(void* slf, nCmd* cmd)
{
    nChannelServer* self = (nChannelServer*) slf;
    int chnIndex = self->GenChannel(cmd->In()->GetS());
    float f0, f1;
    self->GetChannel2f(chnIndex, f0, f1);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getchannel3f

    @input
    s(ChannelName)

    @output
    f(Val0), f(Val1), f(Val2)

    @info
    Get channel as 3d float value.
*/
static void
n_getchannel3f(void* slf, nCmd* cmd)
{
    nChannelServer* self = (nChannelServer*) slf;
    int chnIndex = self->GenChannel(cmd->In()->GetS());
    float f0, f1, f2;
    self->GetChannel3f(chnIndex, f0, f1, f2);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
    cmd->Out()->SetF(f2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getchannel4f

    @input
    s(ChannelName)

    @output
    f(Val0), f(Val1), f(Val2), f(Val3)

    @info
    Get channel as 4d float value.
*/
static void
n_getchannel4f(void* slf, nCmd* cmd)
{
    nChannelServer* self = (nChannelServer*) slf;
    int chnIndex = self->GenChannel(cmd->In()->GetS());
    float f0, f1, f2, f3;
    self->GetChannel4f(chnIndex, f0, f1, f2, f3);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
    cmd->Out()->SetF(f2);
    cmd->Out()->SetF(f3);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getchannelstring
    @input
    s(ChannelName)
    @output
    s(ChannelContent)
    @info
    Get channel content as string.
*/
static void
n_getchannelstring(void* slf, nCmd* cmd)
{
    nChannelServer* self = (nChannelServer*) slf;
    int chnIndex = self->GenChannel(cmd->In()->GetS());
    cmd->Out()->SetS(self->GetChannelString(chnIndex));
}

//------------------------------------------------------------------------------
