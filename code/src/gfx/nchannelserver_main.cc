#define N_IMPLEMENTS nChannelServer
//------------------------------------------------------------------------------
//  nchannelserver_main.cc
//  (C) 2001 Andre Weissflog
//------------------------------------------------------------------------------
#include "gfx/nchannelserver.h"
#include "gfx/nchannelcontext.h"
#include "kernel/nenv.h"

nNebulaScriptClass(nChannelServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nChannelServer::nChannelServer() :
    inBeginScene(false),
    refChannelMap(this),
    contextIndex(0),
    poolIndex(0),
    presetIndex(0)
{
    this->refChannelMap = kernelServer->New("nroot", "/sys/share/channels");
    this->contextPool = n_new nChannelContext[MAXCONTEXTS];
}

//------------------------------------------------------------------------------
/**
*/
nChannelServer::~nChannelServer()
{
    n_delete[] this->contextPool;
    this->contextPool = 0;
    if (this->refChannelMap.isvalid())
    {
        this->refChannelMap->Release();
    }
}

//------------------------------------------------------------------------------
/**
    Allocate a number of channels from the global channel pool. The channel
    pool will be reset at nChannelServer::BeginScene().

    @param  num     number of channels to allocate
    @return         channel pointer
*/
nChannel*
nChannelServer::AllocChannels(int num)
{
    n_assert((this->poolIndex + num) < MAXPOOLSIZE);
    nChannel* chnPtr = &(this->channelPool[poolIndex]);
    this->poolIndex += num;
    return chnPtr;
}

//------------------------------------------------------------------------------
/**
    Look up a global channel index for a channel name. Create channel if
    not exists yet. Throws an assertion if there's a width mismatch for
    an already created channel

    @param  chnName     name of channel to get index for
    @return             the channel index (used in subsequent calls to nChannelServer)
*/
int
nChannelServer::GenChannel(const char* chnName)
{
    n_assert(chnName);

    // channel exists?
    nEnv* env = (nEnv*) this->refChannelMap->Find(chnName);
    if (env)
    {
        return env->GetI();
    }

    // doesn't exist, create and initialize a new one
    kernelServer->PushCwd(this->refChannelMap.get());
    env = (nEnv*) kernelServer->New("nenv", chnName);
    kernelServer->PopCwd();
    
    n_assert(env);
    n_assert(this->presetIndex < MAXCHANNELS);
    env->SetI(this->presetIndex);
    this->channelPresets[this->presetIndex].Clear();
    return this->presetIndex++;
}

//------------------------------------------------------------------------------
/**
    Get a reference to global nChannel object.

    @param  chnIndex    global index of channel
    @return             a const ref to the corresponding nChannel object
*/
const nChannel&
nChannelServer::GetChannel(int chnIndex)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    return channelPresets[chnIndex];
}

//------------------------------------------------------------------------------
/**
    Set a global channel to a float value.

    @param  chnIndex    global channel index
    @param  f0          the float value
*/
void
nChannelServer::SetChannel1f(int chnIndex, float f0)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    this->channelPresets[chnIndex].Set1f(f0);
}

//------------------------------------------------------------------------------
/**
    Set a global channel to a 2D float vector.

    @param  chnIndex    global channel index
    @param  f0          1st value
    @param  f1          2nd value
*/
void
nChannelServer::SetChannel2f(int chnIndex, float f0, float f1)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    this->channelPresets[chnIndex].Set2f(f0, f1);
}

//------------------------------------------------------------------------------
/**
    Set a global channel to a 3D float vector.

    @param  chnIndex    global channel index
    @param  f0          1st value
    @param  f1          2nd value
    @param  f2          3rd value
*/
void
nChannelServer::SetChannel3f(int chnIndex, float f0, float f1, float f2)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    this->channelPresets[chnIndex].Set3f(f0, f1, f2);
}

//------------------------------------------------------------------------------
/**
    Set a global channel to a 4D float vector.

    @param  chnIndex    global channel index
    @param  f0          1st value
    @param  f1          2nd value
    @param  f2          3rd value
    @param  f3          4th value
*/
void
nChannelServer::SetChannel4f(int chnIndex, float f0, float f1, float f2, float f3)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    this->channelPresets[chnIndex].Set4f(f0, f1, f2, f3);
}

//------------------------------------------------------------------------------
/**
    Set a global channel to a pointer value.

    @param  chnIndex    global channel index
    @param  ptr         void pointer
*/
void
nChannelServer::SetChannelPtr(int chnIndex, void* ptr)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    this->channelPresets[chnIndex].SetPtr(ptr);
}

//------------------------------------------------------------------------------
/**
    Set a global channel to a pointer value.

    @param  chnIndex    global channel index
    @param  ptr         void pointer
*/
void
nChannelServer::SetChannelString(int chnIndex, const char* str)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    this->channelPresets[chnIndex].SetString(str);
}

//------------------------------------------------------------------------------
/**
    Set a global channel to a string from the script interface. This
    will make an internal copy of the string. This method cannot be used
    to initialize the channel content for several render contexts, as
    strings for identical channel names will overwrite each other.
    Use the SetChannelString() method with this, and use static
    strings.

    @param  chnIndex    global channel index
    @param  str         pointer to a string (will be copied)
*/
void
nChannelServer::SetChannelStringCopy(int chnIndex, const char* str)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    const char* localString;
    if (str)
    {
        this->channelStrings[chnIndex] = str;
        localString = this->channelStrings[chnIndex].Get();
    }
    else
    {
        this->channelStrings[chnIndex].Set(0);
        localString = 0;
    }
    
    this->channelPresets[chnIndex].SetString(localString);
}


//------------------------------------------------------------------------------
/**
    Get contents of a channel as float value.

    @param  chnIndex    global channel index
    @return             float value
*/
float
nChannelServer::GetChannel1f(int chnIndex)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    return this->channelPresets[chnIndex].Get1f();
}

//------------------------------------------------------------------------------
/**
    Get contents of channel as 2 float values.

    @param  chnIndex    [in] global channel index
    @param  f0          [out] 1st value
    @param  f1          [out] 2nd value
*/
void
nChannelServer::GetChannel2f(int chnIndex, float& f0, float& f1)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    this->channelPresets[chnIndex].Get2f(f0, f1);
}

//------------------------------------------------------------------------------
/**
    Get contents of channel as 3 float values.

    @param  chnIndex    [in] global channel index
    @param  f0          [out] 1st value
    @param  f1          [out] 2nd value
    @param  f2          [out] 3rd value
*/
void
nChannelServer::GetChannel3f(int chnIndex, float& f0, float& f1, float& f2)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    this->channelPresets[chnIndex].Get3f(f0, f1, f2);
}

//------------------------------------------------------------------------------
/**
    Get contents of channel as 4 float values.

    @param  chnIndex    [in] global channel index
    @param  f0          [out] 1st value
    @param  f1          [out] 2nd value
    @param  f2          [out] 3rd value
    @param  f3          [out] 4th value
*/
void
nChannelServer::GetChannel4f(int chnIndex, float& f0, float& f1, float& f2, float& f3)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    this->channelPresets[chnIndex].Get4f(f0, f1, f2, f3);
}

//------------------------------------------------------------------------------
/**
    Get contents of channel as pointer

    @param  chnIndex    global channel index
    @param  ptr         void pointer
*/
void*
nChannelServer::GetChannelPtr(int chnIndex)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    return this->channelPresets[chnIndex].GetPtr();
}

//------------------------------------------------------------------------------
/**
    Get contents of channel as string.

    @param  chnIndex    global channel index
    @param  str         const char pointer
*/
const char*
nChannelServer::GetChannelString(int chnIndex)
{
    n_assert((chnIndex >= 0) && (chnIndex < this->presetIndex));
    return this->channelPresets[chnIndex].GetString();
}

//------------------------------------------------------------------------------
/**
    Prepares the channel server for a new frame. All channel contexts from the 
    previous frame are discarded.
*/
void
nChannelServer::BeginScene()
{
    n_assert(!this->inBeginScene);

    // reset the channel contexts
    this->poolIndex    = 0;
    this->contextIndex = 0;
    this->inBeginScene = true;
}

//------------------------------------------------------------------------------
/**
    Initialize a new channel context and return handle to it. The 
    channel context only contains those channels that are defined by
    the nChannelSet object. The current global values of those channels
    are copied into the channel context. This function may only
    be called inside BeginScene()/EndScene().

    @param  chnSet  nChannelSet object initialized with the required channels
    @return         ref to a new initialized channel context
*/
nChannelContext*
nChannelServer::GetContext(nChannelSet* chnSet)
{
    n_assert(chnSet);
    n_assert(this->inBeginScene);
    n_assert(this->contextIndex < MAXCONTEXTS);
    n_assert(this->contextPool);

    // get a new context and initialize it
    nChannelContext* ctx = &(this->contextPool[this->contextIndex++]);
    ctx->Lock(this, chnSet);

    return ctx;
}

//------------------------------------------------------------------------------
/**
    Finish a channel server frame.
*/
void
nChannelServer::EndScene()
{
    n_assert(this->inBeginScene);

    // unlock all contexts
    int i;
    for (i=0; i<this->contextIndex; i++)
    {
        this->contextPool[i].Unlock();
    }
    this->inBeginScene = false;
}

//------------------------------------------------------------------------------
