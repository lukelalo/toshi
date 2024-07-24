#ifndef N_LISTENERNODE_H
#define N_LISTENERNODE_H

/**
	@class nListenerNode
    @ingroup NebulaAudioModule
	@brief encapsulate listener attributes into nvisnode

	An nlistenernode is similar to an ambient light source, 
	it describes the position of the listener as well as 
	environmental audio attributes.
*/



#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_LISTENER2_H
#include "audio/nlistener2.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#undef N_DEFINES
#define N_DEFINES nListenerNode
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nAudioServer2;
class nListenerNode : public nVisNode 
{
public:
    /// constructor
    nListenerNode();
    /// destructor
    virtual ~nListenerNode();
    /// attach channels to channel set
    virtual void AttachChannels(nChannelSet* chnSet);
    /// attach to scene graph
    virtual bool Attach(nSceneGraph2* sceneGraph);
    /// update internal state and "render"
    virtual void Compute(nSceneGraph2* sceneGraph);
    /// object persistency
    virtual bool SaveCmds(nPersistServer* fileServer);
    /// set the channel which provides velocity in global space
    void SetVelocityChannel(const char* name);
    /// get the channel which provides velocity
    const char* GetVelocityChannel();
    /// set the doppler factor
    void SetDopplerFactor(float f);
    /// get the doppler factor
    float GetDopplerFactor();
    /// set the rolloff factor
    void SetRolloffFactor(float f);
    /// get the rolloff factor
    float GetRolloffFactor();
    /// set a velocity cap
    void SetVelocityCap(float maxVel);
    /// get velocity cap
    float GetVelocityCap();

    static nKernelServer* kernelServer;

private:
    nAutoRef<nAudioServer2> refAudio;
    nAutoRef<nGfxServer> refGfx;
    nListener2 listener;    
    nString velChannelName;
    int velChannelIndex;
    float velocityCap;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nListenerNode::SetDopplerFactor(float f)
{
    this->listener.SetDopplerFactor(f);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nListenerNode::GetDopplerFactor()
{
    return this->listener.GetDopplerFactor();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nListenerNode::SetRolloffFactor(float f)
{
    this->listener.SetRolloffFactor(f);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nListenerNode::GetRolloffFactor()
{
    return this->listener.GetRolloffFactor();
}


//------------------------------------------------------------------------------
/**
*/
inline
void
nListenerNode::SetVelocityCap(float maxVel)
{
    n_assert(maxVel > 0.001f);
    this->velocityCap = maxVel;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nListenerNode::GetVelocityCap()
{
    return this->velocityCap;
}

//------------------------------------------------------------------------------
#endif
