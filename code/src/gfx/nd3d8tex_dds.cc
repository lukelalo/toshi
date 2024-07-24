#define N_IMPLEMENTS nD3D8Texture
//------------------------------------------------------------------------------
//  nd3d8tex_dds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/nd3d8texture.h"

extern const char *nd3d8_Error(HRESULT hr);

//------------------------------------------------------------------------------
/**
    Load a compressed texture into a d3d texture.

    @param  fileName    filename of texture file
    @return             true if everything ok, false if texture could not be
                        loaded (most probably because the file does not exist)
*/
bool
nD3D8Texture::loadFileIntoTextureDDS(const char* fileName)
{
    n_assert(0 == this->tex);
    n_assert(this->ref_gs->d3d8Dev);

n_printf("nD3D8Texture: loading compressed texture\n");

    HRESULT hr;
    hr = D3DXCreateTextureFromFileExA(
        this->ref_gs->d3d8Dev,
        fileName,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        this->gen_mipmaps ? D3DX_DEFAULT : 1,
        0,
        D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED,
        D3DX_FILTER_NONE,
        D3DX_FILTER_BOX,
        0,
        NULL,
        NULL,
        &(this->tex));
    if (FAILED(hr))
    {
        n_printf("nD3D8Texture: D3DXCreateTextureFromFileExA() failed with '%s'\n", nd3d8_Error(hr));
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
