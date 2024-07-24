#ifndef N_CHARACTERNODE_H
#define N_CHARACTERNODE_H
//------------------------------------------------------------------------------
/**
    @class nCharacterNode
    @ingroup NebulaVisnodeModule
    
    @brief Encapsulate a character object into a nVisNode.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#ifndef N_ANIMNODE_H
#include "node/nanimnode.h"
#endif

#ifndef N_RSRCPATH_H
#include "misc/nrsrcpath.h"
#endif

#ifndef N_PRIMITIVESERVER_H
#include "gfx/nprimitiveserver.h"
#endif

#ifndef N_CHARSKELETON_H
#include "character/ncharskeleton.h"
#endif

#ifndef N_CHARSTATEARRAY_H
#include "character/ncharstatearray.h"
#endif

#ifndef N_CHARSKINRENDERER_H
#include "character/ncharskinrenderer.h"
#endif

#undef N_DEFINES
#define N_DEFINES nCharacterNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class vector3;
class quaternion;
class nAnimServer;
class nCharacterServer;
class nAnimCurveArray;
class n3DNode;

class nCharacterNode : public nAnimNode
{
public:
    /// constructor
    nCharacterNode();
    /// destructor
    virtual ~nCharacterNode();
    /// persistency
    virtual bool SaveCmds(nPersistServer* fileServer);
    /// attach yourself to the scene
    virtual bool Attach(nSceneGraph2* sceneGraph);
    /// update your internal state and make yourself visible
    virtual void Compute(nSceneGraph2* sceneGraph);

    /// collect required channels through nVisNode hierarchie
    virtual void AttachChannels(nChannelSet* chnSet);
    /// called by client when rendercontext object has been created
    virtual void RenderContextCreated(int renderContext);
    /// called by client when rendercontext object has been destroyed
    virtual void RenderContextDestroyed(int renderContext);

    /// set filename of nanim file
    void SetAnimFile(const char* name);
    /// get filename of nanim file
    const char* GetAnimFile() const;
    /// set state switch channel name
    void SetStateChannel(const char* name);
    /// get state switch channel name
    const char* GetStateChannel() const;
    /// set read only state
    void SetReadOnly(bool b); 
    /// get read only state
    bool GetReadOnly(void) const;
    /// set path to skin mesh
    void SetSkinMesh(const char* n); 
    /// get path to skin mesh
    const char* GetSkinMesh(void) const;
    /// set shadow caster state
    void SetCastShadow(bool b);
    /// get shadow caster state
    bool GetCastShadow() const;
    /// render debug visualization of skeleton
    void SetRenderSkeleton(bool b);
    /// get render debug vis state
    bool GetRenderSkeleton() const;
    /// set skeleton color, for debug visualization of skeleton
    void SetSkeletonColor(vector4& color);
    /// get skeleton color
    const vector4& GetSkeletonColor() const;

    /// begin defining joints
    void BeginJoints(int num);
    /// add a joint to the joint skeleton
    void AddJoint(int index, const char* jointName, int parentIndex, const vector3& poseTranslate, const quaternion& poseRotate);
    /// finish joints
    void EndJoints();
    /// get number of joints
    int GetNumJoints();
    /// get a joint definition
    void GetJoint(int index, const char*& jointName, int& parentIndex, vector3& poseTranslate, quaternion& poseRotate);

    /// begin states (skeleton must be complete at this point!)
    void BeginStates(int num);
    /// add a named state
    void AddState(int stateIndex, const char* name);
    /// begin adding anims to a state
    void BeginStateAnims(int stateIndex, int numAnims);
    /// add a weighted animation to a state
    void AddStateAnim(int stateIndex, int animIndex, const char* animName, const char* weightChannel);
    /// finish adding weighted anims to a state
    void EndStateAnims(int stateIndex);
    /// finish defining states
    void EndStates();
    /// get number of states
    int GetNumStates();
    /// get state definition
    void GetState(int stateIndex, const char*& name);
    /// get number of anims in a state
    int GetNumStateAnims(int stateIndex);
    /// get state anim definition
    void GetStateAnim(int stateIndex, int animIndex, const char*& animName, const char*& weightChannel);

    /// begin defining hard points
    void BeginHardPoints(int num);
    /// add a hard point
    void AddHardPoint(int index, int jointIndex, const char* n3dNodeName);
    /// finish defining hardpoints
    void EndHardPoints();
    /// get number of hard points
    int GetNumHardPoints();
    /// get definition of a hardpoint
    void GetHardPoint(int index, int& jointIndex, const char*& n3dNodeName);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:
    /// update hard point n3DNode positions
    void UpdateHardPoints(nSceneGraph2* sceneGraph, const nCharSkeleton& charSkeleton);

    /// hardpoint class, maps a joint to an external n3dnode
    class nHardPoint
    {
        friend class nCharacterNode;
        
        /// constructor
        nHardPoint();
        /// initialize object
        void Initialize(nKernelServer* kernelServer, nRoot* owner);
        /// set attributes
        void Set(int jointIndex, const char* n3dNodeName);
        /// get joint index
        int GetJointIndex() const;
        /// get n3d node name
        const char* Get3DNodeName();
        /// get pointer to n3DNode
        n3DNode* Get3DNodePtr();

        int jointIndex;
        nDynAutoRef<n3DNode> ref3DNode;
    };

    nAutoRef<nAnimServer>       refAnimServer;      // ref to anim server
    nAutoRef<nCharacterServer>  refCharServer;      // ref to char server
    nAutoRef<nGfxServer>        refGfxServer;       // ref to gfx server
    nAutoRef<nShadowServer>     refShadowServer;    // ref to shadow server
    nAutoRef<nFileServer2>      refFileServer;      // ref to file server
    nAutoRef<nPrimitiveServer>  refPrimServer;

    nRsrcPath animFilePath;                         // stores the filename of the .nanim file
    nRef<nAnimCurveArray> refCurveArray;            // the corresponding anim curve array object
    
    nCharSkeleton charSkeleton;                     // the character skeleton template
    nCharStateArray charStateArray;                 // character animation states
    nRef<nVisNode>* refStateNodes;                  // array of per state visnodes
    nCharSkinRenderer charSkinRenderer;             // renders the character skin

    int nodeKey;                                    // my half of the character id key
    nString stateChannelName;                       // name of state switch channel
    int stateChannelIndex;                          // index of state switch channel

    nDynAutoRef<nVisNode> refSkin;                  // ref to source skin mesh
    bool readOnly;                                  // read only mesh?
    bool castShadow;                                // cast shadows?
    bool renderSkeleton;                            // skeleton debug visualization?
    vector4 skeletonColor;                          // skeleton debug visualization color

    int numHardPoints;
    nHardPoint* hardPoints;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCharacterNode::nHardPoint::nHardPoint() :
    jointIndex(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacterNode::nHardPoint::Initialize(nKernelServer* kernelServer, nRoot* owner)
{
    this->ref3DNode.initialize(kernelServer, owner);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacterNode::nHardPoint::Set(int jntIndex, const char* n3DNodeName)
{
    n_assert(jntIndex >= 0);
    n_assert(n3DNodeName);
    this->ref3DNode = n3DNodeName;
    this->jointIndex = jntIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacterNode::nHardPoint::GetJointIndex() const
{
    return this->jointIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nCharacterNode::nHardPoint::Get3DNodeName()
{
    return this->ref3DNode.getname();
}

//------------------------------------------------------------------------------
/**
*/
inline
n3DNode*
nCharacterNode::nHardPoint::Get3DNodePtr()
{
    return this->ref3DNode.get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacterNode::SetReadOnly(bool b)
{
    this->readOnly = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCharacterNode::GetReadOnly() const
{
    return this->readOnly;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacterNode::SetSkinMesh(const char* n)
{
    n_assert(n);
    this->refSkin = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nCharacterNode::GetSkinMesh() const
{
    return this->refSkin.getname();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacterNode::SetCastShadow(bool b)
{
    this->castShadow = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCharacterNode::GetCastShadow() const
{
    return this->castShadow;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacterNode::SetRenderSkeleton(bool b)
{
    this->renderSkeleton = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCharacterNode::GetRenderSkeleton() const
{
    return this->renderSkeleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacterNode::SetSkeletonColor(vector4& color)
{
    this->skeletonColor = color;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nCharacterNode::GetSkeletonColor(void) const
{
    return this->skeletonColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nCharacterNode::GetAnimFile() const
{
    return this->animFilePath.GetPath();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacterNode::SetStateChannel(const char* name)
{
    n_assert(name);
    this->stateChannelName = name;
    this->stateChannelIndex = -1;
    this->NotifyChannelSetDirty();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nCharacterNode::GetStateChannel() const
{
    return this->stateChannelName.IsEmpty() ? 0 : this->stateChannelName.Get();
}

//------------------------------------------------------------------------------
#endif
