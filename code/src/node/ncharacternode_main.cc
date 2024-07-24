#define N_IMPLEMENTS nCharacterNode
//------------------------------------------------------------------------------
//  ncharacternode_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "node/ncharacternode.h"
#include "character/ncharstatearray.h"
#include "character/ncharstate.h"
#include "anim/nanimserver.h"
#include "character/ncharacterserver.h"
#include "gfx/nscenegraph2.h"
#include "character/ncharacter.h"
#include "gfx/ngfxserver.h"
#include "node/n3dnode.h"
#include "shadow/nshadowserver.h"
#include "kernel/nfileserver2.h"

nNebulaScriptClass(nCharacterNode, "nanimnode");

//------------------------------------------------------------------------------
/**
*/
nCharacterNode::nCharacterNode() :
    refAnimServer(kernelServer, this),
    refCharServer(kernelServer, this),
    refGfxServer(kernelServer, this),
    refShadowServer(kernelServer, this),
    refFileServer(kernelServer, this),
    refPrimServer(kernelServer, this),
    refCurveArray(this),
    nodeKey(-1),
    refStateNodes(0),
    charSkinRenderer(kernelServer, this),
    stateChannelIndex(-1),
    refSkin(kernelServer, this),
    readOnly(false),
    castShadow(false),
    renderSkeleton(false),
    skeletonColor(0.0f,1.0f,1.0f,1.0f),
    numHardPoints(0),
    hardPoints(0)
{
    this->SetFlags(N_FLAG_SAVEUPSIDEDOWN);
    this->refAnimServer    = "/sys/servers/anim";
    this->refCharServer    = "/sys/servers/character";
    this->refGfxServer     = "/sys/servers/gfx";
    this->refShadowServer  = "/sys/servers/shadow";
    this->refFileServer    = "/sys/servers/file2";
    this->refPrimServer    = "/sys/servers/primitive";
    this->stateChannelName = "charstate";
}

//------------------------------------------------------------------------------
/**
*/
nCharacterNode::~nCharacterNode()
{
    // delete previus refStateNode array if exists
    if (this->refStateNodes)
    {
        delete[] this->refStateNodes;
        this->refStateNodes = 0;
    }

    // release all remaining nCharacter object which share my node key
    if (-1 != this->nodeKey)
    {
        this->refCharServer->ReleaseCharacters(this->nodeKey, 0xffff);
    }

    // release the anim curve array object
    if (this->refCurveArray.isvalid())
    {
        this->refCurveArray->Release();
        this->refCurveArray.invalidate();
    }
    
    // release the hard point object array
    if (this->hardPoints)
    {
        delete[] this->hardPoints;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacterNode::SetAnimFile(const char* filename)
{
    n_assert(filename);
    this->animFilePath.Set(this->refFileServer.get(), filename, this->refFileServer->GetCwd());

    // try to share the anim resource
    const char* absPath = this->animFilePath.GetAbsPath();
    nAnimCurveArray* curveArray = this->refAnimServer->FindAnimCurveArray(absPath);
    if (!curveArray)
    {
        curveArray = this->refAnimServer->NewAnimCurveArray(absPath);
        n_assert(curveArray);
        bool curveLoaded = false;
        if (strstr(absPath, ".nax"))
        {
            curveLoaded = curveArray->LoadNax(this->refFileServer.get(), absPath);
        }
        else
        {
            curveLoaded = curveArray->LoadAnim(this->refFileServer.get(), absPath);
        }
        n_assert(curveLoaded);
    }
    this->refCurveArray = curveArray;
}

//------------------------------------------------------------------------------
/**
    Start joint skeleton definition.

    @param  num     number of joints in skeleton
*/
void
nCharacterNode::BeginJoints(int num)
{
    this->charSkeleton.BeginJoints(num);
}

//------------------------------------------------------------------------------
/**
    Add a joint to the skeleton.

    @param  index           joint index
    @param  name            name of joint (used to link joint to the right anim curve)
    @param  parentIndex     index of parent joint (-1 for root joint)
    @param  poseTranslate   the pose translation
    @param  poseRotate      the pose rotation
*/
void
nCharacterNode::AddJoint(int index, 
                     const char* name, 
                     int parentIndex, 
                     const vector3& poseTranslate,
                     const quaternion& poseRotate)
{
    this->charSkeleton.AddJoint(index, name, parentIndex, poseTranslate, poseRotate);
}

//------------------------------------------------------------------------------
/**
    Finish defining joints.
*/
void
nCharacterNode::EndJoints()
{
    this->charSkeleton.EndJoints();
}

//------------------------------------------------------------------------------
/**
*/
int
nCharacterNode::GetNumJoints()
{
    return this->charSkeleton.GetNumJoints();
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacterNode::GetJoint(int index, 
                     const char*& jointName, 
                     int& parentIndex, 
                     vector3& poseTranslate, 
                     quaternion& poseRotate)
{
    const nCharJoint& joint = this->charSkeleton.GetJoint(index);
    jointName     = joint.GetName();
    parentIndex   = joint.GetParentJointIndex();
    poseTranslate = joint.GetPoseTranslate();
    poseRotate    = joint.GetPoseRotate();
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacterNode::BeginStates(int num)
{
    this->charStateArray.SetAnimCurveArray(this->refCurveArray.get());
    this->charStateArray.BeginStates(num);
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacterNode::AddState(int stateIndex, const char* name)
{
    this->charStateArray.AddState(stateIndex, name);
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacterNode::BeginStateAnims(int stateIndex, int numAnims)
{
    this->charStateArray.BeginStateAnims(stateIndex, numAnims);
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacterNode::AddStateAnim(int stateIndex, int animIndex, const char* animName, const char* weightChannel)
{
    this->charStateArray.AddStateAnim(stateIndex, animIndex, &(this->charSkeleton), animName, weightChannel);
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacterNode::EndStateAnims(int stateIndex)
{
    this->charStateArray.EndStateAnims(stateIndex);
    this->NotifyChannelSetDirty();
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacterNode::EndStates()
{
    this->charStateArray.EndStates();

    // reallocate refStateNode array
    if (this->refStateNodes)
    {
        delete[] this->refStateNodes;
        this->refStateNodes = 0;
    }
    int numStates = this->charStateArray.GetNumStates();
    this->refStateNodes = new nRef<nVisNode>[numStates];

    // initialize the per-state nVisNodes
    int curStateIndex;
    for (curStateIndex = 0; curStateIndex < numStates; curStateIndex++)
    {
        const nCharState& curState = this->charStateArray.GetState(curStateIndex);
        this->refStateNodes[curStateIndex].initialize(this);
        this->refStateNodes[curStateIndex].set((nVisNode*) this->Find(curState.GetName()));
    }
}

//------------------------------------------------------------------------------
/**
*/
int
nCharacterNode::GetNumStates()
{
    return this->charStateArray.GetNumStates();
}

//------------------------------------------------------------------------------
/**
*/
int
nCharacterNode::GetNumStateAnims(int stateIndex)
{
    const nCharState& charState = this->charStateArray.GetState(stateIndex);
    return charState.GetNumAnims();
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacterNode::GetState(int stateIndex, const char*& name)
{
    const nCharState& charState = this->charStateArray.GetState(stateIndex);
    name = charState.GetName();
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacterNode::GetStateAnim(int stateIndex, int animIndex, const char*& animName, const char*& weightChannel)
{
    const nCharAnim& charAnim = this->charStateArray.GetState(stateIndex).GetAnim(animIndex);
    animName = charAnim.GetName();
    weightChannel = charAnim.GetWeightChannelName();
}

//------------------------------------------------------------------------------
/**
    Set number of hard points. A hard point maps a joint to a child n3DNode.
    The child n3DNode will track the current position of the joint and
    can be used to attach objects to a joint (e.g. a sword to the characters
    hand).

    @param  num     number of hardpoints
*/
void
nCharacterNode::BeginHardPoints(int num)
{
    n_assert(num > 0);
    n_assert(0 == this->hardPoints);

    this->numHardPoints = num;
    this->hardPoints = new nHardPoint[num];

    int i;
    for (i = 0; i < this->numHardPoints; i++)
    {
        this->hardPoints[i].Initialize(kernelServer, this);
    }
}

//------------------------------------------------------------------------------
/**
    Add a hardpoint. Expects the joint index, and the name of a child n3DNode.

    @param  index           hardpoint index
    @param  jointIndex      index of joint which gives the hardpoint position
    @param  n3DNodeName     name of child n3DNode object which tracks the joints position
*/
void
nCharacterNode::AddHardPoint(int index, int jointIndex, const char* n3DNodeName)
{
    n_assert((index >= 0) && (index < this->numHardPoints));
    n_assert(this->hardPoints);
    n_assert(n3DNodeName);
    this->hardPoints[index].Set(jointIndex, n3DNodeName);
}

//------------------------------------------------------------------------------
/**
    Finish defining hard points.
*/
void
nCharacterNode::EndHardPoints()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
int
nCharacterNode::GetNumHardPoints()
{
    return this->numHardPoints;
}

//------------------------------------------------------------------------------
/**
    Get hard point parameters.

    @param  index           [in] index of hard point to get parameters for
    @param  jointIndex      [out] joint index of hardpoint
    @param  n3DNodeName     [out] name of child n3dnode 
*/
void 
nCharacterNode::GetHardPoint(int index, int& jointIndex, const char*& n3DNodeName)
{
    n_assert((index >= 0) && (index < this->numHardPoints));
    n_assert(hardPoints);
    jointIndex  = this->hardPoints[index].GetJointIndex();
    n3DNodeName = this->hardPoints[index].Get3DNodeName();
}

//------------------------------------------------------------------------------
/**
    Updates the hard point n3DNode transforms. Gets current transform matrix
    of joint and sets them in the associated n3DNode object of the hardpoint.
*/
void
nCharacterNode::UpdateHardPoints(nSceneGraph2* sceneGraph, const nCharSkeleton& charSkeleton)
{
    n_assert(sceneGraph);

    int i;
    for (i = 0; i < this->numHardPoints; i++)
    {
        int jointIndex = this->hardPoints[i].GetJointIndex();
        n3DNode* n3dn  = this->hardPoints[i].Get3DNodePtr();
        const nCharJoint& charJoint = charSkeleton.GetJoint(jointIndex);

        const matrix44& jointMatrix = charJoint.GetMatrix44();
        n3dn->M(jointMatrix);
        n3dn->Attach(sceneGraph);
    }
}

//------------------------------------------------------------------------------
/**
    Attach all required animblend weights to the channel set.
*/
void
nCharacterNode::AttachChannels(nChannelSet* chnSet)
{
    n_assert(chnSet);

    // first the state channel name
    this->stateChannelIndex = chnSet->AddChannel(this->refChannel.get(), 
                                                 this->stateChannelName.Get());
    
    // for each state...
    int stateIndex;
    int numStates = this->charStateArray.GetNumStates();
    for (stateIndex = 0; stateIndex < numStates; stateIndex++)
    {
        // for each state anim...
        const nCharState& charState = this->charStateArray.GetState(stateIndex);
        int animIndex;
        int numAnims = charState.GetNumAnims();
        for (animIndex = 0; animIndex < numAnims; animIndex++)
        {
            nCharAnim& charAnim = charState.GetAnim(animIndex);
            const char* chnName = charAnim.GetWeightChannelName();
            int chnIndex = chnSet->AddChannel(this->refChannel.get(), chnName);
            charAnim.SetWeightChannelIndex(chnIndex);
        }
    }
    nAnimNode::AttachChannels(chnSet);
}

//------------------------------------------------------------------------------
/**
    Called by client when a new rendercontext instance using this
    object has been created (i.e. when a game object has been created).
    We use this to create a new non-shared character object.
*/
void
nCharacterNode::RenderContextCreated(int renderContext)
{
    // get a unique key from character server if not happened yet
    if (-1 == this->nodeKey)
    {
        this->nodeKey = this->refCharServer->GetUniqueKey();
    }

    // create a node/renderContext Key and create a new character object
    int key = (renderContext << 16) | (this->nodeKey & 0xffff);
    nCharacter* chr = this->refCharServer->NewCharacter(key);
    n_assert(chr);

    // initialize the character
    chr->SetCharSkeleton(this->charSkeleton);
    chr->SetStateArray(&(this->charStateArray));

    nVisNode::RenderContextCreated(renderContext);
}

//------------------------------------------------------------------------------
/**
    Called by client when a rendercontext instance using this object
    has been destroyed (i.e. when a game object has been destroyed). This
    is the time when a non-shared character object should be 
    released.
*/
void
nCharacterNode::RenderContextDestroyed(int renderContext)
{
    n_assert(-1 != this->nodeKey);
    
    // create a node/renderContext Key and create a new character object
    int key = (renderContext << 16) | (this->nodeKey & 0xffff);
    this->refCharServer->ReleaseCharacters(key, 0xffffffff);
    
    nVisNode::RenderContextDestroyed(renderContext);
}

//------------------------------------------------------------------------------
/**
    Attach to scene. This will also evaluate the character skeleton.
*/
bool
nCharacterNode::Attach(nSceneGraph2* sceneGraph)
{
    nChannelContext* chnContext = sceneGraph->GetChannelContext();    
    float time = chnContext->GetChannel1f(this->localChannelIndex);
    const char* chnStateName = chnContext->GetChannelString(this->stateChannelIndex);

    int renderContext = sceneGraph->GetRenderContext();
    int key = (renderContext << 16) | (this->nodeKey & 0xffff);
    nCharacter* chr = this->refCharServer->FindCharacter(key);
    n_assert(chr);
    
    // need to do a anim state switch?
    int curStateIndex = chr->GetActiveState();
    const char* curStateName = this->charStateArray.GetStateNameByIndex(curStateIndex);
    if (chnStateName && (0 != strcmp(chnStateName, curStateName)))
    {
        int chnStateIndex = this->charStateArray.GetStateIndexByName(chnStateName);
        if (chnStateIndex != -1)
        {
            chr->SetActiveState(chnStateIndex, time);
        }
    }

    // route attach to the per state nVisNode childs, this will invoke
    // the channel splitters, weight trees, etc...
    curStateIndex = chr->GetActiveState();
    n_assert(chr);
    n_assert((curStateIndex >= 0) && (curStateIndex < this->charStateArray.GetNumStates()));
    if (this->refStateNodes[curStateIndex].isvalid())
    {
        this->refStateNodes[curStateIndex]->Attach(sceneGraph);
    }

    // update the character's joint skeleton
    chr->EvaluateJoints(time, chnContext);

    // update the hardpoint n3DNode's
    this->UpdateHardPoints(sceneGraph, chr->GetCharSkeleton());

    // attach ourselves as complex visual node
    sceneGraph->AttachVisualNode(this);
    return true;
}

//------------------------------------------------------------------------------
/**
    Update and render object. The character skeleton has already been
    updated inside Attach(), Compute() will just do the rendering.
*/
void
nCharacterNode::Compute(nSceneGraph2* sceneGraph)
{
    // get channel values
    int renderContext = sceneGraph->GetRenderContext();

    // get our character object
    int key = (renderContext << 16) | (this->nodeKey & 0xffff);
    nCharacter* chr = this->refCharServer->FindCharacter(key);
    n_assert(chr);

    // invoke Compute() on skin mesh and get pointer to source vertex buffer
    this->refSkin->Compute(sceneGraph);
    nVertexBuffer* srcSkin = sceneGraph->GetVertexBuffer();
    n_assert(srcSkin);
    sceneGraph->SetVertexBuffer(0);

    // initialize char skin renderer?
    if (!this->charSkinRenderer.IsValid())
    {
        nShadowCaster* shadowCaster = 0;
        if (this->castShadow)
        {
            shadowCaster = sceneGraph->GetShadowCaster();
        }
        this->charSkinRenderer.Initialize(this->refShadowServer.get(), srcSkin, shadowCaster, this->readOnly);
    }

    // render the character's skin
    this->charSkinRenderer.Render(&(chr->GetCharSkeleton()), 
                                    sceneGraph->GetIndexBuffer(), 
                                    srcSkin,
                                    sceneGraph->GetPixelShader(),
                                    sceneGraph->GetTextureArray());

    // attach shadow caster object to shadow scene
    if (this->castShadow)
    {  
        this->refShadowServer->AttachCaster(
            sceneGraph->GetTransform(), 
            this->charSkinRenderer.GetShadowCaster());
    }
    
    if (this->renderSkeleton)
    {
        // render the skeleton (debug visualization)
        if (this->refPrimServer.isvalid()) {
            chr->RenderSkeleton(this->refPrimServer.get(), this->skeletonColor);
        }
    }

    nVisNode::Compute(sceneGraph);
}

//------------------------------------------------------------------------------
