#ifndef N_LINKNODE_H
#define N_LINKNODE_H
//--------------------------------------------------------------------
/**
    @class nLinkNode
    @ingroup NebulaVisnodeModule
    
    @brief link to another nVisNode object 

    Dispatches the Attach() method to the object pointed to
    by SetTarget().

    A nLinkNode object can act as a placeholder in a
    visual hierarchy, which can then 'act' as any target
    nVisNode object. The target object can be switched
    at runtime and is not part of the nLinkNode's
    persistent state.

    See http://nebuladevice.sourceforge.net/cgi-bin/twiki/view/Nebula/LinkNode
    for some further discussion of the usage of nLinkNode.
*/
//--------------------------------------------------------------------
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nLinkNode
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nLinkNode : public nVisNode {
public:
    nLinkNode() 
        : ref_target(ks,this)
    { };
    virtual ~nLinkNode();
    virtual bool SaveCmds(nPersistServer *);
    virtual bool Attach(nSceneGraph2 *);
    virtual bool IsChannelSetDirty();
    virtual void AttachChannels(nChannelSet*);
    virtual void Preload();
    
    /// set target by path name (this is the usual way)
    void SetTarget(const char *s) 
    {
        n_assert(s);
        this->ref_target = s;

        // by adding a new target our channel set could potentially become dirty
        this->NotifyChannelSetDirty();
        if (this->ref_target.isvalid())
        {
            this->ref_target->NotifyChannelSetDirty();
        }
    };

    /// get target by name 
    const char *GetTarget(void) 
    {
        return this->ref_target.getname();
    };

    /// set direct pointer to target (GetName()/Loading/Saving won't work with this!)
    void SetTargetPtr(nVisNode* o)
    {
        this->ref_target = o;

        // by adding a new target our channel set could potentially become dirty
        this->NotifyChannelSetDirty();
        if (o)
        {
            this->ref_target->NotifyChannelSetDirty();
        }
    };

    /// get pointer to target
    nVisNode* GetTargetPtr()
    {
        return this->ref_target.get();
    };

    static nClass *local_cl;
    static nKernelServer *ks;

protected:
    nDynAutoRef<nVisNode> ref_target;
};
//--------------------------------------------------------------------
#endif
