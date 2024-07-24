#define N_IMPLEMENTS nDSoundServer
//-------------------------------------------------------------------
//  ndsound_device.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "audio/ndsoundserver.h"

extern const char *ndsound_Error(HRESULT);

//-------------------------------------------------------------------
/**
    @brief Open and initialize DirectSound according to the settings
    defined by nAudioServer::SetMode().

    16-May-00   floh    created
    24-Sep-01   floh    ported to DX8
*/
//-------------------------------------------------------------------
bool nDSoundServer::OpenAudio(void)
{
    n_assert(!this->audio_open);
    n_assert(NULL == this->ds);
    n_assert(NULL == this->ds_prim);
    n_assert(NULL == this->ds_listener);
    n_assert(NULL == this->channels);
    n_assert(NULL == this->candidates);

    HRESULT hr;

    // open the DirectSound object
    hr = DirectSoundCreate8(NULL, &(this->ds), NULL);
    if (FAILED(hr)) 
    {
        n_printf("OpenAudio(): DirectSoundCreate8() failed with '%s'\n",ndsound_Error(hr));
        return false;
    }

    // set cooperative level...
    hr = this->ds->SetCooperativeLevel(this->hwnd, DSSCL_EXCLUSIVE);
    if (FAILED(hr)) 
    {
        n_printf("OpenAudio(): SetCooperativeLevel() failed with '%s'\n",ndsound_Error(hr));
        goto fail;
    }

    // set the speaker config according to the user defined settings
    DWORD sc;
    if (strcmp(this->mode_speaker_config, "5.1") == 0)
    {
        n_printf("OpenAudio(): setting speaker config to '5.1'.\n");
        sc = DSSPEAKER_5POINT1;
    }
    if (strcmp(this->mode_speaker_config, "head") == 0) 
    {
        n_printf("OpenAudio(): setting speaker config to 'headphones'.\n");
        sc = DSSPEAKER_HEADPHONE;
    } 
    else if (strcmp(this->mode_speaker_config, "mono") == 0) 
    {
        n_printf("OpenAudio(): setting speaker config to 'monoaural'.\n");
        sc = DSSPEAKER_MONO;
    } 
    else if (strcmp(this->mode_speaker_config, "quad") ==0 ) 
    {
        n_printf("OpenAudio(): setting speaker config to 'quadraphonic'.\n");
        sc = DSSPEAKER_QUAD;
    } 
    else if (strcmp(this->mode_speaker_config, "stereo") == 0) 
    {
        n_printf("OpenAudio(): setting speaker config to 'stereo'.\n");
        sc = DSSPEAKER_STEREO;
    } 
    else if (strcmp(this->mode_speaker_config, "surround") == 0) 
    {
        n_printf("OpenAudio(): setting speaker config to 'surround'.\n");
        sc = DSSPEAKER_SURROUND;
    } 
    else 
    {
        n_printf("OpenAudio(): unknown speaker config '%s', using default 'stereo'.\n",
                 this->mode_speaker_config);
        sc = DSSPEAKER_STEREO;
    }
    hr = this->ds->SetSpeakerConfig(sc);
    if (FAILED(hr)) 
    {
        n_printf("OpenAudio(): SetSpeakerConfig() failed with '%s'\n",ndsound_Error(hr));
        goto fail;
    }

    // obtain primary buffer and query for 3d listener interface
    DSBUFFERDESC desc;
    memset(&desc,0,sizeof(desc));
    desc.dwSize  = sizeof(desc);
    desc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER;
    hr = this->ds->CreateSoundBuffer(&desc, &(this->ds_prim), NULL);
    if (FAILED(hr)) 
    {
        n_printf("OpenAudio(): CreateSoundBuffer() on primary failed with '%s'\n",ndsound_Error(hr));
        goto fail;
    }
    hr = this->ds_prim->QueryInterface(IID_IDirectSound3DListener8, (LPVOID *)&(this->ds_listener));
    if (FAILED(hr)) 
    {
        n_printf("OpenAudio(): QueryInterface() for IDirectSound3DListener failed with '%s'\n",ndsound_Error(hr));
        goto fail;
    }

    // set the output format for the primary buffer
    WAVEFORMATEX w;
    memset(&w,0,sizeof(w));
    w.wFormatTag      = WAVE_FORMAT_PCM;
    w.nChannels       = 2;
    w.nSamplesPerSec  = this->mode_hz;
    w.wBitsPerSample  = this->mode_bps;
    w.nBlockAlign     = (w.wBitsPerSample * w.nChannels) / 8;
    w.nAvgBytesPerSec = w.nSamplesPerSec * w.nBlockAlign;
    hr = this->ds_prim->SetFormat(&w);
    if (FAILED(hr)) 
    {
        n_printf("OpenAudio(): SetFormat() on primary failed with '%s'\n",ndsound_Error(hr));
        goto fail;
    }

    // start playing the primary buffer...
    this->ds_prim->Play(0, 0, DSBPLAY_LOOPING);

    // allocate n secondary sound buffers...
    this->num_channels = mode_channels;
    this->channels = new nDSoundChannel[this->num_channels];
    int i;
    for (i=0; i<this->mode_channels; i++) 
    {
        this->channels[i].Initialize(this);
    }

    // allocate playback candidates array
    this->candidates = new nSound[this->num_channels];

    // up to superclass and done...
    return nAudioServer::OpenAudio();

fail:
    if (this->ds_listener) 
    {
        this->ds_listener->Release();
        this->ds_listener = NULL;
    }
    if (this->ds_prim) 
    {
        this->ds_prim->Release();
        this->ds_prim = NULL;
    }
    if (this->ds) 
    {
        this->ds->Release();
        this->ds = NULL;
    }
    return false;
}

//-------------------------------------------------------------------
/**
    @brief Shut down DirectSound.

    16-May-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundServer::CloseAudio(void)
{
    if (!this->audio_open) return;
    
    if (this->candidates) 
    {
        delete[] this->candidates;
        this->candidates = NULL;
    }
    if (this->channels) 
    {
        delete[] this->channels;
        this->channels = NULL;
    }
    if (this->ds_listener) 
    {
        this->ds_listener->Release();
        this->ds_listener = NULL;
    }
    if (this->ds_prim) 
    {
        this->ds_prim->Release();
        this->ds_prim = NULL;
    }
    if (this->ds) 
    {
        this->ds->Release();
        this->ds = NULL;
    }
    nAudioServer::CloseAudio();
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
