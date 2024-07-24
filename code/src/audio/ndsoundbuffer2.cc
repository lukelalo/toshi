#define N_IMPLEMENTS nDSoundBuffer2
//------------------------------------------------------------------------------
//  ndsoundbuffer2.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio/ndsoundbuffer2.h"
#include "audio/ndsoundserver2.h"

extern const char *ndsound_Error(HRESULT);

//------------------------------------------------------------------------------
/**
*/
nDSoundBuffer2::nDSoundBuffer2() :
    initialized(false),
    streamExceptionRequest(false),
    streamDataRequest(false),
    dsBuf(0),
    dsBuf8(0),
    ds3DBuf8(0),
    validDataOffset(0),
    triggerWriteOffset(0),
    nextWriteOffset(0),
    curPlayOffset(0),
    curWriteOffset(0),
    endRangeValid(false),
    endRangeMin(0),
    endRangeMax(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDSoundBuffer2::~nDSoundBuffer2()
{
    this->ReleaseBuffers();   
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundBuffer2::ReleaseBuffers()
{
    if (this->ds3DBuf8)
    {
        this->ds3DBuf8->Release();
        this->ds3DBuf8 = 0;
    }
    if (this->dsBuf8)
    {
        this->dsBuf8->Release();
        this->dsBuf8 = 0;
    }
    if (this->dsBuf)
    {
        this->dsBuf->Release();
        this->dsBuf = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Attach a sound object to the nDSoundBuffer. This will set the 
    playing flag in the sound object, and set a flag which 
    causes a stream exception at the next call to Update().
*/
void
nDSoundBuffer2::AttachSoundObject(const nSound2& obj)
{
    // release old sound object if not in invalid state
    if (this->soundObj.GetState() != nSound2::FREE)
    {
        this->DetachSoundObject();
    }
    this->soundObj = obj;
    this->soundObj.GetStream()->AddRef();
    this->soundObj.SetState(nSound2::STARTED);
    this->streamExceptionRequest = true;
}

//------------------------------------------------------------------------------
/**
    Updates the internal copy of the sound object with the new data
    from the argument nSound2 object.
*/
void
nDSoundBuffer2::UpdateSoundObject(const nSound2& obj)
{
    n_assert(this->soundObj.GetId() != 0);
    n_assert(obj.GetId() == this->soundObj.GetId());
    this->soundObj.SetAmbient(obj.GetAmbient());
    this->soundObj.SetPriority(obj.GetPriority());
    this->soundObj.SetVolume(obj.GetVolume());
    this->soundObj.SetFrequency(obj.GetFrequency());
    this->soundObj.SetPosition(obj.GetPosition());
    this->soundObj.SetVelocity(obj.GetVelocity());
    this->soundObj.SetMinDistance(obj.GetMinDistance());
    this->soundObj.SetMaxDistance(obj.GetMaxDistance());
    this->soundObj.SetState(nSound2::UPDATED);
}

//------------------------------------------------------------------------------
/**
    Detach the currently attached sound object. This will clear the
    playing flag in the sound object, and perform a stream exception
    (which will silence the sound buffer).
*/
void
nDSoundBuffer2::DetachSoundObject()
{
    if (this->soundObj.GetStream())
    {
        this->soundObj.GetStream()->Release();
    }
    this->soundObj.Clear();
    this->streamExceptionRequest = true;
}

//------------------------------------------------------------------------------
/**
    Get a reference of the embedded nSound2 object
*/
nSound2&
nDSoundBuffer2::GetSoundObject()
{
    return this->soundObj;
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundBuffer2::Initialize(int numChannels, IDirectSound8* dSound, IDirectSoundBuffer* primary)
{
    n_assert(dSound);
    n_assert(primary);
    n_assert(!this->initialized);
    n_assert(!(this->dsBuf || this->dsBuf8 || this->ds3DBuf8));
    n_assert((numChannels > 0) && (numChannels < 3));

    HRESULT hr;

    // create a DSound secondary buffer which is compatible with the primary buffer
    DSBUFFERDESC desc = { sizeof(DSBUFFERDESC), 0 };
    WAVEFORMATEX waveFormat = { 0 };
    
    hr = primary->GetFormat(&waveFormat, sizeof(waveFormat), NULL);
    if (FAILED(hr))
    {
        n_printf("nDSoundBuffer2::Initialize(): GetFormat() failed with '%s'\n", ndsound_Error(hr));
        this->ReleaseBuffers();
        return;
    }

    // patch wave format and fill buffer desc
    waveFormat.nChannels = numChannels;
    desc.dwBufferBytes = nDSoundBuffer2::BUFFERBYTES;
    desc.dwReserved = 0;
    desc.lpwfxFormat = &waveFormat;
    if (1 == numChannels)
    {
        waveFormat.nChannels         = 1;
        waveFormat.nAvgBytesPerSec >>= 1;
        waveFormat.nBlockAlign     >>= 1;

        desc.dwFlags = DSBCAPS_CTRL3D |
                       DSBCAPS_CTRLFREQUENCY |
                       DSBCAPS_CTRLVOLUME |
                       DSBCAPS_GETCURRENTPOSITION2 |
                       DSBCAPS_MUTE3DATMAXDISTANCE;

        desc.guid3DAlgorithm = DS3DALG_DEFAULT;
    }
    else
    {
        // stereo buffers cannot be embedded into 3d space
        desc.dwFlags = DSBCAPS_CTRLVOLUME |
                       DSBCAPS_GETCURRENTPOSITION2;
        desc.guid3DAlgorithm = DS3DALG_DEFAULT;
    }
    hr = dSound->CreateSoundBuffer(&desc, &(this->dsBuf), NULL);
    if (FAILED(hr))
    {
        n_printf("nDSoundBuffer2::Initialize(): CreateSoundBuffer() failed with '%s'\n", ndsound_Error(hr));
        this->ReleaseBuffers();
        return;
    }

    // obtain interfaces to sound buffer
    hr = this->dsBuf->QueryInterface(IID_IDirectSoundBuffer8, (void**) &(this->dsBuf8));
    if (FAILED(hr))
    {
        n_printf("nDSoundBuffer2::Initialize(): QueryInterface(IID_IDirectSoundBuffer8) failed with '%s'\n", ndsound_Error(hr));
        this->ReleaseBuffers();
        return;
    }
    if (1 == numChannels)
    {
        hr = this->dsBuf8->QueryInterface(IID_IDirectSound3DBuffer8, (void**) &(this->ds3DBuf8));
        if (FAILED(hr))
        {
            n_printf("nDSoundBuffer2::Initialize(): QueryInterface(IID_IDirectSound3DBuffer8) failed with '%s'\n", ndsound_Error(hr));
            this->ReleaseBuffers();
            return;
        }
    }
    else
    {
        this->ds3DBuf8 = 0;
    }

    // fill the buffer with silence
    void* ptr1;
    void* ptr2;
    DWORD bytes1;
    DWORD bytes2;
    hr = this->dsBuf8->Lock(0, 0, &ptr1, &bytes1, &ptr2, &bytes2, DSBLOCK_ENTIREBUFFER);
    if (FAILED(hr))
    {
        n_printf("nDSoundBuffer2::Initialize(): Lock() failed with '%s'\n", ndsound_Error(hr));
        this->ReleaseBuffers();
        return;
    }
    memset(ptr1, 0, bytes1);
    hr = this->dsBuf8->Unlock(ptr1, bytes1, ptr2, bytes2);
    if (FAILED(hr))
    {
        n_printf("nDSoundBuffer2::Initialize(): Unock() failed with '%s'\n", ndsound_Error(hr));
        this->ReleaseBuffers();
        return;
    }

    // start the sound buffer
    hr = this->dsBuf8->Play(0, 0, DSBPLAY_LOOPING);
    if (FAILED(hr))
    {
        n_printf("nDSoundBuffer2::Initialize(): Play() failed with '%s'\n", ndsound_Error(hr));
    }

    this->initialized = true;
}

//------------------------------------------------------------------------------
/**
    Stream data from the sound object into the dsound buffer. If no
    sound object is attached, no data will be streamed, instead
    the sound buffer will be filled with zeros. Otherwise the sound
    object is asked to stream the data into the DSound buffer.

    @param  offset      DSoundBuffer offset to write to
    @param  numBytes    number of bytes to write
    @param  crossFade   true if a cross fading should be done
*/
void
nDSoundBuffer2::ReadStreamData(int offset, int numBytes, bool crossFade)
{
    HRESULT hr;

    // lock the sound buffer
    void* ptr1 = 0;
    void* ptr2 = 0;
    DWORD bytes1 = 0;
    DWORD bytes2 = 0;
    DWORD bytes1Written = 0;
    DWORD bytes2Written = 0;
    hr = this->dsBuf8->Lock(offset, numBytes, &ptr1, &bytes1, &ptr2, &bytes2, 0);
    if (FAILED(hr))
    {
        n_printf("nDSoundBuffer2::StreamData(): Lock() failed with '%s'\n", ndsound_Error(hr));
        return;
    }

    // read data
    int bytes1Read = 0;
    int bytes2Read = 0;
    if (this->soundObj.GetState() != nSound2::FREE)
    {
        this->soundObj.BeginRead(crossFade);
        if (ptr1)
        {
            bytes1Read = this->soundObj.Read(ptr1, bytes1);
        }
        if (ptr2)
        {
            bytes2Read = this->soundObj.Read(ptr2, bytes2);
        }
        this->soundObj.EndRead();
    }
    else
    {
        // no sound object, just fill buffer with silence
        if (ptr1) 
        {
            memset(ptr1, 0, bytes1);
        } 
        if (ptr2) 
        {
            memset(ptr2, 0, bytes2);
        } 
    }

    // unlock the sound buffer
    hr = this->dsBuf8->Unlock(ptr1, bytes1, ptr2, bytes2);
    if (FAILED(hr))
    {
        n_printf("nDSoundBuffer2::StreamData(): Unlock() failed with '%s'\n", ndsound_Error(hr));
        return;
    }

    // set the playback end range marker
    if ((this->soundObj.GetState() != nSound2::FREE) && (this->soundObj.EndOfStream()) && (!this->endRangeValid))
    {
        this->endRangeValid = true; 
        this->endRangeMin   = this->curPlayOffset;
        this->endRangeMax   = (offset + bytes1Read + bytes2Read) % BUFFERBYTES;
    }
}

//------------------------------------------------------------------------------
/**
    Update the internal buffer offsets based on the position of the current
    write cursor.
*/
void
nDSoundBuffer2::UpdateBufferOffsets()
{
    this->validDataOffset    = (CHUNKBYTES * (this->curWriteOffset / CHUNKBYTES)) % BUFFERBYTES;
    this->triggerWriteOffset = (this->validDataOffset + CHUNKBYTES) % BUFFERBYTES;
    this->nextWriteOffset    = (this->triggerWriteOffset + CHUNKBYTES) % BUFFERBYTES;
}

//------------------------------------------------------------------------------
/**
    Perform a stream exception. This overwrites the data in the sound buffer
    at the next possible position with new data from the sound object's
    stream.
*/
void
nDSoundBuffer2::StreamException(int writeOffset)
{
    n_assert(this->initialized);

    // invalidate the end range
    this->endRangeValid = false; 

    // compute number of bytes needed to fill the current and next buffer chunk
    int numBytes = (2 * CHUNKBYTES) - (writeOffset % CHUNKBYTES);

    // fill sound buffer with stream data, do a cross fade
    this->ReadStreamData(writeOffset, numBytes, true);

    // advance the stream trigger position
    this->UpdateBufferOffsets();
}

//------------------------------------------------------------------------------
/**
    Read a new chunk of stream data into the DSound buffer.
*/
void
nDSoundBuffer2::StreamData()
{
    n_assert(this->initialized);

    int writeOffset = this->nextWriteOffset;
    int numBytes = CHUNKBYTES;

    // fill one stream chunk at the next write offset without crossfade
    this->ReadStreamData(writeOffset, numBytes, false);

    // advance the stream trigger position by 1 chunk
    this->UpdateBufferOffsets();
}

//------------------------------------------------------------------------------
/**
    Check if a buffer offset is inside 2 other buffer, with wrap around.
*/
bool
nDSoundBuffer2::Inside(int pos, int start, int end)
{
    if (start < end) 
    {
        if ((pos >= start) && (pos < end)) return true;
        else                               return false;
    } 
    else 
    {
        if ((pos >= start) || (pos < end)) return true;
        else                               return false;
    }
}


//------------------------------------------------------------------------------
/**
    Checks if a stream exception has occured. This happens if the
    playback cursor has entered an area with invalid stream data.
*/
bool
nDSoundBuffer2::CheckStreamException(int writeOffset)
{
    return (!this->Inside(writeOffset, this->validDataOffset, this->nextWriteOffset));
}

//------------------------------------------------------------------------------
/**
    Checks if new data needs to be streamed into the buffers. This happens
    if the write cursor is beyond the triggerWriteOffset marker, but before
    the nextWriteOffset marker (which is still inside the valid data range).
*/
bool
nDSoundBuffer2::CheckStreamData(int writeOffset)
{
    return this->Inside(writeOffset, this->triggerWriteOffset, this->nextWriteOffset);
}

//------------------------------------------------------------------------------
/**
    Check if sound playback has stopped (for oneshot sounds only).
    We cannot simply check whether the stream file is exhausted (it's not
    guaranteed that all the stream data has already been PLAYED). That's
    why the playback cursor must be checked whether it has already stepped
    past the "end of sample" marker.
*/
bool
nDSoundBuffer2::CheckStopped()
{
    bool stopped = false;
    if ((this->soundObj.GetState() != nSound2::FREE) && this->endRangeValid)
    {
        stopped = !this->Inside(this->curPlayOffset, this->endRangeMin, this->endRangeMax);
        if (stopped)
        {
            this->endRangeValid = false;
        }
    }
    return stopped;
}

//------------------------------------------------------------------------------
/**
    Update the DirectSound buffer with the current nSound2 parameters.
*/
void
nDSoundBuffer2::UpdateAttrs(const nListener2& listener)
{
    n_assert(this->initialized);

    if (this->soundObj.GetState() != nSound2::FREE)
    {
        HRESULT hr;

        // frequency: multiply base frequency with frequency multiplier
        nStream *stream = this->soundObj.GetStream();
        n_assert(stream);
        float baseFreq = (float) stream->GetBaseFreq();
        float freq = baseFreq * this->soundObj.GetFrequency();
        if (freq < DSBFREQUENCY_MIN)
        {
            freq = DSBFREQUENCY_MIN;
        }
        else if (freq > DSBFREQUENCY_MAX)
        {
            freq = DSBFREQUENCY_MAX;
        }
        
        // volume
        float vol = DSBVOLUME_MIN + ((DSBVOLUME_MAX - DSBVOLUME_MIN) * this->soundObj.GetVolume());
        if (vol < DSBVOLUME_MIN)
        {
            vol = DSBVOLUME_MIN;
        }
        else if (vol > DSBVOLUME_MAX)
        {
            vol = DSBVOLUME_MAX;
        }

        // update 3d parameters (only if this is a 3d buffer)
        if (this->ds3DBuf8)
        {
            // position and velocity
            vector3 pos = this->soundObj.GetPosition();
            vector3 vel = this->soundObj.GetVelocity();
            if (this->soundObj.GetAmbient())
            {
                // if we are an ambient sound source, align position and orientation with the listener
                pos = listener.GetMatrix().pos_component();
                vel = listener.GetVelocity();
            }

            // min and max dist
            float minDist = this->soundObj.GetMinDistance();
            float maxDist = this->soundObj.GetMaxDistance();

            // set 3d parameters
            DS3DBUFFER desc = { sizeof(DS3DBUFFER), 0 };
            desc.vPosition.x = pos.x;
            desc.vPosition.y = pos.y;
            desc.vPosition.z = pos.z;
            desc.vVelocity.x = vel.x;
            desc.vVelocity.y = vel.y;
            desc.vVelocity.z = vel.z;
            desc.dwInsideConeAngle = 0;
            desc.dwOutsideConeAngle = 0;
            desc.vConeOrientation.x = 0.0f;
            desc.vConeOrientation.y = 1.0f;
            desc.vConeOrientation.z = 0.0f;
            desc.lConeOutsideVolume = 0;
            desc.flMinDistance = minDist;
            desc.flMaxDistance = maxDist;
            desc.dwMode = DS3DMODE_NORMAL;
            hr = this->ds3DBuf8->SetAllParameters(&desc, DS3D_DEFERRED);
            if (FAILED(hr))
            {
                n_printf("nDSoundBuffer2::UpdateAttrs(): SetAllParameters() failed with '%s'\n", ndsound_Error(hr));
            }
        }

        // set buffer volume
        hr = this->dsBuf8->SetVolume((LONG)vol);
        if (FAILED(hr))
        {
            n_printf("nDSoundBuffer2::UpdateAttrs(): SetVolume() failed with '%s'\n", ndsound_Error(hr));
        }

        // set playback frequency
        hr = this->dsBuf8->SetFrequency((DWORD)freq);
        if (FAILED(hr))
        {
            n_printf("nDSoundBuffer2::UpdateAttrs(): SetFrequency() failed with '%s'\n", ndsound_Error(hr));
        }
    }
}

//------------------------------------------------------------------------------
/**
    Trigger the nDSoundBuffer2 object, this method should be called once per
    frame. The following things will happen:

    - copy the 3d sound parameters from the nSound2 object into the 
      DirectSound buffer object
    - do a stream exception if necessary (this depends on the state
      of the nSound2 object)
    - otherwise make sure that the DirectSound buffer contains valid
      data by streaming chunks by streaming chunks of sound data 
      from disc
*/
void
nDSoundBuffer2::Trigger(const nListener2& listener)
{
    n_assert(this->initialized);
    n_assert(this->dsBuf8);
    HRESULT hr;

    // handle buffer lost/restore
    DWORD bufferStatus;
    hr = this->dsBuf8->GetStatus(&bufferStatus);
    if (FAILED(hr))
    {
        n_printf("nDSoundBuffer2::Update(): GetStatus() failed with '%s'\n", ndsound_Error(hr));
        return;
    }
    if (DSBSTATUS_BUFFERLOST == bufferStatus)
    {
        hr = this->dsBuf8->Restore();
        if (FAILED(hr))
        {
            n_printf("nDSoundBuffer2::Update(): Restore() failed with '%s'\n", ndsound_Error(hr));
            return;
        }
        // do a stream exception because the buffer content may be invalid now
        this->streamExceptionRequest = true;
    }

    // update the current write and playback cursor position
    hr = this->dsBuf8->GetCurrentPosition(&(this->curPlayOffset), &(this->curWriteOffset));
    if (FAILED(hr))
    {
        n_printf("nDSoundBuffer::Update(): GetCurrentPosition() failed with '%s'\n", hr);
    }
    
    // update the sound buffer attributes
    this->UpdateAttrs(listener);

    // do we have a sound object attached at all?
    if (this->soundObj.GetState() != nSound2::FREE)
    {
        nSound2::State state = this->soundObj.GetState();
        if (nSound2::STARTED == state)
        {
            // if the nSound2 object is in the STARTED state, rewind it and do a stream exception,
            // and leave the started state
            this->soundObj.RewindStream();
            this->streamExceptionRequest = true;
            this->soundObj.SetState(nSound2::ORPHANED);
        }
        else if ((nSound2::UPDATED == state) || (nSound2::ORPHANED == state))
        {
            // a normal playback state, check if new data needs to be streamed in
            if (this->CheckStreamException(curWriteOffset))
            {
                this->streamExceptionRequest = true;
            }
            else if (this->CheckStreamData(curWriteOffset))
            {
                this->streamDataRequest = true;
            }
        }
    }
    else
    {
        // if there is no sound object just do the normal processing
        // anyway (which will just write zeros into the buffer)
        if (this->CheckStreamException(curWriteOffset))
        {
            this->streamExceptionRequest = true;
        }
        else if (this->CheckStreamData(curWriteOffset))
        {
            this->streamDataRequest = true;
        }
    }

    if (this->streamExceptionRequest)
    {
        // if a stream exception has been requested, do it now
        this->StreamException(curWriteOffset);
    }
    else if (this->streamDataRequest)
    {
        // if normal data streaming has been requested, do it now
        this->StreamData();
    }
    this->streamExceptionRequest = false;
    this->streamDataRequest = false;
}

//------------------------------------------------------------------------------
