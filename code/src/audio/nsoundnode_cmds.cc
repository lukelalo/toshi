#define N_IMPLEMENTS nSoundNode
//------------------------------------------------------------------------------
//  nsoundnode_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "audio/nsoundnode.h"

static void n_setvelocitychannel(void*, nCmd*);
static void n_getvelocitychannel(void*, nCmd*);
static void n_settriggerchannel(void*, nCmd*);
static void n_gettriggerchannel(void*, nCmd*);
static void n_setfile(void*, nCmd*);
static void n_getfile(void*, nCmd*);
static void n_setsoundcue(void*, nCmd*);
static void n_getsoundcue(void*, nCmd*);
static void n_setambient(void*, nCmd*);
static void n_getambient(void*, nCmd*);
static void n_setdoppler(void*, nCmd*);
static void n_getdoppler(void*, nCmd*);
static void n_setlooping(void*, nCmd*);
static void n_getlooping(void*, nCmd*);
static void n_setpriority(void*, nCmd*);
static void n_getpriority(void*, nCmd*);
static void n_setvolume(void*, nCmd*);
static void n_getvolume(void*, nCmd*);
static void n_setfrequency(void*, nCmd*);
static void n_getfrequency(void*, nCmd*);
static void n_setminmaxdist(void*, nCmd*);
static void n_getminmaxdist(void*, nCmd*);
static void n_start(void*, nCmd*);
static void n_stop(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nsoundnode

    @superclass
    nvisnode

    @classinfo
    Attaches a sound to the scene graph.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setvelocitychannel_s", 'SVCH', n_setvelocitychannel);
    cl->AddCmd("s_getvelocitychannel_v", 'GVCH', n_getvelocitychannel);
    cl->AddCmd("v_settriggerchannel_s",  'STCH', n_settriggerchannel);
    cl->AddCmd("s_gettriggerchannel_v",  'GTCH', n_gettriggerchannel);
    cl->AddCmd("v_setfile_s",            'SFIL', n_setfile);
    cl->AddCmd("s_getfile_v",            'GFIL', n_getfile);
    cl->AddCmd("v_setsoundcue_s",        'SCUE', n_setsoundcue);
    cl->AddCmd("s_getsoundcue_v",        'GCUE', n_getsoundcue);
    cl->AddCmd("v_setambient_b",         'SAMB', n_setambient);
    cl->AddCmd("b_getambient_v",         'GAMB', n_getambient);
    cl->AddCmd("v_setdoppler_b",         'SDOP', n_setdoppler);
    cl->AddCmd("b_getdoppler_v",         'GDOP', n_getdoppler);
    cl->AddCmd("v_setlooping_b",         'SLOP', n_setlooping);
    cl->AddCmd("b_getlooping_v",         'GLOP', n_getlooping);
    cl->AddCmd("v_setpriority_f",        'SPRI', n_setpriority);
    cl->AddCmd("f_getpriority_v",        'GPRI', n_getpriority);
    cl->AddCmd("v_setvolume_f",          'SVOL', n_setvolume);
    cl->AddCmd("f_getvolume_v",          'GVOL', n_getvolume);
    cl->AddCmd("v_setfrequency_f",       'SFRQ', n_setfrequency);
    cl->AddCmd("f_getfrequency_v",       'GFRQ', n_getfrequency);
    cl->AddCmd("v_setminmaxdist_ff",     'SMMD', n_setminmaxdist);
    cl->AddCmd("ff_getminmaxdist_v",     'GMMD', n_getminmaxdist);
    cl->AddCmd("v_start_v",              'STRT', n_start);
    cl->AddCmd("v_stop_v",               'STOP', n_stop);
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
    Set name of 3D-channel which delivers the sound node's velocity
    in global space. The default name is "velocity".
*/
static
void
n_setvelocitychannel(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
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
    Get name of velocity channel.
*/
static
void
n_getvelocitychannel(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    cmd->Out()->SetS(self->GetVelocityChannel());
}

//------------------------------------------------------------------------------
/**
    @cmd
    settriggerchannel

    @input
    s (ChannelName)

    @output
    v

    @info
    Set name of a trigger channel which starts the sound. The trigger
    channel must go to a value of 1.0 for exactly one frame when the
    sound should be started, and to 0.0 otherwise. The default
    name is "soundtrigger".
*/
static
void
n_settriggerchannel(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    self->SetTriggerChannel(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettriggerchannel

    @input
    v

    @output
    s (ChannelName)

    @info
    Get the name of the trigger channel.
*/
static
void
n_gettriggerchannel(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    cmd->Out()->SetS(self->GetTriggerChannel());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfile

    @input
    s (Filename)

    @output
    v

    @info
    Set name of stream file.
*/
static
void
n_setfile(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    self->SetFile(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfile

    @input
    v

    @output
    s (Filename)

    @info
    Get name of stream file.
*/
static
void
n_getfile(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    cmd->Out()->SetS(self->GetFile());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setsoundcue

    @input
    s (SoundCueName)

    @output
    v

    @info
    Set sound cue name.
*/
static
void
n_setsoundcue(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    self->SetSoundCue(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getsoundcue

    @input
    v

    @output
    s (SoundCueName)

    @info
    Get name of sound cue.
*/
static
void
n_getsoundcue(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    cmd->Out()->SetS(self->GetSoundCue());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setambient

    @input
    b (AmbienceFlag)

    @output
    v

    @info
    Set the ambient flag on the sound source. An ambient sound source
    plays in the listeners head.
*/
static
void
n_setambient(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    self->SetAmbient(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getambient

    @input
    b (AmbienceFlag)

    @output
    v

    @info
    Get the ambient flag.
*/
static
void
n_getambient(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    cmd->Out()->SetB(self->GetAmbient());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setdoppler

    @input
    b (DopplerFlag)

    @output
    v

    @info
    Enable/disable the doppler effect for this sound source.
*/
static
void
n_setdoppler(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    self->SetDoppler(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getdoppler

    @input
    v

    @output
    b (DopplerFlag)

    @info
    Get the doppler flag for this sound source.
*/
static
void
n_getdoppler(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    cmd->Out()->SetB(self->GetDoppler());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlooping

    @input
    b (LoopingFlag)

    @output
    v

    @info
    Set the looping flag.
*/
static
void
n_setlooping(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    self->SetLooping(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlooping

    @input
    v

    @output
    b (LoopingFlag)

    @info
    Get the looping flag.
*/
static
void
n_getlooping(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    cmd->Out()->SetB(self->GetLooping());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setpriority

    @input
    f(PriorityFactor)

    @output
    v

    @info
    Set the priority factor. Default is one. The resulting priority
    is computed from the distance, volume and priority factor of
    the sound source.
*/
static
void
n_setpriority(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    self->SetPriority(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getpriority

    @input
    v

    @output
    f(PriorityFactor)

    @info
    Get the priority factor.
*/
static
void
n_getpriority(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    cmd->Out()->SetF(self->GetPriority());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvolume

    @input
    f(Volume)

    @output
    v

    @info
    Set the volume (0.0 .. 1.0)
*/
static
void
n_setvolume(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    self->SetVolume(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvolume

    @input
    v

    @output
    f(Volume)

    @info
    Get the volume.
*/
static
void
n_getvolume(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    cmd->Out()->SetF(self->GetVolume());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfrequency

    @input
    f(Frequency)

    @output
    v

    @info
    Set the frequency multiplier (resulting frequency is base frequency
    of sample * frequency multiplier)
*/
static
void
n_setfrequency(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    self->SetFrequency(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfrequency

    @input
    v

    @output
    f(Frequency)

    @info
    Get the frequency multiplier.
*/
static
void
n_getfrequency(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    cmd->Out()->SetF(self->GetFrequency());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setminmaxdist

    @input
    f(MinDist), f(MaxDist)

    @output
    v

    @info
    Set the minimum and maximum distance of the sound source.
*/
static
void
n_setminmaxdist(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    self->SetMinMaxDist(f0, f1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getminmaxdist

    @input
    v

    @output
    f(MinDist), f(MaxDist)

    @info
    Get the min and max distance of the sound source.
*/
static
void
n_getminmaxdist(void* slf, nCmd* cmd)
{
    nSoundNode* self = (nSoundNode*) slf;
    float f0, f1;
    self->GetMinMaxDist(f0, f1);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    start

    @input
    v

    @output
    v

    @info
    Start the sound source. This is an alternative way to start sound playback
    (alternative to the trigger channel).
*/
static
void
n_start(void* slf, nCmd* /*cmd*/)
{
    nSoundNode* self = (nSoundNode*) slf;
    self->Start();
}

//------------------------------------------------------------------------------
/**
    @cmd
    stop

    @input
    v

    @output
    v

    @info
    Stop the sound source. Please note the oneshot sounds stopped automatically,
    but not loop sounds.
*/
static
void
n_stop(void* slf, nCmd* /*cmd*/)
{
    nSoundNode* self = (nSoundNode*) slf;
    self->Stop();
}

//------------------------------------------------------------------------------
/*
*/
bool
nSoundNode::SaveCmds(nPersistServer* fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs))
    {
        nCmd* cmd;
        float f0, f1;

        //--- setvelocitychannel ---
        if (strcmp(this->GetVelocityChannel(), "velocity") != 0)
        {
            cmd = fs->GetCmd(this, 'SVCH');
            cmd->In()->SetS(this->GetVelocityChannel());
            fs->PutCmd(cmd);
        }

        //--- settriggerchannel ---
        if (strcmp(this->GetTriggerChannel(), "soundtrigger") != 0)
        {
            cmd = fs->GetCmd(this, 'STCH');
            cmd->In()->SetS(this->GetTriggerChannel());
            fs->PutCmd(cmd);
        }

        //--- setfile ---
        cmd = fs->GetCmd(this, 'SFIL');
        cmd->In()->SetS(this->GetFile());
        fs->PutCmd(cmd);

        //--- setsoundcue ---
        if (this->GetSoundCue())
        {
            cmd = fs->GetCmd(this, 'SCUE');
            cmd->In()->SetS(this->GetSoundCue());
            fs->PutCmd(cmd);
        }

        //--- setambient ---
        if (this->GetAmbient())
        {
            cmd = fs->GetCmd(this, 'SAMB');
            cmd->In()->SetB(this->GetAmbient());
            fs->PutCmd(cmd);
        }

        //--- setdoppler ---
        if (!this->GetDoppler())
        {
            cmd = fs->GetCmd(this, 'SDOP');
            cmd->In()->SetB(this->GetDoppler());
            fs->PutCmd(cmd);
        }

        //--- setlooping ---
        if (this->GetLooping())
        {
            cmd = fs->GetCmd(this, 'SLOP');
            cmd->In()->SetB(this->GetLooping());
            fs->PutCmd(cmd);
        }

        //--- setpriority ---
        if (1.0f != this->GetPriority())
        {
            cmd = fs->GetCmd(this, 'SPRI');
            cmd->In()->SetF(this->GetPriority());
            fs->PutCmd(cmd);
        }

        //--- setvolume ---
        if (1.0f != this->GetVolume())
        {
            cmd = fs->GetCmd(this, 'SVOL');
            cmd->In()->SetF(this->GetVolume());
            fs->PutCmd(cmd);
        }

        //--- setfrequency ---
        if (1.0f != this->GetFrequency())
        {
            cmd = fs->GetCmd(this, 'SFRQ');
            cmd->In()->SetF(this->GetFrequency());
            fs->PutCmd(cmd);
        }

        //--- setminmaxdist ---
        this->GetMinMaxDist(f0, f1);
        cmd = fs->GetCmd(this, 'SMMD');
        cmd->In()->SetF(f0);
        cmd->In()->SetF(f1);
        fs->PutCmd(cmd);

        retval = true;
    }
    return retval;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
