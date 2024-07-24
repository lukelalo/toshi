#ifndef N_SOUNDATTRS_H
#define N_SOUNDATTRS_H
//------------------------------------------------------------------------------
/**
	@class nSoundAttrs
    @ingroup NebulaAudioModule
	@brief hold 3d sound attributes in an inline class

	Similar to a material object, a sound source holds the attributes
	of a single sound emitter in 3d space. 
*/

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

//-------------------------------------------------------------------
class nSoundAttrs {
private:
    float timeStamp;
    float volume;
    float freq;
    vector3 position;
    vector3 velocity;
    float max_distance;
    float min_distance;
    bool ambient;
    bool doppler;
    bool looping;

public:
    nSoundAttrs() 
        : timeStamp(0.0f),
          volume(1.0f),
          freq(1.0f),
          max_distance(1000.0f),
          min_distance(1.0f),
          ambient(false),
          doppler(true),
          looping(false)
    { };

    //---------------------------------------------------------------
    void SetTimestamp(float t)
    {
        this->timeStamp = t;
    };
    float GetTimestamp()
    {
        return this->timeStamp;
    };

    //---------------------------------------------------------------
    void SetAmbient(bool b) 
    {
        ambient = b;
    };
    bool GetAmbient()
    {
        return ambient;
    }

    //---------------------------------------------------------------
    void SetDoppler(bool b)
    {
        doppler = b;
    };
    bool GetDoppler()
    {
        return doppler;
    }

    //---------------------------------------------------------------
    void SetLooping(bool b)
    {
        looping = b;
    };
    bool GetLooping()
    {
        return looping;
    }

    //---------------------------------------------------------------
    void SetVolume(float f) {
        volume = f;
    };
    float GetVolume(void) {
        return volume;
    };

    //---------------------------------------------------------------
    void SetFreq(float f) {
        freq = f;
    };
    float GetFreq(void) {
        return freq;
    };

    //---------------------------------------------------------------
    void SetPosition(vector3& v) {
        position = v;
    };
    void GetPosition(vector3& v) {
        v = position;
    };

    //---------------------------------------------------------------
    void SetVelocity(vector3& v) {
        velocity = v;
    };
    void GetVelocity(vector3& v) {
        v = velocity;
    };

    //---------------------------------------------------------------
    void SetMinDistance(float f) {
        if (f < 0.001f) f=0.001f;
        min_distance = f;
    };
    float GetMinDistance(void) {
        return min_distance;
    };

    //---------------------------------------------------------------
    void SetMaxDistance(float f) {
        if (f < 0.001f) f=0.001f;
        max_distance = f;
    };
    float GetMaxDistance(void) {
        return max_distance;
    };
};
//-------------------------------------------------------------------
#endif
