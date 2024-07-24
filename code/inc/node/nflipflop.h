#ifndef N_FLIPFLOP_H
#define N_FLIPFLOP_H
//-------------------------------------------------------------------
/**
    @class nFlipFlop
    @ingroup NebulaVisnodeModule
    
    @brief Selects one of it's child nodes based on a keyframed channel.

    The nFlipFlop class selects one of its child nodes based on keyframes
    on a channel. This can be used to switch nodes at runtime, for 
    example to flip through a series of texture arrays, or to select a
    particular mesh based on a 'damage' channel. For each node you wish
    to flip to add a keyframe with the channel value to select that node.
    The node will remain active until the next keyframe.
*/
//-------------------------------------------------------------------
#ifndef N_ANIMNODE_H
#include "node/nanimnode.h"
#endif

//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nFlipFlop
#include "kernel/ndefdllclass.h"
//-------------------------------------------------------------------
class N_DLLCLASS nFlipFlop : public nAnimNode {
protected:
    int keyarray_size;
    int num_keys;
    nObjectKey *keyarray;

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nFlipFlop();
    virtual ~nFlipFlop();

    // inherited
    virtual bool SaveCmds(nPersistServer *);
    virtual bool Attach(nSceneGraph2 *);
    virtual bool AddKey(float time, const char *name);

private:
    nObjectKey *getKey(void);

};
//-------------------------------------------------------------------
#endif

