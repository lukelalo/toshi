#define N_IMPLEMENTS nListenerNode
//------------------------------------------------------------------------------
//  nlistenernode_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "audio/nlistenernode.h"
#include "audio/naudioserver2.h"
#include "gfx/nchannelset.h"
#include "gfx/nchannelcontext.h"
#include "gfx/ngfxserver.h"

nNebulaScriptClass(nListenerNode, "nvisnode");

//------------------------------------------------------------------------------
/**
    24-May-99   floh    created
*/
nListenerNode::nListenerNode() :
    refAudio(kernelServer, this),
    refGfx(kernelServer, this),
    velChannelIndex(0),
    velocityCap(50.0f)
{
    this->refAudio       = "/sys/servers/audio";
    this->refGfx         = "/sys/servers/gfx";
    this->velChannelName = "lvelocity";
}

//-------------------------------------------------------------------
/**
    24-May-00   floh    created
*/
nListenerNode::~nListenerNode()
{
    // empty
}

//-------------------------------------------------------------------
/**
    24-May-00   floh    created
*/
void
nListenerNode::SetVelocityChannel(const char* name)
{
    n_assert(name);
    this->velChannelName = name;
    this->NotifyChannelSetDirty();
}

//-------------------------------------------------------------------
/**
    24-May-00   floh    created
*/
const char*
nListenerNode::GetVelocityChannel()
{
    return this->velChannelName.IsEmpty() ? 0 : this->velChannelName.Get();
}

//-------------------------------------------------------------------
/**
    24-May-00   floh    created
*/
void
nListenerNode::AttachChannels(nChannelSet* chnSet)
{
    nChannelServer* chnServer = this->refChannel.get();
    this->velChannelIndex = chnSet->AddChannel(chnServer, this->velChannelName.Get());
    nVisNode::AttachChannels(chnSet);
}

//-------------------------------------------------------------------
/**
    24-May-00   floh    created
    27-Jun-01   floh    new scenegraph handling
*/
bool 
nListenerNode::Attach(nSceneGraph2* sceneGraph)
{
    if (nVisNode::Attach(sceneGraph)) 
    {
        sceneGraph->AttachAmbientListenerNode(this);
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
/**
    24-May-00   floh    created
    25-May-00   floh    compute fake velocity?
*/
void
nListenerNode::Compute(nSceneGraph2* sceneGraph)
{
    nVisNode::Compute(sceneGraph);

    // get listener velocity
    vector3 vel(0.0f, 0.0f, 0.0f);
    nChannelContext* chnContext = sceneGraph->GetChannelContext();
    n_assert(chnContext);
    chnContext->GetChannel3f(this->velChannelIndex, vel.x, vel.y, vel.z);

    // cap velocity against velocity cap
    float speed = vel.len();
    if (speed > this->velocityCap)
    {
        float velMul = this->velocityCap / speed;
        vel *= velMul;
    }
    this->listener.SetVelocity(vel);

    // get listener position in view space (same as viewer position)
    matrix44 viewer;
    this->refGfx->GetMatrix(N_MXM_VIEWER, viewer);
    this->listener.SetMatrix(viewer);

    // update the audio server listener
    this->refAudio->UpdateListener(this->listener);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
