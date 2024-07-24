#define N_IMPLEMENTS nSBufShadowServer
//------------------------------------------------------------------------------
//  nsbufshadowserver_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shadow/nsbufshadowserver.h"
#include "shadow/nshadowcaster.h"
#include "mathlib/triangle.h"
#include "gfx/npixelshader.h"

nNebulaClass(nSBufShadowServer, "nshadowserver");

//------------------------------------------------------------------------------
/**
*/
nSBufShadowServer::nSBufShadowServer() :
    refGfx(kernelServer, this),
    refPixelShaderCCW(this),
    refPixelShaderCW(this),
    refPixelShaderPlane(this),
    refIndexBuffer(this),
    dynPlaneVBuf(kernelServer, this),
    dynVBuf(kernelServer, this),
    lightValid(false),
    numEdges(0),
    curVBuf(0),
    coordPtr(0),
    stride4(0),
    numQuads(0),
    curQuad(0)
{
    this->refGfx = "/sys/servers/gfx";

    // the render states for rendering the shadow volume with backface culling
    nPixelShaderDesc& psdCCW = this->pixelShaderDescCCW;
    psdCCW.SetNumStages(0);
    psdCCW.SetLightEnable(false);
    psdCCW.SetZWriteEnable(false);
    psdCCW.SetFogEnable(false);
    psdCCW.SetZFunc(N_CMP_LESS);
    psdCCW.SetCullMode(N_CULL_CCW);       // override!
    psdCCW.SetAlphaTestEnable(false);
    psdCCW.SetAlphaEnable(false);

    // the render states for rendering the shadow volume with frontface culling
    nPixelShaderDesc& psdCW = this->pixelShaderDescCW;
    psdCW.SetNumStages(0);
    psdCW.SetLightEnable(false);
    psdCW.SetZWriteEnable(false);
    psdCW.SetFogEnable(false);
    psdCW.SetZFunc(N_CMP_LESS);
    psdCW.SetCullMode(N_CULL_CW);       // override!
    psdCW.SetAlphaTestEnable(false);
    psdCW.SetAlphaEnable(false);

    // the render states for rendering the final overlay plane
    nPixelShaderDesc& psd = this->pixelShaderDescPlane;
    psd.SetNumStages(0);
    psd.SetLightEnable(false);
    psd.SetZWriteEnable(false);
    psd.SetFogEnable(false);
    psd.SetZFunc(N_CMP_ALWAYS);
    psd.SetCullMode(N_CULL_NONE);
    psd.SetAlphaTestEnable(false);
    psd.SetAlphaEnable(true);
    psd.SetAlphaSrcBlend(N_ABLEND_SRCALPHA);
    psd.SetAlphaDestBlend(N_ABLEND_INVSRCALPHA);

    
    // set the stencil clear color to 128
    nRState rs(N_RS_STENCILCLEAR, 128);
    this->refGfx->SetState(rs);
}

//------------------------------------------------------------------------------
/**
*/
nSBufShadowServer::~nSBufShadowServer()
{
    // release owned objects
    if (this->refPixelShaderCCW.isvalid())
    {
        this->refPixelShaderCCW->Release();
    }
    if (this->refPixelShaderCW.isvalid())
    {
        this->refPixelShaderCW->Release();
    }
    if (this->refPixelShaderPlane.isvalid())
    {
        this->refPixelShaderPlane->Release();
    }
    if (this->refIndexBuffer.isvalid())
    {
        this->refIndexBuffer->Release();
    }
    if (this->refPlaneIBuf.isvalid())
    {
        this->refPlaneIBuf->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nSBufShadowServer::BeginScene()
{
    // do nothing if shadow casting disabled
    if (!this->castShadows)
    {
        return;
    }

    // buffers initialized?
    if (!this->dynVBuf.IsValid())
    {
        this->InitBuffers();
    }
    this->lightValid = false;
    this->numEdges = 0;
}

//------------------------------------------------------------------------------
/**
    Only the first light source will be considered, and it will always
    be interpreted as a directional light. The function computes the
    direction of the light in view space, since that's where all
    shadow computation takes place later. The position of the light
    will be taken from the current modelview matrix, the direction
    of the light is assumed to point along the negative z axis.

    @param  light   a valid light object
*/
void
nSBufShadowServer::AttachLight(const matrix44& modelview, nLight* light)
{
    // do nothing if shadow casting disabled
    if (!this->castShadows)
    {
        return;
    }

    if (!this->lightValid)
    {
        if (light->GetType() == N_LIGHT_DIRECTIONAL)
        {
            this->lightValid = true;

            // get the light's direction vector in view space
            this->lightDir = -(modelview.z_component());
        }
    }
}

//------------------------------------------------------------------------------
/**
    Extract the silhouette edges from the shadow caster, transform them
    to view space, and store them in the edge array.

    @param  modelview   the modelview matrix of this caster
    @param  caster      a shadow caster object
*/
void
nSBufShadowServer::AttachCaster(const matrix44& modelview, nShadowCaster* caster)
{
    // do nothing if shadow casting disabled
    if (!this->castShadows)
    {
        return;
    }

    // do nothing if no light source defined!
    if (!this->lightValid)
    {
        return;
    }
    n_assert(caster);

    // get inverse of model view and transform light direction to model space
    matrix33 invModelview(modelview.M11, modelview.M12, modelview.M13,
                          modelview.M21, modelview.M22, modelview.M23,
                          modelview.M31, modelview.M32, modelview.M33);
    invModelview.transpose();
    vector3 modelLightDir(invModelview * this->lightDir);

    // iterate through winged edges and classify them as 
    // "front" (2 front faces), "back" (2 back faces), 
    // "silhouette" (1 front, 1 back face)
    int i;
    triangle t0, t1;
    caster->Lock();
    for (i = 0; i < caster->GetNumEdges(); i++)
    {
        nWingedEdge* we = &(caster->edges[i]);
        const vector3& v0  = caster->coords[we->v0];
        const vector3& v1  = caster->coords[we->v1];
        const vector3& vp0 = caster->coords[we->vp0];
        const vector3& vp1 = caster->coords[we->vp1];
        t0.set(v0, v1, vp0);
        t1.set(v1, v0, vp1);
        
        // get face normals for the 2 triangles (argh, expensive)
        vector3 n0(t0.normal());
        vector3 n1(t1.normal());

        // compute dot products
        float dot0 = n0 % modelLightDir;
        float dot1 = n1 % modelLightDir;

        // classify winged edge
        if (dot0 <= 0.001f)
        {
            if (!(dot1 <= 0.001f))
            {
                // silhouette
                this->AddEdge(modelview, v0, v1);
            }
        }
        else
        {
            if (dot1 <= 0.001f)
            {
                // silhouette (note: different order then other add edge)
                this->AddEdge(modelview, v1, v0);
            }
        }

    }
    caster->Unlock();
}

//------------------------------------------------------------------------------
/**
    Does the actual shadow rendering. All edges which have been 
    collected during the attach pass are extruded away from the light source,
    (z coordinates behind the front clipping plane will be shifted into the
    view volume to deal with the "viewer in shadow volume" special case),
    then the shadow volumes are rendered twice to the stencil buffer
    (one pass for front faces, one pass for back faces). Finally, a stencil
    masked alpha blended quad will be rendered over the whole screen.
*/  
void
nSBufShadowServer::EndScene()
{
    // do nothing if shadow casting disabled
    if (!this->castShadows)
    {
        return;
    }

    // do nothing if no light source defined
    if (!this->lightValid)
    {
        return;
    }

    // get the extrude vector
    vector3 extrude = this->lightDir * 100000.0f;

    // visualize silhouette edges
    nGfxServer* gs = this->refGfx.get();

    matrix44 ident;
    gs->PushMatrix(N_MXM_MODELVIEW);
    gs->SetMatrix(N_MXM_MODELVIEW, ident);

    this->BeginRender();
    int i;
    vector3 vex0, vex1;
    for (i = 0; i < this->numEdges; i++)
    {
        // shadow volume vertices are moved away from the viewer to reduce the jagginess 
        // when objects shadow themselves at the silhouette edge, the depth test then smoothes
        // the critical areas a bit. Not perfect, but cheap :)
        const vector3& v0 = this->edges[i][0];
        const vector3& v1 = this->edges[i][1];
        vex0 = v0 + extrude;
        vex1 = v1 + extrude;

        // write vertex 0
        this->coordPtr[0] = v0.x; this->coordPtr[1] = v0.y; this->coordPtr[2] = v0.z - 0.02f;
        this->coordPtr += this->stride4;

        // write vertex 1
        this->coordPtr[0] = v1.x; this->coordPtr[1] = v1.y; this->coordPtr[2] = v1.z - 0.02f;
        this->coordPtr += this->stride4;

        // write vertex 2
        this->coordPtr[0] = vex1.x; this->coordPtr[1] = vex1.y; this->coordPtr[2] = vex1.z - 0.02f;
        this->coordPtr += this->stride4;

        // write vertex 3
        this->coordPtr[0] = vex0.x; this->coordPtr[1] = vex0.y; this->coordPtr[2] = vex0.z - 0.02f;
        this->coordPtr += this->stride4;

        // swap buffers?
        this->curQuad++;
        if (this->curQuad >= this->numQuads)
        {
            this->SwapRender();
        }
    }
    this->EndRender();    
    gs->PopMatrix(N_MXM_MODELVIEW);
}

//------------------------------------------------------------------------------
/**
    Initialize the dynamic vertex buffer, the index buffer and the
    pixel shader for rendering.
*/
void
nSBufShadowServer::InitBuffers()
{
    n_assert(!this->refIndexBuffer.isvalid());
    n_assert(!this->refPixelShaderCCW.isvalid());
    n_assert(!this->refPixelShaderCW.isvalid());

    // initialize dynamic vertex buffer
    this->dynVBuf.Initialize(N_VT_COORD, 0);
    this->numQuads = this->dynVBuf.GetNumVertices() / 4;

    // initialize pixel shader
    this->refIndexBuffer = this->refGfx->NewIndexBuffer(0);
    nIndexBuffer* ibuf = this->refIndexBuffer.get();

    int numIndices = this->numQuads * 6;
    ibuf->Begin(N_IBTYPE_STATIC, N_PTYPE_TRIANGLE_LIST, numIndices);
    int indexIndex = 0;
    int vertexIndex = 0;
    for (; vertexIndex < (this->numQuads * 4); vertexIndex += 4)
    {
        ibuf->Index(indexIndex++, vertexIndex + 0);   
        ibuf->Index(indexIndex++, vertexIndex + 1); 
        ibuf->Index(indexIndex++, vertexIndex + 2);
        
        ibuf->Index(indexIndex++, vertexIndex + 0); 
        ibuf->Index(indexIndex++, vertexIndex + 2); 
        ibuf->Index(indexIndex++, vertexIndex + 3);
    }
    ibuf->End();

    // initialize overlay plane vertex and index buffer
    this->dynPlaneVBuf.Initialize(N_VT_COORD | N_VT_RGBA, 4);
    this->refPlaneIBuf = this->refGfx->NewIndexBuffer(0);

    // fill overlay plane indices
    ibuf = this->refPlaneIBuf.get();
    ibuf->Begin(N_IBTYPE_STATIC, N_PTYPE_TRIANGLE_LIST, 6);
    ibuf->Index(0, 0);
    ibuf->Index(1, 1);
    ibuf->Index(2, 2);
    ibuf->Index(3, 2);
    ibuf->Index(4, 3);
    ibuf->Index(5, 0);
    ibuf->End();

    // initialize pixel shaders
    this->refPixelShaderCCW = this->refGfx->NewPixelShader(0);
    this->refPixelShaderCCW->SetShaderDesc(&(this->pixelShaderDescCCW));

    this->refPixelShaderCW = this->refGfx->NewPixelShader(0);
    this->refPixelShaderCW->SetShaderDesc(&(this->pixelShaderDescCW));

    this->refPixelShaderPlane = this->refGfx->NewPixelShader(0);
    this->refPixelShaderPlane->SetShaderDesc(&(this->pixelShaderDescPlane));
}

//------------------------------------------------------------------------------
/**
    Begin rendering the shadow volume.
*/
void
nSBufShadowServer::BeginRender()
{
    this->curVBuf  = this->dynVBuf.Begin(this->refIndexBuffer.get(), this->refPixelShaderCW.get(), 0);
    this->coordPtr = this->curVBuf->coord_ptr;
    this->stride4  = this->curVBuf->stride4;
    this->curQuad  = 0;

    // set up some generic stencil buffer operations
    nGfxServer* gfx = this->refGfx.get();
    nRState rs;
    rs.Set(N_RS_STENCILENABLE,    N_TRUE);              gfx->SetState(rs);
    rs.Set(N_RS_STENCILFUNC,      N_CMP_ALWAYS);        gfx->SetState(rs);
    rs.Set(N_RS_STENCILZFAIL,     N_STENCILOP_KEEP);    gfx->SetState(rs);
    rs.Set(N_RS_STENCILFAIL,      N_STENCILOP_KEEP);    gfx->SetState(rs);
    rs.Set(N_RS_STENCILREF,       129);                 gfx->SetState(rs);
    rs.Set(N_RS_STENCILMASK,      -1);                  gfx->SetState(rs);
    rs.Set(N_RS_COLORWRITEENABLE, N_FALSE);             gfx->SetState(rs);
    rs.Set(N_RS_STENCILCLEAR,     128);                 gfx->SetState(rs);
}

//------------------------------------------------------------------------------
/**
    Set the additional render states to render the front facing shadow
    volume triangles.
*/
void
nSBufShadowServer::SetRenderStatesFront()
{
    nRState rs(N_RS_STENCILPASS, N_STENCILOP_DECR);
    this->refGfx->SetState(rs);
    this->refGfx->Rgba(0.0f, 0.0f, 0.5f, 1.0f);
}

//------------------------------------------------------------------------------
/**
    Set the additional render states to render the back facing shadow volume
    triangles.
*/
void
nSBufShadowServer::SetRenderStatesBack()
{
    nRState rs(N_RS_STENCILPASS, N_STENCILOP_INCR);
    this->refGfx->SetState(rs);
    this->refGfx->Rgba(0.5f, 0.0f, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------
/**
    Set the additional render states to render the final alpha blended
    quad.
*/
void
nSBufShadowServer::SetRenderStatesPlane()
{
    nRState rs;
    nGfxServer* gfx = this->refGfx.get();
    rs.Set(N_RS_STENCILREF, 129);                   gfx->SetState(rs);
    rs.Set(N_RS_STENCILFUNC, N_CMP_LESSEQUAL);      gfx->SetState(rs);
    rs.Set(N_RS_STENCILPASS, N_STENCILOP_KEEP);     gfx->SetState(rs);
    rs.Set(N_RS_COLORWRITEENABLE, N_TRUE);          gfx->SetState(rs);
}

//------------------------------------------------------------------------------
/**
    Render the current shadow volume portion twice, first the front faces,
    then the backfaces.
*/
void
nSBufShadowServer::SwapRender()
{
    // get number of vertices and indices to render
    int numVertices = this->curQuad * 4;
    int numIndices  = this->curQuad * 6;
    
    // render front faces
    this->SetRenderStatesFront();
    this->dynVBuf.Render(numVertices, numIndices, this->refPixelShaderCCW.get(), 0);

    // render back faces and swap dynamic vertex buffer
    this->SetRenderStatesBack();
    this->curVBuf  = this->dynVBuf.Swap(numVertices, numIndices);
    this->coordPtr = this->curVBuf->coord_ptr;
    this->stride4  = this->curVBuf->stride4;
    this->curQuad  = 0;
}

//------------------------------------------------------------------------------
/**
    Finish rendering the shadow volume, just renders the remaining stuff.
*/
void
nSBufShadowServer::EndRender()
{
    // get number of vertices and indices to render
    int numVertices = this->curQuad * 4;
    int numIndices  = this->curQuad * 6;

    // render front faces
    this->SetRenderStatesFront();
    this->dynVBuf.Render(numVertices, numIndices, this->refPixelShaderCCW.get(), 0);

    // render back faces
    this->SetRenderStatesBack();
    this->dynVBuf.End(numVertices, numIndices);
    this->curVBuf  = 0;
    this->coordPtr = 0;
    this->stride4  = 0;
    this->curQuad  = 0;

    // render the overlay quad
    matrix44 ident;

    nGfxServer* gfx = this->refGfx.get();
    gfx->PushMatrix(N_MXM_PROJECTION);
    gfx->SetMatrix(N_MXM_PROJECTION, ident);
    
    this->SetRenderStatesPlane();

    nVertexBuffer* planeVBuf = this->dynPlaneVBuf.Begin(this->refPlaneIBuf.get(), this->refPixelShaderPlane.get(), 0);
    n_assert(planeVBuf);
    ulong color;
    if (planeVBuf->GetColorFormat() == N_COLOR_RGBA)
    {
        color = n_f2rgba(this->shadowColor.x, this->shadowColor.y, this->shadowColor.z, this->shadowColor.w);
    }
    else
    {
        color = n_f2bgra(this->shadowColor.x, this->shadowColor.y, this->shadowColor.z, this->shadowColor.w);
    }
    planeVBuf->Coord(0, vector3(-1.0f, -1.0f, 0.0f));
    planeVBuf->Coord(1, vector3(+1.0f, -1.0f, 0.0f));
    planeVBuf->Coord(2, vector3(+1.0f, +1.0f, 0.0f));
    planeVBuf->Coord(3, vector3(-1.0f, +1.0f, 0.0f));
    planeVBuf->Color(0, color);
    planeVBuf->Color(1, color);
    planeVBuf->Color(2, color);
    planeVBuf->Color(3, color);
    this->dynPlaneVBuf.End(4, 6);

    gfx->PopMatrix(N_MXM_PROJECTION);

    // diable stencil buffer state
    nRState rs(N_RS_STENCILENABLE, N_FALSE);
    this->refGfx->SetState(rs);    
}

//------------------------------------------------------------------------------
