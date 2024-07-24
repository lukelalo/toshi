#ifndef N_SCENEGRAPH2_H
#define N_SCENEGRAPH2_H
//------------------------------------------------------------------------------
/**
    @class nSceneGraph2

    @brief Manage the Nebula scene graph.

    nSceneGraph2 is a cleaner reimplementation of nSceneGraph, the old
    Nebula scene graph object, featuring a cleaner and more abstract
    interface (no more nSGNode's, no more animation channel handling
    in the scene graph, that's now in nChannelServer).

    Rendering nVisNode hierarchies through the scene graph happens in 
    2 passes. The first pass is the Attach() pass. This is initiated
    by invoking the Attach() method on the root object of the 
    nVisNode hierarchy to render. The Attach() pass simply "attaches"
    the objects to the scene, without actually rendering them. Rendering
    the scene happens inside nSceneGraph2::EndScene(). In EndScene() the
    scene graph object has a complete description of the scene. The first
    thing it will do is to sort the internal tree of scene graph nodes
    for minimal render state switches, then it will invoke the Compute()
    methods on the collected nVisNode objects. Inside Compute(), each 
    nVisNode object should update its state (animations, etc) and
    then render itself. 
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nSceneGraph2
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nVertexBuffer;
class nIndexBuffer;
class nPixelShader;
class nTextureArray;
class nLight;
class nChannelServer;
class nChannelContext;
class nAudioServer2;
class nShadowCaster;
class nShadowServer;
class N_PUBLIC nSceneGraph2 : public nRoot
{
public:
    /// default constructor.
    nSceneGraph2();
    /// destructor.
    virtual ~nSceneGraph2();

    /// begin a scene
    bool BeginScene(const matrix44& invViewer);
    /// attach a visnode hierarchy to the scene
    bool Attach(nVisNode* visNode, int renderContext);
    /// end a scene
    void EndScene(bool doRender);

    /// begin a new node
    bool BeginNode();
    /// end a node
    void EndNode();

    /// enable/disable texture mipmap LOD handling
    void SetEnableMipLod(bool b);
    /// get texture mipmap LOD handling
    bool GetEnableMipLod();
    /// begin texture lod table config
    void BeginMipLod(int numLevels);
    /// define a texture lod table entry
    void SetMipLod(int index, float dist);
    /// finish texture lod table config
    void EndMipLod();

    // NOTE: the attach methods are called during
    // the Attach() pass by the specific nVisNode subclasses

    /// set current node's transform node
    void AttachTransformNode(nVisNode*);
    /// set current node's mesh node
    void AttachMeshNode(nVisNode*);
    /// set current node's shader
    void AttachShaderNode(nVisNode*);
    /// set current node's texture
    void AttachTexArrayNode(nVisNode*);
    /// set current node's light
    void AttachLightNode(nVisNode*);
    /// set current node's sound emitter
    void AttachSoundNode(nVisNode*);
    /// set current node's complex node
    void AttachVisualNode(nVisNode*);
    /// set ambient fog
    void AttachAmbientFogNode(nVisNode*);
    /// set ambient audio listener
    void AttachAmbientListenerNode(nVisNode* node);
    /// set opaqueness flag
    void AttachOpaqueness(bool);
    /// set render pri
    void AttachRenderPri(short);

    // NOTE: the following methods are called
    // from inside the various nVisNode::Compute()
    // methods

    /// set the current channel context
    void SetChannelContext(nChannelContext* ctx);
    /// get the current channel context
    nChannelContext* GetChannelContext();

    /// set the current transform
    void SetTransform(const matrix44&);
    /// get the current node's transform
    const matrix44& GetTransform();
    /// get the current node's parent transform
    const matrix44& GetParentTransform();
    /// get the render context of the current node
    int GetRenderContext();
    /// set the current vertex buffer
    void SetVertexBuffer(nVertexBuffer*);
    /// get the current vertex buffer
    nVertexBuffer* GetVertexBuffer();
    /// set the current index buffer
    void SetIndexBuffer(nIndexBuffer*);
    /// get the current index buffer
    nIndexBuffer* GetIndexBuffer();
    /// set the current texture array
    void SetTextureArray(nTextureArray*);
    /// get the current texture array
    nTextureArray* GetTextureArray();
    /// set the current pixel shader
    void SetPixelShader(nPixelShader*);
    /// get the current pixel shader
    nPixelShader* GetPixelShader();
    /// set the current light
    void SetLight(nLight*);
    /// get the current light
    nLight* GetLight();
    /// set the current shadow caster object
    void SetShadowCaster(nShadowCaster*);
    /// get the current shadow caster object
    nShadowCaster* GetShadowCaster();

    /// nKernelServer dependency.
    static nKernelServer* kernelServer;

private:
    /// allocate and initialize a new node from the node pool
    short NewNodeFromPool(short parentNode);
    /// reset the node pool (in BeginScene())
    void ResetNodePool();
    /// push a node on the hierarchy stack
    bool PushNode(short);
    /// pop a node from the hierarchy stack
    short PopNode();
    /// node stack empty?
    bool NodeStackEmpty();
    /// render the scene (called in EndScene())
    void RenderScene();
    /// split nodes by type
    void TransformAndSplitNodes();
    /// render light nodes
    void RenderLights(nGfxServer *gfxServ, nShadowServer* shadowServ);
    /// render sound nodes (and the listener)
    void RenderSounds(nAudioServer2 *audioServ);
    /// sort geometry nodes
    void SortGeoNodes();
    /// reset mipmap levels before doing AdjustMipLevel()
    void ResetMipLevel(nTextureArray* texArray);
    /// compute the max mipmap level for a vertexbuffer/texture combination
    void AdjustMipLevel(const matrix44& modelView, nTextureArray* texArray, int mipLodBias);

    enum {
        MAX_NODES      = 2048,          // max number of scene graph nodes
        MAX_GEONODES   = 2048,          // max number of geometry nodes
        MAX_LIGHTNODES = 64,            // max number of light nodes
        MAX_SOUNDNODES = 128,           // max number of sound emitter nodes
        MAX_STACKDEPTH = 256,           // max depth of hierarchy stack
    };

    // internal super-private ugly stuff
public:
    struct nSceneNode
    {
        enum
        {
            ISOPAQUE = (1<<0),
            ISSHADOWCASTER = (1<<1),
        };
        short               parentNode;
        short               renderPri;
        unsigned char       flags;
        int                 renderContext;
        nChannelContext*    channelContext;
        matrix44            matrix;
        union {
            nVisNode*       meshNode;
            nVertexBuffer*  vertexBuffer;
        };
        union {
            nVisNode*       shaderNode;
            nPixelShader*   pixelShader;
        };
        union {
            nVisNode*       texArrayNode;
            nTextureArray*  texArray;
        };
        union {
            nVisNode*       lightNode;
            nLight*         light;
        };
        nVisNode*   soundNode;
        nVisNode*   visualNode;
        nIndexBuffer* indexBuffer;
        nShadowCaster* shadowCaster;
    };
    static nSceneNode* uglyNodePoolPtr;

private:
    nAutoRef<nChannelServer> refChnServer;  // auto ref to channel server
    nAutoRef<nGfxServer>     refGfx;        // auto ref to gfx server
    nAutoRef<nAudioServer2>  refAudio;      // auto ref to audio server
    nAutoRef<nShadowServer>  refShadow;     // auto ref to shadow server
    bool inBeginScene;
    int curRenderContext;                   // current render context set by Attach()

    nVisNode* ambientFog;               // the ambient fog for this scene
    nVisNode* ambientListener;          // the audio listener for this scene

    short ambientFogNode;               // scene graph node for ambient fog
    short ambientListenerNode;          // scene graph node for ambient listener

    short curNode;                      // current scene graph node index
    short numGeoNodes;                  // number of indices in geoNodes array
    short numLightNodes;                // number of indices in lightNodes array
    short numSoundNodes;                // number of indices in soundNodes array

    short geoNodes[MAX_GEONODES];       // indices of nodes containing geometry
    short lightNodes[MAX_LIGHTNODES];   // indices of nodes containing lights
    short soundNodes[MAX_SOUNDNODES];   // indices of nodes containing sounds

    // texture mipmap level of detail
    bool mipLodEnabled;
    int numMipLods;
    float *mipLodTable;

    // the hierarchy stack
    short topOfStack;
    short nodeStack[MAX_STACKDEPTH];    // node hierarchy stack

    // the node pool
    short curNodePoolIndex;
    nSceneNode nodePool[MAX_NODES];

};

//------------------------------------------------------------------------------
/**
    Resets the node pool, simply rewinds the curNodePoolIndex to 0.
*/
inline
void
nSceneGraph2::ResetNodePool()
{
    this->curNodePoolIndex = 0;
}

//------------------------------------------------------------------------------
/**
    Allocates a new node from the node pool and initializes. Does NOT make
    it the current node. Returns -1 if node pool is exhausted. If successful,
    the returned short is the index into the nodePool.

    @param      parentNode  the parent node index
    @return                 new node index, -1 if no more nodes in pool
*/
inline
short
nSceneGraph2::NewNodeFromPool(short parentNode)
{
    if (this->curNodePoolIndex >= MAX_NODES)
    {
        return -1;
    }
    else
    {
        nSceneNode& node = this->nodePool[this->curNodePoolIndex];
        memset(&node, 0, sizeof(nSceneNode));
        node.parentNode = parentNode;
        return this->curNodePoolIndex++;
    }
}

//------------------------------------------------------------------------------
/**
    Push node on node pool stack. Return false if stack overflow.

    @param  nodeIndex   node index to push on stack
    @return             true if successful, false if stack overflow
*/
inline
bool
nSceneGraph2::PushNode(short nodeIndex)
{
    if (this->topOfStack >= MAX_STACKDEPTH)
    {
        return false;
    }
    else
    {
        this->nodeStack[this->topOfStack++] = nodeIndex;
        return true;
    }
}

//------------------------------------------------------------------------------
/**
    Pop node from pool stack. Fail hard if stack underflow.

    @return         popped node index
*/
inline
short
nSceneGraph2::PopNode()
{
    n_assert(this->topOfStack > 0);
    return this->nodeStack[--this->topOfStack];
}

//------------------------------------------------------------------------------
/**
    Return true if node stack is empty.

    @return     true if node stack is empty.
*/
inline
bool
nSceneGraph2::NodeStackEmpty()
{
    return (this->topOfStack == 0);
}

//------------------------------------------------------------------------------
/**
    Attaches a n3DNode to the currently active scene graph node. The method
    will immediately call back n3DNode::Compute(), which in turn should
    evaluate its current matrix and set it via nSceneGraph2::SetTransform().
    
    @param  node    pointer to a n3DNode object
*/
inline
void
nSceneGraph2::AttachTransformNode(nVisNode* node)
{
    n_assert(node);
    node->Compute(this);
}

//------------------------------------------------------------------------------
/**
    Attaches a nMeshNode to the currently active scene graph node. The
    object pointer will be stored in the scene graph node until its
    Compute() method will be called during nSceneGraph2::EndScene().
    nMeshNode::Compute() is expected to update its internal state 
    and invoke nSceneGraph2::SetVertexBuffer().

    @param  node    pointer to a nMeshNode object
*/
inline
void
nSceneGraph2::AttachMeshNode(nVisNode* node)
{
    n_assert(node);
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].meshNode = node;    
}

//------------------------------------------------------------------------------
/**
    Attaches a shader node to the currently active scene graph node.
    The object pointer will be stored in the scene graph node until
    its Compute() method will be called during nSceneGraph2::EndScene().
    nShaderNode::Compute() is expected to update its internal state
    and invoke nSceneGraph2::SetPixelShader().

    @param  node    pointer to a nShaderNode object
*/
inline
void
nSceneGraph2::AttachShaderNode(nVisNode* node)
{
    n_assert(node);
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].shaderNode = node;
}

//------------------------------------------------------------------------------
/**
    Attaches a texture array node to the currently active scene graph node.
    The object pointer will be stored in the scene graph node until
    its Compute() method will be called during nSceneGraph2::EndScene().
    nTextureArrayNode::Compute() is expected to update its internal state
    and invoke nSceneGraph2::SetTextureArray().

    @param  node    pointer to a nTextureArrayNode object
*/
inline
void
nSceneGraph2::AttachTexArrayNode(nVisNode* node)
{
    n_assert(node);
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].texArrayNode = node;
}

//------------------------------------------------------------------------------
/**
    Attaches a light node to the currently active scene graph node.
    The object pointer will be stored in the scene graph until
    its Compute() method will be called during nSceneGraph2::EndScene().
    nLightNode::Compute() is expected to update its internal state
    and call nSceneGraph2::SetLight().

    @param  node    pointer to nLightNode object
*/
inline
void
nSceneGraph2::AttachLightNode(nVisNode* node)
{
    n_assert(node);
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].lightNode = node;
}

//------------------------------------------------------------------------------
/**
    Attaches a sound node to the currently active scene graph node.
    The object pointer will be stored in the scene graph until 
    its Compute() method will be called during nSceneGraph2::EndScene().
    nSoundNode::Compute() is expected to update its internal state and
    directly make itself audible.
    
    @param  node    pointer to nSoundNode object
*/
inline
void
nSceneGraph2::AttachSoundNode(nVisNode* node)
{
    n_assert(node);
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].soundNode = node;
}

//------------------------------------------------------------------------------
/**
    Attaches a visual node to the currently active scene graph node.
    The object pointer will be stored in the scene graph until 
    its Compute() method will be called during nSceneGraph2::EndScene().
    The Compute() method of the visual node is expected to update its
    internal state and make itself visible.

    @param  node    pointer to nVisNode subclass object
*/
inline
void
nSceneGraph2::AttachVisualNode(nVisNode* node)
{
    n_assert(node);
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].visualNode = node;
}

//------------------------------------------------------------------------------
/**
    Attaches an opaqueness flag to the currently active scene graph node.
    The opaqueness state is normally set by nShaderNode objects during
    their Attach() method and says whether the object is visually opaque
    or not. This is important for the nSceneGraph2 object for its sorting
    pass.

    @param  opaq    true if object is opaque
*/
inline
void
nSceneGraph2::AttachOpaqueness(bool opaq)
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    if (opaq)
    {
        this->nodePool[this->curNode].flags |= nSceneNode::ISOPAQUE;
    }
    else
    {
        this->nodePool[this->curNode].flags &= ~nSceneNode::ISOPAQUE;
    }
}

//------------------------------------------------------------------------------
/**
    Attaches a render priority the the currently active scene graph node.
    The render priority is normally set by nShaderNode object during
    their Attach(). Render priority is important for the nSceneGraph2 object
    for its sorting pass.

    @param  pri     render priority (can be negative, default should be 0)
*/
inline
void
nSceneGraph2::AttachRenderPri(short pri)
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].renderPri = pri;
}

//------------------------------------------------------------------------------
/**
    Attaches an ambient fog node to the scene. The pointer to the fog node
    will be stored an later during EndScene() the nFogNode::Compute() method 
    will be called which is expected to set the necessary render states. Multiple
    fog nodes will overwrite each other.

    @param  node    pointer to nFogNode object
*/
inline
void
nSceneGraph2::AttachAmbientFogNode(nVisNode* node)
{
    n_assert(node);
    this->ambientFogNode = this->curNode;
    this->ambientFog = node;
}

//------------------------------------------------------------------------------
/**
    Attaches an ambient audio listener to the scene. The pointer to the
    listener node will be stored and later during EndScene() the 
    nListenerNode::Compute() method will be called which is expected to
    declare itself as listener in the audio subsystem. Multiple listeners
    will overwrite each other.

    @param  node    pointer to nListenerNode object
*/
inline
void
nSceneGraph2::AttachAmbientListenerNode(nVisNode* node)
{
    n_assert(node);
    this->ambientListenerNode = this->curNode;
    this->ambientListener = node;
}

//------------------------------------------------------------------------------
/**
    Set the transformation matrix for the current scene graph node. This is 
    normally done inside n3DNode::Compute().

    @param  matrix  a 4x4 matrix
*/
inline
void
nSceneGraph2::SetTransform(const matrix44& matrix)
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].matrix = matrix;
}

//------------------------------------------------------------------------------
/**
    Get the transformation for the current scene graph node.

    @return         a const ref to the current node's 4x4 matrix
*/
inline
const matrix44&
nSceneGraph2::GetTransform()
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    return this->nodePool[this->curNode].matrix;
}

//------------------------------------------------------------------------------
/**
    Get the parent node's transform.

    @return     the parent node's transform
*/
inline
const matrix44&
nSceneGraph2::GetParentTransform()
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    nSceneNode& node = this->nodePool[this->curNode];
    n_assert((node.parentNode >= 0) && (this->curNode < MAX_NODES));
    return this->nodePool[node.parentNode].matrix;
}

//------------------------------------------------------------------------------
/**
    Get render context for this node, as defined by the Attach() method.

    @return     the render context
*/
inline
int
nSceneGraph2::GetRenderContext()
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    return this->nodePool[this->curNode].renderContext;
}

//------------------------------------------------------------------------------
/**
    Set the vertex buffer for the current scene graph node. This is normally
    done inside nMeshNode::Compute().

    @param  vertexBuffer    pointer to nVertexBuffer object (can be 0)
*/
inline
void
nSceneGraph2::SetVertexBuffer(nVertexBuffer* vertexBuffer)
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].vertexBuffer = vertexBuffer;
}

//------------------------------------------------------------------------------
/**
    Get vertex buffer of the current scene graph node.

    @return     pointer to nVertexBuffer object (can be 0)
*/
inline
nVertexBuffer*
nSceneGraph2::GetVertexBuffer()
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    return this->nodePool[this->curNode].vertexBuffer;
}

//------------------------------------------------------------------------------
/**
    Set the index buffer for the current scene graph node. This is normally
    done inside nMeshNode::Compute().
    
    @param  indexBuffer     pointer to nIndexBuffer object (can be 0)
*/
inline
void
nSceneGraph2::SetIndexBuffer(nIndexBuffer* indexBuffer)
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].indexBuffer = indexBuffer;
}

//------------------------------------------------------------------------------
/**
    @return         the current index buffer
*/
inline
nIndexBuffer*
nSceneGraph2::GetIndexBuffer()
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    return this->nodePool[this->curNode].indexBuffer;
}

//------------------------------------------------------------------------------
/**
    Set the texture array object for the current scene graph node. This
    is normally done inside nTextureArrayNode::Compute().

    @param  texArray        pointer to nTextureArray object (can be 0)
*/
inline
void
nSceneGraph2::SetTextureArray(nTextureArray* texArray)
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].texArray = texArray;
}

//------------------------------------------------------------------------------
/**
    @return         the current texture array object
*/
inline
nTextureArray*
nSceneGraph2::GetTextureArray()
{
    return this->nodePool[this->curNode].texArray;
}

//------------------------------------------------------------------------------
/**
    Set the pixel shader for the current scene graph node. This
    is normally done inside nShaderNode::Compute().

    @param  pixelShader pointer to nPixelShader object (can be 0)
*/
inline
void
nSceneGraph2::SetPixelShader(nPixelShader* pixelShader)
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].pixelShader = pixelShader;
}

//------------------------------------------------------------------------------
/**
    @return         the current pixel shader
*/
inline
nPixelShader*
nSceneGraph2::GetPixelShader()
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    return this->nodePool[this->curNode].pixelShader;
}

//------------------------------------------------------------------------------
/**
    Set the light object for the current scene graph node. This is
    normally done inside nLightNode::Compute().

    @param  light       pointer to nLight object (can be 0)
*/
inline
void
nSceneGraph2::SetLight(nLight* light)
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].light = light;
}

//------------------------------------------------------------------------------
/**
    @return         the current pixel shader
*/
inline
nLight*
nSceneGraph2::GetLight()
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    return this->nodePool[this->curNode].light;
}

//------------------------------------------------------------------------------
/**
    Set the shadow caster for this node.

    @param  caster      pointer to nShadowCaster object
*/
inline
void
nSceneGraph2::SetShadowCaster(nShadowCaster* caster)
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].shadowCaster = caster;
}

//------------------------------------------------------------------------------
/**
    Get the current shadow caster.

    @return     pointer to current shadow caster object or 0
*/
inline
nShadowCaster*
nSceneGraph2::GetShadowCaster()
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    return this->nodePool[this->curNode].shadowCaster;
}

//------------------------------------------------------------------------------
/**
    Set the channel context for the current scene graph node.

    @param  ctx     pointer to a channel context
*/
inline
void
nSceneGraph2::SetChannelContext(nChannelContext* ctx)
{
    n_assert(ctx);
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    this->nodePool[this->curNode].channelContext = ctx;
}

//------------------------------------------------------------------------------
/**
    Get the channel context of the current scene graph node.

    @return         current channel context (can be 0)
*/
inline
nChannelContext*
nSceneGraph2::GetChannelContext()
{
    n_assert((this->curNode >= 0) && (this->curNode < MAX_NODES));
    return this->nodePool[this->curNode].channelContext;
}

//------------------------------------------------------------------------------
#endif
