#define N_IMPLEMENTS nMeshEmitter
//-------------------------------------------------------------------
//  nmeshemitter_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelcontext.h"
#include "gfx/nvertexbuffer.h"
#include "misc/nmathserver.h"
#include "particle/nmeshemitter.h"

nNebulaScriptClass(nMeshEmitter, "npservemitter");

//-------------------------------------------------------------------
//  ~nMeshEmitter()
//  01-Nov-00   floh    created
//-------------------------------------------------------------------
nMeshEmitter::~nMeshEmitter()
{ }

//-------------------------------------------------------------------
//  Compute()
//  01-Nov-00   floh    created
//  08-Jun-01   floh    + oops, particle emission duration was
//                        messed up 
//  18-Jun-01   floh    + rewritten for new scene graph stuff
//-------------------------------------------------------------------
void 
nMeshEmitter::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    nVisNode::Compute(sceneGraph);

    // get current anim channel value
    nChannelContext* chnContext = sceneGraph->GetChannelContext();
    n_assert(chnContext);
    float tstamp = chnContext->GetChannel1f(this->localChannelIndex);

    // get pointer to particle system from particle server
    nPSystem *psys = this->GetPSystem(sceneGraph->GetRenderContext(), tstamp);

    // timestamp of next particle to be emitted
    float t = psys->GetLastEmitted() + this->delta_t;

    // only proceed if particles need to be generated...
    // if new particles need to be generated...
    if (this->ref_mesh.isvalid() && (t<tstamp)) 
    {
        // call Compute() on nMeshNode object to get vertex buffer object
        this->ref_mesh->Compute(sceneGraph);
        nVertexBuffer* vxbuf = sceneGraph->GetVertexBuffer();
        n_assert(vxbuf);
        sceneGraph->SetVertexBuffer(0);

        // if this is a non-continuous system, compute the
        // current start/stop times...
        float t_rep_start = 0.0f;
        float t_rep_stop  = tstamp;
        if (this->t_repeat > 0.0f) 
        {
            t_rep_start = this->t_start + (this->t_repeat * ((float)floor(tstamp/this->t_repeat)));
            t_rep_stop  = t_rep_start + (this->t_stop - this->t_start);
        }

        // get the pointer to the vertices of the vertex buffer
        vxbuf->LockVertices();
        float *coord_start = vxbuf->coord_ptr;
        int stride = vxbuf->stride4;
        int v_num  = vxbuf->num_v;

        // matrix to transform from modelview to world space
        matrix44 m = sceneGraph->GetTransform();
        matrix44 view;
        this->ref_gs->GetMatrix(N_MXM_VIEWER,view);
        m.mult_simple(view);

        // generate new particles...
        nMathServer *ms = this->ref_ms.get();
        vector3 pos;
        vector3 dir;
        vector3 vel;
        while (t < tstamp) 
        {
            // within valid emission timeframe?
            if ((t>t_rep_start) && (t<t_rep_stop)) 
            {
                // get a pseudo random starting direction
                ms->PseudoRandVec3D(this->rand_key++,dir);
    
                // get a new position from vertex buffer
                float r = (1.0f + ms->PseudoRand(this->vertex_key++)) * 0.5f;
                int vi = (int) (r * ((float)v_num)); 
                if (vi < 0) vi=0;
                else if (vi>=v_num) vi=v_num-1;
                float *coord = coord_start + vi * stride;
                pos.set(coord[0],coord[1],coord[2]);
                pos = m * pos;

                // compute start velocity vector
                vel = dir * this->speed;

                // add particle to system
                psys->AddParticle(t,pos,vel);
            }
            t += this->delta_t;
        }
        vxbuf->UnlockVertices();
    }
    // update the particle system with current acceleration
    psys->SetAccel(this->accel);

    // update the particle systems particles...
    psys->Update(tstamp);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
