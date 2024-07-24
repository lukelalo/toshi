#ifndef N_INDEXBUFFER_H
#define N_INDEXBUFFER_H
//-----------------------------------------------------------------------------
/**
    @class nIndexBuffer

    @brief hold indices for index primitive rendering

    nIndexBuffer objects encapsulate vertex indices for exactly
    one indexed primitive (usually a N_PTRYPE_TRIANGLE_LIST). 
    3d api glue code may choose to subclass nIndexBuffer and
    optimize the implementation for their specific needs.
*/

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#undef N_DEFINES
#define N_DEFINES nIndexBuffer
#include "kernel/ndefdllclass.h"

//-----------------------------------------------------------------------------
class N_PUBLIC nIndexBuffer : public nRoot 
{
public:
    /// the constructor
    nIndexBuffer();
    /// the destructor
    virtual ~nIndexBuffer();
    
    /// begin writing indices into the buffer
    virtual void Begin(nIBufType ibufType, nPrimType primType, int numIndices);
    /// write an index
    void Index(int i, ushort index);
    /// finish writing indices into the buffer
    virtual void End();
    /// lock the index buffer for writing (after Begin()/End()!)
    virtual bool Lock();
    /// unlock the index buffer
    virtual void Unlock();

    /// temporarly truncate index array (useful for dynamic geometry)
    void Truncate(int trunc);
    /// restore original size of index array
    void Untruncate();

    /// return the contained primitive type
    nPrimType GetPrimType();
    /// set the contained primitive type
    void SetPrimType(nPrimType);
    /// return the index buffer type
    nIBufType GetIndexBufferType();
    /// return number of indices in object
    int GetNumIndices();
    /// get pointer to index array
    ushort *GetIndexPointer();

    static nKernelServer *kernelServer;

protected:
    nPrimType pt;           // the primitive type
    nIBufType ibuf_type;    // the index buffer type (static or dynamic)
    int i_num;              // the number of indices
    int i_trunc;            // number of indices if temporarly truncated
    ushort *alloc_array;    // valid after Begin()
    ushort *i_array;        // valid only inside Begin()/End(), Lock()/Unlock()
    bool in_begin;
    bool is_locked;
};

//-----------------------------------------------------------------------------
/**
*/
inline
nIndexBuffer::nIndexBuffer() : 
    pt(N_PTYPE_TRIANGLE_LIST),
    ibuf_type(N_IBTYPE_STATIC),
    i_num(0),
    i_trunc(0),
    i_array(0),
    alloc_array(0),
    in_begin(false),
    is_locked(false)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Write a vertex index at the given position into the index array.
    Calls to Index() are only allowed inside Begin()/End() and Lock()/Unlock()

    @param  i               write position (index into array of vertex indices)
    @param  vertexIndex     the actual vertex index

*/
inline
void
nIndexBuffer::Index(int i, ushort vertexIndex)
{
    n_assert(this->in_begin || this->is_locked);
    n_assert((i >= 0) && (i < this->i_num));
    this->i_array[i] = vertexIndex;    
}

//-----------------------------------------------------------------------------
/**
    Truncates the index array to a smaller size. This is useful when
    rendering through dynamic vertex buffers, where the vertex buffer
    is not completely filled when rendered. After rendering, the
    index buffer must be restored to its original size with 
    Untruncate().
    
    @param  trunc   the truncated number of indices
*/
inline
void
nIndexBuffer::Truncate(int trunc)
{
    n_assert(trunc <= this->i_num);
    this->i_trunc = trunc;
}

//-----------------------------------------------------------------------------
/**
    Sets the number of indices in the nIndexBuffer object to its
    original value, undoing Truncate().
*/
inline
void
nIndexBuffer::Untruncate()
{
    this->i_trunc = this->i_num;
}

//-----------------------------------------------------------------------------
/**
*/
inline
nPrimType
nIndexBuffer::GetPrimType()
{
    return this->pt;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nIndexBuffer::SetPrimType(nPrimType ptype)
{
    this->pt = ptype;
}

//-----------------------------------------------------------------------------
/**
*/
inline
nIBufType
nIndexBuffer::GetIndexBufferType()
{
    return this->ibuf_type;
}
//-----------------------------------------------------------------------------
/**
*/
inline
int
nIndexBuffer::GetNumIndices()
{
    return this->i_trunc;
}

//-----------------------------------------------------------------------------
/**
*/
inline
ushort*
nIndexBuffer::GetIndexPointer()
{
    return this->alloc_array;
}

//-----------------------------------------------------------------------------
#endif

