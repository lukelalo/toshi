#define N_IMPLEMENTS nPServEmitter
//-------------------------------------------------------------------
//  npservemit_main.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "math/nmath.h"
#include "particle/npservemitter.h"

nNebulaClass(nPServEmitter, "npemitter");

//-------------------------------------------------------------------
//  nPServEmitter()
//  21-Mar-00   floh    created
//-------------------------------------------------------------------
nPServEmitter::nPServEmitter()
              : ref_ps(kernelServer,this)
{
    this->ref_ps = "/sys/servers/particle";
    this->key    = 0;
    this->pull_ringbuffer = NULL;
    this->pull_pass = PULLPASS_NONE;
}

//-------------------------------------------------------------------
//  ~nPServEmitter()
//  21-Mar-00   floh    created
//-------------------------------------------------------------------
nPServEmitter::~nPServEmitter()
{ }

//-------------------------------------------------------------------
//  GetPSystem()
//  Get the particle system object from particle server,
//  create a new one if it doesn't exist for some reason
//  (particle systems may go away if there are no living
//  particles in it.
//  21-Mar-00   floh    created
//  18-Jan-01   floh    if lifetime or frequency dirty (which means
//                      that the maximum number of particles has
//                      changed) kill old particle system and
//                      reallocate a new one
//-------------------------------------------------------------------
nPSystem *nPServEmitter::GetPSystem(int renderContext, float tstamp)
{
    // if we didn't allocate a particle system key yet, do it now
    if (0 == this->key) 
    {
        this->key = this->ref_ps->GetUniqueSystemKey();
    } 
    else if (this->lifetime_dirty || this->freq_dirty) 
    {
        // lifetime or freq has changed, particle system
        // needs to be killed
        this->ref_ps->RemSystem(renderContext, this->key);
        this->lifetime_dirty = false;
        this->freq_dirty     = false;
    }

    // get our particle system
    nPSystem *psys = this->ref_ps->GetSystem(renderContext,this->key);

    // if there is no particle system yet, create one
    if (!psys) 
    {
        int num_particles = this->get_max_particles();
        psys = this->ref_ps->AddSystem(renderContext, this->key, tstamp, num_particles);
    }
    return psys;
}

//-------------------------------------------------------------------
//  BeginPullParticles()
//  21-Mar-00   floh    created
//-------------------------------------------------------------------
bool nPServEmitter::BeginPullParticles(int renderContext, float tstamp)
{
    n_assert(0 != this->key);
    nPSystem *psys = this->GetPSystem(renderContext, tstamp);
   nRingBuffer<nPCorn> *pring = psys->GetRingBuffer();
    if (pring->IsEmpty()) 
    {
        this->pull_ringbuffer = NULL;
        this->pull_pass = PULLPASS_DONE;
        return false;
    } 
    else 
    {
        this->pull_ringbuffer = pring;
        this->pull_pass = PULLPASS_BEGIN;
        return true;
    }
}

//-------------------------------------------------------------------
//  PullParticles()
//  21-Mar-00   floh    created
//-------------------------------------------------------------------
int nPServEmitter::PullParticles(nPCorn *& p)
{
    n_assert(PULLPASS_NONE != this->pull_pass);
    n_assert(PULLPASS_DONE != this->pull_pass);
    nPCorn *head  = this->pull_ringbuffer->GetHead();
    nPCorn *tail  = this->pull_ringbuffer->GetTail();
    nPCorn *start = this->pull_ringbuffer->GetStart();
    nPCorn *end   = this->pull_ringbuffer->GetEnd();
    int num;
    switch (this->pull_pass) 
    {
        case PULLPASS_BEGIN:
            if (tail < head) 
            {
                // one continuous chunk...
                p = tail;
                num = ((head-tail)+1);
                this->pull_pass = PULLPASS_ONE_CHUNK;
            } 
            else 
            {
                // first of two chunks...
                p = tail;
                num = (end-tail);
                this->pull_pass = PULLPASS_FIRST_CHUNK;
            }
            break;

        case PULLPASS_FIRST_CHUNK:
            // second of two chunks...
            p   = start;
            num = (head-start)+1;
            this->pull_pass = PULLPASS_SECOND_CHUNK;
            break;

        case PULLPASS_SECOND_CHUNK:
        case PULLPASS_ONE_CHUNK:
            // all done...
            p   = NULL;
            num = 0;
            this->pull_pass = PULLPASS_DONE;
            break;

        default: 
            p = NULL;
            num = 0;
            break;
    }
    return num;
}

//-------------------------------------------------------------------
//  EndPullParticles()
//  21-Mar-00   floh    created
//-------------------------------------------------------------------
void nPServEmitter::EndPullParticles(void)
{
    n_assert(PULLPASS_DONE == this->pull_pass);
    this->pull_pass = PULLPASS_NONE;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
