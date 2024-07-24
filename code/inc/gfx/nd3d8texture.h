#ifndef N_D3D8TEXTURE_H
#define N_D3D8TEXTURE_H
//-----------------------------------------------------------------------------
/**
    @class nD3D8Texture

    @brief Texture object for nD3D8Server.

    Implements nTexture class that works together with the
    nD3D8Server. Uses D3DX functionality to keep the code small
    and simple.
*/
#ifndef N_TEXTURE_H
#include "gfx/ntexture.h"
#endif

#ifndef N_D3D8SERVER_H
#include "gfx/nd3d8server.h"
#endif

#undef N_DEFINES
#define N_DEFINES nD3D8Texture
#include "kernel/ndefdllclass.h"

//-----------------------------------------------------------------------------
class N_DLLCLASS nD3D8Texture : public nTexture
{
public:
    /// the constructor
    nD3D8Texture();
    /// the destructor
    virtual ~nD3D8Texture();
    /// load texture image from disk into d3d texture object
    virtual bool Load();
    /// release d3d texture object
    virtual void Unload();
    /// get d3d8 texture object
    IDirect3DTexture8* GetD3DTexture();

    static nKernelServer *kernelServer;

private:
    /// legacy texture loading function (OBSOLETE)
    bool loadFileIntoTextureOld(const char *pn, const char *an);
    /// OpenIL based texture loading function
    bool loadFileIntoTextureIL(const char* fileName);
    /// load dds file into texture
    bool loadFileIntoTextureDDS(const char* fileName);
    /// load Nebula ntx file into texture
    bool loadFileIntoTextureNTX(const char* filename);

    nAutoRef<nD3D8Server> ref_gs;
    IDirect3DTexture8 *tex;
    int d3dMipLod;                  // the actual real mip lod level of the d3d texture
};
//-----------------------------------------------------------------------------
#endif
