#ifndef N_VISNODE_H
#define N_VISNODE_H
//------------------------------------------------------------------------------
/**
    @defgroup NebulaVisnodeModule Visual Classes
*/
/**
    @class nVisNode
    @ingroup NebulaVisnodeModule

    @brief super class of everything visible (and audible too!)

    nVisNode objects can be attached to the current scene by
    calling nSceneGraph2::Attach(). All the usual components of
    3d objects (transforms, textures, shaders, meshes) exist as
    subclasses of nVisNode. nVisNode subclass objects can be arranged
    into complex hierarchies with animations and all bells and whistles.

    (C) 2001 A.Weissflog
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nVisNode
#include "kernel/ndefdllclass.h"
//------------------------------------------------------------------------------
class nSceneGraph2;
class nPersistServer;
class nChannelSet;
class nGfxServer;
class nChannelServer;
class N_PUBLIC nVisNode : public nRoot
{
public:
    /// constructor
    nVisNode();
    /// destructor
    virtual ~nVisNode();

    /// one time initialization after linkage into name hierarchy
    virtual void Initialize();
    /// persistency
    virtual bool SaveCmds(nPersistServer* fileServer);

    /**
        @name Scene Graph and Named Object Hierarchy Interaction
    */
    //@{
    /// attach a depend node
    void DependsOn(nVisNode* dependObject);
    /// attach yourself to the scene
    virtual bool Attach(nSceneGraph2* sceneGraph);
    /// update your internal state and make yourself visible
    virtual void Compute(nSceneGraph2* sceneGraph);
    /// preload resources (textures, meshes, etc.)
    virtual void Preload();
    //@}

    /**
        @name Channel Management
    */
    //@{
    /// get the up-to-date channel set associated with this object
    nChannelSet* GetChannelSet();
    /// mark the current channel set as dirty
    void NotifyChannelSetDirty();
    /// is the channel set dirty?
    virtual bool IsChannelSetDirty();
    /// collect required channels through nVisNode hierarchie
    virtual void AttachChannels(nChannelSet*);
    //@}

    /// set a finished after timeout (okok, this is Nomads specific!)
    void SetFinishedAfter(float time);
    /// get a finished after timeout
    float GetFinishedAfter();

    /**
        @name Render Context Management

        For some discussion of render contexts, see
        http://nebuladevice.sourceforge.net/cgi-bin/twiki/view/Nebula/RenderContextManagement
    */
    //@{
    /// called by client when rendercontext object has been created
    virtual void RenderContextCreated(int renderContext);
    /// called by client when rendercontext object has been destroyed
    virtual void RenderContextDestroyed(int renderContext);
    //@}

    /// nKernelServer dependency.
    static nKernelServer* kernelServer;

protected:
    // most nVisNode subclasses will require access to the following servers:
    nAutoRef<nGfxServer> refGfx;
    nAutoRef<nChannelServer> refChannel;
    /// list of objects I'm depending on
    nList dependList;
    /// node for linkage into dependList
    nNode dependNode;
    /// finished after time
    float finishedAfter;
    /// handle to nVisNode class object
    nClass* visnodeClass;
    /// channel set object for channel context allocation
    nChannelSet* chnSet;
    /// channel set dirty?
    bool chnSetDirty;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nVisNode::NotifyChannelSetDirty()
{
    // only care about the channel dirty flag if we own a channel
    if (this->chnSet)
    {
        chnSetDirty = true;
    }
    // hand the channel dirty flag up to parent object
    nVisNode* parentObj = (nVisNode*) this->GetParent();
    if (parentObj && parentObj->IsA(this->visnodeClass))
    {
        parentObj->NotifyChannelSetDirty();
    }

    // also notify all objects holding a ref to me, which are
    // of class nVisNode, this is necessary for nLinkNodes to
    // work correctly
    nList* refList = this->GetRefs();
    nRef<nRoot>* curRef;
    for (curRef = (nRef<nRoot>*) refList->GetHead(); 
         curRef; 
         curRef = (nRef<nRoot>*) curRef->GetSucc())
    {
        nVisNode* owner = (nVisNode*) curRef->getowner();
        if (owner && owner->IsA(this->visnodeClass))
        {
            owner->NotifyChannelSetDirty();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVisNode::SetFinishedAfter(float time)
{
    this->finishedAfter = time;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nVisNode::GetFinishedAfter()
{
    return this->finishedAfter;
}

//------------------------------------------------------------------------------
#endif

