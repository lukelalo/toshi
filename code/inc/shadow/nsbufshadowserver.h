#ifndef N_SBUFSHADOWSERVER_H
#define N_SBUFSHADOWSERVER_H
//------------------------------------------------------------------------------
/**
    @class nSBufShadowServer

    @brief Implement stencil buffer shadows.

    The nSBufShadowServer derives from nShadowServer and implements a
    stencil buffer shadow system (find silhouettes of shadow casters 
    from lightsource, create shadow volume by extruding silhouette,
    render front faces with stencil buffer increment, render back
    faces with stencil buffer decrement, render a stencil buffer masked
    alphablended quad the size of the screen).

    Only the first light source will be considered, and will always
    be treated as a directional light.

    (C) 2001 RadonLabs GmbH
*/

#ifndef N_SHADOWSERVER_H
#include "shadow/nshadowserver.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#ifndef N_PIXELSHADERDESC_H
#include "gfx/npixelshaderdesc.h"
#endif

#undef N_DEFINES
#define N_DEFINES nSBufShadowServer
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nGfxServer;
class nSBufShadowServer : public nShadowServer
{
public:
    /// constructor
    nSBufShadowServer();
    /// destructor
    virtual ~nSBufShadowServer();

    /// begin a shadow casting scene
    virtual void BeginScene();
    /// attach a light source
    virtual void AttachLight(const matrix44& modelview, nLight* light);
    /// attach a shadow caster
    virtual void AttachCaster(const matrix44& modelview, nShadowCaster* caster);
    /// finish a shadow casting scene
    virtual void EndScene();

    static nKernelServer *kernelServer;

private:
    /// initialize vertex buffers for rendering
    void InitBuffers();
    /// begin rendering the shadow volume
    void BeginRender();
    /// swap vertex buffers during rendering
    void SwapRender();
    /// finish rendering the shadow volume
    void EndRender();
    /// add edge to edge array
    void AddEdge(const matrix44& modelview, const vector3& v0, const vector3& v1);
    /// set the render states for front facing shadow volume triangles
    void SetRenderStatesFront();
    /// set the render states for back facing shadow volume triangles
    void SetRenderStatesBack();
    /// set the render states to render the final alpha blended quad
    void SetRenderStatesPlane();

protected:
    enum
    {
        MAXNUMEDGES = 8192,
    };

    nAutoRef<nGfxServer> refGfx;            // gfx server to render to
    nRef<nPixelShader>   refPixelShaderCCW; // defines render states for shadow volume rendering
    nRef<nPixelShader>   refPixelShaderCW;  
    nRef<nPixelShader>   refPixelShaderPlane;
    nRef<nIndexBuffer>   refIndexBuffer;    // index buffer for shadow volume rendering
    nRef<nIndexBuffer>   refPlaneIBuf;      // index buffer for final quad
    nDynVertexBuffer     dynVBuf;           // dynamic vertex buffer for rendering shadow volumes
    nDynVertexBuffer     dynPlaneVBuf;      // vertex buffer for final quad
    
    nPixelShaderDesc pixelShaderDescCCW;    // pixel shader for counterclockwise culling
    nPixelShaderDesc pixelShaderDescCW;     // pixel shader for clockwise culling
    nPixelShaderDesc pixelShaderDescPlane;  // pixel shader for shadow plane
    bool lightValid;                        // set to true for 1st light source in current frame
    vector3 lightDir;                       // direction of light source in view space
    
    int numEdges;                           // number of edges in silhouette pool
    vector3 edges[MAXNUMEDGES][2];          // silhouette edge pool

    nVertexBuffer* curVBuf;                 // currently active vertex buffer for shadow volume rendering
    float* coordPtr;                        // current 3d coord pointer in vertex buffer
    int stride4;                            // sizeof(float) stride
    int numQuads;                           // size of vertex buffer in num quads
    int curQuad;                            // currently rendered quad
};

//------------------------------------------------------------------------------
/**
    Transform edge to viewer space and add to edge array. 
    Silently ignore if edge array is full.
*/
inline
void
nSBufShadowServer::AddEdge(const matrix44& modelview, const vector3& v0, const vector3& v1)
{
    if (this->numEdges < MAXNUMEDGES)
    {
        this->edges[this->numEdges][0] = modelview * v0;
        this->edges[this->numEdges][1] = modelview * v1;
        this->numEdges++;
    }
}

//------------------------------------------------------------------------------
#endif
