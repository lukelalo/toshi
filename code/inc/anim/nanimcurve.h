#ifndef N_ANIMCURVE_H
#define N_ANIMCURVE_H
//------------------------------------------------------------------------------
/**
    @class nAnimCurve

    @brief Implement a 4 dimensional animation curve with fixed sample rate.

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_QUATERNION_H
#include "mathlib/quaternion.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

//------------------------------------------------------------------------------
class nAnimCurve
{
public:
    /// interpolation types
    enum nIpolType
    {
        STEP = 0,
        LINEAR,
        QUATERNION,
    };
    /// repeat type
    enum nRepeatType
    {
        LOOP = 0,
        ONESHOT,
    };
    /// internal key type
    enum nKeyType
    {
        VANILLA = 0,    // vector4
        PACKED,         // packed into 4 bytes (used for quaternions)
    };

    // a packed key class
    class nPackedKey
    {
    public:
        /// constructor
        nPackedKey();
        /// set value
        void Pack(const vector4& v);
        /// get value
        void Unpack(vector4& v);
        /// equality operator
        bool operator==(const nPackedKey& rhs);

    private:
        ushort x, y, z, w;
    };

    /// constructor
    nAnimCurve();
    /// destructor
    ~nAnimCurve();
    /// set curve name
    void SetName(const char* name);
    /// get curve name
    const char* GetName() const;
    /// set interpolation type
    void SetIpolType(nIpolType ipolType);
    /// get interpolation type
    nIpolType GetIpolType() const; 
    /// set repeat type
    void SetRepeatType(nRepeatType repType);
    /// get repeat type
    nRepeatType GetRepeatType() const;
    /// get internal key type
    nKeyType GetKeyType() const;
    /// begin setting keys
    void BeginKeys(float keysPerSecond, int startKey, int numKeys, nKeyType type);
    /// set a key (packed or unpacked)
    void SetKey(int keyIndex, const vector4& val);
    /// set a packed key
    void SetPackedKey(int keyIndex, const nPackedKey& val);
    /// finish setting keys
    void EndKeys();
    /// get number of keys
    int GetNumKeys() const;
    /// get keys per second
    float GetKeysPerSecond() const;

    /// get start key
    int GetStartKey() const;
    /// get key
    const vector4& GetKey(int keyIndex);
    /// get a packed key
    const nPackedKey& GetPackedKey(int keyIndex) const;

    /// convert time into key index
    bool Time2Index(float time, int& keyIndex, float& lerp);
    /// clear the key array
    void ClearKeys();
    /// reduce the animation curve into a single key, if possible
    bool Optimize();
    /// pack vector4 keys into nPackedKeys
    void Pack();
    
    /// get sampled key
    void Sample(float time, vector4& val);
    /// convert ipol type to string
    static const char* IpolType2String(nIpolType ipolType);
    /// convert string to ipol type
    static nIpolType String2IpolType(const char* str);
    /// convert repeat type to string
    static const char* RepeatType2String(nRepeatType repType);
    /// convert string to repeat type
    static nRepeatType String2RepeatType(const char* str);
    /// get raw data len in bytes
    int GetDataSize() const;
    /// get raw pointer to start of data
    void* GetDataPtr() const;

private:
    nString name;
    nIpolType   ipolType;
    nRepeatType repeatType;
    nKeyType keyType;
    float keysPerSecond;
    int startKey;
    int numKeys;
    vector4* keyArray;
    nPackedKey* packedKeyArray;
    float keyTime;
    float startTime;
    float endTime;
    vector4 tmpUnpackedKey;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurve::nPackedKey::nPackedKey()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->w = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurve::nPackedKey::Pack(const vector4& v)
{
    float fx = ((1.0f + n_clamp(v.x, -1.0f, 1.0f)) * 32767.5f) + 0.5f;
    float fy = ((1.0f + n_clamp(v.y, -1.0f, 1.0f)) * 32767.5f) + 0.5f;
    float fz = ((1.0f + n_clamp(v.z, -1.0f, 1.0f)) * 32767.5f) + 0.5f;
    float fw = ((1.0f + n_clamp(v.w, -1.0f, 1.0f)) * 32767.5f) + 0.5f;

    this->x = (ushort) n_ftol(fx);
    this->y = (ushort) n_ftol(fy);
    this->z = (ushort) n_ftol(fz);
    this->w = (ushort) n_ftol(fw);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurve::nPackedKey::Unpack(vector4& v)
{
    const float mult = 1.0f / 32767.5f;

    v.x = (float(this->x) * mult) - 1.0f;
    v.y = (float(this->y) * mult) - 1.0f;
    v.z = (float(this->z) * mult) - 1.0f;
    v.w = (float(this->w) * mult) - 1.0f;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAnimCurve::nPackedKey::operator==(const nPackedKey& rhs)
{
    return ((this->x == rhs.x) && (this->y == rhs.y) && (this->z == rhs.z) && (this->w == rhs.w));
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurve::nAnimCurve() :
    ipolType(LINEAR),
    repeatType(LOOP),
    keyType(VANILLA),
    keysPerSecond(12.0f),
    startKey(0),
    numKeys(0),
    keyArray(0),
    packedKeyArray(0),
    keyTime(1.0f / keysPerSecond),
    startTime(0.0f),
    endTime(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurve::~nAnimCurve()
{
    if (this->keyArray)
    {
        delete[] this->keyArray;
    }
    if (this->packedKeyArray)
    {
        delete[] this->packedKeyArray;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurve::SetName(const char* n)
{   
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAnimCurve::GetName() const
{
    return this->name.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurve::SetIpolType(nAnimCurve::nIpolType ipt)
{
    this->ipolType = ipt;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurve::nIpolType
nAnimCurve::GetIpolType() const
{
    return this->ipolType;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurve::SetRepeatType(nAnimCurve::nRepeatType rt)
{
    this->repeatType = rt;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurve::nRepeatType
nAnimCurve::GetRepeatType() const
{
    return this->repeatType;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurve::nKeyType
nAnimCurve::GetKeyType() const
{
    return this->keyType;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurve::BeginKeys(float kps, int start, int num, nKeyType type)
{
    n_assert(!this->keyArray);
    n_assert(!this->packedKeyArray);
    n_assert(kps > 0.0f);
    n_assert(start >= 0);
    n_assert(num > 0);

    this->keyType       = type;
    this->keysPerSecond = kps;
    this->startKey  = start;
    this->numKeys   = num;
    this->keyTime   = 1.0f / kps;
    this->startTime = start * this->keyTime;
    this->endTime       = this->startTime + this->numKeys * this->keyTime;

    if (VANILLA == type)
    {
    this->keyArray  = new vector4[this->numKeys];
        this->packedKeyArray = 0;
    }
    else
    {
        this->keyArray = 0;
        this->packedKeyArray = new nPackedKey[this->numKeys];
    }
}

//------------------------------------------------------------------------------
/**
    Set a key. Internally the key may be packed.
*/
inline
void
nAnimCurve::SetKey(int keyIndex, const vector4& val)
{
    n_assert((keyIndex >= 0) && (keyIndex < this->numKeys));

    if (VANILLA == this->keyType)
    {
    n_assert(this->keyArray);
        this->keyArray[keyIndex] = val;
    }
    else
    {
        n_assert(this->packedKeyArray);
        this->packedKeyArray[keyIndex].Pack(val);
    }
}

//------------------------------------------------------------------------------
/**
    Directly set a packed key.
*/
inline
void
nAnimCurve::SetPackedKey(int keyIndex, const nPackedKey& val)
{
    n_assert((keyIndex >= 0) && (keyIndex < this->numKeys));
    n_assert(this->packedKeyArray);

    this->packedKeyArray[keyIndex] = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurve::EndKeys()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimCurve::GetNumKeys() const
{
    return this->numKeys;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAnimCurve::GetKeysPerSecond() const
{
    return this->keysPerSecond;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimCurve::GetStartKey() const
{
    return this->startKey;
}

//------------------------------------------------------------------------------
/**
    Return key at given index. Unpacks internally if necessary.
*/
inline
const vector4&
nAnimCurve::GetKey(int keyIndex)
{
    n_assert((keyIndex >= 0) && (keyIndex < this->numKeys));
    if (VANILLA == this->keyType)
    {
    n_assert(this->keyArray);
        return this->keyArray[keyIndex];
    }
    else
    {
        n_assert(this->packedKeyArray);
        this->packedKeyArray[keyIndex].Unpack(this->tmpUnpackedKey);
        return this->tmpUnpackedKey;
    }
}

//------------------------------------------------------------------------------
/**
    Explicitely return a packed key.
*/
inline
const nAnimCurve::nPackedKey&
nAnimCurve::GetPackedKey(int keyIndex) const
{
    n_assert((keyIndex >= 0) && (keyIndex < this->numKeys));
    n_assert(this->packedKeyArray);
    return this->packedKeyArray[keyIndex];
}

//------------------------------------------------------------------------------
/**
    Convert a time value to an index into the key array and interpolation 
    value between this key and the next key. Takes the repeat type (oneshot
    or loop) into account.

    @param  time        [in] the time value
    @param  keyIndex    [out] the computed key index
    @param  lerp        [out] the interpolation value between the keys
    @return             true if time outside range and one shot reptype
*/
inline
bool
nAnimCurve::Time2Index(float time, int& keyIndex, float& lerp)
{
    // check for clamping in oneshot mode
    if (ONESHOT == this->repeatType)
    {
        if (time < this->startTime)
        {
            // front clamped
            keyIndex = 0;
            lerp     = 0;
            return true;
        }
        else if (time >= this->endTime)
        {
            // back clamped
            keyIndex = this->numKeys - 1;
            lerp     = 0;
            return true;
        }
    }

    // not clamped or loop mode...
    float frame  = time / this->keyTime;
    keyIndex     = int(frame);
    lerp         = frame - float(keyIndex);
    keyIndex    %= this->numKeys;
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurve::Sample(float time, vector4& val)
{
    n_assert(this->numKeys > 0);
    n_assert(time >= 0.0f);

    // convert time into key index
    bool clamped;
    int keyIndex;
    float lerp;
    clamped = this->Time2Index(time, keyIndex, lerp);
    if (clamped)
    {
        // clamped, return key indicated by index
        val = this->GetKey(keyIndex);
    }
    else
    {
        // not clamped, interpolate
        if (STEP == this->ipolType)
        {
            val = this->GetKey(keyIndex);
        }
        else if (LINEAR == this->ipolType)
        {
            int nextKeyIndex = (keyIndex + 1) % this->numKeys;
            vector4 v0 = this->GetKey(keyIndex);
            vector4 v1 = this->GetKey(nextKeyIndex);
            val = v0 + (v1 - v0) * lerp;
        }
        else
        {
            // quaternion interpolation
            int nextKeyIndex = (keyIndex + 1) % this->numKeys;
            vector4 v0 = this->GetKey(keyIndex);
            vector4 v1 = this->GetKey(nextKeyIndex);
            quaternion q0(v0.x, v0.y, v0.z, v0.w);
            quaternion q1(v1.x, v1.y, v1.z, v1.w);
            quaternion q;
            q.slerp(q0, q1, lerp);
            val.set(q.x, q.y, q.z, q.w);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAnimCurve::RepeatType2String(nAnimCurve::nRepeatType rt)
{
    switch (rt)
    {
        case LOOP:      return "loop";
        case ONESHOT:   return "oneshot";
    }
    n_assert(false);
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurve::nRepeatType
nAnimCurve::String2RepeatType(const char* str)
{
    n_assert(str);
    if (strcmp(str, "loop") == 0)
    {
        return LOOP;
    }
    else if (strcmp(str, "oneshot") == 0)
    {
        return ONESHOT;
    }
    n_assert(false);
    return LOOP;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAnimCurve::IpolType2String(nAnimCurve::nIpolType ipt)
{
    switch (ipt)
    {
        case STEP:          return "step";
        case LINEAR:        return "linear";
        case QUATERNION:    return "quaternion";
    }
    n_assert(false);
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurve::nIpolType
nAnimCurve::String2IpolType(const char* str)
{
    n_assert(str);
    if (strcmp(str, "step") == 0)
    {
        return STEP;
    }
    else if (strcmp(str, "linear") == 0)
    {
        return LINEAR;
    }
    else if (strcmp(str, "quaternion") == 0)
    {
        return QUATERNION;
    }
    n_printf("nAnimCurve: unsupported ipol type: '%s'\n", str);
    // n_error("Aborting.\n");
    n_assert(false);
    return STEP;
}

//------------------------------------------------------------------------------
/**
    Kill the key array and reset key parameters.
*/
inline
void
nAnimCurve::ClearKeys()
{
    this->keyType = VANILLA;
    this->keysPerSecond = 12.0f;
    this->startKey = 0;
    this->numKeys = 0;
    this->keyTime = 1.0f / this->keysPerSecond;
    this->startTime = 0.0f;
    this->endTime = 0.0f;
    if (this->keyArray)
    {
        delete[] this->keyArray;
        this->keyArray = 0;
    }
    if (this->packedKeyArray)
    {
        delete[] this->packedKeyArray;
        this->packedKeyArray = 0;
    }
}

//------------------------------------------------------------------------------
/**
    This creates a 4d-bounding-box of the keys, and if all of them
    are "identical", packs all keys into a single key.

    @return     true if curve could be packed into a single key
*/
inline
bool
nAnimCurve::Optimize()
{
    n_assert(this->numKeys > 0);

    // if curve only consists of one key, return immediately
    if (1 == this->numKeys)
    {
        return false;
    }

    // otherwise create a bounding box
    if (VANILLA == this->keyType)
    {
        n_assert(this->keyArray);
        
        int i;
        vector4 firstKey = this->keyArray[0];
        for (i = 1; i < this->numKeys; i++)
        {
            if (!firstKey.isequal(this->keyArray[i], 0.00001f))
            {
                // first unequal key, drop out
                return false;
            }
        }

        // fallthrough: all keys are identical, let's pack!
        float kps = this->keysPerSecond;
        int start = this->startKey;

        this->ClearKeys();
        this->BeginKeys(kps, start, 1, VANILLA);
        this->SetKey(0, firstKey);
        this->EndKeys();
        
        return true;
    }
    else
    {
        n_assert(this->packedKeyArray);

        int i;
        nPackedKey firstKey = this->packedKeyArray[0];
        for (i = 1; i < this->numKeys; i++)
        {
            if (!(firstKey == this->packedKeyArray[i]))
            {
                // first unequal key, drop out
                return false;
            }
        }

        // fallthrough: all keys are identical, let's pack!
        float kps = this->keysPerSecond;
        int start = this->startKey;

        this->ClearKeys();
        this->BeginKeys(kps, start, 1, PACKED);
        this->SetPackedKey(0, firstKey);
        this->EndKeys();
        
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimCurve::GetDataSize() const
{
    if (VANILLA == this->keyType)
    {
        return sizeof(vector4) * this->numKeys;
    }
    else
    {
        return sizeof(nPackedKey) * this->numKeys;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void*
nAnimCurve::GetDataPtr() const
{
    if (VANILLA == this->keyType)
    {
        n_assert(this->keyArray);
        return (void*) this->keyArray;
    }
    else
    {
        n_assert(this->packedKeyArray);
        return (void*) this->packedKeyArray;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimCurve::Pack()
{
    n_assert(this->keyArray);
    n_assert(!this->packedKeyArray);

    // allocate a packed key array
    this->packedKeyArray = new nPackedKey[this->numKeys];

    // transfer keys
    int i;
    for (i = 0; i < this->numKeys; i++)
    {
        this->packedKeyArray[i].Pack(this->keyArray[i]);
    }

    // delete unpacked key array, and switch type
    delete[] this->keyArray;
    this->keyArray = 0;
    this->keyType = PACKED;
}

//------------------------------------------------------------------------------
#endif
