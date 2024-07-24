#define N_IMPLEMENTS nGlTexture
//-------------------------------------------------------------------
//  ngltex_main.cc
//
//  (C) 1998 Andre Weissflog
//-------------------------------------------------------------------
#include "gfx/ngltexture.h"

#ifndef GL_ARB_multitexture
#error "ARB_multitexture not supported by compile environment!"
#endif

#ifdef __WIN32__
#define glActiveTextureARB gs->procActiveTextureARB
#endif

nNebulaClass(nGlTexture, "ntexture");

//-------------------------------------------------------------------
/**
    25-Nov-98   floh    created
*/
//-------------------------------------------------------------------
nGlTexture::nGlTexture()
          : ref_gs(kernelServer,this)
{
    this->pix_fmt   = NULL;
    this->tex_id    = 0;
    this->ref_gs    = "/sys/servers/gfx";
}

//-------------------------------------------------------------------
/**
    25-Nov-98   floh    created
*/
//-------------------------------------------------------------------
nGlTexture::~nGlTexture()
{
    this->Unload();
}

//-------------------------------------------------------------------
/**
    18-Feb-99   floh    created
    10-Jan-00   floh    mem_texture Variable
*/
//-------------------------------------------------------------------
void nGlTexture::Unload(void)
{
    if (0 != this->tex_id) {
        glDeleteTextures(1,&this->tex_id);
        this->tex_id = 0;
    }
    if (this->pix_fmt) {
        n_delete this->pix_fmt;
        this->pix_fmt = NULL;
    }
}

//-------------------------------------------------------------------
/**
    18-Feb-99   floh    created
    06-Jun-99   floh    Support fuer 'gfx_maxmiplevel' Hint 
    10-Jan-00   floh    + support for mem_texture
    11-Oct-00   floh    + rewritten for automatic mipmap generation
                        + dropped support for maxmiplevel
                        + dropped support for defining single
                          mipmap bmp files
    29-Aug-01   floh    + support for Nebula ntx files
*/
//-------------------------------------------------------------------
bool nGlTexture::Load(void)
{
    // unload the old stuff
    this->Unload();

    // FIXME: use old texture loader if extra alpha channel file given
    bool res = false;
    if (this->aname)
    {
        // if separate alpha channel file defined, use old texture loading code
        res = this->loadFileIntoTexture(this->pname,this->aname);
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
        n_printf("nGlTexture::Load(): Failed to load bitmap [%s,%s] into texture surface\n",
                 this->pname, this->aname ? this->aname : "<none>");
        this->Unload();
        return false;
    }
    return true;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
