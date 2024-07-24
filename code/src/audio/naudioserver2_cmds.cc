#define N_IMPLEMENTS nAudioServer2
//------------------------------------------------------------------------------
//  naudioserver2_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio/naudioserver2.h"
#include "kernel/npersistserver.h"

static void n_setmode(void* slf, nCmd* cmd);
static void n_getmode(void* slf, nCmd* cmd);
static void n_setmastervolume(void* slf, nCmd* cmd);
static void n_getmastervolume(void* slf, nCmd* cmd);
static void n_openaudio(void* slf, nCmd* cmd);
static void n_closeaudio(void* slf, nCmd* cmd);
static void n_flushaudio(void* slf, nCmd* cmd);
static void n_muteaudio(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    naudioserver2
    
    @superclass
    nroot

    @classinfo
    Base server class of the audio2 subsystem.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setmode_s",           'SMOD', n_setmode);
    cl->AddCmd("s_getmode_v",           'GMOD', n_getmode);
    cl->AddCmd("v_setmastervolume_f",   'SMSV', n_setmastervolume);
    cl->AddCmd("f_getmastervolume_v",   'GMSV', n_getmastervolume);
    cl->AddCmd("b_openaudio_v",         'OPAD', n_openaudio);
    cl->AddCmd("v_closeaudio_v",        'CLAD', n_closeaudio);
    cl->AddCmd("v_flushaudio_ff",       'FLAD', n_flushaudio);
    cl->AddCmd("v_muteaudio_b",         'MUAD', n_muteaudio);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmode
    @input
    s (ModeString)
    @output
    v
    @info
    Set audio initialization parameters as a mode string.
*/
static
void
n_setmode(void* slf, nCmd* cmd)
{
    nAudioServer2* self = (nAudioServer2*) slf;
    self->SetMode(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmode
    @input
    v
    @output
    s (ModeString)
    @info
    Get audio initialization parameters as a mode string.
*/
static
void
n_getmode(void* slf, nCmd* cmd)
{
    nAudioServer2* self = (nAudioServer2*) slf;
    cmd->Out()->SetS(self->GetMode());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmastervolume
    @input
    f (MasterVolume)
    @output
    v
    @info
    Set the new master volume (0.0 .. 1.0)
*/
static
void
n_setmastervolume(void* slf, nCmd* cmd)
{
    nAudioServer2* self = (nAudioServer2*) slf;
    self->SetMasterVolume(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmastervolume
    @input
    v
    @output
    f (MasterVolume)
    @info
    Get the new master volume (0.0 .. 1.0)
*/
static
void
n_getmastervolume(void* slf, nCmd* cmd)
{
    nAudioServer2* self = (nAudioServer2*) slf;
    cmd->Out()->SetF(self->GetMasterVolume());
}

//------------------------------------------------------------------------------
/**
    @cmd
    openaudio
    @input
    v
    @output
    b (Success)
    @info
    Open the audio device.
*/
static
void
n_openaudio(void* slf, nCmd* cmd)
{
    nAudioServer2* self = (nAudioServer2*) slf;
    cmd->Out()->SetB(self->OpenAudio());
}

//------------------------------------------------------------------------------
/**
    @cmd
    closeaudio
    @input
    v
    @output
    v
    @info
    Close the audio device.
*/
static
void
n_closeaudio(void* slf, nCmd* /*cmd*/)
{
    nAudioServer2* self = (nAudioServer2*) slf;
    self->CloseAudio();
}

//------------------------------------------------------------------------------
/**
    @cmd
    flushaudio
    @input
    f(FadeOutTime), f(FadeInTime)
    @output
    v
    @info
    Flush the audio queue. Master volume will fade out, the audio queue
    will be flushed, master volume will fade in. Flushing audio will undo
    a mute.
*/
static
void
n_flushaudio(void* slf, nCmd* cmd)
{
    nAudioServer2* self = (nAudioServer2*) slf;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    self->FlushAudio(f0, f1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    muteaudio
    @input
    b(MuteFlag)
    @output
    v
    @info
    Immediately mute/unmute audio.
*/
static
void
n_muteaudio(void* slf, nCmd* cmd)
{
    nAudioServer2* self = (nAudioServer2*) slf;
    self->MuteAudio(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
