#ifndef N_THRESHNODE_H
#define N_THRESHNODE_H
//--------------------------------------------------------------------
/**
    @class nThreshNode
    @ingroup NebulaVisnodeModule
    
    @brief Activate/deactivate node within valid channel range.
*/
//--------------------------------------------------------------------
#ifndef N_ANIMNODE_H
#include "node/nanimnode.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nThreshNode
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class N_DLLCLASS nThreshNode : public nAnimNode {
    float lower_bound;
    float upper_bound;

public:
    static nKernelServer *kernelServer;

    nThreshNode()
        : lower_bound(0.0f),
          upper_bound(1.0f)
    { };
    virtual ~nThreshNode();
    virtual bool SaveCmds(nPersistServer *);
    virtual bool Attach(nSceneGraph2 *);

    void SetLowerBound(float f) {
        this->lower_bound = f;
    };
    float GetLowerBound(void) {
        return this->lower_bound;
    };
    void SetUpperBound(float f) {
        this->upper_bound = f;
    };
    float GetUpperBound(void) {
        return this->upper_bound;
    };
};
//--------------------------------------------------------------------
#endif
