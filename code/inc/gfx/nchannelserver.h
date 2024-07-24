#ifndef N_CHANNELSERVER_H
#define N_CHANNELSERVER_H
//------------------------------------------------------------------------------
/**
    @class nChannelServer

    @brief manages the Nebula animation channels.

    OVERVIEW:
    
    Animation channels are a powerful way to route data from an 
    application to Nebula visual hierarchies in a very abstract way. 
    Application and nVisNode objects just have to agree on a
    channel name and a channel format, then the application can
    write data to the channel, which can then be read out by 
    nVisNode objects somewhere in the scene graph hierarchy.
    This decouples the application from the actual 3d objects,
    meaning more freedom for programmer and designer.

    The most usual case uses the global time channel "time"
    and nIpol objects (Nebula's universal interpolators). nIpol's
    read the "time" channel by default, which is globally available to
    all objects in the scene graph. nIpol uses the channel
    value to interpolate between its keys. Here's a simple
    example

    new n3dnode pos
        sel pos
        new nipol ip
            sel ip
            .connect ry
            .addkey1f 0 0
            .addkey1f 1 360
        sel ..
    sel ..

    Since the nipol object is by default connected to the time
    channel, this construct will rotate "pos" around the Y axis
    once a second.

    Now lets say the application writes out a 1-dimensional channel
    called "wind_direction", this channel could be 0 if the wind
    comes directly from north, 0.5 if coming from south and so on.
    The designer wants to implement a flag which should point away
    from the actual wind direction. That's only one change to the
    above example:

    new n3dnode pos
        sel pos
        new nipol ip
            sel ip
            .connect ry
            .setchannel wind_direction
            .addkey1f 0 0
            .addkey1f 1 360
        sel ..
    sel ..

    The interpolator now reads the wind_direction instead of time. That's
    all. The app just has to make sure to update the wind_direction channel
    each frame. It doesn't need to know who and where is actually
    using the wind_direction data.


    TECHNICAL:

    Nebula's channel system is actually made of 4 classes: nChannelServer,
    nChannelSet, nChannelContext and nChannel.

    nChannelServer is the global master object which lives under the
    name /sys/servers/channel.

    nChannelSet collects the channels which a nVisNode object want's
    to use. Channels are collected recursively through a whole nVisNode
    hierarchie, every child object adds the channels it wants to read
    or write so that the root object of the hierarchie has a "set"
    of all the channels which itself and its children are interested in.
    This nChannelSet is then used to create a new nChannelContext, usually
    one per hierarchie root. 

    nChannelContext object actually hold the data for a visual hierarchie.

    nChannel is the lowest level object of the channel system. Its just
    a data container for 1..4 dimensional float vectors, and in the future
    perhaps other data types.

    (C) 2001 A.Weissflog
*/

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_CHANNEL_H
#include "gfx/nchannel.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#undef N_DEFINES
#define N_DEFINES nChannelServer
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nChannelSet;
class nChannelContext;
class N_PUBLIC nChannelServer : public nRoot{

public:
    /// da constructor
    nChannelServer();
    /// da destructor
    virtual ~nChannelServer();

    /// declare a global channel and get its index
    int GenChannel(const char* chnName);
    
    /// set global channel to float value
    void SetChannel1f(int chnIndex, float f0);
    /// set global channel to 2d float vector
    void SetChannel2f(int chnIndex, float f0, float f1);
    /// set global channel to 3d float vector
    void SetChannel3f(int chnIndex, float f0, float f1, float f2);
    /// set global channel to 4d float vector
    void SetChannel4f(int chnIndex, float f0, float f1, float f2, float f3);
    /// set global channel as pointer
    void SetChannelPtr(int chnIndex, void* ptr);
    /// set global channel as string (does NOT copy string!)
    void SetChannelString(int chnIndex, const char* str);
    /// script helper function to set string, DO NOT USE!!!
    void SetChannelStringCopy(int chnIndex, const char* str);

    /// get global channel as float value
    float GetChannel1f(int chnIndex);
    /// get global channel as 2d float vector
    void GetChannel2f(int chnIndex, float& f0, float& f1);
    /// get global channel as 3d float vector
    void GetChannel3f(int chnIndex, float& f0, float& f1, float& f2);
    /// get global channel as 4d float vector
    void GetChannel4f(int chnIndex, float& f0, float& f1, float& f2, float& f3);
    /// get global channel as pointer
    void* GetChannelPtr(int chnIndex);
    /// get global channel as string
    const char* GetChannelString(int chnIndex);

    /// get channel object for global index
    const nChannel& GetChannel(int chnIndex);

    /// start a new "frame"
    void BeginScene();
    /// get a nChannelContext object using a nChannelSet
    nChannelContext* GetContext(nChannelSet* chnSet);
    /// end a "frame" (invalidates all channel contexts)
    void EndScene();

    /// allocate a chunk of local floats from the float pool
    nChannel* AllocChannels(int num);

    static nKernelServer* kernelServer;

private:
    enum {
        MAXPOOLSIZE = 4096,         // max number of channels in pool
        MAXCONTEXTS = 1024,         // corresponds to max number of objects in scene
        MAXCHANNELS = 512,          // max number of individual global channels
    };

    bool inBeginScene;
    nRef<nRoot> refChannelMap;      // directory of nEnv's which map channel names to channel indices

    /// next free index in contextPool
    int contextIndex;
    /// next free index in float and index pool
    int poolIndex;
    /// next free global channel index
    int presetIndex;

    /// pool of channel contexts
    nChannelContext *contextPool;   // array of MAXCONTEXTS channel objects
    /// the variable channel pool
    nChannel channelPool[MAXPOOLSIZE];
    /// the global channel presets
    nChannel channelPresets[MAXCHANNELS];
    /// string store
    nString channelStrings[MAXCHANNELS];
};
//------------------------------------------------------------------------------
#endif
