#define N_IMPLEMENTS nSoundCluster
//------------------------------------------------------------------------------
//  nsoundcluster.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio/nsoundcluster.h"

//------------------------------------------------------------------------------
/**
*/
nSoundCluster::nSoundCluster(int numPlaybackChannels) :
    maxPlayCandidates(numPlaybackChannels),
    numPlayCandidates(0),
    minPriPlayCandidate(0),
    inBegin(false)
{
    playCandidates = new nSound2[this->maxPlayCandidates];
}

//------------------------------------------------------------------------------
/**
*/
nSoundCluster::~nSoundCluster()
{
    delete[] this->playCandidates;
    this->playCandidates = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nSoundCluster::Begin()
{
    n_assert(!this->inBegin);
    n_assert(this->playCandidates);

    this->inBegin = true;
    this->numPlayCandidates = 0;
    this->minPriPlayCandidate = 0;
}

//------------------------------------------------------------------------------
/**
    Add the sound to the play candidates if a free candidate slot or a
    lower priority candidate exists. If the sound could be added the method
    will return a pointer to the play candidate nSound2 object, otherwise
    0 will returned.
*/
nSound2*
nSoundCluster::AddPlayCandidate(const nSound2& s)
{
    n_assert(this->playCandidates);
    nSound2* newSound = 0;

    // first check if free play candidate slots are available
    if (this->numPlayCandidates < this->maxPlayCandidates)
    {
        this->playCandidates[this->numPlayCandidates] = s;
        newSound = &(this->playCandidates[this->numPlayCandidates]);
        this->numPlayCandidates++;
    }
    else
    {
        // otherwise check if the sound's priority is greater then
        // the current minimum priority, if yes, replace the minpri
        // sound with the new sound
        n_assert(this->minPriPlayCandidate);
        if (this->minPriPlayCandidate->GetPriority() < s.GetPriority())
        {
            *(this->minPriPlayCandidate) = s;
            newSound = this->minPriPlayCandidate;
            this->minPriPlayCandidate = 0;
        }
    }

    // update the minpri play candidate if something has changed
    if (newSound)
    {
        int i;
        this->minPriPlayCandidate = &(this->playCandidates[0]);
        for (i = 1; i < this->numPlayCandidates; i++)
        {
            if (this->playCandidates[i].GetPriority() < this->minPriPlayCandidate->GetPriority())
            {
                this->minPriPlayCandidate = &(this->playCandidates[i]);
            }
        }
    }

    return newSound;
}

//------------------------------------------------------------------------------
/**
*/
void
nSoundCluster::StartSound(const nSound2& s)
{
    n_assert(this->inBegin);
    n_assert(s.GetId() != 0);

    // insert candidate into play candidate array and set sound to STARTED state
    nSound2* newSound = this->AddPlayCandidate(s);
    if (newSound)
    {
        newSound->SetState(nSound2::STARTED);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nSoundCluster::UpdateSound(const nSound2& s)
{
    n_assert(this->inBegin);
    n_assert(s.GetId() != 0);

    // insert candidate into play candidate array and set sound to UPDATED state
    nSound2* newSound = this->AddPlayCandidate(s);
    if (newSound)
    {
        newSound->SetState(nSound2::UPDATED);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nSoundCluster::StopSound(const nSound2& s)
{
    // empty, must be handled by subclass
    n_assert(this->inBegin);
    n_assert(s.GetId() != 0);
}

//------------------------------------------------------------------------------
/**
*/
void
nSoundCluster::End(const nListener2& /*l*/)
{
    n_assert(this->inBegin);
    this->inBegin = false;
}

//------------------------------------------------------------------------------
/**
    Get the play candidate with the highest priority.
*/
nSound2*
nSoundCluster::GetHighPriPlayCandidate()
{
    nSound2* highPri = 0;
    int i;
    for (i = 0; i < this->numPlayCandidates; i++)
    {
        nSound2& cur = this->playCandidates[i];
        if (cur.GetState() != nSound2::FREE)
        {
            if (highPri)
            {
                if (highPri->GetPriority() < cur.GetPriority())
                {
                    highPri = &cur;
                }
            }
            else
            {
                highPri = &cur;
            }
        }
    }
    return highPri;
}

//------------------------------------------------------------------------------
