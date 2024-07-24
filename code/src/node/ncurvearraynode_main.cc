#define N_IMPLEMENTS nCurveArrayNode
//------------------------------------------------------------------------------
//  ncurvearraynode_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "node/ncurvearraynode.h"
#include "gfx/nchannelset.h"
#include "gfx/nchannelcontext.h"
#include "gfx/nscenegraph2.h"
#include "anim/nanimserver.h"
#include "kernel/nfileserver2.h"

nNebulaScriptClass(nCurveArrayNode, "nanimnode");

//------------------------------------------------------------------------------
/**
*/
nCurveArrayNode::nCurveArrayNode() :
    refAnim(kernelServer, this),
    refFile(kernelServer, this),
    refCurveArray(this),
    curveBundles(0),
    numCurveBundles(0),
    curCurveBundle(0),
    numConnects(0),
    connects(0)
{
    this->refAnim = "/sys/servers/anim";
    this->refFile = "/sys/servers/file2";
}

//------------------------------------------------------------------------------
/**
*/
nCurveArrayNode::~nCurveArrayNode()
{
    // release curve array
    if (this->refCurveArray.isvalid())
    {
        this->refCurveArray->Release();
        this->refCurveArray.invalidate();
    }

    // release curve bundles
    if (this->curveBundles)
    {
        delete[] this->curveBundles;
        this->curveBundles = 0;
    }

    // release connects
    if (this->connects)
    {
        delete[] this->connects;
    }
}

//------------------------------------------------------------------------------
/**
    Set the filename of the anim file with curve definitions. The file
    is loaded immediately (no load on demand, because the curve bundle
    and connection definitions need the anim curves).
*/
void
nCurveArrayNode::SetFilename(const char* filename)
{
    n_assert(filename);
    this->animFilePath.Set(this->refFile.get(), filename, this->refFile->GetCwd());
    
    // try to share the anim resource
    const char* absPath = this->animFilePath.GetAbsPath();
    nAnimCurveArray* curveArray = this->refAnim->FindAnimCurveArray(absPath);
    if (!curveArray)
    {
        curveArray = this->refAnim->NewAnimCurveArray(absPath);
        n_assert(curveArray);
        bool curveLoaded = false;
        if (strstr(absPath, ".nax"))
        {
            curveLoaded = curveArray->LoadNax(this->refFile.get(), absPath);
        }
        else
        {
            curveLoaded = curveArray->LoadAnim(this->refFile.get(), absPath);
        }
    n_assert(curveLoaded);
}
    this->refCurveArray = curveArray;
}

//------------------------------------------------------------------------------
/**
*/
const char* 
nCurveArrayNode::GetFilename()
{
    return this->animFilePath.GetPath();
}

//------------------------------------------------------------------------------
/**
    Begin defining curve bundles. A curve bundle takes a number of anim curves
    with associated weights and emits mixed animation samples. Other then
    that it looks just like a simple animation curve to the outside.

    @param  num     the number of bundles
*/
void
nCurveArrayNode::BeginCurveBundles(int num)
{
    n_assert(num > 0);
    n_assert(0 == this->curveBundles);

    this->numCurveBundles = num;
    this->curveBundles = new nAnimCurveBundle[this->numCurveBundles];
}

//------------------------------------------------------------------------------
/**
    Begin a curve bundle definition.

    @param  bundleIndex     index of bundle to define
    @param  bundleName      the name of the bundle
    @param  numCurves       number of curves to add to bundle
*/
void
nCurveArrayNode::BeginBundle(int bundleIndex, const char* bundleName, int numCurves)
{
    n_assert((bundleIndex >= 0) && (bundleIndex < this->numCurveBundles));
    n_assert(bundleName);
    n_assert(numCurves);

    this->curCurveBundle = bundleIndex;
    nAnimCurveBundle& bundle = this->curveBundles[this->curCurveBundle];
    bundle.SetName(bundleName);
    bundle.Begin(numCurves);
}

//------------------------------------------------------------------------------
/**
    @param  curveIndex      curve index in bundle
    @param  animCurve       pointer to anim curve
    @param  channelName     name of channel which provides the mixer weight
*/
void
nCurveArrayNode::SetBundleCurve(int curveIndex, const char* curveName, const char* channelName)
{
    n_assert(curveName && channelName);

    nAnimCurveBundle& bundle = this->curveBundles[this->curCurveBundle];

    // resolve curve name into curve pointer
    nAnimCurveArray* curveArray = this->refCurveArray.get();
    nAnimCurve* curve = 0;
    int i;
    int numCurves = curveArray->GetNumCurves();
    for (i = 0; i < numCurves; i++)
    {
        if (strcmp(curveArray->GetCurve(i).GetName(), curveName) == 0)
        {
            curve = &(curveArray->GetCurve(i));
            break;
        }
    }
    if (!curve)
    {
        n_error("nCurveArrayNode::SetBundleCurve(): curve '%s' not found in file '%s'\n",
                 curveName, this->animFilePath.GetAbsPath());
    }
    bundle.Set(curveIndex, curve, channelName);
    bundle.SetChannelIndex(curveIndex, -1);
    bundle.SetWeight(curveIndex, 0.0f);
}

//------------------------------------------------------------------------------
/**
*/
void
nCurveArrayNode::EndBundle()
{
    nAnimCurveBundle& bundle = this->curveBundles[this->curCurveBundle];
    bundle.End();
}

//------------------------------------------------------------------------------
/**
*/
void
nCurveArrayNode::EndCurveBundles()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Begin defining curve and bundle connections. A connection connects a curve
    or bundle to an object. Every frame, for each connection a script command
    is invoked on the target object with the current sample value from
    the curve or bundle.

    @param  num     number of connections
*/
void
nCurveArrayNode::BeginConnects(int num)
{
    n_assert(num > 0);
    n_assert(0 == this->connects);

    this->numConnects = num;
    this->connects = new nConnection[this->numConnects];
}

//------------------------------------------------------------------------------
/**
    @param  index       the connection index
    @param  curveName   name of the curve 
    @param  objName     pathname of object to connect to
    @param  cmdName     the script command name to invoke on the object
    @param  chnName     name of channel which provided the sample time
*/
void
nCurveArrayNode::SetCurveConnect2(int index, 
                                  const char* curveName, 
                                  const char* objName, 
                                  const char* cmdName,
                                  const char* chnName)
{
    n_assert((index >= 0) && (index < this->numConnects));
    n_assert(this->connects);
    
    // resolve curve name into curve pointer
    nAnimCurveArray* curveArray = this->refCurveArray.get();
    nAnimCurve* curve = 0;
    int i;
    int numCurves = curveArray->GetNumCurves();
    for (i = 0; i < numCurves; i++)
    {
        if (strcmp(curveArray->GetCurve(i).GetName(), curveName) == 0)
        {
            curve = &(curveArray->GetCurve(i));
            break;
        }
    }
    
    n_assert(curve);
    this->connects[index].SetCurve(curve, objName, cmdName, chnName);
}

//------------------------------------------------------------------------------
/**
    This version of SetCurveConnect() doesn't use the local channel name,
    but instead the global channel as defined by nAnimNode.
  
    @param  index       the connection index
    @param  curveName   name of the curve 
    @param  objName     pathname of object to connect to
    @param  cmdName     the script command name to invoke on the object
*/
void
nCurveArrayNode::SetCurveConnect(int index, 
                                 const char* curveName, 
                                 const char* objName, 
                                 const char* cmdName)
{
    this->SetCurveConnect2(index, curveName, objName, cmdName, this->GetChannel());
}

//------------------------------------------------------------------------------
/**
    @param  index       the connection index
    @param  bundleName  name of the bundle
    @param  objName     pathname of object to connect to
    @param  cmdName     the script command name to invoke on the object
    @param  chnName     name of a channel which provides the sample time
*/
void
nCurveArrayNode::SetBundleConnect2(int index, 
                                   const char* bundleName, 
                                   const char* objName, 
                                   const char* cmdName,
                                   const char* chnName)
{
    n_assert((index >= 0) && (index < this->numConnects));
    n_assert(this->connects);

    // resolve bundle name into bundle pointer
    nAnimCurveBundle* bundle = 0;
    int i;
    for (i = 0; i < this->numCurveBundles; i++)
    {
        if (strcmp(this->curveBundles[i].GetName(), bundleName) == 0)
        {
            bundle = &(this->curveBundles[i]);
            break;
        }
    }
    n_assert(bundle);
    this->connects[index].SetBundle(bundle, objName, cmdName, chnName);
}

//------------------------------------------------------------------------------
/**
    This version of SetBundleConnect() takes the sample time from
    the nAnimNode parent class.
  
    @param  index       the connection index
    @param  bundleName  name of the bundle
    @param  objName     pathname of object to connect to
    @param  cmdName     the script command name to invoke on the object
*/
void
nCurveArrayNode::SetBundleConnect(int index, 
                                  const char* bundleName, 
                                  const char* objName, 
                                  const char* cmdName)
{
    this->SetBundleConnect2(index, bundleName, objName, cmdName, this->GetChannel());
}

//------------------------------------------------------------------------------
/**
*/
void
nCurveArrayNode::EndConnects()
{
    n_assert(this->connects);

    // initialize all connections
    int i;
    for (i = 0; i < this->numConnects; i++)
    {
        nConnection& curConnect = this->connects[i];
        curConnect.Initialize(kernelServer, this);
    }
}

//------------------------------------------------------------------------------
/**
    Declares this node as depend node of my parent, so that the parent will
    call my Compute() method.
*/
void
nCurveArrayNode::Initialize()
{
    nVisNode* p = (nVisNode*) this->GetParent();
    if (p && (p->IsA(ks->FindClass("nvisnode"))))
    {
        p->DependsOn(this);
    }
    nVisNode::Initialize();
}

//------------------------------------------------------------------------------
/**
    Attach connection weight channels to channel set.

    @param  chnSet      the channel set object
*/
void
nCurveArrayNode::AttachChannels(nChannelSet* chnSet)
{
    n_assert(chnSet);

    // add bundle weight channels
    int i;
    for (i = 0; i < this->numCurveBundles; i++)
    {
        nAnimCurveBundle& bundle = this->curveBundles[i];
        int j;
        for (j = 0; j < bundle.GetNumCurves(); j++)
        {
            int localIndex = chnSet->AddChannel(this->refChannel.get(), bundle.GetChannelName(j));
            bundle.SetChannelIndex(j, localIndex);
        }
    }

    // add sample time channels of curve connects
    for (i = 0; i < this->numConnects; i++)
    {
        nConnection& conn = this->connects[i];
        int localIndex = chnSet->AddChannel(this->refChannel.get(), conn.GetChannelName());
        conn.SetChannelIndex(localIndex);
    }

    nAnimNode::AttachChannels(chnSet);
}

//------------------------------------------------------------------------------
/**
    Get time-based sample from curve in the connection, and invoke script
    command on target object of connection.
*/
void
nCurveArrayNode::DoCurveAnim(nConnection& con, nChannelContext* chnContext)
{
    n_assert(con.GetConnectionType() == nConnection::CURVE);

    // get the sample time from the channel context
    float time = chnContext->GetChannel1f(con.GetChannelIndex());

    // get sampled value from curve
    nAnimCurve* curve = con.GetCurve();
    vector4 val;
    curve->Sample(time, val);

    // build a cmd object and send it to target object
    nCmdProto* cmdProto = con.GetCmdProto();
    nCmd* cmd = cmdProto->NewCmd();
    cmd->Rewind();
    int numArgs = cmd->GetNumInArgs();
    if (numArgs > 0) cmd->In()->SetF(val.x);
    if (numArgs > 1) cmd->In()->SetF(val.y);
    if (numArgs > 2) cmd->In()->SetF(val.z);
    if (numArgs > 3) cmd->In()->SetF(val.w);
    con.GetObject()->Dispatch(cmd);
    cmdProto->RelCmd(cmd);
}

//------------------------------------------------------------------------------
/**
*/
void
nCurveArrayNode::DoBundleAnim(nConnection& con, nChannelContext* chnContext)
{
    n_assert(con.GetConnectionType() == nConnection::BUNDLE);

    // get bundle
    nAnimCurveBundle* bundle = con.GetBundle();
        
    // get the sample time from the bundle connection
    float time = chnContext->GetChannel1f(con.GetChannelIndex());
        
    // update weights
    int j;
    int numCurves = bundle->GetNumCurves();
    for (j = 0; j < numCurves; j++)
    {
        float w = chnContext->GetChannel1f(bundle->GetChannelIndex(j));
        if (w < 0.0f)
        {
            w = 0.0f;
        }
        else if (w > 1.0f)
        {
            w = 1.0f;
        }
        bundle->SetWeight(j, w);
    }

    // get sampled value from bundle
    vector4 val;
    bundle->Sample(time, val);

    // build a cmd object and send it to target object
    nCmdProto* cmdProto = con.GetCmdProto();
    nCmd* cmd = cmdProto->NewCmd();
    cmd->Rewind();
    int numArgs = cmd->GetNumInArgs();
    if (numArgs > 0) cmd->In()->SetF(val.x);
    if (numArgs > 1) cmd->In()->SetF(val.y);
    if (numArgs > 2) cmd->In()->SetF(val.z);
    if (numArgs > 3) cmd->In()->SetF(val.w);
    con.GetObject()->Dispatch(cmd);
    cmdProto->RelCmd(cmd);
}

//------------------------------------------------------------------------------
/**
    Iterate through connections, and invoke DoCurveAnim() or DoBundleAnim()
    based on the connection type (curve or bundle).
*/
void
nCurveArrayNode::Compute(nSceneGraph2* sceneGraph)
{
    nVisNode::Compute(sceneGraph);

    // get current anim channel value (usually the current time)
    nChannelContext* chnContext = sceneGraph->GetChannelContext();
    n_assert(chnContext);

    // for each connection...
    int i;
    for (i = 0; i < this->numConnects; i++)
    {
        switch (this->connects[i].GetConnectionType())
        {
            case nConnection::CURVE:
                this->DoCurveAnim(this->connects[i], chnContext);
                break;

            case nConnection::BUNDLE:
                this->DoBundleAnim(this->connects[i], chnContext);
                break;

            default:
                n_assert(false);
        }
    }
}

//------------------------------------------------------------------------------
