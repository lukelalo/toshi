#define N_IMPLEMENTS nStaticMeshEmitter
//------------------------------------------------------------------------------
//  nstaticmeshemitter_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "particle/nstaticmeshemitter.h"
#include "gfx/nvertexbuffer.h"
#include "gfx/nscenegraph2.h"
#include "node/nmeshnode.h"

nNebulaScriptClass(nStaticMeshEmitter, "npemitter");

//------------------------------------------------------------------------------
/**
*/
nStaticMeshEmitter::nStaticMeshEmitter() :
    refMesh(kernelServer, this),
    refGfx(kernelServer, this),
    refSceneGraph(kernelServer, this),
    vertexBuffer(0),
    curTimeStamp(0.0f),
    curVertex(0),
    numVertices(0),
    curVertexPtr(0),
    vertexStride(0)
{
    this->refGfx = "/sys/servers/gfx";
    this->refSceneGraph = "/sys/servers/sgraph2";
}

//------------------------------------------------------------------------------
/**
*/
nStaticMeshEmitter::~nStaticMeshEmitter()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticMeshEmitter::SetMeshNode(const char* path)
{
    n_assert(path);
    this->refMesh = path;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nStaticMeshEmitter::GetMeshNode()
{
    return this->refMesh.getname();
}

//------------------------------------------------------------------------------
/**
*/
bool
nStaticMeshEmitter::BeginPullParticles(int /*renderContext*/, float tstamp)
{   
    if (this->vertexBuffer && 
        ((this->numVertices = this->vertexBuffer->GetNumVertices()) > 0))
    {
        // make sure its a read only vertex buffer
        n_assert(this->vertexBuffer->GetVBufType() == N_VBTYPE_READONLY);

        // lock vertex buffer for reading and get pointer to coord array
        this->vertexBuffer->LockVertices();
        this->vertexStride = this->vertexBuffer->stride4;
        this->curVertex    = 0;
        this->curVertexPtr = this->vertexBuffer->coord_ptr;

        // compute a birth time stamp so that particle age 
        // circles between 0 and lifetime
        n_assert(this->lifetime > 0.0f);
        this->curTimeStamp = (float) n_ftol(tstamp / this->lifetime) * this->lifetime;

        // matrix to transfom from model to world space
        this->transform = this->refSceneGraph->GetTransform();
        matrix44 view;
        this->refGfx->GetMatrix(N_MXM_VIEWER, view);
        this->transform.mult_simple(view);

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
int
nStaticMeshEmitter::PullParticles(nPCorn*& pcornPtr)
{
    n_assert(this->curVertexPtr);
    n_assert(this->numVertices > 0);
    vector3 pos;

    // fill particle chunk with data
    int i;
    for (i = 0; 
         (i < CHUNKSIZE) && (this->curVertex < this->numVertices); 
         i++, this->curVertex++, this->curVertexPtr += this->vertexStride)
    {
        // get position of current vertex
        pos.set(this->curVertexPtr[0], this->curVertexPtr[1], this->curVertexPtr[2]);
        
        // transform to world coords
        pos = this->transform * pos;

        // fill current particle
        this->particles[i].p      = pos; 
        this->particles[i].tstamp = this->curTimeStamp;
    }
         
    // validate pointer to particles and return number of valid particles
    pcornPtr = this->particles;
    return i;
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticMeshEmitter::EndPullParticles()
{
    n_assert(this->vertexBuffer);
    this->curVertexPtr = 0;
    this->vertexBuffer->UnlockVertices();
}

//------------------------------------------------------------------------------
/**
*/
void
nStaticMeshEmitter::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    nVisNode::Compute(sceneGraph);

    // resolve vertex buffer pointer
    if (this->refMesh.isvalid())
    {
        // first invoke compute (which update current vertex buffer pointer in scene graph) 
        this->refMesh->Compute(sceneGraph);
        
        // get current vertex buffer pointer from scene graph
        this->vertexBuffer = sceneGraph->GetVertexBuffer();
        n_assert(this->vertexBuffer);
        sceneGraph->SetVertexBuffer(0);
    
        // that's it, the vertex buffer will then be used by the
        // pull particle methods
    }
}

//------------------------------------------------------------------------------

