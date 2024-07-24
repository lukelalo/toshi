#ifndef N_MESHCLUSTER_H
#define N_MESHCLUSTER_H
//--------------------------------------------------------------------
/**
    @class nMeshCluster
    @ingroup NebulaVisnodeModule

    @brief Implements dynamic mesh where weighted vertices are
    bound to joints.
    
    This replaces the obsolete nCluster class. The main difference
    is that the weights and joint indices are now part of the vertex
    buffer, not the cluster object. This is more conformant to the way
    OpenGL and D3D handle vertex weights, and while we don't use their
    interfaces for skinning yet (because they suck, have to wait for
    D3D8's vertex shaders), the transition will be made easier.
*/
//--------------------------------------------------------------------
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nMeshCluster
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nGfxServer;
class nJoint;
class nShadowCaster;
class nShadowServer;
class nMeshCluster : public nVisNode 
{
public:
    /// constructor
    nMeshCluster();
    /// destructor
    virtual ~nMeshCluster();
    /// object persistency
    virtual bool SaveCmds(nPersistServer*);
    /// attach to scene
    virtual bool Attach(nSceneGraph2*);
    /// update and render
    virtual void Compute(nSceneGraph2*);
    /// set read only state
    void SetReadOnly(bool b); 
    /// get read only state
    bool GetReadOnly(void);
    /// set path to skin mesh
    void SetSkinMesh(const char *n); 
    /// get path to skin mesh
    const char *GetSkinMesh(void);
    /// set path to root joint
    void SetRootJoint(const char *n);
    /// get path to root joint
    const char *GetRootJoint(void);
    /// set shadow caster state
    void SetCastShadow(bool b);
    /// get shadow caster state
    bool GetCastShadow();

    static nClass *local_cl;
    static nKernelServer *ks;

private:
    /// do actual skinning
    void eval_skin(nVertexBuffer *, nSceneGraph2 *);
    /// begin defining joint palette
    void begin_joints(int num);
    /// set a joint in the joint palette
    void set_joint(int i, nJoint *o);
    /// finish defining joints
    void end_joints(void);
    /// count joints in joint palette
    void count_joints(nRoot *, int&);
    /// build the joint palette
    void collect_joints(nRoot *, int&);

    nAutoRef<nGfxServer>  ref_gs;
    nAutoRef<nShadowServer> refShadowServer;
    nDynAutoRef<nVisNode> ref_skin;     // the source skin (class nMeshNode)
    nDynAutoRef<nJoint>   ref_root;     // the root joint (class nJoint)
    nClass *njoint_class;

    // the joint palette
    int num_j;
    nJoint **j_array;
    bool joints_dirty;

    // dynamic vertex buffer (the render target)
    nDynVertexBuffer dyn_vb;

    // shadow stuff
    bool castShadow;
    nRef<nShadowCaster> refShadowCaster;
};

//--------------------------------------------------------------------
/**
*/
inline
nMeshCluster::nMeshCluster() :
    num_j(0),
    j_array(NULL),
    ref_gs(ks,this),
    refShadowServer(ks, this),
    ref_skin(ks,this),
    ref_root(ks,this),
    joints_dirty(false),
    dyn_vb(ks,this),
    njoint_class(NULL),
    castShadow(false),
    refShadowCaster(this)
{
    this->SetFlags(N_FLAG_SAVEUPSIDEDOWN);
    this->ref_gs = "/sys/servers/gfx";
    this->refShadowServer = "/sys/servers/shadow";
}

//--------------------------------------------------------------------
/**
*/
inline
void
nMeshCluster::SetReadOnly(bool b)
{
    this->dyn_vb.SetReadOnly(b);
}

//--------------------------------------------------------------------
/**
*/
inline
bool
nMeshCluster::GetReadOnly(void) 
{
    return this->dyn_vb.GetReadOnly();
}

//--------------------------------------------------------------------
/**
*/
inline
void
nMeshCluster::SetSkinMesh(const char* n)
{
    n_assert(n);
    this->ref_skin = n;
}

//--------------------------------------------------------------------
/**
*/
inline
const char *
nMeshCluster::GetSkinMesh(void) 
{
    return this->ref_skin.getname();
}

//--------------------------------------------------------------------
/**
*/
inline
const char *
nMeshCluster::GetRootJoint(void) 
{
    return this->ref_root.getname();
}

//--------------------------------------------------------------------
/**
*/
inline
void
nMeshCluster::SetCastShadow(bool b)
{
    this->castShadow = b;
}

//--------------------------------------------------------------------
/**
*/
inline
bool
nMeshCluster::GetCastShadow()
{
    return this->castShadow;
}

//--------------------------------------------------------------------
/**
*/
inline
void 
nMeshCluster::begin_joints(int num) 
{
    n_assert(num > 0);
    n_assert(!this->j_array);
    this->num_j   = num;
    this->j_array = new nJoint*[num];    
}

//--------------------------------------------------------------------
/**
*/
inline
void 
nMeshCluster::set_joint(int i, nJoint *o) 
{
    n_assert((i>=0) && (i<this->num_j));
    this->j_array[i] = o;
}

//--------------------------------------------------------------------
/**
*/
inline
void 
nMeshCluster::end_joints(void) 
{
    this->joints_dirty = true;
}

//--------------------------------------------------------------------
#endif
