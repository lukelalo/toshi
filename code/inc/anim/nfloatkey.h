#ifndef N_FLOATKEY_H
#define N_FLOATKEY_H
//------------------------------------------------------------------------------
/**
    @class nFloatKey

    @brief Subclass of nAnimKey for holding an animation key with a single
    float value.
*/
#ifndef N_ANIMKEY_H
#include "anim/nanimkey.h"
#endif

//------------------------------------------------------------------------------
class nFloatKey : public nAnimKey
{
public:
    /// default constructor
    nFloatKey();
    /// constructor taking a key value
    nFloatKey(float time, float f);

    /// set key value
    void Set(float f);
    /// get key value
    float Get() const;
    /// set value to interpolated result of 2 source keys
    void Interpolate(float lerp, const nFloatKey& from, const nFloatKey& to);

protected:
    float val;
};

//------------------------------------------------------------------------------
/**
*/
inline
nFloatKey::nFloatKey() :
    val(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nFloatKey::nFloatKey(float time, float f) :
    nAnimKey(time),
    val(f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFloatKey::Set(float f)
{
    this->val = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nFloatKey::Get() const
{
    return this->val;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFloatKey::Interpolate(float lerp, const nFloatKey& from, const nFloatKey& to)
{
    this->val = from.val + ((to.val - from.val) * lerp);
    nAnimKey::Interpolate(lerp, from, to);
}

//------------------------------------------------------------------------------
#endif
