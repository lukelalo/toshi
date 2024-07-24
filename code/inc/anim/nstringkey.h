#ifndef N_STRINGKEY_H
#define N_STRINGKEY_H
//------------------------------------------------------------------------------
/**
    @class nStringKey

    @brief A string animation key actually doesn't do any "interpolation",
    it just switches to the key in the array.
*/
#ifndef N_ANIMKEY_H
#include "anim/nanimkey.h"
#endif

//------------------------------------------------------------------------------
class nStringKey : public nAnimKey
{
public:
    /// default constructor 
    nStringKey();
    /// constructor 1
    nStringKey(float time, const char* s);
    /// copy constructor
    nStringKey(const nStringKey& src);
    /// destructor
    ~nStringKey();
    /// assignment operator
    nStringKey& operator=(const nStringKey& rhs);
    
    /// set key value
    void Set(const char* s);
    /// get key value
    const char* Get() const;
    /// set this key to the interpolated result of the other two keys
    void Interpolate(float lerp, const nStringKey& from, const nStringKey& to);

protected:
    /// copy contents
    void Copy(const nStringKey& src);
    /// delete contents
    void Delete();

    const char* str;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nStringKey::Copy(const nStringKey& src)
{
    if (src.str)
    {
        this->str = n_strdup(src.str);
    }
    else
    {
        this->str = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStringKey::Delete()
{
    if (this->str)
    {
        n_free((void*) this->str);
        this->str = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nStringKey::nStringKey() :
    str(0),
    nAnimKey()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nStringKey::nStringKey(float time, const char* s) :
    nAnimKey(time)
{
    this->str = n_strdup(s);
}    

//------------------------------------------------------------------------------
/**
*/
inline
nStringKey::nStringKey(const nStringKey& src) :
    str(0),
    nAnimKey(src.time)
{
    this->Copy(src);
}

//------------------------------------------------------------------------------
/**
*/
inline
nStringKey::~nStringKey()
{
    this->Delete();
}

//------------------------------------------------------------------------------
/**
*/
inline
nStringKey& 
nStringKey::operator=(const nStringKey& rhs)
{
    this->time = rhs.time;
    this->Delete();
    this->Copy(rhs);
    return (*this);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStringKey::Set(const char* s)
{
    n_assert(s);
    this->Delete();
    this->str = n_strdup(s);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char* 
nStringKey::Get() const
{
    return this->str;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStringKey::Interpolate(float lerp, const nStringKey& from, const nStringKey& to)
{
    this->Delete();
    this->Copy(from);
    nAnimKey::Interpolate(lerp, from, to);
}

//------------------------------------------------------------------------------
#endif
