#define N_IMPLEMENTS nCharacterServer
//------------------------------------------------------------------------------
//  ncharstatearray.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "character/ncharstatearray.h"
#include "character/ncharstate.h"

//------------------------------------------------------------------------------
/**
*/
nCharStateArray::nCharStateArray() :
    animCurveArray(0),
    numStates(0),
    stateArray(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nCharStateArray::~nCharStateArray()
{
    // delete owned object (the curveArray object is NOT owned)
    if (this->stateArray)
    {
        delete[] this->stateArray;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCharStateArray::SetAnimCurveArray(const nAnimCurveArray* anim)
{
    n_assert(anim);
    this->animCurveArray = anim;
}

//------------------------------------------------------------------------------
/**
*/
const nAnimCurveArray*
nCharStateArray::GetAnimCurveArray() const
{
    return this->animCurveArray;
}

//------------------------------------------------------------------------------
/**
    Start defining the animation states.

    @param  num     number of animation states
*/
void
nCharStateArray::BeginStates(int num)
{
    n_assert(num > 0);
    n_assert(!this->stateArray);
    n_assert(this->animCurveArray);

    this->stateArray = new nCharState[num];
    this->numStates = num;
}

//------------------------------------------------------------------------------
/**
    Add a new animation state.

    @param  index           state index
    @param  stateName       state name (used to find right animation curve
    @param  numAnims        number of weighted animations in state
*/
void
nCharStateArray::AddState(int stateIndex, const char* stateName)
{
    n_assert((stateIndex >= 0) && (stateIndex < this->numStates));

    nCharState& charState = this->stateArray[stateIndex];
    charState.SetName(stateName);
    charState.SetAnimCurveArray(this->animCurveArray);
}

//------------------------------------------------------------------------------
/**
    Begin adding weighted anims to a state.
*/
void
nCharStateArray::BeginStateAnims(int stateIndex, int numAnims)
{
    n_assert((stateIndex >= 0) && (stateIndex < this->numStates));
    this->stateArray[stateIndex].BeginAnims(numAnims);
}

//------------------------------------------------------------------------------
/**
    Add a weight-blended animation to a state.
*/
void
nCharStateArray::AddStateAnim(int stateIndex, int animIndex, const nCharSkeleton* charSkeleton, const char* animName, const char* weightChannel)
{
    n_assert((stateIndex >= 0) && (stateIndex < this->numStates));
    this->stateArray[stateIndex].AddAnim(animIndex, charSkeleton, animName, weightChannel);
}

//------------------------------------------------------------------------------
/**
    Finish adding animations to a state.
*/
void
nCharStateArray::EndStateAnims(int stateIndex)
{
    n_assert((stateIndex >= 0) && (stateIndex < this->numStates));
    this->stateArray[stateIndex].EndAnims();
}

//------------------------------------------------------------------------------
/**
    Finish defining states.
*/
void
nCharStateArray::EndStates()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Update current state of provided character skeleton. Possibly do
    a state transition blend between current and previous animation state.
*/
void 
nCharStateArray::UpdateSkeleton(nCharSkeleton* charSkeleton,
                                int curStateIndex, 
                                float curStateTime, 
                                float* curStateChannels,
                                int numStateChannels,
                                float transitionLerp) const
{
    n_assert(charSkeleton);
    n_assert(curStateChannels);
    vector3 t0, t1;
    quaternion r0, r1;

    // get the current 
    const nCharState& curCharState  = this->GetState(curStateIndex);

    // update translation and rotation of joints
    int curJointIndex;
    int numJoints = charSkeleton->GetNumJoints();
    for (curJointIndex = 0; curJointIndex < numJoints; curJointIndex++)
    {
        // initialize translate and rotate with current joint translate/rotate
        nCharJoint& curJoint = charSkeleton->GetJoint(curJointIndex);
        t0 = curJoint.GetTranslate();
        r0 = curJoint.GetRotate();
        t1 = t0;
        r1 = r0;

        // get the blended joint transform and rotation
        curCharState.Sample(curStateTime, curJointIndex, curStateChannels, numStateChannels, t1, r1);

        // if necessary blend between new state and current joint position
        if (transitionLerp < 1.0f)
        {
            vector3 t = t0 + ((t1 - t0) * transitionLerp);
            quaternion q;
            q.slerp(r0, r1, transitionLerp);
            t1 = t;
            r1 = q;
        }

        // update joint with new translate/rotate
        curJoint.SetTranslate(t1);
        curJoint.SetRotate(r1);

        // finally evaluate the joint, this will generate the skinning matrices
        curJoint.Evaluate();
    }
}

//------------------------------------------------------------------------------


