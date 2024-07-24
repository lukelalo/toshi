#ifndef N_VECTOR3KEY_H
#define N_VECTOR3KEY_H
//------------------------------------------------------------------------------
/**
    @class nVector3Key

    @brief Subclass of nAnimKey, holding a vector3.
*/
#ifndef N_ANIMKEY_H
#include "anim/nanimkey.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

//------------------------------------------------------------------------------
class nVector3Key : public nAnimKey
{
public:
    /// default constructor
    nVector3Key();
    /// constructor 1
    nVector3Key(float time, float x, float y, float z);
    /// constructor 2
    nVector3Key(float time, const vector3& v);
    /// set key value
    void Set(const vector3& v);
    /// get key value
    const vector3& Get() const;
    /// interpolate
    void Interpolate(float lerp, const nVector3Key& from, const nVector3Key& to);

protected:
    vector3 vec;
};

//------------------------------------------------------------------------------
/**
*/
inline
nVector3Key::nVector3Key()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nVector3Key::nVector3Key(float time, float x, float y, float z) :
    nAnimKey(time),
    vec(x,y,z)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nVector3Key::nVector3Key(float time, const vector3& v) :
    vec(v),
    nAnimKey(time)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVector3Key::Set(const vector3& v)
{
    this->vec = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nVector3Key::Get() const
{
    return this->vec;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVector3Key::Interpolate(float lerp, const nVector3Key& from, const nVector3Key& to)
{
    this->vec = from.vec + ((to.vec - from.vec) * lerp);
    nAnimKey::Interpolate(lerp, from, to);
}

//------------------------------------------------------------------------------
#endif
