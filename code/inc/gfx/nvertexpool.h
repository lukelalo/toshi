#ifndef N_VERTEXPOOL_H
#define N_VERTEXPOOL_H
//--------------------------------------------------------------------
/**
    @class nVertexPool

    @brief a pool for vertices

    Vertex buffer handling has been rewritten completely. 
    Having many small independent vertex buffers around is bad
    for hardware t&l, since vertex buffer switches are evil.

    Here's the plan:

    The gfx servers handle the API specific vertex buffer handling
    completely by having pools of optimally sized vertex pool buffers,
    which contain vertices for one or more 'public' vertex buffers.

    During rendering, vertex buffer nodes are sorted by their
    vertex pool buffer handle (and by their texture), so that 
    several objects can be rendered without switching API specific
    vertex buffers. 

    The vertex pool buffer handler works much like a small chunk
    memory allocator. It keeps a list of free chunks around and
    allocates new vertex pool buffers if a vertex allocation request
    cannot be satisfied.
*/
//--------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_NODE_H
#include "util/nnode.h"
#endif

#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#include <memory.h>

//--------------------------------------------------------------------
class nIndexBuffer;
class nVertexBubble;
class nPixelShader;
class nTextureArray;
class nGfxServer;
class nVertexPoolManager;
class N_PUBLIC nVertexPool : public nNode {
protected:
    nRef<nGfxServer> ref_gs;
    
    nVBufType vbuf_type;        // the vertex buffer type (read only, static, ...)
    int vtype;                  // the vertex type
    int vnum;                   // the number of vertices in the pool
    int stride4;                // stride in 4-byte-units
    
    nList free_bubbles;         // list of free vertex bubbles, sorted by starting index
    nList full_bubbles;         // list of used vertex bubbles

    void *pool;                 // the actual vertex pool

    float *coord_ptr;           // component pointers (only valid inside LockBubble(),UnlockBubble())
    float *norm_ptr;
    ulong *color_ptr;
    float *uv_ptrs[N_MAXNUM_TEXCOORDS];
    float *w_ptr;               // w0,[w1],[w2],[w3]
    int   *j_ptr;               // ji0,[ji1],[ji2],[ji3]

public:
    nVertexPool(nGfxServer *gs, nVertexPoolManager *vpm, nVBufType vbt, int vt, int size) 
        : vbuf_type(vbt),
          vtype(vt),
          vnum(size),
          stride4(0),
          pool(NULL),
          coord_ptr(NULL),
          norm_ptr(NULL),
          color_ptr(NULL),
          w_ptr(NULL),
          j_ptr(NULL)
    {
        n_assert(vpm);
        n_assert(gs);
        this->ref_gs = gs;
        this->SetPtr(vpm);
        memset(uv_ptrs,0,sizeof(uv_ptrs));
    };
    virtual ~nVertexPool();

    virtual bool Initialize(void);
    virtual bool LockBubble(nVertexBubble *);
    virtual void UnlockBubble(nVertexBubble *);
    virtual void RenderBubble(nVertexBubble *, nIndexBuffer *, nPixelShader *, nTextureArray *);

    virtual nVertexBubble *NewBubble(int size);
    virtual void ReleaseBubble(nVertexBubble *);

    //---------------------------------------------------------------
    int GetVBufType(void) {
        return this->vbuf_type;
    };
    int GetVertexType(void) {
        return this->vtype;
    };
    int GetNumVertices(void) {
        return this->vnum;
    };
    nVertexPoolManager *GetVertexPoolManager(void) {
        return (nVertexPoolManager *) this->GetPtr();
    };
    float *GetCoordPtr(void) {
        return this->coord_ptr;
    };
    float *GetNormPtr(void) {
        return this->norm_ptr;
    };
    ulong *GetColorPtr(void) {
        return this->color_ptr;
    };
    float *GetUvPtr(int stage) {
        n_assert((stage>=0) && (stage<N_MAXNUM_TEXCOORDS));
        return this->uv_ptrs[stage];
    };
    float *GetWeightPtr(void) {
        return this->w_ptr;
    };
    int *GetJointIndexPtr(void) {
        return this->j_ptr;
    };
    int GetStride4(void) {
        return this->stride4;
    };
    bool IsEmpty(void) {
        return this->full_bubbles.IsEmpty();
    };
};
//--------------------------------------------------------------------
#endif
