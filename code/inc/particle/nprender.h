#ifndef N_PRENDER_H
#define N_PRENDER_H
//--------------------------------------------------------------------
/**
    @class nPRender

    @brief superclass for particle system renderers 

    Renders particles emitted by a nPEmitter object.  
*/
//--------------------------------------------------------------------
#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_ANIMNODE_H
#include "node/nanimnode.h"
#endif

#ifndef NMATH_MATH_H
#include "math/nmath.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nPRender
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
//  nPAttrs
//  Particle attributes that can be animated over the particles
//  lifetime.
//--------------------------------------------------------------------
struct nPAttrs {
    enum {
        N_SIZE   = 0,
        N_BOUNCE = 1,
        N_RED    = 2,
        N_GREEN  = 3,
        N_BLUE   = 4,
        N_ALPHA  = 5,

        N_NUM    = 6,
    };
    float f[N_NUM];
    ulong rgba;         // color in rgba format
    ulong bgra;         // color in bgra format
};

//--------------------------------------------------------------------
class nPEmitter;
class N_DLLCLASS nPRender : public nAnimNode {
protected:
    int   i_num_keys;
    float f_num_keys;
    int   bounce_key;
    nPAttrs *keys;

    float spin;
    float spin_accel;
    bool stretch;

    nRef<nPEmitter> ref_emitter;

public:
    static nKernelServer *kernelServer;

    nPRender()
         : ref_emitter(this),
           i_num_keys(0),
           f_num_keys(0.0f),
           bounce_key(0),
           keys(NULL),
           spin(0.0f),
           spin_accel(0.0f),
           stretch(false)
    {
        this->SetFlags(N_FLAG_SAVEUPSIDEDOWN);
    };
    virtual ~nPRender();
    virtual bool SaveCmds(nPersistServer *);

    void  SetSpin(float f) {
        this->spin = n_deg2rad(f);
    };
    float GetSpin(void) {
        return n_rad2deg(this->spin);
    };
    void  SetSpinAccel(float f) {
        this->spin_accel = n_deg2rad(f);
    };
    float GetSpinAccel(void) {
        return n_rad2deg(this->spin_accel);
    };
    void SetStretch(bool b) {
        this->stretch = b;
    };
    bool GetStretch(void) {
        return this->stretch;
    };

    virtual void BeginKeys(int num);
    virtual void SetKey(int i, float size, float bounce, float r, float g, float b, float a);
    virtual void EndKeys(void);
    virtual int  GetNumKeys(void);
    virtual void GetKey(int i, float& size, float& bounce, float& r, float& g, float& b, float& a);

    virtual void SetEmitter(const char *name);
    virtual char *GetEmitter(char *buf, int size);
protected:
    void getParticleAttrs(float rel_age, nPAttrs& pa);
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nPRender::getParticleAttrs(float rel_age, nPAttrs& pa)
{
    float findex = rel_age * (this->f_num_keys-1.0f);
    int i0 = n_ftol(findex);
    int i1 = i0 + 1;
    float between = findex - float(i0);
    if (i0 < 0) i0=0;
    else if (i0 >= this->i_num_keys) i0=this->i_num_keys-1;
    if (i1 < 0) i1=0;
    else if (i1 >= this->i_num_keys) i1=this->i_num_keys-1;
    nPAttrs *k0 = &(this->keys[i0]); 
    nPAttrs *k1 = &(this->keys[i1]); 
    int i;
    for (i=0; i<nPAttrs::N_NUM; i++) {
        pa.f[i] = k0->f[i]*(1.0f-between) + k1->f[i]*between;
    }

    // interpolate color values
    short uslerp = (short) n_ftol(256.0f * between);
    ulong c0,c1,r0,g0,b0,a0,r1,g1,b1,a1;
    
    c0 = k0->rgba;
    c1 = k1->rgba;
    r0 = short(c0>>24 & 0xff);
    g0 = short(c0>>16 & 0xff);
    b0 = short(c0>>8  & 0xff);
    a0 = short(c0     & 0xff);
    r1 = short(c1>>24 & 0xff);
    g1 = short(c1>>16 & 0xff);
    b1 = short(c1>>8  & 0xff);
    a1 = short(c1     & 0xff);
    pa.rgba = (((r0 + (((r1-r0)*uslerp)>>8)) & 0xff)<<24) |
              (((g0 + (((g1-g0)*uslerp)>>8)) & 0xff)<<16) |
              (((b0 + (((b1-b0)*uslerp)>>8)) & 0xff)<<8)  |
              (((a0 + (((a1-a0)*uslerp)>>8)) & 0xff));

    c0 = k0->bgra;
    c1 = k1->bgra;
    r0 = short(c0>>24 & 0xff);
    g0 = short(c0>>16 & 0xff);
    b0 = short(c0>>8  & 0xff);
    a0 = short(c0     & 0xff);
    r1 = short(c1>>24 & 0xff);
    g1 = short(c1>>16 & 0xff);
    b1 = short(c1>>8  & 0xff);
    a1 = short(c1     & 0xff);
    pa.bgra = (((r0 + (((r1-r0)*uslerp)>>8)) & 0xff)<<24) |
              (((g0 + (((g1-g0)*uslerp)>>8)) & 0xff)<<16) |
              (((b0 + (((b1-b0)*uslerp)>>8)) & 0xff)<<8)  |
              (((a0 + (((a1-a0)*uslerp)>>8)) & 0xff));
}

//--------------------------------------------------------------------
#endif
