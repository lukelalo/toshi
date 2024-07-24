#define N_IMPLEMENTS nCharacterServer
//------------------------------------------------------------------------------
//  ncharacter.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "character/ncharacter.h"
#include "character/ncharstatearray.h"
#include "gfx/ngfxserver.h"

//------------------------------------------------------------------------------
/**
*/
nCharacter::nCharacter() :
    charStateArray(0),
    activateStateCounter(0),
    stateTransitionTime(0.25f),
    activeStateStarted(0.0f),
    activeState(0)
{
    memset(this->activeStateChannels, 0, sizeof(this->activeStateChannels));
}

//------------------------------------------------------------------------------
/**
*/
nCharacter::~nCharacter()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Read weight channels needed by current animation into the internal
    anim weight channel array, and normalize the weight channels.
*/
void
nCharacter::ReadWeightChannels(float frameTime, nChannelContext* chnContext)
{
    // get a max allowed channel change for this frameTime, this
    // smoothes out any rapid channel changes
    float maxAllowedChange = frameTime / this->stateTransitionTime;

    // copy the anim blend weights from the channel context to the activeStateChannels array
    const nCharState& curState = this->charStateArray->GetState(this->activeState);
    int numAnims = curState.GetNumAnims();
    n_assert(numAnims < MAXSTATECHANNELS);

    int i;
    float weightSum = 0.0f;
    for (i = 0; i < numAnims; i++)
    {
        const nCharAnim& curAnim = curState.GetAnim(i);
        float newWeight = chnContext->GetChannel1f(curAnim.GetWeightChannelIndex());
        float curWeight = this->activeStateChannels[i];

        newWeight = this->ClampWeight(newWeight);
        curWeight = this->ClampWeight(curWeight);

        curWeight = n_smooth(newWeight, curWeight, maxAllowedChange);

        this->activeStateChannels[i] = curWeight;
        weightSum += curWeight;
    }

    // normalize the weights so that they add up to 1.0
    if (weightSum > 0.0f)
    {
        float oneDivWeightSum = 1.0f / weightSum;
        for (i = 0; i < numAnims; i++)
        {
            this->activeStateChannels[i] *= oneDivWeightSum;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Evaluate the joint skeleton for a given time stamp.
*/
void
nCharacter::EvaluateJoints(float time, nChannelContext* chnContext)
{
    n_assert(chnContext);
    n_assert(this->charStateArray);

    float activeStateTime = time - this->activeStateStarted;
    if (activeStateTime < 0.0f)
    {
        // a timer exception
        n_printf("nCharacter::EvaluateJoints(): Timer exception\n");
        this->activeStateStarted = time;
        activeStateTime = 0.0f;
    }

    // read weight channels
    float frameTime = time - this->lastEvalTime;
    if (frameTime <= 0.0f)
    {
        frameTime = 0.01f;
    }
    else if (frameTime > 1.0f)
    {
        frameTime = 0.1f;
    }
    this->lastEvalTime = time;

    this->ReadWeightChannels(frameTime, chnContext);

    // are we within a state transition?
    float stateLerp;
    if (this->activateStateCounter > 1)
    {
        stateLerp = (time - this->activeStateStarted) / this->stateTransitionTime;
        if (stateLerp < 0.0f)
        {
            stateLerp = 0.0f;
        }
        else if (stateLerp > 1.0f)
        {
            stateLerp = 1.0f;
        }
    }
    else
    {
        // first stateswitch, don't lerp
        stateLerp = 1.0f;
    }

    // evaluate the skeleton
    const nCharState& curState = this->charStateArray->GetState(this->activeState);
    int numAnims = curState.GetNumAnims();
    this->charStateArray->UpdateSkeleton(&(this->charSkeleton),
                                         this->activeState, 
                                         activeStateTime, 
                                         this->activeStateChannels,
                                         numAnims,
                                         stateLerp);
}

//------------------------------------------------------------------------------
/**
    Render the skeleton as a wireframe model. Only for debug visualization.
*/
void
nCharacter::RenderSkeleton(nPrimitiveServer* primServer, vector4& color)
{
    if (primServer) {

        // save old state
        nRStateParam zf = primServer->GetPixelShader()->GetShaderDesc()->GetZFunc();
        // set zfunc to always, so that skeleton overlays skin.
        primServer->GetPixelShader()->GetShaderDesc()->SetZFunc(N_CMP_ALWAYS);

        primServer->Begin(N_PTYPE_LINE_LIST);
        primServer->Rgba(color.x, color.y, color.z, color.w);

        int curJointIndex;
        int numJoints = this->charSkeleton.GetNumJoints();
        for (curJointIndex = 0; curJointIndex < numJoints; curJointIndex++)
        {
            const nCharJoint& curJoint = this->charSkeleton.GetJoint(curJointIndex);
            const nCharJoint* parentJoint = curJoint.GetParentJoint();
            if (parentJoint)
            {
                vector3 curPos = curJoint.GetMatrix44().pos_component();
                vector3 parentPos = parentJoint->GetMatrix44().pos_component();
                primServer->Coord(curPos.x, curPos.y, curPos.z);
                primServer->Coord(parentPos.x, parentPos.y, parentPos.z);
            }
        }
        primServer->End();

        // restore old state.
        primServer->GetPixelShader()->GetShaderDesc()->SetZFunc(zf);
    }
}

//------------------------------------------------------------------------------
