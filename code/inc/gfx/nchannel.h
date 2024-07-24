#ifndef N_CHANNEL_H
#define N_CHANNEL_H
//------------------------------------------------------------------------------
/**
    @class nChannel

    @brief A nChannel object is the atomic unit of the channel system. A
    channel holds a name and a 1..4 dimensional float value.

    (C) 2001 A.Weissflog
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

//------------------------------------------------------------------------------
class nChannel
{
public:
    /// the constructor
    nChannel();

    /// clear channel
    void Clear();

    /// set channel content
    void Set1f(float f0);
    /// set channel content
    void Set2f(float f0, float f1);
    /// set channel content
    void Set3f(float f0, float f1, float f2);
    /// set channel content
    void Set4f(float f0, float f1, float f2, float f3);
    /// set channel content
    void SetPtr(void* p);
    /// set channel contents as string (not copied!)
    void SetString(const char* s);

    /// get channel content as single float
    float Get1f();
    /// get channel content as vector2
    void Get2f(float& f0, float& f1);
    /// get channel content as vector3
    void Get3f(float& f0, float& f1, float& f2);
    /// get channel content as vector4
    void Get4f(float& f0, float& f1, float& f2, float& f3);
    /// get channel content as poiner
    void* GetPtr();
    /// get channel contents as string
    const char* GetString();

private:
    union
    {
        float f[4];
        void* ptr;
        const char* str;
    };
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nChannel::Clear()
{
    f[0] = 0.0f;
    f[1] = 0.0f;
    f[2] = 0.0f;
    f[3] = 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
inline
nChannel::nChannel()
{
    this->Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nChannel::Set1f(float f0)
{
    this->f[0] = f0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nChannel::Set2f(float f0, float f1)
{
    this->f[0] = f0;
    this->f[1] = f1;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nChannel::Set3f(float f0, float f1, float f2)
{
    this->f[0] = f0;
    this->f[1] = f1;
    this->f[2] = f2;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nChannel::Set4f(float f0, float f1, float f2, float f3)
{
    this->f[0] = f0;
    this->f[1] = f1;
    this->f[2] = f2;
    this->f[3] = f3;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nChannel::SetPtr(void* p)
{
    this->ptr = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nChannel::SetString(const char* s)
{
    this->str = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nChannel::Get1f()
{
    return this->f[0];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nChannel::Get2f(float& f0, float& f1)
{
    f0 = this->f[0];
    f1 = this->f[1];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nChannel::Get3f(float& f0, float& f1, float& f2)
{
    f0 = this->f[0];
    f1 = this->f[1];
    f2 = this->f[2];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nChannel::Get4f(float& f0, float& f1, float& f2, float& f3)
{
    f0 = this->f[0];
    f1 = this->f[1];
    f2 = this->f[2];
    f3 = this->f[3];
}

//------------------------------------------------------------------------------
/**
*/
inline
void*
nChannel::GetPtr()
{
    return this->ptr;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nChannel::GetString()
{
    return this->str;
}

//------------------------------------------------------------------------------
#endif

