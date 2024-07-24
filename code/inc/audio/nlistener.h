#ifndef N_LISTENER_H
#define N_LISTENER_H
//------------------------------------------------------------------------------
/**
	@class nListener
    @ingroup NebulaAudioModule
	@brief encapsulate listener and environmental audio attributes

	Similar to an ambient light source, nListener keeps global
	parameters for audio playback. There should only be one
	nListener object per scene and frame, otherwise they will
	overwrite each other in an undefined behaviour.

	The listener is always positioned at (0,0,0) and looks
	towards the negative z axis. This conforms with the OpenGL
	camera model and fits nicely into Nebula scene graph handling.
*/
#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif
class nListener {
    float doppler_factor;
    float rolloff_factor;
    vector3 velocity;

public:
    nListener()
        : doppler_factor(1.0f),
          rolloff_factor(1.0f)
    { };

    //---------------------------------------------------------------
    void SetDopplerFactor(float f) 
    { 
        doppler_factor = f;
    };
    float GetDopplerFactor(void) {
        return doppler_factor;
    };
    //---------------------------------------------------------------
    void SetRolloffFactor(float f) {
        rolloff_factor = f;
    };
    float GetRolloffFactor(void) {
        return rolloff_factor;
    };
    //---------------------------------------------------------------
    void SetVelocity(vector3& v) {
        velocity = v;
    };
    void GetVelocity(vector3& v) {
        v = velocity;
    };
};
//-------------------------------------------------------------------
#endif
