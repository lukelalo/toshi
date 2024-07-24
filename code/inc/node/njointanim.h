#ifndef N_JOINTANIM_H
#define N_JOINTANIM_H
//--------------------------------------------------------------------
/**
    @class nJointAnim
    @ingroup NebulaVisnodeModule

    @brief Compact joint-skeleton-animator

    As the component system of mixers and interpolators turned
    out to be to slow for fat blend animations here's an optimized
    class. The class combines the whole animation for a joint skeleton
    in one object, and it is therefore capable of many internal 
    optimizations.
*/
//--------------------------------------------------------------------
#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#ifndef N_ANIMNODE_H
#include "node/nanimnode.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nJointAnim
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
struct nJAKey {
    float x,y,z;
};

//--------------------------------------------------------------------
class nJoint;
class nJAJoint {
public:
    enum {
        N_JASTATE_HASROT = (1<<0),
        N_JASTATE_HASTRANS = (1<<1),
    };
    char name[N_MAXNAMELEN];    // Kurz-Name des Joints
    const char *path;           // Pfadname des Joints
    nJoint *joint;              // zugehoeriger Objekt-Pointer
    int flags;                  // N_JAJOINT_*

    nJAJoint() {
        name[0] = 0;
        path    = NULL;
        joint   = NULL;
        flags   = 0;
    }
    ~nJAJoint() {
        if (path) n_free((void *)path);
    };
    void Set(const char *n, const char *p, bool has_trans, bool has_rot) {
        n_strncpy2(name,n,sizeof(name));
        path = n_strdup(p);
        if (has_trans) this->flags |= N_JASTATE_HASTRANS;
        if (has_rot)   this->flags |= N_JASTATE_HASROT;
    };
    bool HasTrans(void) {
        return (this->flags & N_JASTATE_HASTRANS) ? true : false;
    };
    bool HasRot(void) {
        return (this->flags & N_JASTATE_HASROT) ? true : false;
    };
    bool LookupJoint(nKernelServer *ks) {
        this->joint = (nJoint *) ks->Lookup(path);
        return (this->joint ? true : false);
    };
};
//---------------------------------------------------------------------
class nJAState {
public:
    char name[N_MAXNAMELEN];    // Name (== Channel-Name)
    int chn_num;                // zugehoerige Channel-Nummer
    float w;                    // State-Weight, wird aus Channel gelesen
    int num_joints;             // Anzahl Joints
    int num_keys;               // Anzahl Keyframes

    nJAKey **trans;             // Translate-Keys, darf NULL sein, size=num_joints
    nJAKey **rot;               // Rotate-Keys, darf NULL sein, size=num_joints

    //---------------------------------------------------------------
    nJAState() {
        name[0] = 0;
        chn_num = -1;
        w = 0.0f;
        num_joints = 0;
        num_keys   = 0;
        trans = NULL;
        rot   = NULL;
    };
    //---------------------------------------------------------------
    ~nJAState() {
        if (trans)  n_free(trans);
        if (rot)    n_free(rot);
    };
    //---------------------------------------------------------------
    void Set(const char *n, int njoints, int nkeys) {
        n_strncpy2(name,n,sizeof(name));
        num_keys   = nkeys;
        num_joints = njoints;
    };
    //---------------------------------------------------------------
    void SetTransKey(int i, int jnum, float x, float y, float z) {
        n_assert(trans);
        n_assert(trans[jnum]);
        n_assert((i>=0) && (i<num_keys));
        nJAKey *t = trans[jnum];
        t[i].x = x;
        t[i].y = y;
        t[i].z = z;
    };
    //---------------------------------------------------------------
    void SetRotKey(int i, int jnum, float x, float y, float z) {
        n_assert(rot);
        n_assert(rot[jnum]);
        n_assert((i>=0) && (i<num_keys));
        nJAKey *r = rot[jnum];
        r[i].x = x;
        r[i].y = y;
        r[i].z = z;
    };
    //---------------------------------------------------------------
    void GetTransKey(int i, int jnum, float& x, float& y, float& z) {
        n_assert(trans);
        n_assert(trans[jnum]);
        n_assert((i>=0) && (i<num_keys));
        nJAKey *t = trans[jnum];
        x = t[i].x;
        y = t[i].y;
        z = t[i].z;
    };
    //---------------------------------------------------------------
    void GetRotKey(int i, int jnum, float& x, float& y, float& z) {
        n_assert(rot);
        n_assert(rot[jnum]);
        n_assert((i>=0) && (i<num_keys));
        nJAKey *r = rot[jnum];
        x = r[i].x;
        y = r[i].y;
        z = r[i].z;
    };
    //---------------------------------------------------------------
    bool InitPointers(nJAJoint *joints, nJAKey *& cur_key) {
        n_assert(!trans);
        n_assert(!rot);

        // allokiere Pointer-Arrays...
        int trans_size = num_joints * sizeof(nJAKey *);
        int rot_size   = num_joints * sizeof(nJAKey *);
        trans = (nJAKey **) n_malloc(trans_size);
        rot   = (nJAKey **) n_malloc(rot_size);
        if ((!trans) || (!rot)) return false;

        // Initialisiere Pointer-Arrays, Pointer sind nur gueltig, wenn
        // dieser Joint rotiert und/oder translatiert wird!
        memset(trans,0,trans_size);
        memset(rot,0,rot_size);
        int i;
        for (i=0; i<num_joints; i++) {
            nJAJoint *j = &(joints[i]);
            bool has_r = j->HasRot();
            bool has_t = j->HasTrans();
            // Rotations-Keys
            if (has_r) {
                rot[i] = cur_key;
                cur_key += num_keys;
            }
            // Translations-Keys
            if (has_t) {
                trans[i] = cur_key;
                cur_key += num_keys;
            }
        }
        return true;
    };
    //---------------------------------------------------------------
    // Berechnet fuer jeden Joint die aktuelle Rotation/Translation
    // dieses States, multipliziert das Ergebnis mit eigener Wichtung
    // und addiert das Ergebnis auf die Werte in 't_res' und
    // 'r_res'.
    // 'key' ist absolute Keyframe-Nummer, die noch auf die Anzahl
    // Keyframes fuer diesen Frame normalisiert werden muss.
    // 'lerp' ist die Position zwischen den Keyframes.
    //---------------------------------------------------------------
    void Eval(int key, float l, nJAKey *t_res, nJAKey *r_res) {
        n_assert(num_keys > 1);
        key %= (num_keys-1);
        int j;
        for (j=0; j<this->num_joints; j++) {
            nJAKey *t = trans[j];
            if (t) {
                nJAKey *t0 = &(t[key]);
                nJAKey *t1 = &(t[key+1]);
                t_res[j].x += w * (t0->x + ((t1->x-t0->x)*l));
                t_res[j].y += w * (t0->y + ((t1->y-t0->y)*l));
                t_res[j].z += w * (t0->z + ((t1->z-t0->z)*l));
            }
        }
        for (j=0; j<this->num_joints; j++) {
            nJAKey *r = rot[j];
            if (r) {
                nJAKey *r0 = &(r[key]);
                nJAKey *r1 = &(r[key+1]);
                r_res[j].x += w * (r0->x + ((r1->x-r0->x)*l));
                r_res[j].y += w * (r0->y + ((r1->y-r0->y)*l));
                r_res[j].z += w * (r0->z + ((r1->z-r0->z)*l));
            }
        }
    };
};
//--------------------------------------------------------------------
class nChannelContext;
class N_DLLCLASS nJointAnim : public nAnimNode {
protected:
    enum nState {
        N_JA_INSIDE_NONE,
        N_JA_INSIDE_JOINTS,
        N_JA_INSIDE_STATES,
        N_JA_INSIDE_KEYS,
        N_JA_INSIDE_TRANSLATE,
        N_JA_INSIDE_ROTATE,
    };
    int num_joints;
    int num_states; 
    int cur_joint;              // fuer SetKey()
    int cur_state;              // fuer SetKey()
    double key_time;            // Laenge eines Keys in sec
    bool jointPointersDirty;    // true if LookupJoint() needs to be called on joint array elements

    nDynAutoRef<nRoot> refJointRoot;
    nJAJoint *joint_array;
    nJAState *state_array;
    nJAKey   *key_array;        // Platz fuer Keys
    nJAKey   *t_res;            // Translations-Ergebniss fuer jeden Joint
    nJAKey   *r_res;            // Rotations-Ergebnisse fuer jeden Joint
    nState   in_state;          // in welchen Begin()/End()?

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nJointAnim();
    virtual ~nJointAnim();
    virtual void Initialize(void);
    virtual bool SaveCmds(nPersistServer*);
    virtual void AttachChannels(nChannelSet*);
    virtual bool Attach(nSceneGraph2*);
    virtual void Compute(nSceneGraph2*); 

    /// set optional root path of joint skeleton
    void SetJointRoot(const char* name);
    /// get optional root path of joint skeleton
    const char* GetJointRoot();

    void BeginJoints(int num_joints);
    void SetJoint(int i, const char *name, const char *path, bool has_trans, bool has_rot);
    void EndJoints(void);

    void BeginStates(int num_states);
    void SetState(int i, const char *name, int num_keys);
    void EndStates(void);

    void BeginKeys(double key_time);
    void BeginRotate(const char *joint, const char *state);
    void BeginTranslate(const char *joint, const char *state);
    void SetKey(int i, float x, float y, float z);
    void EndRotate(void);
    void EndTranslate(void);
    void EndKeys(void);

    int  GetNumJoints(void);
    int  GetNumStates(void);
    void GetJoint(int, const char *&, const char *&, bool&, bool&);
    void GetState(int, const char *&, int&);
    void GetRotKey(int, const char *, const char *, float&, float&, float&);
    void GetTransKey(int, const char *, const char *, float&, float&, float&);

private:
    void allocArrays(void);
    int getJointByName(const char *);
    int getStateByName(const char *);
    void getCurJointState(const char *, const char *);
    void normWeights(void);
    void evalJoints(double);
    void updateJoints(void);
    void lookupJointPointers();
    void getChannels(nChannelContext* chnContext);
    void updateAll(nSceneGraph2* sceneGraph);
};
//--------------------------------------------------------------------
#endif
