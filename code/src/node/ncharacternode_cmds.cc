#define N_IMPLEMENTS nCharacterNode
//------------------------------------------------------------------------------
//  ncharacternode_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "node/ncharacternode.h"
#include "kernel/npersistserver.h"

static void n_setreadonly(void* slf, nCmd* cmd);
static void n_getreadonly(void* slf, nCmd* cmd);
static void n_setskinmesh(void* slf, nCmd* cmd);
static void n_getskinmesh(void* slf, nCmd* cmd);
static void n_setcastshadow(void* slf, nCmd* cmd);
static void n_getcastshadow(void* slf, nCmd* cmd);
static void n_setstatechannel(void* slf, nCmd* cmd);
static void n_getstatechannel(void* slf, nCmd* cmd);
static void n_setanimfile(void* slf, nCmd* cmd);
static void n_getanimfile(void* slf, nCmd* cmd);
static void n_beginjoints(void* slf, nCmd* cmd);
static void n_addjoint(void* slf, nCmd* cmd);
static void n_endjoints(void* slf, nCmd* cmd);
static void n_getnumjoints(void* slf, nCmd* cmd);
static void n_getjoint(void* slf, nCmd* cmd);
static void n_beginstates(void* slf, nCmd* cmd);
static void n_addstate(void* slf, nCmd* cmd);
static void n_beginstateanims(void* slf, nCmd* cmd);
static void n_addstateanim(void* slf, nCmd* cmd);
static void n_endstateanims(void* slf, nCmd* cmd);
static void n_endstates(void* slf, nCmd* cmd);
static void n_getnumstates(void* slf, nCmd* cmd);
static void n_getstate(void* slf, nCmd* cmd);
static void n_getnumstateanims(void* slf, nCmd* cmd);
static void n_getstateanim(void* slf, nCmd* cmd);
static void n_setrenderskeleton(void* slf, nCmd* cmd);
static void n_getrenderskeleton(void* slf, nCmd* cmd);
static void n_setskeletoncolor(void* slf, nCmd* cmd);
static void n_getskeletoncolor(void* slf, nCmd* cmd);
static void n_beginhardpoints(void* slf, nCmd* cmd);
static void n_addhardpoint(void* slf, nCmd* cmd);
static void n_endhardpoints(void* slf, nCmd* cmd);
static void n_getnumhardpoints(void* slf, nCmd* cmd);
static void n_gethardpoint(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ncharacternode
    
    @superclass
    nanimnode

    @classinfo
    Wrap a character object complete with skeleton and animations into a
    nvisnode object.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setreadonly_b",           'SRDO', n_setreadonly);
    cl->AddCmd("b_getreadonly_v",           'GRDO', n_getreadonly);
    cl->AddCmd("v_setskinmesh_s",           'SSKM', n_setskinmesh);
    cl->AddCmd("s_getskinmesh_v",           'GSKM', n_getskinmesh);
    cl->AddCmd("v_setcastshadow_b",         'SCSS', n_setcastshadow);
    cl->AddCmd("b_getcastshadow_v",         'GCSS', n_getcastshadow);
    cl->AddCmd("v_setstatechannel_s",       'SSTC', n_setstatechannel);
    cl->AddCmd("s_getstatechannel_v",       'GSTC', n_getstatechannel);
    cl->AddCmd("v_setanimfile_s",           'SANF', n_setanimfile);
    cl->AddCmd("s_getanimfile_v",           'GANF', n_getanimfile);
    cl->AddCmd("v_beginjoints_i",           'BGJN', n_beginjoints);
    cl->AddCmd("v_addjoint_isifffffff",     'ADJN', n_addjoint);
    cl->AddCmd("v_endjoints_v",             'EDJN', n_endjoints);
    cl->AddCmd("i_getnumjoints_v",          'GNJN', n_getnumjoints);
    cl->AddCmd("sifffffff_getjoint_i",      'GTJN', n_getjoint);
    cl->AddCmd("v_beginstates_i",           'BGST', n_beginstates);
    cl->AddCmd("v_addstate_is",             'ADST', n_addstate);
    cl->AddCmd("v_beginstateanims_ii",      'BGSA', n_beginstateanims);
    cl->AddCmd("v_addstateanim_iiss",       'ADSA', n_addstateanim);
    cl->AddCmd("v_endstateanims_i",         'EDSA', n_endstateanims);
    cl->AddCmd("v_endstates_v",             'EDST', n_endstates);
    cl->AddCmd("i_getnumstates_v",          'GNST', n_getnumstates);
    cl->AddCmd("s_getstate_i",              'GTST', n_getstate);
    cl->AddCmd("i_getnumstateanims_i",      'GNSA', n_getnumstateanims);
    cl->AddCmd("ss_getstateanim_ii",        'GSTA', n_getstateanim);
    cl->AddCmd("v_setrenderskeleton_b",     'SRSK', n_setrenderskeleton);
    cl->AddCmd("b_getrenderskeleton_v",     'GRSK', n_getrenderskeleton);
    cl->AddCmd("v_setskeletoncolor_ffff",   'SSKC', n_setskeletoncolor);
    cl->AddCmd("ffff_getskeletoncolor_v",   'GSKC', n_getskeletoncolor);
    cl->AddCmd("v_beginhardpoints_i",       'BGHP', n_beginhardpoints);
    cl->AddCmd("v_addhardpoint_iis",        'ADHP', n_addhardpoint);
    cl->AddCmd("v_endhardpoints_v",         'EDHP', n_endhardpoints);
    cl->AddCmd("i_getnumhardpoints_v",      'GNHP', n_getnumhardpoints);
    cl->AddCmd("is_gethardpoint_i",         'GTHP', n_gethardpoint);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setreadonly
    @input
    b(ReadOnly)
    @output
    v
    @info
    Must be set to true if the characternode is the source for another
    dynamic vertex buffer class. Otherwise (if its going to be rendered),
    set it to false. Default is false.
*/
static
void
n_setreadonly(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    self->SetReadOnly(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getreadonly
    @input
    v
    @output
    b(ReadOnly)
    @info
    Get the read only flag.
*/
static
void
n_getreadonly(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    cmd->Out()->SetB(self->GetReadOnly());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setskinmesh
    @input
    s(SkinMesh)
    @output
    v
    @info
    Set path to source skin mesh node.
*/
static
void
n_setskinmesh(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    self->SetSkinMesh(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getskinmesh
    @input
    v
    @output
    s(SkinMesh)
    @info
    Get path to source skin mesh node.
*/
static
void
n_getskinmesh(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    cmd->Out()->SetS(self->GetSkinMesh());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcastshadow
    @input
    b(CastShadow)
    @output
    v
    @info
    Set to true if the character object should cast a shadow. Note that the
    source skin mesh must also be marked as a shadow caster.
*/
static
void
n_setcastshadow(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    self->SetCastShadow(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcastshadow
    @input
    v
    @output
    b(CastShadow)
    @info
    Get shadow caster flag.
*/
static
void
n_getcastshadow(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    cmd->Out()->SetB(self->GetCastShadow());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstatechannel
    @input
    s(StateChannelName)
    @output
    v
    @info
    Set name of a string animation channel with defines state switches.
*/
static
void
n_setstatechannel(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    self->SetStateChannel(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstatechannel
    @input
    v
    @output
    s(StateChannelName)
    @info
    Get name of a string animation channel with defines state switches.
*/
static
void
n_getstatechannel(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    cmd->Out()->SetS(self->GetStateChannel());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setanimfile
    @input
    s(AnimFileName)
    @output
    v
    @info
    Set name of .nanim file which contains all animation curves required by
    this character.
*/
static
void
n_setanimfile(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    self->SetAnimFile(cmd->In()->GetS());
}
//------------------------------------------------------------------------------
/**
    @cmd
    getanimfile
    @input
    v
    @output
    s(AnimFileName)
    @info
    Get name of .nanim file.
*/
static
void
n_getanimfile(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    cmd->Out()->SetS(self->GetAnimFile());
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginjoints
    @input
    i(NumJoints)
    @output
    v
    @info
    Begin defining the joint skeleton.
*/
static
void
n_beginjoints(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    self->BeginJoints(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    addjoint
    @input
    i(JointIndex), s(JointName), i(ParentIndex), fff(PoseTranslate), ffff(PoseRotate)
    @output
    v
    @info
    Add a joint to the joint skeleton.
*/
static
void
n_addjoint(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    int i0         = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    int i1         = cmd->In()->GetI();
    float f0       = cmd->In()->GetF();
    float f1       = cmd->In()->GetF();
    float f2       = cmd->In()->GetF();
    float f3       = cmd->In()->GetF();
    float f4       = cmd->In()->GetF();
    float f5       = cmd->In()->GetF();
    float f6       = cmd->In()->GetF();
    vector3 v0(f0, f1, f2);
    quaternion q0(f3, f4, f5, f6);
    self->AddJoint(i0, s0, i1, v0, q0);
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
    Finish defining the joint skeleton.
*/
static
void
n_endjoints(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    self->EndJoints();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumjoints
    @input
    v
    @output
    i
    @info
    Returns number of joints in skeleton.
*/
static
void
n_getnumjoints(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    cmd->Out()->SetI(self->GetNumJoints());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getjoint
    @input
    i(JointIndex)
    @output
    s(JointName), i(ParentIndex), fff(PoseTranslate), ffff(PoseRotate)    
    @info
    Returns joint parameters of given joint index.
*/
static
void
n_getjoint(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    const char* s0;
    int i0;
    vector3 v0;
    quaternion q0;
    self->GetJoint(cmd->In()->GetI(), s0, i0, v0, q0);
    cmd->Out()->SetS(s0);
    cmd->Out()->SetI(i0);
    cmd->Out()->SetF(v0.x);
    cmd->Out()->SetF(v0.y);
    cmd->Out()->SetF(v0.z);
    cmd->Out()->SetF(q0.x);
    cmd->Out()->SetF(q0.y);
    cmd->Out()->SetF(q0.z);
    cmd->Out()->SetF(q0.w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginstates
    @input
    i(NumStates)
    @output
    v
    @info
    Begin defining the animation states.
*/
static
void
n_beginstates(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    self->BeginStates(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    addstate
    @input
    i(StateIndex), s(StateName)
    @output
    v
    @info
    Add an animation state to the character.
*/
static
void
n_addstate(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    int i0 = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    self->AddState(i0, s0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginstateanims
    @input
    i(StateIndex), i(NumAnims)
    @output
    v
    @info
    Begin adding animations to a state.
*/
static
void
n_beginstateanims(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    int i0 = cmd->In()->GetI();
    int i1 = cmd->In()->GetI();
    self->BeginStateAnims(i0, i1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    addstateanim
    @input
    i(StateIndex), i(AnimIndex), s(AnimName), s(WeightChannel)
    @output
    v
    @info
    Add an animation to a state.
*/
static
void
n_addstateanim(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    int i0 = cmd->In()->GetI();
    int i1 = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->AddStateAnim(i0, i1, s0, s1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endstateanims
    @input
    i(StateIndex)
    @output
    v
    @info
    Finish adding animations to a state.
*/
static
void
n_endstateanims(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    self->EndStateAnims(cmd->In()->GetI());
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
    Finish adding states to the character.
*/
static
void
n_endstates(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    self->EndStates();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumstates
    @input
    v
    @output
    i(NumStates)
    @info
    Get number of animation states.
*/
static
void
n_getnumstates(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    cmd->Out()->SetI(self->GetNumStates());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstate
    @input
    i(StateIndex)
    @output
    v
    @info
    Get description of a state.
*/
static
void
n_getstate(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    const char* s0;
    self->GetState(cmd->In()->GetI(), s0);
    cmd->Out()->SetS(s0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumstateanims
    @input
    i(StateIndex)
    @output
    i(NumAnims)
    @info
    Get number of animations in a state.
*/
static
void
n_getnumstateanims(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    cmd->Out()->SetI(self->GetNumStateAnims(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstateanim
    @input
    i(StateIndex), i(AnimIndex)
    @output
    s(AnimName), s(WeightChannel)    
    @info
    Get description of a animation in a state.
*/
static
void
n_getstateanim(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    int i0 = cmd->In()->GetI();
    int i1 = cmd->In()->GetI();
    const char* s0;
    const char* s1;
    self->GetStateAnim(i0, i1, s0, s1);
    cmd->Out()->SetS(s0);
    cmd->Out()->SetS(s1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setrenderskeleton
    @input
    b(RenderSkeleton)
    @output
    v
    @info
    Turn skeleton debug visualization on/off.
*/
static
void
n_setrenderskeleton(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    self->SetRenderSkeleton(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setskeletoncolor
    @input
    f(Red), f(Green), f(Blue), f(Alpha)
    @output
    v
    @info
    Set color of skeleton. Only valid when setrenderskeleton is true.
*/
static
void
n_setskeletoncolor(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    vector4 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    v.w = cmd->In()->GetF();
    self->SetSkeletonColor(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getskeletoncolor

    @input
    v

    @output
    f(Red), f(Green), f(Blue), f(Alpha)

    @info
    Get color of skeleton.
*/
static void n_getskeletoncolor(void *o, nCmd *cmd)
{
    nCharacterNode *self = (nCharacterNode *) o;
    vector4 v = self->GetSkeletonColor();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrenderskeleton
    @input
    v
    @output
    b(RenderSkeleton)
    @info
    Get skeleton debug visualization flag.
*/
static
void
n_getrenderskeleton(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    cmd->Out()->SetB(self->GetRenderSkeleton());
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginhardpoints
    @input
    i(NumHardPoints)
    @output
    v
    @info
    Begin defining hardpoints. A hardpoint maps the position of a joint
    to a child n3DNode. Hardpoints are used to render 3d objects that
    are attached to a character joint.
*/
static
void
n_beginhardpoints(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    self->BeginHardPoints(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    addhardpoint
    @input
    i(HardPointIndex), i(JointIndex), s(3DNodeName)
    @output
    v
    @info
    Add a hardpoint. Takes the index of the hardpoint, a joint index,
    and the name of a child n3DNode.
*/
static
void
n_addhardpoint(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    int i0 = cmd->In()->GetI();
    int i1 = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    self->AddHardPoint(i0, i1, s0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endhardpoints
    @input
    v
    @output
    v
    @info
    Finish hardpoint definitions.
*/
static
void
n_endhardpoints(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    self->EndHardPoints();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumhardpoints
    @input
    v
    @output
    i(NumHardPoints)
    @info
    Get number of hardpoints.
*/
static
void
n_getnumhardpoints(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    cmd->Out()->SetI(self->GetNumHardPoints());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gethardpoint
    @input
    i(HardPointIndex)
    @output
    i(JointIndex), s(n3DNodeName)
    @info
    Get a hardpoint definition.
*/
static
void
n_gethardpoint(void* slf, nCmd* cmd)
{
    nCharacterNode* self = (nCharacterNode*) slf;
    int i0 = cmd->In()->GetI();
    int i1;
    const char* s0;
    self->GetHardPoint(i0, i1, s0);
    cmd->Out()->SetI(i1);
    cmd->Out()->SetS(s0);
}

//------------------------------------------------------------------------------
/**
    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nCharacterNode::SaveCmds(nPersistServer* fs)
{
    if (nAnimNode::SaveCmds(fs))
    {
        nCmd* cmd;

        //--- setreadonly ---
        cmd = fs->GetCmd(this, 'SRDO');
        cmd->In()->SetB(this->GetReadOnly());
        fs->PutCmd(cmd);

        //--- setskinmesh ---
        cmd = fs->GetCmd(this, 'SSKM');
        cmd->In()->SetS(this->GetSkinMesh());
        fs->PutCmd(cmd);

        //--- setcastshadow ---
        cmd = fs->GetCmd(this, 'SCSS');
        cmd->In()->SetB(this->GetCastShadow());
        fs->PutCmd(cmd);

        //--- setstatechannel --
        cmd = fs->GetCmd(this, 'SSTC');
        cmd->In()->SetS(this->GetStateChannel());
        fs->PutCmd(cmd);

        //--- setanimfile ---
        cmd = fs->GetCmd(this, 'SANF');
        cmd->In()->SetS(this->GetAnimFile());
        fs->PutCmd(cmd);
        
        // save joint skeleton
        int numJoints = this->GetNumJoints();
        if (numJoints > 0)
        {
            //--- beginjoints ---
            cmd = fs->GetCmd(this, 'BGJN');
            cmd->In()->SetI(numJoints);
            fs->PutCmd(cmd);

            // for each joint...
            int i;
            for (i = 0; i < numJoints; i++)
            {
                const char* jointName;
                int parentIndex;
                vector3 poseTranslate;
                quaternion poseRotate;

                //--- addjoint ---
                cmd = fs->GetCmd(this, 'ADJN');
                this->GetJoint(i, jointName, parentIndex, poseTranslate, poseRotate);                
                cmd->In()->SetI(i);
                cmd->In()->SetS(jointName);
                cmd->In()->SetI(parentIndex);
                cmd->In()->SetF(poseTranslate.x);
                cmd->In()->SetF(poseTranslate.y);
                cmd->In()->SetF(poseTranslate.z);
                cmd->In()->SetF(poseRotate.x);
                cmd->In()->SetF(poseRotate.y);
                cmd->In()->SetF(poseRotate.z);
                cmd->In()->SetF(poseRotate.w);
                fs->PutCmd(cmd);
            }

            //--- endjoints ---
            cmd = fs->GetCmd(this, 'EDJN');
            fs->PutCmd(cmd);
        }

        // save animation state
        int numStates = this->GetNumStates();
        if (numStates > 0)
        {
            //--- beginstates ---
            cmd = fs->GetCmd(this, 'BGST');
            cmd->In()->SetI(numStates);
            fs->PutCmd(cmd);

            // for each state...
            int i;
            for (i = 0; i < numStates; i++)
            {
                const char* s0;

                //--- addstate ---
                cmd = fs->GetCmd(this, 'ADST');
                this->GetState(i, s0);
                cmd->In()->SetI(i);
                cmd->In()->SetS(s0);
                fs->PutCmd(cmd);

                // save state animations
                int numAnims = this->GetNumStateAnims(i);
                if (numAnims > 0)
                {
                    //--- beginstateanims ---
                    cmd = fs->GetCmd(this, 'BGSA');
                    cmd->In()->SetI(i);
                    cmd->In()->SetI(numAnims);
                    fs->PutCmd(cmd);

                    // for each state anim...
                    int j;
                    for (j = 0; j < numAnims; j++)
                    {
                        const char* s0;
                        const char* s1;
                        
                        //--- addstateanim ---
                        cmd = fs->GetCmd(this, 'ADSA');
                        this->GetStateAnim(i, j, s0, s1);
                        cmd->In()->SetI(i);
                        cmd->In()->SetI(j);
                        cmd->In()->SetS(s0);
                        cmd->In()->SetS(s1);
                        fs->PutCmd(cmd);
                    }

                    //--- endstateanims ---
                    cmd = fs->GetCmd(this, 'EDSA');
                    cmd->In()->SetI(i);
                    fs->PutCmd(cmd);
                }
            }

            //--- endstates ---
            cmd = fs->GetCmd(this, 'EDST');
            fs->PutCmd(cmd);
            
            // save hardpoints
            if (this->numHardPoints > 0)
            {
                //--- beginhardpoints
                cmd = fs->GetCmd(this, 'BGHP');
                cmd->In()->SetI(this->numHardPoints);
                fs->PutCmd(cmd);
            
                int i;
                for (i = 0; i < this->numHardPoints; i++)
                {
                    int jointIndex;
                    const char* n3DNodeName;
                    this->GetHardPoint(i, jointIndex, n3DNodeName);

                    //--- addhardpoint ---
                    cmd = fs->GetCmd(this, 'ADHP');
                    cmd->In()->SetI(i);
                    cmd->In()->SetI(jointIndex);
                    cmd->In()->SetS(n3DNodeName);
                    fs->PutCmd(cmd);
                }

                //--- endhardpoints ---
                cmd = fs->GetCmd(this, 'EDHP');
                fs->PutCmd(cmd);
            }
        }
        return true;
    }
    return false;
}
