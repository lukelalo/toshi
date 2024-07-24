#ifndef N_GLTEXTURE_H
#define N_GLTEXTURE_H
//-------------------------------------------------------------------
/**
    @class nGlTexture

    @brief wraps OpenGL texture into nTexture object

    (C) 1999 A.Weissflog
*/
//-------------------------------------------------------------------
#ifndef N_TEXTURE_H
#include "gfx/ntexture.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_GLSERVER_H
#include "gfx/nglserver.h"
#endif
//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nGlTexture
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nGlServer;
class nGlPixelShader;
class N_DLLCLASS nGlTexture : public nTexture {
    
    friend class nGlPixelShader;

public:
    nGlTexture();
    virtual ~nGlTexture();
    virtual bool Load(void);
    virtual void Unload(void);

    static nKernelServer *kernelServer;

private:
    bool loadFileIntoTexture(const char *, const char *);
    bool loadFileIntoTextureIL(const char*);
    bool loadFileIntoTextureNTX(const char*);

    nAutoRef<nGlServer> ref_gs;
    GLuint tex_id;
    nPixelFormat *pix_fmt;
};
//--------------------------------------------------------------------
#endif


