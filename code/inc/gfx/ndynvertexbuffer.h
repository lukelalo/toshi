#ifndef N_DYNVERTEXBUFFER_H
#define N_DYNVERTEXBUFFER_H
//-------------------------------------------------------------------
/**
    @class nDynVertexBuffer

    @brief simple wrapper for dynamic meshes
*/
//-------------------------------------------------------------------
#ifndef N_VERTEXBUFFER_H
#include "gfx/nvertexbuffer.h"
#endif

#ifndef N_INDEXBUFFER_H
#include "gfx/nindexbuffer.h"
#endif

#ifndef N_TEXTUREARRAY_H
#include "gfx/ntexturearray.h"
#endif

#ifndef N_GFXSERVER_H
#include "gfx/ngfxserver.h"
#endif

//-------------------------------------------------------------------
class nDynVertexBuffer {

    nAutoRef<nGfxServer> ref_gs;
    nRef<nVertexBuffer> ref_vb;

private:
    nVertexBuffer *vb;
    nIndexBuffer  *ib;
    nPixelShader  *ps;
    nTextureArray *ta;
    bool in_begin;
    bool read_only;
    int vertex_type;            // values from Initialize()
    int num_vertices;

public:
    nDynVertexBuffer(nKernelServer *ks, nRoot *o)
        : ref_gs(ks,o),
          ref_vb(o),
          vb(NULL),
          ib(NULL),
          ps(NULL),
          ta(NULL),
          in_begin(false),
          read_only(false)
    {
        this->ref_gs = "/sys/servers/gfx";
    };

    ~nDynVertexBuffer() {
        if (this->ref_vb.isvalid()) {
            this->ref_vb->Release();
            this->ref_vb.invalidate();
        }
    };

    // read-only vertex buffers can't render themselves,
    // instead they provide target vertex buffers in sysmem
    // which can be read-out relatively fast
    void SetReadOnly(bool b) {
        read_only = b;
    };
    bool GetReadOnly(void) {
        return read_only;
    };

    int GetNumVertices(void) {
        return this->ref_vb->GetNumVertices();
    };
    nColorFormat GetColorFormat(void) {
        return this->ref_vb->GetColorFormat();
    };

    //---------------------------------------------------------------
    //  IsValid()
    //  Return true the nDynVertexBuffer is valid and can be
    //  used. Otherwise the Initialize() method should be called.
    //  17-Oct-00   floh    created
    //---------------------------------------------------------------
    bool IsValid(void) {
        return this->ref_vb.isvalid();
    };

    //---------------------------------------------------------------
    //  Initialize()
    //  Initialize internal vertex buffer, call once after creating
    //  the object.
    //  17-Oct-00   floh    created
    //---------------------------------------------------------------
    void Initialize(int vtype, int size)
    {
        // a size of '0' means "I don't care since I'm filling
        // the vertex buffer until its full, and then Swap()"
        n_assert(!this->ref_vb.isvalid());

        this->vertex_type  = vtype;
        this->num_vertices = size;

        if (this->read_only) {
            // in the read only case, a private vertex buffer is
            // created which is optimized for read/write
            n_assert(size > 0);
            this->ref_vb = this->ref_gs->NewVertexBuffer(NULL, N_VBTYPE_READONLY, vtype, size);
            n_assert(this->ref_vb.isvalid());
            
        } else {

            // if the node is a render target, there is no read access
            // to the target vertex buffers

            // get the shared resource identifiers for the vertex buffer
            char vb_name[N_MAXNAMELEN];
            strcpy(vb_name,"dm_");
            if (vtype & N_VT_COORD) strcat(vb_name,"c"); 
            if (vtype & N_VT_NORM)  strcat(vb_name,"n"); 
            if (vtype & N_VT_RGBA)  strcat(vb_name,"r"); 
            if (vtype & N_VT_UV0)   strcat(vb_name,"u0"); 
            if (vtype & N_VT_UV1)   strcat(vb_name,"u1");
            if (vtype & N_VT_UV2)   strcat(vb_name,"u2");
            if (vtype & N_VT_UV3)   strcat(vb_name,"u3");

            // get the vertex buffer
            nVertexBuffer *vb;
            vb = this->ref_gs->FindVertexBuffer(vb_name);
            if (!vb) vb = this->ref_gs->NewVertexBuffer(vb_name,N_VBTYPE_WRITEONLY,vtype,0);
            this->ref_vb = vb;
            n_assert(this->ref_vb.isvalid());
        }
    }

    //---------------------------------------------------------------
    //  Begin()
    //  Call at begin of rendering, returns locked target vertex 
    //  buffer which can be filled. When target vertex buffer is
    //  full, call Swap() to flush the buffer and return the same
    //  locked target buffer (rely on 3D API to do clever double
    //  buffering stuff...
    //  Call End() if finished with everything.
    //  17-Oct-00   floh    created
    //---------------------------------------------------------------
    nVertexBuffer *Begin(nIndexBuffer *ibuf, nPixelShader *pshader, nTextureArray *tarray) {
        n_assert(!this->in_begin);
        n_assert(ibuf);

        // check if vertex buffers have become invalid (this may
        // happen if switching gfx servers) in this case we
        // need to reinitialize...
        if (!this->ref_vb.isvalid()) {
            this->Initialize(this->vertex_type,this->num_vertices);
        }

        // initialize target vbuffer
        this->vb = this->ref_vb.get();

        // initialize other pointers
        this->ib = ibuf;
        this->ps = pshader;
        this->ta = tarray;

        // lock vertex buffer
        this->vb->LockVertices();

        // return...
        this->in_begin = true;
        return this->vb;
    };

    //---------------------------------------------------------------
    //  Swap()
    //  Call during rendering if vertex buffer is full.
    //  Flushes the current vertex buffer and returns a new locked 
    //  vertex buffer.
    //  17-Oct-00   floh    created
    //  26-Nov-00   floh    don't render if num_vertices or 
    //                      num_indices is 0
    //---------------------------------------------------------------
    nVertexBuffer *Swap(int num_vertices, int num_indices) {
        n_assert(this->in_begin);
        n_assert((num_vertices >= 0) && (num_indices >= 0));

        // first, unlock vertices...
        this->vb->UnlockVertices();

        if (this->read_only) {
            // the read-only case is simply: nothing happens
            return this->vb;
        } else {

            // only render if there is something to render
            if ((num_vertices > 0) && (num_indices > 0)) {
                this->vb->Truncate(num_vertices);
                this->ib->Truncate(num_indices);
                this->vb->Render(this->ib,this->ps,this->ta);
                this->ib->Untruncate();
                this->vb->Untruncate();
            }

            // lock vertex buffer and return
            this->vb->LockVertices();
            return this->vb;
        }
    };

    //---------------------------------------------------------------
    //  Render()
    //  Render the current contents of the vertex buffer 
    //  with an alternative shader.
    //  04-Aug-01   floh    created
    //---------------------------------------------------------------
    void Render(int num_vertices, int num_indices, nPixelShader* pixelShader, nTextureArray* texArray)
    {
        n_assert(this->in_begin);
        n_assert((num_vertices >= 0) && (num_indices >= 0));

        // first, unlock vertices...
        this->vb->UnlockVertices();

        if (!this->read_only) {
            // only render if there is something to render
            if ((num_vertices > 0) && (num_indices > 0)) {
                this->vb->Truncate(num_vertices);
                this->ib->Truncate(num_indices);
                this->vb->Render(this->ib, pixelShader, texArray);
                this->ib->Untruncate();
                this->vb->Untruncate();
            }

            // lock vertex buffer and return
            this->vb->LockVertices();
        }
    }

    //---------------------------------------------------------------
    //  End()
    //  Call when finished with everything. The current vertex buffer
    //  is unlocked and flushed, and a pointer to it is returned.
    //  17-Oct-00   floh    created
    //  26-Nov-00   floh    don't render if num_vertices or 
    //                      num_indices is 0
    //---------------------------------------------------------------
    nVertexBuffer *End(int num_vertices, int num_indices)
    {
        n_assert(this->in_begin);
        n_assert((num_vertices >= 0) && (num_indices >= 0));

        // first, unlock vertices
        this->vb->UnlockVertices();

        if (this->read_only) {
            // this is a no-render node, simply hand the result
            // up whomever wants it
        } else {
            // only render if there is something to render
            if ((num_vertices > 0) && (num_indices > 0)) {
                this->vb->Truncate(num_vertices);
                this->ib->Truncate(num_indices);
                this->vb->Render(this->ib,this->ps,this->ta);
                this->ib->Untruncate();
                this->vb->Untruncate();
            }
        }
        this->ib = NULL;
        this->ps = NULL;
        this->ta = NULL;
        this->in_begin = false;
        return this->vb;
    };
};
//-------------------------------------------------------------------
#endif
