#ifndef N_TRAILRENDER_H
#define N_TRAILRENDER_H

/**
    @class nTrailRender

    @brief render particles connected as triangle strips.

    as opposed to building a sprite "around" a particle, this renderer connects
    particles in a triangle "strip" (technically it's a list of independent 
    triangles, may be optimized to a strip later), each particle representing 
    a junction of the strip (depicted by the "pipe" in the asci diagram below).

    -------------------------
    | / | \ | / | \ | / | \ |
    -------------------------
    ^   ^   ^   ^   ^   ^   ^
*/

#ifndef N_PRENDER_H
#include "particle/nprender.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#undef N_DEFINES
#define N_DEFINES nTrailRender
#include "kernel/ndefdllclass.h"

class nGfxServer;
class nPixelShader;
class nTextureArray;
class nIndexBuffer;
class nPCorn;
class N_DLLCLASS nTrailRender : public nPRender
{
public:
    /// default constructor.
    nTrailRender();
    /// default destructor.
    virtual ~nTrailRender();

    /// attach this nTrailRender instance to the scene.
    virtual bool Attach(nSceneGraph2* sceneGraph);
    /// perform processing for this nTrailRender instance for this frame.
    virtual void Compute(nSceneGraph2* sceneGraph);

    /// nKernelServer dependency.
    static nKernelServer* kernelServer;

protected:
    /// initialize the nDynVertexBuffer for use
    void InitBuffers();
    /// prepare nDynVertexBuffer for rendering
    void BeginRender(nPixelShader* pixelShader, nTextureArray* textureArray);
    /// finish one rendering pass
    void SwapRender();
    /// finish rendering process
    void EndRender();
    /// write one vertex to the nDynVertexBuffer
    void WriteVertex(const vector3& pos, ulong color, float u, float v);
    /// wrap the entire particle rendering
    void RenderParticles(nSceneGraph2*, float timeStamp);
    /// calculate the trail attributes for this particle (width)
    bool CalcSegmentData(
        nPCorn* particle,
        float timeStamp,
        const matrix44& view,
        vector3& segmentPosRt,
        vector3& segmentPosLt,
        float& segmentV,
        ulong& segmentColor);
    /// write the quad based on two particles into the vertex buffer.
    void ConstructQuad(
        nPCorn* from,
        nPCorn* to,
        float timeStamp,
        const matrix44& view);

private:
    /// we need access to the gfx server for object construction
    nAutoRef<nGfxServer>  refGfx;
    /// an index buffer which holds the gfx primitive definition
    nRef<nIndexBuffer> refIBuf;
    /// a dynamic vertex buffer where the vertices go
    nDynVertexBuffer dynVBuf;
    /// number of quads that fit into dynVB
    int numQuads;
    /// the currently rendered quad
    int curQuad;
    /// the current vertex buffer to render to (returned by dynVB)
    nVertexBuffer* curVBuf;
    /// tmp pointer to vertex coordinates
    float* coordPtr;
    /// tmp pointer to vertex colors
    ulong* colorPtr;
    /// tmp pointer to uv coordinates
    float* uvPtr;
    /// the stride from one vertex to the next in sizeof(float)
    int stride4;
    /// an identity matrix which we need in the render loop
    matrix44 identity;
};

#endif
