#ifndef N_CHARSTATE_H
#define N_CHARSTATE_H
//------------------------------------------------------------------------------
/**
    @class nCharState
    
    @brief A character state can contain multiple animation loops which are
    blended based on weights into a final animation.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_CHARANIM_H
#include "character/ncharanim.h"
#endif

#ifndef N_CHANNELCONTEXT_H
#include "gfx/nchannelcontext.h"
#endif

//------------------------------------------------------------------------------
class nCharState
{
public:
    /// constructor
    nCharState();
    /// destructor
    ~nCharState();
    /// set state name
    void SetName(const char* name);
    /// get state name
    const char* GetName() const;
    /// set anim curve object which provides the animation curves
    void SetAnimCurveArray(const nAnimCurveArray* curveArray);
    /// get anim curve object which provides the animation curves
    const nAnimCurveArray* GetAnimCurveArray() const;
    /// begin adding animations
    void BeginAnims(int num);
    /// add a weighted animation to the state
    void AddAnim(int index, const nCharSkeleton* charSkeleton, const char* animName, const char* weightChannel);
    /// finish adding animations
    void EndAnims();
    /// get number of anims in state
    int GetNumAnims() const;
    /// get anim object by index
    nCharAnim& GetAnim(int index) const;
    /// get weight-blended sample for a joint index
    void Sample(float time, int jointIndex, float* weightArray, int numWeights, vector3& translate, quaternion& quat) const;

private:
    nString stateName;
    const nAnimCurveArray* animCurveArray;
    int numCharAnims;
    nCharAnim* charAnims;
    bool inBeginAnims;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCharState::nCharState() :
    animCurveArray(0),
    numCharAnims(0),
    charAnims(0),
    inBeginAnims(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharState::~nCharState()
{
    // delete owned objects
    if (this->charAnims)
    {
        delete[] this->charAnims;
        this->charAnims = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharState::SetName(const char* name)
{
    n_assert(name);
    this->stateName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nCharState::GetName() const
{
    return this->stateName.IsEmpty() ? 0 : this->stateName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharState::SetAnimCurveArray(const nAnimCurveArray* curveArray)
{
    n_assert(curveArray);
    n_assert(0 == this->animCurveArray);
    this->animCurveArray = curveArray;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nAnimCurveArray*
nCharState::GetAnimCurveArray() const
{
    return this->animCurveArray;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharState::BeginAnims(int num)
{
    n_assert(num > 0);
    n_assert(!this->inBeginAnims);

    this->inBeginAnims = true;
    this->numCharAnims = num;
    this->charAnims = new nCharAnim[num];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharState::AddAnim(int index, const nCharSkeleton* charSkeleton, const char* animName, const char* weightChannel)
{
    n_assert((index >= 0) && (index < this->numCharAnims));
    n_assert(charSkeleton);
    n_assert(animName);
    n_assert(this->inBeginAnims);
    n_assert(weightChannel);
    n_assert(!this->stateName.IsEmpty());
    n_assert(this->animCurveArray);

    this->charAnims[index].SetParams(this->stateName.Get(), animName, this->animCurveArray, charSkeleton);
    this->charAnims[index].SetWeightChannelName(weightChannel);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharState::EndAnims()
{
    n_assert(this->inBeginAnims);
    this->inBeginAnims = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharState::GetNumAnims() const
{
    return this->numCharAnims;
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharAnim&
nCharState::GetAnim(int index) const
{
    n_assert((index >= 0) && (index < this->numCharAnims));
    return this->charAnims[index];
}

//------------------------------------------------------------------------------
/**
    NOTE: If translation or rotation of the joint is not animated the 
    content of the joint will not be touched!

    @param  time            sample time
    @param  jointIndex      the joint index to sample data for
    @param  weightArray     float array with animation weight blends
    @param  numWeights      number of valid values in weightArray
    @param  translate       [out] the resulting joint translation
    @param  quat            [out] the resulting joint rotation
*/
inline
void
nCharState::Sample(float time, int jointIndex, float* weightArray, int numWeights, vector3& translate, quaternion& quat) const
{
    n_assert(weightArray);
    n_assert(numWeights == this->numCharAnims);

    int i;
    quaternion quatCur;
    quaternion quatSlerp;
    float weightAccum = 0.0f;
    bool firstIter = true;
    bool isTranslated = false;
    bool isRotated = false;
    
    vector4 curTranslate;
    vector4 curRotate;
    vector3 tmpTranslate;
    quaternion tmpRotate;

    for (i = 0; i < this->numCharAnims; i++)
    {
        // get values for current animation
        nCharAnim& curAnim = this->charAnims[i];
        float curWeight = weightArray[i];
        curAnim.Sample(time, jointIndex, curTranslate, curRotate);

        if (curWeight > 0.0f)
        {
            // blend translation (if joint has translation)
            if (curAnim.JointHasTranslation(jointIndex))
            {
                isTranslated = true;
                tmpTranslate.x += curTranslate.x * curWeight;
                tmpTranslate.y += curTranslate.y * curWeight;
                tmpTranslate.z += curTranslate.z * curWeight;
            }

            // blend rotation (if joint has rotation)
            if (curAnim.JointHasRotation(jointIndex))
            {
                isRotated = true;
                
                // blend rotation
                if (firstIter)
                {
                    // first time init of quat and weightAccum
                    firstIter = false;
                    tmpRotate.set(curRotate.x, curRotate.y, curRotate.z, curRotate.w);
                    weightAccum = curWeight;
                }
                else
                {
                    // perform interpolation
                    quatCur.set(curRotate.x, curRotate.y, curRotate.z, curRotate.w);

                    // scale weightAccum so that 1 == (weightAccum + weight)
                    float scaledWeight = curWeight / (weightAccum + curWeight);
                    quatSlerp.slerp(tmpRotate, quatCur, scaledWeight);
                    tmpRotate = quatSlerp;
                    weightAccum += curWeight;
                }
            }
        }         
    }
    if (isTranslated)
    {
        translate = tmpTranslate;
    }
    if (isRotated)
    {
        quat = tmpRotate;
    }
}

//------------------------------------------------------------------------------
#endif
