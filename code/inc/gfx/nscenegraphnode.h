#ifndef N_SGNODE2_H
#define N_SGNODE2_H
//------------------------------------------------------------------------------
/**
    @class nSGNode2

    @brief stores all necessary information for a node in a scene graph

    Scenegraph nodes are filled during the nVisNode::Attach() pass of a 
    scene and store all information necessary to render a single node in 
    the scene graph. Since the same nVisNode object hierarchies can be
    rendered multiple times per frame (with different positions and
    animation channel sets), nSceneGraphNodes are necessary to store
    the necessary "render pass state" for the visual hierarchies.
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

//------------------------------------------------------------------------------
class matrix44;
class nChannelContext;
class nSGNode2
{
    /// the constructor
    nSceneGraphNode();
    /// the destructor
    ~nSceneGraphNode();

    /// clear contents
    void Clear();

    /// set the parent node
    void SetParent(nSceneGraphNode* p);
    /// get the parent node
    nSceneGraphNode* GetParent();

    /// set transparency flag (important for sorting)
    void SetOpaque(bool);
    /// get transparency flag
    bool GetOpaque();

    /// set the render priority
    void SetRenderPri(short);
    /// get the render priority
    short GetRenderPri();

    /// set the render context
    void SetRenderContext(int);
    /// get the render context
    int GetRenderContext();

    /// set the transformation matrix
    void SetMatrix44(const matrix44& m);
    /// get the transformation matrix
    const matrix44& GetMatrix44();

    /// set the channel context
    void SetChannelContext(nChannelContext*);
    /// get the channel context
    nChannelContext* GetChannelContext();

    /// set the vertex buffer object (for minimal state switch sorting)
    void SetVertexBuffer(nVertexBuffer* o);
    /// get the vertex buffer object
    nVertexBuffer* GetVertexBuffer();
    /// set the pixel shader object (for minimal state switch sorting)
    void SetPixelShader(nPixelShader* o);
    /// get the pixel shader object
    nPixelShader* GetPixelShader();
    /// set the texture array object (for minimal state switch sorting)
    void SetTextureArray(nTextureArray* o);
    /// get the texture array object
    nTextureArray* GetTextureArray();
    /// set the light object (must be rendered before geometric nodes)
    void SetLight(nLight* o);
    /// get the light object
    nLight* GetLight();
    
    /// set the complex visnode object (rendering happens inside Compute())
    void SetComplex(nVisNode* o);
    /// get the complex visnode object
    nVisNode* GetComplex();

private:
};




