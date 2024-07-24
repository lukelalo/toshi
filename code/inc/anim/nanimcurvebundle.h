#ifndef N_ANIMCURVEBUNDLE_H
#define N_ANIMCURVEBUNDLE_H
//------------------------------------------------------------------------------
/**
    @class nAnimCurveBundle

    @brief An anim curve bundle mixes weighted samples from several anim curves
    into one resulting sample.

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_ANIMCURVE_H
#include "anim/nanimcurve.h"
#endif

//------------------------------------------------------------------------------
class nAnimCurveBundle
{
public:
    /// constructor
    nAnimCurveBundle();
    /// destructor
    virtual ~nAnimCurveBundle();
    /// set bundle name
    void SetName(const char* n);
    /// get bundle name
    const char* GetName();
    /// begin defining the bundle
    void Begin(int numCurves);
    /// add a curve to the bundle
    void Set(int curveIndex, nAnimCurve* animCurve, const char* channelName);
    /// finish defining the bundle
    void End();
    /// get number of curves in bundle
    int GetNumCurves();
    /// get curve pointer
    nAnimCurve* GetCurve(int curveIndex);
    /// get channel name for a curve
    const char* GetChannelName(int curveIndex);
    /// set channel index for a curve
    void SetChannelIndex(int curveIndex, int channelIndex);
    /// get channel index for a curve
    int GetChannelIndex(int curveIndex);
    /// set a curve's current weight
    void SetWeight(int curveIndex, float weight);
    /// get a curve's current weight
    float GetWeight(int curveIndex);
    /// get a mixed sample
    void Sample(float time, vector4& val);

private:
    class nElement
    {
        friend class nAnimCurveBundle;

        /// constructor
        nElement();
        /// destructor
        ~nElement();
        /// set nAnimCurve object
        void SetAnimCurve(nAnimCurve* ptr);
        /// get nAnimCurve object
        nAnimCurve* GetAnimCurve();
        /// set channel name
        void SetChannelName(const char* n);
        /// get channel name
        const char* GetChannelName();
        /// set channel index
        void SetChannelIndex(int i);
        /// get channel index
        int GetChannelIndex();
        /// set current weight
        void SetWeight(float w);
        /// get current weight
        float GetWeight();

        nAnimCurve* curve;
        nString channelName;
        int channelIndex;
        float weight;
    };

    nString name;
    int numElements;
    nElement* elements;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurveBundle::nElement::nElement() :
    curve(0),
    channelIndex(-1),
    weight(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurveBundle::nElement::~nElement()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurveBundle::nElement::SetAnimCurve(nAnimCurve* ptr)
{
    n_assert(ptr);
    this->curve = ptr;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurve*
nAnimCurveBundle::nElement::GetAnimCurve()
{
    return this->curve;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurveBundle::nElement::SetChannelName(const char* n)
{
    this->channelName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAnimCurveBundle::nElement::GetChannelName()
{
    return this->channelName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurveBundle::nElement::SetChannelIndex(int i)
{
    this->channelIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimCurveBundle::nElement::GetChannelIndex()
{
    return this->channelIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurveBundle::nElement::SetWeight(float w)
{
    this->weight = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAnimCurveBundle::nElement::GetWeight()
{
    return this->weight;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurveBundle::nAnimCurveBundle() :
    numElements(0),
    elements(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurveBundle::~nAnimCurveBundle()
{
    if (this->elements)
    {
        delete[] this->elements;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurveBundle::SetName(const char* n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAnimCurveBundle::GetName()
{
    return this->name.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurveBundle::Begin(int numCurves)
{
    n_assert(!this->elements);
    n_assert(numCurves > 0);
    this->numElements = numCurves;
    this->elements = new nElement[this->numElements];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurveBundle::Set(int curveIndex, nAnimCurve* animCurve, const char* channelName)
{
    n_assert((curveIndex >= 0) && (curveIndex < this->numElements));
    n_assert(animCurve);
    n_assert(channelName);
    nElement& elm = this->elements[curveIndex];
    elm.SetAnimCurve(animCurve);
    elm.SetChannelName(channelName);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurveBundle::End()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimCurveBundle::GetNumCurves()
{
    return this->numElements;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurve*
nAnimCurveBundle::GetCurve(int curveIndex)
{
    n_assert((curveIndex >= 0) && (curveIndex < this->numElements));
    return this->elements[curveIndex].GetAnimCurve();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAnimCurveBundle::GetChannelName(int curveIndex)
{
    n_assert((curveIndex >= 0) && (curveIndex < this->numElements));
    return this->elements[curveIndex].GetChannelName();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurveBundle::SetChannelIndex(int curveIndex, int channelIndex)
{
    n_assert((curveIndex >= 0) && (curveIndex < this->numElements));
    this->elements[curveIndex].SetChannelIndex(channelIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimCurveBundle::GetChannelIndex(int curveIndex)
{
    n_assert((curveIndex >= 0) && (curveIndex < this->numElements));
    return this->elements[curveIndex].GetChannelIndex();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurveBundle::SetWeight(int curveIndex, float weight)
{
    n_assert((curveIndex >= 0) && (curveIndex < this->numElements));
    this->elements[curveIndex].SetWeight(weight);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAnimCurveBundle::GetWeight(int curveIndex)
{
    n_assert((curveIndex >= 0) && (curveIndex < this->numElements));
    return this->elements[curveIndex].GetWeight();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurveBundle::Sample(float time, vector4& val)
{
    n_assert(this->numElements > 0);
    if (this->elements[0].GetAnimCurve()->GetIpolType() == nAnimCurve::QUATERNION)
    {
        // special case: quaternion blending, this blends matrices by
        // doing an accumulative interpolation between 2 quaternions
        quaternion quatAccum;
        quaternion quatCur;
        quaternion quatSlerp;
        float weightAccum = 0.0f;

        int i;
        bool firstIter = true;
        for (i = 0; i < this->numElements; i++)
        {
            nElement& curElm = this->elements[i];
            float weight = curElm.GetWeight();
            if (weight > 0.0f)
            {
                vector4 sample;
                curElm.GetAnimCurve()->Sample(time, sample);

                if (firstIter)
                {
                    // first time init of quatAccum and weightAccum
                    firstIter = false;
                    quatAccum.set(sample.x, sample.y, sample.z, sample.w);
                    weightAccum = weight;
                }
                else
                {
                    // perform interpolation
                    quatCur.set(sample.x, sample.y, sample.z, sample.w);

                    // scale weightAccum so that 1 == (weightAccum + weight)
                    float scaledWeight = weight / (weightAccum + weight);
                    quatSlerp.slerp(quatAccum, quatCur, scaledWeight);
                    quatAccum = quatSlerp;
                    weightAccum += weight;
                }
            }
        }
        val.set(quatAccum.x, quatAccum.y, quatAccum.z, quatAccum.w);
    } 
    else 
    {
        // normal vector4 blending
        vector4 curVal;
        val.set(0.0f, 0.0f, 0.0f, 0.0f);
        int i;
        for (i = 0; i < this->numElements; i++)
        {
            nElement& curElm = this->elements[i];
            float weight = curElm.GetWeight();
            if (weight > 0.0f)
            {
                curElm.GetAnimCurve()->Sample(time, curVal);
                val += curVal * weight;
            }
        }
    }
}

//------------------------------------------------------------------------------
#endif
