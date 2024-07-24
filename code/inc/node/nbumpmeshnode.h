#ifndef N_BUMPMESHNODE_H
#define N_BUMPMESHNODE_H
//------------------------------------------------------------------------------
/* Copyright (c) 2002 Dirk Ulbricht of Insane Software
 *
 * See the file "nbump_license.txt" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------
/**
    @class nBumpMeshNode
    @ingroup NebulaVisnodeModule

    @brief A node that encodes a light position into the color channel
    of the source mesh

    The nBumpMeshNode class is an implementation of tangent space dot3 bump
    mapping. The light position is tranformed into the tangent space of
    every vertex and this vector is then encoded in the vertex color channel
    of the source vertex buffer. A nShaderNode is then set up to perform
    the dot3 product between the vertex color (our encoded light vector) and
    a texture. For this to work the texture needs to have the surface normals
    encoded - see nvidias developer pages (http://www.nvidia.com/developer) for
    tools that convert bump maps into normal maps.

    See the included script for an example of how to use nBumpMeshNode.

    See nebula/doc/source/nbump_license.txt for licensing terms.

    Requirements:
     - GeForce256/Radeon7000 class hardware or better
     - meshes need proper uv coordinates

    Limitations
     - not 100% correct bump mapping, the light vector is not
       renormalized per pixel. As a result, brightness might
       vary across a flat surface. Fixing this requires more
       advanced hardware (gf2) and improved nebula shaders.
     - very simple lighting model: one point light without
       attenuation, no dynamic vertex lighting possible
     - no self shadowing (also requires at least gf2 class
       hardware and improved nebula shaders)

     @todo Integrate better with nebula lights.
     @todo Dynamic mesh support (simply do the tangent setup every frame?
           could stuff be precomputed?).
     @todo This code could probably be optimized a lot.

    (C) 2002 Insane Software
*/
//------------------------------------------------------------------------------
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#include "node/n3dnode.h"

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#undef N_DEFINES
#define N_DEFINES nBumpMeshNode

#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nGfxServer;
class nJoint2;
class nShadowCaster;
class nShadowServer;

// used to store additional per-vertex info (tangent, binormal and normal)
struct nDot3Info
{
    vector3 S, T, SxT;
    nDot3Info(vector3 s, vector3 t, vector3 sxt) : S(s), T(t), SxT(sxt) {}
    nDot3Info() {}
    nDot3Info(const nDot3Info& src) { S = src.S; T = src.T; SxT = src.SxT; }
    nDot3Info& operator=(const nDot3Info& rhs) { S = rhs.S; T = rhs.T; SxT = rhs.SxT; return (*this);}
};

class nBumpMeshNode : public nVisNode
{
public:
    /// constructor
    nBumpMeshNode();
    /// destructor
    virtual ~nBumpMeshNode();
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
    void SetMesh(const char *n); 
    /// get path to mesh
    const char *GetMesh(void);
    
    /// set path to light position provider
    void SetLight(const char* n);
    /// get path to light provider
    const char* GetLight(void);
    
    /// set shadow caster state
    void SetCastShadow(bool b);
    /// get shadow caster state
    bool GetCastShadow();

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:
    /// render the mesh
    void RenderMesh(nSceneGraph2* sceneGraph, nVertexBuffer* srcMesh);

    /// setup tangent space for every vertex
    void setupTangentSpace(nVertexBuffer* mesh, nIndexBuffer* tris);
  
    nAutoRef<nGfxServer>    refGfx;
    nAutoRef<nShadowServer> refShadowServer;
    nDynAutoRef<nVisNode>   refMesh;
    nDynAutoRef<n3DNode>    refLight;

    nArray<nDot3Info>  dot3info;
    bool dot3IsDirty;

    nDynVertexBuffer dynVBuf;               // the render target

    bool castShadow;                        // act as shadow caster?
    nRef<nShadowCaster> refShadowCaster;    // ref to shadow caster object
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nBumpMeshNode::SetReadOnly(bool b)
{
    this->dynVBuf.SetReadOnly(b);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nBumpMeshNode::GetReadOnly()
{
    return this->dynVBuf.GetReadOnly();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBumpMeshNode::SetMesh(const char* n)
{
    n_assert(n);
    this->refMesh = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nBumpMeshNode::GetMesh()
{
    return this->refMesh.getname();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBumpMeshNode::SetCastShadow(bool b)
{
    this->castShadow = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nBumpMeshNode::GetCastShadow()
{
    return this->castShadow;
}

//------------------------------------------------------------------------------
/**
/ get path to light provider
*/
inline
const char* nBumpMeshNode::GetLight(void)
{
    if (refLight.isvalid())
        return refLight.getname();
    return NULL;
}

//------------------------------------------------------------------------------
/**
/ set path to light position provider
*/
inline
void nBumpMeshNode::SetLight(const char* n)
{
    refLight = n;
}

//------------------------------------------------------------------------------
#endif
