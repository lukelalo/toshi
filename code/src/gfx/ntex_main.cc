#define N_IMPLEMENTS nTexture
//--------------------------------------------------------------------
//  ntex_main.cc 
//  (C) 1998-2000 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "gfx/ngfxtypes.h"
#include "gfx/nbmpfile.h"
#include "gfx/ntexture.h"
#include "gfx/ngfxserver.h"

nNebulaClass(nTexture, "nroot");

//--------------------------------------------------------------------
/**
    - 24-Nov-98   floh    created
    - 16-Feb-99   floh    + Mipmap-Handling
    - 11-Oct-00   floh    + removed mipmap stuff, mipmaps are now
                            generated automatically
    - 13-Aug-01   floh    + highQuality flag
*/
//--------------------------------------------------------------------
nTexture::nTexture() :
    ref_gs(kernelServer,this),
    pname(NULL),
    aname(NULL),
    line_numbytes(0),
    line(0),
    pf(0),
    numMipLevels(0),
    width(0),
    height(0),
    bytesPerPixel(0),
    curMipLod(0),
    gen_mipmaps(false),
    highQuality(false),
    useCount(0)
{
    this->ref_gs = "/sys/servers/gfx";
}

//--------------------------------------------------------------------
/**
    - 24-Nov-98   floh    created
    - 16-Feb-99   floh    + Mipmap-Handling
    - 16-Nov-00   floh    + see if we are one of the current textures,
                            and delete accordingly
*/
//--------------------------------------------------------------------
nTexture::~nTexture()
{
    n_assert(NULL == this->pf);
    n_assert(NULL == this->line);
    if (this->pname) n_free((void *)this->pname);
    if (this->aname) n_free((void *)this->aname);

    if (this->ref_gs.isvalid()) {
        int i;
        for (i=0; i<N_MAXNUM_TEXSTAGES; i++) {
            if (this == ref_gs->GetCurrentTexture(i)) {
                n_printf("~nTexture: clearing current texture\n");
                this->ref_gs->SetCurrentTexture(i,NULL);
            }
        }
    }
}

//--------------------------------------------------------------------
/**
    @param pn Filename containing pixel channel of texture.
    @param an Filename containing alpha channel of texture.

    Note that if 'an' is not NULL both the pixel and alpha file 
    have to be BMPs.

    - 17-Feb-99   floh    created
*/
//--------------------------------------------------------------------
bool nTexture::SetTexture(const char *pn, const char *an)
{
    if (this->pname) {
        n_free((void *)this->pname);
        this->pname = NULL;
    }
    if (this->aname) {
        n_free((void *)this->aname);
        this->aname = NULL;
    }
    if (pn) this->pname = n_strdup(pn);
    if (an) this->aname = n_strdup(an);
    return true;
}


//--------------------------------------------------------------------
/**
    @param fname Filename containing pixel channel of texture.
    @param aname Filename containing alpha channel of texture.

    - 24-Nov-98   floh    created
    - 27-Nov-99   floh    angepasst an neue nBmpFile Klasse
    - 17-Dec-99   floh    nochmal angepasst
    - 29-Feb-00   floh    Bugfix: when allocating pixel line buffer,
                          height was used instead of width, caused 
                          mem corruption with non-square textures...
*/
//--------------------------------------------------------------------
bool nTexture::BeginLoad(const char *fname, const char *aname)
{
    n_assert(fname);
    bool pix_ok;

    // try to open pixel file
    pix_ok = this->bmp_pixels.Open(fname,"rb");
    if (pix_ok) {
        bool alpha_ok = false;
        if (aname) {
            // separate alpha channel exists, load it
            alpha_ok = this->bmp_alpha.Open(aname,"rb");
            if (alpha_ok) {
                // alpha must be 8 bit CLUT and have identical size
                nPixelFormat *apf = this->bmp_alpha.GetPixelFormat();
                if ((apf->type == N_PFTYPE_CLUT) &&
                    (apf->bpp == 8) &&
                    (this->bmp_alpha.GetWidth()  == this->bmp_pixels.GetWidth()) &&
                    (this->bmp_alpha.GetHeight() == this->bmp_pixels.GetHeight()))
                {
                    // alpha channel ok
                    this->pf = n_new nPixelFormat(32,
                                                0x000000ff,
                                                0x0000ff00,
                                                0x00ff0000,
                                                0xff000000);
               
                } else {
                    n_warn("nTexture: alpha channel not 8bpp or wrong size");
                    this->bmp_alpha.Close();
                    this->bmp_pixels.Close();
                    return false;
                }
            } else {
                n_warn("nTexture: alpha channel defined, but loading failed");
                this->bmp_pixels.Close();
                return false;
            }
        } else {
            // no alpha channel defined...
            this->pf = n_new nPixelFormat(this->bmp_pixels.GetPixelFormat());
        }
    } else {
        n_warn("nTexture: loading pixel channel failed");
        return false;
    }
    this->pf->BeginConv(this->bmp_pixels.GetPixelFormat());
    this->line_numbytes = this->pf->bpp/8 * this->bmp_pixels.GetWidth();
    this->line = (uchar *) n_malloc(this->line_numbytes);
    return true;
}

//-------------------------------------------------------------------
/**
    Since BMP files are mirrored along Y, this routine returns the
    number of the line that has been loaded.

    - 24-Nov-98   floh    created
*/
//-------------------------------------------------------------------
long nTexture::LoadLine(void)
{
    int y = -1;    
    
    // read rgb channel
    if (this->bmp_pixels.IsOpen()) {
        uchar *l = this->bmp_pixels.ReadLine(y);
        if (!l) return -1L;
        this->pf->Conv(l,this->line,this->bmp_pixels.GetWidth());
    }

    // read optional alpha channel
    if (this->bmp_alpha.IsOpen()) {
        long i;
        int y;
        uchar *al = this->bmp_alpha.ReadLine(y);
        ulong *pl = (ulong *) this->line;
        struct nPalEntry *pal = this->bmp_alpha.GetPixelFormat()->pal;
        for (i=0; i < this->bmp_pixels.GetWidth(); i++) {
            ulong c = pl[i] & 0x00ffffff;
            c |= ((ulong)pal[al[i]].r) << 24;
            pl[i] = c;
        }
    }
    return y;   
}

//-------------------------------------------------------------------
/**
    - 24-Nov-98   floh    created
*/
//-------------------------------------------------------------------
void nTexture::EndLoad(void)
{
    if (this->bmp_pixels.IsOpen()) this->bmp_pixels.Close();
    if (this->bmp_alpha.IsOpen())  this->bmp_alpha.Close();
    if (this->pf) {
        n_delete this->pf;
        this->pf = NULL;
    }
    if (this->line) {
        n_free(this->line);
        this->line = NULL;
    }
}

//-------------------------------------------------------------------
/**
   - 16-Feb-99   floh    created
*/
//-------------------------------------------------------------------
bool nTexture::Load(void)
{
    n_error("pure virtual function called");
    return false;
}

//-------------------------------------------------------------------
/**
    - 16-Feb-99   floh    created
*/
//-------------------------------------------------------------------
void nTexture::Unload(void)
{
    n_error("pure virtual function called");
}

//------------------------------------------------------------------------------
/**
*/
int
nTexture::GetByteSize()
{
    int size = 0;
    if (this->gen_mipmaps)
    {
        // mipmapping enabled, compute resulting videomem upload size
        int i;
        for (i = this->curMipLod; i < this->numMipLevels; i++)
        {
            int mipWidth  = this->width >> i;
            int mipHeight = this->height >> i;
            if (mipWidth == 0)
            {
                mipWidth = 1;
            }
            if (mipHeight == 0)
            {
                mipHeight = 1;
            }
            size += mipWidth * mipHeight * this->bytesPerPixel;
        }
    }
    else
    {
        // no mipmapping, just return base size
        size = this->width * this->height * this->bytesPerPixel;
    }
    return size;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

