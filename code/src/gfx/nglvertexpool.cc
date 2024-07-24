#define N_IMPLEMENTS nGlVertexPool
//-------------------------------------------------------------------
//  nglvertexpool.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nglvertexpool.h"
#include "gfx/nvertexbubble.h"
#include "gfx/nglpixelshader.h"
#include "gfx/nglserver.h"
#include "gfx/nindexbuffer.h"

#ifdef __WIN32__
#define glLockArraysEXT             gs->procLockArraysEXT
#define glUnlockArraysEXT           gs->procUnlockArraysEXT
#define glActiveTextureARB          gs->procActiveTextureARB
#define glClientActiveTextureARB    gs->procClientActiveTextureARB
#endif

//-------------------------------------------------------------------
/**
    08-Sep-00   floh    created
    20-Oct-00   floh    no support for NV_vertex_array_range under
                        Linux (FOR NOW)
*/
//-------------------------------------------------------------------
nGlVertexPool::~nGlVertexPool()
{
    if (this->base_ptr) n_free((void *)this->base_ptr);
}

//-------------------------------------------------------------------
/**
    Vertex component format and pointer alignment:
    
    COMP    SIZE
    - xyz     16 bytes (4 bytes waste)
    - norm    16 bytes (4 bytes waste)
    - color    8 bytes (4 bytes waste)
    - uv0      8 bytes (0 bytes waste)
    - uv1      8 bytes (0 bytes waste)
    - uv2      8 bytes (0 bytes waste)
    - uv3      8 bytes (0 bytes waste)
  
    08-Sep-00   floh    created
    21-Oct-00   floh    + may not contain (yet) weight and joint index
                          vertex components
*/
//-------------------------------------------------------------------
bool nGlVertexPool::Initialize(void)
{
    n_assert(this->vtype != 0);
    n_assert(this->vnum > 0);
    n_assert(NULL == this->base_ptr);
    n_assert(this->vbuf_type != N_VBTYPE_READONLY);

    // make sure there are no weights and joint indices in the
    // vertex type
    n_assert((this->vtype & N_VT_JW) == 0);

    // get the vertex size for the vertex buffer
    int v_size = 0;
    if (this->vtype & N_VT_COORD) v_size += 4*sizeof(float);
    if (this->vtype & N_VT_NORM)  v_size += 4*sizeof(float);
    if (this->vtype & N_VT_RGBA)  v_size += 2*sizeof(ulong);
    if (this->vtype & N_VT_UV0)   v_size += 2*sizeof(float);
    if (this->vtype & N_VT_UV1)   v_size += 2*sizeof(float);
    if (this->vtype & N_VT_UV2)   v_size += 2*sizeof(float);
    if (this->vtype & N_VT_UV3)   v_size += 2*sizeof(float);

    // allocate the vertex buffer
    int all_size = v_size * this->vnum;

    // allocate vertex buffer
    this->base_ptr = (float *) n_malloc(all_size);
 
    // fill the superclass members because we may not call nVertexPool::Initialize()
    this->stride4 = v_size>>2;

    // create the initial 'free' bubble which represents the
    // whole vertex pool
    nVertexBubble *vb = n_new nVertexBubble(this,0,this->vnum);
    this->free_bubbles.AddTail(vb);
    return true;
}

//-------------------------------------------------------------------
/**
    08-Sep-00   floh    created
*/
//-------------------------------------------------------------------
bool nGlVertexPool::LockBubble(nVertexBubble *vb)
{
    n_assert(vb);
    n_assert(vb->GetVertexPool() == this);
    n_assert(this->base_ptr);

    // get the base pointer to the vertex data
    float *bp = this->base_ptr + vb->GetStart()*this->stride4;
    
    // get the pointers to the vertex components
    if (this->vtype & N_VT_COORD) {
        this->coord_ptr = bp;
        bp += 4;
    } else this->coord_ptr = NULL;
    if (this->vtype & N_VT_NORM) {
        this->norm_ptr = bp;
        bp += 4;
    } else this->norm_ptr = NULL;
    if (this->vtype & N_VT_RGBA) {
        this->color_ptr = (ulong *) bp;
        bp += 2;
    } else this->color_ptr = NULL;

    if (this->vtype & N_VT_UV0) {
        this->uv_ptrs[0] = bp; 
        bp += 2;
    } else this->uv_ptrs[0] = NULL;
    if (this->vtype & N_VT_UV1) {
        this->uv_ptrs[1] = bp;
        bp += 2;
    } else this->uv_ptrs[1] = NULL;
    if (this->vtype & N_VT_UV2) {
        this->uv_ptrs[2] = bp;
        bp += 2;
    } else this->uv_ptrs[2] = NULL;
    if (this->vtype & N_VT_UV3) {
        this->uv_ptrs[3] = bp;
        bp += 2;
    } else this->uv_ptrs[3] = NULL;

    return true;
}

//-------------------------------------------------------------------
/**
    08-Sep-00   floh    created
*/
//-------------------------------------------------------------------
void nGlVertexPool::UnlockBubble(nVertexBubble *vb)
{
    n_assert(vb);
    n_assert(vb->GetVertexPool() == this);
    
    this->coord_ptr = NULL;
    this->norm_ptr  = NULL;
    this->color_ptr = NULL;
    memset(uv_ptrs,0,sizeof(uv_ptrs));
}

//-------------------------------------------------------------------
/**
    08-Sep-00   floh    created
    12-Oct-00   floh    + pixelshader no longer required
    16-Nov-00   floh    + nTextureArray argument
    26-Nov-00   floh    + added assertions to make sure that
                          number of vertices and indices are greater
                          0
*/
//-------------------------------------------------------------------
void nGlVertexPool::RenderBubble(nVertexBubble *vb, 
                                 nIndexBuffer *ib, 
                                 nPixelShader *ps,
                                 nTextureArray *ta)
{
    n_assert(vb);
    n_assert(ib);

    nGlServer *gs = (nGlServer *) this->ref_gs.get();

    int vb_start = vb->GetStart();
    int vb_size  = vb->GetTruncSize();
    int stride = this->stride4<<2;

    n_assert(vb_size > 0);

    // see how many pixel shader passes we require
    int num_passes = 1;
    if (ps) num_passes = ps->BeginRender(ta);

    // get vertex component pointers
    float *bp = this->base_ptr + vb_start*this->stride4;

    // define pointer to vertex array components
    if (this->vtype & N_VT_COORD) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3,GL_FLOAT,stride,bp);
        bp += 4;
    } else glDisableClientState(GL_VERTEX_ARRAY);

    if (this->vtype & N_VT_NORM) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT,stride,bp);
        bp += 4;
    } else glDisableClientState(GL_NORMAL_ARRAY);

    if (this->vtype & N_VT_RGBA) {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4,GL_UNSIGNED_BYTE,stride,bp);
        bp += 2;
    } else glDisableClientState(GL_COLOR_ARRAY);
        
    // compute pointers to texcoord array
    float *uvptr[N_MAXNUM_TEXCOORDS] = { NULL };
    if (this->vtype & N_VT_UV0) {
        uvptr[0] = bp;
        bp += 2;
    }
    if (this->vtype & N_VT_UV1) {
        uvptr[1] = bp;
        bp += 2;
    }
    if (this->vtype & N_VT_UV2) {
        uvptr[2] = bp;
        bp += 2;
    }
    if (this->vtype & N_VT_UV3) {
        uvptr[3] = bp;
        bp += 2;
    }

/*
// DEBUG: turn on wireframe mode
glColor3f(1.0f,1.0f,1.0f);
glPolygonMode(GL_FRONT,GL_LINE);
glPolygonMode(GL_BACK,GL_LINE);
*/
    // for each pixel shader pass...
    int num_triangles = 0;
    int current_pass;
    for (current_pass=0; current_pass<num_passes; current_pass++) {

        // render the current pixel shader pass...
        if (ps) {
            
            ps->Render(current_pass);

            // texture coordinate pointer mapping is evil, because
            // we may possibly be in a multipass situation, basically,
            // we have to look at the nGlPixelShader's pass mapping table...
            if (this->vtype & (N_VT_UV0|N_VT_UV1|N_VT_UV2|N_VT_UV3)) {
    
                int j;
                nGlPixelShader *glps = (nGlPixelShader *) ps;
                for (j=0; j<glps->pass_map_num[current_pass]; j++) {
                    #ifdef GL_ARB_multitexture
                    if (gs->arb_multitexture) {
                        glClientActiveTextureARB(GLenum(GL_TEXTURE0_ARB + j));
                    }
                    #else
                    #error "ARB_multitexture not supported by compile environment!"
                    #endif

                    float *cur_uv = NULL;
                    nRStateParam tex_coord_src;
                    tex_coord_src = glps->ps_desc->GetTexCoordSrc(glps->pass_map_base[current_pass]+j);
                    switch (tex_coord_src) {
                        case N_TCOORDSRC_UV0:
                            cur_uv = uvptr[0];
                            break;
                        case N_TCOORDSRC_UV1:
                            cur_uv = uvptr[1];
                            break;
                        case N_TCOORDSRC_UV2:
                            cur_uv = uvptr[2];
                            break;
                        case N_TCOORDSRC_UV3:
                            cur_uv = uvptr[3];
                            break;
                        default: break;
                    }
                    if (cur_uv) {
                        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                        glTexCoordPointer(2,GL_FLOAT,stride,cur_uv);
                    } else {
                        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                    }
                }

                #ifdef GL_ARB_multitexture
                if (gs->arb_multitexture) {
                    // disable leftover texture coord arrays
                    for (; j<gs->num_texture_units; j++) {
                        glClientActiveTextureARB(GLenum(GL_TEXTURE0_ARB + j));
                        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                    }
                }
                #else
                #error "ARB_multitexture not supported by compile environment!"
                #endif
            }
        } else {
            // no pixelshader...
            if (uvptr[0]) {
                #ifdef GL_ARB_multitexture
                if (gs->arb_multitexture) {
                    glClientActiveTextureARB(GL_TEXTURE0_ARB);
                }
                #else
                #error "ARB_multitexture not supported by compile environment!"
                #endif
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(2,GL_FLOAT,stride,uvptr[0]);
            } else {
                #ifdef GL_ARB_multitexture
                if (gs->arb_multitexture) {
                    glClientActiveTextureARB(GL_TEXTURE0_ARB);
                }
                #else
                #error "ARB_multitexture not supported by compile environment!"
                #endif
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        }

        // render the primitives
        #ifdef GL_EXT_compiled_vertex_array
        if (gs->ext_compiled_vertex_array) {
            glLockArraysEXT(0,vb_size);
        }
        #endif

        // render indexed primitives
        GLenum glpt;
        nPrimType pt = ib->GetPrimType();
        ushort *iptr = ib->GetIndexPointer();
        int inum  = ib->GetNumIndices();
        switch(pt) {
            case N_PTYPE_TRIANGLE_LIST:
                glpt = GL_TRIANGLES;
                num_triangles += inum/3;
                break;
            case N_PTYPE_TRIANGLE_FAN:
                glpt = GL_TRIANGLE_FAN;
                num_triangles += inum-2;  
                break;
            case N_PTYPE_TRIANGLE_STRIP:
                glpt = GL_TRIANGLE_STRIP;
                num_triangles += inum-2;
                break;
            case N_PTYPE_LINE_LIST:
                glpt = GL_LINES;
                break;
            case N_PTYPE_LINE_STRIP:
                glpt = GL_LINE_STRIP;
                break;
            case N_PTYPE_POINT_LIST:
            default:
                glpt = GL_POINTS;
                break;
        }
        n_assert(inum > 0);

/*
// DEBUG: make sure all indices are within the valid range
int i;
for (i=0; i<inum; i++) {
    n_assert(iptr[i]<vb_size);
}
*/
        glDrawElements(glpt,inum,GL_UNSIGNED_SHORT,iptr);

        #ifdef GL_EXT_compiled_vertex_array
        if (gs->ext_compiled_vertex_array) {
           glUnlockArraysEXT();
        }
        #endif
    }
    // tell pixel shader that we are finished
    if (ps) ps->EndRender();

    // communicate number of triangles to gfx server
    gs->SetStats(nGfxServer::N_GFXSTATS_TRIANGLES,num_triangles);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
