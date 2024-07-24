#define N_IMPLEMENTS nJointAnim
//-------------------------------------------------------------------
//  node/njanim_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/njointanim.h"

static void n_setjointroot(void*, nCmd*);
static void n_getjointroot(void*, nCmd*);
static void n_beginjoints(void *, nCmd *);
static void n_setjoint(void *, nCmd *);
static void n_endjoints(void *, nCmd *);
static void n_beginstates(void *, nCmd *);
static void n_setstate(void *, nCmd *);
static void n_endstates(void *, nCmd *);
static void n_getnumjoints(void *, nCmd *);
static void n_getnumstates(void *, nCmd *);
static void n_getjoint(void *, nCmd *);
static void n_getstate(void *, nCmd *);
static void n_beginkeys(void *, nCmd *);
static void n_beginrotate(void *, nCmd *);
static void n_begintranslate(void *, nCmd *);
static void n_setkey(void *, nCmd *);
static void n_endrotate(void *, nCmd *);
static void n_endtranslate(void *, nCmd *);
static void n_endkeys(void *, nCmd *);
static void n_getrotkey(void *, nCmd *);
static void n_gettranskey(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    njointanim

    @superclass
    nanimnode

    @classinfo
    As the component system of mixers and interpolators turned
    out to be to slow for fat blend animations here's an optimized
    class. The class combines the whole animation for a joint skeleton
    in one object, and it is therefore capable of many internal 
    optimizations.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setjointroot_s",      'SJTR', n_setjointroot);
    cl->AddCmd("s_getjointroot_v",      'GJTR', n_getjointroot);
    cl->AddCmd("v_beginjoints_i",       'BJNT', n_beginjoints);
    cl->AddCmd("v_setjoint_issbb",      'SJNT', n_setjoint);
    cl->AddCmd("v_endjoints_v",         'EJNT', n_endjoints);
    cl->AddCmd("v_beginstates_i",       'BSTT', n_beginstates);
    cl->AddCmd("v_setstate_isi",        'SSTT', n_setstate);
    cl->AddCmd("v_endstates_v",         'ESTT', n_endstates);
    cl->AddCmd("i_getnumjoints_v",      'GNJS', n_getnumjoints);
    cl->AddCmd("i_getnumstates_v",      'GNST', n_getnumstates);
    cl->AddCmd("ssbb_getjoint_i",       'GJNT', n_getjoint);
    cl->AddCmd("si_getstate_i",         'GSTT', n_getstate);
    cl->AddCmd("v_beginkeys_f",         'BGKS', n_beginkeys);
    cl->AddCmd("v_beginrotate_ss",      'BGRT', n_beginrotate);
    cl->AddCmd("v_begintranslate_ss",   'BGTR', n_begintranslate);
    cl->AddCmd("v_setkey_ifff",         'SKEY', n_setkey);
    cl->AddCmd("v_endrotate_v",         'EDRT', n_endrotate);
    cl->AddCmd("v_endtranslate_v",      'EDTR', n_endtranslate);
    cl->AddCmd("v_endkeys_v",           'EDKS', n_endkeys);
    cl->AddCmd("fff_getrotkey_iss",     'GRKY', n_getrotkey);
    cl->AddCmd("fff_gettranskey_iss",   'GTKY', n_gettranskey);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setjointroot

    @input
    s (JointRoot)

    @output
    v

    @info
    Define an optional joint root object which is put in front
    of the joint names defined inside 'beginjoints'/'endjoints'.
    This is optional. 
*/
static void n_setjointroot(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    self->SetJointRoot(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getjointroot

    @input
    v

    @output
    s (JointRoot)

    @info
    Get optional path to joint root object.
*/
static void n_getjointroot(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    cmd->Out()->SetS(self->GetJointRoot());
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginjoints

    @input
    i (NumJoints)

    @output
    v

    @info
    Starts the joint definition. You pass the number of joints
    in the skeleton. For each joint there must follow a 'setjoint'.
    The joint definition is ended with a 'endjoints'
*/
static void n_beginjoints(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    self->BeginJoints(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setjoint

    @input
    i (Index), s (ShortName), s (Path), b (HasTrans), b (HasRot) 

    @output
    v

    @info
    Sets joint informations. Besides Index you pass:
    ShortName    - identifies the joint from now on.
    Path         - path to the joint, starting with the root-joint.
    HasTrans/Rot - two bools stating if the joint will be translated 
                   and/or rotated. 
*/
static void n_setjoint(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    int i = cmd->In()->GetI();
    const char *s0 = cmd->In()->GetS();
    const char *s1 = cmd->In()->GetS();
    bool b0 = cmd->In()->GetB();
    bool b1 = cmd->In()->GetB();
    self->SetJoint(i,s0,s1,b0,b1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endjoints

    @input
    v

    @output
    v

    @info
    Ends the definition of joints.
*/
static void n_endjoints(void *o, nCmd *)
{
    nJointAnim *self = (nJointAnim *) o;
    self->EndJoints();
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginstates

    @input
    i (NumStates)

    @output
    v

    @info
    Starts the definition of AnimStates. AnimStates are
    mixed to a resulting animation by the means of
    weights. The name of the AnimState is identical to
    the name of a channel describing the actual weighting
    of the state. The definition of AnimStates must take
    place AFTER the joint definition.
*/
static void n_beginstates(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    self->BeginStates(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstate

    @input
    i (Index), s (Name), i (NumKeys)

    @output
    v

    @info
    Describes an AnimState. Besides the Index the Name
    of the States (identical with the weight channel name)
    and the number of keyframes is passed.
*/
static void n_setstate(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    {
        int i         = cmd->In()->GetI();
        const char *s = cmd->In()->GetS();
        int nk        = cmd->In()->GetI();
        self->SetState(i,s,nk);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    endstates

    @input
    v

    @output
    v

    @info
    Ends a state definition.
*/
static void n_endstates(void *o, nCmd *)
{
    nJointAnim *self = (nJointAnim *) o;
    self->EndStates();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumjoints

    @input
    v

    @output
    i (NumJoints)

    @info
    Returns the number of joint definitions as
    created with 'beginjoints'
*/
static void n_getnumjoints(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    cmd->Out()->SetI(self->GetNumJoints());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumstates

    @input
    v

    @output
    i (NumStates)

    @info
    Returns the number of state definitions as
    defined with 'beginstate'
*/
static void n_getnumstates(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    cmd->Out()->SetI(self->GetNumStates());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getjoint

    @input
    i (Index)

    @output
    s (ShortName), s (Path), b (HasTrans), b (HasRot) 

    @info
    Returns the definition joint number 'Index'.
*/
static void n_getjoint(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    const char *s0,*s1;
    bool b0,b1;
    int i = cmd->In()->GetI();
    self->GetJoint(i,s0,s1,b0,b1);
    cmd->Out()->SetS(s0);
    cmd->Out()->SetS(s1);
    cmd->Out()->SetB(b0);
    cmd->Out()->SetB(b1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstate

    @input
    i (Index)

    @output
    s (Name), i (NumKeys)

    @info
    Returns the state definition number 'Index'.
*/
static void n_getstate(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    const char *s;
    int nk;
    int i = cmd->In()->GetI();
    self->GetState(i,s,nk);
    cmd->Out()->SetS(s);
    cmd->Out()->SetI(nk);
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginkeys

    @input
    f (KeyframeTime)

    @output
    v

    @info
    Starts the keyframe definition. You pass the length
    of a frame in seconds. Between 'beginkeys'/'endkeys'
    there are ONLY 'beginrotate' and 'begintranslate' block
    allowed.
*/
static void n_beginkeys(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    self->BeginKeys(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginrotate

    @input
    s (JointName), s (StateName)

    @output
    v

    @info
    Starts the rotation definition for the given
    joint and state. There must follow as many
    'setkeys' as the state has keyframes. Additionally
    the joint must have been created on 'setjoint' with
    the HasRot flag enabled.
*/
static void n_beginrotate(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    const char *jname = cmd->In()->GetS();
    const char *sname = cmd->In()->GetS();
    self->BeginRotate(jname,sname);
}

//------------------------------------------------------------------------------
/**
    @cmd
    begintranslate

    @input
    s (JointName), s (StateName)

    @output
    v

    @info
    Starts the translation definition for the given joint
    and state. There must follow as many
    'setkeys' as the state has keyframes. Additionally
    the joint must have been created on 'setjoint' with
    the HasTrans flag enabled.
*/
static void n_begintranslate(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    const char *jname = cmd->In()->GetS();
    const char *sname = cmd->In()->GetS();
    self->BeginTranslate(jname,sname);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setkey

    @input
    i (Index), f (X), f (Y), f (Z)

    @output
    v

    @info
    Sets a rotation or translation keyframe, depending on
    if inside a 'beginrotate' or 'begintranslate'.
*/
static void n_setkey(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    int i   = cmd->In()->GetI();
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->SetKey(i,x,y,z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endrotate

    @input
    v

    @output
    v

    @info
    Ends a rotation keyframe definition.
*/
static void n_endrotate(void *o, nCmd *)
{
    nJointAnim *self = (nJointAnim *) o;
    self->EndRotate();
}

//------------------------------------------------------------------------------
/**
    @cmd
    endtranslate

    @input
    v

    @output
    v

    @info
    Ends a translation keyframe definition.
*/
static void n_endtranslate(void *o, nCmd *)
{
    nJointAnim *self = (nJointAnim *) o;
    self->EndTranslate();
}

//------------------------------------------------------------------------------
/**
    @cmd
    endkeys

    @input
    v

    @output
    v

    @info
    Ends a complete keyframe definition.
*/
static void n_endkeys(void *o, nCmd *)
{
    nJointAnim *self = (nJointAnim *) o;
    self->EndKeys();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrotkey

    @input
    i (Index), s (JointName), s (StateName)

    @output
    f (X), f (Y), f (Z)

    @info
    Return the rotation keyframe definition for keyframe
    number 'Index' for the given joint and state.
*/
static void n_getrotkey(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    int i = cmd->In()->GetI();
    const char *jname = cmd->In()->GetS();
    const char *sname = cmd->In()->GetS();
    float x,y,z;
    self->GetRotKey(i,jname,sname,x,y,z);
    cmd->Out()->SetF(x);
    cmd->Out()->SetF(y);
    cmd->Out()->SetF(z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettranskey

    @input
    i (Index), s (JointName), s (StateName)

    @output
    f (X), f (Y), f (Z)

    @info
    Return the translation keyframe definition for keyframe
    number 'Index' for the given joint and state.
*/
static void n_gettranskey(void *o, nCmd *cmd)
{
    nJointAnim *self = (nJointAnim *) o;
    int i = cmd->In()->GetI();
    const char *jname = cmd->In()->GetS();
    const char *sname = cmd->In()->GetS();
    float x,y,z;
    self->GetTransKey(i,jname,sname,x,y,z);
    cmd->Out()->SetF(x);
    cmd->Out()->SetF(y);
    cmd->Out()->SetF(z);
}

//-------------------------------------------------------------------
//  SaveCmds()
//  02-Jan-99   floh    machine generated
//  16-May-00   floh    setaligntimes
//  23-Jun-01   floh    removed setaligntimes, never really worked
//-------------------------------------------------------------------
bool nJointAnim::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nAnimNode::SaveCmds(fs)) {
        nCmd *cmd;
        int i;
        const char *s0,*s1;
        bool b0,b1;
        int i0;

        //--- setjointroot ---
        if (this->GetJointRoot())
        {
            cmd = fs->GetCmd(this, 'SJTR');
            cmd->In()->SetS(this->GetJointRoot());
            fs->PutCmd(cmd);
        }

        //--- beginjoints/setjoint/endjoints ---
        if (this->num_joints > 0)
        {
            cmd = fs->GetCmd(this, 'BJNT');
            cmd->In()->SetI(this->num_joints);
            fs->PutCmd(cmd);
        
            for (i=0; i<this->num_joints; i++) {
                this->GetJoint(i,s0,s1,b0,b1);
                cmd = fs->GetCmd(this, 'SJNT');
                cmd->In()->SetI(i);
                cmd->In()->SetS(s0);
                cmd->In()->SetS(s1);
                cmd->In()->SetB(b0);
                cmd->In()->SetB(b1);
                fs->PutCmd(cmd);
            }

            cmd = fs->GetCmd(this, 'EJNT');
            fs->PutCmd(cmd);
        }

        //--- beginstates/setstate/endstates ---
        if (this->num_states > 0)
        {
            cmd = fs->GetCmd(this, 'BSTT');
            cmd->In()->SetI(this->num_states);
            fs->PutCmd(cmd);

            for (i=0; i<this->num_states; i++) {
                this->GetState(i,s0,i0);
                cmd = fs->GetCmd(this, 'SSTT');
                cmd->In()->SetI(i);
                cmd->In()->SetS(s0);
                cmd->In()->SetI(i0);
                fs->PutCmd(cmd);
            }

            cmd = fs->GetCmd(this, 'ESTT');
            fs->PutCmd(cmd);
        }

        //--- write keys ---
        int s,j;

        //--- beginkeys ---
        cmd = fs->GetCmd(this, 'BGKS');
        cmd->In()->SetF((float)this->key_time);
        fs->PutCmd(cmd);

        for (s=0; s<this->num_states; s++) 
        {
            nJAState *state   = &(this->state_array[s]);
            const char *sname = (const char *) &(state->name);
            for (j=0; j<this->num_joints; j++) 
            {
                nJAJoint *joint   = &(this->joint_array[j]);
                const char *jname = (const char *) &(joint->name);

                if (joint->HasTrans()) 
                {
                    //--- begintranslate ---
                    cmd = fs->GetCmd(this, 'BGTR');
                    cmd->In()->SetS(jname);
                    cmd->In()->SetS(sname);
                    fs->PutCmd(cmd);

                    int k;
                    for (k=0; k<state->num_keys; k++) 
                    {
                        //--- setkey ---
                        float x,y,z;
                        state->GetTransKey(k,j,x,y,z);
                        cmd = fs->GetCmd(this, 'SKEY');
                        cmd->In()->SetI(k);
                        cmd->In()->SetF(x);
                        cmd->In()->SetF(y);
                        cmd->In()->SetF(z);
                        fs->PutCmd(cmd);
                    }

                    //--- endtranslate ---
                    cmd = fs->GetCmd(this, 'EDTR');
                    fs->PutCmd(cmd);
                }

                if (joint->HasRot()) {
                    //--- beginrotate ---
                    cmd = fs->GetCmd(this, 'BGRT');
                    cmd->In()->SetS(jname);
                    cmd->In()->SetS(sname);
                    fs->PutCmd(cmd);

                    int k;
                    for (k=0; k<state->num_keys; k++) 
                    {
                        //--- setkey ---
                        float x,y,z;
                        state->GetRotKey(k,j,x,y,z);
                        cmd = fs->GetCmd(this, 'SKEY');
                        cmd->In()->SetI(k);
                        cmd->In()->SetF(x);
                        cmd->In()->SetF(y);
                        cmd->In()->SetF(z);
                        fs->PutCmd(cmd);
                    }

                    //--- endrotate ---
                    cmd = fs->GetCmd(this, 'EDRT');
                    fs->PutCmd(cmd);
                }
            }
        }

        //--- endkeys ---
        cmd = fs->GetCmd(this, 'EDKS');
        fs->PutCmd(cmd);

        retval = true;
    }
    return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
