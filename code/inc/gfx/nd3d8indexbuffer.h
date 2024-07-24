#ifndef N_D3D8INDEXBUFFER_H
#define N_D3D8INDEXBUFFER_H
//-----------------------------------------------------------------------------
/**
    @class nD3D8IndexBuffer

    @brief Encapsulate Direct3D8 index buffer object into a 
    Nebula nIndexBuffer subclass.
*/
#ifndef N_INDEXBUFFER_H
#include "gfx/nindexbuffer.h"
#endif

#ifndef N_D3D8SERVER_H
#include "gfx/nd3d8server.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nD3D8IndexBuffer
#include "kernel/ndefdllclass.h"

//-----------------------------------------------------------------------------
class nD3D8IndexBuffer : public nIndexBuffer
{
    friend class nD3D8VertexPool;

public:
    /// the constructor
    nD3D8IndexBuffer();
    /// the destructor
    virtual ~nD3D8IndexBuffer();

    /// begin writing indices into the buffer
    virtual void Begin(nIBufType ibufType, nPrimType primType, int numIndices);
    /// finish writing indices into the buffer
    virtual void End();
    /// lock the index buffer for writing (after Begin()/End()!)
    virtual bool Lock();
    /// unlock the index buffer
    virtual void Unlock();

    static nKernelServer* kernelServer;

private:
    /// get pointer to the embedded d3d8 index buffer object
    IDirect3DIndexBuffer8* GetD3D8IndexBuffer();

protected:
    nAutoRef<nD3D8Server> ref_gs;
    IDirect3DIndexBuffer8 *d3dIBuffer;
};

//-----------------------------------------------------------------------------
/**
*/
inline
IDirect3DIndexBuffer8*
nD3D8IndexBuffer::GetD3D8IndexBuffer()
{
    n_assert(this->d3dIBuffer);
    return this->d3dIBuffer;
}

//-----------------------------------------------------------------------------
#endif
