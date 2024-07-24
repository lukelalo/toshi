#define N_IMPLEMENTS nDSoundServer2
//------------------------------------------------------------------------------
//  ndsoundserver2_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio/ndsoundserver2.h"
#include "audio/ndsoundcluster.h"
#include "kernel/nenv.h"
#include "misc/nquickwatch.h"

nNebulaClass(nDSoundServer2, "naudioserver2");

extern const char *ndsound_Error(HRESULT);

//------------------------------------------------------------------------------
/**
*/
nDSoundServer2::nDSoundServer2() :
    refHwnd(kernelServer, this),
    dSound(0),
    dsPrim(0),
    dsListener(0),
    monoCluster(0),
    stereoCluster(0)
{    
    // the /sys/env/hwnd object has been provided by the gfx server!
    this->refHwnd = "/sys/env/hwnd";
}

//------------------------------------------------------------------------------
/**
*/
nDSoundServer2::~nDSoundServer2()
{
    if (this->audioOpen)
    {
        this->CloseAudio();
    }
}

//------------------------------------------------------------------------------
/**
    Private mastervolume manipulation, which only modifies DSound's 
    master volume, but not the master volume stored in the sound server object.
*/
void
nDSoundServer2::SetDSoundMasterVolume(float f)
{
    n_assert(this->dsPrim);
    LONG dsVol = (LONG) (float(DSBVOLUME_MIN) + (float(DSBVOLUME_MAX - DSBVOLUME_MIN) * f));
    this->dsPrim->SetVolume(dsVol);
}

//------------------------------------------------------------------------------
/**
    Set the master volume in the DirectSound primary buffer.
*/
void
nDSoundServer2::SetMasterVolume(float f)
{
    if (!this->audioOpen)
    {
        return;
    }
    n_printf("nDSoundServer2::SetMasterVolume(%f)\n", f);
    nAudioServer2::SetMasterVolume(f);
    this->SetDSoundMasterVolume(f * this->muteModulator);
}

//------------------------------------------------------------------------------
/**
    Initialize DirectSound, create primary buffer, listener and secondary
    buffers.
*/
bool
nDSoundServer2::OpenAudio()
{
    n_assert(!this->audioOpen);
    n_assert(0 == this->dSound);
    n_assert(0 == this->dsPrim);
    n_assert(0 == this->dsListener);
    n_assert(0 == this->monoCluster);
    n_assert(0 == this->stereoCluster);

    HRESULT hr;
    nAudioServer2::OpenAudio();

    // create DirectSound
    hr = DirectSoundCreate8(0, &(this->dSound), NULL);
    if (FAILED(hr))
    {
        n_printf("nDSoundServer2::OpenAudio(): DirectSoundCreate8() failed with '%s'\n", ndsound_Error(hr));
        return false;
    }

    // set cooperative level
    HWND hwnd = (HWND) this->refHwnd->GetI();
    hr = this->dSound->SetCooperativeLevel(hwnd, DSSCL_EXCLUSIVE);
    if (FAILED(hr))
    {
        n_printf("nDSoundServer2::OpenAudio(): SetCooperativeLevel() failed with '%s'\n", ndsound_Error(hr));
        this->CloseAudio();
        return false;
    }

    // set speaker config
    DWORD speakers;
    switch (this->modeSpeakerConfig)
    {
        case FIVEPOINTONE:  speakers = DSSPEAKER_5POINT1; break;
        case HEADPHONE:     speakers = DSSPEAKER_HEADPHONE; break;
        case MONO:          speakers = DSSPEAKER_MONO; break;
        case QUAD:          speakers = DSSPEAKER_QUAD; break;
        case STEREO:        speakers = DSSPEAKER_STEREO; break;
        case SURROUND:      speakers = DSSPEAKER_SURROUND; break;
        default:            speakers = DSSPEAKER_STEREO; break;
    }
    hr = this->dSound->SetSpeakerConfig(speakers);
    if (FAILED(hr))
    {
        n_printf("nDSoundServer2::OpenAudio(): SetSpeakerConfig() failed with '%s'\n", ndsound_Error(hr));
        this->CloseAudio();
        return false;
    }

    // create the primary buffer
    DSBUFFERDESC desc = { sizeof(DSBUFFERDESC), 0 };
    desc.dwFlags        = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
    hr = this->dSound->CreateSoundBuffer(&desc, &(this->dsPrim), 0);
    if (FAILED(hr))
    {
        n_printf("nDSoundServer2::OpenAudio(): Create primary buffer failed with '%s'\n", ndsound_Error(hr));
        this->CloseAudio();
        return false;
    }

    // set output format for the primary buffer
    WAVEFORMATEX waveFormat = { 0 };
    waveFormat.wFormatTag      = WAVE_FORMAT_PCM;
    waveFormat.nChannels       = 2;
    waveFormat.nSamplesPerSec  = this->modeSamplesPerSec;
    waveFormat.wBitsPerSample  = this->modeBitsPerSample;
    waveFormat.nBlockAlign     = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    hr = this->dsPrim->SetFormat(&waveFormat);
    if (FAILED(hr))
    {
        n_printf("nDSoundServer2::OpenAudio(): SetFormat() on primary buffer failed with '%s'\n", ndsound_Error(hr));
        this->CloseAudio();
        return false;
    }

    // query the 3d listener interface
    hr = this->dsPrim->QueryInterface(IID_IDirectSound3DListener8, (void**) &(this->dsListener));
    if (FAILED(hr))
    {
        n_printf("nDSoundServer2::OpenAudio(): QueryInterface() for listener failed with '%s'\n", ndsound_Error(hr));
        this->CloseAudio();
        return false;
    }

    // start playing the primary buffer
    this->dsPrim->Play(0, 0, DSBPLAY_LOOPING);

    // create the sound clusters
    this->monoCluster = new nDSoundCluster(this->modeChannels, 1, this->dSound, this->dsPrim);
    this->stereoCluster = new nDSoundCluster(4, 2, this->dSound, this->dsPrim);

    return true;
}

//------------------------------------------------------------------------------
/**
    Shutdown DirectSound.
*/
void
nDSoundServer2::CloseAudio()
{
    if (!this->audioOpen)
    {
        return;
    }

    // delete sound clusters
    if (this->monoCluster)
    {
        delete this->monoCluster;
        this->monoCluster = 0;
    }
    if (this->stereoCluster)
    {
        delete this->stereoCluster;
        this->stereoCluster = 0;
    }

    // release DirectSound interfaces
    if (this->dsListener)
    {
        this->dsListener->Release();
        this->dsListener = 0;
    }
    if (this->dsPrim)
    {
        this->dsPrim->Release();
        this->dsPrim = 0;
    }
    if (this->dSound)
    {
        this->dSound->Release();
        this->dSound = 0;
    }
    nAudioServer2::CloseAudio();
}

//------------------------------------------------------------------------------
/**
    Copy the listener attributes to the DirectSound listener.
*/
void
nDSoundServer2::SetListenerAttributes()
{
    n_assert(this->dsListener);

    HRESULT hr;
    DS3DLISTENER d3dListener = { sizeof(DS3DLISTENER), 0 };

    const matrix44& matrix  = this->listener.GetMatrix();
    const vector3& velocity = this->listener.GetVelocity();

    d3dListener.vPosition.x = matrix.M41;
    d3dListener.vPosition.y = matrix.M42;
    d3dListener.vPosition.z = matrix.M43;

    d3dListener.vVelocity.x = velocity.x;
    d3dListener.vVelocity.y = velocity.y;
    d3dListener.vVelocity.z = velocity.z;

    d3dListener.vOrientFront.x = -matrix.M31;
    d3dListener.vOrientFront.y = -matrix.M32;
    d3dListener.vOrientFront.z = -matrix.M33;

    d3dListener.vOrientTop.x = -matrix.M21;
    d3dListener.vOrientTop.y = -matrix.M22;
    d3dListener.vOrientTop.z = -matrix.M23;

    d3dListener.flDistanceFactor = 1.0f;
    d3dListener.flRolloffFactor  = this->listener.GetRolloffFactor();
    d3dListener.flDopplerFactor  = this->listener.GetDopplerFactor();

    hr = this->dsListener->SetAllParameters(&d3dListener, DS3D_DEFERRED);
    if (FAILED(hr))
    {
        n_printf("nDSoundServer2::SetListenerAttributes(): SetAllParameters() failed with '%s'\n", ndsound_Error(hr));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer2::BeginScene(double time)
{
    n_assert(this->monoCluster);
    n_assert(this->stereoCluster);

    nAudioServer2::BeginScene(time);

    this->monoCluster->Begin();
    this->stereoCluster->Begin();
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer2::StartSound(const nSound2& s)
{
    n_assert(this->monoCluster);
    n_assert(this->stereoCluster);
    
    switch (s.GetNumChannels())
    {
        case 1:
            this->monoCluster->StartSound(s);
            break;

        case 2:
            this->stereoCluster->StartSound(s);
            break;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer2::UpdateSound(const nSound2& s)
{
    n_assert(this->monoCluster);
    n_assert(this->stereoCluster);
    
    switch (s.GetNumChannels())
    {
        case 1:
            this->monoCluster->UpdateSound(s);
            break;

        case 2:
            this->stereoCluster->UpdateSound(s);
            break;
    }
}

//------------------------------------------------------------------------------
/**
    StopSound() checks if the sound is currently playing, and if yes
    stops and clears that sound channel.
*/
void
nDSoundServer2::StopSound(const nSound2& s)
{
    n_assert(this->monoCluster);
    n_assert(this->stereoCluster);
    
    switch (s.GetNumChannels())
    {
        case 1:
            this->monoCluster->StopSound(s);
            break;

        case 2:
            this->stereoCluster->StopSound(s);
            break;
    }
}

//------------------------------------------------------------------------------
/**
    Handle an audio flush.
*/
void
nDSoundServer2::HandleFlush()
{
    n_assert(this->flushActive);

    float curVolume = 1.0f;

    if (this->timeStamp < this->fadeoutTimeStamp)
    {
        // fade out mastervolume
        float fadeoutTime = float(this->fadeoutTimeStamp - this->flushStarted);
        float volModulate = 0.0f;
        if (fadeoutTime > 0.0f)
        {
            volModulate = float(this->fadeoutTimeStamp - this->timeStamp) / fadeoutTime;
        }
        curVolume = n_sqrt(volModulate);
    }
    else if (!this->hasBeenFlushed)
    {
        // hold mastervolume at 0, and if not happened yet, flush all active sounds
        curVolume = 0.0f;

        // if muted, unmute
        nAudioServer2::MuteAudio(false);

        n_printf("*** Flushing sounds\n");
        this->monoCluster->Flush();
        this->stereoCluster->Flush();
        this->hasBeenFlushed = true;
    }
    else if (this->timeStamp < this->fadeinTimeStamp)
    {
        // fade in master volume
        float fadeinTime = float(this->fadeinTimeStamp - this->fadeoutTimeStamp);
        float volModulate = 0.0f;
        if (fadeinTime > 0.0f)
        {
            volModulate = 1.0f - float(this->fadeinTimeStamp - this->timeStamp) / fadeinTime;
        }
        curVolume = n_sqrt(volModulate);
    }
    else
    {
        curVolume = 1.0f;
        this->flushActive = false;
    }

    // set new mastervolume
    this->SetDSoundMasterVolume(curVolume * this->muteModulator * this->masterVolume);
}

//------------------------------------------------------------------------------
/**
    Finish the sound scene.
*/
void
nDSoundServer2::EndScene()
{
    n_assert(this->monoCluster);
    n_assert(this->stereoCluster);

    // flush active?
    if (this->flushActive)
    {
        this->HandleFlush();
    }

    // trigger the sound clusters
    this->monoCluster->End(this->listener);
    this->stereoCluster->End(this->listener);

    // update DirectSound listener
    this->SetListenerAttributes();

    // commit deferred 3d settings
    HRESULT hr = this->dsListener->CommitDeferredSettings();
    if (FAILED(hr))
    {
        n_printf("nDSoundServer2::EndScene(): CommitDeferredSettings() failed!\n");
    }

    // hand control to parent class
    nAudioServer2::EndScene();
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundServer2::MuteAudio(bool b)
{
    nAudioServer2::MuteAudio(b);

    // update the dsound master volume
    this->SetDSoundMasterVolume(this->masterVolume * this->muteModulator);
}

//------------------------------------------------------------------------------
/**
    Override standard behaviour: if fadeout and fadein time is 0.0, 
    immediately flush audio.
*/
void 
nDSoundServer2::FlushAudio(float fadeoutTime, float fadeinTime)
{
    if ((fadeoutTime < 0.01f) && (fadeinTime < 0.01f))
    {
        this->monoCluster->Flush();
        this->stereoCluster->Flush();
    }
    else
    {
        nAudioServer2::FlushAudio(fadeoutTime, fadeinTime);
    }
}

//------------------------------------------------------------------------------
