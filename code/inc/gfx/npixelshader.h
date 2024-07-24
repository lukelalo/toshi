#ifndef N_PIXELSHADER_H
#define N_PIXELSHADER_H
//-------------------------------------------------------------------
/**
    @class nPixelShader

    @brief describe surface attributes

    nPixelShader replaces the obsolete nMaterial class for
    surface attribute definition. nPixelShader describes the
    complete pipeline through which each pixel has to 
    flow during rasterization until it reaches the framebuffer.

    The pixel shader describes a sequence of operations which
    are executed to yield a final color value which is written
    into the frame buffer. Each operation is described by
    2 input arguments and an opcode.

    These are the valid input arguments:
    ====================================

    tex
        The texel described by the texture read operation
        for the accompanying texture unit

    prev
        The result of the previous pixel shader stage

    const
        The predefined constant #N.

    These are the valid opcode arguments:
    =====================================
    mul[.1,.2,.4]  - multiply (modulate)   res = arg0 * arg1
    addu[.1,.2,.4] - unsigned add          res = arg0 + arg1
    adds[.1,.2,.4] - signed add            res = arg0 + arg1 - 0.5
    sub[.1,.2,.4]  - subtract              res = arg0 - arg1
    dot3[.1,.2,.4] - dotproduct3           res = arg0r*arg1r + arg0g*arg1g + arg0b*arg1b

    The postfix number defines an optional factor by which the
    result will be scaled.
    Separate operations may be defined for the rgb and alpha components
    of a pixel. 

    Texture Read Operation:
    =======================
    There are up to 'N' virtual texture units, each is described
    by a texture image (possibly mipmapped), a texel read operation
    (nearest, linear, ...), a texture coordinate generation mode
    (texture coordinates can either be read from the vertex, or
    can be generated automatically), and a texture coordinate
    transformation (a 4x4 matrix may be applied to texture coordinates).

    Other:
    ======
    nPixelShader also contains a bunch of conventional render states.
*/
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_PIXELSHADERDESC_H
#include "gfx/npixelshaderdesc.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nPixelShader
#include "kernel/ndefdllclass.h"

//-------------------------------------------------------------------
//  nPixelShader
//-------------------------------------------------------------------
class nGfxServer;
class nPixelShaderDesc;
class nTextureArray;
class N_PUBLIC nPixelShader : public nRoot {
protected:
    nAutoRef<nGfxServer> ref_gs;        // our gfx server
    nPixelShaderDesc *ps_desc;          // the generic shader description
    nTextureArray *tarray;              // only valid inside BeginRender()/EndRender()
    bool in_begin_render;

public:
    static nKernelServer *kernelServer;
 
    nPixelShader();
    virtual ~nPixelShader();

    virtual bool Compile(void);
    virtual int BeginRender(nTextureArray *);
    virtual void Render(int);
    virtual void EndRender(void);
    
    void SetShaderDesc(nPixelShaderDesc *psd) {
        n_assert(psd);
        this->ps_desc = psd;
        this->ps_desc->SetAllDirty();
    };
    nPixelShaderDesc* GetShaderDesc()
    {
        n_assert(this->ps_desc);
        return this->ps_desc;
    }
};

//-------------------------------------------------------------------
#endif
