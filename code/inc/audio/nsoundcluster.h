#ifndef N_SOUNDCLUSTER_H
#define N_SOUNDCLUSTER_H
//------------------------------------------------------------------------------
/**
    @class nSoundCluster
    @ingroup NebulaAudioModule
    @brief A cluster of nSound2 objects which encapsulates voice management.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_SOUND2_H
#include "audio/nsound2.h"
#endif

#undef N_DEFINES
#define N_DEFINES nSoundCluster
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nListener2;
class N_PUBLIC nSoundCluster
{
public:
    /// format enums
    enum
    {
        STEREO,
        THREEDEE,
    };

    /// constructor
    nSoundCluster(int numPlaybackChannels);
    /// destructor
    ~nSoundCluster();
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

protected:
    /// priority-insert a play candidate into the internal play candidates array
    nSound2* AddPlayCandidate(const nSound2& s);
    /// get the highest priority play candidate
    nSound2* GetHighPriPlayCandidate();

    int maxPlayCandidates;
    int numPlayCandidates;
    nSound2* minPriPlayCandidate;
    nSound2* playCandidates;
    bool inBegin;
};
//------------------------------------------------------------------------------
#endif

