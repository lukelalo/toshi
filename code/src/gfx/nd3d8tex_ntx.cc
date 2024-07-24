#define N_IMPLEMENTS nD3D8Texture
//------------------------------------------------------------------------------
//  nd3d8tex_ntx.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/nd3d8texture.h"
#include "gfx/nntxfile.h"

extern const char *nd3d8_Error(HRESULT hr);

//------------------------------------------------------------------------------
/**
    Create a d3d8 texture from a Nebula ntx texture file.

    @param  filename    absolute path to ntx file
    @return             true if successful
*/
bool
nD3D8Texture::loadFileIntoTextureNTX(const char* filename)
{
    n_assert(filename);
    n_assert(0 == this->tex);

    IDirect3DDevice8* d3d8Dev = this->ref_gs->d3d8Dev;
    n_assert(d3d8Dev);

    void* imageData = 0;
    HRESULT hr;
    int numLevels;
    int dataSize;

// n_printf("nD3D8Texture: Using ntx texture loading code\n");

    // create a nNtxFile object 
    nNtxFile ntxFile(kernelServer);

    // open ntx object for reading
    if (!ntxFile.OpenRead(filename))
    {
        n_printf("nD3D8Texture: failed to open ntx file '%s'!\n", filename);
        return false;
    }

    // agree on a d3d8 format
    D3DFORMAT d3dSrcFormat;
    D3DFORMAT d3dDstFormat;
    ntxFile.SetCurrentBlock(0);
    nNtxFile::Format ntxFormat = ntxFile.GetFormat();
    int bytesPerPixel = 0;
    if ((ntxFormat == nNtxFile::FORMAT_R8G8B8) || (ntxFormat == nNtxFile::FORMAT_R5G6B5))
    {
        // no alpha information in this texture
        if (this->GetHighQuality())
        {
            d3dSrcFormat = D3DFMT_R8G8B8;
            d3dDstFormat = D3DFMT_X8R8G8B8;
            ntxFormat = nNtxFile::FORMAT_R8G8B8;
            bytesPerPixel = 4;
        }
        else
        {
            d3dSrcFormat = D3DFMT_R5G6B5;
            d3dDstFormat = D3DFMT_R5G6B5;
            ntxFormat = nNtxFile::FORMAT_R5G6B5;
            bytesPerPixel = 2;
        }
    }
    else
    {
        // this texture has an alpha channel
        if (this->GetHighQuality())
        {
            d3dSrcFormat = D3DFMT_A8R8G8B8;
            d3dDstFormat = D3DFMT_A8R8G8B8;
            ntxFormat = nNtxFile::FORMAT_A8R8G8B8;
            bytesPerPixel = 4;
        }
        else
        {
            d3dSrcFormat = D3DFMT_A4R4G4B4;
            d3dDstFormat = D3DFMT_A4R4G4B4;
            ntxFormat = nNtxFile::FORMAT_A4R4G4B4;
            bytesPerPixel = 2;
        }
    }

    // set first mipmap level as current block and get texture size
    int firstBlock = ntxFile.FindBlock(nNtxFile::TYPE_TEXTURE2D, ntxFormat, 0);
    n_assert(firstBlock != -1);
    ntxFile.SetCurrentBlock(firstBlock);
    int imgWidth  = ntxFile.GetWidth();
    int imgHeight = ntxFile.GetHeight();

    this->SetWidth(imgWidth);
    this->SetHeight(imgHeight);
    this->SetBytesPerPixel(bytesPerPixel);

    // create d3d texture
    hr = d3d8Dev->CreateTexture(
        imgWidth,                   // Width
        imgHeight,                  // Height
        this->gen_mipmaps ? 0 : 1,  // Levels
        0,                          // Usage
        d3dDstFormat,               // Format
        D3DPOOL_MANAGED,            // Pool
        &(this->tex));              // ppTexture
    if (FAILED(hr))
    {
        n_printf("nD3D8Texture: CreateTexture() failed with '%s'\n", nd3d8_Error(hr));
        goto fail;
    }

    // allocate a memory block for the image data
    dataSize = ntxFile.GetSize();
    imageData = n_malloc(dataSize);

    // read each block and write to d3d8 texture
    numLevels = this->tex->GetLevelCount();
    this->SetNumMipLevels(numLevels);
    int curLevel;
    for (curLevel = 0; curLevel < numLevels; curLevel++)
    {
        int curBlock = ntxFile.FindBlock(nNtxFile::TYPE_TEXTURE2D, ntxFormat, curLevel);
        if (curBlock != -1)
        {            
            // get current d3d8 mipmap surface
            IDirect3DSurface8* mipSurface;
            hr = this->tex->GetSurfaceLevel(curLevel, &mipSurface);
            if (FAILED(hr))
            {
                n_printf("nD3D8Texture: GetSurfaceLevel() failed with '%s'\n", nd3d8_Error(hr));
                goto fail;
            }

            // read block data
            ntxFile.SetCurrentBlock(curBlock);
            ntxFile.ReadBlock(imageData, dataSize);

            // fill a RECT structure with width and height
            RECT srcRect;
            srcRect.left   = 0;
            srcRect.top    = 0;
            srcRect.right  = ntxFile.GetWidth();
            srcRect.bottom = ntxFile.GetHeight();

            // transfer image data to d3d8 surface
            hr = D3DXLoadSurfaceFromMemory(
                mipSurface,                 // pDestSurface
                NULL,                       // pDestPalette (none)
                NULL,                       // pDestRect (entire surface)
                imageData,                  // pSrcMemory
                d3dSrcFormat,               // SrcFormat
                ntxFile.GetBytesPerRow(),   // SrcPitch
                NULL,                       // pSrcPalette
                &srcRect,                   // pSrcRect
                D3DX_FILTER_NONE,           // Filter (no filtering since no scaling takes place)
                0);                         // ColorKey (none)
            if (FAILED(hr))
            {
                n_printf("nD3D8Texture: D3DXLoadSurfaceFromMemory() failed with '%s'\n", nd3d8_Error(hr));
                goto fail;
            }

            // release the mipSurface (as required by GetSurfaceLevel())
            mipSurface->Release();
            mipSurface = 0;
        }
    }

    // success
    n_free(imageData);
    ntxFile.CloseRead();
    ntxFile.FreeBlocks();
    return true;

fail:
    // failure, cleanup and exit
    if (imageData)
    {
        n_free(imageData);
    }
    ntxFile.CloseRead();
    ntxFile.FreeBlocks();
    if (this->tex)
    {
        this->tex->Release();
        this->tex = 0;
    }
    return false;
}
//------------------------------------------------------------------------------
