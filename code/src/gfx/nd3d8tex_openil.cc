#define N_IMPLEMENTS nD3D8Texture
//------------------------------------------------------------------------------
/**
    nd3d8tex_openil.cc
    
    Texture load function using OpenIL library functions.

    (C) A.Weissflog 2001
*/
#include "gfx/nd3d8texture.h"
#include "il/il.h"
#include "il/ilu.h"

extern const char *nd3d8_Error(HRESULT hr);

// WORKAROUND FOR NON-SQUARE-MIPMAPPED-TEXTURES-BUG in DevIL
// (not needed anymore)
#undef NONSQUARE_MIPMAP_WORKAROUND

//------------------------------------------------------------------------------
/**
    Load an image into a d3d8 texture using DevIL image loading and
    mipmap generation functions.

    @param  fileName    filename of the texture file
    @return             true if texture successfully loaded

    24-May-01   leaf    fixed bad logic when no conversion is needed
    24-May-01   leaf    fixed bug when no mipmaps requested
    28-Jul-01   leaf    Updated to DevIL
    15-Aug-01   leaf    fixed mipmap numlevels bug (thanks to Aaron Cram)
    11-Aug-01   floh    workaround for the non-square Mipmap Generation Bug
                        in DevIL (non-square mipmaps are simply not mipmapped,
                        see the define at the top of this file!)
    17-Aug-01   floh    Support for highquality texture loading flag
*/
bool
nD3D8Texture::loadFileIntoTextureIL(const char* fileName)
{
    n_assert(0 == this->tex);
    n_assert(this->ref_gs->d3d8Dev);
    HRESULT hr;
    int imgWidth, imgHeight, numMipLevels, imgFormat, numLevels; 
    D3DFORMAT srcD3DFormat;
    D3DFORMAT dstD3DFormat;
    int srcBytesPerPixel;
    int dstBytesPerPixel;

    // load the image
    ILuint imgId;
    ilGenImages(1, &imgId);
    ilBindImage(imgId);
    if (!ilLoadImage((char*) fileName)) {
        n_printf("nD3D8Texture: Failed loading image file '%s' as texture!\n", fileName);
        n_printf("OpenIL error: '%s'\n", iluErrorString(ilGetError()));
        goto fail;
    }

    // do image format conversion
    ILboolean convResult;
    ILint imageFormat;
    imageFormat = ilGetInteger(IL_IMAGE_FORMAT);
    switch (imageFormat)
    {
        case IL_COLOR_INDEX:
            // convert paletted to BGR
            convResult = ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
            srcD3DFormat     = D3DFMT_R8G8B8;
            srcBytesPerPixel = 3;
            if (this->highQuality)
            {
                dstD3DFormat     = D3DFMT_R8G8B8;
                dstBytesPerPixel = 3;
                n_printf("*** NOTE: loading high quality '%s'\n", fileName);
            }
            else
            {
                dstD3DFormat     = D3DFMT_R5G6B5;
                dstBytesPerPixel = 2;
            }
            break;

        case IL_RGB:
            // swizzle rgb to bgr format
            convResult = ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
            srcD3DFormat     = D3DFMT_R8G8B8;
            srcBytesPerPixel = 3;
            if (this->highQuality)
            {
                dstD3DFormat     = D3DFMT_R8G8B8;
                dstBytesPerPixel = 3;
                n_printf("*** NOTE: loading high quality '%s'\n", fileName);
            }
            else
            {
                dstD3DFormat     = D3DFMT_R5G6B5;
                dstBytesPerPixel = 2;
            }
            break;

        case IL_RGBA:
            // swizzle rgba to bgra format
            convResult = ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
            srcD3DFormat     = D3DFMT_A8R8G8B8;
            srcBytesPerPixel = 4;
            if (this->highQuality)
            {
                dstD3DFormat     = D3DFMT_A8R8G8B8;
                dstBytesPerPixel = 4;
                n_printf("*** NOTE: loading high quality '%s'\n", fileName);
            }
            else
            {
                dstD3DFormat     = D3DFMT_A4R4G4B4;
                dstBytesPerPixel = 2;
            }
            break;

        case IL_BGR:
            // don't convert
            convResult = IL_TRUE;
            srcD3DFormat     = D3DFMT_R8G8B8;
            srcBytesPerPixel = 3;
            if (this->highQuality)
            {
                dstD3DFormat     = D3DFMT_R8G8B8;
                dstBytesPerPixel = 3;
                n_printf("*** NOTE: loading high quality '%s'\n", fileName);
            }
            else
            {
                dstD3DFormat     = D3DFMT_R5G6B5;
                dstBytesPerPixel = 2;
            }
            break;

        case IL_BGRA:
            // don't convert
            convResult = IL_TRUE;
            srcD3DFormat     = D3DFMT_A8R8G8B8;
            srcBytesPerPixel = 4;
            if (this->highQuality)
            {
                dstD3DFormat     = D3DFMT_A8R8G8B8;
                dstBytesPerPixel = 4;
                n_printf("*** NOTE: loading high quality '%s'\n", fileName);
            }
            else
            {
                dstD3DFormat     = D3DFMT_A4R4G4B4;
                dstBytesPerPixel = 2;
            }
            break;

        default:
            // all other formats: convert to BGR
            convResult = ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
            srcD3DFormat     = D3DFMT_R8G8B8;
            srcBytesPerPixel = 3;
            if (this->highQuality)
            {
                dstD3DFormat     = D3DFMT_R8G8B8;
                dstBytesPerPixel = 3;
                n_printf("*** NOTE: loading high quality '%s'\n", fileName);
            }
            else
            {
                dstD3DFormat     = D3DFMT_R5G6B5;
                dstBytesPerPixel = 2;
            }
            break;
    }

    if (!convResult)
    {
        n_printf("nD3D8Texture: ilConvertImage() failed on file '%s'!\n", fileName);
        goto fail;
    }

    // generate mipmaps if necessary
    // DO NOT BUILD MIPMAPS IF TEXTURE IS NOT SQUARE, THIS WORKS
    // AROUND A BUG IN OPENIL.
    imgWidth  = ilGetInteger(IL_IMAGE_WIDTH);
    imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
    numMipLevels = 1;
    numLevels    = 1;
    if (this->gen_mipmaps)
    { 
#ifdef NONSQUARE_MIPMAP_WORKAROUND
        if (imgWidth == imgHeight)
        {
            iluBuildMipmaps();
            numMipLevels = D3DX_DEFAULT;
            numLevels    = ilGetInteger(IL_NUM_MIPMAPS) + 1;
        }
        else
        {
            n_printf("WARNING: non-square texture '%s', not mipmapping.\n", fileName);
        }
#else
            iluBuildMipmaps();
            numMipLevels = D3DX_DEFAULT;
            numLevels    = ilGetInteger(IL_NUM_MIPMAPS) + 1;
#endif    
    }

    // get relevant image data
    imgFormat = ilGetInteger(IL_IMAGE_FORMAT);
    this->SetWidth(imgWidth);
    this->SetHeight(imgHeight);
    this->SetBytesPerPixel(dstBytesPerPixel);
    this->SetNumMipLevels(numLevels);

    // create a d3d8 texture
    hr = D3DXCreateTexture(
        this->ref_gs->d3d8Dev,      // pDevice
        imgWidth,                   // Width
        imgHeight,                  // Height
        numMipLevels,               // MipLevels
        0,                          // Usage (not as render target)
        dstD3DFormat,               // Format
        D3DPOOL_MANAGED,            // d3d manages the textures for us
        &(this->tex));              // ppTexture
    if (FAILED(hr))
    {
        n_printf("nD3D8Texture: D3DXCreateTexture() failed with '%s'\n", nd3d8_Error(hr));
        goto fail;
    }

    // load each mipmap level into the texture
    int curLevel;
    for (curLevel = 0; curLevel < numLevels; curLevel++)
    {
        // need to rebind original image before selecting 
        // mipmap level (is this a design bug in OpenIL?)
        ilBindImage(imgId);
        if (!ilActiveMipmap(curLevel))
        {
            n_printf("nD3D8Texture: ilActiveMipmap() failed with '%s'\n", iluErrorString(ilGetError()));
            goto fail;            
        }

        // get the current surface level of the d3d8 texture object
        IDirect3DSurface8* mipSurface;
        hr = this->tex->GetSurfaceLevel(curLevel, &mipSurface);
        if (FAILED(hr))
        {
            n_printf("nD3D8Texture: GetSurfaceLevel() failed with '%s'\n", nd3d8_Error(hr));
            goto fail;
        }

        // get image data of current mipmap level
        int curWidth        = ilGetInteger(IL_IMAGE_WIDTH);
        int curHeight       = ilGetInteger(IL_IMAGE_HEIGHT);
        int bytesPerRow     = srcBytesPerPixel * curWidth;
        const void* curData = (const void*) ilGetData();

        // fill a RECT structure with width and height
        RECT srcRect;
        srcRect.left   = 0;
        srcRect.top    = 0;
        srcRect.right  = curWidth;
        srcRect.bottom = curHeight;

        // load mipmap image data into surface
        hr = D3DXLoadSurfaceFromMemory(
            mipSurface,         // pDestSurface
            NULL,               // pDestPalette (none)
            NULL,               // pDestRest (entire surface)
            curData,            // pSrcMemory
            srcD3DFormat,       // SrcFormat
            bytesPerRow,        // SrcPitch
            NULL,               // pSrcPalette
            &srcRect,           // pSrcRect
            D3DX_FILTER_NONE,   // Filter (since no scaling takes place, no filter needed)
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

    // success!
    ilDeleteImages(1, &imgId);
    return true;

fail:
    // gracefully exit on failure
    ilDeleteImages(1, &imgId);
    if (this->tex)
    {
        this->tex->Release();
        this->tex = 0;
    }
    return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
