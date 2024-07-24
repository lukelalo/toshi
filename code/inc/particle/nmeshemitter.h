#ifndef N_MESHEMITTER_H
#define N_MESHEMITTER_H
//--------------------------------------------------------------------
/**
    @class nMeshEmitter

    @brief emit particles from mesh vertices
*/
//--------------------------------------------------------------------
#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#ifndef N_PSERVEMITTER_H
#include "particle/npservemitter.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nMeshEmitter
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nGfxServer;
class nMathServer;
class N_DLLCLASS nMeshEmitter : public nPServEmitter {
    nDynAutoRef<nVisNode> ref_mesh;
    nAutoRef<nGfxServer>  ref_gs;
    nAutoRef<nMathServer> ref_ms;
    int rand_key;       // random number key
    int vertex_key;     // current vertex index in attached vbuffer

public:
    static nKernelServer *kernelServer;

    nMeshEmitter()
        : ref_mesh(kernelServer,this),
          ref_gs(kernelServer,this),
          ref_ms(kernelServer,this),
          rand_key(0),
          vertex_key(0)
    {
        this->ref_gs = "/sys/servers/gfx";
        this->ref_ms = "/sys/servers/math";
    };
    virtual ~nMeshEmitter();
    virtual void Compute(nSceneGraph2* sceneGraph);
    virtual bool SaveCmds(nPersistServer *);

    void SetMeshNode(const char *p) {
        n_assert(p);
        this->ref_mesh = p;
    };
    const char *GetMeshNode(void) {
        return this->ref_mesh.getname();
    };
};
//--------------------------------------------------------------------
#endif
