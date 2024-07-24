#ifndef N_LISTENER2_H
#define N_LISTENER2_H
//------------------------------------------------------------------------------
/*
    @class nListener2
    @ingroup NebulaAudioModule
    @brief Hold audio listener attributes.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

//------------------------------------------------------------------------------
class nListener2
{
public:
    /// constructor
    nListener2();
    /// set the listener's pos/orientation in global space
    void SetMatrix(const matrix44& m);
    /// get the listener's matrix
    const matrix44& GetMatrix() const;
    /// set velocity in global space
    void SetVelocity(const vector3& vel);
    /// get velocity in global space
    const vector3& GetVelocity() const;
    /// set the doppler factor
    void SetDopplerFactor(float f);
    /// get doppler factor
    float GetDopplerFactor() const;
    /// set rolloff factor
    void SetRolloffFactor(float f);
    /// get rolloff factor
    float GetRolloffFactor() const;

private:
    matrix44 matrix;
    vector3 velocity;
    float dopplerFactor;
    float rolloffFactor;
};

//------------------------------------------------------------------------------
/**
*/
inline
nListener2::nListener2() :
    dopplerFactor(1.0f),
    rolloffFactor(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nListener2::SetMatrix(const matrix44& m)
{
    this->matrix = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nListener2::GetMatrix() const
{
    return this->matrix;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nListener2::SetVelocity(const vector3& vel)
{
    this->velocity = vel;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nListener2::GetVelocity() const
{
    return this->velocity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nListener2::SetDopplerFactor(float f)
{
    this->dopplerFactor = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nListener2::GetDopplerFactor() const
{
    return this->dopplerFactor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nListener2::SetRolloffFactor(float f)
{
    this->rolloffFactor = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nListener2::GetRolloffFactor() const
{
    return this->rolloffFactor;
}

//------------------------------------------------------------------------------
#endif
