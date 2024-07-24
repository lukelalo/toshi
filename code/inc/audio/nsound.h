#ifndef N_SOUND_H
#define N_SOUND_H
//------------------------------------------------------------------------------
/**
	@class nSound
    @ingroup NebulaAudioModule
	@brief completly describe a sound emitting object

	An nSound object embeds:

	- an nSoundAttrs object which holds 3d attributes
	  (with access methods)
	- a pointer to a nStream object which defines the data source,
	  the stream object may be shared between several sound
	  emitters
	- a unique 32 bit id
*/
#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_SOUNDATTRS_H
#include "audio/nsoundattrs.h"
#endif

//--------------------------------------------------------------------
class nStream;
class N_PUBLIC nSound {
public:
    enum nSoundState {
        NONE,
        START,
        UPDATE,
    };

protected:
    int id;                 // a 32 bit unique id for this sound
    float timestamp;        // timestamp of last update
    nStream *stream;        // the data source for this sound (shared)
    nSoundAttrs attrs;      // the sound attributes for this sound
    nSoundState state;
    float priority;

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nSound() 
    : id(0),
      stream(NULL),
      state(NONE),
      priority(0.0f)
    { };

    void SetState(nSoundState s)        { state = s; };
    nSoundState GetState(void)          { return state; };
    void SetTimeStamp(float t)          { timestamp = t; };
    float GetTimeStamp(void)            { return timestamp; };
    void SetStream(nStream *str)        { stream = str; };
    nStream *GetStream(void)            { return stream; };
    void SetId(int _id)                 { this->id = _id;  };
    int GetId(void)                     { return this->id; };
    void SetPriority(float p)           { priority = p; };
    float GetPriority(void)             { return priority; };
    void SetVolume(float f)             { attrs.SetVolume(f); };
    float GetVolume(void)               { return attrs.GetVolume(); };
    void SetFreq(float f)               { attrs.SetFreq(f); };
    float GetFreq(void)                 { return attrs.GetFreq(); };
    void SetPosition(vector3& v)        { attrs.SetPosition(v); };
    void GetPosition(vector3& v)        { attrs.GetPosition(v); };
    void SetVelocity(vector3& v)        { attrs.SetVelocity(v); };
    void GetVelocity(vector3& v)        { attrs.GetVelocity(v); };
    void SetMinDistance(float f)        { attrs.SetMinDistance(f); };
    float GetMinDistance(void)          { return attrs.GetMinDistance(); };
    void SetMaxDistance(float f)        { attrs.SetMaxDistance(f); };
    float GetMaxDistance(void)          { return attrs.GetMaxDistance(); };
};
//--------------------------------------------------------------------
#endif
