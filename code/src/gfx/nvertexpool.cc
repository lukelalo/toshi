#define N_IMPLEMENTS nGfxServer
//-------------------------------------------------------------------
//  nvertexpool.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nvertexpool.h"
#include "gfx/nvertexbubble.h"
#include "gfx/ngfxserver.h"

//-------------------------------------------------------------------
//  ~nVertexPool()
//  31-Aug-00   floh    created
//-------------------------------------------------------------------
nVertexPool::~nVertexPool()
{
    // free all owned vertex bubbles
    nVertexBubble *vb;
    while ((vb = (nVertexBubble *) this->free_bubbles.RemHead())) n_delete vb;
    while ((vb = (nVertexBubble *) this->full_bubbles.RemHead())) n_delete vb;

    // free vertex pool pointer
    if (this->pool) {
        n_free(this->pool);
        this->pool = NULL;
    }
}

//-------------------------------------------------------------------
/**
    Initialize the vertex pool. Most likely overwritten by
    3D-API-specific subclasses. Vertex type, vertex format 
    and number of vertices in pool must already be initialized.

    History:
     - 31-Aug-00   floh    created
     - 21-Oct-00   floh    + weight and joint-index vertex components
*/
//-------------------------------------------------------------------
bool nVertexPool::Initialize(void)
{
    n_assert(this->vtype != 0);
    n_assert(this->vnum > 0);
    n_assert(NULL == this->pool);

    // compute the required pool size in bytes
    int vertex_size = 0;
    if (this->vtype & N_VT_COORD) vertex_size += 3*sizeof(float);
    if (this->vtype & N_VT_NORM)  vertex_size += 3*sizeof(float);
    if (this->vtype & N_VT_RGBA)  vertex_size += sizeof(ulong);
    if (this->vtype & N_VT_UV0)   vertex_size += 2*sizeof(float);
    if (this->vtype & N_VT_UV1)   vertex_size += 2*sizeof(float);
    if (this->vtype & N_VT_UV2)   vertex_size += 2*sizeof(float);
    if (this->vtype & N_VT_UV3)   vertex_size += 2*sizeof(float);
    if (this->vtype & N_VT_JW)    vertex_size += 4*sizeof(float) + 4*sizeof(int);

    // allocate vertex pool
    this->pool = n_malloc(vertex_size * this->vnum);
    this->stride4 = vertex_size>>2;

    // create the initial 'free' bubble which represents the
    // whole vertex pool
    nVertexBubble *vb = n_new nVertexBubble(this,0,this->vnum);
    this->free_bubbles.AddTail(vb);
    return true;
}

//-------------------------------------------------------------------
/**
    Create a new vertex bubble object which represents a chunk
    of continous vertices in the vertex pool. Returns NULL
    if no bubble of the requested size can be allocated.
    The minimum bubble size is 16 to prevent excessive
    fragmentation.

    @param size             size of the bubble required

    History:
     - 31-Aug-00   floh    created
*/
//-------------------------------------------------------------------
nVertexBubble *nVertexPool::NewBubble(int size)
{
    // find the smallest free bubble which can hold the required size
    nVertexBubble *best_bubble = NULL;
    nVertexBubble *bubble;
    int min_diff = 1000000;
    for (bubble = (nVertexBubble *) this->free_bubbles.GetHead();
         bubble;
         bubble = (nVertexBubble *) bubble->GetSucc())
    {
        int d = bubble->GetBubbleSize() - size;
        if (d >= 0) {
            if (d<min_diff) {
                best_bubble = bubble;
                min_diff = d;
            }
        }
    }
    
    // no bubbles available in this pool...
    if (!best_bubble) return NULL;

    // There is a good bubble, see if we should split it.
    // Splitting happens if the candidate bubble has
    // at least 16 vertices more then the required size
    if ((best_bubble->GetBubbleSize()-size) >= 16) {
        
        // split it, baby!
        int bb_start = best_bubble->GetStart();
        int bb_size  = best_bubble->GetBubbleSize();

        // shrink the old bubble...
        best_bubble->Set(bb_start+size,bb_size-size);

        // ...and create a new one
        nVertexBubble *new_vb = n_new nVertexBubble(this,bb_start,size);
        this->full_bubbles.AddTail(new_vb);

        return new_vb;
    
    } else {

        // no bubble splitting, move the good bubble from
        // the free bubble pool into the used bubble pool
        // and return it
        best_bubble->Remove();
        this->full_bubbles.AddTail(best_bubble);
        return best_bubble;
    }
}
    
//-------------------------------------------------------------------
/**
    Release a bubble into the free bubble pool.

     - 31-Aug-00   floh    created
*/
//-------------------------------------------------------------------
void nVertexPool::ReleaseBubble(nVertexBubble *vb)
{
    n_assert(vb);
    n_assert(vb->GetVertexPool() == this);

    // if this is the global 'current bubble', set it to NULL
    if (vb == this->ref_gs->GetCurrentVertexBubble()) {
        this->ref_gs->SetCurrentVertexBubble(NULL);
    }

    // move from used bubble pool into free bubble pool,
    // keep the list sorted so that neighboring bubbles
    // can be merged into a bigger bubble if possible
    vb->Remove();
    nVertexBubble *succ_bubble;
    for (succ_bubble = (nVertexBubble *) this->free_bubbles.GetHead();
         succ_bubble;
         succ_bubble = (nVertexBubble *) succ_bubble->GetSucc())
    {
        if (succ_bubble->GetStart() > vb->GetStart()) break;
    }
    if (succ_bubble) vb->InsertBefore(succ_bubble);
    else             this->free_bubbles.AddTail(vb);

    // see if the bubble can be merged with its neighbors
    nVertexBubble *pred = (nVertexBubble *) vb->GetPred();
    nVertexBubble *succ = (nVertexBubble *) vb->GetSucc();
    if (pred) {
        int pred_start = pred->GetStart();
        int pred_size  = pred->GetBubbleSize();
        if ((pred_start+pred_size) == vb->GetStart()) {
            // merge with previous node
            vb->Set(pred_start,pred_size+vb->GetBubbleSize());
            pred->Remove();
            n_delete pred;
            pred = NULL;
        }
    }
    if (succ) {
        int vb_start = vb->GetStart();
        int vb_size  = vb->GetBubbleSize();
        if ((vb_start+vb_size) == succ->GetStart()) {
            // merge with next node
            vb->Set(vb_start,vb_size+succ->GetBubbleSize());
            succ->Remove();
            n_delete succ;
            succ = NULL;
        }
    }
}

//-------------------------------------------------------------------
/**
    Lock the vertex bubble for read/write access to the vertex 
    data and return a pointer to the 1st vertex
    and the vertex stride in 4-byte-units (for painless float and ulong
    pointer arithmetic).

    History:
     - 31-Aug-00   floh    created
     - 21-Oct-00   floh    + joint and weight vertex components
*/
//-------------------------------------------------------------------
bool nVertexPool::LockBubble(nVertexBubble *vb)
{
    n_assert(vb);
    n_assert(vb->GetVertexPool() == this);
    
    // get the base pointer to the vertex data
    float *base_ptr = ((float *)this->pool) + vb->GetStart()*this->stride4;
    
    // get the pointers to the vertex components
    if (this->vtype & N_VT_COORD) {
        this->coord_ptr = base_ptr;
        base_ptr += 3;
    } else this->coord_ptr = NULL;

    // normals pointer
    if (this->vtype & N_VT_NORM) {
        this->norm_ptr = base_ptr;
        base_ptr += 3;
    } else this->norm_ptr = NULL;

    // color pointer
    if (this->vtype & N_VT_RGBA) {
        this->color_ptr = (ulong *) base_ptr;
        base_ptr += 1;
    } else this->color_ptr = NULL;

    // uv pointers
    int i;
    int m = N_VT_UV0;
    for (i=0; i<N_MAXNUM_TEXCOORDS; i++, m<<=1) {
        if (this->vtype & m) {
            this->uv_ptrs[i] = base_ptr;
            base_ptr += 2;
        } else {
            this->uv_ptrs[i] = NULL;
        }
    }
    
    // weight pointer
    if (this->vtype & N_VT_JW) {
        this->w_ptr = base_ptr;
        base_ptr += 4;
        this->j_ptr = (int *) base_ptr;
        base_ptr += 4;
    }

    return true;
}

//-------------------------------------------------------------------
/**
    Unlock the currently locked bubble. 

     - 31-Aug-00   floh    created
*/
//-------------------------------------------------------------------
void nVertexPool::UnlockBubble(nVertexBubble *vb)
{
    n_assert(vb);
    n_assert(vb->GetVertexPool() == this);
    
    this->coord_ptr = NULL;
    this->norm_ptr  = NULL;
    this->color_ptr = NULL;
    this->w_ptr     = NULL;
    this->j_ptr     = NULL; 
    memset(uv_ptrs,0,sizeof(uv_ptrs));
}

//-------------------------------------------------------------------
/**
    Rendering a vertex bubble must be done by code specific
    to the 3D API.  This must be overridden by a subclass.

    History:
     - 31-Aug-00   floh    created
*/
//-------------------------------------------------------------------
void nVertexPool::RenderBubble(nVertexBubble *, nIndexBuffer *, nPixelShader *, nTextureArray *)
{
    // nVertexPool() leaves vertex bubble rendering to
    // subclasses, which are written to a specific 3d api
    n_printf("nVertexPool::RenderBubble() called.\n");
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
