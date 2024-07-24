#ifndef N_CHARANIM_H
#define N_CHARANIM_H
//------------------------------------------------------------------------------
/**
    @class nCharAnim
    
    @brief A nCharAnim object assoziates animation curves with joints in a 
    character skeleton (you can give it a joint index and a time stamp,
    and get the translation and rotation of the joint back).

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ANIMCURVEARRAY_H
#include "anim/nanimcurvearray.h"
#endif

#ifndef N_CHARSKELETON_H
#include "character/ncharskeleton.h"
#endif

//------------------------------------------------------------------------------
class nCharSkeleton;
class nCharAnim
{
public:
    /// constructor
    nCharAnim();
    /// destructor
    ~nCharAnim();
    /// set required parameters and build internal tables
    void SetParams(const char* stateName, const char* aName, const nAnimCurveArray* curveArray, const nCharSkeleton* skel);
    /// get the anim curve array
    const nAnimCurveArray* GetAnimCurveArray() const;
    /// get name of animation
    const char* GetName() const;
    /// set name of weight channel
    void SetWeightChannelName(const char* chnName);
    /// get weight channel name
    const char* GetWeightChannelName() const;
    /// set weight channel index
    void SetWeightChannelIndex(int index);
    /// get weight channel index
    int GetWeightChannelIndex() const;
    /// return true if the joint has a translate animation attached
    bool JointHasTranslation(int jointIndex) const;
    /// return true if the joint has a rotate animation attached
    bool JointHasRotation(int jointIndex) const;
    /// get translation and rotation for a joint index at timestamp
    void Sample(float time, int jointIndex, vector4& translate, vector4& rotate);

private:
    const nAnimCurveArray* animCurveArray;  // not owned
    nString animName;
    int numCurves;
    nAnimCurve** translateCurves;       // pointers to translate curves for each joint
    nAnimCurve** rotateCurves;          // pointers to rotate curves for each joint
    nString weightChannelName;
    int weightChannelIndex;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCharAnim::nCharAnim() :
    animCurveArray(0),
    numCurves(0),
    translateCurves(0),
    rotateCurves(0),
    weightChannelIndex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharAnim::~nCharAnim()
{
    // delete owned objects (animCurveArray and NOT owned)
    if (this->translateCurves)
    {
        delete[] this->translateCurves;
        this->translateCurves = 0;
    }
    if (this->rotateCurves)
    {
        delete[] this->rotateCurves;
        this->rotateCurves = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharAnim::SetWeightChannelName(const char* chnName)
{
    n_assert(chnName);
    this->weightChannelName = chnName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nCharAnim::GetWeightChannelName() const
{
    return this->weightChannelName.IsEmpty() ? 0 : this->weightChannelName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharAnim::SetWeightChannelIndex(int index)
{
    this->weightChannelIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharAnim::GetWeightChannelIndex() const
{
    return this->weightChannelIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nAnimCurveArray*
nCharAnim::GetAnimCurveArray() const
{
    return this->animCurveArray;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nCharAnim::GetName() const
{
    return this->animName.IsEmpty() ? 0 : this->animName.Get();
}

//------------------------------------------------------------------------------
/**
    Initialize internal curves-2-joint mapping table. Please note that
    the names in the AnimCurveArray must follow a naming convention:

    [stateName]_[animName]_[trans|rot]_[jointname]

    The anim curve doesn't have to contain matching curves for all joints.
    If a curve has not been found, its associated curve pointer in the
    curve arrays will be 0, which means "this nanim resource does not
    translate/rotate this joint".
*/
inline
void
nCharAnim::SetParams(const char* stateName, const char* aName, const nAnimCurveArray* curveArray, const nCharSkeleton* skel)
{
    n_assert(stateName && aName && curveArray && skel);
    n_assert(!this->animCurveArray);
    n_assert(!this->translateCurves);
    n_assert(!this->rotateCurves);

    this->animName       = aName;
    this->animCurveArray = curveArray;

    // fill the joint curve association map
    this->numCurves = skel->GetNumJoints();
    this->translateCurves = new nAnimCurve*[this->numCurves];
    this->rotateCurves    = new nAnimCurve*[this->numCurves];

    // initialize curve pointers with 0
    int i;
    for (i = 0; i < this->numCurves; i++)
    {
        this->translateCurves[i] = 0;
        this->rotateCurves[i] = 0;
    }
    
    char transBuf[N_MAXPATH];
    char rotBuf[N_MAXPATH];
    for (i = 0; i < this->numCurves; i++)
    {
        // build current curve name
        const char* jointName = skel->GetJoint(i).GetName();
        sprintf(transBuf, "%s_%s_trans_%s", stateName, this->animName.Get(), jointName);
        sprintf(rotBuf, "%s_%s_rot_%s", stateName, this->animName.Get(), jointName);

        // find curve by name
        this->translateCurves[i] = this->animCurveArray->FindCurveByName(transBuf);
        this->rotateCurves[i]    = this->animCurveArray->FindCurveByName(rotBuf);

/*
        // find curve in curve array
        int j;
        for (j = 0; j < this->animCurveArray->GetNumCurves(); j++)
        {
            const char* curCurveName = this->animCurveArray->GetCurve(j).GetName();
            if (0 == strcmp(curCurveName, transBuf))
            {
                this->translateCurves[i] = &(this->animCurveArray->GetCurve(j));
            }
            else if (0 == strcmp(curCurveName, rotBuf))
            {
                this->rotateCurves[i] = &(this->animCurveArray->GetCurve(j));
            }
        }
*/
    }
}

//------------------------------------------------------------------------------
/**
    Return true if the joint defined by index has a translation animcurve
    attached.
*/
inline
bool
nCharAnim::JointHasTranslation(int jointIndex) const
{
    n_assert((jointIndex >= 0) && (jointIndex < this->numCurves));
    return (this->translateCurves[jointIndex] != 0);
}

//------------------------------------------------------------------------------
/**
    Return true if the joint defined by index has a rotation animcurve
    attached.
*/
inline
bool
nCharAnim::JointHasRotation(int jointIndex) const
{
    n_assert((jointIndex >= 0) && (jointIndex < this->numCurves));
    return (this->rotateCurves[jointIndex] != 0);
}

//------------------------------------------------------------------------------
/**
    NOTE: If the joint has no translation or rotation animation attached,
    the output args will NOT be modified.
*/
inline
void
nCharAnim::Sample(float time, int jointIndex, vector4& translate, vector4& rotate)
{
    n_assert((jointIndex >= 0) && (jointIndex < this->numCurves));
    n_assert(this->translateCurves && this->rotateCurves);

    // FIXME: sampling is quite a complex operation -> see nAnimCurve::Sample()
    if (this->translateCurves[jointIndex])
    {
        this->translateCurves[jointIndex]->Sample(time, translate);
    }
    if (this->rotateCurves[jointIndex])
    {
        this->rotateCurves[jointIndex]->Sample(time, rotate);
    }
}

//------------------------------------------------------------------------------
#endif
