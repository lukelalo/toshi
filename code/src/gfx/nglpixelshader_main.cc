#define N_IMPLEMENTS nGlPixelShader
//-------------------------------------------------------------------
//  nglpixelshader_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nglpixelshader.h"
#include "gfx/ngltexture.h"
#include "gfx/nglserver.h"
#include "gfx/npixelshaderdesc.h"
#include "gfx/ntexturearray.h"

#ifdef __WIN32__
#define glActiveTextureARB gs->procActiveTextureARB
#define glBlendColorEXT    gs->procBlendColorEXT
#endif

// make sure that all extension are there in the build environment
#ifndef GL_EXT_blend_color
#error "EXT_blend_color not supported by compile environment"
#endif
#if !defined(GL_EXT_texture_env_combine) && !defined(GL_ARB_texture_env_combine)
#error "EXT_texture_env_combine not supported by compile environment!"
#endif
#if !defined(GL_EXT_texture_env_add) && !defined(GL_ARB_texture_env_add)
#error "EXT_texture_env_add not supported by compile environment"
#endif
#ifndef GL_ARB_multitexture
#error "ARB_multitexture not supported by compile environment!"
#endif
#if !defined(GL_EXT_texture_env_dot3) && !defined(GL_ARB_texture_env_dot3)
#error "EXT_texture_env_dot3 not supported by compile environment!"
#endif

nNebulaClass(nGlPixelShader, "npixelshader");

//-------------------------------------------------------------------
/**
    23-Aug-00   floh    created
*/
//-------------------------------------------------------------------
nGlPixelShader::nGlPixelShader()
{
    memset(this->dlist,0,sizeof(this->dlist));
    memset(this->pass_map_base,0,sizeof(pass_map_base));
    memset(this->pass_map_num,0,sizeof(pass_map_num));
}

//-------------------------------------------------------------------
/**
    23-Aug-00   floh    created
*/
//-------------------------------------------------------------------
nGlPixelShader::~nGlPixelShader()
{
    // free display list
    int i;
    for (i=0; i<nPixelShaderDesc::MAXSTAGES; i++) {
        if (this->dlist[i] != 0) glDeleteLists(this->dlist[i],1);
    }
}

//-------------------------------------------------------------------
/**
    @brief Translate nPSI-Arg to GL enum.

    24-Aug-00   floh    created
    22-Jul-01   floh    + oops, there was no support for nPSI::PRIMARY!?!?
*/
//-------------------------------------------------------------------
static inline GLenum arg2gl(nPSI::nArg arg)
{
    arg = (nPSI::nArg) (arg & nPSI::ARGMASK);
    switch (arg) {
        case nPSI::TEX:       return GL_TEXTURE; 
        case nPSI::PREV:      return (GLenum) GL_PREVIOUS_EXT;
        case nPSI::CONSTANT:  return (GLenum) GL_CONSTANT_EXT; 
        case nPSI::PRIMARY:   return (GLenum) GL_PRIMARY_COLOR_EXT;
        default:              return GL_TEXTURE;
    }
    return GL_TEXTURE;
}

//-------------------------------------------------------------------
/**
    @brief Translate nPSI-Opcode to GL enum.

    24-Aug-00   floh    created
*/
//-------------------------------------------------------------------
static inline GLenum op2gl(nGlServer *gs, nPSI::nOp op) 
{
    if (gs->ext_texture_env_combine) {
        switch (op) {
            case nPSI::REPLACE:     return GL_REPLACE;
            case nPSI::MUL:         return GL_MODULATE; 
            case nPSI::ADD:         return GL_ADD; 
            case nPSI::ADDS:        return (GLenum) GL_ADD_SIGNED_EXT;
            case nPSI::IPOL:        return (GLenum) GL_INTERPOLATE_EXT;
            case nPSI::DOT:
                if (gs->ext_texture_env_dot3) return (GLenum) GL_DOT3_RGBA_EXT;
                else                          return (GLenum) GL_MODULATE;
            default:                return GL_MODULATE;
        }
    } else {
        switch (op) {
            case nPSI::REPLACE:     return GL_REPLACE;
            case nPSI::MUL:         return GL_MODULATE; 
            case nPSI::ADD:         return GL_ADD; 
            case nPSI::ADDS:        return GL_ADD;      // req. texture_env_combine!
            case nPSI::IPOL:        return GL_MODULATE; // req. texture_env_combine!
            case nPSI::DOT:         return GL_MODULATE; // req. texture_env_combine!
            default:                return GL_MODULATE;
        }
    }
    return GL_MODULATE;
}

//-------------------------------------------------------------------
/**
    @brief Translate nPSI-Scale to GL enum.

    24-Aug-00   floh    created
*/
//-------------------------------------------------------------------
static inline GLfloat scale2gl(nPSI::nScale scale)
{
    switch (scale) {
        case nPSI::ONE:     return 1.0f;
        case nPSI::TWO:     return 2.0f;
        case nPSI::FOUR:    return 4.0f;
    }
    return 1.0f;
}

//-------------------------------------------------------------------
/**
    @brief Write the 'simple' attributes which are not related to texture
    stages.

    23-Aug-00   floh    created
    09-Jan-00   floh    moved fog state switch from here to 
                        Render(), the fog state may not be compiled
                        into the display list, because it also
                        depends on the global_fog_enabled flag, which
                        may change independently from embedded
                        object-local fog enabled state
    12-Jul-01   floh    alpha test stuff
*/
//-------------------------------------------------------------------
void nGlPixelShader::emit_simple_states(nPixelShaderDesc& psd)
{
    // boolean stuff...
    if (psd.light_enable)       glEnable(GL_LIGHTING);
    else                        glDisable(GL_LIGHTING);
    if (psd.alpha_enable)       glEnable(GL_BLEND);
    else                        glDisable(GL_BLEND);
    if (psd.zwrite_enable)      glDepthMask(1);
    else                        glDepthMask(0);

	if (psd.wireframe_enable) {
		glPolygonMode(GL_FRONT,GL_LINE);
		glPolygonMode(GL_BACK,GL_LINE);
	} else {
		glPolygonMode(GL_FRONT,GL_FILL);
	    glPolygonMode(GL_BACK,GL_FILL);
	}

	if (psd.normalizenormals_enable) {
		glEnable(GL_NORMALIZE);
	} else {
		glDisable(GL_NORMALIZE);
	}

    // alpha blending factors
    GLenum src,dst;
    switch (psd.alpha_src_blend) {
        case N_ABLEND_ZERO:         src = GL_ZERO; break;
        case N_ABLEND_ONE:          src = GL_ONE;  break;
        case N_ABLEND_SRCCOLOR:     src = GL_SRC_COLOR; break;
        case N_ABLEND_INVSRCCOLOR:  src = GL_ONE_MINUS_SRC_COLOR; break;
        case N_ABLEND_SRCALPHA:     src = GL_SRC_ALPHA; break;
        case N_ABLEND_INVSRCALPHA:  src = GL_ONE_MINUS_SRC_ALPHA; break;
        case N_ABLEND_DESTALPHA:    src = GL_DST_ALPHA; break;
        case N_ABLEND_INVDESTALPHA: src = GL_ONE_MINUS_DST_ALPHA; break;
        case N_ABLEND_DESTCOLOR:    src = GL_DST_COLOR; break;
        case N_ABLEND_INVDESTCOLOR: src = GL_ONE_MINUS_DST_COLOR; break;
        default:                    src = GL_ZERO; break;
    };
    switch (psd.alpha_dest_blend) {
        case N_ABLEND_ZERO:         dst = GL_ZERO; break;
        case N_ABLEND_ONE:          dst = GL_ONE;  break;
        case N_ABLEND_SRCCOLOR:     dst = GL_SRC_COLOR; break;
        case N_ABLEND_INVSRCCOLOR:  dst = GL_ONE_MINUS_SRC_COLOR; break;
        case N_ABLEND_SRCALPHA:     dst = GL_SRC_ALPHA; break;
        case N_ABLEND_INVSRCALPHA:  dst = GL_ONE_MINUS_SRC_ALPHA; break;
        case N_ABLEND_DESTALPHA:    dst = GL_DST_ALPHA; break;
        case N_ABLEND_INVDESTALPHA: dst = GL_ONE_MINUS_DST_ALPHA; break;
        case N_ABLEND_DESTCOLOR:    dst = GL_DST_COLOR; break;
        case N_ABLEND_INVDESTCOLOR: dst = GL_ONE_MINUS_DST_COLOR; break;
        default:                    dst = GL_ZERO; break;
    };
    glBlendFunc(src,dst);

    // the z test function
    GLenum zf;
    switch (psd.zfunc) {
        case N_CMP_NEVER:           zf=GL_NEVER; break;
        case N_CMP_LESS:            zf=GL_LESS; break;
        case N_CMP_EQUAL:           zf=GL_EQUAL; break;
        case N_CMP_LESSEQUAL:       zf=GL_LEQUAL; break;
        case N_CMP_GREATER:         zf=GL_GREATER; break;
        case N_CMP_NOTEQUAL:        zf=GL_NOTEQUAL; break;
        case N_CMP_GREATEREQUAL:    zf=GL_GEQUAL; break;
        case N_CMP_ALWAYS:          zf=GL_ALWAYS; break;
        default:                    zf=GL_LESS; break;
    }
    glDepthFunc(zf);
 
    // the cull mode
    switch (psd.cull_mode) {
        case N_CULL_NONE:
            glDisable(GL_CULL_FACE);
            break;
        case N_CULL_CW:
            glFrontFace(GL_CW);
            glEnable(GL_CULL_FACE);
            break;
        case N_CULL_CCW:
            glFrontFace(GL_CCW);
            glEnable(GL_CULL_FACE);
            break;
        default: break;
    }

    // color material source
    switch (psd.color_material) {
        case N_CMAT_MATERIAL:
            glDisable(GL_COLOR_MATERIAL);
            break;
        case N_CMAT_DIFFUSE:
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
            break;
        case N_CMAT_EMISSIVE:
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT_AND_BACK,GL_EMISSION);
            break;
        case N_CMAT_AMBIENT:
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT);
            break;
        default: break;
    }

    // alpha test
    if (psd.alphatest_enable)
    {
        glEnable(GL_ALPHA_TEST);
        GLenum af;
        switch (psd.alphafunc)
        {
            case N_CMP_NEVER:           af=GL_NEVER; break;
            case N_CMP_LESS:            af=GL_LESS; break;
            case N_CMP_EQUAL:           af=GL_EQUAL; break;
            case N_CMP_LESSEQUAL:       af=GL_LEQUAL; break;
            case N_CMP_GREATER:         af=GL_GREATER; break;
            case N_CMP_NOTEQUAL:        af=GL_NOTEQUAL; break;
            case N_CMP_GREATEREQUAL:    af=GL_GEQUAL; break;
            case N_CMP_ALWAYS:          af=GL_ALWAYS; break;
            default:                    af=GL_LESS; break;
        }
        glAlphaFunc(af, psd.alpharef); 
    }
    else
    {
        glDisable(GL_ALPHA_TEST);
    }
}

//-------------------------------------------------------------------
/**
    @brief Emit states for a texture unit (without texture matrix because
    that's dynamic).

    23-Aug-00   floh    created
    29-Aug-00   floh    + changes for new tex coord src
    16-Nov-00   floh    + texture object no longer part of 
                          compiled render state
*/
//-------------------------------------------------------------------
void nGlPixelShader::emit_texture_states(nGlServer *gs, nTextureUnit& tu)
{
    n_assert(gs);    

    GLenum r;

    // texture address
    switch (tu.addr_u) {
        case N_TADDR_WRAP:  r=GL_REPEAT;  break;
        case N_TADDR_CLAMP: r=GL_CLAMP; break;
        default:            r=GL_REPEAT; break;
    }
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,r);
    
    switch (tu.addr_v) {
        case N_TADDR_WRAP:  r=GL_REPEAT;  break;
        case N_TADDR_CLAMP: r=GL_CLAMP; break;
        default:            r=GL_REPEAT; break;
    }
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,r);

    // min filter
    switch (tu.filter_min) {
        case N_TFILTER_NEAREST:                 r=GL_NEAREST; break;
        case N_TFILTER_LINEAR:                  r=GL_LINEAR; break;
        case N_TFILTER_NEAREST_MIPMAP_NEAREST:  r=GL_NEAREST_MIPMAP_NEAREST; break;
        case N_TFILTER_LINEAR_MIPMAP_NEAREST:   r=GL_LINEAR_MIPMAP_NEAREST; break;
        case N_TFILTER_NEAREST_MIPMAP_LINEAR:   r=GL_NEAREST_MIPMAP_LINEAR; break;
        case N_TFILTER_LINEAR_MIPMAP_LINEAR:    r=GL_LINEAR_MIPMAP_LINEAR; break;
        default:                                r=GL_LINEAR; break;
    }
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,r);

    // mag filter
    switch (tu.filter_mag) {
        case N_TFILTER_NEAREST: r=GL_NEAREST; break;
        case N_TFILTER_LINEAR:  r=GL_LINEAR; break;
        default:                r=GL_LINEAR; break;
    }
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,r);

    // tex gen mode
    switch(tu.coord_src) {
        case N_TCOORDSRC_UV0:
        case N_TCOORDSRC_UV1:
        case N_TCOORDSRC_UV2:
        case N_TCOORDSRC_UV3:
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            break;
        case N_TCOORDSRC_OBJECTSPACE:
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
            glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
            break;
        case N_TCOORDSRC_EYESPACE:
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
            glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
            break;
        case N_TCOORDSRC_SPHEREMAP:
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
            glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
            break;
        default: break;
    }
}

//-------------------------------------------------------------------
/**
    @brief Emit the render states describing the current pixel
    operation where the 'previous' stage is the frame
    buffer.
    
    This is necessary if multipass rendering must be used.

    28-Aug-00   floh    created
*/
//-------------------------------------------------------------------
void nGlPixelShader::emit_fbuffer_blend_op(nPSI& color_op, nPSI& /*alpha_op*/, nGlServer *gs)
{
    n_assert(gs);
    
    // feed the texture color directly
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // enable alpha blending, and define the blending operation
    // which simulates our pixel op
    glEnable(GL_BLEND);
    
    // we ignore the alpha op, also most accelerators don't have
    // destination alpha, which is a problem if the operation 
    // wants to use prev.a
    GLenum src,dst;
    switch (color_op.op) {
        case nPSI::ADD:
        case nPSI::ADDS:
            src = GL_ONE;
            dst = GL_ONE;
            break;
        case nPSI::IPOL:
            if (arg2gl(color_op.args[2]) == GL_CONSTANT_EXT) {
                if (gs->ext_blend_color) {
                    src = GL_CONSTANT_ALPHA_EXT;
                    dst = GL_ONE_MINUS_CONSTANT_ALPHA_EXT;
                } else {
                    src = GL_SRC_ALPHA;
                    dst = GL_ONE_MINUS_SRC_ALPHA;
                }
            } else {
                src = GL_SRC_ALPHA;
                dst = GL_ONE_MINUS_SRC_ALPHA;
            }
            break;
        case nPSI::MUL:
        default:
            src = GL_ZERO;
            dst = GL_SRC_COLOR;
            break;
    }
    glBlendFunc(src,dst);

    // avoid zbuffer fighting and turn off some unnecessary stuff...
    glPolygonOffset(-1.0f,-1.0f);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glDepthMask(0);
    glDisable(GL_FOG);
}

//-------------------------------------------------------------------
/**
    @brief Emit the render states describing the current pixel 
    operation in a multitexture pipeline with or without
    EXT_texture_env_combine.

    24-Aug-00   floh    created
*/
//-------------------------------------------------------------------
void nGlPixelShader::emit_blend_op(nPSI& color_op, nPSI& alpha_op, nGlServer *gs)
{
    n_assert(gs);

    if (gs->ext_texture_env_combine) {

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

        // define the color operation, if there is one defined
        if (color_op.op != nPSI::NOP) {

            // translate the color opcode
            glTexEnvi(GL_TEXTURE_ENV, (GLenum)GL_COMBINE_RGB_EXT, op2gl(gs,color_op.op));

            // argument modifier handling
            nPSI::nArg arg0 = color_op.args[0];
            nPSI::nArg arg1 = color_op.args[1];
            nPSI::nArg arg2 = color_op.args[2];
            
            GLenum r;
            if (arg0 & nPSI::ONE_MINUS) {
                if (arg0 & nPSI::ALPHA) r=GL_ONE_MINUS_SRC_ALPHA;
                else                    r=GL_ONE_MINUS_SRC_COLOR;
            } else {
                if (arg0 & nPSI::ALPHA) r=GL_SRC_ALPHA;
                else                    r=GL_SRC_COLOR;
            }
            glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_OPERAND0_RGB_EXT, r);

            if (arg1 & nPSI::ONE_MINUS) {
                if (arg1 & nPSI::ALPHA) r=GL_ONE_MINUS_SRC_ALPHA;
                else                    r=GL_ONE_MINUS_SRC_COLOR;
            } else {
                if (arg1 & nPSI::ALPHA) r=GL_SRC_ALPHA;
                else                    r=GL_SRC_COLOR;
            }
            glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_OPERAND1_RGB_EXT, r);
            glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_OPERAND2_RGB_EXT, GL_SRC_ALPHA);

            // translate the color op scale
            glTexEnvf(GL_TEXTURE_ENV, (GLenum) GL_RGB_SCALE_EXT, scale2gl(color_op.scale));

            // translate color op args
            if (color_op.args[0] != nPSI::NOARG) {
                glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_SOURCE0_RGB_EXT, arg2gl(arg0));
            }
            if (color_op.args[1] != nPSI::NOARG) {
                glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_SOURCE1_RGB_EXT, arg2gl(arg1));
            }
            if (color_op.args[2] != nPSI::NOARG) {
                glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_SOURCE2_RGB_EXT, arg2gl(arg2));
            }
        }

        // translate the alpha opcode
        if (alpha_op.op != nPSI::NOP) {
            
            // translate the alpha op code
            glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_COMBINE_ALPHA_EXT, op2gl(gs,alpha_op.op));

            // argument modifier handling
            nPSI::nArg arg0 = alpha_op.args[0];
            nPSI::nArg arg1 = alpha_op.args[1];
            nPSI::nArg arg2 = alpha_op.args[2];
            GLenum r;
            if (arg0 & nPSI::ONE_MINUS) r=GL_ONE_MINUS_SRC_ALPHA;
            else                        r=GL_SRC_ALPHA;
            glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_OPERAND0_ALPHA_EXT, r);

            if (arg1 & nPSI::ONE_MINUS) r=GL_ONE_MINUS_SRC_ALPHA;
            else                        r=GL_SRC_ALPHA;
            glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_OPERAND1_ALPHA_EXT, r);
            glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_OPERAND2_ALPHA_EXT, GL_SRC_ALPHA);

            // translate the alpha op scale
            glTexEnvf(GL_TEXTURE_ENV, GL_ALPHA_SCALE, scale2gl(alpha_op.scale));

            // translate alpha op args
            if (alpha_op.args[0] != nPSI::NOARG) {
                glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_SOURCE0_ALPHA_EXT, arg2gl(arg0));
            }
            if (alpha_op.args[1] != nPSI::NOARG) {
                glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_SOURCE1_ALPHA_EXT, arg2gl(arg1));
            }
            if (alpha_op.args[2] != nPSI::NOARG) {
                glTexEnvi(GL_TEXTURE_ENV, (GLenum) GL_SOURCE2_ALPHA_EXT, arg2gl(arg2));
            }
        }
    } else {
        // no runtime support for texture env combine
        // this limits the pixel shader drastically, but we
        // try to do our best...
        GLenum r;
        switch (color_op.op) {
            case nPSI::REPLACE:
                r = GL_REPLACE;
                break;
            case nPSI::MUL:
                r = GL_MODULATE;
                break;
            case nPSI::ADD:
            case nPSI::ADDS:
                if (gs->ext_texture_env_add) r = GL_ADD;
                else                         r = GL_MODULATE;
                break;

            case nPSI::IPOL:
                if (arg2gl(color_op.args[2]) == (GLenum) GL_CONSTANT_EXT) {
                    r = GL_BLEND;
                } else {
                    r = GL_DECAL;
                }
                break;
            default:
                r = GL_REPLACE;
                break;
        }
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, r);
    }
}

//-------------------------------------------------------------------
/**
    FIXME: is the fog handling right? if global fog state changes,
    the pixel shader remains clean, so the display list would
    not be recompiled!
  
    23-Aug-00   floh    created
*/
//-------------------------------------------------------------------
void nGlPixelShader::emit_pass_multitexture(nPixelShaderDesc& psd, nGlServer *gs, int pass)
{
    // set the 'simple' render states in the first pass only
    if (0 == pass) this->emit_simple_states(psd);

    // fill the texture stages for this pass
    int j;
    for (j=0; j<this->pass_map_num[pass]; j++) {

        glActiveTextureARB(GLenum(GL_TEXTURE0_ARB + j));
        
        int cur_stage = this->pass_map_base[pass] + j;

        // emit the texture read operation
        this->emit_texture_states(gs, psd.tunit[cur_stage]);

        if ((this->num_passes>1) && (0==j) && (pass>0)) {
            // if this is the first operation in a multipass
            // rendering process, a framebuffer blending
            // operation must be defined, unless we are
            // the very first pass which uses the polygon
            // surface color as 'prev'
            this->emit_fbuffer_blend_op(psd.color_op[cur_stage], psd.alpha_op[cur_stage], gs);
        } else {
            // otherwise this is the 'normal' multitexture case
            this->emit_blend_op(psd.color_op[cur_stage],psd.alpha_op[cur_stage],gs);
        }
    }
}


//-------------------------------------------------------------------
/**
    23-Aug-00   floh    created
*/
//-------------------------------------------------------------------
void nGlPixelShader::emit_pass_no_multitexture(nPixelShaderDesc& psd, nGlServer *gs, int pass)
{
    // set the 'simple' render states in the first list only
    if (0==pass) this->emit_simple_states(psd);

    // emit the texture read operation
    this->emit_texture_states(gs, psd.tunit[pass]);

    // emit the current render pass
    if (0==pass) {
        this->emit_blend_op(psd.color_op[pass],psd.alpha_op[pass],gs);
    } else {
        this->emit_fbuffer_blend_op(psd.color_op[pass], psd.alpha_op[pass], gs);
    }
}

//-------------------------------------------------------------------
/**
    04-Oct-00   floh    created
    16-Nov-00   floh    bind texture objects
    09-Jan-01   floh    moved fog state switch from inside Compile()
                        to here
*/
//-------------------------------------------------------------------
void nGlPixelShader::emit_pass(int pass)
{
    n_assert(this->ps_desc);
    nGlServer *gs = (nGlServer *) this->ref_gs.get();
    n_assert(this->dlist[pass] != 0);
    glCallList(this->dlist[pass]);

    // in the first pass, emit the fog state
    if (0 == pass) {
        if (this->ps_desc->fog_enable && gs->global_fog_enable) glEnable(GL_FOG);
        else                                                    glDisable(GL_FOG);
    }
}

//-------------------------------------------------------------------
/**
    @brief Set the right texture objects for this pass.

    16-Nov-00   floh    created
*/
//-------------------------------------------------------------------
void nGlPixelShader::set_texture_objects(int pass, nGlServer *gs)
{
    // bind texture objects
    int i=0;
    if (this->tarray) {
        for (; i<this->pass_map_num[pass]; i++) {
            int stage = this->pass_map_base[pass]+i;
            nGlTexture *gltex = (nGlTexture *) this->tarray->GetTexture(stage);
            if (gs->GetCurrentTexture(i) != gltex) {

//                n_printf("upd_texture (stage %d)\n", i);
                
                if (gs->arb_multitexture) glActiveTextureARB(GLenum(GL_TEXTURE0_ARB + i));
                if (gltex) {
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, gltex->tex_id);
                    gs->SetCurrentTexture(i,(nTexture *)gltex);
                } else {
                    glDisable(GL_TEXTURE_2D);
                    gs->SetCurrentTexture(i,NULL);
                }
            }
        }
    }
    // disable texturing for all "leftover" texture units
    for (; i<gs->num_texture_units; i++) {
        if (gs->arb_multitexture) glActiveTextureARB(GLenum(GL_TEXTURE0_ARB + i));
        glDisable(GL_TEXTURE_2D);
        if (i<N_MAXNUM_TEXSTAGES)
            gs->SetCurrentTexture(i,NULL);
    }
}

//-------------------------------------------------------------------
/**
    @brief Compile the static state of the externel pixel shader desc into
    a display list.

    23-Aug-00   floh    created
*/
//-------------------------------------------------------------------
bool nGlPixelShader::Compile(void)
{
    n_assert(this->ps_desc);
    n_assert(this->ps_desc->IsStateDirty());
    n_assert(this->num_passes > 0);

//    n_printf("nGlPixelShader::Compile()\n");

    nPixelShaderDesc *psd = this->ps_desc;

    // undirtify the pixel shader desc...
    psd->SetStateDirty(false);
    psd->SetStateChanged(true);

    nGlServer *gs = (nGlServer *) this->ref_gs.get();

    int i;
    if (gs->arb_multitexture) {
        // for each required rendering pass, a display list is generated
        for (i=0; i<this->num_passes; i++) {
            n_assert(0 != this->dlist[i]);
            glNewList(this->dlist[i],GL_COMPILE);
            this->emit_pass_multitexture(*psd, gs, i);
            glEndList();
        }
    } else {
        for (i=0; i<this->num_passes; i++) {
            n_assert(0 != this->dlist[i]);
            glNewList(this->dlist[i],GL_COMPILE);
            this->emit_pass_no_multitexture(*psd, gs, i);
            glEndList();
        }
    }
    return true;
}

//-------------------------------------------------------------------
/**
    @brief  Begin a (possibly multipass) render operation
    
    @return the number of passes required to render the pixel shader.

    28-Aug-00   floh    created
    02-Oct-00   floh    + non-textured pixel shaders special cased
    04-Oct-00   floh    + display list workaround for Matrox and
                          3Dfx ICDs
    16-Nov-00   floh    + nTextureArray parameter (may be NULL)
*/
//-------------------------------------------------------------------
int nGlPixelShader::BeginRender(nTextureArray *ta)
{
    n_assert(!this->in_begin_render);
    this->in_begin_render = true;
    
    bool ps_state_dirty = this->ps_desc->IsStateDirty();
    nGlServer *gs = (nGlServer *) this->ref_gs.get();

    // store the texture array object
    this->tarray = ta;

    // if not happened already, compute the number of passes required
    // to render the pixel shader
    if ((this->num_passes == 0) || ps_state_dirty) {

        // compute number of required passes
        if (0 == this->ps_desc->num_stages) {
            this->num_passes = 1;
        } else {
            this->num_passes = (this->ps_desc->num_stages+(gs->num_texture_units-1)) / gs->num_texture_units;
        }
        
        // fill the pass-mapper-tables
        int i;
        int base = 0;
        for (i=0; i<this->num_passes; i++) {
            this->pass_map_base[i] = base;
            if ((base+gs->num_texture_units) <= this->ps_desc->num_stages) {
                this->pass_map_num[i] = gs->num_texture_units;
            } else {
                this->pass_map_num[i] = this->ps_desc->num_stages - base;
            }
            base += gs->num_texture_units;
        }

        // allocate new display lists on demand
        for (i=0; i<this->num_passes; i++) {
            if (this->dlist[i]==0) {
                this->dlist[i] = glGenLists(1);
            }
        }
    }

    // if pixel shader desc dirty, recompile it
    if (ps_state_dirty) this->Compile();

    return this->num_passes;
}

//-------------------------------------------------------------------
/**
    @brief Render the pixel shader display list, and the "other"
    render states.

    23-Aug-00   floh    created
    28-Aug-00   floh    new arg 'pass' for multipass rendering
    14-Sep-00   floh    more efficient handling of redundant render
                        state changes
    04-Oct-00   floh    + display list workaround for Matrox and
                          3Dfx ICDs
*/
//-------------------------------------------------------------------
void nGlPixelShader::Render(int pass)
{
    n_assert(this->in_begin_render);
    n_assert(this->ps_desc);
    nGlServer *gs = (nGlServer *) this->ref_gs.get();
    nPixelShaderDesc *psd = this->ps_desc;

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

    // set textures
    this->set_texture_objects(pass,gs);

    // render compiled display list
    if (upd_state) {

//        n_printf("upd_state\n");
        
        this->emit_pass(pass);
        psd->SetStateChanged(false);
    }        

    // the lighting parameters are only set in the first pass
    if ((0 == pass) && (upd_lighting)) {

//        n_printf("upd_lighting\n");
       
        // the lighting equation surface attributes
        glMaterialfv(GL_FRONT,GL_DIFFUSE, (const float *) &(this->ps_desc->diffuse));
        glMaterialfv(GL_FRONT,GL_EMISSION, (const float *) &(this->ps_desc->emissive));
        glMaterialfv(GL_FRONT,GL_AMBIENT, (const float *) &(this->ps_desc->ambient));
        psd->SetLightingChanged(false);
    }

    // set constants (the texture unit 0 constant is distributed to all texture
    // units, this is because d3d only has one texture constant for all stages,
    // and we need to emulate the same behaviour under OpenGL)
    int i;
    if (upd_const) {

//        n_printf("upd_const\n");
        
        for (i=0; i<this->pass_map_num[pass]; i++) {
            if (gs->arb_multitexture) glActiveTextureARB(GLenum(GL_TEXTURE0_ARB + i));
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, (GLfloat *)&(this->ps_desc->consts[0]));
        }
        if (gs->ext_blend_color) {
            vector4& bc = this->ps_desc->consts[0];
            glBlendColorEXT(bc.x, bc.y, bc.z, bc.w);
        }
        psd->SetConstChanged(false);
    }

    if (upd_transform) {

//        n_printf("upd_transform\n");
        
        // set the right texture unit constants and matrices
        for (i=0; i<this->pass_map_num[pass]; i++) {
            if (gs->arb_multitexture) glActiveTextureARB(GLenum(GL_TEXTURE0_ARB + i));
            int cur_stage = this->pass_map_base[pass]+i;
            if (this->ps_desc->tunit[cur_stage].enable_transform) {
                const matrix44& mx = this->ps_desc->tunit[cur_stage].GetMatrix();
                const GLfloat *glm = &(mx.M11);
                glMatrixMode(GL_TEXTURE);
                glLoadMatrixf(glm);
            } else {
                glMatrixMode(GL_TEXTURE);
                glLoadIdentity();
            }
        }
        psd->SetTransformChanged(false);
    }
        
    // done.
}

//-------------------------------------------------------------------
/**
    @brief  Finish rendering the pixel shader.

    28-Aug-00   floh    created
    16-Nov-00   floh    TextureArray pointer invalidated
*/
//-------------------------------------------------------------------
void nGlPixelShader::EndRender(void)
{
    n_assert(this->in_begin_render);
    this->in_begin_render = false;

    nGlServer *gs = (nGlServer *) this->ref_gs.get();

    if (this->num_passes > 1) {
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDepthMask(1);
    } 

    // FIXME: make texture unit 0 the active one (in case there are matnodes around)
    if (gs->arb_multitexture) glActiveTextureARB(GL_TEXTURE0_ARB);

    // we are the current pixel shader!
    gs->SetCurrentPixelShader(this);
    this->tarray = NULL;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
