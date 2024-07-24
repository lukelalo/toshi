#ifndef N_AUDIOSERVER2_H
#define N_AUDIOSERVER2_H
//------------------------------------------------------------------------------
/**
    @class nAudioServer2
    @ingroup NebulaAudioModule    
    @brief Audio server base class.

    It needs to be subclassed to actually have a working audio subsystem
    (there is only one subclass at the moment, nDSoundServer2, which links
    to DirectSound). The architecture is very similar to the gfx servers,
    there is a 'audio frame', during which audio sources are attached to
    the 'audio scene'. Once the 'audio scene' has been described, audio
    for this frame is updated.

    The life of a nSound2 object has the following states:

      - @c FREE        - the sound object is free and can be used
      - @c STARTED     - the sound object has started to play in this frame
      - @c UPDATED     - the sound object has been updated in this frame
      - @c STOPPED     - the sound object has been stopped in this frame
      - @c ORPHANED    - the sound object has not been stopped or updated in this frame

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_SOUND2_H
#include "audio/nsound2.h"
#endif

#ifndef N_LISTENER2_H
#include "audio/nlistener2.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nAudioServer2
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nAudioServer2 : public nRoot
{
public:
    /// constructor
    nAudioServer2();
    /// destructor
    virtual ~nAudioServer2();

    /// set the audio parameters
    virtual void SetMode(const char* modeStr);
    /// get the audio parameters
    virtual const char* GetMode();
    /// set the master volume
    virtual void SetMasterVolume(float v);
    /// get the master volume
    virtual float GetMasterVolume();
    /// open the audio device
    virtual bool OpenAudio();
    /// close the audio device
    virtual void CloseAudio();
    /// get a unique id
    virtual int GetUniqueId();

    /// begin an audio frame
    virtual void BeginScene(double time);
    /// update the listener attributes for the current frame
    virtual void UpdateListener(const nListener2& l);
    /// start a sound
    virtual void StartSound(const nSound2& s);
    /// update a playing sound
    virtual void UpdateSound(const nSound2& s);
    /// explicetly stop a sound
    virtual void StopSound(const nSound2& s);
    /// finish the audio scene
    virtual void EndScene();
    /// flush the audio queue with fade in and fade out
    virtual void FlushAudio(float fadeoutTime, float fadeinTime);
    /// mute audio on/off
    virtual void MuteAudio(bool b);

    /// create an audio stream object
    virtual nStream* NewStream(const char* streamName, bool looping);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:
    /// priority-insert a play candidate into the play candidates array
    nSound2* AddPlayCandidate(const nSound2& s);
    /// convert a stream name into a resource id string
    const char* GetResourceId(const char* name, char* buf, int bufSize);
    /// get a mode tag value from a mode string
    const char* GetModeTag(const char *desc, const char *key, char *buf, int buf_size);

protected:
    enum nSpeakerConfig
    {
        FIVEPOINTONE,
        HEADPHONE,
        MONO,
        QUAD,
        STEREO,
        SURROUND,
    };

    int uniqueId;
    int modeDevice;
    int modeBitsPerSample;
    int modeSamplesPerSec;
    int modeChannels;
    nSpeakerConfig modeSpeakerConfig;
    nRef<nRoot> refStreams;
    bool audioOpen;
    bool inBeginScene;
    float muteModulator;
    float masterVolume;
    nListener2 listener;

    double timeStamp;           // current timestamp
    double flushStarted;        // timestamp when flush has been started

    bool flushActive;           // true for the entire flush process (FadeOut/Hold/FadeIn)
    bool hasBeenFlushed;        // true if sound source have been flushed
    double fadeoutTimeStamp;
    double fadeinTimeStamp;
};

//------------------------------------------------------------------------------
#endif
