#ifndef N_D3D8VERTEXPOOL_H
#define N_D3D8VERTEXPOOL_H
//-----------------------------------------------------------------------------
/**
    @class nD3D8VertexPool

    @brief vertex pool class on top of d3d8

    A Nebula vertex pool holds the vertices for many vertex buffer
    objects at once. The goal is to reduce vertex buffer switches inside
    the 3d api, which is considered evil. Before rendering, vertex buffer
    objects should be grouped by the vertex pool they are contained in.
    This is normally done inside Nebula's scene graph object.
    The vertex pool objects are managed internally by the gfx server
    object, they are never visible to the client code (that's what the
    nVertexBuffer objects are there for).

    A nD3D8VertexPool object behaves very differently internally for
    static (N_VBTYPE_STATIC) and dynamic (N_VBTYPE_WRITEONLY) vertex 
    pools (as indicated by the nVBufType constructor parameter):

    A static vertex pool can contain many vertex bubbles, and will
    eventually fill up (when there is no space left in the pool for
    new vertex bubbles), so that the responsible vertex pool manager
    will create a new vertex pool. Normally there will be several
    static vertex pools for each used vertex format.

    In the case of dynamic vertex pools, there will only be ONE
    vertex bubble which spans the whole pool. Internally, a
    D3D8 vertex buffer object is used, which is created with the
    D3DUSAGE_WRITEONLY and D3DUSAGE_DYNAMIC flags, and lock with 
    the D3DLOCK_NOSYSLOCK and D3DLOCK_DISCARD flags, which basically
    means that d3d will do the necessary housekeeping (double buffering
    etc internally). 

    To the outside, both techniques should look the same, just don't
    try to do anything funky with the returned nVertexBubble objects.
    Actually, other vertex pool implementation may choose to implement
    a completely different mechanism, whatever is best for the
    backend 3d api.

    (C) 2001 A.Weissflog
*/
#ifndef N_VERTEXPOOL_H
#include "gfx/nvertexpool.h"
#endif

#ifndef N_D3D8SERVER_H
#include "gfx/nd3d8server.h"
#endif

//------------------------------------------------------------------------------
class nD3D8VertexPool : public nVertexPool
{
public:
    /// the constructor
    nD3D8VertexPool(
        nGfxServer *gs,             // the responsible gfx server object
        nVertexPoolManager *vpm,    // the vertex pool manager object owning this pool
        nVBufType vbt,              // useage indicator for this pool
        int vt,                     // vertex format for this pool
        int size);                  // number of vertices in this pool
    /// the destructor
    virtual ~nD3D8VertexPool();
    /// initialize the vertex pool (call before first use)
    virtual bool Initialize();
    /// returns the native vertex color format for this vertex pool
    virtual nColorFormat GetColorFormat();
    /// return a new vertex bubble object, or NULL if pool full
    virtual nVertexBubble *NewBubble(int size);
    /// release a vertex bubble object owned by this pool
    virtual void ReleaseBubble(nVertexBubble *vbubble);
    /// lock a vertex bubble inside the pool
    virtual bool LockBubble(nVertexBubble *vbubble);
    /// unlock a vertex bubble inside the pool
    virtual void UnlockBubble(nVertexBubble *vbubble);
    /// render a vertex bubble owned by this pool
    virtual void RenderBubble(nVertexBubble *vbubble, nIndexBuffer *ib, nPixelShader *ps, nTextureArray *ta);

private:
    IDirect3DVertexBuffer8 *d3dVBuffer;
    DWORD d3dFVF;
};

//-----------------------------------------------------------------------------
#endif
