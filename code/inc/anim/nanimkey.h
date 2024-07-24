#ifndef N_ANIMKEY_H
#define N_ANIMKEY_H
//------------------------------------------------------------------------------
/**
    @class nAnimKey

    @brief The common parent class of animation keys of all types. Used in
    conjunction with class nAnimKeyArray.
*/

class nAnimKey 
{
public:
    /// the default constructor
    nAnimKey();
    /// constructor taking time arg
    nAnimKey(float t);
    /// set key time
    void SetTime(float t);
    /// get key time
    float GetTime() const;
    /// set this key to the interpolated result of the other two keys
    void Interpolate(float lerp, const nAnimKey& from, const nAnimKey& to);

protected:
    float time;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAnimKey::nAnimKey() :
    time(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimKey::nAnimKey(float t) :
    time(t)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimKey::SetTime(float t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAnimKey::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
    Sets this object to the interpolated result of the arguments. It makes not
    much sense to interpolate the time stamp, thus, the time stamp will be
    set to the "time distance" from the "from" key.

    @param	lerp	the interpolant, must be between 0 and 1
    @param	from	the "from" key
    @param	key1	the "to" key
*/
inline
void
nAnimKey::Interpolate(float lerp, const nAnimKey& from, const nAnimKey& to)
{
    this->time = (to.time - from.time) * lerp;
}

//------------------------------------------------------------------------------
#endif
