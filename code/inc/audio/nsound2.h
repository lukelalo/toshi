#ifndef N_SOUND2_H
#define N_SOUND2_H
//------------------------------------------------------------------------------
/**
    @class nSound2
    @ingroup NebulaAudioModule    
    @brief nSound2 holds the attributes for a sound.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_STREAM_H
#include "audio/nstream.h"
#endif

//------------------------------------------------------------------------------
class nStream;
class nSound2
{
public:
    /// states
    enum State
    {
        FREE,
        STARTED,
        UPDATED,
        ORPHANED,
        STOPPED,
    };

    /// constructor
    nSound2();
    /// destructor
    ~nSound2();
    /// assignment operator
    nSound2& operator=(const nSound2& rhs);
    /// equality operator
    bool operator==(const nSound2& rhs);
    /// clear content
    void Clear();
    /// set identification code
    void SetId(int idCode);
    /// get identification code
    int GetId() const;
    /// set current state
    void SetState(State newState);
    /// get current state
    State GetState() const;
    /// set pointer to streaming data source
    void SetStream(nStream* str);
    /// get pointer to streaming data source
    nStream* GetStream() const;
    /// ambient sound? (attached to listener)
    void SetAmbient(bool b);
    /// get ambient sound flag
    bool GetAmbient() const;
    /// is this a looping sound?
    bool GetLooping() const;
    /// get number of channels in embedded stream
    int GetNumChannels() const;
    /// set overall playback priority
    void SetPriority(float p);
    /// get overall playback priority
    float GetPriority() const;
    /// set playback volume
    void SetVolume(float v);
    /// get playback volume
    float GetVolume() const;
    /// set playback frequency
    void SetFrequency(float f);
    /// get playback frequency
    float GetFrequency() const;
    /// set position
    void SetPosition(const vector3& p);
    /// get position
    const vector3& GetPosition() const;
    /// set velocity
    void SetVelocity(const vector3& v);
    /// get velocity
    const vector3& GetVelocity() const;
    /// set minimum distance (sound doesn't get louder when closer)
    void SetMinDistance(float d);
    /// get minimum distance
    float GetMinDistance() const;
    /// set maximum distance (sound is not hearable beyong that distance)
    void SetMaxDistance(float d);
    /// get maximum distance
    float GetMaxDistance() const;
    /// rewind the stream object
    void RewindStream();
    /// begin a sequence of stream read operations
    void BeginRead(bool crossFade);
    /// read chunk of data from current stream position into provided buffer
    int Read(void* buf, int numBytes);
    /// finish a sequence of stream read operations
    void EndRead();
    /// check for end of stream
    bool EndOfStream();

private:
    bool isAmbient;
    bool inBeginRead;
    bool xFade;
    int id;
    State state;
    nStream* stream;
    int streamPos;
    float priority;
    float volume;
    float frequency;
    vector3 position;
    vector3 velocity;
    float minDist;
    float maxDist;
};

//------------------------------------------------------------------------------
/**
*/
inline
nSound2::nSound2() :
    isAmbient(false),
    inBeginRead(false),
    xFade(false),
    id(0),
    state(FREE),
    stream(0),
    streamPos(0),
    priority(1.0f),
    volume(1.0f),
    frequency(1.0f),
    minDist(50.0),
    maxDist(500.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nSound2::~nSound2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSound2::operator==(const nSound2& rhs)
{
    return (this->id == rhs.id);
}

//------------------------------------------------------------------------------
/**
*/
inline
nSound2&
nSound2::operator=(const nSound2& rhs)
{
    this->isAmbient      = rhs.isAmbient;
    this->id             = rhs.id;
    this->state          = rhs.state;
    this->stream         = rhs.stream;
    this->streamPos      = rhs.streamPos;
    this->priority       = rhs.priority;
    this->volume         = rhs.volume;
    this->frequency      = rhs.frequency;
    this->position       = rhs.position;
    this->velocity       = rhs.velocity;
    this->minDist        = rhs.minDist;
    this->maxDist        = rhs.maxDist;
    return *this;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound2::Clear()
{
    this->isAmbient      = false;
    this->id             = 0;
    this->state          = FREE;
    this->stream         = 0;
    this->priority       = 1.0f;
    this->volume         = 1.0f;
    this->frequency      = 1.0f;
    this->position.set(0.0f, 0.0f, 0.0f);
    this->velocity.set(0.0f, 0.0f, 0.0f);
    this->minDist        = 50.0f;
    this->maxDist        = 500.0f;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound2::SetId(int idCode)
{
    this->id = idCode;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSound2::GetId() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound2::SetState(State newState)
{
    this->state = newState;
}

//------------------------------------------------------------------------------
/**
*/
inline
nSound2::State
nSound2::GetState() const
{
    return this->state;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound2::SetStream(nStream* str)
{
    this->stream = str;
}

//------------------------------------------------------------------------------
/**
*/
inline
nStream*
nSound2::GetStream() const
{
    return this->stream;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound2::SetAmbient(bool b)
{
    this->isAmbient = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSound2::GetAmbient() const
{
    return this->isAmbient;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSound2::GetLooping() const
{
    if (this->stream)
    {
        return this->stream->GetLooping();
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSound2::GetNumChannels() const
{
    if (this->stream)
    {
        return this->stream->GetNumChannels();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound2::SetPriority(float p)
{
    this->priority = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nSound2::GetPriority() const
{
    return this->priority;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound2::SetVolume(float v)
{
    this->volume = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nSound2::GetVolume() const
{
    return this->volume;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound2::SetFrequency(float f)
{
    this->frequency = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nSound2::GetFrequency() const
{
    return this->frequency;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound2::SetPosition(const vector3& p)
{
    this->position = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nSound2::GetPosition() const
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound2::SetVelocity(const vector3& v)
{
    this->velocity = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nSound2::GetVelocity() const
{
    return this->velocity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound2::SetMinDistance(float d)
{
    this->minDist = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nSound2::GetMinDistance() const
{
    return this->minDist;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound2::SetMaxDistance(float d)
{
    this->maxDist = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nSound2::GetMaxDistance() const
{
    return this->maxDist;
}

//------------------------------------------------------------------------------
/**
    Rewind the stream object. Normally happens when the sound object
    has entered the started state.
*/
inline
void
nSound2::RewindStream()
{
    this->streamPos = 0;
}

//------------------------------------------------------------------------------
/**
    Initiate a sequence of stream read commands. This positions the 
    file position at the current position of this sound object.

    @param  crossFade   true if cross fading enabled, generates a smoot blend 
                        between old and new data
*/
inline
void
nSound2::BeginRead(bool crossFade)
{
    n_assert(this->stream);
    n_assert(!this->inBeginRead);
    this->stream->Seek(this->streamPos);
    this->inBeginRead = true;
    this->xFade = crossFade;
    if (crossFade)
    {
        this->stream->BeginXFade();
    }
}

//------------------------------------------------------------------------------
/**
    Read a chunk of data from the data stream into provided memory buffer.
    Return bytes actually read.
*/
inline
int
nSound2::Read(void* buf, int numBytes)
{
    n_assert(buf);
    n_assert(numBytes > 0);
    n_assert(this->stream);
    n_assert(this->inBeginRead);
    return stream->Read((char*) buf, numBytes);
}

//------------------------------------------------------------------------------
/**
    Finish a sequence of stream read commands. This stores the current
    stream position in the nSound object.
*/
inline
void
nSound2::EndRead()
{
    n_assert(this->stream);
    n_assert(this->inBeginRead);
    this->streamPos = stream->Tell();
    this->inBeginRead = false;
    if (this->xFade)
    {
        this->stream->EndXFade();
        this->xFade = false;
    }
}

//------------------------------------------------------------------------------
/**
    Return true if end of stream reached. This only happens with oneshot streams.
*/
inline
bool
nSound2::EndOfStream()
{
    n_assert(this->stream);
    return this->stream->EndOfStream();
}

//------------------------------------------------------------------------------
#endif
