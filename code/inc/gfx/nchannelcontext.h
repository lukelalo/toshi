#ifndef N_CHANNELCONTEXT_H
#define N_CHANNELCONTEXT_H
//------------------------------------------------------------------------------
/**
    @class nChannelContext

    nChannelContext defines a local set of channels for (normally) a nVisNode
    hierarchy. There is one nChannelContext for each "render" of a nVisNode
    hierarchy. Since one hierarchy can be rendered multiple times in one frame,
    nChannelContext makes sure that each "render" has its own local
    copy of channels which don't interfere with the other "renders" of the
    same nVisNode hierarchy.

    nChannelContext objects are initialized by providing a nChannelSet
    object which defines a set of channels requested by a nVisNode
    hierarchy. During initialization, the nChannelContext makes a
    local copy for each of the global channels defined by the nChannelSet
    object. Read/write access to the local channels is provided by 
    nChannelContext methods. Those methods don't take the global
    channel index used in the channel server, instead they use the
    local channel indices as returned by nChannelSet::AddChannel().

    (C) 2001 A.Weissflog
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_CHANNELSET_H
#include "gfx/nchannelset.h"
#endif

//------------------------------------------------------------------------------
class nChannelContext 
{
public:
    /// constructor
    nChannelContext();
    /// destructor
    ~nChannelContext();

    /// initialize nChannelContext (incs refcount of chnSet)
    void Lock(nChannelServer* chnServer, nChannelSet* chnSet);
    /// release nChannelContext (decs refcount of chnSet)
    void Unlock();

    /// set local 1d-channel
    void SetChannel1f(int chnIndex, float f0);
    /// set local 2d-channel
    void SetChannel2f(int chnIndex, float f0, float f1);
    /// set local 3d-channel
    void SetChannel3f(int chnIndex, float f0, float f1, float f2);
    /// set local 4d-channel
    void SetChannel4f(int chnIndex, float f0, float f1, float f2, float f3);
    /// set local ptr channel
    void SetChannelPtr(int chnIndex, void* ptr);
    /// set local string channel
    void SetChannelString(int chnIndex, const char* str);

    /// get local 1d-channel
    float GetChannel1f(int chnIndex);
    /// get local 2d-channel
    void GetChannel2f(int chnIndex, float& f0, float& f1);
    /// get local 3d-channel
    void GetChannel3f(int chnIndex, float& f0, float& f1, float& f2);
    /// get local 4d-channel
    void GetChannel4f(int chnIndex, float& f0, float& f1, float& f2, float& f3);
    /// get local ptr channel
    void* GetChannelPtr(int chnIndex);
    /// get local string channel
    const char* GetChannelString(int chnIndex);

private:
    nChannelSet* channelSet;        // pointer to our ref counted channel set object
    nChannel*    channelPoolStart;  // pointer to start of local channel array
};

//------------------------------------------------------------------------------
/*
*/
inline
nChannelContext::nChannelContext() :
    channelSet(0),
    channelPoolStart(0)
{
    // empty
}

//------------------------------------------------------------------------------
/*
*/
inline
nChannelContext::~nChannelContext()
{
    // make sure we are unlocked
    n_assert(0 == this->channelSet);
}

//------------------------------------------------------------------------------
/*
    Initialize nChannelContext for use. Increments ref count of the
    provided nChannelSet object. The nChannelSet object must be initialized
    with the channels this context should offer. nChannelContext makes
    a local copy of the current state of those global channels.

    @param      chnServer   nChannelServer object which "owns" us
    @param      chnSet      the nChannelSet object providing channel useage info
*/
inline
void
nChannelContext::Lock(nChannelServer* chnServer, nChannelSet* chnSet)
{
    n_assert(chnServer);
    n_assert(chnSet);
    n_assert(0 == this->channelSet);

    // bump ref count of nChannelSet object and keep pointer to it
    this->channelSet = chnSet;
    chnSet->AddRef();

    // allocate our own local set of channels
    this->channelPoolStart = chnServer->AllocChannels(chnSet->GetNumChannels());

    // copy global preset channels to local pool channels
    int i;
    int numChannels = chnSet->GetNumChannels();
    for (i = 0; i < numChannels; i++)
    {
        int globalIndex = chnSet->GetChannelAt(i);
        this->channelPoolStart[i] = chnServer->GetChannel(globalIndex);
    }
}

//------------------------------------------------------------------------------
/*
    Unlock the nChannelContext, this will decrement the ref count
    of the contained nChannelSet object.
*/
inline
void
nChannelContext::Unlock()
{
    n_assert(this->channelSet);
    this->channelSet->RemRef();
    this->channelSet = 0;
}

//------------------------------------------------------------------------------
/*
    @param  chnIndex    global channel index
    @param  f0          value
*/
inline
void
nChannelContext::SetChannel1f(int chnIndex, float f0)
{
    n_assert(this->channelSet);
    n_assert(this->channelPoolStart);
    int localIndex = this->channelSet->Find(chnIndex);
    this->channelPoolStart[localIndex].Set1f(f0);
}

//------------------------------------------------------------------------------
/*
    @param  chnIndex    global channel index
    @param  f0          value 0
    @param  f1          value 1
*/
inline
void
nChannelContext::SetChannel2f(int chnIndex, float f0, float f1)
{
    n_assert(this->channelSet);
    n_assert(this->channelPoolStart);
    int localIndex = this->channelSet->Find(chnIndex);
    this->channelPoolStart[localIndex].Set2f(f0, f1);
}

//------------------------------------------------------------------------------
/*
    @param  chnIndex    global channel index
    @param  f0          value 0
    @param  f1          value 1
    @param  f2          value 2
*/
inline
void
nChannelContext::SetChannel3f(int chnIndex, float f0, float f1, float f2)
{
    n_assert(this->channelSet);
    n_assert(this->channelPoolStart);
    int localIndex = this->channelSet->Find(chnIndex);
    this->channelPoolStart[localIndex].Set3f(f0, f1, f2);
}

//------------------------------------------------------------------------------
/*
    @param  chnIndex    global channel index
    @param  f0          value 0
    @param  f1          value 1
    @param  f2          value 2
    @param  f3          value 3
*/
inline
void
nChannelContext::SetChannel4f(int chnIndex, float f0, float f1, float f2, float f3)
{
    n_assert(this->channelSet);
    n_assert(this->channelPoolStart);
    int localIndex = this->channelSet->Find(chnIndex);
    this->channelPoolStart[localIndex].Set4f(f0, f1, f2, f3);
}

//------------------------------------------------------------------------------
/*
    @param  chnIndex    global channel index
    @param  ptr         a void pointer
*/
inline
void
nChannelContext::SetChannelPtr(int chnIndex, void* ptr)
{
    n_assert(this->channelSet);
    n_assert(this->channelPoolStart);
    int localIndex = this->channelSet->Find(chnIndex);
    this->channelPoolStart[localIndex].SetPtr(ptr);
}

//------------------------------------------------------------------------------
/*
    @param  chnIndex    global channel index
    @param  str         pointer to static string
*/
inline
void
nChannelContext::SetChannelString(int chnIndex, const char* str)
{
    n_assert(this->channelSet);
    n_assert(this->channelPoolStart);
    int localIndex = this->channelSet->Find(chnIndex);
    this->channelPoolStart[localIndex].SetString(str);
}

//------------------------------------------------------------------------------
/*
    @param  chnIndex    global channel index
    @return             float value in channel
*/
inline
float
nChannelContext::GetChannel1f(int chnIndex)
{
    n_assert(this->channelSet);
    n_assert(this->channelPoolStart);
    int localIndex = this->channelSet->Find(chnIndex);
    return this->channelPoolStart[localIndex].Get1f();
}

//------------------------------------------------------------------------------
/*
    @param  chnIndex    [in] global channel index
    @param  f0          [out] value 0
    @param  f1          [out] value 1
*/
inline
void
nChannelContext::GetChannel2f(int chnIndex, float& f0, float& f1)
{
    n_assert(this->channelSet);
    n_assert(this->channelPoolStart);
    int localIndex = this->channelSet->Find(chnIndex);
    this->channelPoolStart[localIndex].Get2f(f0, f1);
}

//------------------------------------------------------------------------------
/*
    @param  chnIndex    [in] global channel index
    @param  f0          [out] value 0
    @param  f1          [out] value 1
    @param  f2          [out] value 2
*/
inline
void
nChannelContext::GetChannel3f(int chnIndex, float& f0, float& f1, float& f2)
{
    n_assert(this->channelSet);
    n_assert(this->channelPoolStart);
    int localIndex = this->channelSet->Find(chnIndex);
    this->channelPoolStart[localIndex].Get3f(f0, f1, f2);
}

//------------------------------------------------------------------------------
/*
    @param  chnIndex    [in] global channel index
    @param  f0          [out] value 0
    @param  f1          [out] value 1
    @param  f2          [out] value 2
    @param  f3          [out] value 3
*/
inline
void
nChannelContext::GetChannel4f(int chnIndex, float& f0, float& f1, float& f2, float& f3)
{
    n_assert(this->channelSet);
    n_assert(this->channelPoolStart);
    int localIndex = this->channelSet->Find(chnIndex);
    this->channelPoolStart[localIndex].Get4f(f0, f1, f2, f3);
}

//------------------------------------------------------------------------------
/*
    @param  chnIndex    global channel index
    @return             void pointer in channel
*/
inline
void*
nChannelContext::GetChannelPtr(int chnIndex)
{
    n_assert(this->channelSet);
    n_assert(this->channelPoolStart);
    int localIndex = this->channelSet->Find(chnIndex);
    return this->channelPoolStart[localIndex].GetPtr();
}

//------------------------------------------------------------------------------
/*
    @param  chnIndex    global channel index
    @return             string pointer in channel
*/
inline
const char*
nChannelContext::GetChannelString(int chnIndex)
{
    n_assert(this->channelSet);
    n_assert(this->channelPoolStart);
    int localIndex = this->channelSet->Find(chnIndex);
    return this->channelPoolStart[localIndex].GetString();
}

//------------------------------------------------------------------------------
#endif
