#ifndef N_D3D8PIXELSHADER_H
#define N_D3D8PIXELSHADER_H
//------------------------------------------------------------------------------
/**
    @class nD3D8PixelShader

    @brief Implement pixel shader class for D3D8

    The nD3D8PixelShader class does not (yet?) use the d3d8 pixel shader,
    but uses the oldfashioned fixed function multitexture pipeline.
*/
#ifndef N_D3D8SERVER_H
#include "gfx/nd3d8server.h"
#endif

#ifndef N_PIXELSHADER_H
#include "gfx/npixelshader.h"
#endif

#ifndef N_PIXELSHADERDESC_H
#include "gfx/npixelshaderdesc.h"
#endif

#undef N_DEFINES
#define N_DEFINES nD3D8PixelShader
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nD3D8PixelShader : public nPixelShader
{
public:
    /// the constructor
    nD3D8PixelShader();
    /// the destructor
    virtual ~nD3D8PixelShader();

    /// compile pixel shader desc
    virtual bool Compile(void);
    /// start rendering the pixel shader
    virtual int BeginRender(nTextureArray *);
    /// render a pixel shader pass
    virtual void Render(int);
    /// finish rendering the pixel shader
    virtual void EndRender(void);

    static nKernelServer *kernelServer;

private:
    void set_texture_objects(nD3D8Server *, int);
    void emit_simple_states(nD3D8Server *, nPixelShaderDesc *);
    void emit_texture_states(nD3D8Server *, nTextureUnit&, int, int);
    void emit_fbuffer_blend_op(nD3D8Server *, int, nPSI&, nPSI&);
    D3DTEXTUREOP translate_pixel_op(nPSI&, D3DCAPS8 *);
    void emit_blend_op(nD3D8Server *, int, nPSI&, nPSI&);
    void emit_pass(nPixelShaderDesc *, nD3D8Server *, int);
    DWORD arg2d3d(nPSI::nArg arg);

    int num_passes;
    int pass_map_base[nPixelShaderDesc::MAXSTAGES];     // the base stage for pass N
    int pass_map_num[nPixelShaderDesc::MAXSTAGES];      // the number of ops for pass N
    DWORD state_block[nPixelShaderDesc::MAXSTAGES];     // state block handle (one for each pass)
    bool spheremap_hint[nPixelShaderDesc::MAXSTAGES];
    bool objectspace_hint[nPixelShaderDesc::MAXSTAGES];

};
//--------------------------------------------------------------------
#endif
