#define N_IMPLEMENTS nGfxServer
//------------------------------------------------------------------------------
//  nmeshloader.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/nmeshloader.h"
#include "gfx/nvertexbuffer.h"
#include "gfx/nindexbuffer.h"
#include "shadow/nshadowcaster.h"

//------------------------------------------------------------------------------
/**
*/
nMeshLoader::nMeshLoader(nKernelServer* ks) :
    kernelServer(ks),
    refFileServer(ks, 0),
    inBegin(false),
    readOnly(false),
    gfxServer(0),
    shadowServer(0),
    vertexBuffer(0),
    indexBuffer(0),
    shadowCaster(0)
{
    this->refFileServer = "/sys/servers/file2";
}

//------------------------------------------------------------------------------
/**
*/
nMeshLoader::~nMeshLoader()
{
    n_assert(!this->inBegin);
    n_assert(0 == this->vertexBuffer);
    n_assert(0 == this->indexBuffer);
    n_assert(0 == this->shadowCaster);
}

//------------------------------------------------------------------------------
/**
    Begin loading a mesh, this initializes the pointer to the server objects
    which are should be used to construct the vertex buffer, index buffer and 
    shadow caster objects.

    @param  gfx         pointer to gfx server used for object construction
    @param  shadow      pointer to shadow server used for object construction
    @param  readonly    true if vertex buffer should be created as readonly
*/
void
nMeshLoader::Begin(nGfxServer* gfx, nShadowServer* shadow, bool readonly)
{
    n_assert(gfx);
    n_assert(shadow);
    n_assert(!this->inBegin);

    this->gfxServer    = gfx;
    this->shadowServer = shadow;
    this->readOnly     = readonly;
    this->inBegin      = true;
}

//------------------------------------------------------------------------------
/**
    Load a mesh, this should construct the vertex buffer, index buffer and
    shadow caster, read the mesh file and fill the constructed objects with
    the mesh file data.
    After loading, call the Obtain*() method to obtain pointers to 
    the objects created and initialized during load.
    This method should be overwritten by specific subclasses. 

    @param  fileName    absolute path to mesh file to be loaded
    @return             true, if loading successful
*/
bool
nMeshLoader::Load(const char* /*fileName*/)
{
    n_assert(this->inBegin);
    n_printf("nMeshLoader::Load() called!\n");
    return false;
}

//------------------------------------------------------------------------------
/**
    Obtain a pointer to the created vertex buffer object. This will increment
    the ref count of the vertex buffer object by one. Call this method
    after a successful Load() and before End().

    @return     pointer to initialized vertex buffer object
*/
nVertexBuffer*
nMeshLoader::ObtainVertexBuffer()
{
    n_assert(this->inBegin);
    n_assert(this->vertexBuffer);
    this->vertexBuffer->AddRef();
    return this->vertexBuffer;
}

//------------------------------------------------------------------------------
/**
    Obtain a pointer to the created index buffer object. This will increment
    the ref count of the index buffer object by one. Call this method after
    a successful Load() and before End().

    @return     pointer to initialized index buffer object
*/
nIndexBuffer*
nMeshLoader::ObtainIndexBuffer()
{
    n_assert(this->inBegin);
    n_assert(this->indexBuffer);
    this->indexBuffer->AddRef();
    return this->indexBuffer;
}

//------------------------------------------------------------------------------
/**
    Obtain a pointer to the created shadow caster object. This will increment
    the ref count of the shadow caster object by one. Call this method after
    a successful Load() and before End().

    @return     pointer to initialized shadow caster object
*/
nShadowCaster*
nMeshLoader::ObtainShadowCaster()
{
    n_assert(this->inBegin);
    if (this->shadowCaster)
    {
        this->shadowCaster->AddRef();
    }
    return this->shadowCaster;
}

//------------------------------------------------------------------------------
/**
    Finish loading mesh file and obtaining pointers to the created objects.
    Release() will be invoked on the created objects once. This will lead
    to the destruction of the objects which have not been obtained through
    the various Obtain*() methods (it is assumed that the client is not
    interested in those objects if he didn't obtain them.
*/
void
nMeshLoader::End()
{
    n_assert(this->inBegin);

    if (this->vertexBuffer)
    {
        this->vertexBuffer->Release();
        this->vertexBuffer = 0;
    }
    if (this->indexBuffer)
    {
        this->indexBuffer->Release();
        this->indexBuffer = 0;
    }
    if (this->shadowCaster)
    {
        this->shadowCaster->Release();
        this->shadowCaster = 0;
    }
    this->gfxServer    = 0;
    this->shadowServer = 0;
    this->inBegin = false;
}

//------------------------------------------------------------------------------
