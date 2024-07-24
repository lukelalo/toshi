#define N_IMPLEMENTS nGlTexture
//------------------------------------------------------------------------------
/**
    ngltex_old.cc

    (C) A.Weissflog 2001
*/
#include "gfx/ngltexture.h"

//------------------------------------------------------------------------------
/**
    Load bmp file into gl texture the old style (using own quick'n dirty
    bmp loader). Will reserve an OpenGL texture name for the new texture,
    which is written to this->tex_id.

    @param  pn      path to pixel bmp file
    @param  an      path to optional alpha channel bmp file (can be NULL)

    @return         true if texture successfully loaded and bound
*/
bool nGlTexture::loadFileIntoTexture(const char *pn, const char *an)
{
    n_assert(0 == this->tex_id);
    if (this->BeginLoad(pn,an)) {

        int w,h,bpr;

        // determine pixel format
        if (this->pf->bpp == 8) {
            // convert 8bpp file to 24 bit texture during load time
            this->pix_fmt = n_new nPixelFormat(24,
                                0x00ff0000,
                                0x0000ff00,
                                0x000000ff,
                                0x00000000);
        } else if (this->pf->bpp == 24) {
            this->pix_fmt = n_new nPixelFormat(24,
                                0x00ff0000,
                                0x0000ff00,
                                0x000000ff,
                                0x00000000);
        } else if (this->pf->bpp == 32) {
            this->pix_fmt = n_new nPixelFormat(32,
                                0x000000ff,
                                0x0000ff00,
                                0x00ff0000,
                                0xff000000);
        }

        // set pixel alignment to 1
        glPixelStorei(GL_PACK_ALIGNMENT,1);

        w = this->bmp_pixels.GetWidth();
        h = this->bmp_pixels.GetHeight();
        bpr = w * (this->pix_fmt->bpp / 8);
        
        // update my texture attributes
        this->SetWidth(w);
        this->SetHeight(h);
        this->SetNumMipLevels(1);   // FIXME: for simplicity, BMP loading is obsolete anyway
        this->SetBytesPerPixel(this->pix_fmt->bpp / 8);
        
        uchar *src_buf = (uchar *) n_malloc(bpr * h);
        
        // load line by line and do pixel conversion
        long l;
        this->pix_fmt->BeginConv(this->pf);
        while ((l=this->LoadLine()) != -1) {
            uchar *addr = src_buf + l*bpr;
            this->pix_fmt->Conv(this->line,addr,w);
        }
        this->pix_fmt->EndConv();

        // finish loading...
        this->EndLoad();

        // transfer the texture data to the gl
        glGenTextures(1, &this->tex_id);
        glBindTexture(GL_TEXTURE_2D, this->tex_id);

        if (this->pix_fmt->bpp == 24) {
            if (this->gen_mipmaps) {
                // build mipmap chain
                gluBuild2DMipmaps(GL_TEXTURE_2D,    // target
                                  GL_RGB4,          // number of color components
                                  w,                // width
                                  h,                // height
                                  GL_RGB,           // format
                                  GL_UNSIGNED_BYTE, // type
                                  src_buf);
            } else {
                // no mipmaps
                glTexImage2D(GL_TEXTURE_2D,     // target
                             0,                 // level
                             GL_RGB4,           // components (RGB)
                             w,                 // width
                             h,                 // height
                             0,                 // border
                             GL_RGB,            // format
                             GL_UNSIGNED_BYTE,  // type
                             src_buf);
            }
        } else if (this->pix_fmt->bpp == 32) {
            if (this->gen_mipmaps) {
                // build mipmap chain
                gluBuild2DMipmaps(GL_TEXTURE_2D,    // target
                                  GL_RGBA4,         // number of color components
                                  w,                // width
                                  h,                // height
                                  GL_RGBA,          // format
                                  GL_UNSIGNED_BYTE, // type
                                  src_buf);
            } else {
                // no mipmaps
                glTexImage2D(GL_TEXTURE_2D,     // target
                             0,                 // level
                             GL_RGBA4,          // components (RGBA)
							 w,                 // width
                             h,                 // height
                             0,                 // border
                             GL_RGBA,           // format
                             GL_UNSIGNED_BYTE,  // type
                             src_buf);
            }
        }

        if (src_buf) n_free(src_buf);
    }
    return true;
}
