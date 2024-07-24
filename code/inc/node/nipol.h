#ifndef N_IPOL_H
#define N_IPOL_H
//-------------------------------------------------------------------
/**
    @class nIpol
    @ingroup NebulaVisnodeModule

    @brief Animation interpolator

    A nipol (short for interpolator) object is able to animate
    properties of the parent object. Basically it goes like this:
     -# You create one, or more, nipol objects as children of the
        object to be animated. The class of the object to be animated
        must derive from a nVisNode class.
     -# You 'connect' the nipol object to a 'command' of its parent
        object. For the nipol class a channel is a script command which
        accepts 1..4 float arguments (for example n3dnode::t[xyz], 
        n3dnode::r[xyz], n3dnode::s[xyz], but also n3dlight::setcolor, 
        etc...).
     -# You select the channel that will drive the nipol, this is by 
        default the 'time' channel.
     -# You start to attach 1,2,3 or 4 dimensional keyframes to the
        nipol object (depending on what the channel needs). 
     -# Select the type of interpolation:
        -  Linear     - Does a linear interpolation between keyframes.
        -  Step       - Steps between keyframes, no interpolation.
        -  Quaternion - Spherical quaternion interpolation. Only makes 
           sense when connected to a command that takes quaternions.
        -  Cubic      - Cubic interpolation.
        -  Spline     - Catmull-Rom spline interpolation.
            
    The nipol node will attach to its parent as a depend node. The nipol
    will compute the actual interpolation values and generate a dispatch()
    call to the parent object.

    Consider also that the object does not animate the parent object
    (the to be animated object) BEFORE the first keyframe or AFTER the
    last keyframe. So you can have several interpolators working
    on the same channel but working one after the other.
    In the loop mode the timer switches from the last keyframe back
    to the start of the local timeline (t = 0). However the first keyframe
    does not have to start at (t = 0) but can be anytime later.
    the interpolator is completely inactive up to the point where the
    first keyframe is reached.
*/
//-------------------------------------------------------------------
#ifndef N_ANIMNODE_H
#include "node/nanimnode.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nIpol
#include "kernel/ndefdllclass.h"

//-------------------------------------------------------------------
class nIpol : public nAnimNode 
{
public:
    enum nIpolType
    {
        N_STEP,
        N_LINEAR,
        N_QUATERNION,
        N_CUBIC,
        N_SPLINE
    };

    /// constructor
    nIpol();
    /// destructor
    virtual ~nIpol();
    /// post namespace linkage initialization
    virtual void Initialize();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *);
    /// update object
    virtual void Compute(nSceneGraph2 *); 
    /// set script command to execute on target
    bool Connect(const char *);
    /// disconnect from target
//    void Disconnect(void);
    /// get the script command
    const char *GetConnect(void);
    /// set interpolation type
    void SetIpolType(nIpolType t);
    /// get interpolation type
    nIpolType GetIpolType();
    /// add a scalar key
    void AddKey1f(float time, float x);
    /// add a 2d key
    void AddKey2f(float time, float x, float y);
    /// add a 3d key
    void AddKey3f(float time, float x, float y, float z);
    /// add a 4d key
    void AddKey4f(float time, float x, float y, float z, float w);
    /// get current scalar value    
    void GetVal1f(float& x);
    /// get current 2d value
    void GetVal2f(float& x, float& y);
    /// get current 3d value
    void GetVal3f(float& x, float& y, float& z);
    /// get current 4d value
    void GetVal4f(float& x, float& y, float& z, float& w);
    /// begin keys (random access interface
    void BeginKeys(int num, int size);
    /// set scalar key
    void SetKey1f(int i, float t, float x);
    /// set 2d key
    void SetKey2f(int i, float t, float x, float y);
    /// set 3d key
    void SetKey3f(int i, float t, float x, float y, float z);
    /// set 4d key
    void SetKey4f(int i, float t, float x, float y, float z, float w);
    /// finish setting keys through the random access interface
    void EndKeys(void);
    /// get key information
    void GetKeyInfo(int& num, int& size);
    /// get scalar key definition
    void GetKey1f(int i, float& t, float& x);
    /// get 2d key definition
    void GetKey2f(int i, float& t, float& x, float& y);
    /// get 3d key definition
    void GetKey3f(int i, float& t, float& x, float& y, float& z);
    /// get 4f key definition
    void GetKey4f(int i, float& t, float& x, float& y, float& z, float& w);

    /// current interpolated value
    vector4 curval;

    static nKernelServer *kernelServer;

protected:
    enum nIpolSpecialCase
    {
        N_SCASE_NONE,
        N_SCASE_TXYZ,
        N_SCASE_TX,
        N_SCASE_TY,
        N_SCASE_TZ,
        N_SCASE_RXYZ,
        N_SCASE_RX,
        N_SCASE_RY,
        N_SCASE_RZ,
    };

    // a private interpolation key class
    class nIpolKey 
    {
    public:
        /// default constructor
        nIpolKey();
        /// constructor 1
        nIpolKey(float time, float x);
        /// constructor 2
        nIpolKey(float time, float x, float y);
        /// constructor 3
        nIpolKey(float time, float x, float y, float z);
        /// constructor 4
        nIpolKey(float time, float x, float y, float z, float w);
        /// get 1d value
        void Get(float& time, float& x);
        /// get 2d value
        void Get(float& time, float& x, float& y);
        /// get 3d value
        void Get(float& time, float& x, float& y, float& z);
        /// get 4d value
        void Get(float& time, float& x, float& y, float& z, float& w);

        float t;
        int num_f;
        vector4 f;
    };
     
    nIpolSpecialCase scase;
    nCmdProto *cmd_proto;
    nArray<nIpolKey> keyArray;
    nIpolType ipol_type;
    int key_size;
	int key_index;
};

//------------------------------------------------------------------------------
/**
*/
inline
nIpol::nIpolKey::nIpolKey() :
    t(0.0f),
    num_f(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nIpol::nIpolKey::nIpolKey(float time, float x) :
    t(time),
    num_f(1),
    f(x, 0.0f, 0.0f, 0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nIpol::nIpolKey::nIpolKey(float time, float x, float y) :
    t(time),
    num_f(2),
    f(x, y, 0.0f, 0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nIpol::nIpolKey::nIpolKey(float time, float x, float y, float z) :
    t(time),
    num_f(3),
    f(x, y, z, 0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nIpol::nIpolKey::nIpolKey(float time, float x, float y, float z, float w) :
    t(time),
    num_f(4),
    f(x, y, z, w)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nIpol::nIpolKey::Get(float& time, float& x)
{
    time = t;
    x = f.x;
}


//------------------------------------------------------------------------------
/**
*/
inline
void
nIpol::nIpolKey::Get(float& time, float& x, float& y) 
{
    time = t; 
    x = f.x; 
    y = f.y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nIpol::nIpolKey::Get(float& time, float& x, float& y, float& z) 
{
    time = t; 
    x = f.x; 
    y = f.y; 
    z = f.z;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nIpol::nIpolKey::Get(float& time, float& x, float& y, float& z, float& w) 
{
    time = t; 
    x = f.x; 
    y = f.y; 
    z = f.z; 
    w = f.w;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nIpol::SetIpolType(nIpolType t)
{
    this->ipol_type = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nIpol::nIpolType 
nIpol::GetIpolType(void)
{
    return this->ipol_type;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nIpol::GetVal1f(float& x) 
{
    x = this->curval.x;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nIpol::GetVal2f(float& x, float& y) 
{
    x = this->curval.x;
    y = this->curval.y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nIpol::GetVal3f(float& x, float& y, float& z) 
{
    x = this->curval.x;
    y = this->curval.y;
    z = this->curval.z;
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
nIpol::GetVal4f(float& x, float& y, float& z, float& w) 
{
    x = this->curval.x;
    y = this->curval.y;
    z = this->curval.z;
    w = this->curval.w;
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
nIpol::BeginKeys(int num, int size) 
{
    this->keyArray.Reallocate(num, num);
    this->key_size = size;
	this->key_index = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nIpol::SetKey1f(int i, float t, float x) 
{
    nIpolKey key(t, x);
    this->keyArray.Set(i, key);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nIpol::SetKey2f(int i, float t, float x, float y) 
{
    nIpolKey key(t, x, y);
    this->keyArray.Set(i, key);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nIpol::SetKey3f(int i, float t, float x, float y, float z) 
{
    nIpolKey key(t, x, y, z);
    this->keyArray.Set(i, key);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nIpol::SetKey4f(int i, float t, float x, float y, float z, float w) 
{
    nIpolKey key(t, x, y, z, w);
    this->keyArray.Set(i, key);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nIpol::EndKeys(void) 
{ 
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nIpol::GetKeyInfo(int& num, int& size) 
{
    num  = this->keyArray.Size();
    size = this->key_size;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nIpol::GetKey1f(int i, float& t, float& x) 
{
    this->keyArray.At(i).Get(t, x);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nIpol::GetKey2f(int i, float& t, float& x, float& y) 
{
    this->keyArray.At(i).Get(t, x, y);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nIpol::GetKey3f(int i, float& t, float& x, float& y, float& z)
{
    this->keyArray.At(i).Get(t, x, y, z);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nIpol::GetKey4f(int i, float& t, float& x, float& y, float& z, float& w) 
{
    this->keyArray.At(i).Get(t, x, y, z, w);
}

//------------------------------------------------------------------------------
#endif
