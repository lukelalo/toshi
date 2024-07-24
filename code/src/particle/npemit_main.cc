#define N_IMPLEMENTS nPEmitter
//-------------------------------------------------------------------
//  npemit_main.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "particle/npemitter.h"

nNebulaScriptClass(nPEmitter, "nanimnode");

//-------------------------------------------------------------------
//  ~nPEmitter()
//  17-Mar-00   floh    created
//-------------------------------------------------------------------
nPEmitter::~nPEmitter()
{ }

//-------------------------------------------------------------------
//  Initialize()
//  If parent is a nVisNode object, link to it as depend node.
//  13-Nov-00   floh    created
//-------------------------------------------------------------------
void nPEmitter::Initialize(void)
{
    nVisNode *p = (nVisNode *) this->GetParent();
    if (p && p->IsA(kernelServer->FindClass("nvisnode"))) {
        p->DependsOn(this);
    }
    nVisNode::Initialize();
}

//-------------------------------------------------------------------
//  BeginPullParticles()
//  20-Mar-00   floh    created
//-------------------------------------------------------------------
bool nPEmitter::BeginPullParticles(int, float)
{
    n_error("Pure virtual function called!\n");
    return false;
}

//-------------------------------------------------------------------
//  PullParticles()
//  20-Mar-00   floh    created
//-------------------------------------------------------------------
int nPEmitter::PullParticles(nPCorn *&)
{
    n_error("Pure virtual function called!\n");
    return 0;
}

//-------------------------------------------------------------------
//  EndPullParticles()
//  20-Mar-00   floh    created
//-------------------------------------------------------------------
void nPEmitter::EndPullParticles(void)
{
    n_error("Pure virtual function called!\n");
}

//-------------------------------------------------------------------
//  get_max_particles()
//  Compute maximum number of living particles based on lifetime,
//  emission frequency and start/stop/repeat times.
//  18-Mar-00   floh    created
//-------------------------------------------------------------------
int nPEmitter::get_max_particles(void)
{
    if (this->t_repeat > 0.0f) {
        int cycles_per_lifetime = (int)ceil(this->lifetime / this->t_repeat);
        int emits_per_cycle = (int)ceil((this->t_stop-this->t_start)*this->freq);
        return (cycles_per_lifetime * emits_per_cycle);
    } else {
        return (int) ceil(this->lifetime * this->freq);
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
