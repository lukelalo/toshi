#define N_IMPLEMENTS nD3D8IndexBuffer
//-----------------------------------------------------------------------------
//  nd3d8indexbuffer_main.cc
//  (C) 2001 A.Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nd3d8indexbuffer.h"

nNebulaClass(nD3D8IndexBuffer, "nindexbuffer");

extern const char *nd3d8_Error(HRESULT hr);

//-----------------------------------------------------------------------------
/**
*/
nD3D8IndexBuffer::nD3D8IndexBuffer()
:   ref_gs(kernelServer,this),
    d3dIBuffer(0)
{
    this->ref_gs = "/sys/servers/gfx";
}

//-----------------------------------------------------------------------------
/**
    Releases the D3D index buffer object.
*/
nD3D8IndexBuffer::~nD3D8IndexBuffer()
{
    n_assert(!this->in_begin);
    if (this->d3dIBuffer)
    {
        this->d3dIBuffer->Release();
        this->d3dIBuffer = NULL;
    }
}

//-----------------------------------------------------------------------------
/**
    Begin writing indices. Will create a managed d3d8 index buffer object 
    and lock it, so that nIndexBuffer::Index() will work. This method
    can only be called once on the object.

    @param  ibufType    the index buffer type
    @param  primType    the primitive type (only one primitive per index buffer)
    @param  numIndices  number of indices to store in the buffer

    11-Jul-01   floh    force static index and vertex buffers into system mem, to save
                        video mem for textures
*/
void
nD3D8IndexBuffer::Begin(nIBufType ibufType, nPrimType primType, int numIndices)
{
    n_assert(!this->in_begin);
    n_assert(!this->d3dIBuffer);
    n_assert(this->ref_gs->d3d8Dev);

    // initialize instance data
    this->pt       = primType;
    this->i_num    = numIndices;
    this->i_trunc  = numIndices;
    this->in_begin = true;

    // get useage indicator based on ibufType
    DWORD d3d_usage;
    D3DPOOL d3d_pool;
    if (N_IBTYPE_STATIC == ibufType)
    {
        d3d_usage = D3DUSAGE_WRITEONLY;
//        d3d_pool  = D3DPOOL_SYSTEMMEM;
        d3d_pool  = D3DPOOL_MANAGED;
    }
    else
    {
        d3d_usage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
        d3d_pool  = D3DPOOL_SYSTEMMEM;
    }

    // first, create the d3d index buffer object
    HRESULT hr;
    UINT byteSize = numIndices * sizeof(ushort);
    hr = this->ref_gs->d3d8Dev->CreateIndexBuffer(
        byteSize,                                   // length (in bytes)
        d3d_usage,                                  // usage indicator
        D3DFMT_INDEX16,                             // format (== ushort)
        d3d_pool,                                   // memory pool identifier
        &(this->d3dIBuffer));                       // ppIndexBuffer
    if (FAILED(hr))
    {
        n_error("nD3D8IndexBuffer: CreateIndexBuffer() failed with '%s'\n", nd3d8_Error(hr));
        return;
    }
    n_assert(this->d3dIBuffer);

    // lock the d3d index buffer, and write the pointer to the
    // beginning of the index array to the place where
    // nIndexBuffer::Index() expects it!
    hr = this->d3dIBuffer->Lock(0, byteSize, (BYTE**) &(this->i_array), D3DLOCK_NOSYSLOCK);
    if (FAILED(hr))
    {
        n_printf("nD3D8IndexBuffer: Lock() failed with '%s'\n", nd3d8_Error(hr));
        this->d3dIBuffer->Release();
        this->d3dIBuffer = 0;
        return;
    }

}

//-----------------------------------------------------------------------------
/**
    Finish writing the indices. The internal d3d8 index buffer object
    will be unlocked.
*/
void
nD3D8IndexBuffer::End()
{
    n_assert(this->in_begin);
    n_assert(this->d3dIBuffer);
    
    this->i_array  = 0;
    this->in_begin = false;

    HRESULT hr;
    hr = this->d3dIBuffer->Unlock();
    if (FAILED(hr))
    {
        n_printf("nD3D8IndexBuffer: Unlock() failed with '%s'\n", nd3d8_Error(hr));
        return;
    }
}

//-----------------------------------------------------------------------------
/**
    Lock d3d8 index buffer so that new indices can be written to the 
    buffer.
*/
bool 
nD3D8IndexBuffer::Lock()
{
    n_assert((!this->is_locked) && (!this->in_begin));
    n_assert(this->d3dIBuffer);

    HRESULT hr;
    UINT byteSize = this->i_num * sizeof(ushort);
    hr = this->d3dIBuffer->Lock(0, byteSize, (BYTE**) &(this->i_array), D3DLOCK_NOSYSLOCK);
    if (FAILED(hr))
    {
        n_printf("nD3D8IndexBuffer: Lock() failed with '%s'\n", nd3d8_Error(hr));
        return false;
    }
    this->is_locked = true;
    return true;
}

//-----------------------------------------------------------------------------
/**
    Unlocks the d3d8 index buffer.
*/
void
nD3D8IndexBuffer::Unlock()
{
    n_assert(this->is_locked);
    n_assert(this->d3dIBuffer);

    HRESULT hr;
    hr = this->d3dIBuffer->Unlock();
    if (FAILED(hr))
    {
        n_printf("nD3D8IndexBuffer: Unlock() failed with '%s'\n", nd3d8_Error(hr));
        return;
    }
    this->is_locked = false;
    this->i_array   = 0;
}

//-----------------------------------------------------------------------------
