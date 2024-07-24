#ifndef N_PSERVEMITTER_H
#define N_PSERVEMITTER_H
//--------------------------------------------------------------------
/**
    @class nPServEmitter

    @brief superclass for all particle emitter that use particle server
    particle systems, never used directly
*/
//--------------------------------------------------------------------
#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_PEMITTER_H
#include "particle/npemitter.h" 
#endif

#ifndef N_RINGBUFFER_H
#include "util/nringbuffer.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nPServEmitter
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nParticleServer;
class N_DLLCLASS nPServEmitter : public nPEmitter {
public:
    nPServEmitter();
    virtual ~nPServEmitter();
    
    virtual nPSystem *GetPSystem(int renderContext, float tstamp);
    virtual bool BeginPullParticles(int renderContext, float tstamp);
    virtual int  PullParticles(nPCorn *&);
    virtual void EndPullParticles(void);
    
    static nKernelServer *kernelServer;

protected:
    nAutoRef<nParticleServer> ref_ps;
    int key;        // particle server emitter key
   nRingBuffer<nPCorn> *pull_ringbuffer;
    enum nPullPass {
        PULLPASS_NONE,
        PULLPASS_BEGIN,
        PULLPASS_FIRST_CHUNK,
        PULLPASS_SECOND_CHUNK,
        PULLPASS_ONE_CHUNK,
        PULLPASS_DONE,
    };
    nPullPass pull_pass;

};
//--------------------------------------------------------------------
#endif
