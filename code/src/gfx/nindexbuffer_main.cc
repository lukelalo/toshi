#define N_IMPLEMENTS nIndexBuffer
//-----------------------------------------------------------------------------
//  nindexbuffer_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nindexbuffer.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nIndexBuffer, "nroot");

//-----------------------------------------------------------------------------
/**
*/
nIndexBuffer::~nIndexBuffer()
{
    n_assert(!this->in_begin);
    n_assert(!this->is_locked);
    if (this->alloc_array) 
    {
        n_free(this->alloc_array);
        this->alloc_array = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Start filling the nIndexBuffer object with indices. This will
    actually create the index array and reset the internal
    write position. Fill the array by calling Index() multiple times,
    then call nIndexBuffer::End() to finish. Subclasses may choose
    to overwrite this method to their needs.

    @param  primType    the primitive type (of enum nPrimType)
    @param  ibufType    index buffer type, indicating useage pattern
    @param  numIndices  the number of indices
*/
void
nIndexBuffer::Begin(nIBufType ibufType, nPrimType primType, int numIndices)
{
    n_assert(!this->in_begin);
    n_assert(!this->alloc_array);
    n_assert(!this->is_locked);

    this->pt            = primType;
    this->ibuf_type     = ibufType;
    this->i_num         = numIndices;
    this->i_trunc       = numIndices;
    this->alloc_array   = (ushort *) n_malloc(numIndices * sizeof(ushort));
    this->i_array       = this->alloc_array;
    this->in_begin      = true;
}

//-----------------------------------------------------------------------------
/**
    Finish filling the nIndexBuffer object with indices. All indices
    must have been initialized, otherwise an assertion will be
    thrown.
*/
void 
nIndexBuffer::End(void) 
{
    n_assert(this->in_begin);

    this->in_begin = false;
    this->i_array  = 0;
}

//-----------------------------------------------------------------------------
/**
    Locks the index buffer for writing new indices to it. This operation
    can only be called after the index buffer has actually been initialized
    by calling Begin()/End().
    
    @return     true if lock was successful and calls to Index() are allowed
*/
bool
nIndexBuffer::Lock()
{
    n_assert((!this->is_locked) && (!this->in_begin));
    this->is_locked = true;
    this->i_array   = this->alloc_array;
    return true;
}

//-----------------------------------------------------------------------------
/**
    Unlocks the index buffer after a successful call to Lock().
*/
void
nIndexBuffer::Unlock()
{
    n_assert(this->is_locked);
    this->is_locked = false;
    this->i_array   = 0;
}

//-----------------------------------------------------------------------------
