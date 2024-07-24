#ifndef N_ANIMKEYARRAY_H
#define N_ANIMKEYARRAY_H
//------------------------------------------------------------------------------
/**
    @class nAnimKeyArray

    @brief manages an array of animation keys, with interpolated lookups

    nAnimKeyArray is a class template which accepts instances of subclasses 
    of nAnimKey as the keys it manages.

     - 11-Aug-01   floh    + added caching mechanism for faster lookup
*/

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

//------------------------------------------------------------------------------
template<class TYPE> class nAnimKeyArray
{
public:
    /// default constructor
    nAnimKeyArray();
    /// add a key
    void AddKey(const TYPE& key);
    /// check if array is empty
    bool IsEmpty();
    /// get an interpolated key based on timestamp
    const TYPE& Interpolate(float time);
    /// get number of keys
    int GetNumKeys();
    /// get key at index
    const TYPE& GetKeyAt(int index);
    /// get duration
    float GetDuration();
    /// remove key at index
    void RemKeyAt(int index);

protected:
    TYPE curKey;
    int lastKeyIndex;
    nArray<TYPE> keys;
};

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nAnimKeyArray<TYPE>::nAnimKeyArray() :
    lastKeyIndex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
void
nAnimKeyArray<TYPE>::AddKey(const TYPE& key)
{
    // assert that the added key's time stamp
    // is greater then the last key's time stamp
    if (!this->keys.Empty())
    {
        n_assert(this->keys.Back().GetTime() < key.GetTime());
    }

    this->keys.PushBack(key);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
bool
nAnimKeyArray<TYPE>::IsEmpty()
{
    return this->keys.Empty();
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
int
nAnimKeyArray<TYPE>::GetNumKeys()
{
    return this->keys.Size();
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
float
nAnimKeyArray<TYPE>::GetDuration()
{
    if (!this->keys.Empty())
    {
        return this->keys.Back().GetTime();
    }
    else
    {
        return 0.0f;
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
const TYPE&
nAnimKeyArray<TYPE>::GetKeyAt(int index)
{
    n_assert((index >= 0) && (index < (int)this->keys.Size()));
    return this->keys.At(index);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
void
nAnimKeyArray<TYPE>::RemKeyAt(int index)
{
    n_assert((index >= 0) && (index < (int)this->keys.Size()));
    this->keys.Erase(index);
}

//------------------------------------------------------------------------------
/**
    Returns interpolated result based on given timestamp. For now, 
    timestamps outside the valid region will be clamped (if needed,
    nAnimKeyArray should be extended to support looping). If there
    are no keys added, the result will be the "default" key
    (the key created by the default constructor).
*/
template<class TYPE>
inline
const TYPE& 
nAnimKeyArray<TYPE>::Interpolate(float time)
{   
    if (!this->keys.Empty())
    {
        if (time <= this->keys.Front().GetTime())
        {
            // clamp front?
            const TYPE& key = this->keys.Front();
            this->curKey.Interpolate(0.0f, key, key);
            return this->curKey;
        } 
        else if (time >= this->keys.Back().GetTime())
        {
            // clamp back?
            const TYPE& key = this->keys.Back();
            this->curKey.Interpolate(0.0f, key, key);
            return this->curKey;
        } 
        else
        {   
            int i = 0;
            int num = this->keys.Size();
            
            // cache hit?
            if ((this->lastKeyIndex < num) && (this->keys.At(lastKeyIndex).GetTime() <= time))
            {
                i = this->lastKeyIndex;
            }

            // iterate through keys
            for (; i < num; i++)
            {
                const TYPE& key1 = this->keys.At(i);
                float t1 = key1.GetTime();
                if (t1 > time)
                {
                    const TYPE& key0 = (i > 0) ? this->keys.At(i-1) : key1;
                    float t0 = key0.GetTime();

                    // set embedded curKey to interpolated result and return
                    float lerp = 1.0f;
                    if (t1 > t0)
                    {
                        lerp = (time - t0) / (t1 - t0);
                    }
                    this->curKey.Interpolate(lerp, key0, key1);
                    this->lastKeyIndex = i;
                    break;
                }
            }
        }
    }
    return this->curKey;
}
//------------------------------------------------------------------------------
#endif
