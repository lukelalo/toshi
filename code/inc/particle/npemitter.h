#ifndef N_PEMITTER_H
#define N_PEMITTER_H
//--------------------------------------------------------------------
/**
    @class nPEmitter

    @brief super class for particle emitters

    Particle emitters and rendereres are decoupled to allow
    better combination. Any nPEmitter subclass can use any
    nPRender subclass for rendering.

    Emitters usually create a particle system context in
    the particle server in order to store particle attributes
    between frames.

    *** TODO ***
    Problem: particle emitters have only a chance to update themselves
    correctly when Attach() and/or Compute() is called. If the
    object owning the particle system is outside of the view frustum,
    the object is normally not rendered -> particles are not updated.
    THUS: The particle server needs to do updating, but how?  
  
    Really clean solution: see bug #433.
*/
//--------------------------------------------------------------------
#ifndef N_PARTICLESERVER_H
#include "misc/nparticleserver.h"
#endif

#ifndef N_ANIMNODE_H
#include "node/nanimnode.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nPEmitter
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class N_DLLCLASS nPEmitter : public nAnimNode {
public:
    nPEmitter();
    virtual ~nPEmitter();
    virtual void Initialize(void);
    virtual bool SaveCmds(nPersistServer *fs);

    void  SetTimes(float start, float stop, float repeat);
    void  GetTimes(float& start, float& stop, float& repeat) const;
    void  SetLifetime(float l);
    float GetLifetime(void) const;
    void  SetFreq(float f);
    float GetFreq(void) const;
    void  SetSpeed(float s);
    float GetSpeed(void) const;
    void  SetAccel(float ax, float ay, float az);
    void  GetAccel(float& ax, float& ay, float& az) const;
    void  SetInnerCone(float f);
    float GetInnerCone(void) const;
    void  SetOuterCone(float f);
    float GetOuterCone(void) const;

    virtual bool BeginPullParticles(int renderContext, float tstamp);
    virtual int  PullParticles(nPCorn *&);
    virtual void EndPullParticles(void);

    static nKernelServer *kernelServer;

protected:
    int get_max_particles(void);

    float t_start;
    float t_stop;
    float t_repeat;
    float lifetime;
    float freq;
    float delta_t;
    float speed;
    float inner_cone;
    float outer_cone;
    vector3 accel;

    float icone0;           // radiants of top right inner cone limit
    float icone1;           // radiants of top left inner cone limit
    float ocone0;           // radiants of bottom left outer cone
    float ocone1;           // radiants of bottom right outer cone

    bool lifetime_dirty;    // may caus reallocation of particle system
    bool freq_dirty;        // may caus reallocation of particle system

};
//--------------------------------------------------------------------
inline 
nPEmitter::nPEmitter()
        : t_start(0.0f),
          t_stop(0.0f),
          t_repeat(0.0f),
          lifetime(1.0f),
          freq(1.0f),
          delta_t(1.0f),
          speed(1.0f),
          inner_cone(0.0f),
          outer_cone(0.0f),
          icone0(n_deg2rad(0.0f)),
          icone1(n_deg2rad(0.0f)),
          ocone0(n_deg2rad(180.0f)),
          ocone1(n_deg2rad(180.0f)),
          lifetime_dirty(false),
          freq_dirty(true)
{ };
//--------------------------------------------------------------------
inline 
void 
nPEmitter::SetTimes(float start, float stop, float repeat)
{
    n_assert(stop >= start);
    n_assert(repeat >= stop);
    this->t_start  = start;
    this->t_stop   = stop;
    this->t_repeat = repeat;
};
//--------------------------------------------------------------------
inline
void 
nPEmitter::GetTimes(float& start, float& stop, float& repeat) const
{
    start  = this->t_start;
    stop   = this->t_stop;
    repeat = this->t_repeat; 
};
//--------------------------------------------------------------------
inline
void 
nPEmitter::SetLifetime(float l) 
{
    n_assert(l > 0.0f);
    if (l != this->lifetime) this->lifetime_dirty = true;
    this->lifetime = l;
};
//--------------------------------------------------------------------
inline
float 
nPEmitter::GetLifetime(void) const
{
    return this->lifetime;
};
//--------------------------------------------------------------------
inline
void 
nPEmitter::SetFreq(float f) 
{
    n_assert(freq > 0.0f);
    if (f != this->freq) this->freq_dirty = true;
    this->freq = f;
    this->delta_t = 1.0f / f;
};
//--------------------------------------------------------------------
inline
float 
nPEmitter::GetFreq(void) const
{
    return this->freq;
};
//--------------------------------------------------------------------
inline
void 
nPEmitter::SetSpeed(float s) 
{
    this->speed = s;
};
//--------------------------------------------------------------------
inline
float 
nPEmitter::GetSpeed(void) const
{
    return this->speed;
};
//--------------------------------------------------------------------
inline
void 
nPEmitter::SetAccel(float ax, float ay, float az) 
{
    this->accel.set(ax,ay,az);
};
//--------------------------------------------------------------------
inline
void 
nPEmitter::GetAccel(float& ax, float& ay, float& az) const
{
    ax = this->accel.x;
    ay = this->accel.y;
    az = this->accel.z;
};
//--------------------------------------------------------------------
inline
void 
nPEmitter::SetInnerCone(float f) 
{
    n_assert((f >= 0.0f) && (f <= 180.0f));
    this->inner_cone = f;
    this->icone0 = n_deg2rad(0.0f+f);
    this->icone1 = n_deg2rad(0.0f-f);
};
//--------------------------------------------------------------------
inline
float 
nPEmitter::GetInnerCone(void) const
{
    return this->inner_cone;
};
//--------------------------------------------------------------------
inline
void 
nPEmitter::SetOuterCone(float f) 
{
    n_assert((f >= 0.0f) && (f <= 180.0f));
    this->outer_cone = f;
    this->ocone0 = n_deg2rad(180.0f-f);
    this->ocone1 = n_deg2rad(-180.0f+f);
};
//--------------------------------------------------------------------
inline
float 
nPEmitter::GetOuterCone(void) const
{
    return this->outer_cone;
};
//--------------------------------------------------------------------
#endif
