#ifndef N_EULERANGLEKEY_H
#define N_EULERANGLEKEY_H
//-----------------------------------------------------------------------------
/**
    @class nEulerAngleKey

    @brief Animation key for euler angles.
*/
#ifndef N_ANIMKEY_H
#include "anim/nanimkey.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

//-----------------------------------------------------------------------------
class nEulerAngleKey : public nAnimKey
{
public:
    /// default constructor
    nEulerAngleKey();
    /// constructor 1
    nEulerAngleKey(float time, float x, float y, float z);
    /// constructor 2
    nEulerAngleKey(float time, const vector3& v);
    /// set key value
    void Set(const vector3& v);
    /// get key value
    const vector3& Get() const;
    /// interpolate
    void Interpolate(float lerp, 
                     const nEulerAngleKey& from, 
                     const nEulerAngleKey& to);

private:
    vector3 vec;
};

//-----------------------------------------------------------------------------
/**
*/
inline
nEulerAngleKey::nEulerAngleKey()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
inline
nEulerAngleKey::nEulerAngleKey(float time, float x, float y, float z) :
    nAnimKey(time),
    vec(x,y,z)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
inline
nEulerAngleKey::nEulerAngleKey(float time, const vector3& v) :
    vec(v),
    nAnimKey(time)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nEulerAngleKey::Set(const vector3& v)
{
    this->vec = v;
}

//-----------------------------------------------------------------------------
/**
*/
inline
const vector3&
nEulerAngleKey::Get() const
{
    return this->vec;
}

//-----------------------------------------------------------------------------
/**
   !!! THIS MUST BE OPTIMIZED !!!
*/
inline
void
nEulerAngleKey::Interpolate(float lerp, 
                            const nEulerAngleKey& from, 
                            const nEulerAngleKey& to)
{
    // Use quaternions for interpolation.
    matrix33 matrix;
    
    matrix.from_euler(from.vec);
    quaternion qfrom;
    qfrom = matrix.get_quaternion();

    matrix.from_euler(to.vec);
    quaternion qto;
    qto = matrix.get_quaternion();
    
    quaternion qipol;
    qipol.slerp(qfrom, qto, lerp);
    
    matrix33 matrix2(qipol);
    vec = matrix2.to_euler();

    nAnimKey::Interpolate(lerp, from, to);
}

//-----------------------------------------------------------------------------
#endif
