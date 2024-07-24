#ifndef N_3DNODE_H
#define N_3DNODE_H
//------------------------------------------------------------------------------
/**
    @class n3DNode
    @ingroup NebulaVisnodeModule

    @brief Group and transform objects in the 3d world.

    n3DNode objects are both grouping nodes and transform nodes (as they
    are usually called). A n3DNode is usually the root object of 
    a hierarchy of nVisNode subclass objects, which form together
    a visible/audible object floating in 3d space. This can range
    from the most simplest static textured object (requiring a n3DNode,
    a nMeshNode, nShaderNode and nTextureArrayNode) to
    very complex animated 3d hierarchies, or joint skeletons that
    influence a weighted vertex skin. In any way, at the top is
    always a n3DNode.

    n3DNode have a few more features then just building hierarchies:

    - can lock position on viewer
    - sprite/billboard behaviour (automatic viewer relative orientation)
    - a simple "active/inactive" switch
    - lod computation, plus a "max lod" threshold
    - an optional quaternion interface
    - has a "high detail" flag, which can activate/deactivate the
      node based on the value in the "detail" channel

    (C) 2001 A.Weissflog
*/
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_QUATERNION_H
#include "mathlib/quaternion.h"
#endif

#undef N_DEFINES
#define N_DEFINES n3DNode
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nSceneGraph2;
class N_PUBLIC n3DNode : public nVisNode 
{
public:
    /// constructor
    n3DNode();

    /// persistency
    virtual bool SaveCmds(nPersistServer* fileServer);
    /// attach to scene
    virtual bool Attach(nSceneGraph2* sceneGraph);
    /// update internal state and set scene graph node transform
    virtual void Compute(nSceneGraph2*);

    /// collect required channels through nVisNode hierarchie
    virtual void AttachChannels(nChannelSet*);

    /// set active flag
    void SetActive(bool b);
    /// get active flag
    bool GetActive();
    /// set max lod threshold
    void SetMaxLod(float lod);
    /// get max lod threshold
    float GetMaxLod();

    /// set sprite behaviour
    void SetSprite(bool b);
    /// get sprite behaviour
    bool GetSprite();
    /// set billboard behaviour
    void SetBillboard(bool b);
    /// get billboard behaviour
    bool GetBillboard();
    /// lock position on viewer
    void SetLockViewer(bool b);
    /// get lock viewer status
    bool GetLockViewer();
    /// set/unset high detail flag
    void SetHighDetail(bool b);
    /// get the high detail flag
    bool GetHighDetail();
    /// set view space flag
    void SetViewSpace(bool b);
    /// get view space flag
    bool GetViewSpace();

    /// directly set matrix, overrides Txyz(), Rxyz() etc...
    void M(const matrix44& m);
    /// set xyz translation
    void Txyz(float x, float y, float z);
    /// set x translation
    void Tx(float x);
    /// set y translation
    void Ty(float y);
    /// set z translation
    void Tz(float z);
    /// set xyz rotation (rotation order is always x->y->z)
    void Rxyz(float x, float y, float z);
    /// set x rotation
    void Rx(float x);
    /// set y rotation
    void Ry(float y);
    /// set z rotation
    void Rz(float z);
    /// set xyz scale
    void Sxyz(float x, float y, float z);
    /// set x scale
    void Sx(float x);
    /// set y scale
    void Sy(float y);
    /// set z scale
    void Sz(float z);
    /// set quaternion
    void Qxyzw(float x, float y, float z, float w);

    /// get resulting matrix
    const matrix44& GetM();
    /// get translation
    void GetT(float& x, float& y, float& z);
    /// get rotation
    void GetR(float& x, float& y, float& z);
    /// get scale
    void GetS(float& x, float& y, float& z);
    /// get quaternion
    void GetQ(float& x, float& y, float& z, float& w);

    /// nKernelServer dependency.
    static nKernelServer* kernelServer;

protected:
    enum {
        MXDIRTY     = (1<<0),
        OVERRIDE    = (1<<1),
        SPRITE      = (1<<2),
        BILLBOARD   = (1<<3),
        LOCKVWR     = (1<<4),
        USEQUAT     = (1<<5),   // use (q) to validate mx, not (r)
        ACTIVE      = (1<<6),   // currently active?
        HIGHDETAIL  = (1<<7),   // high detail node?
        VIEWSPACE   = (1<<8),   // this nodes lives in viewer space
    };

    matrix44 mx;        // matrix resulting from (t,r,s) OR (t,q,s)
    vector3 t;          // translate vector
    vector3 r;          // rotation angles (rotation order x->y->z)
    vector3 s;          // scale
    quaternion q;       // quaternion (alternative to r)

    /// set a flag
    void set(int f);
    /// delete a flag
    void unset(int f);
    /// get a flag
    bool get(int f);

    float maxLod;
    int n3dFlags;
    int lodChn;
    int detailChn;

    void updateMatrix();
    void bboard_lookat(matrix44& m, vector3& to, vector3& up);
};

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::set(int f)
{
    this->n3dFlags |= f;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::unset(int f)
{
    this->n3dFlags &= ~f; 
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
n3DNode::get(int f)
{
    return (this->n3dFlags & f) ? true:false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::SetViewSpace(bool b)
{
    if (b) set(VIEWSPACE);
    else   unset(VIEWSPACE);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
n3DNode::GetViewSpace()
{
    return get(VIEWSPACE);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::SetActive(bool b)
{
    if (b) set(ACTIVE);
    else   unset(ACTIVE);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
n3DNode::GetActive()
{
    return get(ACTIVE);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::SetMaxLod(float lod)
{
    this->maxLod = lod;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
n3DNode::GetMaxLod()
{
    return maxLod;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::SetSprite(bool b) 
{
    if (b) set(SPRITE);
    else   unset(SPRITE);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
n3DNode::GetSprite(void) 
{
    return get(SPRITE);
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::SetBillboard(bool b) 
{
    if (b) set(BILLBOARD);
    else   unset(BILLBOARD);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
n3DNode::GetBillboard(void) 
{
    return get(BILLBOARD);
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::SetLockViewer(bool b) 
{
    if (b) set(LOCKVWR);
    else   unset(LOCKVWR);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
n3DNode::GetLockViewer(void) 
{
    return get(LOCKVWR);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::SetHighDetail(bool b)
{
    this->NotifyChannelSetDirty();
    if (b)  set(HIGHDETAIL);
    else    unset(HIGHDETAIL);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
n3DNode::GetHighDetail()
{
    return get(HIGHDETAIL);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::M(const matrix44& m) 
{
    unset(MXDIRTY);
    set(OVERRIDE);
    mx = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Txyz(float x, float y, float z) 
{
    set(MXDIRTY);
    unset(OVERRIDE);
    t.set(x,y,z);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Tx(float x) 
{
    set(MXDIRTY);
    unset(OVERRIDE);
    t.x = x;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Ty(float y) 
{
    set(MXDIRTY);
    unset(OVERRIDE);
    t.y = y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Tz(float z) 
{
    set(MXDIRTY);
    unset(OVERRIDE);
    t.z = z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Rxyz(float x, float y, float z) 
{
    set(MXDIRTY);
    unset(OVERRIDE);
    unset(USEQUAT);
    r.set(n_deg2rad(x),n_deg2rad(y),n_deg2rad(z));
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Rx(float x) 
{
    set(MXDIRTY);
    unset(OVERRIDE);
    unset(USEQUAT);
    r.x = n_deg2rad(x);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Ry(float y) 
{
    set(MXDIRTY);
    unset(OVERRIDE);
    unset(USEQUAT);
    r.y = n_deg2rad(y);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Rz(float z) 
{
    set(MXDIRTY);
    unset(OVERRIDE);
    unset(USEQUAT);
    r.z = n_deg2rad(z);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Sxyz(float x, float y, float z) 
{
    set(MXDIRTY);
    unset(OVERRIDE);
    s.set(x,y,z);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Sx(float x) 
{
    set(MXDIRTY);
    unset(OVERRIDE);
    s.x = x;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Sy(float y) 
{
    set(MXDIRTY);
    unset(OVERRIDE);
    s.y = y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Sz(float z) 
{
    set(MXDIRTY);
    unset(OVERRIDE);
    s.z = z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Qxyzw(float x, float y, float z, float w) 
{
    set(MXDIRTY);
    set(USEQUAT);
    unset(OVERRIDE);
    q.set(x,y,z,w);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::GetT(float& x, float& y, float& z) 
{
    x = t.x;
    y = t.y;
    z = t.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::GetR(float& x, float& y, float& z) 
{
    x = n_rad2deg(r.x);
    y = n_rad2deg(r.y);
    z = n_rad2deg(r.z);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::GetS(float& x, float& y, float& z) 
{
    x = s.x;
    y = s.y;
    z = s.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::GetQ(float& x, float& y, float& z, float& w) 
{
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
n3DNode::GetM() 
{
    this->updateMatrix();
    return this->mx;
}

//--------------------------------------------------------------------
#endif
