#ifndef N_MESHNODE_H
#define N_MESHNODE_H
//------------------------------------------------------------------------------
/**
    @class nMeshNode
    @ingroup NebulaVisnodeModule

    @brief Define a static mesh in the visual hierarchy.
*/
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_RSRCPATH_H
#include "misc/nrsrcpath.h"
#endif

#undef N_DEFINES
#define N_DEFINES nMeshNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nGfxServer;
class nVertexBuffer;
class nIndexBuffer;
class nShadowServer;
class nShadowCaster;
class nShadowServer;
class N_PUBLIC nMeshNode : public nVisNode 
{
public:
    /// constructor
    nMeshNode();
    /// destructor
    virtual ~nMeshNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *);
    /// attach to scene
    virtual bool Attach(nSceneGraph2*);
    /// update internal state and render
    virtual void Compute(nSceneGraph2*);
    /// preload resources
    virtual void Preload();

    /// get pointer to embedded vertex buffer object
    nVertexBuffer *GetVertexBuffer(void);

    /// set read only flag
    void SetReadOnly(bool b);
    /// get readonly flag
    bool GetReadOnly();
    /// set filename of mesh file
    void SetFilename(const char *);
    /// get filename of mesh file
    const char *GetFilename();
    /// set as shadow caster state
    void SetCastShadow(bool b);
    /// can cast shadows?
    bool GetCastShadow();

    static nKernelServer *kernelServer;

protected:
    nAutoRef<nShadowServer> refShadowServer;
    nAutoRef<nFileServer2>  refFileServer;
    nRef<nShadowCaster>     refShadowCaster;
    nRef<nVertexBuffer>     ref_vb;
    nRef<nIndexBuffer>      ref_ibuf;
    nRsrcPath               rsrc_path;
    bool readonly;
    bool castShadows;

private:
    bool loadMesh(void);
};

//------------------------------------------------------------------------------
#endif
