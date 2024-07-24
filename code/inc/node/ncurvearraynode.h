#ifndef N_CURVEARRAYNODE_H
#define N_CURVEARRAYNODE_H
//------------------------------------------------------------------------------
/**
    @class nCurveArrayNode
    @ingroup NebulaVisnodeModule

    @brief Encapsulate animation curves into a nVisNode object.

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_ANIMNODE_H
#include "node/nanimnode.h"
#endif

#ifndef N_ANIMCURVEARRAY_H
#include "anim/nanimcurvearray.h"
#endif

#ifndef N_ANIMCURVEBUNDLE_H
#include "anim/nanimcurvebundle.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#ifndef N_RSRCPATH_H
#include "misc/nrsrcpath.h"
#endif

#undef N_DEFINES
#define N_DEFINES nCurveArrayNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nChannelContext;
class nAnimServer;
class N_PUBLIC nCurveArrayNode : public nAnimNode
{
public:
    /// constructor
    nCurveArrayNode();
    /// destructor
    virtual ~nCurveArrayNode();
    /// initialize after linkage into name hierarchie
    virtual void Initialize(void);
    /// persistency
    virtual bool SaveCmds(nPersistServer* fileServer);
    /// attach animation channels
    virtual void AttachChannels(nChannelSet*);
    /// perform mixing
    virtual void Compute(nSceneGraph2*);

    /// set filename of anim file to load
    void SetFilename(const char* filename);
    /// get filename of anim curve file
    const char* GetFilename();

    /// begin defining curve bundles
    void BeginCurveBundles(int num);
    /// begin defining a single curve bundle
    void BeginBundle(int bundleIndex, const char* bundleName, int numCurves);
    /// add a curve to a bundle
    void SetBundleCurve(int curveIndex, const char* curveName, const char* channelName);
    /// finish defining a bundle
    void EndBundle();
    /// finish defining the curve bundles
    void EndCurveBundles();

    /// begin defining connections
    void BeginConnects(int num);
    /// define an animation curve connection
    void SetCurveConnect(int index, const char* curveName, const char* objName, const char* cmdName);
    /// define an animation curve connection with channel
    void SetCurveConnect2(int index, const char* curveName, const char* objName, const char* cmdName, const char* chnName);
    /// define an curve bundle connection
    void SetBundleConnect(int index, const char* bundleName, const char* objName, const char* cmdName);
    /// define a bundle connection with channel
    void SetBundleConnect2(int index, const char* bundleName, const char* objName, const char* cmdName, const char* chnName);
    /// finish defining connections
    void EndConnects();

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:

    /// a private class to hold curve and bundle connection info
    class nConnection
    {
        friend class nCurveArrayNode;
    
    public:
        /// connection types
        enum nConnectionType
        {
            NONE,
            CURVE,
            BUNDLE,
        };

    private:
        /// constructor
        nConnection();
        /// destructor
        ~nConnection();
        /// initialize internal ref objects
        void Initialize(nKernelServer* ks, nRoot* owner);
        /// set all params for curve type
        void SetCurve(nAnimCurve* curve, const char* objName, const char* cmdName, const char* chnName);
        /// set all params for bundle type
        void SetBundle(nAnimCurveBundle* bundle, const char* objName, const char* cmdName, const char* chnName);
        /// get the connection type
        nConnectionType GetConnectionType();
        /// get the curve 
        nAnimCurve* GetCurve();
        /// get the bundle
        nAnimCurveBundle* GetBundle();
        /// get the object name
        const char* GetObjectName();
        /// get the command name
        const char* GetCmdName();
        /// get the channel name
        const char* GetChannelName();
        /// set the channel index
        void SetChannelIndex(int i);
        /// get the channel index
        int GetChannelIndex();
        /// get the object pointer to invoke command on
        nRoot* GetObject();
        /// get the command proto for the script command
        nCmdProto* GetCmdProto();

        nConnectionType connectType;
        nDynAutoRef<nRoot> refObject;
        union
        {
            nAnimCurve* curve;
            nAnimCurveBundle* bundle;
        };
        nString cmdName;
        nString chnName;
        int chnIndex;
        nCmdProto* cmdProto;
    };

    /// animate target object with sampled curved value
    void DoCurveAnim(nConnection& con, nChannelContext* chnContext);
    /// animate target object with sampled and mixed bundle value
    void DoBundleAnim(nConnection& con, nChannelContext* chnContext);

    nAutoRef<nAnimServer> refAnim;              // animation server
    nAutoRef<nFileServer2> refFile;             // file server
    nRef<nAnimCurveArray> refCurveArray;        // an anim curve array

    int numCurveBundles;                        // number of curve bundles
    int curCurveBundle;                         // index of current bundle
    nAnimCurveBundle* curveBundles;             // array of curve bundle refs

    nRsrcPath animFilePath;
    int numConnects;                    // number of connections
    nConnection *connects;              // connections
};

//------------------------------------------------------------------------------
/**
*/
inline
nCurveArrayNode::nConnection::nConnection() :
    connectType(NONE),
    cmdProto(0)
{
    this->curve = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nCurveArrayNode::nConnection::~nConnection()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCurveArrayNode::nConnection::Initialize(nKernelServer* ks, nRoot* owner)
{
    n_assert(ks && owner);
    this->refObject.initialize(ks, owner);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCurveArrayNode::nConnection::SetCurve(nAnimCurve* curve, const char* objName, const char* cmdName, const char* chnName)
{
    n_assert(curve && objName && cmdName && chnName);
    this->connectType = CURVE;
    this->curve       = curve;
    this->refObject   = objName;
    this->cmdName     = cmdName;
    this->chnName     = chnName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCurveArrayNode::nConnection::SetBundle(nAnimCurveBundle* bundle, const char* objName, const char* cmdName, const char* chnName)
{
    n_assert(bundle && objName && cmdName);
    this->connectType = BUNDLE;
    this->bundle      = bundle;
    this->refObject   = objName;
    this->cmdName     = cmdName;
    this->chnName     = chnName;
}

//------------------------------------------------------------------------------
/**
*/
inline
nCurveArrayNode::nConnection::nConnectionType
nCurveArrayNode::nConnection::GetConnectionType()
{
    return this->connectType;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurve*
nCurveArrayNode::nConnection::GetCurve()
{
    n_assert(CURVE == this->connectType);
    return this->curve;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimCurveBundle*
nCurveArrayNode::nConnection::GetBundle()
{
    n_assert(BUNDLE == this->connectType);
    return this->bundle;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nCurveArrayNode::nConnection::GetObjectName()
{
    return this->refObject.getname();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nCurveArrayNode::nConnection::GetCmdName()
{
    return this->cmdName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nRoot*
nCurveArrayNode::nConnection::GetObject()
{
    return this->refObject.get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nCmdProto* 
nCurveArrayNode::nConnection::GetCmdProto()
{
    if (!this->cmdProto)
    {
        n_assert(!this->cmdName.IsEmpty());
        this->cmdProto = this->refObject->GetClass()->FindCmdByName(this->cmdName.Get());
        n_assert(this->cmdProto);
    }
    return this->cmdProto;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nCurveArrayNode::nConnection::GetChannelName()
{
    return this->chnName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCurveArrayNode::nConnection::SetChannelIndex(int i)
{
    this->chnIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCurveArrayNode::nConnection::GetChannelIndex()
{
    return this->chnIndex;
}

//------------------------------------------------------------------------------
#endif
