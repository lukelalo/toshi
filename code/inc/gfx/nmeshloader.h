#ifndef N_MESHLOADER_H
#define N_MESHLOADER_H
//------------------------------------------------------------------------------
/**
    @class nMeshLoader

    @brief Abstract base class for mesh loaders. Mesh loaders construct 
    vertex buffer, index buffer and shadow caster objects from a mesh file.

    (C) 2001 RadonLabs GmbH
*/

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

//------------------------------------------------------------------------------
class nGfxServer;
class nShadowServer;
class nVertexBuffer;
class nIndexBuffer;
class nShadowCaster;
class nKernelServer;
class nFileServer2;

class nMeshLoader
{
public:
    /// constructor
    nMeshLoader(nKernelServer* ks);
    /// destructor
    ~nMeshLoader();

    /// begin loading a mesh file
    void Begin(nGfxServer* gfx, nShadowServer* shadow, bool readonly);
    /// load a mesh file
    bool Load(const char* fileName);
    /// obtain pointer to vertex buffer object, incrs refcount!
    nVertexBuffer* ObtainVertexBuffer();
    /// obtain pointer to index buffer object, incrs refcount!
    nIndexBuffer* ObtainIndexBuffer();
    /// obtain pointer to shadow caster object, incrs refcount!
    nShadowCaster* ObtainShadowCaster();
    /// finish loading, will call release on created objects
    void End();

protected:
    nKernelServer* kernelServer;            // pointer to kernel server
    nAutoRef<nFileServer2> refFileServer;   // all file operations go through nFileServer2
    bool inBegin;                           // currently inside Begin()/End()
    bool readOnly;                          // create vertex buffer in readonly mode?
    nGfxServer* gfxServer;                  // pointer to gfx server object
    nShadowServer* shadowServer;            // pointer to shadow server object
    nVertexBuffer* vertexBuffer;            // pointer to created vertex buffer object
    nIndexBuffer*  indexBuffer;             // pointer to created index buffer
    nShadowCaster* shadowCaster;            // pointer to created shadow caster object
};
//------------------------------------------------------------------------------
#endif
