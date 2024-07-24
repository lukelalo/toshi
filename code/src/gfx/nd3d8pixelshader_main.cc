#define N_IMPLEMENTS nD3D8PixelShader
//------------------------------------------------------------------------------
//  nd3d8pixelshader_main.cc
//  (C) 2001 A.Weissflog
//------------------------------------------------------------------------------
#include "gfx/nd3d8pixelshader.h"
#include "gfx/nd3d8texture.h"
#include "gfx/ntexturearray.h"

nNebulaClass(nD3D8PixelShader, "npixelshader");

extern const char *nd3d8_Error(HRESULT hr);

//------------------------------------------------------------------------------
/**
*/
nD3D8PixelShader::nD3D8PixelShader()
{
    this->num_passes = 0;
    memset(this->pass_map_base,     0,  sizeof(this->pass_map_base));
    memset(this->pass_map_num,      0,  sizeof(this->pass_map_num));
    memset(this->state_block,       0,  sizeof(this->state_block));
    memset(this->spheremap_hint,    0,  sizeof(this->spheremap_hint));
    memset(this->objectspace_hint,  0,  sizeof(this->objectspace_hint));
}

//------------------------------------------------------------------------------
/**
    @brief Delete any used d3d8 state blocks
*/
nD3D8PixelShader::~nD3D8PixelShader()
{
    nD3D8Server *gs = (nD3D8Server *) this->ref_gs.get();
    n_assert(gs->d3d8Dev);

    // delete state blocks...
    HRESULT hr;
    int i;
    for (i=0; i<nPixelShaderDesc::MAXSTAGES; i++) {
        if (this->state_block[i]) {
            hr = gs->d3d8Dev->DeleteStateBlock(this->state_block[i]);
            if (FAILED(hr)) {
                n_printf("nD3D8PixelShader: DeleteStateBlock() failed with '%s'\n",nd3d8_Error(hr));
            }
            this->state_block[i] = NULL;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Convert an nPSI::nArg into a D3DTA_* enum.

    @param  arg     the pixel shader argument (nPSI::nArg)
    @return         the d3d texture argument (D3DTA_*)
*/
DWORD 
nD3D8PixelShader::arg2d3d(nPSI::nArg arg)
{
    DWORD d3dta = D3DTA_TEXTURE;
    switch (arg & nPSI::nArg::ARGMASK) {
        case nPSI::nArg::TEX:       d3dta = D3DTA_TEXTURE; break;
        case nPSI::nArg::PREV:      d3dta = D3DTA_CURRENT; break;
        case nPSI::nArg::CONSTANT:  d3dta = D3DTA_TFACTOR; break;
        case nPSI::nArg::PRIMARY:   d3dta = D3DTA_DIFFUSE; break;
    }
    if (arg & nPSI::nArg::ONE_MINUS) d3dta |= D3DTA_COMPLEMENT;
    return d3dta;
}

//------------------------------------------------------------------------------
/**
    @brief Update the D3D textures for this pass.

    @param  gs      the nD3D8Server object
    @param  pass    number of current pass
*/
void 
nD3D8PixelShader::set_texture_objects(nD3D8Server *gs, int pass)
{
    HRESULT hr;
    IDirect3DDevice8 *dev = gs->d3d8Dev;

    // set texture objects
    int i=0;
    if (this->tarray) {
        for (; i<this->pass_map_num[pass]; i++) {
            int stage = this->pass_map_base[pass]+i;
            nD3D8Texture *d3dtex = (nD3D8Texture *) this->tarray->GetTexture(stage);
            if (gs->GetCurrentTexture(i) != d3dtex) {

// n_printf("upd_texture (stage %d)\n", i);

                if (d3dtex) {
                    IDirect3DTexture8* tex = d3dtex->GetD3DTexture();
                    n_assert(tex);

                    // FIXME: is it really necessary to explicitely
                    // set a NULL texture before setting another texture???
                    hr = dev->SetTexture(i, tex);
                    if (FAILED(hr)) {
                        n_printf("nD3D8PixelShader: SetTexture() failed with '%s'\n",nd3d8_Error(hr));
                    }
                    gs->SetCurrentTexture(i, (nTexture *)d3dtex);
                } else {
                    // no texture object, disable texturing for this stage
                    hr = dev->SetTexture(i, NULL);
                    hr = dev->SetTextureStageState(i,D3DTSS_COLOROP,D3DTOP_DISABLE);
                    hr = dev->SetTextureStageState(i,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
                    gs->SetCurrentTexture(i, NULL);
                }
            }
        }
    }

    // disable the first unused texture unit
    hr = dev->SetTexture(i, NULL);
    hr = dev->SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_DISABLE);
    hr = dev->SetTextureStageState(i, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    if (i<N_MAXNUM_TEXSTAGES)
        gs->SetCurrentTexture(i,NULL);
}

//------------------------------------------------------------------------------
/**
    @brief Write render states which are not related to a texture stage.

    @param  gs      the nD3D8Server object
    @param  psd     the nPixelShaderDesc object

    - 12-Jul-01   floh    + alphatest stuff
	- 28-Jul-01   leaf    invert alpha test alpharef to match OpenGL

*/
void 
nD3D8PixelShader::emit_simple_states(nD3D8Server *gs, nPixelShaderDesc *psd)
{
    IDirect3DDevice8 *dev = gs->d3d8Dev;

    // boolean stuff...
    DWORD r;
    if (psd->light_enable) r = TRUE;
    else                   r = FALSE;
    dev->SetRenderState(D3DRS_LIGHTING, r);

    if (psd->alpha_enable)  r = TRUE;
    else                    r = FALSE;
    dev->SetRenderState(D3DRS_ALPHABLENDENABLE,r);

    if (psd->zwrite_enable) r = TRUE;
    else                    r = FALSE;
    dev->SetRenderState(D3DRS_ZWRITEENABLE,r);
    
	if (psd->wireframe_enable)
		dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    else
		dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
	if (psd->normalizenormals_enable) {
		dev->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);
	} else {
		dev->SetRenderState(D3DRS_NORMALIZENORMALS,FALSE);
	}

    // alpha blending factors
    switch (psd->alpha_src_blend) {
        case N_ABLEND_ZERO:         r = D3DBLEND_ZERO;         break;
        case N_ABLEND_ONE:          r = D3DBLEND_ONE;          break;
        case N_ABLEND_SRCCOLOR:     r = D3DBLEND_SRCCOLOR;     break;
        case N_ABLEND_INVSRCCOLOR:  r = D3DBLEND_INVSRCCOLOR;  break;
        case N_ABLEND_SRCALPHA:     r = D3DBLEND_SRCALPHA;     break;
        case N_ABLEND_INVSRCALPHA:  r = D3DBLEND_INVSRCALPHA;  break;
        case N_ABLEND_DESTALPHA:    r = D3DBLEND_DESTALPHA;    break;
        case N_ABLEND_INVDESTALPHA: r = D3DBLEND_INVDESTALPHA; break;
        case N_ABLEND_DESTCOLOR:    r = D3DBLEND_DESTCOLOR;    break;
        case N_ABLEND_INVDESTCOLOR: r = D3DBLEND_INVDESTCOLOR; break;
        default:                    r = D3DBLEND_ZERO;         break;
    }
    dev->SetRenderState(D3DRS_SRCBLEND,r);

    switch (psd->alpha_dest_blend) {
        case N_ABLEND_ZERO:         r = D3DBLEND_ZERO;         break;
        case N_ABLEND_ONE:          r = D3DBLEND_ONE;          break;
        case N_ABLEND_SRCCOLOR:     r = D3DBLEND_SRCCOLOR;     break;
        case N_ABLEND_INVSRCCOLOR:  r = D3DBLEND_INVSRCCOLOR;  break;
        case N_ABLEND_SRCALPHA:     r = D3DBLEND_SRCALPHA;     break;
        case N_ABLEND_INVSRCALPHA:  r = D3DBLEND_INVSRCALPHA;  break;
        case N_ABLEND_DESTALPHA:    r = D3DBLEND_DESTALPHA;    break;
        case N_ABLEND_INVDESTALPHA: r = D3DBLEND_INVDESTALPHA; break;
        case N_ABLEND_DESTCOLOR:    r = D3DBLEND_DESTCOLOR;    break;
        case N_ABLEND_INVDESTCOLOR: r = D3DBLEND_INVDESTCOLOR; break;
        default:                    r = D3DBLEND_ZERO;         break;
    };
    dev->SetRenderState(D3DRS_DESTBLEND,r);

    // the z test function
    switch (psd->zfunc) {
        case N_CMP_NEVER:           r=D3DCMP_NEVER;         break;
        case N_CMP_LESS:            r=D3DCMP_LESS;          break;
        case N_CMP_EQUAL:           r=D3DCMP_EQUAL;         break;
        case N_CMP_LESSEQUAL:       r=D3DCMP_LESSEQUAL;     break;
        case N_CMP_GREATER:         r=D3DCMP_GREATER;       break;
        case N_CMP_NOTEQUAL:        r=D3DCMP_NOTEQUAL;      break;
        case N_CMP_GREATEREQUAL:    r=D3DCMP_GREATEREQUAL;  break;
        case N_CMP_ALWAYS:          r=D3DCMP_ALWAYS;        break;
        default:                    r=D3DCMP_LESSEQUAL;     break;
    }
    dev->SetRenderState(D3DRS_ZFUNC,r);
 
    // the cull mode
    switch (psd->cull_mode) {
        case N_CULL_NONE:   r=D3DCULL_NONE; break;
        case N_CULL_CW:     r=D3DCULL_CCW; break;
        case N_CULL_CCW:    r=D3DCULL_CW; break;
        default:            r=D3DCULL_CW;
    }
    dev->SetRenderState(D3DRS_CULLMODE,r);

    // color material source
    switch (psd->color_material) {
        case N_CMAT_MATERIAL:
            dev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
            dev->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
            dev->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
            dev->SetRenderState(D3DRS_COLORVERTEX, FALSE);
            break;
        case N_CMAT_DIFFUSE:
            dev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
            dev->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
            dev->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
            dev->SetRenderState(D3DRS_COLORVERTEX, TRUE);
            break;
        case N_CMAT_EMISSIVE:
            dev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
            dev->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
            dev->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
            dev->SetRenderState(D3DRS_COLORVERTEX, TRUE);
            break;
        case N_CMAT_AMBIENT:
            dev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
            dev->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
            dev->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
            dev->SetRenderState(D3DRS_COLORVERTEX, TRUE);
            break;
    }

    // alphatest stuff
    if (psd->alphatest_enable)
    {
        dev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        switch (psd->alphafunc) 
        {
            case N_CMP_NEVER:           r = D3DCMP_NEVER;         break;
            case N_CMP_LESS:            r = D3DCMP_LESS;          break;
            case N_CMP_EQUAL:           r = D3DCMP_EQUAL;         break;
            case N_CMP_LESSEQUAL:       r = D3DCMP_LESSEQUAL;     break;
            case N_CMP_GREATER:         r = D3DCMP_GREATER;       break;
            case N_CMP_NOTEQUAL:        r = D3DCMP_NOTEQUAL;      break;
            case N_CMP_GREATEREQUAL:    r = D3DCMP_GREATEREQUAL;  break;
            case N_CMP_ALWAYS:          r = D3DCMP_ALWAYS;        break;
            default:                    r = D3DCMP_LESSEQUAL;     break;
        }
        dev->SetRenderState(D3DRS_ALPHAFUNC, r);
		// invert alpharef to match OpenGL
        DWORD alpharef = (DWORD) ((1.0f-psd->alpharef) * 255.0f);
        dev->SetRenderState(D3DRS_ALPHAREF, alpharef);
    }
    else
    {
        dev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    }
}

//------------------------------------------------------------------------------
/**
    @brief Emit texture stage states which describe how a texel should be sampled from
    the texture data.
*/
void 
nD3D8PixelShader::emit_texture_states(
    nD3D8Server *gs,
    nTextureUnit& tu,
    int pixelshader_stage,
    int stage)
{
    IDirect3DDevice8 *dev = gs->d3d8Dev;

    // texture address
    DWORD r;
    switch (tu.addr_u) {
        case N_TADDR_WRAP:  r=D3DTADDRESS_WRAP; break;
        case N_TADDR_CLAMP: r=D3DTADDRESS_CLAMP; break;
        default:            r=D3DTADDRESS_WRAP; break;
    }
    dev->SetTextureStageState(stage,D3DTSS_ADDRESSU,r);

    switch (tu.addr_v) {
        case N_TADDR_WRAP:  r=D3DTADDRESS_WRAP; break;
        case N_TADDR_CLAMP: r=D3DTADDRESS_CLAMP; break;
        default:            r=D3DTADDRESS_WRAP; break;
    }
    dev->SetTextureStageState(stage,D3DTSS_ADDRESSV,r);

    // min filter
    D3DTEXTUREFILTERTYPE min = D3DTEXF_POINT;
    D3DTEXTUREFILTERTYPE mip = D3DTEXF_NONE;
    switch (tu.filter_min) {
        case N_TFILTER_NEAREST:
            min = D3DTEXF_POINT;
            mip = D3DTEXF_NONE;
            break;
        case N_TFILTER_LINEAR:
            min = D3DTEXF_LINEAR;
            mip = D3DTEXF_NONE;
            break;
        case N_TFILTER_NEAREST_MIPMAP_NEAREST:
            min = D3DTEXF_POINT;
            mip = D3DTEXF_POINT;
            break;
        case N_TFILTER_LINEAR_MIPMAP_NEAREST:
            min = D3DTEXF_LINEAR;
            mip = D3DTEXF_POINT;
            break;
        case N_TFILTER_NEAREST_MIPMAP_LINEAR:
            min = D3DTEXF_POINT;
            mip = D3DTEXF_LINEAR;
            break;
        case N_TFILTER_LINEAR_MIPMAP_LINEAR:
            min = D3DTEXF_LINEAR;
            mip = D3DTEXF_LINEAR;
        default: break;
    }
    dev->SetTextureStageState(stage, D3DTSS_MINFILTER, min);
    dev->SetTextureStageState(stage, D3DTSS_MIPFILTER, mip);

    // mag filter
    switch (tu.filter_mag) {
        case N_TFILTER_NEAREST: r = D3DTEXF_POINT; break;
        case N_TFILTER_LINEAR:  r = D3DTEXF_LINEAR; break;
        default:                r = D3DTEXF_LINEAR; break;
    }
    dev->SetTextureStageState(stage, D3DTSS_MAGFILTER, r);

    // texture coordinate source
    D3DTEXTURETRANSFORMFLAGS tf;
    if (tu.enable_transform) tf=D3DTTFF_COUNT2;
    else                     tf=D3DTTFF_DISABLE;
    dev->SetTextureStageState(stage,D3DTSS_TEXTURETRANSFORMFLAGS,tf);

    this->spheremap_hint[pixelshader_stage] = false;
    switch(tu.coord_src) {
        case N_TCOORDSRC_UV0:
            dev->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU|0);
            break;
        case N_TCOORDSRC_UV1:
            dev->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU|1);
            break;
        case N_TCOORDSRC_UV2:
            dev->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU|2);
            break;
        case N_TCOORDSRC_UV3:
            dev->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU|3);
            break;

        case N_TCOORDSRC_OBJECTSPACE:
            dev->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION|stage);
            this->objectspace_hint[pixelshader_stage] = true;
            this->ps_desc->SetTransformChanged(true);
            break;

        case N_TCOORDSRC_EYESPACE:
            dev->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION|stage);
            break;

        case N_TCOORDSRC_SPHEREMAP:
            dev->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACENORMAL|stage);
            
            // the texture coordinates generated here are
            // in the range of -1.0 to +1.0, but for sphere mapping
            // we need it in the range of 0..1.0, thus we'll have to
            // setup the texture matrix to scale and translate by 0.5,
            // we hint this to the texture matrix setup code like this:
            this->spheremap_hint[pixelshader_stage] = true;
            this->ps_desc->SetTransformChanged(true);
            break;
        default: break;
    }
}

//------------------------------------------------------------------------------
/**
    Emit the render states describing the current pixel
    operation where the 'previous' stage is the frame
    buffer. This is necessary if multipass rendering must be
    used.
*/
void 
nD3D8PixelShader::emit_fbuffer_blend_op(
    nD3D8Server *gs,
    int stage,
    nPSI& color_op,
    nPSI& alpha_op)
{
    IDirect3DDevice8 *dev = gs->d3d8Dev;

    // feed the incoming color directly
    dev->SetTextureStageState(stage, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
    dev->SetTextureStageState(stage, D3DTSS_COLORARG1, arg2d3d(color_op.args[0]));

    // color_op for alpha is NO TYPO!!!
    dev->SetTextureStageState(stage, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
    dev->SetTextureStageState(stage, D3DTSS_ALPHAARG1, arg2d3d(color_op.args[0]));

    // enable alpha blending and define the alpha blending
    // operation which simulates the pixel operation
    dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    // we ignore the alpha op, also most accelerators don't have
    // destination alpha, which is a problem if the operation 
    // wants to use prev.a
    D3DBLEND src,dst;
    switch (color_op.op) {
        case nPSI::nOp::ADD:
        case nPSI::nOp::ADDS:
            src = D3DBLEND_ONE;
            dst = D3DBLEND_ONE;
            break;
        case nPSI::nOp::IPOL:
            src = D3DBLEND_SRCALPHA;
            dst = D3DBLEND_INVSRCALPHA;
            break;
        case nPSI::nOp::MUL:
        default:
            src = D3DBLEND_ZERO;
            dst = D3DBLEND_SRCCOLOR;
            break;
    }
    dev->SetRenderState(D3DRS_SRCBLEND,src);
    dev->SetRenderState(D3DRS_DESTBLEND,dst);

    // avoid zbuffer fighting and turn off some unnecessary stuff...
    dev->SetRenderState(D3DRS_ZBIAS, 1);
    dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
}

//------------------------------------------------------------------------------
/**
    @brief Translate a pixel shader op into a D3D texture stage op.
*/
D3DTEXTUREOP 
nD3D8PixelShader::translate_pixel_op(nPSI& psi, D3DCAPS8 *desc)
{
    D3DTEXTUREOP d3d_op = D3DTOP_DISABLE;
    switch (psi.op) {
        //--- replace ---
        case nPSI::nOp::REPLACE:
            d3d_op = D3DTOP_SELECTARG1;
            break;

        //--- mul, mul.2, mul.4 ---
        case nPSI::nOp::MUL:
            switch (psi.scale) {
                case nPSI::nScale::ONE:
                    if (desc->TextureOpCaps & D3DTEXOPCAPS_MODULATE) {
                        d3d_op = D3DTOP_MODULATE;
                    } else {
                        n_printf("nD3D8PixelShader: D3DTOP_MODULATE not supported!\n");
                        d3d_op = D3DTOP_SELECTARG1;
                    }
                    break;
            
                case nPSI::nScale::TWO:
                    if (desc->TextureOpCaps & D3DTEXOPCAPS_MODULATE2X) {
                        d3d_op = D3DTOP_MODULATE2X;
                    } else {
                        n_printf("nD3D8PixelShader: D3DTOP_MODULATE2X not supported!\n");
                        if (desc->TextureOpCaps & D3DTEXOPCAPS_MODULATE) {
                            d3d_op = D3DTOP_MODULATE;
                        } else {
                            n_printf("nD3D8PixelShader: D3DTOP_MODULATE not supported!\n");
                            d3d_op = D3DTOP_SELECTARG1;
                        }
                    }
                    break;

                case nPSI::nScale::FOUR:
                    if (desc->TextureOpCaps & D3DTEXOPCAPS_MODULATE4X) {
                        d3d_op = D3DTOP_MODULATE4X;
                    } else {
                        n_printf("nD3D8PixelShader: D3DTOP_MODULATE4X not supported!\n");
                        if (desc->TextureOpCaps & D3DTEXOPCAPS_MODULATE2X) {
                            d3d_op = D3DTOP_MODULATE2X;
                        } else {
                            n_printf("nD3D8PixelShader: D3DTOP_MODULATE2X not supported!\n");
                            if (desc->TextureOpCaps & D3DTEXOPCAPS_MODULATE) {
                                d3d_op = D3DTOP_MODULATE;
                            } else {
                                n_printf("nD3D8PixelShader: D3DTOP_MODULATE not supported!\n");
                                d3d_op = D3DTOP_SELECTARG1;
                            }
                        }
                    }
                    break;
            }
            break;

        //--- add ---
        case nPSI::nOp::ADD:
            if (desc->TextureOpCaps & D3DTEXOPCAPS_ADD) {
                d3d_op = D3DTOP_ADD;
            } else {
                n_printf("nD3D8PixelShader: D3DTOP_ADD not supported!\n");
                d3d_op = D3DTOP_SELECTARG1;
            }
            break;

        //--- adds ---
        case nPSI::nOp::ADDS:
            switch (psi.scale) {
                case nPSI::nScale::ONE:
                    if (desc->TextureOpCaps & D3DTEXOPCAPS_ADDSIGNED) {
                        d3d_op = D3DTOP_ADDSIGNED;
                    } else {
                        n_printf("nD3D8PixelShader: D3DTOP_ADDSIGNED not supported!\n");
                        if (desc->TextureOpCaps & D3DTEXOPCAPS_ADD) {
                            d3d_op = D3DTOP_ADD;
                        } else {
                            n_printf("nD3D8PixelShader: D3DTOP_ADD not supported!\n");
                            d3d_op = D3DTOP_SELECTARG1;
                        }
                    }
                    break;

                case nPSI::nScale::TWO:
                case nPSI::nScale::FOUR:
                    if (desc->TextureOpCaps & D3DTEXOPCAPS_ADDSIGNED2X) {
                        d3d_op = D3DTOP_ADDSIGNED2X;
                    } else {
                        n_printf("nD3D8PixelShader: D3DTOP_ADDSIGNED2X not supported!\n");
                        if (desc->TextureOpCaps & D3DTEXOPCAPS_ADDSIGNED) {
                            d3d_op = D3DTOP_ADDSIGNED;
                        } else {
                            n_printf("nD3D8PixelShader: D3DTOP_ADDSIGNED not supported!\n");
                            if (desc->TextureOpCaps & D3DTEXOPCAPS_ADD) {
                                d3d_op = D3DTOP_ADD;
                            } else {
                                n_printf("nD3D8PixelShader: D3DTOP_ADD not supported!\n");
                                d3d_op = D3DTOP_SELECTARG1;
                            }
                        }
                    }
                    break;
            }
            break;

        //--- ipol ---
        case nPSI::nOp::IPOL:
            switch (psi.args[2] & nPSI::nArg::ARGMASK) {
                case nPSI::nArg::TEX:
                    if (desc->TextureOpCaps & D3DTEXOPCAPS_BLENDTEXTUREALPHA) {
                        d3d_op = D3DTOP_BLENDTEXTUREALPHA;
                    } else {
                        n_printf("nD3D8PixelShader: D3DTOP_BLENDTEXTUREALPHA not supported!\n");
                        d3d_op = D3DTOP_SELECTARG1;
                    }
                    break;

                case nPSI::nArg::PREV:
                    if (desc->TextureOpCaps & D3DTEXOPCAPS_BLENDCURRENTALPHA) {
                        d3d_op = D3DTOP_BLENDCURRENTALPHA;
                    } else {
                        n_printf("nD3D8PixelShader: D3DTOP_BLENDCURRENTALPHA not supported!\n");
                        if (desc->TextureOpCaps & D3DTEXOPCAPS_BLENDTEXTUREALPHA) {
                            d3d_op = D3DTOP_BLENDTEXTUREALPHA;
                        } else {
                            n_printf("nD3D8PixelShader: D3DTOP_BLENDTEXTUREALPHA not supported!\n");
                            d3d_op = D3DTOP_SELECTARG1;
                        }
                    }
                    break;


                case nPSI::nArg::CONSTANT:
                    if (desc->TextureOpCaps & D3DTEXOPCAPS_BLENDFACTORALPHA) {
                        d3d_op = D3DTOP_BLENDFACTORALPHA;
                    } else {
                        n_printf("nD3D8PixelShader: D3DTOP_BLENDFACTORALPHA not supported!\n");
                        if (desc->TextureOpCaps & D3DTEXOPCAPS_BLENDTEXTUREALPHA) {
                            d3d_op = D3DTOP_BLENDTEXTUREALPHA;
                        } else {
                            n_printf("nD3D8PixelShader: D3DTOP_BLENDTEXTUREALPHA not supported!\n");
                            d3d_op = D3DTOP_SELECTARG1;
                        }
                    }
                    break;
            }
            break;

        //--- dot ---
        case nPSI::nOp::DOT:
            if (desc->TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3) {
                d3d_op = D3DTOP_DOTPRODUCT3;
            } else {
                n_printf("nD3D8PixelShader: D3DTOP_DOTPRODUCT3 not supported!\n");
                d3d_op = D3DTOP_SELECTARG1;
            }
            break;
            
    }
    return d3d_op;
}

//------------------------------------------------------------------------------
/**
    @brief Emit the render states describing the current pixel 
    operation in a multitexture pipeline.
*/
void 
nD3D8PixelShader::emit_blend_op(
    nD3D8Server *gs,
    int stage,
    nPSI& color_op,
    nPSI& alpha_op)
{
    IDirect3DDevice8 *dev = gs->d3d8Dev;
    D3DCAPS8 *desc = &(gs->devCaps);

    // translate the color and alpha op code
    D3DTEXTUREOP d3d_color_op;
    nPSI::nArg d3d_color_arg0;
    nPSI::nArg d3d_color_arg1;

    d3d_color_op = this->translate_pixel_op(color_op, desc);
    dev->SetTextureStageState(stage, D3DTSS_COLOROP, d3d_color_op);
    
    // translate the color op arguments
    d3d_color_arg0 = color_op.args[0];
    d3d_color_arg1 = color_op.args[1];
    dev->SetTextureStageState(stage, D3DTSS_COLORARG1, arg2d3d(d3d_color_arg0));
    dev->SetTextureStageState(stage, D3DTSS_COLORARG2, arg2d3d(d3d_color_arg1));

    // the alpha op may not be disabled, even if there is no
    // alpha operation defined for this stage!
    D3DTEXTUREOP d3d_alpha_op;
    nPSI::nArg d3d_alpha_arg0;
    nPSI::nArg d3d_alpha_arg1;

    d3d_alpha_op = this->translate_pixel_op(alpha_op,desc);
    if (d3d_alpha_op == D3DTOP_DISABLE) {
        d3d_alpha_op = d3d_color_op;
        d3d_alpha_arg0 = d3d_color_arg0;
        d3d_alpha_arg1 = d3d_color_arg1;
    } else {
        d3d_alpha_arg0 = alpha_op.args[0];
        d3d_alpha_arg1 = alpha_op.args[1];
    }
    dev->SetTextureStageState(stage,D3DTSS_ALPHAOP, d3d_alpha_op);

    // translate the alpha op arguments
    dev->SetTextureStageState(stage, D3DTSS_ALPHAARG1, arg2d3d(d3d_alpha_arg0));
    dev->SetTextureStageState(stage, D3DTSS_ALPHAARG2, arg2d3d(d3d_alpha_arg1));
}

//------------------------------------------------------------------------------
/**
    @brief Emit the render states for a single pass.
*/
void 
nD3D8PixelShader::emit_pass(nPixelShaderDesc *psd, nD3D8Server *gs, int pass)
{
    // set the rasterization render states for the 1st pass only
    if (0 == pass) this->emit_simple_states(gs, psd);

    // fill the texture stages for this pass...
    int j;
    for (j=0; j<this->pass_map_num[pass]; j++) {

        int cur_device_stage = j;
        int cur_psd_stage = this->pass_map_base[pass] + j;

        // emit the texture read operation
        this->emit_texture_states(gs, psd->tunit[cur_psd_stage], cur_psd_stage, cur_device_stage);

        if ((this->num_passes>1) && (0==j) && (pass>0)) {
            // if this is the first operation in a multipass
            // rendering process, a framebuffer blending
            // operation must be defined, unless we are
            // the very first pass which uses the polygon
            // surface color as 'prev'
            this->emit_fbuffer_blend_op(gs, cur_device_stage, psd->color_op[cur_psd_stage], psd->alpha_op[cur_psd_stage]);
        } else {
            // otherwise this is the 'normal' multitexture case
            this->emit_blend_op(gs, cur_device_stage, psd->color_op[cur_psd_stage],psd->alpha_op[cur_psd_stage]);
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Compile the pixel shader desc into one or multiple state blocks
    (one state block for each required rendering pass).
*/
bool 
nD3D8PixelShader::Compile(void)
{
    n_assert(this->ps_desc);
    n_assert(this->ps_desc->IsStateDirty());
    n_assert(this->num_passes > 0);

//    n_printf("nD3D8PixelShader::Compile()\n");

    nPixelShaderDesc *psd = this->ps_desc;

    // undirtify the pixel shader desc...
    psd->SetStateDirty(false);
    psd->SetStateChanged(true);

    nD3D8Server *gs = (nD3D8Server *) this->ref_gs.get();
    IDirect3DDevice8 *dev = gs->d3d8Dev;
    n_assert(dev);
    HRESULT hr;

    // generate a state block for each required pass...
    int i;
    for (i=0; i<this->num_passes; i++) {

        // delete existing state block
        if (this->state_block[i]) {
            hr = dev->DeleteStateBlock(this->state_block[i]);
            if (FAILED(hr)) {
                n_printf("nD3D8PixelShader: DeleteStateBlock() failed with '%s'\n",nd3d8_Error(hr));
            }
            this->state_block[i] = NULL;
        }

        // create a new state block
        hr = dev->BeginStateBlock();
        if (FAILED(hr)) {
            n_printf("nD3D8PixelShader::Compile(): BeginStateBlock() failed with '%s'\n",nd3d8_Error(hr));
            return false;
        }
        this->emit_pass(psd, gs, i);
        hr = dev->EndStateBlock(&(this->state_block[i]));
        if (FAILED(hr)) {
            n_printf("nD3D8PixelShader::Compile(): EndStateBlock() failed with '%s'\n",nd3d8_Error(hr));
            return false;
        }  
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Begin rendering the pixel shader.
    
    @return the number of rendering passes.
*/
int 
nD3D8PixelShader::BeginRender(nTextureArray *ta)
{
    n_assert(this->ps_desc);
    n_assert(!this->in_begin_render);
    this->in_begin_render = true;
       
    bool ps_state_dirty = this->ps_desc->IsStateDirty();

    // if not happened already, compute the number of passes required
    // to render the pixel shader
    if ((this->num_passes == 0) || ps_state_dirty) {

        nD3D8Server *gs = (nD3D8Server *) this->ref_gs.get();
        int num_tex_units = gs->devCaps.MaxSimultaneousTextures;

/*
        // FIXME: workaround for devices which export more then 2 
        // texture units, but fail to properly support the 3rd one
        // because it is specialized to some hardwired functionality
        // (most notably the Matrox cards)
        if (num_tex_units > 2) num_tex_units = 2;
*/

        // compute number of required passes
        if (0 == this->ps_desc->num_stages) {
            this->num_passes = 1;
        } else {
            this->num_passes = (this->ps_desc->num_stages+(num_tex_units-1)) / num_tex_units;
        }
        
        // fill the pass-mapper-tables
        int i;
        int base = 0;
        for (i=0; i<this->num_passes; i++) {
            this->pass_map_base[i] = base;
            if ((base+num_tex_units) <= this->ps_desc->num_stages) {
                this->pass_map_num[i] = num_tex_units;
            } else {
                this->pass_map_num[i] = this->ps_desc->num_stages - base;
            }
            base += num_tex_units;
        }
    }

    // set texture array pointer (may be NULL)
    this->tarray = ta;

    // if pixel shader state dirty, recompile it
    if (ps_state_dirty) this->Compile();

    return this->num_passes;
}

//------------------------------------------------------------------------------
/**
    @brief Render the given pixel shader pass.
*/
void 
nD3D8PixelShader::Render(int pass)
{
    n_assert(this->ps_desc);
    n_assert(this->in_begin_render);

    nD3D8Server *gs = (nD3D8Server *) this->ref_gs.get();
    nPixelShaderDesc *psd = this->ps_desc;
    n_assert(psd);

    // decide which state chunks need to be updated
    bool upd_state = true;
    bool upd_const = true;
    bool upd_transform = true;
    bool upd_lighting  = true;
    if ((gs->GetCurrentPixelShader() == this) && (this->num_passes == 1)) {
        
        // we can only skip render state changed if we are the current
        // pixel shader, and we only have 1 pass to render
        // now see which parts of the pixel shader descs have changed...
        upd_state     = psd->HasStateChanged();
        upd_const     = psd->HasConstChanged();
        upd_transform = psd->HasTransformChanged();
        upd_lighting  = psd->HasLightingChanged();
    }

    IDirect3DDevice8 *dev = gs->d3d8Dev;
    n_assert(dev);
    HRESULT hr;

    // apply the state block for this pass (if necessary)
    if (upd_state) {
        
// n_printf("upd_state\n");

        // set the states embedded in the state block        
        n_assert(this->state_block[pass] != 0);
        hr = dev->ApplyStateBlock(this->state_block[pass]);
        if (FAILED(hr)) {
            n_printf("nD3D8PixelShader: ApplyStateBlock() failed with '%s'\n",nd3d8_Error(hr));
            return;
        }

        // if this is the first pass, set fog state
        if (0 == pass) {
            DWORD r;
            if (psd->fog_enable && gs->globalFogEnable)
            {
                r=TRUE;
            }
            else
            {
                r=FALSE;
            }
            dev->SetRenderState(D3DRS_FOGENABLE,r);
        }

        psd->SetStateChanged(false);
    }

    // set the textures for this pass (must happen after applying state
    // blocks, because missing texture objects may turn off
    // texturing for a stage)
    this->set_texture_objects(gs,pass);

    // the lighting parameters and consts are only set in 
    // the first pass (if necessary at all)
    if (0 == pass) {
        if (upd_lighting) {

// n_printf("upd_lighting\n");
            
            D3DMATERIAL8 d3d8_mat;
            memcpy(&(d3d8_mat.Diffuse),&(this->ps_desc->diffuse),4*sizeof(float));
            memcpy(&(d3d8_mat.Ambient),&(this->ps_desc->ambient),4*sizeof(float));
            memset(&(d3d8_mat.Specular),0,4*sizeof(float));
            memcpy(&(d3d8_mat.Emissive),&(this->ps_desc->emissive),4*sizeof(float));
            d3d8_mat.Power = 0.0f;
            hr = dev->SetMaterial(&d3d8_mat);
            if (hr != D3D_OK) {
                n_printf("nD3D8PixelShader: SetMaterial() failed with '%s'\n",nd3d8_Error(hr));
                return;
            }
            psd->SetLightingChanged(false);
        }

        if (upd_const) {

// n_printf("upd_const\n");
            
            // set the right texture unit constants and matrices 
            // (argh, there's only one!)
            vector4& v = this->ps_desc->consts[0];
            DWORD const_color = D3DCOLOR_COLORVALUE(v.x,v.y,v.z,v.w);
            dev->SetRenderState(D3DRS_TEXTUREFACTOR,const_color);
            psd->SetConstChanged(false);
        }
    }

    // set the texture matrices (if necessary)
    if (upd_transform) {

// n_printf("upd_transform\n");

        int i;
        for (i=0; i<this->pass_map_num[pass]; i++) {
            int cur_tunit = this->pass_map_base[pass] + i;
            if (this->ps_desc->tunit[cur_tunit].enable_transform) {
                matrix44 mx(this->ps_desc->tunit[cur_tunit].GetMatrix());
                if (this->spheremap_hint[cur_tunit]) {
                    // if sphere mapping is enabled for this stage,
                    // then we need to shift the texture coordinates
                    // by 0.5 and scaled by 0.5 (since texture coordinates
                    // are generated in the range of -1.0 .. +1.0)
                    mx.scale(0.5f,0.5f,1.0f);
                    mx.translate(0.5f,0.5f,0.0f);
                } else if (this->objectspace_hint[cur_tunit]) {
                    // d3d doesn't support texture coordinate
                    // generation from object space. to achieve
                    // this, we simply multiply the current inverse
                    // modelview matrix into the texture matrix...
                    matrix44 m0;
                    gs->GetMatrix(N_MXM_MODELVIEW,m0);

                    // this matrix undoes the 'mirror along z'
                    // effect which we are using to make the
                    // D3D coordinate system handedness compatible
                    // with OpenGL's
                    static const matrix44 m1(1.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, 1.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f,-1.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 1.0f);
                    m0 *= m1;
                    m0.invert_simple();
                    // mx *= m0;
                    // possible fix...
                    mx = m0 * mx;

                } else {
                    // generate a 3x3 matrix from the 4x4 matrix, since d3d
                    // expects 2d texture coordinates
                    mx.M13 = 0.0f; mx.M14 = 0.0f; 
                    mx.M23 = 0.0f; mx.M24 = 0.0f;
                    mx.M31 = mx.M41; mx.M32 = mx.M42; mx.M33 = 1.0f; mx.M34 = 0.0f;
                    mx.M41 = 0.0f;   mx.M42 = 0.0f;   mx.M43 = 0.0f; mx.M44 = 1.0f;
                }
                hr = dev->SetTransform(D3DTRANSFORMSTATETYPE(D3DTS_TEXTURE0+i),(D3DMATRIX *)&mx);
                if (FAILED(hr)) {
                    n_printf("nD3D8PixelShader: SetTransform() failed with '%s'\n",nd3d8_Error(hr));
                }
            } else {
                // texture transform disabled, set identity matrix
                matrix44 ident;
                hr = dev->SetTransform(D3DTRANSFORMSTATETYPE(D3DTS_TEXTURE0+i),(D3DMATRIX *)&ident);
                if (FAILED(hr)) {
                    n_printf("nD3D8PixelShader: SetTransform(ident) failed with '%s'\n",nd3d8_Error(hr));
                }
            }
        }
        psd->SetTransformChanged(false);
    }

    // done.
}

//------------------------------------------------------------------------------
/**
    @brief Finish rendering the pixel shader (for cleanup etc...).
*/
void 
nD3D8PixelShader::EndRender(void)
{
    n_assert(this->in_begin_render);
    this->in_begin_render = false;

    nD3D8Server *gs = (nD3D8Server *) this->ref_gs.get();

    // if we are a multipass shader, we need to clean up
    // a few render states
    if (this->num_passes > 1) {
        IDirect3DDevice8 *dev = gs->d3d8Dev;
        n_assert(dev);
        dev->SetRenderState(D3DRS_ZBIAS, 0);
        dev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    }

    // we are the current pixel shader
    gs->SetCurrentPixelShader(this);
    this->tarray = NULL;
}

//-------------------------------------------------------------------
