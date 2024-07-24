#define N_IMPLEMENTS nJointAnim
//-------------------------------------------------------------------
//  njanim_joints.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "node/njointanim.h"

//--------------------------------------------------------------------
/**
    Sets an optional root object for the joint hierarchy, this
    is necessary if the joint hierarchy is not a direct child of
    this object. One can think of this joint root object as a path
    prefix which is put in front of the joint names defined inside
    BeginJoints()/EndJoints().

    @param  path    path to the parent of the topmost joint

    26-Jul-01   floh    created
*/
void
nJointAnim::SetJointRoot(const char* name)
{
    n_assert(name);
    this->refJointRoot = name;
}

//--------------------------------------------------------------------
/**
    Get the joint root object as defined per SetJointRoot().
    A return value of 0 is valid (no joint root).

    @return     path to parent of the topmost joint

    26-Jul-01   floh    created
*/
const char* 
nJointAnim::GetJointRoot()
{
    return this->refJointRoot.getname();
}

//--------------------------------------------------------------------
//  BeginJoints()
//  Die Joint-Hierarchie muss bereits existieren.
//  12-Jul-99   floh    created
//  14-Jul-99   floh    + t_res und r_res Arrays werden allokiert
//--------------------------------------------------------------------
void nJointAnim::BeginJoints(int num)
{
    n_assert(this->in_state == N_JA_INSIDE_NONE);
    n_assert(NULL == this->joint_array);
    this->num_joints = num;
    this->cur_joint  = 0;
    this->joint_array = new nJAJoint[num];
    this->t_res = (nJAKey *) n_malloc(num * sizeof(nJAKey));
    this->r_res = (nJAKey *) n_malloc(num * sizeof(nJAKey));
    n_assert(this->joint_array);
    n_assert(this->t_res);
    n_assert(this->r_res);
    this->in_state = N_JA_INSIDE_JOINTS;
}

//--------------------------------------------------------------------
//  SetJoint()
//  12-Jul-99   floh    created
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//--------------------------------------------------------------------
void nJointAnim::SetJoint(int i,
                          const char *name,
                          const char *path,
                          bool has_trans,
                          bool has_rot)
{
    n_assert(N_JA_INSIDE_JOINTS == this->in_state);
    n_assert(i < this->num_joints);
    n_assert(name);
    n_assert(path);
    this->joint_array[i].Set(name,path,has_trans,has_rot);
}

//--------------------------------------------------------------------
//  EndJoints()
//  Testet, ob alle Joints ausgefuellt wurden.
//  12-Jul-99   floh    created
//--------------------------------------------------------------------
void nJointAnim::EndJoints(void)
{
    n_assert(N_JA_INSIDE_JOINTS == this->in_state);
    this->in_state = N_JA_INSIDE_NONE;
    this->jointPointersDirty = true;
}

//------------------------------------------------------------------------------
/**
    Lookup the joint pointers, call from inside Compute() if 
    this->jointPointersDirty = true.
*/
void 
nJointAnim::lookupJointPointers()
{
    int i;
    if (this->refJointRoot.getname())
    {
            
        ks->PushCwd(this->refJointRoot.get());
    }
    else
    {
        ks->PushCwd(this->GetParent());
    }

    for (i=0; i<this->num_joints; i++)
    {
        if (!this->joint_array[i].LookupJoint(ks))
        {
            n_printf("Could not lookup nJoint '%s'!\n", this->joint_array[i].path);
            n_assert(false);
        }
    }
    this->jointPointersDirty = false;
    
    ks->PopCwd();
}

//--------------------------------------------------------------------
//  GetNumJoints()
//  12-Jul-99   floh    created
//--------------------------------------------------------------------
int nJointAnim::GetNumJoints(void)
{
    return this->num_joints;
}

//--------------------------------------------------------------------
//  GetJoint()
//  12-Jul-99   floh    created
//--------------------------------------------------------------------
void nJointAnim::GetJoint(int i, const char *&jname, const char *&path, bool& has_trans, bool& has_rot)
{
    n_assert(this->joint_array);
    n_assert(i < this->num_joints);
    jname = this->joint_array[i].name;
    path  = this->joint_array[i].path;
    has_trans = this->joint_array[i].HasTrans();
    has_rot   = this->joint_array[i].HasRot();
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

