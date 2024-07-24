#ifndef N_MESHCLUSTER2_H
#define N_MESHCLUSTER2_H
//------------------------------------------------------------------------------
/**
    @class nMeshCluster2
    @ingroup NebulaVisnodeModule

    @brief New mesh cluster class (renders a weighted vertex skin) which
    works together with nJoint2 objects.

    (C) 2001 RadonLabs GmbH
*/
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

#undef N_DEFINES
#define N_DEFINES nMeshCluster2
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nGfxServer;
class nJoint2;
class nShadowCaster;
class nShadowServer;
class nMeshCluster2 : public nVisNode
{
public:
    /// constructor
    nMeshCluster2();
    /// destructor
    virtual ~nMeshCluster2();
    /// persistency
    virtual bool SaveCmds(nPersistServer* fileServer);
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

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:
    /// count number of joints in skeleton
    int CountJoints(nRoot* joint, int num);
    /// recursively add joints to joint palette
    int CollectJoints(nRoot* joint, int curJointIndex);
    /// build the joint palette
    void BuildJointPalette();
    /// render the skin
    void RenderSkin(nSceneGraph2* sceneGraph, nVertexBuffer* srcSkin);

    nAutoRef<nGfxServer>    refGfx;
    nAutoRef<nShadowServer> refShadowServer;
    nDynAutoRef<nVisNode>   refSkin;
    nDynAutoRef<nVisNode>   refBones;
    nClass* njoint2Class;

    int numJoints;                          // number of joints in joint palette
    nJoint2** jointPalette;                  // the joint palette
    bool jointsDirty;                       // joint palette dirty?

    nDynVertexBuffer dynVBuf;               // the render target

    bool castShadow;                        // act as shadow caster?
    nRef<nShadowCaster> refShadowCaster;    // ref to shadow caster object
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshCluster2::SetReadOnly(bool b)
{
    this->dynVBuf.SetReadOnly(b);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshCluster2::GetReadOnly()
{
    return this->dynVBuf.GetReadOnly();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshCluster2::SetSkinMesh(const char* n)
{
    n_assert(n);
    this->refSkin = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nMeshCluster2::GetSkinMesh()
{
    return this->refSkin.getname();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nMeshCluster2::GetRootJoint()
{
    return this->refBones.getname();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshCluster2::SetCastShadow(bool b)
{
    this->castShadow = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshCluster2::GetCastShadow()
{
    return this->castShadow;
}

//------------------------------------------------------------------------------
#endif
