#define N_IMPLEMENTS nSpriteRender
//-------------------------------------------------------------------
//  nsprender_main.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/ngfxserver.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelcontext.h"
#include "misc/nmathserver.h"
#include "particle/npemitter.h"
#include "particle/nspriterender.h"

//-------------------------------------------------------------------
//  ~nSpriteRender()
//  20-Mar-00   floh    created
//  18-Oct-00   floh    rewritten to nDynVertexBuffer
//-------------------------------------------------------------------
nSpriteRender::~nSpriteRender()
{
    if (this->ref_ibuf.isvalid()) 
    {
        this->ref_ibuf->Release();
        this->ref_ibuf.invalidate();
    }
}

//-------------------------------------------------------------------
//  init_buffers()
//  20-Mar-00   floh    created
//  18-Oct-00   floh    + rewritten to ndynvertexbuffer
//-------------------------------------------------------------------
void nSpriteRender::init_buffers(void)
{
    n_assert(!this->ref_ibuf.isvalid());

    this->dyn_vb.Initialize((N_VT_COORD|N_VT_RGBA|N_VT_UV0), 0);

    // get the number of quads which would fit into the 
    // vertex buffer
    this->num_quads = this->dyn_vb.GetNumVertices() / 4;

    nIndexBuffer *ibuf = this->ref_gs->FindIndexBuffer("nspriterender_ibuf");
    if (!ibuf) 
    {
        ibuf = this->ref_gs->NewIndexBuffer("nspriterender_ibuf");

        // get the number of required indices
        int num_indices = this->num_quads * 6;
        ibuf->Begin(N_IBTYPE_STATIC, N_PTYPE_TRIANGLE_LIST, num_indices);
        int indexIndex  = 0; 
        int vertexIndex = 0;
        for (; vertexIndex < (this->num_quads*4); vertexIndex += 4) 
        {
            ibuf->Index(indexIndex++, vertexIndex + 0);
            ibuf->Index(indexIndex++, vertexIndex + 1);
            ibuf->Index(indexIndex++, vertexIndex + 2);

            ibuf->Index(indexIndex++, vertexIndex + 3);
            ibuf->Index(indexIndex++, vertexIndex + 0);
            ibuf->Index(indexIndex++, vertexIndex + 2);
        }
        ibuf->End();
    }

    n_assert(ibuf);
    this->ref_ibuf = ibuf;
}

//-------------------------------------------------------------------
//  begin_render()
//  Initialize vertex pointers, stride and cur_quad.
//  18-Oct-00   floh    created
//-------------------------------------------------------------------
#ifdef __VC__
#pragma optimize("agt", on)
#endif

void nSpriteRender::begin_render(nPixelShader *ps, nTextureArray *ta)
{
    this->cur_vb    = this->dyn_vb.Begin(this->ref_ibuf.get(),ps,ta);
    this->coord     = this->cur_vb->coord_ptr;
    this->color     = this->cur_vb->color_ptr;
    this->uv        = this->cur_vb->uv_ptr[0];
    this->stride4   = this->cur_vb->stride4;
    this->cur_quad = 0;

    // initialize transformed reference quads
    matrix44 tform44;
    this->ref_gs->GetMatrix(N_MXM_VIEWER, tform44);
    matrix33 tform33(tform44.M11, tform44.M12, tform44.M13,
                     tform44.M21, tform44.M22, tform44.M23,
                     tform44.M31, tform44.M32, tform44.M33);
    int i;
    for (i = 0; i < (REFQUADS * 4); i++)
    {
        tform33.mult(this->refVertices[i], this->tformedVertices[i]);
    }

    // compute "lookup table spin" from original spin
    this->spinRef = (this->spin / (2.0f*N_PI)) * float(REFQUADS);
    this->spinAccelRef = (this->spin_accel / (2.0f*N_PI)) * float(REFQUADS);
}

//-------------------------------------------------------------------
//  swap_render()
//  20-Mar-00   floh    created
//  18-Oct-00   floh    + rewritten to ndynvertexbuffer
//-------------------------------------------------------------------
void nSpriteRender::swap_render(void)
{
    // render the current buffer, swap to other buffer
    int num_vertices = this->cur_quad * 4;
    int num_indices  = this->cur_quad * 6;
    this->cur_vb = this->dyn_vb.Swap(num_vertices,num_indices);

    this->coord   = this->cur_vb->coord_ptr;
    this->color   = this->cur_vb->color_ptr;
    this->uv      = this->cur_vb->uv_ptr[0];
    this->stride4 = this->cur_vb->stride4; 

    this->cur_quad = 0;
}

//-------------------------------------------------------------------
//  end_render()
//  18-Oct-00   floh    created
//-------------------------------------------------------------------
void nSpriteRender::end_render(void)
{
    int num_vertices = this->cur_quad * 4;
    int num_indices  = this->cur_quad * 6;
    this->dyn_vb.End(num_vertices,num_indices);
    this->cur_vb  = NULL;
    this->coord   = NULL;
    this->color   = NULL;
    this->stride4 = 0;
    this->cur_quad = 0;
}

//-------------------------------------------------------------------
//  Attach()
//  nSpriteRender acts as a complex node, so that the scene graph
//  will call us back to do actual rendering.
//  20-Mar-00   floh    created
//  18-Jun-01   floh    changed for new scenegraph stuff
//-------------------------------------------------------------------
bool 
nSpriteRender::Attach(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);
    
    if (nVisNode::Attach(sceneGraph)) 
    {
        sceneGraph->AttachVisualNode(this);
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
//  render_particles()
//  20-Mar-00   floh    created
//  15-Oct-00   floh    + stretch handling
//  18-Oct-00   floh    + rewritten to ndynvertexbuffer
//                      + removed visualization, to complex for
//                        inner loop, should be moved somewhere else...
//  08-Aug-01   floh    + aaargh, somehow the particle system 
//                        suffers from featuritis, makes the inner
//                        loop faaat
//  10-Oct-01   floh    + fixed mirror bug in texture coords
//-------------------------------------------------------------------
#ifdef __VC__
#pragma optimize("agt", on)
#endif

void nSpriteRender::render_particles(nSceneGraph2* sceneGraph, float tstamp)
{
    nPEmitter *e = this->ref_emitter.get();
    
    // which color format?
    nColorFormat cformat = this->dyn_vb.GetColorFormat();

    if (e->BeginPullParticles(sceneGraph->GetRenderContext(), tstamp)) 
    {
        // start rendering
        this->begin_render(sceneGraph->GetPixelShader(), sceneGraph->GetTextureArray());

        float lifetime = e->GetLifetime();
        n_assert(lifetime > 0.0f);
        float one_div_lifetime = 1.0f / lifetime;

        // these will become the 4 corner vertices...
        nGfxServer *gs  = this->ref_gs.get();
        nMathServer *ms = this->ref_math.get(); 

        // get matrix which transforms from world space to view space
        // and set that as the current modelview matrix (since our
        // particle render loop works in world space)
        matrix44 view;
        gs->GetMatrix(N_MXM_INVVIEWER, view);
        gs->PushMatrix(N_MXM_MODELVIEW);
        gs->SetMatrix(N_MXM_MODELVIEW, view);

        // get particle chunks
        nPCorn* particles;
        vector3 bounceVec;
        int numParticles = e->PullParticles(particles);
        if (numParticles > 0) do
        {
            int i;
            for (i = 0; i < numParticles; i++)
            {
                // get current particle
                nPCorn* curParticle = &(particles[i]);

                // only considers particles of correct age
                float age = tstamp - curParticle->tstamp;
                if (age < lifetime)
                {
                    nPAttrs pAttrs;

                    // get relative age of current particle
                    float relAge = age * one_div_lifetime;
    
                    // get interpolated particle attributes
                    this->getParticleAttrs(relAge, pAttrs);

                    // get world space particle position
                    float x = curParticle->p.x;
                    float y = curParticle->p.y;
                    float z = curParticle->p.z;

                    // add bounce?
                    float bounce = pAttrs.f[nPAttrs::N_BOUNCE];
                    if (bounce > 0.0f)
                    {
                        ms->PseudoRandVec3D(this->bounce_key++, bounceVec);
                        x += bounceVec.x * bounce;
                        y += bounceVec.y * bounce;
                        z += bounceVec.z * bounce;
                    }

                    // compute current spin
                    float curSpin = 0.0f;
                    if (this->spinRef != 0.0f) 
                    {
                        curSpin = this->spinRef * age;
                    } 
                    if (this->spinAccelRef != 0.0f) 
                    {
                        curSpin += this->spinAccelRef * age * age * 0.5f;
                    }

                    int index = (n_ftol(curSpin) & (REFQUADS-1)) << 2;
                    float size   = pAttrs.f[nPAttrs::N_SIZE];
                    ulong c = (N_COLOR_RGBA == cformat) ? pAttrs.rgba : pAttrs.bgra;

                    // generate topleft vertex
                    this->coord[0] = x + (size * this->tformedVertices[index + 0].x);
                    this->coord[1] = y + (size * this->tformedVertices[index + 0].y);
                    this->coord[2] = z + (size * this->tformedVertices[index + 0].z);
                    this->color[0] = c;
                    this->uv[0] = 0.0f;
                    this->uv[1] = 0.0f;
                    this->coord += this->stride4;
                    this->color += this->stride4;
                    this->uv    += this->stride4;

                    // generate topright vertex
                    this->coord[0] = x + (size * this->tformedVertices[index + 1].x);
                    this->coord[1] = y + (size * this->tformedVertices[index + 1].y);
                    this->coord[2] = z + (size * this->tformedVertices[index + 1].z);
                    this->color[0] = c;
                    this->uv[0] = 1.0f;
                    this->uv[1] = 0.0f;
                    this->color += this->stride4;
                    this->uv    += this->stride4;
                    this->coord += this->stride4;
                        
                    // generate bottom right vertex
                    this->coord[0] = x + (size * this->tformedVertices[index + 2].x);
                    this->coord[1] = y + (size * this->tformedVertices[index + 2].y);
                    this->coord[2] = z + (size * this->tformedVertices[index + 2].z);
                    this->color[0] = c;
                    this->uv[0] = 1.0f;
                    this->uv[1] = 1.0f;
                    this->coord += this->stride4;
                    this->color += this->stride4;
                    this->uv    += this->stride4;

                    // generate bottom left vertex
                    this->coord[0] = x + (size * this->tformedVertices[index + 3].x);
                    this->coord[1] = y + (size * this->tformedVertices[index + 3].y);
                    this->coord[2] = z + (size * this->tformedVertices[index + 3].z);
                    this->color[0] = c;
                    this->uv[0] = 0.0f;
                    this->uv[1] = 1.0f;
                    this->coord += this->stride4;
                    this->color += this->stride4;
                    this->uv    += this->stride4;

                    // swap vbuffers?
                    this->cur_quad++;
                    if (this->cur_quad >= this->num_quads) this->swap_render();
                }
            }
        } while ((numParticles=e->PullParticles(particles)));

        // flush current vbuffer
        this->end_render();
        e->EndPullParticles();
        gs->PopMatrix(N_MXM_MODELVIEW);
    }
}

//-------------------------------------------------------------------
//  Compute()
//  Pull particles from emitter and render them through the dynamic 
//  vertex buffer.
//  20-Mar-00   floh    created
//  18-Jun-01   floh    changed for new scene graph stuff
//-------------------------------------------------------------------
void 
nSpriteRender::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    nVisNode::Compute(sceneGraph); 

    // demand-create dynamic vertex buffer
    if (!this->dyn_vb.IsValid()) 
    {
        this->init_buffers();
    }

    // render particles
    if (this->ref_emitter.isvalid()) 
    {
        nChannelContext* chnContext = sceneGraph->GetChannelContext();
        n_assert(chnContext);
        float tstamp = chnContext->GetChannel1f(this->localChannelIndex);
        this->render_particles(sceneGraph, tstamp);
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------


