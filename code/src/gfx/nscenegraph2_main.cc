#define N_IMPLEMENTS nSceneGraph2
//------------------------------------------------------------------------------
//  nscenegraph2_main.cc
//  (C) 2001 A.Weissflog
//------------------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelserver.h"

nNebulaScriptClass(nSceneGraph2, "nroot");

// this is a hack to make qsort() work
nSceneGraph2::nSceneNode* nSceneGraph2::uglyNodePoolPtr = 0;

//------------------------------------------------------------------------------
/**
    @brief Construct a nSceneGraph2 object.
*/
nSceneGraph2::nSceneGraph2() :
    refChnServer(kernelServer, this),
    refGfx(kernelServer, this),
    refAudio(kernelServer, this),
    refShadow(kernelServer, this),
    inBeginScene(false),
    curRenderContext(0),
    ambientFog(0),
    ambientListener(0),
    curNode(-1),
    numGeoNodes(0),
    numLightNodes(0),
    numSoundNodes(0),
    topOfStack(0),
    curNodePoolIndex(0),
    numMipLods(0),
    mipLodTable(0),
    mipLodEnabled(false)
{
    this->refGfx       = "/sys/servers/gfx";
    this->refChnServer = "/sys/servers/channel";
    this->refAudio     = "/sys/servers/audio";
    this->refShadow    = "/sys/servers/shadow";
    memset(this->geoNodes,      0, sizeof(this->geoNodes));
    memset(this->lightNodes,    0, sizeof(this->lightNodes));
    memset(this->soundNodes,    0, sizeof(this->soundNodes));
    memset(this->nodeStack,     0, sizeof(this->nodeStack));
    memset(this->nodePool,      0, sizeof(this->nodePool));
}

//------------------------------------------------------------------------------
/**
    @brief Destruct a nSceneGraph2 object.
*/
nSceneGraph2::~nSceneGraph2()
{ 
    if (this->mipLodTable)
    {
        delete[] this->mipLodTable;
        this->mipLodTable = 0;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Begin a new scene graph frame.
    
    Takes the inverse of the current view matrix as the root of
    all transformations.

    @param  invViewer   inverse of current viewer matrix
    @return             false: ooops...
*/
bool
nSceneGraph2::BeginScene(const matrix44& invViewer)
{
    n_assert(!this->inBeginScene);

    // invoke BeginScene() on channel server
    this->refChnServer->BeginScene();

    this->inBeginScene = true;

    // reset per-frame variables
    this->ambientFog      = 0;
    this->ambientListener = 0;
    this->curNode         = -1;
    this->numGeoNodes     = 0;
    this->numLightNodes   = 0;
    this->numSoundNodes   = 0;

    // reset node pool and assert that stack is empty
    this->ResetNodePool();
    n_assert(this->NodeStackEmpty());

    // create the "root node" and set its transform to the inverse viewer matrix
    this->BeginNode();
    this->SetTransform(invViewer);

    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Attach a nVisNode hierarchy to the scene graph.
    
    This will simply invoke Attach() on the top nVisNode object in
    the hierarchy (should be a n3DNode). The optional render context
    key is used to distinguish between identical visnodes that are
    rendered multiple times per frame. This context key is only
    necessary for a few subsystems (like the particle server).

    @param  visNode         pointer to a nVisNode object
    @param  renderContext   optional unique "renderContext" key
    @return                 true if all ok, false: don't attach any more objects!
*/
bool
nSceneGraph2::Attach(nVisNode* visNode, int renderContext)
{
    n_assert(visNode);
    this->curRenderContext = renderContext;
    return visNode->Attach(this);
}

//------------------------------------------------------------------------------
/**
    @brief Finish a scene graph scene.
    
    This is where the interesting stuff happens. The attached scene
    graph nodes are sorted to ensure minimal render state switches,
    and then rendered.
*/
void
nSceneGraph2::EndScene(bool doRender)
{
    n_assert(this->inBeginScene);

    // finish the root node and check stack
    this->EndNode();
    n_assert(this->NodeStackEmpty());
    n_assert(-1 == this->curNode);

    // finally render the scene
    if (doRender)
    {
        this->RenderScene();
    }
    this->inBeginScene = false;

    // invoke EndScene() on channel server
    this->refChnServer->EndScene();
}

//------------------------------------------------------------------------------
/**
    Pull a new scene graph node from the pool, initialize it and make
    it the current scene graph node. This is normally done by a n3DNode object
    (other scene graph packages would do this in a group node). After this,
    nVisNode subclasses should fill the scene graph node by calling the
    nSceneGraph2::Attach*() methods, this is also called the Attach pass.
    If the method returns 'false', internal static pools are exhausted
    (either the node hierarchy stack, or the node pool). No new objects
    should be attached in this case, and nSceneGraph2::EndNode() should
    NOT be called.

    @return     false if static pool exhausted or hierarchy stack overflow
*/
bool
nSceneGraph2::BeginNode()
{
    n_assert(this->inBeginScene);

    // push current node on node stack (if there's actually something to push!)
    // NOTE: it is not a bug to push the "-1" node on the stack
    if (!this->PushNode(this->curNode))
    {
        // oops, stack overflow
        n_printf("nSceneGraph2: *** WARNING *** node stack overflow, 3d hierarchy too deep!\n");
        return false;
    }

    // get new node and make current
    this->curNode = this->NewNodeFromPool(this->curNode);
    if (-1 == this->curNode)
    {
        // oops, no more nodes in pool
        n_printf("nSceneGraph2: *** WARNING *** too many nodes in scene\n");
        this->curNode = this->PopNode();
        return false;
    }

    // set the render context
    this->nodePool[this->curNode].renderContext = this->curRenderContext;

    return true;
}

//------------------------------------------------------------------------------
/**
    Finish the attach pass for a node. Makes the previous scene graph node
    current by popping it from the internal node stack.
*/
void
nSceneGraph2::EndNode()
{
    n_assert(this->inBeginScene);

    // pop node from node stack and make current
    this->curNode = this->PopNode();
}

//------------------------------------------------------------------------------
