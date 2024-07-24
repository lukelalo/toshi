#ifndef N_DSOUNDBUFFER2_H
#define N_DSOUNDBUFFER2_H
//------------------------------------------------------------------------------
/**
    @class nDSoundBuffer2
    @ingroup NebulaAudioModule    
    @brief DirectSound secondary buffer wrapper, maps a nSound2 object to a 
    sound buffer.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_DSOUNDSERVER2_H
#include "audio/ndsoundserver2.h"
#endif

#undef N_DEFINES
#define N_DEFINES nDSoundBuffer2
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nSound2;

class nDSoundBuffer2
{
public:
    /// constructor
    nDSoundBuffer2();
    /// destructor
    ~nDSoundBuffer2();
    /// initialize the object
    void Initialize(int numChannels, IDirectSound8* dSound, IDirectSoundBuffer* primary);
    /// attach a sound object
    void AttachSoundObject(const nSound2& obj);
    /// update sound object attributes
    void UpdateSoundObject(const nSound2& obj);
    /// detach current sound object, this free the channel for a new sound
    void DetachSoundObject();
    /// trigger sound playback, called once per frame
    void Trigger(const nListener2& listener);
    /// check if sound playback has stopped (if oneshot sound playing)
    bool CheckStopped();
    /// return a reference to the embedded nSound2 object
    nSound2& GetSoundObject();
    
private:
    /// release internal DSound buffers
    void ReleaseBuffers();
    /// read a new chunk of data from the source stream
    void ReadStreamData(int offset, int numBytes, bool crossFade);
    /// update the internal buffer offsets according to the write position
    void UpdateBufferOffsets();
    /// do a stream exception
    void StreamException(int writeOffset);
    /// do a normal stream chunk update
    void StreamData();
    /// check if wrap-around position is inside a start and end offset
    bool Inside(int pos, int start, int end);
    /// check if a stream exception has happened
    bool CheckStreamException(int writeOffset);
    /// check if a new chunk of data must be streamed
    bool CheckStreamData(int writeOffset);
    /// update DirectSound buffer attributes
    void UpdateAttrs(const nListener2& listener);

    enum
    {
        BUFFERBYTES = (1<<16),                  // sound buffer size in bytes
        NUMCHUNKS   = 4,                        // number of stream chunks in buffer
        CHUNKBYTES  = (BUFFERBYTES/NUMCHUNKS),  // read chunk size in bytes
    };
    bool initialized;
    bool streamExceptionRequest;
    bool streamDataRequest;
    IDirectSoundBuffer* dsBuf;
    IDirectSoundBuffer8* dsBuf8;
    IDirectSound3DBuffer8* ds3DBuf8;    // optional interface to direct sound 3d sound buffer
    nSound2 soundObj;
    int validDataOffset;                // start of chunk containing valid data
    int triggerWriteOffset;             // fill buffer if play cursor past this point
    int nextWriteOffset;                // write new stream data at this position
    DWORD curPlayOffset;                // the current playback offset (updated in Update())
    DWORD curWriteOffset;               // the current write offset (updated in Update())

    bool endRangeValid;
    int endRangeMin;
    int endRangeMax;
};
//------------------------------------------------------------------------------
#endif
