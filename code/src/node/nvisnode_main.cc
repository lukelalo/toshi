#define N_IMPLEMENTS nVisNode
//------------------------------------------------------------------------------
//  nvisnode_main.cc
//  (C) 2001 Andre Weissflog
//------------------------------------------------------------------------------
#include "node/nvisnode.h"
#include "kernel/nkernelserver.h"
#include "gfx/nchannelset.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelcontext.h"

nNebulaScriptClass(nVisNode, "nroot");

//------------------------------------------------------------------------------
/**
*/
nVisNode::nVisNode() :
    finishedAfter(0.0f),
    chnSet(0),
    chnSetDirty(true),
    refGfx(kernelServer, this),
    refChannel(kernelServer, this),
    dependNode(this)
{
    this->visnodeClass  = kernelServer->FindClass("nvisnode");
    this->refGfx     = "/sys/servers/gfx";
    this->refChannel = "/sys/servers/channel";
    n_assert(this->visnodeClass);
}

//------------------------------------------------------------------------------
/**
*/
nVisNode::~nVisNode()
{
    // clear depend list
    while (this->dependList.RemHead());

    // if I'm myself a depend node, unlink me
    if (this->dependNode.IsLinked())
    {
        this->dependNode.Remove();
    }

    // free nChannelSet object?
    if (this->chnSet)
    {
        delete this->chnSet;
        this->chnSet = 0;
    }
}

//------------------------------------------------------------------------------
/**
    This is only here to have a common calling point for subclasses.
*/
void 
nVisNode::Initialize(void)
{
    // force update of hierarchy channel set
    this->NotifyChannelSetDirty();

    // hand initialize up to nRoot class
    nRoot::Initialize();
}

//------------------------------------------------------------------------------
/**
    Attach a new depend object to my depend list. It shall be defined that
    these are the object which my own state depends on, thus, to update
    my own state inside Compute(), I have to invoke Compute() on my
    depend objects first, because they may alter my state.

    @param  dependObject    pointer to nVisNode object I'm depending on
*/
void
nVisNode::DependsOn(nVisNode* dependObject)
{
    n_assert(dependObject);
    n_assert(!dependObject->dependNode.IsLinked());
    
    this->dependList.AddTail(&(dependObject->dependNode));
}

//------------------------------------------------------------------------------
/**
    This method can be invoked on a root object of a nVisNode hierarchy
    to force the whole nVisNode hierarchy to preload any resources
    (like textures, meshes, etc...). If preload is not used, resources
    are loaded when the object is first rendered, which may lead to frame
    stuttering.
*/
void
nVisNode::Preload()
{
    // recursively call preload on child object
    nVisNode* child;
    for (child = (nVisNode*) this->GetHead(); child; child = (nVisNode*) child->GetSucc())
    {
        if (child->IsA(this->visnodeClass))
        {
            child->Preload();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Check whether the channel set is dirty. Subclasses may override this
    method if they want to do something funky (this is currently only
    used by the nLinkNode class).

    @return     true if channel set is dirty
*/
bool
nVisNode::IsChannelSetDirty()
{
    return this->chnSetDirty;
}

//------------------------------------------------------------------------------
/**
    Get the up-to-date channel set for my visnode hierarchy (including me).
    If there is no nChannelSet object created yet, do it. When created, or
    when the channelset-dirty flag is set, do a roundtrip through all children
    and ask them to announce the channels they are interested in for reading
    or writing via the AttachChannels() method.
 
    @return     pointer to uptodate nChannelSet object

    06-Aug-01   floh    channel set no longer deleted, hopefully this fixes
                        a hard to reproduce assertion in the nChannelSet destructor
*/
nChannelSet*
nVisNode::GetChannelSet()
{
    // create a channel set on demand
    if (!this->chnSet)
    {
        this->chnSet = new nChannelSet;
    }

    // if we are dirty, clear the old channel set and collect channels
    if (this->IsChannelSetDirty())
    {
        this->chnSet->Clear();
        this->AttachChannels(this->chnSet);
        this->chnSetDirty = false;
    }
    return this->chnSet;
}

//------------------------------------------------------------------------------
/**
    Attach channels for reading or writing to the channel set object. Subclasses
    should override this method if they need read or write access to channels
    and simply hand the method to their parent class. On the nVisNode level,
    the method will be recursively called on the children, so that at the end
    of the roundtrip nChannelSet contains all channels a complete nVisNode 
    hierarchy requires.

    @param  channelSet  the channel set to attach channels to
*/
void
nVisNode::AttachChannels(nChannelSet* channelSet)
{
    n_assert(channelSet);
    n_assert(this->visnodeClass);

    // down to children...
    nVisNode* curChild;
    for (curChild = (nVisNode*) this->GetHead();
         curChild;
         curChild = (nVisNode*) curChild->GetSucc())
    {
        if (curChild->IsA(this->visnodeClass))
        {
            curChild->AttachChannels(channelSet);
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method must be called by external client objects when a
    render context has been created. nVisNode objects which need to create
    objects for each render client use this method to create new instances
    of those object.
*/
void
nVisNode::RenderContextCreated(int renderContext)
{
    // down to children...
    nVisNode* curChild;
    for (curChild = (nVisNode*) this->GetHead(); curChild; curChild = (nVisNode*) curChild->GetSucc())
    {
        if (curChild->IsA(this->visnodeClass))
        {
            curChild->RenderContextCreated(renderContext);
        }
    }
}

//------------------------------------------------------------------------------
/**
    See OnRenderContextCreated().
*/
void
nVisNode::RenderContextDestroyed(int renderContext)
{
    // down to children...
    nVisNode* curChild;
    for (curChild = (nVisNode*) this->GetHead(); curChild; curChild = (nVisNode*) curChild->GetSucc())
    {
        if (curChild->IsA(this->visnodeClass))
        {
            curChild->RenderContextDestroyed(renderContext);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Attach myself to the scene graph. The important work actually happens
    in the subclasses. On nVisNode level, the method is just distributed
    to my child objects recursively. Check out n3DNode::Attach() for the 
    important stuff.

    @param  sceneGraph      pointer to scene graph object I shall attach to
    @return                 true if success

    11-Apr-02   floh    lod channel must now be provided externally by 
                        application
*/
bool
nVisNode::Attach(nSceneGraph2* sceneGraph)
{
    nVisNode* curChild = (nVisNode*) this->GetHead();
    if (curChild)
    {
        // distribute Attach() to child objects
        for (; curChild; curChild = (nVisNode*) curChild->GetSucc())
        {
            if (curChild->IsA(this->visnodeClass))
            {
                curChild->Attach(sceneGraph);
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Update my own internal state and optionally make myself visible.
    My own internal state actually depends on my depend nodes (they are
    allowed to manipulate me). Thus Compute() is first invoked on
    all the objects in the depend list. The scene graph has stored our 
    render context as a internal scene graph node, which is guaranteed
    to be current when Compute() is called upon us.

    @param  sceneGraph      scene 

    11-Apr-02   floh    lod channel must now be provided externally by 
                        application
*/
void
nVisNode::Compute(nSceneGraph2* sceneGraph)
{
    nNode* curNode = this->dependList.GetHead();
    if (curNode)
    {
        // invoke Compute() on our depend nodes
        for (; curNode; curNode = curNode->GetSucc())
        {
            // NOTE: we just assume that the child is a nVisNode!
            // for performance reasons
            nVisNode* curDepend = (nVisNode*) curNode->GetPtr();
            n_assert(curDepend);
            curDepend->Compute(sceneGraph);
        }
    }
}

//------------------------------------------------------------------------------

