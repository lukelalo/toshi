#ifndef N_VERTEXBUFFER_H
#define N_VERTEXBUFFER_H
//--------------------------------------------------------------------
/**
    @class nVertexBuffer

    @brief shared vertex buffer object

    Encapsulates a vertex bubble into a nRoot object, keeps
    an uptodate bounding box of the vertex bubble contents.
*/
//--------------------------------------------------------------------
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_BBOX_H
#include "mathlib/bbox.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif

#ifndef N_VERTEXBUBBLE_H
#include "gfx/nvertexbubble.h"
#endif

#ifndef N_VERTEXPOOL_H
#include "gfx/nvertexpool.h"
#endif

#ifndef N_GFXSERVER_H
#include "gfx/ngfxserver.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nVertexBuffer
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nTextureArray;
class N_PUBLIC nVertexBuffer : public nRoot {

    nRef<nGfxServer> ref_gs;    // the gfx server who created us
    nVertexBubble *vbubble;     // our vertex bubble
    int lock_count;             // read-only buffers can be locked multiple times
    bbox3 bbox;                 // our bounding box

public:
    // only valid inside LockVertices()/UnlockVertices()
    float *coord_ptr;
    float *norm_ptr;
    ulong *color_ptr;
    float *uv_ptr[N_MAXNUM_TEXCOORDS];
    float *w_ptr;
    int   *j_ptr;
    int num_v;
    int stride4;

    static nKernelServer *kernelServer;

    nVertexBuffer()
        : ref_gs(this),
          vbubble(NULL),
          lock_count(0),
          coord_ptr(NULL),
          norm_ptr(NULL),
          color_ptr(NULL),
          w_ptr(NULL),
          j_ptr(NULL),
          num_v(0),
          stride4(0)
    {
        memset(uv_ptr,0,sizeof(uv_ptr));
    };
    virtual ~nVertexBuffer();

    // render the buffer through its gfx server
    void Render(nIndexBuffer *ib, nPixelShader *ps, nTextureArray *ta) {
        n_assert(this->vbubble && ib);
        nVertexPool *vp = this->vbubble->GetVertexPool();
        n_assert(vp);
        vp->RenderBubble(this->vbubble, ib, ps, ta);
    };

    // temporarily truncate the vertex buffer to a smaller size
    void Truncate(int t) {
        n_assert(this->vbubble);
        this->vbubble->Truncate(t);
    };
    void Untruncate(void) {
        this->vbubble->Untruncate();
    };

    // called by gfx server after creating us
    void Initialize(nGfxServer *gs, nVertexBubble *vb) {
        n_assert(gs);
        n_assert(vb);
        this->ref_gs  = gs;
        this->vbubble = vb;
    };

    // access to vertex buffer and index list
    bool LockVertices(void);
    void UnlockVertices(void);
    
    void Coord(int i, const vector3& v) {
        n_assert((this->lock_count>0) && (i<this->num_v) && this->coord_ptr);
        float *p = this->coord_ptr + i*stride4;
        p[0]=v.x; p[1]=v.y; p[2]=v.z;
    };
    void Norm(int i, const vector3& v) {
        n_assert((this->lock_count>0) && (i<this->num_v) && this->norm_ptr);
        float *p = this->norm_ptr + i*stride4;
        p[0]=v.x; p[1]=v.y; p[2]=v.z;
    };
    void Color(int i, ulong c) {
        n_assert((this->lock_count>0) && (i<this->num_v) && this->color_ptr);
        ulong *p = this->color_ptr + i*stride4;
        p[0] = c;
    };
    void Uv(int i, int set, const vector2& v) {
        n_assert(set < N_MAXNUM_TEXCOORDS);
        n_assert((this->lock_count>0) && (i<this->num_v) && this->uv_ptr[set]);
        float *p = this->uv_ptr[set] + i*stride4;
        p[0]=v.x; p[1]=v.y;
    };
    void JointWeight1(int i, int j0, float w0) {
        n_assert((this->lock_count>0) && (i<this->num_v) && this->j_ptr && this->w_ptr);
        n_assert(this->GetVertexType() && N_VT_JW);

        // normalize weight sum
        w0 = 1.0f;

        int *jp   = this->j_ptr + i*stride4;
        float *wp = this->w_ptr + i*stride4;
        jp[0]=j0; wp[0]=w0;
        jp[1]=0;  wp[1]=0.0f;
        jp[2]=0;  wp[2]=0.0f;
        jp[3]=0;  wp[3]=0.0f;
    };
    void JointWeight2(int i, int j0, float w0, int j1, float w1) {
        n_assert((this->lock_count>0) && (i<this->num_v) && this->j_ptr && this->w_ptr);
        n_assert(this->GetVertexType() && N_VT_JW);

        // normalize weight sum
        float sum = w0 + w1;
        n_assert(sum > 0.01f);
        float oneDivSum = 1.0f / sum;
        w0 *= oneDivSum;
        w1 *= oneDivSum;

        int *jp   = this->j_ptr + i*stride4;
        float *wp = this->w_ptr + i*stride4;
        jp[0]=j0; wp[0]=w0;
        jp[1]=j1; wp[1]=w1;
        jp[2]=0;  wp[2]=0.0f;
        jp[3]=0;  wp[3]=0.0f;
    };
    void JointWeight3(int i, int j0, float w0, int j1, float w1, int j2, float w2) {
        n_assert((this->lock_count>0) && (i<this->num_v) && this->j_ptr && this->w_ptr);
        n_assert(this->GetVertexType() && N_VT_JW);

        // normalize weight sum
        float sum = w0 + w1 + w2;
        n_assert(sum > 0.01f);
        float oneDivSum = 1.0f / sum;
        w0 *= oneDivSum;
        w1 *= oneDivSum;
        w2 *= oneDivSum;

        int *jp   = this->j_ptr + i*stride4;
        float *wp = this->w_ptr + i*stride4;
        jp[0]=j0; wp[0]=w0;
        jp[1]=j1; wp[1]=w1;
        jp[2]=j2; wp[2]=w2;
        jp[3]=0;  wp[3]=0.0f;
    };
    void JointWeight4(int i, int j0, float w0, int j1, float w1, int j2, float w2, int j3, float w3) {
        n_assert((this->lock_count>0) && (i<this->num_v) && this->j_ptr && this->w_ptr);
        n_assert(this->GetVertexType() && N_VT_JW);

        // normalize weight sum
        float sum = w0 + w1 + w2 + w3;
        n_assert(sum > 0.01f);
        float oneDivSum = 1.0f / sum;
        w0 *= oneDivSum;
        w1 *= oneDivSum;
        w2 *= oneDivSum;
        w3 *= oneDivSum;

        int *jp   = this->j_ptr + i*stride4;
        float *wp = this->w_ptr + i*stride4;
        jp[0]=j0; wp[0]=w0;
        jp[1]=j1; wp[1]=w1;
        jp[2]=j2; wp[2]=w2;
        jp[3]=j3; wp[3]=w3;
    };

    // bounding box and misc stuff...
    void SetBBox(const bbox3& bb) {
        this->bbox = bb;
    };
    const bbox3& GetBBox(void) {
        return this->bbox;
    };

    // get information
    int GetVertexType(void) {
        n_assert(this->vbubble);
        return this->vbubble->GetVertexPool()->GetVertexType();
    };
    int GetNumVertices(void) {
        n_assert(this->vbubble);
        return this->vbubble->GetBubbleSize();
    };
    int GetVBufType(void) {
        n_assert(this->vbubble);
        return this->vbubble->GetVertexPool()->GetVBufType();
    };
    nColorFormat GetColorFormat(void) {
        return this->ref_gs->GetColorFormat();
    };

    // access to internal objects
    nVertexBubble *GetVertexBubble(void) {
        return this->vbubble;
    };
};
//--------------------------------------------------------------------
#endif
