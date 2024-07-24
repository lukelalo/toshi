#define N_IMPLEMENTS nD3D8Texture
//-----------------------------------------------------------------------------
//  nd3d8tex_main.cc
//  (C) 2001 A.Weissflog
//-----------------------------------------------------------------------------
#include "gfx/nd3d8texture.h"

nNebulaClass(nD3D8Texture, "ntexture");

//-----------------------------------------------------------------------------
/**
*/
nD3D8Texture::nD3D8Texture()
: ref_gs(kernelServer,this),
  tex(0),
  d3dMipLod(0)
{
    this->ref_gs = "/sys/servers/gfx";
}

//-----------------------------------------------------------------------------
/**
*/
nD3D8Texture::~nD3D8Texture()
{
    this->Unload();
}

//-----------------------------------------------------------------------------
/**
    Destroys the embedded d3d texture object.
*/
void 
nD3D8Texture::Unload()
{
    if (this->tex)
    {
        this->tex->Release();
        this->tex = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Create a d3d texture object by loading it from a file. Automatically
    generates the mipmaps for the texture. Uses the D3DXCreateTextureFromFileEx()
    function for simplicity.
    
    FIXME: doesn't handle the alpha channel yet, switch to different
    image file format with embedded alpha channel!

*/
bool nD3D8Texture::Load()
{
    // unload the old stuff
    this->Unload();

    // decide on a texture loader...
    bool res = false;
    if (this->aname)
    {
        // old BMP file loader with separate pixel and alpha file
        res = this->loadFileIntoTextureOld(this->pname, this->aname);
    }
    else
    {
        if (strstr(this->pname, ".ntx"))
        {
            // a Nebula ntx file
            res = this->loadFileIntoTextureNTX(this->pname);
        }
        else
        {
            // some other file format
            res = this->loadFileIntoTextureIL(this->pname);
        }
    }

    // complain about errors
    if (!res)
    {
        n_printf("nD3D8Texture::Load(): Failed to load bitmap [%s,%s] into texture surface\n",
                 this->pname, this->aname ? this->aname : "<none>");
        this->Unload();
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
/**
    Returns pointer to d3d8 texture object. Sets the current LOD level
    before doing so.
*/
IDirect3DTexture8*
nD3D8Texture::GetD3DTexture()
{
    n_assert(this->tex);

    // only change miplod if necessary
    if (this->d3dMipLod != this->curMipLod)
    {
        this->tex->SetLOD(this->curMipLod);
        this->d3dMipLod = this->curMipLod;
    }

    // return the texture
    return this->tex;
}

//-----------------------------------------------------------------------------
//  EOF
//-----------------------------------------------------------------------------
