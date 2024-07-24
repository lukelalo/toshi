#define N_IMPLEMENTS nJointAnim
//-------------------------------------------------------------------
//  njanim_main.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelset.h"
#include "gfx/nchannelcontext.h"
#include "node/njoint.h"
#include "node/njointanim.h"

//--------------------------------------------------------------------
//  nJointAnim()
//  12-Jul-99   floh    created
//  15-May-00   floh    + align_times
//--------------------------------------------------------------------
nJointAnim::nJointAnim() :
    num_joints(0),
    num_states(0),
    cur_joint(0),
    cur_state(0),
    key_time(0),
    joint_array(0),
    state_array(0),
    key_array(0),
    t_res(0),
    r_res(0),
    in_state(N_JA_INSIDE_NONE),
    jointPointersDirty(false),
    refJointRoot(ks, this)
{
    // empty
}

//--------------------------------------------------------------------
//  ~nJointAnim()
//  12-Jul-99   floh    created
//--------------------------------------------------------------------
nJointAnim::~nJointAnim()
{
    if (this->joint_array)  delete[] this->joint_array;
    if (this->state_array)  delete[] this->state_array;
    if (this->key_array)    n_free(this->key_array);
    if (this->t_res)        n_free(this->t_res);
    if (this->r_res)        n_free(this->r_res);
}

//-------------------------------------------------------------------
/**
    If parent is a nVisNode object, link to it as depend node.

    History:
     - 13-Nov-00   floh    created
*/
//-------------------------------------------------------------------
void nJointAnim::Initialize(void)
{
    // register as depend node on parent
    nVisNode *p = (nVisNode *) this->GetParent();
    if (p && p->IsA(ks->FindClass("nvisnode"))) 
    {
        p->DependsOn(this);
    }
    
    // up to parent class
    nVisNode::Initialize();
}

//-------------------------------------------------------------------
/**
    Attach the channels are interested in to given channel set.

    History:
     - 23-Jun-01   floh    created
*/
//-------------------------------------------------------------------
void
nJointAnim::AttachChannels(nChannelSet* chnSet)
{
    n_assert(chnSet);
    int i;
    for (i = 0; i < this->num_states; i++)
    {
        nJAState *state = &(this->state_array[i]);
        state->chn_num = chnSet->AddChannel(this->refChannel.get(), state->name);
    }
    nAnimNode::AttachChannels(chnSet);
}

//--------------------------------------------------------------------
/**
    Read channel values for anim state weights.

    History:
     - 14-Jul-99   floh    created
*/
//-------------------------------------------------------------------
void 
nJointAnim::getChannels(nChannelContext* chnContext)
{
    n_assert(chnContext);
    int i;
    for (i=0; i<this->num_states; i++) 
    {
        nJAState *state = &(this->state_array[i]);
        state->w = chnContext->GetChannel1f(state->chn_num);
    }
}

//-------------------------------------------------------------------
/**
    Normalize all weights to a sum of 1.0.

    History:
     - 14-Jul-99   floh    created
*/
//-------------------------------------------------------------------
void nJointAnim::normWeights(void)
{
    int i;
    float w_sum = 0.0f;
    for (i=0; i<this->num_states; i++) {
        nJAState *state = &(this->state_array[i]);
        w_sum += state->w;
    }
    if (w_sum > 1.0f) {
        for (i=0; i<this->num_states; i++) {
            nJAState *state = &(this->state_array[i]);
            if (state->w > 0.0f) state->w /= w_sum;
        }
    }
}

//-------------------------------------------------------------------
/**
    Evaluate current state of joint depending on current time.

    History:
     - 14-Jul-99   floh    created
*/
//-------------------------------------------------------------------
void nJointAnim::evalJoints(double t)
{
    // reset translation and rotation
    memset(this->t_res,0,this->num_joints*sizeof(nJAKey));
    memset(this->r_res,0,this->num_joints*sizeof(nJAKey));

    // if anim states are not aligned we can compute
    // one current keyframe for all animations...
    double abs_frame = t / this->key_time;
    double f_num = floor(abs_frame);
    double f_pos = abs_frame - f_num;
    int key = (int) f_num;
    float l = (float) f_pos;
    int i;

    // evaluate joint positions
    for (i=0; i<this->num_states; i++) {
        nJAState *state = &(this->state_array[i]);
        if (state->w > 0.0f) {
            state->Eval(key,l,this->t_res,this->r_res);
        }
    }
}

//-------------------------------------------------------------------
/**
    Update joint objects with translation and rotation.

    History:
     - 14-Jul-99   floh    created
*/
//-------------------------------------------------------------------
void nJointAnim::updateJoints(void)
{
    int i;
    for (i=0; i<this->num_joints; i++) {
        nJAJoint *jnt = &(this->joint_array[i]);
        if (jnt->HasTrans()) {
            jnt->joint->Txyz(this->t_res[i].x,this->t_res[i].y,this->t_res[i].z);
        }
        if (jnt->HasRot()) {
            jnt->joint->Rxyz(this->r_res[i].x,this->r_res[i].y,this->r_res[i].z);
        }
    }
}

//-------------------------------------------------------------------
/**
    Does everything, is called both from Attach() and Compute()
    (unfortunately, joint hierarchies need to be updated twice per
    frame to act both as a hierarchy of 3d nodes (if you want to
    attach 3d objects to the joints) and as skeleton for a weighted
    mesh.

    History:
     - 23-Jun-01   floh    created
*/
//-------------------------------------------------------------------
void
nJointAnim::updateAll(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);
    if (this->jointPointersDirty)
    {
        this->lookupJointPointers();
    }

    nChannelContext* chnContext = sceneGraph->GetChannelContext();
    n_assert(chnContext);

    // get time channel (or whatever we are connected to)
    float time = chnContext->GetChannel1f(this->localChannelIndex);
    time /= this->scale;

    // get state weights
    this->getChannels(chnContext);

    // do the anim stuff
    this->normWeights();
    this->evalJoints(time);
    this->updateJoints();
}

//-------------------------------------------------------------------
/**
    Updates the joint hierarchy. This is necessary if the
    joints want to render themselves as 3d nodes.

    History:
     - 23-Jun-01   floh    created
*/
//-------------------------------------------------------------------
bool
nJointAnim::Attach(nSceneGraph2* sceneGraph)
{
    this->updateAll(sceneGraph);
    return nVisNode::Attach(sceneGraph);
}

//-------------------------------------------------------------------
/**
    Updates the joint hierarchy. This is usually called
    by the parent object, because we are its depend node.

    History:
     - 23-Jun-01   floh    created
*/
//-------------------------------------------------------------------
void
nJointAnim::Compute(nSceneGraph2* sceneGraph)
{
    nVisNode::Compute(sceneGraph);
    this->updateAll(sceneGraph);
}
        
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

