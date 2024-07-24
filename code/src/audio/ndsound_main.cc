#define N_IMPLEMENTS nDSoundServer
//-------------------------------------------------------------------
//  ndsound_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nenv.h"
#include "audio/ndsoundserver.h"

extern const char *ndsound_Error(HRESULT);

//-------------------------------------------------------------------
/**
    15-May-00   floh    created
*/
//-------------------------------------------------------------------
nDSoundServer::nDSoundServer()
: ref_hwnd(ks,this)
{
    this->ref_hwnd = "/sys/env/hwnd";
    this->hwnd = (HWND) this->ref_hwnd->GetI();
    this->ds          = NULL;
    this->ds_prim     = NULL;
    this->ds_listener = NULL;
    memset(&(this->caps),0,sizeof(caps));
    this->caps.dwSize = sizeof(this->caps);
    this->num_channels = 0;
    this->channels = NULL;
    this->cur_candidates = 0;
    this->candidates = NULL;
}

//-------------------------------------------------------------------
/**
    15-May-00   floh    created
*/
//-------------------------------------------------------------------
nDSoundServer::~nDSoundServer()
{ 
    if (this->audio_open) this->CloseAudio();
}

//-------------------------------------------------------------------
/**
    23-Jun-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundServer::SetMasterVolume(float f)
{
    if (!this->audio_open) return;
    
    n_assert(this->ds_prim);
    nAudioServer::SetMasterVolume(f);
    LONG ds_vol = (LONG) (float(DSBVOLUME_MIN) + (float(DSBVOLUME_MAX-DSBVOLUME_MIN)*f));
    this->ds_prim->SetVolume(ds_vol);
}

//-------------------------------------------------------------------
/**
    Clear play candidates.

    19-May-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundServer::BeginScene(void)
{
    if (!this->audio_open) return;

    n_assert(!this->in_begin_scene);
    n_assert(this->channels);
    n_assert(this->candidates);
    this->cur_candidates = 0;
    this->in_begin_scene = true;
}

//-------------------------------------------------------------------
/**
    @brief Get the playback candidate with the lowest priority.

    22-May-00   floh    created
*/
//-------------------------------------------------------------------
nSound *nDSoundServer::get_lowpri_candidate(void)
{
    n_assert(this->candidates);
    int i;
    float min_pri   = this->candidates[0].GetPriority();
    int min_index = 0;
    for (i=1; i<this->cur_candidates; i++) {
        nSound *snd = &(this->candidates[i]);
        if (snd->GetPriority() < min_pri) {
            min_pri = snd->GetPriority();
            min_index = i;
        }
    }
    return &(this->candidates[min_index]);
}

//-------------------------------------------------------------------
/**
    @brief The listener always sits at (0,0,0) and looks towards the
    negative Z-axis, just like the OpenGL camera.

    24-May-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundServer::update_listener(void)
{
    n_assert(this->ds_prim);
    n_assert(this->ds_listener);

    HRESULT hr;
    vector3 pos(0.0f,0.0f,0.0f);
    vector3 front(0.0f,0.0f,-1.0f);
    vector3 up(0.0f,1.0f,0.0f);
    vector3 vel;
 
    this->listener.GetVelocity(vel);
 
    float doppler = this->listener.GetDopplerFactor();
    if (doppler < DS3D_MINDOPPLERFACTOR)        doppler = DS3D_MINDOPPLERFACTOR;
    else if (doppler > DS3D_MAXDOPPLERFACTOR)   doppler = DS3D_MAXDOPPLERFACTOR;

    float rolloff = this->listener.GetRolloffFactor();
    if (rolloff < DS3D_MINROLLOFFFACTOR)        rolloff = DS3D_MINROLLOFFFACTOR;
    else if (rolloff > DS3D_MAXROLLOFFFACTOR)   rolloff = DS3D_MAXROLLOFFFACTOR;

    DS3DLISTENER l;
    memset(&l,0,sizeof(l));
    l.dwSize = sizeof(l);
    l.vPosition.x = 0.0f;
    l.vPosition.y = 0.0f;
    l.vPosition.z = 0.0f;
    l.vVelocity.x = vel.x;
    l.vVelocity.y = vel.y;
    l.vVelocity.z = vel.z;
    l.vOrientFront.x = 0.0f;
    l.vOrientFront.y = 0.0f;
    l.vOrientFront.z = -1.0f;
    l.vOrientTop.x = 0.0f;
    l.vOrientTop.y = -1.0f;
    l.vOrientTop.z = 0.0f;
    l.flDistanceFactor = 1.0f;
    l.flDopplerFactor = doppler;
    l.flRolloffFactor = rolloff;
    hr = this->ds_listener->SetAllParameters(&l,DS3D_DEFERRED);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundServer::update_listener(): SetAllParameters() failed with '%s'\n",ndsound_Error(hr));
    }

    hr = this->ds_listener->CommitDeferredSettings();
    if (FAILED(hr)) 
    {
        n_printf("nDSoundServer::update_listener(): CommitDeferredSettings() failed with '%s'\n",ndsound_Error(hr));
    }
}

//-------------------------------------------------------------------
/**
    @brief Insert a new play candidate with a given state.

    Evict lowest pri play candidate if candidate stack is full.

    29-May-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundServer::insert_candidate(nSound *snd)
{
    if (this->cur_candidates == this->num_channels) 
    {
        // play candidates array full, overwrite candidate
        // with lowest priority
        nSound *replace = this->get_lowpri_candidate();
        n_assert(replace);
        if (replace->GetPriority() <= snd->GetPriority()) 
        {
            *replace = *snd;
        }
    } 
    else 
    {
        // candidate array not yet full, just add to the end
        this->candidates[this->cur_candidates] = *snd;
        this->cur_candidates++;
    }
}

//-------------------------------------------------------------------
/**
    Put the new sound into the play candidate array and
    set the STARTED flag. If candidate array is full, evict
    lowest priority candidate.

    29-May-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundServer::SetSound(nSound *snd)
{
    if (!this->audio_open) return;
    
    n_assert(snd);
    n_assert(this->channels);
    n_assert(this->candidates);
    n_assert(in_begin_scene);
    this->insert_candidate(snd);
}

//-------------------------------------------------------------------
/**
    @brief If sound source is currently played, stop it and remove
    it immediately from the channel set.

    29-May-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundServer::StopSound(nSound *snd)
{
    if (!this->audio_open) return;
    
    n_assert(snd);
    n_assert(this->channels);
    n_assert(this->candidates);
    int i;
    for (i=0; i<this->num_channels; i++) {
        if (this->channels[i].IsActive() && 
            this->channels[i].IsIdenticalSound(snd))
        {
            this->channels[i].Clear();
        }
    }
}

//-------------------------------------------------------------------
/**
    @brief Define the listener for this frame.

    24-May-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundServer::SetListener(nListener *l)
{
    if (!this->audio_open) return;
    
    n_assert(this->in_begin_scene);
    n_assert(l);
    this->listener = *l;
}

//-------------------------------------------------------------------
/**
    At this point we have a complete set of the highest priority
    play candidates for this frame and the set of the currently
    playing sound sources from the last frame.

    For each candidate:
    - clear each channel that doesn't already hold one of the play 
      candidates
    - for each other channel, if the sound source is in STARTED
      state, attach the sound source again
    - fill each cleared channel with the remaining play candidates

    19-May-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundServer::EndScene(void)
{
    if (!this->audio_open) return;

    n_assert(this->in_begin_scene);

    int num_new_sounds = 0;
    nSound *new_sounds[nAudioServer::N_MAXCHANNELS];
    uchar channel_busy[nAudioServer::N_MAXCHANNELS];
    memset(channel_busy,0,sizeof(channel_busy));

    // find channels that already hold play candidates,
    // and restart them as necessary
    int i;
    for (i=0; i<this->cur_candidates; i++) {
        nSound *snd = &(this->candidates[i]);
        int j;
        for (j=0; j<this->num_channels; j++) {
            nDSoundChannel *chn = &(this->channels[j]);
            if (chn->IsActive() && chn->IsIdenticalSound(snd)) {
                channel_busy[j] = 1;
                if (chn->IsOlder(snd)) {
                    // restart this sound source
                    chn->AttachSound(snd);
                } else {
                    // just refresh the channel with new values
                    chn->UpdateSound(snd);
                }
                break;
            }
        }
        // if fall through then this is a new source
        if (j==this->num_channels) {
            new_sounds[num_new_sounds++] = snd;
        }
    }

    // distribute the new sounds among the free channels, 
    // clear any remaining channels
    int cur_new_sound = 0;
    for (i=0; i<this->num_channels; i++) {
        if (!channel_busy[i]) {
            // here's a free channel
            nDSoundChannel *chn = &(this->channels[i]);
            if (cur_new_sound < num_new_sounds) {
                // ... and a new sound to play
                nSound *snd = new_sounds[cur_new_sound++];
                chn->AttachSound(snd);
            } else if (chn->IsActive()) {
                // there are no more new sound source so this
                // channels has to be silenced
                chn->Clear();
            }
        }
    }

    // trigger all channels, so they can update their streams and
    // 3d attributes
    for (i=0; i<this->num_channels; i++) {
        this->channels[i].Update();
    }

    // update the listener
    this->update_listener();
 
    this->in_begin_scene = false;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

