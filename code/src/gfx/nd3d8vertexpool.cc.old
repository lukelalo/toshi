#define N_IMPLEMENTS nD3D8VertexPool
//-----------------------------------------------------------------------------
//  nd3d8vertexpool.cc
//  (C) 2001 A.Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nd3d8vertexpool.h"
#include "gfx/nvertexbubble.h"
#include "gfx/nd3d8indexbuffer.h"
#include "gfx/npixelshader.h"

extern const char *nd3d8_Error(HRESULT hr);

//-----------------------------------------------------------------------------
/**
    The default constructor does not actually create a d3d vertex buffer
    object, but only initialized the instance data. Call Initialize() to
    get a fully useable object!

    @param gs       the responsible gfx server object
    @param vpm      the vertex pool manager object owning this pool
    @param vbt      usage indicator for this pool
    @param vt       vertex format for this pool (combination of N_VT_*)
    @param size     number of vertices in this pool
*/
nD3D8VertexPool::nD3D8VertexPool(
    nGfxServer* gs,
    nVertexPoolManager* vpm,
    nVBufType vbt,
    int vt,
    int size)
:   
    nVertexPool(gs,vpm,vbt,vt,size),
    d3dVBuffer(0),
    d3dFVF(0)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    If exists, release the d3d8 vertex buffer object.
*/
nD3D8VertexPool::~nD3D8VertexPool()
{
    if (this->d3dVBuffer)
    {
        this->d3dVBuffer->Release();
        this->d3dVBuffer = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    The nD3D8VertexPool uses the N_COLOR_BGRA format for the vertex color
    component.
*/
nColorFormat
nD3D8VertexPool::GetColorFormat()
{
    return N_COLOR_BGRA;
}

//-----------------------------------------------------------------------------
/**
    Will create the actual d3d8 vertex buffer object with the size and
    usage flags requested by the constructor params. Will create the
    initial vertex bubble which spans the whole pool.

    @return     true if d3d8 vertex buffer could be created

    11-Jul-01   floh    force static index and vertex buffers into system mem, to save
                        video mem for textures
*/
bool
nD3D8VertexPool::Initialize()
{
    n_assert(this->vtype != 0);
    n_assert(this->vnum != 0);
    n_assert(0 == this->d3dVBuffer);
    n_assert(this->vbuf_type != N_VBTYPE_READONLY);

    nD3D8Server *gs = (nD3D8Server *) this->ref_gs.get();
    n_assert(gs->d3d8Dev);
    HRESULT hr;

    // make sure there are no weights and joint indices in the
    // vertex format, we only hand "pure" vertex data to d3d
    // for now (until we make use of custom vertex shaders)
    n_assert((this->vtype & N_VT_JW) == 0);

    // get the FVF flags and vertex size
    this->d3dFVF = 0;
    int v_size   = 0;
    if (this->vtype & N_VT_COORD) 
    {
        this->d3dFVF |= D3DFVF_XYZ;
        v_size += 3*sizeof(float);
    }
    if (this->vtype & N_VT_NORM)  
    {
        this->d3dFVF |= D3DFVF_NORMAL;
        v_size += 3*sizeof(float);
    }
    if (this->vtype & N_VT_RGBA)  
    {
        this->d3dFVF |= D3DFVF_DIFFUSE;
        v_size += sizeof(DWORD);
    }

    int num_tcoords = 0;
    if (this->vtype & N_VT_UV0) num_tcoords = 1;
    if (this->vtype & N_VT_UV1) num_tcoords = 2;
    if (this->vtype & N_VT_UV2) num_tcoords = 3;
    if (this->vtype & N_VT_UV3) num_tcoords = 4;
    v_size += num_tcoords * 2 * sizeof(float);

    switch (num_tcoords) 
    {
        case 1: this->d3dFVF |= D3DFVF_TEX1; break;
        case 2: this->d3dFVF |= D3DFVF_TEX2; break;
        case 3: this->d3dFVF |= D3DFVF_TEX3; break;
        case 4: this->d3dFVF |= D3DFVF_TEX4; break;
    }

    // get the usage indicator and resource manager flags
    // (we let static vertex buffers manage by the d3d
    // resource manager, but this makes no sense for 
    // dynamic vertex buffers)
    DWORD d3d_usage;
    D3DPOOL d3d_pool;
    if (N_VBTYPE_STATIC == this->vbuf_type)
    {
        d3d_usage = D3DUSAGE_WRITEONLY;
//        d3d_pool  = D3DPOOL_SYSTEMMEM;
        d3d_pool  = D3DPOOL_MANAGED;
    }
    else
    {
        d3d_usage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
        d3d_pool  = D3DPOOL_SYSTEMMEM;  // DEFAULT collides with shadows (why?)
    }
    
    // create the d3d vertex buffer object
    hr = gs->d3d8Dev->CreateVertexBuffer(
            this->vnum * v_size,    // Length
            d3d_usage,              // Usage
            this->d3dFVF,           // FVF
            d3d_pool,               // Pool
            &(this->d3dVBuffer));   // ppVertexBuffer
    if (FAILED(hr))
    {
        n_printf("nD3D8VertexPool: CreateVertexBuffer failed with '%s'!\n",nd3d8_Error(hr));
        return false;
    }

    // initialize remaining superclass members
    this->stride4 = v_size>>2;

    // create the initial 'free' bubble which represents the whole
    // vertex pool
    nVertexBubble *vb = n_new nVertexBubble(this, 0, this->vnum);
    if (N_VBTYPE_STATIC == this->vbuf_type) 
        // static: add bubble to free pool 
        this->free_bubbles.AddTail(vb);
    else
        // dynamic: add bubble to full pool, since there will only be one bubble
        this->full_bubbles.AddTail(vb);

    return true;
}

//-----------------------------------------------------------------------------
/**
    Create a new vertex bubble object. If we are a static vertex pool,
    we simply hand the request to the superclass, since she does the
    right thing. If we are a dynamic vertex pool, we always return
    the same bubble which spans the whole vertex pool!

    @param  size    the number of vertices in the bubble
    @return         nVertexBubble object, or NULL if too big for this pool
*/
nVertexBubble*
nD3D8VertexPool::NewBubble(int size)
{
    if (this->vbuf_type == N_VBTYPE_STATIC)
    {
        // if we are a static vertex pool, simply hand the request
        // to our superclass nVertexBuffer 
        return nVertexPool::NewBubble(size);
    }
    else
    {
        // if we are a dynamic vertex pool, we will always return
        // the same bubble which spans the whole vertex pool
        if (size <= this->vnum)
        {
            nVertexBubble* vb = (nVertexBubble*) this->full_bubbles.GetHead();
            n_assert(vb);
            return vb;
        }
        else
        {
            // bubble too big for this pool
            return 0;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Release a vertex bubble created and owned by us. Hand bubble to
    superclass nVertexPool if we are a static pool, in the dynamic case
    do nothing (@see nD3D8VertexPool::NewBubble())

    @param  vbubble     the vertex bubble object to be released
*/
void
nD3D8VertexPool::ReleaseBubble(nVertexBubble* vbubble)
{
    n_assert(vbubble);
    n_assert(vbubble->GetVertexPool() == this);

    if (this->vbuf_type == N_VBTYPE_STATIC)
    {
        // if we are a static vertex pool, simply hand the request
        // to our superclass nVertexBuffer 
        nVertexPool::ReleaseBubble(vbubble);
    }
    else
    {
        // the dynamic vertex pool case: don't delete the vertex pool
        // object, because it is shared amongst all clients

        // if this is the global 'current bubble', set it to NULL
        if (vbubble == this->ref_gs->GetCurrentVertexBubble()) 
        {
            this->ref_gs->SetCurrentVertexBubble(NULL);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Lock a vertex bubble owned by this pool for write access. Nebula
    doesn't support reading from vertex buffers!

    @param  vbubble     the vertex bubble to be locked
    @return true if the bubble could be locked
*/
bool
nD3D8VertexPool::LockBubble(nVertexBubble* vbubble)
{
    n_assert(vbubble);
    n_assert(vbubble->GetVertexPool() == this);
    n_assert(this->d3dVBuffer);
    
    HRESULT hr;
    
    // compute lock parameters
    int vertexSize = this->stride4<<2;
    UINT offsetToLock;
    UINT sizeToLock;
    DWORD lockFlags;
    if (N_VBTYPE_STATIC == this->vbuf_type)
    {
        // if we are a static vertex pool, only lock the
        // portion of the pool as defined by the vertex bubble
        offsetToLock = vbubble->GetStart() * vertexSize;
        sizeToLock   = vbubble->GetBubbleSize() * vertexSize;
        lockFlags    = D3DLOCK_NOSYSLOCK;
    }
    else
    {
        // we are a dynamic vertex pool, lock the entire buffer
        offsetToLock = 0;
        sizeToLock   = 0;
        lockFlags    = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
    }

    // lock da buffa
    float *basePtr = 0;
    hr = this->d3dVBuffer->Lock(offsetToLock, sizeToLock, (BYTE**) &basePtr, lockFlags);
    if (FAILED(hr))
    {
        n_printf("nD3D8VertexPool: Lock() failed with '%s'\n", nd3d8_Error(hr));
        return false;
    }
    n_assert(basePtr);

    // fill vertex component pointers
    if (this->vtype & N_VT_COORD) 
    {
        this->coord_ptr = basePtr;
        basePtr += 3;
    } else this->coord_ptr = NULL;
    
    if (this->vtype & N_VT_NORM) 
    {
        this->norm_ptr = basePtr;
        basePtr += 3;
    } else this->norm_ptr = NULL;
    
    if (this->vtype & N_VT_RGBA) 
    {
        this->color_ptr = (ulong *) basePtr;
        basePtr += 1;
    } else this->color_ptr = NULL;

    if (this->vtype & N_VT_UV0) 
    {
        this->uv_ptrs[0] = basePtr; 
        basePtr += 2;
    } else this->uv_ptrs[0] = NULL;
    
    if (this->vtype & N_VT_UV1) 
    {
        this->uv_ptrs[1] = basePtr;
        basePtr += 2;
    } else this->uv_ptrs[1] = NULL;
    
    if (this->vtype & N_VT_UV2) 
    {
        this->uv_ptrs[2] = basePtr;
        basePtr += 2;
    } else this->uv_ptrs[2] = NULL;
    
    if (this->vtype & N_VT_UV3) 
    {
        this->uv_ptrs[3] = basePtr;
        basePtr += 2;
    } else this->uv_ptrs[3] = NULL;

    // done.
    return true;
}

//-----------------------------------------------------------------------------
/**
    Unlock a locked vertex bubble.

    @param  vbubble     the nVertexBubble object to be unlocked
*/
void
nD3D8VertexPool::UnlockBubble(nVertexBubble* vbubble)
{
    n_assert(vbubble);
    n_assert(vbubble->GetVertexPool() == this);
    n_assert(this->d3dVBuffer);

    // invalidate vertex component pointers
    this->coord_ptr = 0;
    this->norm_ptr  = 0;
    this->color_ptr = 0;
    this->uv_ptrs[0] = 0;
    this->uv_ptrs[1] = 0;
    this->uv_ptrs[2] = 0;
    this->uv_ptrs[3] = 0;

    // unlock the d3d vertex buffer
    HRESULT hr;
    hr = this->d3dVBuffer->Unlock();
    if (FAILED(hr))
    {
        n_printf("nD3D8VertexPool: Unlock() failed with '%s'\n", nd3d8_Error(hr));
    }
}

//-----------------------------------------------------------------------------
/**
    Render a vertex bubble. Requires a pointer to nIndexBuffer, nPixelShader
    and nTextureArray objects. 

    @param  vbubble     the vertex bubble (must be owned by this pool)
    @param  ib          the index buffer object (actually of class nD3D8IndexBuffer!)
    @param  ps          the pixel shader object (can be NULL)
    @param  ta          the texture array object (can be NULL)
*/
void
nD3D8VertexPool::RenderBubble(nVertexBubble* vbubble,
                              nIndexBuffer* ib,
                              nPixelShader* ps,
                              nTextureArray* ta)
{
    n_assert(this->d3dVBuffer);
    n_assert(vbubble);
    n_assert(ib);
    HRESULT hr;

    nD3D8Server* gs = (nD3D8Server*) this->ref_gs.get();
    IDirect3DDevice8* dev = gs->d3d8Dev;
    n_assert(dev);

    // cast pointer to nD3D8IndexBuffer (ouch!)
    nD3D8IndexBuffer *d3d8_ib = (nD3D8IndexBuffer*) ib;

    // setup d3d vertex stream stuff
    UINT baseIndex   = vbubble->GetStart();
    UINT numVertices = vbubble->GetTruncSize();
    hr = dev->SetIndices(d3d8_ib->GetD3D8IndexBuffer(), baseIndex);
    if (FAILED(hr))
    {
        n_printf("nD3D8VertexPool: SetIndices() failed with '%s'!\n", nd3d8_Error(hr));
        return;
    }
    hr = dev->SetStreamSource(0, this->d3dVBuffer, this->stride4<<2);
    if (FAILED(hr))
    {
        n_printf("nD3D8VertexPool: SetStreamSource() failed with '%s'!\n", nd3d8_Error(hr));
        return;
    }
    hr = dev->SetVertexShader(this->d3dFVF);
    if (FAILED(hr))
    {
        n_printf("nD3D8VertexPool: SetVertexShader() failed with '%s'\n", nd3d8_Error(hr));
        return;
    }

    // get primitive type, number of primitives, and so on...
    D3DPRIMITIVETYPE d3dPType;
    UINT numPrimitives;
    unsigned int numIndices = ib->GetNumIndices();
    nPrimType primType = ib->GetPrimType();
    switch (primType)
    {
        case N_PTYPE_TRIANGLE_LIST:
            d3dPType = D3DPT_TRIANGLELIST;
            numPrimitives = numIndices/3;
            break;

        case N_PTYPE_LINE_LIST:
            d3dPType = D3DPT_LINELIST;
            numPrimitives = numIndices/2;
            break;
        
        case N_PTYPE_POINT_LIST:
            d3dPType = D3DPT_POINTLIST;
            numPrimitives = numIndices;
            break;

        case N_PTYPE_LINE_STRIP:
            d3dPType = D3DPT_LINESTRIP;
            numPrimitives = numIndices/2;
            break;
        
        case N_PTYPE_TRIANGLE_STRIP:
            d3dPType = D3DPT_TRIANGLESTRIP;
            numPrimitives = numIndices - 2;
            break;
        
        case N_PTYPE_TRIANGLE_FAN:
            d3dPType = D3DPT_TRIANGLEFAN;
            numPrimitives = numIndices - 2;
            break;
        
        default:
            n_printf("nD3D8VertexPool: Unsupported primitive type!\n");
            return;
    }
    n_assert(numPrimitives > 0);
    n_assert(numVertices > 0);

    // for each pixel shader pass...
    int currentPass;
    int numPasses = 1;
    if (ps) numPasses = ps->BeginRender(ta);
    for (currentPass = 0; currentPass < numPasses; currentPass++)
    {
        // render the current pixelshader pass
        if (ps) ps->Render(currentPass);

        // render the vertex buffer fragment
        hr = dev->DrawIndexedPrimitive(d3dPType, 0, numVertices, 0, numPrimitives);
        if (FAILED(hr))
        {
            n_printf("nD3D8VertexPool: DrawIndexPrimitive() failed with '%s'\n", nd3d8_Error(hr));
        }
    }

    // finish rendering the pixel shader
    if (ps) ps->EndRender();

    // update the triangle counter
    int numTriangles = numPrimitives * numPasses;
    gs->SetStats(nGfxServer::N_GFXSTATS_TRIANGLES, numTriangles);
}

//-----------------------------------------------------------------------------
