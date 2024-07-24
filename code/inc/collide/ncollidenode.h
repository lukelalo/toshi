#ifndef N_COLLIDENODE_H
#define N_COLLIDENODE_H
//------------------------------------------------------------------------------
/**
    @class nCollideNode
    @ingroup NebulaCollideModule

    @brief Wraps nCollideObject into a nVisNode for debug visualization.

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_COLLIDESERVER_H
#include "collide/ncollideserver.h"
#endif

#ifndef N_RSRCPATH_H
#include "misc/nrsrcpath.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nCollideNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nCollideServer;
class nGfxServer;

class N_DLLCLASS nCollideNode : public nVisNode {
public:
    /// constructor
    nCollideNode();
    /// destructor
    virtual ~nCollideNode();
    /// attach to the scene
    virtual bool Attach(nSceneGraph2* sceneGraph);
    /// update your internal state and make visible
    virtual void Compute(nSceneGraph2* sceneGraph);
    
    /// set collssion shape file
    void SetFilename(const char* fname);
    /// get collision shape file
    const char* GetFilename();
    /// set collision class
    void SetCollClass(const char* cc);
    /// get collision class
    const char* GetCollClass();

    static nKernelServer* kernelServer;

private:
    nAutoRef<nFileServer2>   refFileServer;
    nAutoRef<nCollideServer> refCollServer;
    nAutoRef<nGfxServer>     refGfxServer;
    nCollideContext* collContext;
    nCollideShape*   collShape;
    nCollideObject*  collObject;
    bool fileDirty;
    nRsrcPath rsrcPath;
    const char* collClass;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCollideNode::nCollideNode() :
    refFileServer(kernelServer, this),
    refCollServer(kernelServer, this),
    refGfxServer(kernelServer, this),
    collContext(0),
    collShape(0),
    collObject(0),
    fileDirty(false),
    collClass(0)
{
    this->refFileServer = "/sys/servers/file2";
    this->refCollServer = "/sys/servers/collide";
    this->refGfxServer  = "/sys/servers/gfx";
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCollideNode::SetFilename(const char* fname)
{
    n_assert(fname);
    this->rsrcPath.Set(this->refFileServer.get(), fname, this->refFileServer->GetCwd());
    this->fileDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char* 
nCollideNode::GetFilename()
{
    return this->rsrcPath.GetPath();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCollideNode::SetCollClass(const char* cc)
{
    n_assert(!this->collClass);
    n_assert(cc);
    this->collClass = n_strdup(cc);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nCollideNode::GetCollClass()
{
    return this->collClass;
}

//------------------------------------------------------------------------------
#endif
