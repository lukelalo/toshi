#ifndef N_ANIMNODE_H
#define N_ANIMNODE_H
//-------------------------------------------------------------------
/**
    @class nAnimNode
    @ingroup NebulaVisnodeModule
    @brief nAnimNode is the superclass of nodes which control animation.

    The animnode class administers some attributes shared by all
    animating node classes, for the purposes of animating or
    manipulating other nodes.
*/
//-------------------------------------------------------------------
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif
//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nAnimNode
#include "kernel/ndefdllclass.h"
//-------------------------------------------------------------------
//  allgemeines Keyframe-Objekt mit Objekt-Pointer
//-------------------------------------------------------------------
class nObjectKey {
public:
    float t;
    nVisNode *o;
    nObjectKey() {
        t = 0.0f;
        o = NULL;
    };
    void Set(float _t, nVisNode *_o) {
        t = _t;
        o = _o;
    };
};
//-------------------------------------------------------------------
enum nRepType 
{
    N_REPTYPE_ONESHOT,
    N_REPTYPE_LOOP,
};
//-------------------------------------------------------------------
class N_PUBLIC nAnimNode : public nVisNode {
public:
    /// constructor
    nAnimNode();
    /// destructor
    virtual ~nAnimNode();
    /// persistency
    virtual bool SaveCmds(nPersistServer *);
    /// allocate required channels
    virtual void AttachChannels(nChannelSet*);
    /// set animation repetition type
    void SetRepType(nRepType rType);
    /// get animation repetition type
    nRepType GetRepType();
    /// set animation channel name (default == "time")
    void SetChannel(const char *chnName);
    /// get animation channel name
    const char *GetChannel();
    /// set time scale
    void SetScale(float tScale);
    /// get time scale
    float GetScale();

    static nClass *local_cl;
    static nKernelServer *ks;

protected:
    nRepType repType;
    float scale;
    int localChannelIndex;
    char channelName[16];
};

//------------------------------------------------------------------------------
/**
    Set the repetition type of the animation. This can be N_REPTYPE_ONESHOT
    or N_REPTYPE_LOOP. The default is N_REPTYPE_LOOP.

    @param  rType       the repetition type
*/
inline
void
nAnimNode::SetRepType(nRepType rType)
{
    this->repType = rType;
}

//------------------------------------------------------------------------------
/**
    Get the repetition type for the animation.

    @return             the repetition type
*/
inline
nRepType
nAnimNode::GetRepType()
{
    return this->repType;
}

//------------------------------------------------------------------------------
/**
    Set the animation channel, from which the animation should get its 
    "time info". This defaults to a channel named "time", which Nebula
    feeds with the current uptime of Nebula. The channel must be
    1-dimensional.

    @param  chnName     the channel name
*/
inline
void
nAnimNode::SetChannel(const char* chnName)
{
    n_assert(chnName);
    n_strncpy2(this->channelName, chnName, sizeof(this->channelName));
    this->NotifyChannelSetDirty();
}

//------------------------------------------------------------------------------
/**
    Return the current animation channel name.

    @return         the channel name
*/
inline
const char*
nAnimNode::GetChannel()
{
    return this->channelName;
}

//------------------------------------------------------------------------------
/**
    Set the time scale, can be used to accelerate or decelerate the
    animation.

    @param      tScale      the time scale, defaults to 1.0f
*/
inline
void
nAnimNode::SetScale(float tScale)
{
    n_assert(tScale != 0.0f);
    this->scale = tScale;
}

//------------------------------------------------------------------------------
/**
    Get the current time scale.
    
    @return     the current time scale
*/
inline
float
nAnimNode::GetScale()
{
    return this->scale;
}

//-------------------------------------------------------------------
#endif
