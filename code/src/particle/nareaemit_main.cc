//------------------------------------------------------------------------------
/* Copyright (c) 2003 Leaf Garland.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#define N_IMPLEMENTS nAreaEmitter
//-------------------------------------------------------------------
//  npointemit_main.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/ngfxserver.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelcontext.h"
#include "misc/nmathserver.h"
#include "misc/nparticleserver.h"
#include "particle/nareaemitter.h"

nNebulaScriptClass(nAreaEmitter, "npservemitter");

//------------------------------------------------------------------------------
/**
    Constructor
*/
nAreaEmitter::nAreaEmitter()
:  ref_gs(kernelServer,this),
   ref_ms(kernelServer,this)
{
    this->ref_gs = "/sys/servers/gfx";
    this->ref_ms = "/sys/servers/math";
    this->rand_key = 0;
    this->areaWidth = 0.0f;
    this->areaHeight = 0.0f;
    this->areaDepth = 0.0f;
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nAreaEmitter::~nAreaEmitter()
{ }

//------------------------------------------------------------------------------
/**
    Compute particle positions.
*/
void 
nAreaEmitter::Compute(nSceneGraph2 *sceneGraph)
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

    // if new particles need to be generated...
    if (t < tstamp) 
    {
        nMathServer *ms = this->ref_ms.get();
        matrix44 view_matrix;
        this->ref_gs->GetMatrix(N_MXM_VIEWER,view_matrix);

        // if this is a non-continuous system, compute the
        // current start/stop times...
        float t_rep_start = 0.0f;
        float t_rep_stop  = tstamp;
        if (this->t_repeat > 0.0f) 
        {
            t_rep_start = this->t_start + (this->t_repeat * ((float)floor(tstamp/this->t_repeat)));
            t_rep_stop  = t_rep_start + (this->t_stop - this->t_start);
        }

        // compute the model matrix (transforms from model into world space)
        matrix44 model_matrix = sceneGraph->GetTransform() * view_matrix;

        // compute current emitter position in world space
        vector3 pos(model_matrix.pos_component());

        // construct a 3x3 rotational matrix
        matrix33 modelMatrix33(
            model_matrix.M11, model_matrix.M12, model_matrix.M13,
            model_matrix.M21, model_matrix.M22, model_matrix.M23,
            model_matrix.M31, model_matrix.M32, model_matrix.M33);

        // generate new particles...
        vector3 dir;
        vector3 vel;
        while (t < tstamp) 
        {
            // within valid emission timeframe?
            if ((t>t_rep_start) && (t<t_rep_stop)) 
            {

                // get a pseudo random a and b angle 
                // between -180.0 and +180.0 degree
                float a = ms->PseudoRand(this->rand_key++);
                float b = ms->PseudoRand(this->rand_key++);

                // align a to inner and outer cone
                if (a > 0.0f) 
                {
                    a = this->icone0 + (a * (this->ocone0 - this->icone0));
                } 
                else 
                {
                    a = this->icone1 + (a * (this->ocone1 - this->icone1));
                }
                b *= 2 * N_PI;

                // construct a 3d vector in modelspace from the pseudo-random 
                // angles:
                // a is rotation around the xz plane,
                // b is rotation around y
                float cos_a = float(cos(a));
                float sin_a = float(sin(a));
                float cos_b = float(cos(b));
                float sin_b = float(sin(b));
                dir.x = sin_a * sin_b;
                dir.y = cos_a;
                dir.z = sin_a * cos_b;

                // Find random position within area
                if (areaWidth > 0)
                    pos.x += ms->PseudoRand(this->rand_key++) * this->areaWidth;
                if (areaHeight > 0)
                    pos.y += ms->PseudoRand(this->rand_key++) * this->areaHeight;
                if (areaDepth > 0)
                    pos.z += ms->PseudoRand(this->rand_key++) * this->areaDepth;

                // transform the modelspace direction into world space
                vel = (modelMatrix33 * dir) * this->speed;

                // add particle to system
                psys->AddParticle(t, pos, vel, modelMatrix33);
            }
            t += this->delta_t;
        }
    }

    // update the particle system with current acceleration
    psys->SetAccel(this->accel);

    // update the particle systems particles...
    psys->Update(tstamp);
}
