#define N_IMPLEMENTS nDSoundServer
//-------------------------------------------------------------------
//  ndsound_channel.cc
//  Implement nDSoundChannel class.
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "audio/ndsoundserver.h"
#include "audio/nstream.h"

extern const char *ndsound_Error(HRESULT);

//-------------------------------------------------------------------
/**
    17-May-00   floh    created
*/
//-------------------------------------------------------------------
nDSoundChannel::~nDSoundChannel()
{
    if (this->ds3dbuf8) 
    {
        this->ds3dbuf8->Release();
        this->ds3dbuf8 = NULL;
    }
    if (this->dsbuf8) 
    {
        this->dsbuf8->Release();
        this->dsbuf8 = NULL;
    }
    if (this->dsbuf7) 
    {
        this->dsbuf7->Release();
        this->dsbuf7 = NULL;
    }
}

//-------------------------------------------------------------------
/**
    17-May-00   floh    created
    24-Sep-01   floh    ported to DSound8
*/
//-------------------------------------------------------------------
bool nDSoundChannel::Initialize(nDSoundServer *dsound)
{
    n_assert(!this->is_initialized);
    n_assert(NULL == this->dsbuf7);
    n_assert(NULL == this->dsbuf8);
    n_assert(NULL == this->ds3dbuf8);

    HRESULT hr;
    
    // create a DirectSound secondary buffer compatible with the primary buffer
    DSBUFFERDESC desc;
    WAVEFORMATEX wf;
    dsound->ds_prim->GetFormat(&wf,sizeof(wf),NULL);
    wf.nChannels         = 1;
    wf.nAvgBytesPerSec >>= 1;
    wf.nBlockAlign     >>= 1;
    memset(&desc,0,sizeof(desc));
    desc.dwSize  = sizeof(desc);
    desc.dwFlags = DSBCAPS_CTRL3D | 
                   DSBCAPS_CTRLFREQUENCY |
                   DSBCAPS_CTRLVOLUME |
                   DSBCAPS_GETCURRENTPOSITION2 |
                   DSBCAPS_MUTE3DATMAXDISTANCE;
    desc.dwBufferBytes = BUFFERBYTES;
    desc.lpwfxFormat   = &wf;
    desc.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
    hr = dsound->ds->CreateSoundBuffer(&desc, &this->dsbuf7, NULL);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::Initialize(): CreateSoundBuffer() failed with '%s'\n",ndsound_Error(hr));
        goto fail;
    }

    // get IDirectSound8 interface
    hr = this->dsbuf7->QueryInterface(IID_IDirectSoundBuffer8, (void**) &(this->dsbuf8));
    if (FAILED(hr))
    {
        n_printf("nDSoundChannel::Initialize: QueryInterface(IID_IDirectSoundBuffer8) failed with '%s'\n", ndsound_Error(hr));
        goto fail;
    }

    // retrieve 3d buffer interface
    hr = this->dsbuf8->QueryInterface(IID_IDirectSound3DBuffer8, (void **)&(this->ds3dbuf8));
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::Initialize(): QueryInterface(IID_IDirectSound3DBuffer8) failed with '%s'\n",ndsound_Error(hr));
        goto fail;
    }

    // fill buffer with silence
    void *ptr1,*ptr2;
    DWORD bytes1,bytes2;
    hr = this->dsbuf8->Lock(0, 0, &ptr1, &bytes1, &ptr2, &bytes2, DSBLOCK_ENTIREBUFFER);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::Initialize(): Lock() failed with '%s'\n",ndsound_Error(hr));
        goto fail;
    }
    memset(ptr1, 0, bytes1);
    hr = this->dsbuf8->Unlock(ptr1,bytes1,ptr2,bytes2);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::Initialize(): Unlock() failed with '%s'\n",ndsound_Error(hr));
        goto fail;
    }

    // start the sound buffer
    hr = this->dsbuf8->Play(0, 0, DSBPLAY_LOOPING);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::AttachSoundSource(): Play() failed with '%s'\n",ndsound_Error(hr));
    }

    this->is_initialized = true;
    return true;

fail:
    if (this->ds3dbuf8) 
    {
        this->ds3dbuf8->Release();
        this->ds3dbuf8 = NULL;
    }
    if (this->dsbuf8) 
    {
        this->dsbuf8->Release();
        this->dsbuf8 = NULL;
    }
    if (this->dsbuf7) 
    {
        this->dsbuf7->Release();
        this->dsbuf7 = NULL;
    }
    return false;
}

//-------------------------------------------------------------------
/**
    @brief Attach a new sound to this channel.

    Do a stream exception so that new wave data is streamed in immediately.
    If DirectSoundBuffer not started, start it.

    19-May-00   floh    created
    20-May-00   floh    now copies sound object
    23-Jun-00   floh    + no longer start the sound buffer, the
                          sound buffer is already started at initialization
*/
//-------------------------------------------------------------------
void nDSoundChannel::AttachSound(nSound *snd)
{
    n_assert(snd);
    n_assert(this->dsbuf8);

    HRESULT hr;

    // copy object contents, because data in 'snd' is volatile
    this->is_active = true;
    this->sound     = *snd;
    this->filepos   = 0;

    // initiate a stream exception
    DWORD play_cursor;
    DWORD write_cursor;
    hr = this->dsbuf8->GetCurrentPosition((DWORD *)&play_cursor,(DWORD *)&write_cursor);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::Update(): GetCurrentPosition() failed with '%s'.\n",ndsound_Error(hr));
        return;
    }
    this->read_ahead(play_cursor);
}

//-------------------------------------------------------------------
/**
    @brief Just refresh the embedded sound source with updated values.

    24-May-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundChannel::UpdateSound(nSound *snd)
{
    n_assert(snd);
    n_assert(this->IsIdenticalSound(snd));
    n_assert(this->IsActive());
    this->sound = *snd;
}

//-------------------------------------------------------------------
/**
    @brief  Remove sound, stop DirectSound buffer.

    19-May-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundChannel::Clear(void)
{
    n_assert(this->dsbuf8);
    n_assert(this->IsActive());

    this->filepos = 0;
    this->is_active = false;

    // fill sound buffer with silence
    void *ptr1,*ptr2;
    DWORD bytes1,bytes2;
    HRESULT hr;
    hr = this->dsbuf8->Lock(0, 0, &ptr1, &bytes1, &ptr2, &bytes2, DSBLOCK_ENTIREBUFFER);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::Clear(): Lock() failed with '%s'\n",ndsound_Error(hr));
        return;
    }
    memset(ptr1, 0, bytes1);
    hr = this->dsbuf8->Unlock(ptr1, bytes1, ptr2, bytes2);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::Clear(): Unlock() failed with '%s'\n",ndsound_Error(hr));
        return;
    }
}

//-------------------------------------------------------------------
/**
    @brief Check if given sound is identical to embedded sound.

    19-May-00   floh    created
*/
//-------------------------------------------------------------------
bool nDSoundChannel::IsIdenticalSound(nSound *snd)
{
    n_assert(snd);
    return (this->sound.GetId() == snd->GetId());
}

//-------------------------------------------------------------------
/**
    29-May-00   floh    created
*/
//-------------------------------------------------------------------
bool nDSoundChannel::IsOlder(nSound *snd)
{
    n_assert(snd);
    return (this->sound.GetTimeStamp() > snd->GetTimeStamp());
}

//-------------------------------------------------------------------
/**
    @brief Read CHUNKBYTES bytes into buffer at position 'next_write_offset'.

    Then:

    @verbatim
    valid_start_offset   = trigger_write_offset
    trigger_write_offset = next_write_offset
    next_write_offset   += CHUNKBYTES
    @endverbatim

    If 'at_position' != -1 a stream exception has occured (the
    write cursor has been catched outside the valid data range,
    in this case, data is streamed in at the current write position,
    to fill the rest of the current and all of the next chunk.

    17-May-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundChannel::read_ahead(int at_position)
{
    n_assert(this->dsbuf8);
    n_assert(this->IsActive());
    HRESULT hr;

    int write_offset;
    int num_bytes;
    nStream *stream = this->sound.GetStream();
    n_assert(stream);

    if (at_position != -1) 
    {
        // intercept mode, get the current position of the write cursor
        write_offset = at_position;
        
        // number of bytes to read is one full chunk (the next chunk) plus
        // the remaining bytes in the current chunk
        num_bytes = (2*CHUNKBYTES) - (write_offset % CHUNKBYTES);
        
        // update offsets for the exception position
        this->valid_start_offset   = CHUNKBYTES * (write_offset / CHUNKBYTES);
        this->trigger_write_offset = (this->valid_start_offset + CHUNKBYTES) % BUFFERBYTES;
        this->next_write_offset    = (this->trigger_write_offset + CHUNKBYTES) % BUFFERBYTES;

    } 
    else 
    {
        // not in intercept mode, read a full chunk at the next write position
        write_offset = this->next_write_offset;
        num_bytes = CHUNKBYTES;

        // update offsets
        this->valid_start_offset   = this->trigger_write_offset;
        this->trigger_write_offset = this->next_write_offset;
        this->next_write_offset = (this->next_write_offset + CHUNKBYTES) % BUFFERBYTES;
    }

    // lock the sound buffer at the desired position
    void *ptr1,*ptr2;
    DWORD bytes1,bytes2;
    int bytes1_written,bytes2_written;
    hr = this->dsbuf8->Lock(write_offset,num_bytes,&ptr1,&bytes1,&ptr2,&bytes2,0);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::ReadAhead(): Lock() failed with '%s'\n",ndsound_Error(hr));
        return;
    }

    // if the sound source is active, we will read from file,
    // otherwise, we'll just fill the buffer with silence
    if (this->IsActive()) 
    {
        // read data chunk from wav file
        stream->Seek(this->filepos);
        if (ptr1) bytes1_written = stream->Read((char *)ptr1,bytes1);
        else      bytes1_written = 0;
        if (ptr2) bytes2_written = stream->Read((char *)ptr2,bytes2);
        else      bytes2_written = 0;
        this->filepos = stream->Tell();
    } 
    else 
    {
        // we are not active, just stream silence into the buffer
        if (ptr1) {
            memset(ptr1,0,bytes1);
            bytes1_written = bytes1;
        } else bytes1_written = 0;
        if (ptr2) {
            memset(ptr2,0,bytes2);
            bytes2_written = bytes2;
        } else bytes2_written = 0;
    }
        
    hr = this->dsbuf8->Unlock(ptr1, bytes1_written, ptr2, bytes2_written);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::ReadAhead(): Unlock() failed with '%s'\n",ndsound_Error(hr));
        return;
    }
}

//-------------------------------------------------------------------
/**
    @brief Check if buffer position is inside 2 other positions with
    wrap around check.

    18-May-00   floh    created
*/
//-------------------------------------------------------------------
bool nDSoundChannel::inside(int pos, int start, int end)
{
    if (start < end) 
    {
        if ((pos>=start) && (pos<end)) return true;
        else                           return false;
    } 
    else 
    {
        if ((pos>=start) || (pos<end)) return true;
        else                           return false;
    }
}

//-------------------------------------------------------------------
/**
    @brief  Write 3d positional information from sound to
    DirectSound buffer.

    24-May-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundChannel::update_dsbuffer(void)
{
    n_assert(this->dsbuf8);
    n_assert(this->ds3dbuf8);
    n_assert(this->IsActive());

    HRESULT hr;

    // frequency: multiply base frequency with frequency multiplier
    nStream *stream = this->sound.GetStream();
    n_assert(stream);
    float base_freq = (float) stream->GetBaseFreq();
    float freq = base_freq * this->sound.GetFreq();
    if      (freq < DSBFREQUENCY_MIN) freq=DSBFREQUENCY_MIN;
    else if (freq > DSBFREQUENCY_MAX) freq=DSBFREQUENCY_MAX;

    // base volume
    float v = this->sound.GetVolume(); // this is between 0.0 and 1.0
    float vol = DSBVOLUME_MIN + ((DSBVOLUME_MAX-DSBVOLUME_MIN)*v);
    if      (vol < DSBVOLUME_MIN) vol=DSBVOLUME_MIN;
    else if (vol > DSBVOLUME_MAX) vol=DSBVOLUME_MAX;

    vector3 pos;
    vector3 vel;
    this->sound.GetPosition(pos);
    this->sound.GetVelocity(vel);

    float min_dist = this->sound.GetMinDistance();
    if (min_dist < 0.0001f) min_dist = 0.0001f;
    float max_dist = this->sound.GetMaxDistance();
    if (min_dist >= max_dist) min_dist = max_dist;
    
    DS3DBUFFER buf;
    memset(&buf,0,sizeof(buf));
    buf.dwSize = sizeof(buf);
    buf.vPosition.x = pos.x;
    buf.vPosition.y = pos.y;
    buf.vPosition.z = pos.z;
    buf.vVelocity.x = vel.x;
    buf.vVelocity.y = vel.y;
    buf.vVelocity.z = vel.z;
    buf.dwInsideConeAngle  = 0;
    buf.dwOutsideConeAngle = 0;
    buf.vConeOrientation.x = 0.0f;
    buf.vConeOrientation.y = 1.0f;
    buf.vConeOrientation.z = 0.0f;
    buf.lConeOutsideVolume = 0;
    buf.flMinDistance = min_dist;
    buf.flMaxDistance = max_dist;
    buf.dwMode = DS3DMODE_NORMAL;
    hr = this->ds3dbuf8->SetAllParameters(&buf,DS3D_DEFERRED);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::update_dsbuffer(): SetAllParameters() failed with '%s'\n",ndsound_Error(hr));
    }
    
    hr = this->dsbuf8->SetVolume((LONG)vol);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::update_dsbuffer(): SetVolume() failed with '%s'\n",ndsound_Error(hr));
    }
    hr = this->dsbuf8->SetFrequency((DWORD)freq);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::update_dsbuffer(): SetFrequency() failed with '%s'\n",ndsound_Error(hr));
    }
}

//-------------------------------------------------------------------
/**
    @brief  Call once a frame.

     - check if active 
     - if lost, restore sound buffer
     - write 3d positional data to sound buffer
     - stream new sound data in if necessary

    17-May-00   floh    created
*/
//-------------------------------------------------------------------
void nDSoundChannel::Update(void)
{
    n_assert(this->dsbuf8);
    n_assert(this->ds3dbuf8);

    // if we are not active, return now
    if (!this->IsActive()) return;

    HRESULT hr;
    DWORD status;
    hr = this->dsbuf8->GetStatus(&status);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::Update(): GetStatus() failed with '%s'.\n",ndsound_Error(hr));
        return;
    }
    if (DSBSTATUS_BUFFERLOST == status) 
    {
        n_printf("audio buffer lost!\n");
        hr = this->dsbuf8->Restore();
        if (FAILED(hr)) 
        {
            n_printf("nDSoundChannel::Update(): Restore() failed with '%s'.\n",ndsound_Error(hr));
            return;
        }
        // FIXME: need to handle a stream exception here...
    }

    // check if we need to stream new data into the buffer
    DWORD play_cursor;
    DWORD write_cursor;
    hr = this->dsbuf8->GetCurrentPosition((DWORD *)&play_cursor,(DWORD *)&write_cursor);
    if (FAILED(hr)) 
    {
        n_printf("nDSoundChannel::Update(): GetCurrentPosition() failed with '%s'.\n",ndsound_Error(hr));
        return;
    }

    // a stream exception occurs if the write cursor is outside the current
    // valid data range (which is between valid_data_start and next_write_offset)
    if (!this->inside(write_cursor, this->valid_start_offset, this->next_write_offset)) 
    {
        //n_printf("AUDIO STREAM EXCEPTION: w=%d, s=%d, e=%d\n",
        //         write_cursor,this->valid_start_offset,this->next_write_offset);
        this->read_ahead(write_cursor);
    } 
    else 
    {
        // otherwise, check if new data needs to be streamed into buffers,
        // this happens if the write cursor passes beyond the
        // 'trigger_write_offset' position
        if (inside(write_cursor, this->trigger_write_offset, this->next_write_offset)) 
        {
            this->read_ahead(-1);
        }
    }

    // update 3d parameters
    this->update_dsbuffer();
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
