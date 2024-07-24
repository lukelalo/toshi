#ifndef N_SOUNDNODE_H
#define N_SOUNDNODE_H
//------------------------------------------------------------------------------
/**
	@class nSoundNode
    @ingroup NebulaAudioModule
	@brief embed nSoundSource into a nVisNode subclass for
		   inclusion into a visual hierarchy
*/
#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_RSRCPATH_H
#include "misc/nrsrcpath.h"
#endif

#ifndef N_MATHLIB_H
#include "mathlib/vector.h"
#endif

#ifndef N_SOUND2_H
#include "audio/nsound2.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#undef N_DEFINES
#define N_DEFINES nSoundNode
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nAudioServer2;
class N_PUBLIC nSoundNode : public nVisNode
{
public:
    /// constructor
    nSoundNode();
    /// destructor
    virtual ~nSoundNode();
    /// attach animation channels to channel set
    virtual void AttachChannels(nChannelSet* chnSet);
    /// attach to scene graph
    virtual bool Attach(nSceneGraph2* sceneGraph);
    /// update internal state and "render"
    virtual void Compute(nSceneGraph2* sceneGraph);
    /// object persistency
    virtual bool SaveCmds(nPersistServer* fileServer);
    /// set the audio trigger channel name
    void SetTriggerChannel(const char* name);
    /// get the audio trigger channel name
    const char* GetTriggerChannel();
    /// set velocity channel name
    void SetVelocityChannel(const char* name);
    /// get velocity channel name
    const char* GetVelocityChannel();
    /// set stream file name
    void SetFile(const char* name);
    /// get stream file name
    const char* GetFile();
    /// set sound cue name
    void SetSoundCue(const char* name);
    /// get sound cue name
    const char* GetSoundCue() const;
    /// set the ambient sound flag
    void SetAmbient(bool b);
    /// get ambient flag
    bool GetAmbient();
    /// enable/disable dopper effect
    void SetDoppler(bool b);
    /// get doppler status
    bool GetDoppler();
    /// set looping flag
    void SetLooping(bool b);
    /// get looping status
    bool GetLooping();
    /// set priority factor
    void SetPriority(float f);
    /// get priority factor
    float GetPriority();
    /// set volume (0.0 .. 1.0f)
    void SetVolume(float f);
    /// get volume
    float GetVolume();
    /// set frequency factor
    void SetFrequency(float f);
    /// get frequency factor
    float GetFrequency();
    /// set minimum/maximum distance
    void SetMinMaxDist(float minDist, float maxDist);
    /// get minimum/maximum distance
    void GetMinMaxDist(float& minDist, float& maxDist);
    /// start the sound
    void Start();
    /// stop the sound
    void Stop();

    static nKernelServer* kernelServer;

private:
    nAutoRef<nAudioServer2> refAudio;
    nAutoRef<nGfxServer> refGfx;
    nAutoRef<nFileServer2> refFile;
    nRef<nStream> refStream;
    nSound2 sound;
    nRsrcPath filePath;         // stream file path
    int soundId;                // a unique id for this sound
    float priFactor;            // priority factor
    nString velChannelName;     // velocity channel name
    nString triggerChannelName; // playback trigger channel name
    int velChannelIndex;        // velocity channel index
    int triggerChannelIndex;    // playback trigger channel name
    bool looping;               // looping flag
    bool doppler;               // doppler effect flag
    bool startRequested;
    bool stopRequested;

    nString soundCue;
};

//------------------------------------------------------------------------------
/*
*/
inline
void
nSoundNode::SetAmbient(bool b)
{
    this->sound.SetAmbient(b);
}

//------------------------------------------------------------------------------
/*
*/
inline
bool
nSoundNode::GetAmbient()
{
    return this->sound.GetAmbient();
}

//------------------------------------------------------------------------------
/*
*/
inline
void
nSoundNode::SetDoppler(bool b)
{
    this->doppler = b;
}

//------------------------------------------------------------------------------
/*
*/
inline
bool
nSoundNode::GetDoppler()
{
    return this->doppler;
}

//------------------------------------------------------------------------------
/*
*/
inline
void
nSoundNode::SetLooping(bool b)
{
    this->looping = b;
}

//------------------------------------------------------------------------------
/*
*/
inline
bool
nSoundNode::GetLooping()
{
    return this->looping;
}

//------------------------------------------------------------------------------
/*
*/
inline
void
nSoundNode::SetPriority(float f)
{
    this->priFactor = f;
}

//------------------------------------------------------------------------------
/*
*/
inline
float
nSoundNode::GetPriority()
{
    return this->priFactor;
}

//------------------------------------------------------------------------------
/*
*/
inline
void
nSoundNode::SetVolume(float f)
{
    this->sound.SetVolume(f);
}

//------------------------------------------------------------------------------
/*
*/
inline
float
nSoundNode::GetVolume()
{
    return this->sound.GetVolume();
}

//------------------------------------------------------------------------------
/*
*/
inline
void
nSoundNode::SetFrequency(float f)
{
    this->sound.SetFrequency(f);
}

//------------------------------------------------------------------------------
/*
*/
inline
float
nSoundNode::GetFrequency()
{
    return this->sound.GetFrequency();
}

//------------------------------------------------------------------------------
/*
*/
inline
void
nSoundNode::SetMinMaxDist(float minDist, float maxDist)
{
    this->sound.SetMinDistance(minDist);
    this->sound.SetMaxDistance(maxDist);
}

//------------------------------------------------------------------------------
/*
*/
inline
void
nSoundNode::GetMinMaxDist(float& minDist, float& maxDist)
{
    minDist = this->sound.GetMinDistance();
    maxDist = this->sound.GetMaxDistance();
}

//------------------------------------------------------------------------------
/*
*/
inline
void
nSoundNode::SetSoundCue(const char* cue)
{
    this->soundCue = cue;
}

//------------------------------------------------------------------------------
/*
*/
inline
const char*
nSoundNode::GetSoundCue() const
{
    return this->soundCue.IsEmpty() ? 0 : this->soundCue.Get();
}

//------------------------------------------------------------------------------
/*
*/
inline
void
nSoundNode::Start()
{
    this->startRequested = true;
}

//------------------------------------------------------------------------------
/*
*/
inline
void
nSoundNode::Stop()
{
    this->stopRequested = true;
}

//------------------------------------------------------------------------------
#endif
