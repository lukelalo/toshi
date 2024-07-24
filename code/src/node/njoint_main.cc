#define N_IMPLEMENTS nJoint
//------------------------------------------------------------------------------
//  njoint_main.cc
//  (C) 1999 A.Weissflog
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "misc/nmathserver.h"
#include "node/njoint.h"
#include "gfx/nscenegraph2.h"

//------------------------------------------------------------------------------
/**
    One time evaluation of internal matrices (all matrices which represent
    the relaxed state of the joint in one way or another).

    @param  parentJoint     the parent joint (0 if root joint)
*/
void 
nJoint::EvalRelaxedState(nJoint *p)
{
    // compute rm
    this->rm = this->mo;
    this->rm.translate(this->n);
    if (p) this->rm.mult_simple(p->rm);
    
    // compute irm
    this->irm = this->rm;
    this->irm.invert_simple();

    // compute nirm
    this->nirm = this->irm;
    this->nirm.M41 = this->nirm.M42 = this->nirm.M43 = 0.0f;

    // recurively distribute to children in the joint skeleton
    nClass *jointClass = this->GetClass();
    nJoint *curChild;
    for (curChild = (nJoint*) this->GetHead(); 
         curChild;
         curChild = (nJoint *) curChild->GetSucc()) 
    {
        if (curChild->IsInstanceOf(jointClass))
        {
            curChild->EvalRelaxedState(this);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Updates the current state of this joint.
    
    @param  parentJoint     pointer to parent joint (0 if root joint)
*/
void
nJoint::updateMatrix(nJoint *p)
{
    if (get(USEQUAT)) {

        matrix44 moq(this->oq);
        matrix44 mq(this->q);

        this->m = moq;
        this->m.mult_simple(mq);
        this->m.translate(this->t + this->n);
        if (p) {
            this->m.mult_simple(p->m);
            this->pm = p->m;
        } else {
            this->pm.ident();
        };

        this->irm_times_m = this->irm;
        this->irm_times_m.mult_simple(this->m);

        this->nm = this->m;
        this->nm.M41 = this->nm.M42 = this->nm.M43 = 0.0f;
        this->nirm_times_nm = this->nirm;
        this->nirm_times_nm.mult_simple(this->nm);
        
    } else {
/*
        this->mx.ident();
        this->mx.rotate_x(this->r.x);
        this->mx.rotate_y(this->r.y);
        this->mx.rotate_z(this->r.z);
        this->mx *= this->rm;        
        this->mx.translate(this->t);
*/
        matrix44 mat;
        mat.rotate_x(this->r.x);
        mat.rotate_y(this->r.y);
        mat.rotate_z(this->r.z);
        mat.mult_simple(this->rm);
        mat.translate(this->t);

        this->m = this->irm;
        this->m.mult_simple(mat);
        if (p) {
            this->m.mult_simple(p->m);
            this->pm = p->m;
        } else {
            this->pm.ident();
        };

        this->irm_times_m = this->m;

        this->nm = this->m;
        this->nm.M41 = this->nm.M42 = this->nm.M43 = 0.0f;
        this->nirm_times_nm = this->nm;
    }
}

//------------------------------------------------------------------------------
/**
    Recursively update the state of the whole joint skeleton. Call on
    parent joint with an argument of 0.
    
    @param  parentJoint     pointer to parent joint or 0
*/
void
nJoint::EvalCurrentState(nJoint* parentJoint)
{
    // update own state
    nJoint::updateMatrix(parentJoint);

    // recursively update child joints
    nClass *jointClass = this->GetClass();
    nJoint *curChild;
    for (curChild = (nJoint*) this->GetHead(); 
         curChild; 
         curChild = (nJoint*) curChild->GetSucc()) 
    {
        if (curChild->IsInstanceOf(jointClass)) 
        {
            curChild->EvalCurrentState(this);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Overridden Compute() method, this will act like n3DNode::Compute(),
    but takes the orientation of the joint into account. This will basically
    let the joint act as a normal 3d node, so that visual objects can
    be attached to joints (like a sword to a hand, etc).

    @param  sceneGraph      pointer to nSceneGraph2 object
*/
void
nJoint::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    // first hand Compute() to my depend nodes (skip n3DNode::Compute())
    nVisNode::Compute(sceneGraph);

    // get my own updated state
    matrix44 resMatrix;
    resMatrix.scale(this->s);
    resMatrix.rotate_x(this->r.x);
    resMatrix.rotate_y(this->r.y);
    resMatrix.rotate_z(this->r.z);
    resMatrix.mult_simple(this->mo);
    resMatrix.translate(this->n);
    resMatrix.translate(this->t);

    // multiply with the parent matrix stored in n3DNode::Attach()
    // to flatten the matrix hierarchy
    resMatrix.mult_simple(*this->parentMatrix);

    // write the resulting transformation into the current scene graph node
    sceneGraph->SetTransform(resMatrix);

    // FIXME: LOD value???
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
