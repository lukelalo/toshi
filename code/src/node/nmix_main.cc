#define N_IMPLEMENTS nMixer
//------------------------------------------------------------------------------
//  nmix_main.cc
//  (C) 1999,2001 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelset.h"
#include "gfx/nchannelcontext.h"
#include "node/nmixer.h"

nNebulaScriptClass(nMixer, "nvisnode");

//-------------------------------------------------------------------
/**
*/
nMixer::nMixer() :
    numConnects(0),
    numSources(0),
    normalize(true),
    connectsInitialized(false)
{
    this->normalize = true;
    this->SetFlags(N_FLAG_SAVEUPSIDEDOWN);
    int i;
    for (i = 0; i < MAXSOURCES; i++)
    {
        this->sources[i].Initialize(kernelServer,this);
    }
}

//-------------------------------------------------------------------
/**
*/
nMixer::~nMixer()
{ 
    // empty
}

//-------------------------------------------------------------------
/**
    Attach to parent object as depend node.
*/
void nMixer::Initialize(void)
{
    nVisNode *p = (nVisNode *) this->GetParent();
    if (p && p->IsA(kernelServer->FindClass("nvisnode"))) 
    {
        p->DependsOn(this);
    }
    nVisNode::Initialize();
}

//-------------------------------------------------------------------
/**
    Attach connection weight channels to channel set.
    
    @param  chnSet      the channel set object
*/
void
nMixer::AttachChannels(nChannelSet* chnSet)
{
    n_assert(chnSet);
    int i;
    for (i = 0; i < this->numSources; i++)
    {
        nMixerSource* curSource = &(this->sources[i]);
        int localIndex = chnSet->AddChannel(this->refChannel.get(), curSource->GetChannelName());
        curSource->SetChannelIndex(localIndex);
    }
    nVisNode::AttachChannels(chnSet);
}

//-------------------------------------------------------------------
/**
*/
void 
nMixer::SetNormalize(bool b)
{
    this->normalize = b;
}

//-------------------------------------------------------------------
/**
*/
bool 
nMixer::GetNormalize(void)
{
    return this->normalize;
}

//-------------------------------------------------------------------
/**
    Start defining source objects and their weight channels.

    @param  num     number of source objects
*/
void
nMixer::BeginSources(int num)
{
    n_assert((num >= 0) && (num < MAXSOURCES));
    this->numSources = num;
}

//-------------------------------------------------------------------
/**
    Define a source object and its weight channel.

    @param  index       current index
    @param  srcObj      name of source object (must be direct child!)
    @param  srcChannel  name of weight channel
*/
void
nMixer::SetSource(int index, const char* srcObj, const char* srcChannel)
{
    n_assert((index >= 0) && (index < this->numSources));
    n_assert(srcObj);
    n_assert(srcChannel);
    this->sources[index].Clear();
    this->sources[index].SetSourceName(srcObj);
	this->sources[index].SetChannelName(srcChannel);
}

//-------------------------------------------------------------------
/**
*/
void
nMixer::EndSources()
{
    this->NotifyChannelSetDirty();
}

//-------------------------------------------------------------------
/**
    @return     number of mixer sources
*/
int
nMixer::GetNumSources()
{
    return this->numSources;
}

//-------------------------------------------------------------------
/**
    @param  index       [in] source index
    @param  srcObj      [out] path name of source object
    @param  srcChannel  [out] name of source channel
*/
void
nMixer::GetSource(int index, const char*& srcObj, const char*& srcChannel)
{
    n_assert((index >= 0) && (index < this->numSources));
    srcObj     = this->sources[index].GetSourceName();
    srcChannel = this->sources[index].GetChannelName();
}

//-------------------------------------------------------------------
/**
    @param  num     number of connections
*/
void
nMixer::BeginConnects(int num)
{
    n_assert((num >= 0) && (num < MAXCONNECTS));
    this->numConnects = num;
    this->connectsInitialized = false;
}

//-------------------------------------------------------------------
/**
    @param  index       current index
    @param  tarCmd      script command to invoke on target object
    @param  srcCmd      script command to invoke on source object
*/
void
nMixer::SetConnect(int index, const char* tarCmd, const char* srcCmd)
{
    n_assert((index >= 0) && (index < this->numConnects));
    this->connects[index].SetCmdNames(tarCmd, srcCmd);
}

//-------------------------------------------------------------------
/**
*/
void
nMixer::EndConnects()
{
    // empty
}

//-------------------------------------------------------------------
/**
    @return     number of connections
*/
int
nMixer::GetNumConnects()
{
    return this->numConnects;
}

//-------------------------------------------------------------------
/**
    @param  index   [in] index of connection of interest
    @param  tarCmd  [out] script command to invoke on target object
    @param  srcCmd  [out] script command to invoke on source object    
*/
void
nMixer::GetConnect(int index, const char*& tarCmd, const char*& srcCmd)
{
    n_assert((index >= 0) && (index < this->numConnects));
    tarCmd = this->connects[index].GetTarCmdName();
    srcCmd = this->connects[index].GetSrcCmdName();
}

//-------------------------------------------------------------------
/**
    Perform the actual mixing. First, the current weights are
    gathered for all connections by reading the appropriate
    channels. The weight normalization is performed (if enabled).
    Then for each connection, the script command is invoked on
    the source object, which returns the actual values to mix,
    then mixing happens, and the script command to the target
    object is initialized and invoked.

    @param  sceneGraph  pointer to scene graph object
*/
void
nMixer::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    // first, hand Compute() to super class
    nVisNode::Compute(sceneGraph);

    // if not happened yet, initialize connections
    if (!this->connectsInitialized)
    {
        kernelServer->PushCwd(this);
        int i;
        for (i = 0; i < this->numConnects; i++)
        {
            this->connects[i].InitCmdProtos((nVisNode*) this->GetParent(), this->sources[0].GetSource());
        }
        this->connectsInitialized = true;
        kernelServer->PopCwd();
    }

    // get channel context object
    nChannelContext* chnContext = sceneGraph->GetChannelContext();
    n_assert(chnContext);

	// read weights and invoke Compute() on those sources whose
	// weights are greater 0.0
    float weights[MAXSOURCES];
    float w_sum = 0.0f;
    int i;
	for (i = 0; i < this->numSources; i++) 
	{
        nMixerSource* curSource = &(this->sources[i]);
		float w = chnContext->GetChannel1f(curSource->GetChannelIndex());
		weights[i] = w;
		w_sum += w;
	}

    // normalize weights if sum of weight is different from one
    if ((w_sum>=TINY) && (fabs(1.0f-w_sum)>TINY) && this->GetNormalize()) 
	{
        for (i = 0; i < this->numSources; i++) 
        {
            weights[i] /= w_sum;
        }
    }

    // for each connection, gather source values, mix them, and 
    // route them to the target object
    for (i = 0; i < this->numConnects; i++)
    {
        nMixerConnect* curConnect = &(this->connects[i]);
        nCmd* srcCmd = curConnect->GetSrcCmdProto()->NewCmd();
        int numArgs  = curConnect->GetNumArgs();

        // gather source values and get mixed result
        float val[MAXARGS];
        memset(val, 0, sizeof(val));
        int j,k;
        for (j = 0; j < this->numSources; j++)
        {
            float curWeight = weights[j];

            // skip sources with zero weight
            if (curWeight > 0.0f)
            {
                nMixerSource* curSource = &(this->sources[j]);
                nVisNode* srcObj = curSource->GetSource();

                // invoke src command on src object and accumulate weighted result
                srcObj->Dispatch(srcCmd);
                srcCmd->Rewind();
                for (k = 0; k < numArgs; k++)
                {
                    val[k] += srcCmd->Out()->GetF() * curWeight;
                }
                srcCmd->Rewind();
            }
        }

        // fill target command and invoke on target object (our parent)
        nCmd* tarCmd = curConnect->GetTarCmdProto()->NewCmd();
        for (k = 0; k < numArgs; k++)
        {
            tarCmd->In()->SetF(val[k]);
        }
        this->GetParent()->Dispatch(tarCmd);

        // release nCmd objects for this connection
        curConnect->GetSrcCmdProto()->RelCmd(srcCmd);
        curConnect->GetTarCmdProto()->RelCmd(tarCmd);
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
        
