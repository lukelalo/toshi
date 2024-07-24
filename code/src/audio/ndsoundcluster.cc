#define N_IMPLEMENTS nDSoundCluster
//------------------------------------------------------------------------------
//  ndsoundcluster.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio/ndsoundcluster.h"
#include "audio/ndsoundbuffer2.h"

//------------------------------------------------------------------------------
/**
*/
nDSoundCluster::nDSoundCluster(int numPlaybackChannels,
                               int numSampleChannels,
                               IDirectSound8* dSound,
                               IDirectSoundBuffer* dsPrimary) 
:
    nSoundCluster(numPlaybackChannels),    
    numBuffers(numPlaybackChannels)
{ 
    this->soundBuffers = new nDSoundBuffer2[this->numBuffers];
    int i;
    for (i = 0; i < this->numBuffers; i++)
    {
        this->soundBuffers[i].Initialize(numSampleChannels, dSound, dsPrimary);
    }
}

//------------------------------------------------------------------------------
/**
*/
nDSoundCluster::~nDSoundCluster()
{
    delete[] this->soundBuffers;
    this->soundBuffers = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundCluster::Begin()
{
    nSoundCluster::Begin();
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundCluster::StartSound(const nSound2& s)
{
    nSoundCluster::StartSound(s);
}

//------------------------------------------------------------------------------
/**
    An updated sound may normally only be added to the play candidates if it
    is currently playing (if it is not playing it may have been intercepted
    by another higher priority sound). Sound updates for non-playing sounds
    make no sense. Thus this function checks first if the sound is 
    actually playing. If yes it will just hand the method call up to the
    parent class which adds it to the play candidates. Exception: Looping
    sounds that are not playing will be restarted. This makes sure
    that the looping sound kicks in again if it had been intercepted by
    other higher pri sound sources which are either gone, or have gone
    down to lower priority.
*/
void
nDSoundCluster::UpdateSound(const nSound2& s)
{
    n_assert(s.GetId() != 0);

    // if the sound is currently playing then everything is ok
    // and we can hand the method call to the parent class
    int i;
    for (i = 0; i < this->numBuffers; i++)
    {
        nDSoundBuffer2& curBuffer = this->soundBuffers[i];
        if (curBuffer.GetSoundObject() == s)
        {
            nSoundCluster::UpdateSound(s);
            return;
        }
    }

    // otherwise check if it is a looping sound, if yes, just restart
    if (s.GetLooping())
    {
        this->StartSound(s);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nDSoundCluster::StopSound(const nSound2& s)
{
    n_assert(s.GetId() != 0);
    nSoundCluster::StopSound(s);

    // check if the sound is currently playing in one of the buffers,
    // if yes, detach it from the buffer
    int i;
    for (i = 0; i < this->numBuffers; i++)
    {
        nDSoundBuffer2& curBuffer = this->soundBuffers[i];
        if (curBuffer.GetSoundObject() == s)
        {
            curBuffer.DetachSoundObject();
        }
    }    
}

//------------------------------------------------------------------------------
/**
    Flush (stop) all sounds currently attached to the sound buffer.
*/
void
nDSoundCluster::Flush()
{
    int i;
    for (i = 0; i < this->numBuffers; i++)
    {
        nDSoundBuffer2& curBuffer = this->soundBuffers[i];
        curBuffer.DetachSoundObject();
    }
}

//------------------------------------------------------------------------------
/**
    Get the lowest priority playing sound buffer.
*/
nDSoundBuffer2*
nDSoundCluster::GetLowPriSoundBuffer()
{
    nDSoundBuffer2* lowPri = 0;
    int i;
    for (i = 0; i < this->numBuffers; i++)
    {
        nDSoundBuffer2& cur = this->soundBuffers[i];
        if (cur.GetSoundObject().GetState() != nSound2::FREE)
        {
            if (lowPri)
            {
                if (lowPri->GetSoundObject().GetPriority() > cur.GetSoundObject().GetPriority())
                {
                    lowPri = &cur;
                }
            }
            else
            {
                lowPri = &cur;
            }
        }
    }
    return lowPri;
}

//------------------------------------------------------------------------------
/**
    Finish the local sound scene. The playCandidates array
    now contains the N highest priority play candidates for this frame. The
    first thing todo is to free any sound buffers which have stopped playing
    their oneshot sample. The next step is to distribute as many play candidates
    as possible to the sound buffers. This happens by the following rules:

    - A play candidate that is already playing continues to play on that buffer
      those play candidates are marked as "mapped"
    - next, all free sound buffers are filled with the highest priority 
      unmapped play candidates. Those play candidates are marked as "mapped".
    - If there are play candidates remaining they will overwrite the
      lowest priority sound buffers.

    This gives the following results:
    
    - High priority sound buffer which have NO play candidate in this frame
      continue playing (these are called orphaned sounds), orphaned sounds
      will eventually terminate themselves if they are not intercepted
      by new higher priority play candidates.
    - Play candidates that are already playing continue to play on the
      same buffer.
    - Always the highest priority sounds that should play will play.
*/
void
nDSoundCluster::End(const nListener2& listener)
{
    // step 1: check for oneshot sounds that are finished and
    // detach them from their sound buffer, also check for
    // loop sounds that have NO corresponding play candidate
    // and detach them
    int i;
    for (i = 0; i < this->numBuffers; i++)
    {
        nDSoundBuffer2& curBuffer = this->soundBuffers[i];
        if (curBuffer.CheckStopped())
        {
            // if there is a playing candidate for this sound, remove
            // the play candidate 
            int candIndex;
            for (candIndex = 0; candIndex < this->numPlayCandidates; candIndex++)
            {
                nSound2& curCand = this->playCandidates[candIndex];
                if (curCand == curBuffer.GetSoundObject())
                {
                    curCand.Clear();
                }
            }
            curBuffer.DetachSoundObject();
        } 
        else if (curBuffer.GetSoundObject().GetLooping())
        {
            int candIndex;
            bool playCandidateExists = false;
            for (candIndex = 0; candIndex < this->numPlayCandidates; candIndex++)
            {
                nSound2& curCand = this->playCandidates[candIndex];
                if (curCand == curBuffer.GetSoundObject())
                {
                    playCandidateExists = true;
                    break;
                }
            }
            if (!playCandidateExists)
            {
                curBuffer.DetachSoundObject();                
            }
        }
    }

    // step 2: find all play candidates that are already playing, update
    // their corresponding sound buffer attributes, and clear that
    // play candidate (because it has been handled)
    int candIndex;
    for (candIndex = 0; candIndex < this->numPlayCandidates; candIndex++)
    {
        nSound2& curCandidate = this->playCandidates[candIndex];
        if (nSound2::FREE != curCandidate.GetState())
        {
            // check if a sound buffer plays this play candidate
            int bufIndex;
            for (bufIndex = 0; bufIndex < this->numBuffers; bufIndex++)
            {
                nDSoundBuffer2& curBuffer = this->soundBuffers[bufIndex];
                if (curBuffer.GetSoundObject() == curCandidate)
                {
                    if (curCandidate.GetState() == nSound2::STARTED)
                    {
                        // restart the sound buffer
                        curBuffer.AttachSoundObject(curCandidate);
                    }
                    else
                    {
                        curBuffer.UpdateSoundObject(curCandidate);
                    }
                    curCandidate.Clear();
                    break;
                }
            }
        }
    }

    // step 3: fill the free sound buffers with the remaining highest priority
    // play candidates
    int bufIndex;
    for (bufIndex = 0; bufIndex < this->numBuffers; bufIndex++)
    {
        nDSoundBuffer2& curBuffer = this->soundBuffers[bufIndex];
        if (curBuffer.GetSoundObject().GetState() == nSound2::FREE)
        {
            nSound2* highCandidate = this->GetHighPriPlayCandidate();
            if (highCandidate)
            {
                curBuffer.AttachSoundObject(*highCandidate);
                highCandidate->Clear();
            }
        }
    }

    // step 4: if there are play candidates remaining, clear
    // lower priority sound buffers and play the higher
    // priority sound buffer instead
    for (candIndex = 0; candIndex < this->numPlayCandidates; candIndex++)
    {
        nSound2& curCandidate = this->playCandidates[candIndex];
        if (nSound2::FREE != curCandidate.GetState())
        {
            // check a sound buffer plays this play candidate
            nDSoundBuffer2* lowBuffer = this->GetLowPriSoundBuffer();
            if (lowBuffer && (lowBuffer->GetSoundObject().GetPriority() < curCandidate.GetPriority()))
            {
                lowBuffer->AttachSoundObject(curCandidate);
            }
            curCandidate.Clear();
        }
    }

    // trigger the sound buffers
    for (i = 0; i < this->numBuffers; i++)
    {
        this->soundBuffers[i].Trigger(listener);
    }

    // hand control to parent class
    nSoundCluster::End(listener);
}