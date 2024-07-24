#ifndef N_GLVERTEXPOOL_H
#define N_GLVERTEXPOOL_H
//-------------------------------------------------------------------
/**
    @class nGlVertexPool

    @brief vertex pool on top of OpenGL

    @see nD3D8VertexPool for a explanation of vertex pools.
*/
//-------------------------------------------------------------------
#ifndef N_VERTEXPOOL_H
#include "gfx/nvertexpool.h"
#endif

#ifndef N_GLSERVER_H
#include "gfx/nglserver.h"
#endif

//-------------------------------------------------------------------
class nGlVertexPool : public nVertexPool {

    float *base_ptr;    // the pointer to the vertex array 

public:
    nGlVertexPool(nGfxServer *gs, nVertexPoolManager *vpm, nVBufType vbt, int vt, int size)
        : nVertexPool(gs,vpm,vbt,vt,size),
          base_ptr(NULL)
    { };
    virtual ~nGlVertexPool();
    virtual bool Initialize(void);
    virtual bool LockBubble(nVertexBubble *);
    virtual void UnlockBubble(nVertexBubble *);
    virtual void RenderBubble(nVertexBubble *, nIndexBuffer *, nPixelShader *, nTextureArray *);
};
//-------------------------------------------------------------------
#endif
