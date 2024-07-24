#ifndef N_POINTEMITTER_H
#define N_POINTEMITTER_H
//--------------------------------------------------------------------
/**
    @class nPointEmitter

    @brief emit particles from point
*/
//--------------------------------------------------------------------

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_PSERVEMITTER_H
#include "particle/npservemitter.h" 
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nPointEmitter
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nGfxServer;
class nMathServer;
class N_DLLCLASS nPointEmitter : public nPServEmitter {
    nAutoRef<nGfxServer>      ref_gs;
    nAutoRef<nMathServer>     ref_ms;
    int rand_key;   // random number key

public:
    static nKernelServer *kernelServer;

    nPointEmitter();
    virtual ~nPointEmitter();
    virtual void Compute(nSceneGraph2* sceneGraph);
};
//--------------------------------------------------------------------
#endif
