#ifndef N_MESHIPOL_H
#define N_MESHIPOL_H
//------------------------------------------------------------------------------
/**
    @class nMeshIpol
    @ingroup NebulaVisnodeModule

    @brief Interpolates between meshes.

    nMeshIpol generates a dynamic mesh which is the interpolated
    result between 2 source meshes with the same number and structure
    of vertex components. Any number of source meshes can be chained
    to result in a complex animation. Interpolation can be explicitely
    enabled/disabled for each vertex component, the default is to
    disable all component interpolation.

    (C) 2001 A.Weissflog
*/
#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#ifndef N_ANIMNODE_H
#include "node/nanimnode.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nMeshIpol
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nMeshNode;
class nMeshIpolKey 
{
public:
    nDynAutoRef<nVisNode> ref_source;
    float t;

    /// constructor
    nMeshIpolKey()
        : t(0.0f)
    { };
    /// set timestamp and name of source mesh
    void Set(float tstamp, const char *s) 
    {
        n_assert((tstamp >= 0.0f) && s);
        ref_source = s;
        t = tstamp;
    };
};

//------------------------------------------------------------------------------
class nSceneGraph2;
class nMeshIpol : public nAnimNode 
{
public:
    /// constructor
    nMeshIpol();
    /// destructor
    virtual ~nMeshIpol();

    /// persistency
    virtual bool SaveCmds(nPersistServer*);
    /// attach to scene graph
    virtual bool Attach(nSceneGraph2*);
    /// update and render
    virtual void Compute(nSceneGraph2 *);
    
    /// set read only state (if source mesh for another dynamic mesh)
    void SetReadOnly(bool b);
    /// get read only state
    bool GetReadOnly();
    /// start setting animation keys
    void BeginKeys(int num);
    /// set an animation key
    void SetKey(int index, float time, const char* meshNode);
    /// finish setting keys
    void EndKeys();
    
    /// get number of animation keys
    int GetNumKeys();
    /// get key definition at given key index
    void GetKey(int index, float& time, const char *& meshNode);
    /// enable/disable interpolation of 3d coords
    void SetUpdateCoord(bool b);
    /// 3d coord interpolation enabled?
    bool GetUpdateCoord();
    /// enable/disable interpolation of 3d normals
    void SetUpdateNorm(bool b);
    /// 3d normal interpolation enabled?
    bool GetUpdateNorm();
    /// enable/disable interpolation of vertex colors
    void SetUpdateColor(bool b);
    /// vertex color interpolation enabled?
    bool GetUpdateColor();

    /// enable/disable interpolation of uv set 0
    void SetUpdateUv0(bool b);
    /// uv set 0 interpolation enabled?
    bool GetUpdateUv0();
    /// enable/disable interpolation of uv set 1
    void SetUpdateUv1(bool b);
    /// uv set 1 interpolation enabled?
    bool GetUpdateUv1();
    /// enable/disable interpolation of uv set 2
    void SetUpdateUv2(bool b);
    /// uv set 2 interpolation enabled?
    bool GetUpdateUv2();
    /// enable/disable interpolation of uv set 3
    void SetUpdateUv3(bool b);
    /// uv set 3 interpolation enabled?
    bool GetUpdateUv3();

    static nClass* local_cl;
    static nKernelServer* ks;

private:
    enum {
        N_UPDATE_COORD  = (1<<0),
        N_UPDATE_NORM   = (1<<1),
        N_UPDATE_RGBA   = (1<<2),
        N_UPDATE_UV0    = (1<<3),
        N_UPDATE_UV1    = (1<<4),
        N_UPDATE_UV2    = (1<<5),
        N_UPDATE_UV3    = (1<<6),
    };

    int update_flags;
    int num_keys;
    nMeshIpolKey *key_array;
    bool in_begin_keys;
    nDynVertexBuffer dyn_vb;

    void interpolate(nSceneGraph2* sceneGraph, float l, nVertexBuffer *vb0, nVertexBuffer *vb1);
};

//------------------------------------------------------------------------------
/**
*/
inline
nMeshIpol::nMeshIpol() :
    update_flags(0),
    num_keys(0),
    key_array(0),
    in_begin_keys(false),
    dyn_vb(ks, this)
{
    // make sure source meshes are saved before myself
    this->SetFlags(N_FLAG_SAVEUPSIDEDOWN);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshIpol::SetReadOnly(bool b)
{
    this->dyn_vb.SetReadOnly(b);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshIpol::GetReadOnly()
{
    return this->dyn_vb.GetReadOnly();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nMeshIpol::BeginKeys(int num) 
{
    n_assert(NULL == this->key_array);
    n_assert(num > 0);
    this->in_begin_keys = true;
    this->num_keys  = num;
    this->key_array = new nMeshIpolKey[num];
    int i;
    for (i=0; i<num; i++) 
    {
        this->key_array[i].ref_source.initialize(ks,this);
    };
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nMeshIpol::SetKey(int index, float time, const char* meshNode) 
{
    n_assert((index >= 0) && (index < this->num_keys));
    n_assert(this->in_begin_keys);
    n_assert(meshNode);
    this->key_array[index].Set(time, meshNode);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nMeshIpol::EndKeys(void) 
{
    n_assert(this->in_begin_keys);
    this->in_begin_keys = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshIpol::GetNumKeys(void) 
{
    return this->num_keys;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshIpol::GetKey(int index, float& time, const char*& meshNode) 
{
    n_assert((index >= 0) && (index < this->num_keys));
    time     = this->key_array[index].t;
    meshNode = this->key_array[index].ref_source.getname();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nMeshIpol::SetUpdateCoord(bool b) 
{
    if (b) this->update_flags |= N_UPDATE_COORD;
    else   this->update_flags &= ~N_UPDATE_COORD;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nMeshIpol::GetUpdateCoord(void) 
{
    return (this->update_flags & N_UPDATE_COORD) ? true : false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nMeshIpol::SetUpdateNorm(bool b) 
{
    if (b) this->update_flags |= N_UPDATE_NORM;
    else   this->update_flags &= ~N_UPDATE_NORM;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nMeshIpol::GetUpdateNorm(void) 
{
    return (this->update_flags & N_UPDATE_NORM) ? true : false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nMeshIpol::SetUpdateColor(bool b) 
{
    if (b) this->update_flags |= N_UPDATE_RGBA;
    else   this->update_flags &= ~N_UPDATE_RGBA;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshIpol::GetUpdateColor(void) 
{
    return (this->update_flags & N_UPDATE_RGBA) ? true : false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nMeshIpol::SetUpdateUv0(bool b) 
{
    if (b) this->update_flags |= N_UPDATE_UV0;
    else   this->update_flags &= ~N_UPDATE_UV0;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nMeshIpol::GetUpdateUv0(void) 
{
    return (this->update_flags & N_UPDATE_UV0) ? true : false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nMeshIpol::SetUpdateUv1(bool b) 
{
    if (b) this->update_flags |= N_UPDATE_UV1;
    else   this->update_flags &= ~N_UPDATE_UV1;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nMeshIpol::GetUpdateUv1(void) 
{
    return (this->update_flags & N_UPDATE_UV1) ? true : false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nMeshIpol::SetUpdateUv2(bool b) 
{
    if (b) this->update_flags |= N_UPDATE_UV2;
    else   this->update_flags &= ~N_UPDATE_UV2;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nMeshIpol::GetUpdateUv2(void) 
{
    return (this->update_flags & N_UPDATE_UV2) ? true : false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nMeshIpol::SetUpdateUv3(bool b) 
{
    if (b) this->update_flags |= N_UPDATE_UV3;
    else   this->update_flags &= ~N_UPDATE_UV3;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nMeshIpol::GetUpdateUv3(void) 
{
    return (this->update_flags & N_UPDATE_UV3) ? true : false;
}

//------------------------------------------------------------------------------
#endif
