#define N_IMPLEMENTS nD3D8Server
//-----------------------------------------------------------------------------
//  nd3d8_resources.cc
//  (C) 2001 Andre Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nd3d8server.h"
#include "gfx/nvertexbuffer.h"
#include "gfx/nd3d8vertexpool.h"

//-----------------------------------------------------------------------------
/**
    Create a new uninitialized texture object of class nTexture. The
    object will actually be of class nD3D8Texture, but that should
    be irrelevant to the outside. Before creating a texture you
    should first check if the texture is already loaded (via
    nGfxServer::FindTexture()). Use the full pathname of the texture
    file as identifier for the resource sharing.

    @param  identifier   string id used for resource sharing
    @return              an empty nTexture object
*/
nTexture *
nD3D8Server::NewTexture(const char* identifier)
{
    nTexture *t;
    char resid[N_MAXNAMELEN];
    this->getResourceID(identifier, resid, sizeof(resid));
    kernelServer->PushCwd(this->ref_texdir.get());
    t = (nTexture *) kernelServer->New("nd3d8texture", resid);
    kernelServer->PopCwd();
    return t;
}

//-----------------------------------------------------------------------------
/**
    Create a new uninitialized nPixelShader object. The actual object
    returned will be of class nD3D8PixelShader, but that's irrelevant
    to the outside. If you want to use resource sharing on the
    pixel shaders, use nGfxServer::FindPixelShader() first to see
    if a pixel shader with the same identifier already exists.

    @param identifier   string id used for resource sharing
    @return             an empty nPixelShader object
*/
nPixelShader*
nD3D8Server::NewPixelShader(const char* identifier)
{
    nPixelShader *ps;
    char resid[N_MAXNAMELEN];
    this->getResourceID(identifier, resid, sizeof(resid));
    kernelServer->PushCwd(this->ref_pshaderdir.get());
    ps = (nPixelShader *) kernelServer->New("nd3d8pixelshader",resid);
    kernelServer->PopCwd();
    return ps;
}

//-----------------------------------------------------------------------------
/**
    Create a new uninitialized nIndexBuffer object (actually an
    nD3D8IndexBuffer object). If you want to use resource sharing you
    should check via nGfxServer::FindIndexBuffer() if an index buffer
    object with the same identifier already exists.

    @param identifier   string id used for resource sharing
    @return             an empty nIndexBuffer object
*/
nIndexBuffer*
nD3D8Server::NewIndexBuffer(const char* identifier)
{
    nIndexBuffer *ib;
    char resid[N_MAXNAMELEN];
    this->getResourceID(identifier, resid, sizeof(resid));
    kernelServer->PushCwd(this->ref_ixbufdir.get());
    ib = (nIndexBuffer *) kernelServer->New("nd3d8indexbuffer", resid);
    n_assert(ib);
    kernelServer->PopCwd();
    return ib;
}

//-----------------------------------------------------------------------------
/**
    Create a new unitialized nVertexBuffer object (actually an nD3D8VertexBuffer
    object). If you want to use resource sharing you
    should check via nGfxServer::FindIndexBuffer() if an index buffer
    object with the same identifier already exists.
    Please note that for N_VBTYPE_WRITEONLY vertex buffers the numVertices
    parameter will be ignored.

    @param identifier   string id used for resource sharing
    @return             an empty nVertexBuffer object
*/
nVertexBuffer*
nD3D8Server::NewVertexBuffer(const char *identifier,
                             nVBufType vbufType, 
                             int vertexType, 
                             int numVertices)
{
    nVertexBuffer *vbuffer;
    char resid[N_MAXNAMELEN];

    // create a new vertex buffer object
    this->getResourceID(identifier, resid, sizeof(resid));
    kernelServer->PushCwd(this->ref_vxbufdir.get());
    vbuffer = (nVertexBuffer *) kernelServer->New("nvertexbuffer", resid);
    n_assert(vbuffer);
    kernelServer->PopCwd();

    // ask the vertex pool manager for the requested vertex format
    // to create a vertex bubble object and initialize vertex buffer 
    // object with it
    n_assert((vbufType>=0) && (vbufType<N_NUM_VBTYPES));
    nVertexPoolManager *vpm = this->vpoolManager[vbufType];
    n_assert(vpm);

    // allocate a vertex bubble of the requested size from the vertex pool manager
    // nD3D8VertexPool ignores numVertices if we are a writeonly vertex buffer
    nVertexBubble *vbubble = vpm->NewVertexBubble(vertexType, numVertices);
    n_assert(vbubble);

    // attach the vertex bubble and the index list object to the vertex buffer
    vbuffer->Initialize(this, vbubble);
    return vbuffer;
}


//-----------------------------------------------------------------------------
/**
    Create a new nVertexPool object, this method should only be
    called from the code that implements the vertex pool infrastructure.
    Use nGfxServer::CreateVertexBuffer() to create an actual vertex
    buffer object.

    @param  vpManager   pointer to responsible vertex pool manager 
    @param  vbType      requested nVBufType of the pool
    @param  vertexType  requested vertex type (or vertex format)
    @param  numVertices number of vertices which should fit into the pool, or 0 if doesn't matter
    @return             an empty nVertexPool object

    history:
    - 23-Jan-2001   floh    created
*/
nVertexPool*
nD3D8Server::NewVertexPool(nVertexPoolManager* vpManager,
                           nVBufType vbType,
                           int vertexType,
                           int numVertices)
{
    // readonly vertex buffer are never rendered and don't require
    // the overhead of wrapping a d3d vertex buffer around it
    nVertexPool *vp = NULL;
    if (N_VBTYPE_READONLY == vbType) 
    {
        vp = (nVertexPool *) n_new nVertexPool(this,vpManager, vbType, vertexType, numVertices);
    } 
    else 
    {
        vp = (nVertexPool *) n_new nD3D8VertexPool(this, vpManager, vbType, vertexType, numVertices);
    }
    vp->Initialize();
    return vp;
}

//-----------------------------------------------------------------------------
