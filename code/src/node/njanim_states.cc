#define N_IMPLEMENTS nJointAnim
//-------------------------------------------------------------------
//  njanim_states.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "node/njointanim.h"

//-------------------------------------------------------------------
//  BeginStates()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::BeginStates(int num)
{
    n_assert(N_JA_INSIDE_NONE == this->in_state);
    n_assert(NULL == this->state_array);
    this->num_states  = num;
    this->cur_state   = 0;
    this->state_array = new nJAState[num];
    n_assert(this->state_array);
    this->in_state = N_JA_INSIDE_STATES;
}

//-------------------------------------------------------------------
//  SetState()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::SetState(int i, const char *name, int num_keys)
{
    n_assert(N_JA_INSIDE_STATES == this->in_state);
    n_assert(i < this->num_states);
    n_assert(name);
    this->state_array[i].Set(name,this->num_joints,num_keys);
}

//-------------------------------------------------------------------
//  EndStates()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::EndStates(void)
{
    n_assert(N_JA_INSIDE_STATES == this->in_state);

    // testen, ob alle States ausgefuellt sind
    int i;
    for (i=0; i<this->num_states; i++) {
        if (0 == this->state_array[i].num_keys) {
            n_error("nJointAnim::EndStates(): no state definition for index %d\n",i);
        }
    }

    // allokiere Arrays...
    this->allocArrays();
    this->in_state = N_JA_INSIDE_NONE;

    // notify visnode that channel set needs to be updated
    this->NotifyChannelSetDirty();
}

//-------------------------------------------------------------------
//  allocArrays()
//  Berechnet die Groesse der benoetigten Arrays fuer Rotations- und
//  Translations-Keys, allokiert diese und initialisiert die Array-
//  Pointer im State-Array.
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::allocArrays(void)
{
    n_assert(this->joint_array);
    n_assert(this->state_array);
    n_assert(NULL == this->key_array);
    int num_keys = 0;

    // zaehlen...
    int s,j;
    for (j=0; j<this->num_joints; j++) {
        nJAJoint *joint = &(this->joint_array[j]);
        bool has_r = joint->HasRot();
        bool has_t = joint->HasTrans();
        for (s=0; s<this->num_states; s++) {
            nJAState *state = &(this->state_array[s]);
            if (has_r) num_keys += state->num_keys;
            if (has_t) num_keys += state->num_keys;
        }
    }

    // allokieren...
    this->key_array = (nJAKey *) n_malloc(num_keys * sizeof(nJAKey));
    n_assert(this->key_array);

    // Pointer initialisieren
    nJAKey *cur_key = this->key_array;
    for (s=0; s<this->num_states; s++) {
        nJAState *state = &(this->state_array[s]);
        state->InitPointers(this->joint_array,cur_key);
    }
}

//-------------------------------------------------------------------
//  GetNumStates()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
int nJointAnim::GetNumStates(void)
{
    return this->num_states;
}

//-------------------------------------------------------------------
//  GetState()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::GetState(int i, const char *&name, int& nkeys)
{
    n_assert(this->state_array);
    n_assert(i < this->num_states);
    name  = this->state_array[i].name;
    nkeys = this->state_array[i].num_keys;
}

//-------------------------------------------------------------------
//  BeginKeys()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::BeginKeys(double ktime)
{
    n_assert(N_JA_INSIDE_NONE == this->in_state);
    n_assert(ktime > 0.0);
    this->key_time = ktime;
    this->in_state = N_JA_INSIDE_KEYS;
}

//-------------------------------------------------------------------
//  EndKeys()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::EndKeys(void)
{
    n_assert(N_JA_INSIDE_KEYS == this->in_state);
    this->in_state = N_JA_INSIDE_NONE;
}

//-------------------------------------------------------------------
//  getJointByName()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
int nJointAnim::getJointByName(const char *n)
{
    n_assert(n);
    n_assert(this->joint_array);
    int i;
    for (i=0; i<this->num_joints; i++) {
        nJAJoint *joint = &(this->joint_array[i]);
        if (strcmp(n,joint->name)==0) return i;
    }
    return -1;
}

//-------------------------------------------------------------------
//  getStateByName()
//  12-Jul-99   float   created
//-------------------------------------------------------------------
int nJointAnim::getStateByName(const char *n)
{
    n_assert(n);
    n_assert(this->state_array);
    int i;
    for (i=0; i<this->num_states; i++) {
        nJAState *state = &(this->state_array[i]);
        if (strcmp(n,state->name)==0) return i;
    }
    return -1;
}

//-------------------------------------------------------------------
//  getCurJointState()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::getCurJointState(const char *jname, const char *sname)
{
    int joint = this->getJointByName(jname);
    int state = this->getStateByName(sname);
    if (-1 == joint) {
        n_error("No joint '%s' defined!\n", jname);
    }
    if (-1 == state) {
        n_error("No state '%s' defined!\n", sname);
    }
    this->cur_joint = joint;
    this->cur_state = state;
}    

//-------------------------------------------------------------------
//  BeginRotate()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::BeginRotate(const char *jname, const char *sname)
{
    n_assert(this->joint_array);
    n_assert(this->state_array);
    n_assert(N_JA_INSIDE_KEYS == this->in_state);
    this->getCurJointState(jname,sname);

    // sicherstellen, dass fuer 'cur_joint' Rotation erlaubt ist
    nJAJoint *joint = &(this->joint_array[this->cur_joint]);
    if (!joint->HasRot()) {
        n_error("Joint '%s' has been defined with 'has no rotation'\n",   
                 joint->name);
    }
    this->in_state = N_JA_INSIDE_ROTATE;
}

//-------------------------------------------------------------------
//  BeginTranslate()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::BeginTranslate(const char *jname, const char *sname)
{
    n_assert(this->joint_array);
    n_assert(this->state_array);
    n_assert(N_JA_INSIDE_KEYS == this->in_state);
    this->getCurJointState(jname,sname);

    // sicherstellen, dass fuer 'cur_joint' Translation erlaubt ist
    nJAJoint *joint = &(this->joint_array[this->cur_joint]);
    if (!joint->HasTrans()) {
        n_error("Joint '%s' has been defined with 'has no translation'\n",   
                 joint->name);
    }
    this->in_state = N_JA_INSIDE_TRANSLATE;
}

//-------------------------------------------------------------------
//  SetKey()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::SetKey(int i, float x, float y, float z)
{
    n_assert((N_JA_INSIDE_ROTATE==this->in_state)||(N_JA_INSIDE_TRANSLATE==this->in_state));
    nJAState *state = &(this->state_array[this->cur_state]);
    if (N_JA_INSIDE_ROTATE == this->in_state) {
        state->SetRotKey(i, this->cur_joint, x, y, z);
    } else {
        state->SetTransKey(i, this->cur_joint, x, y, z);
    }
}

//-------------------------------------------------------------------
//  EndRotate()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::EndRotate(void)
{
    n_assert(N_JA_INSIDE_ROTATE == this->in_state);
    this->in_state = N_JA_INSIDE_KEYS;
}

//-------------------------------------------------------------------
//  EndTranslate()
//  12-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::EndTranslate(void)
{
    n_assert(N_JA_INSIDE_TRANSLATE == this->in_state);
    this->in_state = N_JA_INSIDE_KEYS;
}

//-------------------------------------------------------------------
//  GetRotKey()
//  13-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::GetRotKey(int i,
                           const char *jname,
                           const char *sname,
                           float& x, float& y, float& z)
{
    n_assert(this->joint_array);
    n_assert(this->state_array);
    n_assert(N_JA_INSIDE_NONE == this->in_state);
    this->getCurJointState(jname,sname);

    // sicherstellen, dass fuer 'cur_joint' Rotation erlaubt ist
    nJAJoint *joint = &(this->joint_array[this->cur_joint]);
    nJAState *state = &(this->state_array[this->cur_state]);
    if (!joint->HasRot()) {
        n_error("Joint '%s' has been defined with 'has no rotation'\n",   
                 joint->name);
    }
    state->GetRotKey(i,this->cur_joint,x,y,z);
}

//-------------------------------------------------------------------
//  GetTransKey()
//  13-Jul-99   floh    created
//-------------------------------------------------------------------
void nJointAnim::GetTransKey(int i,
                             const char *jname,
                             const char *sname,
                             float& x, float& y, float& z)
{
    n_assert(this->joint_array);
    n_assert(this->state_array);
    n_assert(N_JA_INSIDE_NONE == this->in_state);
    this->getCurJointState(jname,sname);

    // sicherstellen, dass fuer 'cur_joint' Translation erlaubt ist
    nJAJoint *joint = &(this->joint_array[this->cur_joint]);
    nJAState *state = &(this->state_array[this->cur_state]);
    if (!joint->HasTrans()) {
        n_error("Joint '%s' has been defined with 'has no translation'\n",   
                 joint->name);
    }
    state->GetTransKey(i,this->cur_joint,x,y,z);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

