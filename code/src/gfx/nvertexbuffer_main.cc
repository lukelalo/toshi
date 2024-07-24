#define N_IMPLEMENTS nVertexBuffer
//-------------------------------------------------------------------
//  nvertexbuffer_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nvertexbuffer.h"
#include "gfx/nvertexpool.h"
#include "gfx/nvertexbubble.h"
#include "gfx/nvertexpoolmanager.h"

nNebulaClass(nVertexBuffer, "nroot");

//-------------------------------------------------------------------
//  ~nVertexBuffer()
//  01-Sep-00   floh    created
//-------------------------------------------------------------------
nVertexBuffer::~nVertexBuffer()
{
    n_assert(0 == this->lock_count);

    // releasing the vertex bubble is a bit tricky, since
    // we first have to find the responsible vertex pool,
    // and then the vertex pool manager which is responsible
    // for the vertex pool...
    if (this->vbubble) {
        nVertexPool *vp = this->vbubble->GetVertexPool();
        n_assert(vp);
        nVertexPoolManager *vpm = vp->GetVertexPoolManager();
        n_assert(vpm);
        vpm->ReleaseVertexBubble(this->vbubble);
        this->vbubble = NULL;
    }
}

//-------------------------------------------------------------------
/**
    Lock the vertex bubble and validate public pointers.

    History:
     - 01-Sep-00   floh    created
     - 16-Oct-00   floh    readonly buffers can now be locked multiple
                           times, non-readonly buffer are protected from
                           being locked
     - 23-Oct-00   floh    + JointIndices, Weights
*/
//-------------------------------------------------------------------
bool nVertexBuffer::LockVertices(void)
{
    n_assert(this->vbubble);

    // read only vertex buffers can be locked multiple times
    if ((this->lock_count>0) && 
        (N_VBTYPE_READONLY != this->vbubble->GetVertexPool()->GetVBufType())) 
    {
        char buf[N_MAXPATH];
        n_error("Trying to lock non-readonly vbuffer '%s' multiple times!\n",
                 this->GetFullName(buf,sizeof(buf)));
    }

    nVertexPool *vp = this->vbubble->GetVertexPool();
    n_assert(vp);
    
    if (vp->LockBubble(this->vbubble)) {
        this->coord_ptr = vp->GetCoordPtr();
        this->norm_ptr  = vp->GetNormPtr();
        this->color_ptr = vp->GetColorPtr();
        int i;
        for (i=0; i<N_MAXNUM_TEXCOORDS; i++) {
            this->uv_ptr[i] = vp->GetUvPtr(i);
        };
        this->j_ptr   = vp->GetJointIndexPtr();
        this->w_ptr   = vp->GetWeightPtr();
        this->stride4 = vp->GetStride4();
        this->num_v   = this->vbubble->GetBubbleSize();
        this->lock_count++;
        return true;
    } else {
        return false;
    }
}

//-------------------------------------------------------------------
/**
    Unlock the vertex bubble and invalidate public array pointers.

    History:
     - 01-Sep-00   floh    created
*/
//-------------------------------------------------------------------
void nVertexBuffer::UnlockVertices(void)
{
    n_assert(this->vbubble);
    n_assert(this->lock_count > 0);

    nVertexPool *vp = this->vbubble->GetVertexPool();
    n_assert(vp);
    vp->UnlockBubble(this->vbubble);

    this->lock_count--;

    this->coord_ptr = NULL;
    this->norm_ptr  = NULL;
    this->color_ptr = NULL;
    this->j_ptr     = NULL;
    this->w_ptr     = NULL;
    memset(this->uv_ptr,0,sizeof(this->uv_ptr));
    this->stride4 = 0;
    this->num_v   = 0;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
