#ifndef N_GLPIXELSHADER_H
#define N_GLPIXELSHADER_H
//--------------------------------------------------------------------
/**
    @class nGlPixelShader

    @brief OpenGL implementation of pixel shader
*/
//--------------------------------------------------------------------
#ifndef N_GLSERVER_H
#include "gfx/nglserver.h"
#endif

#ifndef N_PIXELSHADER_H
#include "gfx/npixelshader.h"
#endif

#ifndef N_PIXELSHADERDESC_H
#include "gfx/npixelshaderdesc.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nGlPixelShader
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nPSI;
class nTextureUnit;
class nGlServer;
class N_DLLCLASS nGlPixelShader : public nPixelShader {

    friend class nGlVertexPool;
    
    int num_passes;
    int pass_map_base[nPixelShaderDesc::MAXSTAGES]; // the base stage for pass N
    int pass_map_num[nPixelShaderDesc::MAXSTAGES];  // the number of ops for pass N
    GLuint dlist[nPixelShaderDesc::MAXSTAGES]; // the display lists reserved to hold OpenGL commands

public:
    static nKernelServer *kernelServer;

    nGlPixelShader();
    virtual ~nGlPixelShader();
    virtual bool Compile(void);
    virtual int BeginRender(nTextureArray *);
    virtual void Render(int);
    virtual void EndRender(void);

private:
    void set_texture_objects(int, nGlServer *);
    void emit_pass(int);
    void emit_simple_states(nPixelShaderDesc&);
    void emit_blend_op(nPSI&, nPSI&, nGlServer *);
    void emit_fbuffer_blend_op(nPSI&, nPSI&, nGlServer *);
    void emit_pass_multitexture(nPixelShaderDesc&, nGlServer *, int);
    void emit_pass_no_multitexture(nPixelShaderDesc&, nGlServer *, int);
    void emit_texture_states(nGlServer *, nTextureUnit&);
};
//--------------------------------------------------------------------
#endif
