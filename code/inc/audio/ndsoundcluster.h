#ifndef N_DSOUNDCLUSTER_H
#define N_DSOUNDCLUSTER_H
//------------------------------------------------------------------------------
/**
    @class nDSoundCluster
    @ingroup NebulaAudioModule    
    @brief DirectSound based specialization of nSoundCluster.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_SOUNDCLUSTER_H
#include "audio/nsoundcluster.h"
#endif

#ifndef N_DSOUNDSERVER2_H
#include "audio/ndsoundserver2.h"
#endif

#undef N_DEFINES
#define N_DEFINES nDSoundCluster
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nDSoundBuffer2;
class nDSoundCluster : public nSoundCluster
{
public:
    /// constructor
    nDSoundCluster(int numPlaybackChannels, int numSampleChannels, IDirectSound8* dSound, IDirectSoundBuffer* dsPrimary);
    /// destructor
    ~nDSoundCluster();
    /// configure the sound buffers
    void Configure(int numSampleChannels, IDirectSound8* dSound, IDirectSoundBuffer* dsPrimary);
    /// begin a frame
    void Begin();
    /// start a sound in this sound cluster
    void StartSound(const nSound2& s);
    /// update a sound in this sound cluster
    void UpdateSound(const nSound2& s);
    /// stop a sound in this sound cluster
    void StopSound(const nSound2& s);
    /// end a frame
    void End(const nListener2& l);
    /// flush all sounds
    void Flush();

protected:
    /// get the lowest priority nDSoundBuffer
    nDSoundBuffer2* GetLowPriSoundBuffer();

    int numBuffers;
    nDSoundBuffer2* soundBuffers;
};
//------------------------------------------------------------------------------
#endif
