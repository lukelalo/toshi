#ifndef N_STATICMESHEMITTER_H
#define N_STATICMESHEMITTER_H
//------------------------------------------------------------------------------
/**
    @class nStaticMeshEmitter

    @brief A particle "emitter" which simply places a static particle at
    each vertex of a mesh.
*/
#ifndef N_PEMITTER_H
#include "particle/npemitter.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nStaticMeshEmitter
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nMeshNode;
class nGfxServer;
class nVertexBuffer;
class nSceneGraph2;
class nStaticMeshEmitter : public nPEmitter
{
public:
    /// default constructor.
    nStaticMeshEmitter();
    /// default destructor.
    virtual ~nStaticMeshEmitter();
    /// update internal state
    virtual void Compute(nSceneGraph2* sceneGraph);
    /// object persistency
    virtual bool SaveCmds(nPersistServer *);

    /// begin pulling particles
    virtual bool BeginPullParticles(int renderContext, float tstamp);
    /// pull a chunk of particles
    virtual int  PullParticles(nPCorn *&);
    /// finish pulling particles
    virtual void EndPullParticles(void);
    /// set source mesh
    void SetMeshNode(const char* path);
    /// get mesh node
    const char* GetMeshNode();

    static nKernelServer* kernelServer;

protected:
    enum
    {
        CHUNKSIZE = 128,    // size of particle chunk for PullParticles()
    };
    nDynAutoRef<nMeshNode> refMesh;         // ref to source mesh object
    nAutoRef<nGfxServer>   refGfx;          // ref to gfx server object
    nAutoRef<nSceneGraph2> refSceneGraph;   // ref to scene graph object
    matrix44 transform;                 // matrix to transform from model to world space
    matrix33 identity;                  // const identity matrix
    nVertexBuffer* vertexBuffer;        // vertex buffer object for current frame
    float curTimeStamp;                 // current time stamp
    int curVertex;                      // current vertex in mesh
    int numVertices;                    // number of vertices in mesh
    float* curVertexPtr;                // pointer to current vertex data
    int vertexStride;                   // float* stride of vertex array
    nPCorn particles[CHUNKSIZE];        // array of particle chunk
};
//------------------------------------------------------------------------------
#endif
