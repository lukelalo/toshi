#define N_IMPLEMENTS nD3D8Texture
//------------------------------------------------------------------------------
/**
    nd3d8tex_old.cc
    
    Obsolete texture loading function for backward compatibility
    (used if 2 files are given for color and alpha channel)

    (C) A.Weissflog 2001
*/
#include "gfx/nd3d8texture.h"

extern const char *nd3d8_Error(HRESULT hr);

//------------------------------------------------------------------------------
/**
    *** OBSOLETE *** texture loading function which is used if 2 filenames 
    are given for the color and alpha channel.

    @param  pn  filename of bmp file that provides the rgb channels
    @param  an  filename of bmp file that provides the alpha channel (can be NULL)
    @return     true if successful
*/
bool 
nD3D8Texture::loadFileIntoTextureOld(const char *pn, const char *an)
{
    n_assert(pn);
    n_assert(0 == this->tex);

// n_printf("nD3D8Texture: Using old texture loading code.\n");

    HRESULT hr;
    uchar *src_buf = NULL;
    D3DFORMAT dst_d3dfmt;
    D3DFORMAT src_d3dfmt;
    PALETTEENTRY pal_entries[256];
    int src_pixel_size    = 0;
    int dst_pixel_size    = 0;
    int src_bytes_per_row = 0;
    int w,h;
    int actNumMipLevels, numMipLevels;

    // load the bmp file(s)...
    if (this->BeginLoad(pn, an)) {
        
        w = this->bmp_pixels.GetWidth();
        h = this->bmp_pixels.GetHeight();

        // 'this->pf' now contains the pixel format of the file,
        // find the best match corresponding d3dx texture format
        // enumerants
        switch (this->pf->bpp) {
            case 8:
                // 8 bit paletted (will be converted to rgba)
                src_d3dfmt = D3DFMT_P8;
                src_pixel_size = 1;
                if (this->highQuality)
                {
                    dst_d3dfmt = D3DFMT_R8G8B8;
                    dst_pixel_size = 3;
                    n_printf("*** NOTE: loading high quality '%s'\n", pn);
                }
                else
                {
                    dst_d3dfmt = D3DFMT_R5G6B5;
                    dst_pixel_size = 2;
                }
                break;
            case 24:
                // if 24 bit rgb texture (no alpha)
                src_d3dfmt = D3DFMT_R8G8B8;
                src_pixel_size = 3;
                if (this->highQuality)
                {
                    dst_d3dfmt = D3DFMT_R8G8B8;
                    dst_pixel_size = 3;                
                    n_printf("*** NOTE: loading high quality '%s'\n", pn);
                }
                else
                {
                    dst_d3dfmt = D3DFMT_R5G6B5;
                    dst_pixel_size = 2;
                }
                break;
            case 32:
                // 32 bit rgba texture
                src_d3dfmt = D3DFMT_A8R8G8B8;
                src_pixel_size = 4;
                if (this->highQuality)
                {
                    dst_d3dfmt = D3DFMT_A8R8G8B8;
                    dst_pixel_size = 4;                
                    n_printf("*** NOTE: loading high quality '%s'\n", pn);
                }
                else
                {
                    dst_d3dfmt = D3DFMT_A4R4G4B4;
                    dst_pixel_size = 2;
                }
                break;
            default:
                n_printf("nD3D8Texture: unsupported pixel format in file '%s'\n", pn);
                goto fail;
        }

        // allocate a tmp buffer to load raw texture data to
        int src_size      = w * h * src_pixel_size;
        src_bytes_per_row = w * src_pixel_size;
        src_buf = (uchar *) n_malloc(src_size);

        // fill texture data 
        this->SetWidth(w);
        this->SetHeight(h);
        this->SetBytesPerPixel(dst_pixel_size);

        // load the raw texture data into the src buffer
        long l;
        while ((l=this->LoadLine()) != -1) {
            uchar *addr = src_buf + l * src_bytes_per_row;
            if (D3DFMT_A8R8G8B8 == src_d3dfmt) {
                // swizzle bits from RGBA to ARGB
                int i;
                for (i=0; i<src_bytes_per_row; i+=4) {
                    addr[i+0] = this->line[i+2];
                    addr[i+1] = this->line[i+1];
                    addr[i+2] = this->line[i+0];
                    addr[i+3] = this->line[i+3];
                }
            } else {
                memcpy(addr, this->line, src_bytes_per_row);
            }
        }
        
        // create color palette? if yes, read the color palette
        // before it becomes invalid during EndLoad()
        if (src_d3dfmt == D3DFMT_P8) {
            int i;
            for (i=0; i<256; i++) {
                nPalEntry *pe = &(this->pf->pal[i]);
                pal_entries[i].peRed   = pe->r;
                pal_entries[i].peGreen = pe->g;
                pal_entries[i].peBlue  = pe->b;
                pal_entries[i].peFlags = 0;
            }
        }

        // finish loading the texture data
        this->EndLoad();

    } else {
        n_printf("nD3D8Texture: BeginLoad(%s) failed!\n", pn);
        goto fail;
    }

    // create a d3d8 texture
    numMipLevels = this->gen_mipmaps ? D3DX_DEFAULT : 1;
    hr = D3DXCreateTexture(
        this->ref_gs->d3d8Dev,      // pDevice
        w,                          // Width
        h,                          // Height
        numMipLevels,               // MipLevels
        0,                          // Usage (not as render target)
        dst_d3dfmt,                 // Format
        D3DPOOL_MANAGED,            // d3d manages the textures for us
        &(this->tex));              // ppTexture
    if (FAILED(hr))
    {
        n_printf("nD3D8Texture: D3DXCreateTexture() failed with '%s'\n", nd3d8_Error(hr));
        goto fail;
    }

    // compute the actual number of mipmap levels
    actNumMipLevels = 0;
    if (this->gen_mipmaps)
    {
        int curWidth = w;
        while (curWidth != 0)
        {
            actNumMipLevels++;
            curWidth >>= 1;
        }
    }
    else
    {
        actNumMipLevels = 1;
    }

    // fill a RECT structure with width and height
    RECT srcRect;
    srcRect.left   = 0;
    srcRect.top    = 0;
    srcRect.right  = w;
    srcRect.bottom = h;

    // load the mipmap levels, this is a bit HACKY, since all mipmaps
    // are created from the source image, and only a point filter is used
    int curLevel;
    this->SetNumMipLevels(actNumMipLevels);
    for (curLevel = 0; curLevel < actNumMipLevels; curLevel++)
    {
        // get the current surface level of the d3d8 texture object
        IDirect3DSurface8* mipSurface;
        hr = this->tex->GetSurfaceLevel(curLevel, &mipSurface);
        if (FAILED(hr))
        {
            n_printf("nD3D8Texture: GetSurfaceLevel() failed with '%s'\n", nd3d8_Error(hr));
            goto fail;
        }
            
        // load mipmap image data into surface
        hr = D3DXLoadSurfaceFromMemory(
            mipSurface,         // pDestSurface
            NULL,               // pDestPalette (none)
            NULL,               // pDestRest (entire surface)
            src_buf,            // pSrcMemory
            src_d3dfmt,         // SrcFormat
            src_bytes_per_row,  // SrcPitch
            (src_d3dfmt == D3DFMT_P8) ? pal_entries : NULL, // pSrcPalette
            &srcRect,           // pSrcRect
            D3DX_FILTER_POINT,  // Filter (since no scaling takes place, no filter needed)
            0);                 // ColorKey (none)
        if (FAILED(hr))
        {
            n_printf("nD3D8Texture: D3DXLoadSurfaceFromMemory() failed with '%s'\n", nd3d8_Error(hr));
            goto fail;
        }

        // release the mipSurface (as required by GetSurfaceLevel())
        mipSurface->Release();
        mipSurface = 0;
    }

    // success
    if (src_buf) {
        n_free(src_buf);
    }
    return true;

fail:
    if (src_buf) {
        n_free(src_buf);
    }
    if (this->tex)
    {
        this->tex->Release();
        this->tex = NULL;
    }
    return false;
}

