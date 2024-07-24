#define N_IMPLEMENTS nSoundNode
//------------------------------------------------------------------------------
//  nsoundnode_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio/nsoundnode.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelset.h"
#include "gfx/nchannelcontext.h"
#include "audio/naudioserver2.h"
#include "gfx/ngfxserver.h"
#include "kernel/nfileserver2.h"

nNebulaScriptClass(nSoundNode, "nvisnode");

//------------------------------------------------------------------------------
/**

*/
nSoundNode::nSoundNode() :
    refAudio(kernelServer, this),
    refGfx(kernelServer, this),
    refFile(kernelServer, this),
    refStream(this),
    soundId(0),
    priFactor(1.0f),
    velChannelIndex(0),
    looping(false),
    doppler(true),
    startRequested(false),
    stopRequested(false)
{
    this->refAudio = "/sys/servers/audio";
    this->refGfx   = "/sys/servers/gfx";
    this->refFile  = "/sys/servers/file2";
    this->velChannelName = "velocity";
    this->triggerChannelName = "soundtrigger";
}

//------------------------------------------------------------------------------
/**
    19-May-00   floh    created
*/
nSoundNode::~nSoundNode()
{
    if (this->refStream.isvalid())
    {
        this->refStream->Release();
        this->refStream.invalidate();
    }
}

//-------------------------------------------------------------------
/**
    19-May-00   floh    created
*/
void
nSoundNode::SetFile(const char* name)
{
    n_assert(name);
    this->filePath.Set(this->refFile.get(), name, this->refFile->GetCwd());
    
    // release old stream object
    if (this->refStream.isvalid())
    {
        this->refStream->Release();
        this->refStream.invalidate();
        this->sound.SetStream(0);
    }
}

//-------------------------------------------------------------------
/**
    25-May-00   floh    created
*/
const char*
nSoundNode::GetFile()
{
    return this->filePath.GetPath();
}

//-------------------------------------------------------------------
/**
    05-Jun-00   floh    created
*/
void
nSoundNode::SetTriggerChannel(const char* name)
{
    n_assert(name);
    this->triggerChannelName = name;
    this->NotifyChannelSetDirty();
}

//-------------------------------------------------------------------
/**
    05-Jun-00   floh    created
*/
const char*
nSoundNode::GetTriggerChannel()
{
    return this->triggerChannelName.IsEmpty() ? 0 : this->triggerChannelName.Get();
}

//-------------------------------------------------------------------
/**
    05-Jun-00   floh    created
*/
void
nSoundNode::SetVelocityChannel(const char* name)
{
    n_assert(name);
    this->velChannelName = name;
    this->NotifyChannelSetDirty();
}

//-------------------------------------------------------------------
/**
    05-Jun-00   floh    created
*/
const char*
nSoundNode::GetVelocityChannel()
{
    return this->velChannelName.IsEmpty() ? 0 : this->velChannelName.Get();
}

//-------------------------------------------------------------------
/**
    27-Jun-01   floh    created
*/
void
nSoundNode::AttachChannels(nChannelSet* chnSet)
{
    nChannelServer* chnServer = this->refChannel.get();
    this->velChannelIndex = chnSet->AddChannel(chnServer, this->velChannelName.Get());
    this->triggerChannelIndex = chnSet->AddChannel(chnServer, this->triggerChannelName.Get());
    nVisNode::AttachChannels(chnSet);
}

//-------------------------------------------------------------------
/**
    22-May-00   floh    created
*/
bool
nSoundNode::Attach(nSceneGraph2* sceneGraph)
{
    if (nVisNode::Attach(sceneGraph))
    {
        sceneGraph->AttachSoundNode(this);
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
/**
    22-May-00   floh    created
    25-May-00   floh    + fake velocity computation added
    23-Jun-00   floh    + ambient handling
    14-Jul-00   floh    + nodoppler handling
*/
void
nSoundNode::Compute(nSceneGraph2* sceneGraph)
{
    nVisNode::Compute(sceneGraph);

    // get trigger channel contents
    nChannelContext* chnContext = sceneGraph->GetChannelContext();
    n_assert(chnContext);
    float soundTrigger = chnContext->GetChannel1f(this->triggerChannelIndex);

    // initialize unique sound id
    if (0 == this->soundId)
    {
        this->soundId = this->refAudio->GetUniqueId();
    }

    // initialize the stream object on demand
    if (!this->refStream.isvalid())
    {
        const char* absPath = this->filePath.GetAbsPath();
        this->refStream = this->refAudio->NewStream(absPath, this->looping);
        this->sound.SetStream(this->refStream.get());
    }
    n_assert(this->refStream.isvalid());

    // set current sound "context id"
    this->sound.SetId((sceneGraph->GetRenderContext()<<16) | this->soundId);

    // compute world space position, orientation and velocity
    float distToListener = 0.0f;
    if (!this->sound.GetAmbient())
    {
        matrix44 viewerMatrix;
        this->refGfx->GetMatrix(N_MXM_VIEWER, viewerMatrix);

        // compute world space position of the sound source
        const matrix44& modelView = sceneGraph->GetTransform();
        vector3 viewSpacePos = modelView.pos_component();
        distToListener = viewSpacePos.len(); 
        
        // do nothing if distToListener > maxDist
        if (distToListener > this->sound.GetMaxDistance())
        {
            return;
        }

        // update position and velocity
        this->sound.SetPosition(viewerMatrix * viewSpacePos);
        
        vector3 vel(0.0f, 0.0f, 0.0f);
        if (this->doppler)
        {
            chnContext->GetChannel3f(this->velChannelIndex, vel.x, vel.y, vel.z);
        }
        this->sound.SetVelocity(vel);
    }

    // compute priority
    float pri;
    if (distToListener > 0.0f) 
    {
        float div;
        if (distToListener < 1.0f) 
        {
            div = 1.0f;
        }
        else 
        {
            div = 1.0f / distToListener;
        }
        pri = div * this->priFactor;
    } 
    else 
    {
        pri = this->priFactor;
    }
    this->sound.SetPriority(pri);

    // check if we need to start or update the sound
    nAudioServer2* audio = this->refAudio.get();
    if ((soundTrigger > 0.99f) || (this->startRequested))
    {
        audio->StartSound(this->sound);
    }
    else
    {
        audio->UpdateSound(this->sound);
    }

    // stop sound on request
    if (this->stopRequested)
    {
        audio->StopSound(this->sound);
    }

    // reset the start/stop request flags
    this->startRequested = false;
    this->stopRequested = false;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
