#ifndef N_DSOUNDSERVER2_H
#define N_DSOUNDSERVER2_H
//------------------------------------------------------------------------------
/**
    @class nDSoundServer2
    @ingroup NebulaAudioModule    
    @brief nAudioServer2 derived subclass which wraps DirectSound8

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_AUDIOSERVER2_H
#include "audio/naudioserver2.h"
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <mmreg.h>
#include <dsound.h>

#undef N_DEFINES
#define N_DEFINES nDSoundServer2
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nDSoundCluster;
class nDSoundServer2 : public nAudioServer2
{
public:
    /// constructor
    nDSoundServer2();
    /// destructor
    virtual ~nDSoundServer2();
    /// set the master volume
    virtual void SetMasterVolume(float v);
    /// open the audio device
    virtual bool OpenAudio();
    /// close the audio device
    virtual void CloseAudio();
    /// begin an audio frame
    virtual void BeginScene(double time);
    /// start a sound
    virtual void StartSound(const nSound2& s);
    /// update a playing sound
    virtual void UpdateSound(const nSound2& s);
    /// explicetly stop a sound
    virtual void StopSound(const nSound2& s);
    /// finish the audio scene
    virtual void EndScene();
    /// mute audio on/off
    virtual void MuteAudio(bool b);
    /// flush audio override
    virtual void FlushAudio(float fadeoutTime, float fadeinTime);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:
    /// update listener attributes
    void SetListenerAttributes();
    /// handle an audio server flush
    void HandleFlush();
    /// set dsound's master volume
    void SetDSoundMasterVolume(float f);

    nAutoRef<nEnv> refHwnd;                 // contains hwnd of gfx server window
    IDirectSound8* dSound;                  // interface to DirectSound object
    IDirectSoundBuffer* dsPrim;             // primary buffer
    IDirectSound3DListener8* dsListener;    // the listener

    nDSoundCluster* monoCluster;            // mono (3D-) voice cluster
    nDSoundCluster* stereoCluster;          // stereo voice cluster
};
//------------------------------------------------------------------------------
#endif
