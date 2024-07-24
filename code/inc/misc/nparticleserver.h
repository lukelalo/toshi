#ifndef N_PARTICLESERVER_H
#define N_PARTICLESERVER_H
//--------------------------------------------------------------------
/**
    @class nParticleServer

    @brief particle system context server

    nVisNode objects may not store any state between frames,
    because one nVisNode object in a database may be rendered
    multiple times per frames with different channel sets.
    Animations are ideally independent of their previous states.
    Particle systems that are embedded into a 'living' world don't
    fit into this scheme. Since the world's environment may influence
    particles by external forces in an unforeseeable way, positions
    of particles must be stored between frames.

    The nParticleServer will manage those context blocks for particle
    emitter objects.
*/
//--------------------------------------------------------------------
#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_NODE_H
#include "util/nnode.h"
#endif

#ifndef N_KEYARRAY_H
#include "util/nkeyarray.h"
#endif

#ifndef N_RINGBUFFER_H
#include "util/nringbuffer.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif


//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nParticleServer
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nPCorn {
public:
    float   tstamp;     // timestamp of birth
    vector3 p;          // current position
    vector3 v;          // current velocity
    vector3 xvec;       // x component at time of birth
    
    nPCorn() {
        tstamp = 0.0f;
    };
    void Set(float timeStamp, 
             const vector3& pos, 
             const vector3& vel, 
             const matrix33& matrix) 
    {
        tstamp = timeStamp;
        p = pos;
        v = vel;
        xvec.set(matrix.M11, matrix.M12, matrix.M13);
    };
};

//--------------------------------------------------------------------
/**
    @class nPSystem

    Defines a particle system, particles are kept in a ring buffer.

    11-Apr-00   floh    knows about lifetime of particles now
                        to keep track of number of valid particles
*/
//--------------------------------------------------------------------
class nPSystem : public nNode {
    int key;                    // search key
    float cur_tstamp;           // the last timestamp this particle has been triggered with
    float last_emitted;         // timestamp of last emission in visual time frame
    vector3 accel;              // current global acceleration
   nRingBuffer<nPCorn> ring;
    bool triggered;
    matrix33 identity;

public:
    nPSystem(int _key, float _tstamp, int _num)
        : ring(_num)
    {
        this->key = _key;
        this->cur_tstamp   = _tstamp;
        this->last_emitted = _tstamp; 
        this->triggered = false;
    };

    ~nPSystem() { };
    
    //---------------------------------------------------------------
    //  SetAccel()
    //  Update the global acceleration vector for this
    //  particle system.
    //---------------------------------------------------------------
    void SetAccel(vector3& a)
    {
        this->accel = a;
    };

    //---------------------------------------------------------------
    int GetKey(void) {
        return this->key;
    };

    //---------------------------------------------------------------
    void SetTriggered(bool b) {
        this->triggered = b;
    };

    //---------------------------------------------------------------
    bool GetTriggered(void) {
        return this->triggered;
    };

    //---------------------------------------------------------------
    //  GetLastEmitted()
    //  Get the timestamp when the last particle has been emitted.
    //---------------------------------------------------------------
    float GetLastEmitted(void) {
        return this->last_emitted;
    };

    //---------------------------------------------------------------
    //  Update()
    //  Update particle positions based on frame time. 
    //  15-Oct-00   floh    + handles prev_p now
    //---------------------------------------------------------------
    void Update(float tstamp) {
        float ftime = tstamp - this->cur_tstamp;
        this->cur_tstamp = tstamp;
        float gax = accel.x * ftime;
        float gay = accel.y * ftime;
        float gaz = accel.z * ftime;
        nPCorn *p = ring.GetTail();
        if (p) do {
            p->v.x += gax;  
            p->v.y += gay;  
            p->v.z += gaz;
            p->p.x += p->v.x * ftime; 
            p->p.y += p->v.y * ftime;
            p->p.z += p->v.z * ftime;
        } while ((p = ring.GetPred(p)));
        this->triggered = true;
    };

    //---------------------------------------------------------------
    //  AddParticle()
    //  Add particle to ringbuffer, set particle orientation to
    //  identity (backward compatibilty).
    //---------------------------------------------------------------
    void AddParticle(float tstamp, const vector3& pos, const vector3& vel) 
    {
        nPCorn *p = ring.Add();
        p->Set(tstamp, pos, vel, identity);
        this->last_emitted = tstamp;
    };

    //---------------------------------------------------------------
    //  AddParticle()
    //  Add particle to ringbuffer, with orientation.
    //---------------------------------------------------------------
    void AddParticle(float tstamp, const vector3& pos, const vector3& vel, const matrix33 m) 
    {
        nPCorn *p = ring.Add();
        p->Set(tstamp, pos, vel, m);
        this->last_emitted = tstamp;
    };

    //---------------------------------------------------------------
    //  NextParticle()
    //  Return first particle (if p=NULL), or next particle
    //  (following p).
    //---------------------------------------------------------------
    nPCorn *NextParticle(nPCorn *p) {
        if (p) return ring.GetPred(p);
        else   return ring.GetTail();
    };

    //---------------------------------------------------------------
    //  GetRingBuffer()
    //  For direct Read-Only(!) access to the ring buffer...
    //---------------------------------------------------------------
   nRingBuffer<nPCorn> *GetRingBuffer(void) {
        return &(this->ring);
    };
};

//--------------------------------------------------------------------
/**
    @class nParticleContext

    A nParticleContext holds a unique collection of nPSystems
    objects. There should be one nParticleContext for each individual
    game object that may possibly have particle systems attached.
    nParticleContexts should be allocated on demand (only objects that really
    HAVE particle systems attached should allocate them to keep the
    number of contexts small (anytime a context needs to be looked
    up a binary search will take place).
*/
//--------------------------------------------------------------------
class nParticleContext : public nNode {
    int       key;
    nList     ps_list;
    nKeyArray<nPSystem *> ps_array;

public:
    //---------------------------------------------------------------
    nParticleContext()
    : ps_array(16,16)
    {
        key = 0;
    };
    //---------------------------------------------------------------
    nParticleContext(int _key)
    : ps_array(16,16)
    {
        key = _key;
    };
    //---------------------------------------------------------------
    ~nParticleContext() {
        nPSystem *ps;
        while ((ps = (nPSystem *) this->ps_list.RemHead())) delete ps;
    };
    //---------------------------------------------------------------
    void SetKey(int _key) {
        this->key = _key;
    };
    //---------------------------------------------------------------
    int GetKey(void) {
        return this->key;
    };
    //---------------------------------------------------------------
    nPSystem *AddPSystem(int _key, float _tstamp, int _numparticles)
    {
        nPSystem *ps = new nPSystem(_key,_tstamp,_numparticles);
        this->ps_list.AddTail(ps);
        this->ps_array.Add(_key,ps);
        return ps;
    };
    //---------------------------------------------------------------
    void RemPSystem(int _key) {
        nPSystem *ps = NULL;
        this->ps_array.Find(_key,ps);
        n_assert(ps);
        this->ps_array.Rem(_key);
        ps->Remove();
        delete ps;
    };
    //---------------------------------------------------------------
    nPSystem *GetPSystem(int _key) {
        nPSystem *ps = NULL;
        this->ps_array.Find(_key,ps);
        return ps;
    };
    //---------------------------------------------------------------
    int GetNumPSystems(void) {
       return this->ps_array.Size();
    };

    //---------------------------------------------------------------
    //  EvictOrphanPSystems()
    //  Evict all psystems which dont have the triggered flag
    //  set. 
    //---------------------------------------------------------------
    void EvictOrphanedPSystems(void) {
        nPSystem *ps;
        nPSystem *next_ps;
        for (ps = (nPSystem *) this->ps_list.GetHead();
             ps;
             ps = next_ps)
        {
            next_ps = (nPSystem *) ps->GetSucc();
            if (!ps->GetTriggered()) {
                ps->Remove();
                ps_array.Rem(ps->GetKey());
                delete ps;
            } else {
                ps->SetTriggered(false);
            }
        }
    };
};

//--------------------------------------------------------------------
class N_DLLCLASS nParticleServer : public nRoot {
    int psystem_key;
    nList ctx_list;                             // list of nParticleContext objects
    nKeyArray<nParticleContext *> *ctx_array;   // assoc array for fast find by key
    
public:
    static nKernelServer* kernelServer;
    
    nParticleServer();
    virtual ~nParticleServer();
    virtual void Trigger(void);

    virtual nPSystem *AddSystem(int ctx_key, int emit_key, float tstamp, int num_particles);
    virtual nPSystem *GetSystem(int ctx_key, int emit_key);
    virtual void RemSystem(int ctx_key, int emit_key);
    
    virtual void EvictContexts(void);
    virtual int GetUniqueSystemKey(void);           // return a unique psystem key

private:
    nParticleContext *new_context(int key);
    nParticleContext *get_context(int key);
};
//--------------------------------------------------------------------
#endif
