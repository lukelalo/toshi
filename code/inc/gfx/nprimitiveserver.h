#ifndef N_PRIMITIVESERVER_H
#define N_PRIMITIVESERVER_H
//------------------------------------------------------------------------------
/**
    @class nPrimitiveServer
    @brief OpenGL style immediate mode rendering and primitives for debugging purposes.

    nPrimitiveServer is primarily for debugging purposes. It allows for
    OpenGL style immediate mode rendering and provides functions for drawing
    common shapes like capsules, spheres, boxes, cylinders, etc. 
    The interface is somewhat similar to GLUT, so
    in order to rotate the primitives you'll need to manipulate the
    MODELVIEW matrix in the gfx server.

    (C) 2003  Leaf Garland & Vadim Macagon

    nPrimitiveServer is licensed under the terms of the Nebula License.
*/

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_PIXELSHADER_H
#include "gfx/npixelshader.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#undef N_DEFINES
#define N_DEFINES nPrimitiveServer
#include "kernel/ndefdllclass.h"

class nGfxServer;
//------------------------------------------------------------------------------
class N_PUBLIC nPrimitiveServer : public nRoot
{
public:
    /// constructor
    nPrimitiveServer();
    /// destructor
    virtual ~nPrimitiveServer();
    /// persistency
    virtual bool SaveCmds( nPersistServer* persistServer );

    // OpenGL style immediate mode rendering functions.
    void Begin(nPrimType);
    void Coord(float, float, float);
    void Norm(float, float, float);
    void Rgba(float, float, float, float);
    void Uv(ulong layer, float, float);
    void End(void); 

    // Prefab shapes for your pleasure
    void WirePlane( float planeExtent = 25.0f, float gridSize = 1.0f );
    void SolidPlane( float planeExtent = 25.0f );
    void WireSphere( float radius, int slices, int stacks );
    void SolidSphere( float radius, int slices, int stacks );
    void WireCone( float base, float height, bool closed, int slices, int stacks );
    void SolidCone( float base, float height, bool closed, int slices, int stacks );
    void WireCapsule( float radius, float length, int slices, int stacks );
    void SolidCapsule( float radius, float length, int slices, int stacks );
    void WireCylinder( float radius, float length, bool closed = true,
        int slices = 16, int stacks = 1 );
    void SolidCylinder( float radius, float length, bool closed = true,
        int slices = 16, int stacks = 1 );
    void WireCube( float size = 1.0f );
    void SolidCube( float size = 1.0f );
    void WireBox( float lx, float ly, float lz );
    void SolidBox( float lx, float ly, float lz );
    void WireTorus( float innerRadius, float outerRadius, int sides, int rings );
    void SolidTorus( float innerRadius, float outerRadius, int sides, int rings );
    void CheckerPlane(float planeExtent, float gridSize);
    
    // State management
    void EnableLighting( bool enable );
    void SetColor( float r, float g, float b, float a );
    void SetPixelShader( nPixelShader* ps );
    nPixelShader* GetPixelShader();

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:
    nAutoRef<nGfxServer> ref_GfxServer;
    nRef<nPixelShader> ref_ps;
    nPixelShaderDesc* psDesc;
    nDynVertexBuffer dynVB;
    nVertexBuffer* VB;
    nRef<nIndexBuffer> ref_ibuf;
    nPrimType primType;
    int numVerts, maxVerts;
    bool inBegin;
    ulong curColor;

    void InitPixelShader();
    void CheckPixelShader();

};

//------------------------------------------------------------------------------
#endif

