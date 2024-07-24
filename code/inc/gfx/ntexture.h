#ifndef N_TEXTURE_H
#define N_TEXTURE_H
//------------------------------------------------------------------------------
/**
    @class nTexture

    @brief nGfxServer-neutral texture object

    The nTexture class is the superclass for nGfxServer-specific
    texture classes. It contains a minimum interface for managing
    a list of textures as well as loading BMP files (8 and 24 bits).
    An optional alpha channel (8 bits BMP) can be loaded explicitly
    if required.

    Mipmapping:

    An nTexture object can manage an entire mipmap chain. By calling
    SetMipLevel() the current mipmap level is set, which can then be loaded
    using LoadFile(). The mipmap level is related to the size of the mipmap
    as follows. If n is the mipmap level and m is the size of the bitmap then,

    m = 2 ** n

    Level 0 corresponds to the bitmap size of 1x1, level 1 corresponds
    to a size of 2x2, level 2 corresponds to a size of 4x4 and so on.
    The nGfxServer has hardware-specific information which it uses to select
    the maximum texture size. An nTexture object ignores all mipmap sizes
    which exceed the set maximum size.

     - 11-Oct-00   floh    removed support for specific mipmap filenames
*/
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

#ifndef N_PIXELFORMAT_H
#include "gfx/npixelformat.h"
#endif

#ifndef N_BMPFILE_H
#include "gfx/nbmpfile.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

//------------------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nTexture
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nGfxServer;
class N_PUBLIC nTexture : public nRoot {
public:
    /// constructor
    nTexture();
    /// destructor
    virtual ~nTexture();
    
    /// set filenames of texture color and optional alpha file
    bool SetTexture(const char *, const char *);
    /// turn mipmap generation on/off
    void SetGenMipMaps(bool b);
    /// get mipmap generation status
    bool GetGenMipMaps();
    /// set high quality flag
    void SetHighQuality(bool b);
    /// get high quality flag
    bool GetHighQuality();
    /// get width of texture in pixels
    int GetWidth(void);
    /// get height of texture in pixels
    int GetHeight();
    /// get bytes per pixel
    int GetBytesPerPixel();
    /// get current byte size in videomem
    int GetByteSize();
    /// get number of mipmap levels
    int GetNumMipLevels();
    /// set current maximum mipmap LOD
    void SetMipLod(int lod);
    /// get current maximum mipmap LOD
    int GetMipLod();
    /// load from file resource into 3D API texture object
    virtual bool Load(void);
    /// unload 3D API texture object
    virtual void Unload(void);
    /// set per frame use count
    void SetUseCount(int count);
    /// get per frame use count
    int GetUseCount();
    /// OBSOLETE: begin loading a legacy bmp file
    bool BeginLoad(const char *, const char *);
    /// OBSOLETE: load a single line from a legacy bmp file
    long LoadLine(void);
    /// OBSOLETE: finish loading a legacy bmp file
    void EndLoad(void);

    static nKernelServer *kernelServer;

protected:
    /// set number of mipmap levels
    void SetNumMipLevels(int num);
    /// set width of texture in pixels
    void SetWidth(int w);
    /// set height of texture in pixels
    void SetHeight(int h);
    /// set bytes per pixel
    void SetBytesPerPixel(int bpp);

protected:
    nAutoRef<nGfxServer> ref_gs;
    const char *pname;
    const char *aname;

    int numMipLevels;       // number of mipmap levels (at least 1)
    int width;              // width of texture in pixels
    int height;             // height of texture in pixels
    int bytesPerPixel;      // bytes per pixel
    int curMipLod;          // current maximum mipmap LOD

    bool gen_mipmaps;       // if true, generate mipmaps
    bool highQuality;       // if true, don't sacrifice quality for speed
    int useCount;           // per frame use count for this texture 

public:
    nBmpFile bmp_pixels;
    nBmpFile bmp_alpha;     // optionaler Alphachannel
    nPixelFormat *pf;       // resultierendes Pixelformat
    ulong line_numbytes;
    uchar *line;            // resultierende Pixeldaten
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture::SetNumMipLevels(int num)
{
    n_assert(num > 0);
    this->numMipLevels = num;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTexture::GetNumMipLevels()
{
    return this->numMipLevels;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture::SetMipLod(int lod)
{
    if (lod < 0)
    {
        lod = 0;
    } 
    else if (lod >= this->numMipLevels)
    {
        lod = this->numMipLevels - 1;
    }
    this->curMipLod = lod;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTexture::GetMipLod()
{
    return this->curMipLod;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture::SetGenMipMaps(bool b)
{
    this->gen_mipmaps = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nTexture::GetGenMipMaps()
{
    return this->gen_mipmaps;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture::SetWidth(int w)
{
    this->width = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTexture::GetWidth()
{
    return this->width;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture::SetHeight(int h)
{
    this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTexture::GetHeight()
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture::SetBytesPerPixel(int bpp)
{
    this->bytesPerPixel = bpp;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTexture::GetBytesPerPixel()
{
    return this->bytesPerPixel;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture::SetHighQuality(bool b)
{
    this->highQuality = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nTexture::GetHighQuality()
{
    return this->highQuality;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTexture::SetUseCount(int count)
{
    this->useCount = count;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTexture::GetUseCount()
{
    return this->useCount;
}

//------------------------------------------------------------------------------
#endif

