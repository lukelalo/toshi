#define N_IMPLEMENTS nAudioServer
//-------------------------------------------------------------------
//  naudio_cmds.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "audio/naudioserver.h"

static void n_setmode(void *, nCmd *);
static void n_getmode(void *, nCmd *);
static void n_openaudio(void *, nCmd *);
static void n_closeaudio(void *, nCmd *);
static void n_setmastervolume(void *, nCmd *);
static void n_getmastervolume(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    naudioserver

    @superclass
    nroot

    @classinfo
    Superclass of all audio server classes. The audio server
    lives under the name '/sys/servers/audio'.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setmode_s",           'SMOD', n_setmode);
    cl->AddCmd("s_getmode_v",           'GMOD', n_getmode);
    cl->AddCmd("b_openaudio_v",         'OPNA', n_openaudio);
    cl->AddCmd("v_closeaudio_v",        'CLSA', n_closeaudio);
    cl->AddCmd("v_setmastervolume_f",   'SMVL', n_setmastervolume);
    cl->AddCmd("f_getmastervolume_v",   'GMVL', n_getmastervolume);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmode

    @input
    s (ModeDescriptor)

    @output
    v

    @info
    Configure audio output. Will not take effect until
    audio is (re-)opened. The mode descriptor string contains 
    tag-value pairs, all of them are optional:
    dev(id) 
        Define the output device to be used, the default output
        device is '0', if there are more then one sound output device
        in the system, the other devices are number 1..n. The valid
        audio playback devices can be found under '/sys/share/audio'.
    bps(bits_per_sample)
        Define format of a sample (bits per sample). Default is 16.
    chn(num_channels)
        Set the number of channels that can be played back 
        simultaneously. Default is 16.
    hz(sample_rate)
        Define the playback sample rate in Hz. Default is 22000.
    speakers(head|mono|stereo|quad|surround)
        Define the speaker configuration.
*/
static void n_setmode(void *o, nCmd *cmd)
{
    nAudioServer *self = (nAudioServer *) o;
    self->SetMode(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmode

    @input
    v

    @output
    s (ModeDescriptor)

    @info
    Get current mode descriptor string.
*/
static void n_getmode(void *o, nCmd *cmd)
{
    nAudioServer *self = (nAudioServer *) o;
    cmd->Out()->SetS(self->GetMode());
}

//------------------------------------------------------------------------------
/**
    @cmd
    openaudio

    @input
    v

    @output
    v

    @info
    Open the audio device in the current configuration (which has
    been defined by the last 'setmode').
*/
static void n_openaudio(void *o, nCmd *cmd)
{
    nAudioServer *self = (nAudioServer *) o;
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
    Close audio device.
*/
static void n_closeaudio(void *o, nCmd *)
{
    nAudioServer *self = (nAudioServer *) o;
    self->CloseAudio();
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
    Set the current master volume in the range 0.0f to 1.0f.
*/
static void n_setmastervolume(void *o, nCmd *cmd)
{
    nAudioServer *self = (nAudioServer *) o;
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
    Get the current master volume.
*/
static void n_getmastervolume(void *o, nCmd *cmd)
{
    nAudioServer *self = (nAudioServer *) o;
    cmd->Out()->SetF(self->GetMasterVolume());
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
