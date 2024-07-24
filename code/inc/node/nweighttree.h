#ifndef N_WEIGHTTREE_H
#define N_WEIGHTTREE_H
//-------------------------------------------------------------------
/**
    @class nWeightTree
    @ingroup NebulaVisnodeModule

    @brief Binary-tree for control of relationships between weights.

    A nweight tree controls the relationships between a set of
    weight channels.  One can define relationships like "the increase of a 
    channel 'foo' results in the decrease of the channel 'foo2'".
    This definition is built up via a binary tree, whose 
    leafnodes represent one channel each, and nodes define 
    one dominant relationship each, until all relationships 
    in the root node are being solved.
*/
//-------------------------------------------------------------------
#ifndef N_STRLIST_H
#include "util/nstrlist.h"
#endif

#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nWeightTree
#include "kernel/ndefdllclass.h"

//-------------------------------------------------------------------
class nWeightTree;
class nWeightNode : public nStrNode 
{
    friend class nWeightTree;
    
    nWeightNode *parent;        // Ptr auf Parent-Node oder NULL
    nWeightNode *c[2];          // Childnodes: c[0] dominiert c[1]
    float val;
    int chn_num;

    nWeightNode(const char *n) : nStrNode(n) 
    {
        this->parent = NULL;
        this->c[0] = this->c[1] = 0;
        this->val  = 0.0;
        this->chn_num = -1;
    };    
};

//-------------------------------------------------------------------
class nChannelContext;
class nWeightTree : public nVisNode {
protected:
    nStrList wlist;         // Liste der WeightNodes

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nWeightTree();
    virtual ~nWeightTree();

    virtual bool SaveCmds(nPersistServer*);
    virtual bool Attach(nSceneGraph2*);
    virtual void Compute(nSceneGraph2*);
    virtual void AttachChannels(nChannelSet* chnSet);

    virtual bool AddLeaf(const char *);
    virtual bool AddNode(const char *, const char *, const char *);
private:
    void initNodeVals(nChannelContext*);
    void writeLeafVals(nChannelContext*);
    void evalNode(nWeightNode *);
};
//-------------------------------------------------------------------
#endif
