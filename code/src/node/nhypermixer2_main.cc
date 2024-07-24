#define N_IMPLEMENTS nHyperMixer2
//------------------------------------------------------------------------------
/**
    (C) 2001 RadonLabs GmbH
*/
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelset.h"
#include "gfx/nchannelcontext.h"
#include "node/nhypermixer2.h"

//------------------------------------------------------------------------------
/**
*/
nHyperMixer2::nHyperMixer2() :
    numSources(0),
    numConnects(0),
    connectsInitialized(false),
    objPtrsDirty(true)
{
    int src;
    for (src = 0; src < MAXSOURCES; src++)
    {
        this->sources[src].Initialize(ks, this);
    }
    memset(this->tarObjPtrs, 0, sizeof(this->tarObjPtrs));
    memset(this->srcObjPtrs, 0, sizeof(this->srcObjPtrs));
}

//------------------------------------------------------------------------------
/**
*/
nHyperMixer2::~nHyperMixer2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nHyperMixer2::Initialize()
{
    nVisNode* parent = (nVisNode*) this->GetParent();
    if (parent && parent->IsA(ks->FindClass("nvisnode")))
    {
        parent->DependsOn(this);
    }
    nVisNode::Initialize();
}

//------------------------------------------------------------------------------
/**
    Attach connection weight channels to channel set.

    @param  chnSet      the channel set object
*/
void
nHyperMixer2::AttachChannels(nChannelSet* chnSet)
{
    n_assert(chnSet);
    int src;
    for (src = 0; src < this->numSources; src++)
    {
        nMixerSource* curSource = &(this->sources[src]);
        int localIndex = chnSet->AddChannel(this->refChannel.get(), curSource->GetChannelName());
        curSource->SetChannelIndex(localIndex);
    
        // ask current source to attach its channels as well
        curSource->GetSource()->AttachChannels(chnSet);
    }
    nVisNode::AttachChannels(chnSet);
}

//------------------------------------------------------------------------------
/**
    Start defining mixer sources. These are the root objects
    of nVisNode hierarchies to mix, and the names of the associated 
    weight channels.

    @param  num     number of mixer sources
*/
void
nHyperMixer2::BeginSources(int num)
{
    n_assert((num >= 0) && (num < MAXSOURCES));
    this->numSources = num;
}

//------------------------------------------------------------------------------
/**
    Define a mixer source and its associated weight channel.

    @param  index       current index
    @param  srcObj      name of source root object
    @param  srcChannel  name of channel providing the weight
*/
void
nHyperMixer2::SetSource(int index, const char* srcRoot, const char* srcChannel)
{
    n_assert((index >= 0) && (index < this->numSources));
    n_assert(srcRoot);
    n_assert(srcChannel);
    this->sources[index].Clear();
    this->sources[index].SetSourceName(srcRoot);
    this->sources[index].SetChannelName(srcChannel);
}

//------------------------------------------------------------------------------
/**
    Finish defining mixer sources.
*/
void
nHyperMixer2::EndSources()
{
    this->objPtrsDirty = true;
    this->NotifyChannelSetDirty();
}

//------------------------------------------------------------------------------
/**
    @return     number of mixer sources
*/
int
nHyperMixer2::GetNumSources()
{
    return this->numSources;
}

//------------------------------------------------------------------------------
/**
    @param  index       [in] source index
    @param  srcObj      [out] path name of source object
    @param  srcChannel  [out] name of source channel
*/
void
nHyperMixer2::GetSource(int index, const char*& srcObj, const char*& srcChannel)
{
    n_assert((index >= 0) && (index < this->numSources));
    srcObj     = this->sources[index].GetSourceName();
    srcChannel = this->sources[index].GetChannelName();
}

//------------------------------------------------------------------------------
/**
    @param  num     number of connections
*/
void
nHyperMixer2::BeginConnects(int num)
{
    n_assert((num >= 0) && (num < MAXCONNECTS));
    this->numConnects = num;
    this->connectsInitialized = false;
}

//------------------------------------------------------------------------------
/**
    Define a mixer connection, a mixer connection is defined by paths to the
    target and source object (the target object path is relative to the 
    hypermixer object, the source object path is relative to the source root
    object paths defined in the mixer sources), and finally the script commands
    to invoke on the source and target objects to gather and route the data
    to be mixed.

    @param  index           connection index
    @param  tarObjPath      relative path to target object
    @param  tarCmd          script command to invoke on target object
    @param  srcObjPath      relative path to source object (relative to source root)
    @param  srcCmd          script command to invoke on source object
*/
void
nHyperMixer2::SetConnect(
    int index, 
    const char* tarObjPath,
    const char* tarCmd,
    const char* srcObjPath,
    const char* srcCmd)
{
    n_assert((index >= 0) && (index < this->numConnects));
    n_assert(tarObjPath);
    n_assert(tarCmd);
    n_assert(srcObjPath);
    n_assert(srcCmd);
    this->connects[index].Set(tarObjPath, tarCmd, srcObjPath, srcCmd);
}

//------------------------------------------------------------------------------
/**
    Finish defining mixer connections.
*/
void
nHyperMixer2::EndConnects()
{
    this->objPtrsDirty = true;
}

//------------------------------------------------------------------------------
/**
    @return     number of mixer connections
*/
int
nHyperMixer2::GetNumConnects()
{
    return this->numConnects;
}

//------------------------------------------------------------------------------
/**
    @param  index       [in] index of connection to get
    @param  tarObjPath  [out] path to target object
    @param  tarCmd      [out] target command
    @param  srcObjPath  [out] path to source object
    @param  srcCmd      [out] source command
*/
void
nHyperMixer2::GetConnect(
    int index, 
    const char*& tarObjPath,
    const char*& tarCmd,
    const char*& srcObjPath,
    const char*& srcCmd)
{
    n_assert((index >= 0) && (index < this->numConnects));
    tarObjPath = this->connects[index].GetTarObjPath();
    tarCmd     = this->connects[index].GetTarCmd();
    srcObjPath = this->connects[index].GetSrcObjPath();
    srcCmd     = this->connects[index].GetSrcCmd();
}

//------------------------------------------------------------------------------
/**
    Lookup the object pointers, unset the dirty flag. Call from inside
    Compute() if the objPtrsDirty flag is true.
*/
void
nHyperMixer2::LookupObjPointers()
{
    n_assert(true == this->objPtrsDirty);

    // first the source object pointers
    int src, cnc;
    for (src = 0; src < this->numSources; src++)
    {
        // declare source object as current object
        nMixerSource* curSource = &(this->sources[src]);
        ks->PushCwd(curSource->GetSource());
            
        for (cnc = 0; cnc < this->numConnects; cnc++)
        {
            nHyperMixerConnect* curConnect = &(this->connects[cnc]);
            const char* srcObjPath = curConnect->GetSrcObjPath();
            this->srcObjPtrs[cnc][src] = (nVisNode*) ks->Lookup(srcObjPath);
            if (!this->srcObjPtrs[cnc][src])
            {
                char buf[N_MAXPATH];
                n_error("nHyperMixer2: object '%s/%s' does not exist!\n", 
                    curSource->GetSource()->GetFullName(buf, sizeof(buf)), curConnect->GetSrcObjPath());
            }
        }
        ks->PopCwd();
    }

    // then the target object pointers
    ks->PushCwd(this);
    for (cnc = 0; cnc < this->numConnects; cnc++)
    {
        nHyperMixerConnect* curConnect = &(this->connects[cnc]);
        this->tarObjPtrs[cnc] = (nVisNode*) ks->Lookup(curConnect->GetTarObjPath());
        if (!this->tarObjPtrs[cnc])
        {
            n_error("nHyperMixer2: object '%s' does not exist!\n", curConnect->GetTarObjPath());
        }
    }
    ks->PopCwd();

    // undirtify the flag
    this->objPtrsDirty = false;
}

//------------------------------------------------------------------------------
/**
    @param  sceneGraph      pointer to scene graph object
*/
void
nHyperMixer2::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);
    int src, cnc;

    // first, hand Compute() to super class
    nVisNode::Compute(sceneGraph);

    // initialize the object pointers if they are dirty
    if (this->objPtrsDirty)
    {
        this->LookupObjPointers();
    }

    // if not happened yet, initialize connections
    if (!this->connectsInitialized)
    {
        for (cnc = 0; cnc < this->numConnects; cnc++)
        {
            nVisNode* tarObj = this->tarObjPtrs[cnc];
            nVisNode* srcObj = this->srcObjPtrs[cnc][0];
            this->connects[cnc].InitCmdProtos(tarObj, srcObj);
        }
        this->connectsInitialized = true;
    }

    // get channel context object
    nChannelContext* chnContext = sceneGraph->GetChannelContext();
    n_assert(chnContext);

    // gather weights from channels and update source objects
    float weights[MAXSOURCES];
	for (src = 0; src < this->numSources; src++) 
	{
        nMixerSource* curSource = &(this->sources[src]);
		float w = chnContext->GetChannel1f(curSource->GetChannelIndex());
		weights[src] = w;
	}

    // handle each connection
    for (cnc = 0; cnc < this->numConnects; cnc++)
    {
        nHyperMixerConnect* curConnect = &(this->connects[cnc]);
        nCmd* srcCmd = curConnect->GetSrcCmdProto()->NewCmd();
        int numArgs  = curConnect->GetNumArgs();
        nVisNode* tarObj = this->tarObjPtrs[cnc];

        // gather source values and get mixed result
        float val[MAXARGS];
        memset(val, 0, sizeof(val));
        int k;
        for (src = 0; src < this->numSources; src++)
        {
            float curWeight = weights[src];

            // skip sources with zero weight
            if (curWeight > 0.0f)
            {
                nVisNode* srcObj = this->srcObjPtrs[cnc][src];

                // invoke src command on src object and accumulate weighted result
                srcObj->Compute(sceneGraph);
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
        tarObj->Dispatch(tarCmd);

        // release nCmd objects for this connection
        curConnect->GetSrcCmdProto()->RelCmd(srcCmd);
        curConnect->GetTarCmdProto()->RelCmd(tarCmd);
    }
}

//------------------------------------------------------------------------------
