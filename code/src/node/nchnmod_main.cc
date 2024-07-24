#define N_IMPLEMENTS nChnModulator
//-------------------------------------------------------------------
//  nchnmod_main.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelset.h"
#include "gfx/nchannelcontext.h"
#include "node/nchnmodulator.h"

//-------------------------------------------------------------------
//  nChnModulator()
//  08-Jul-99   floh    created
//-------------------------------------------------------------------
nChnModulator::nChnModulator()
{
    this->num_defs = 0;
    this->defarray = NULL;
}

//-------------------------------------------------------------------
//  ~nChnModulator()
//  08-Jul-99   floh    created
//-------------------------------------------------------------------
nChnModulator::~nChnModulator()
{
    if (this->defarray) delete[] this->defarray;
}

//-------------------------------------------------------------------
//  Begin()
//  08-Jul-99   floh    created
//-------------------------------------------------------------------
void nChnModulator::Begin(int num)
{
    n_assert(NULL == this->defarray);
    this->num_defs = num;
    this->defarray = new nChnModDef[num];
}

//-------------------------------------------------------------------
//  Set()
//  08-Jul-99   floh    created
//-------------------------------------------------------------------
void nChnModulator::Set(int i, const char *c0, const char *c1)
{
    n_assert(i >= 0);
    n_assert(i < this->num_defs);
    n_assert(this->defarray);
    nChnModDef *d = &(this->defarray[i]);
    d->Set(c0,c1);
}

//-------------------------------------------------------------------
//  End()
//  08-Jul-99   floh    created
//-------------------------------------------------------------------
void nChnModulator::End(void)
{
    this->NotifyChannelSetDirty();
}

//-------------------------------------------------------------------
//  GetNum()
//  08-Jul-99   floh    created
//-------------------------------------------------------------------
int nChnModulator::GetNum(void)
{
    return this->num_defs;
}

//-------------------------------------------------------------------
//  Get()
//  08-Jul-99   floh    created
//-------------------------------------------------------------------
void nChnModulator::Get(int i, const char *& c0, const char *& c1)
{
    n_assert(i >= 0);
    n_assert(i < this->num_defs);
    n_assert(this->defarray);
    nChnModDef *d = &(this->defarray[i]);
    c0 = d->chn0_name;
    c1 = d->chn1_name;
}

//-------------------------------------------------------------------
//  AttachChannels()
//  Attach channels we are interested in for reading or writing
//  to the nChannelSet object.
//  24-Jun-01   floh    created
//-------------------------------------------------------------------
void
nChnModulator::AttachChannels(nChannelSet* chnSet)
{
    n_assert(chnSet);
    n_assert(this->defarray);
    int i;
    for (i = 0; i < this->num_defs; i++)
    {
        nChnModDef* def = &(this->defarray[i]);
        def->chn0_num = chnSet->AddChannel(this->refChannel.get(), def->chn0_name);
        def->chn1_num = chnSet->AddChannel(this->refChannel.get(), def->chn1_name);
    }
    nVisNode::AttachChannels(chnSet);
}

//-------------------------------------------------------------------
//  Attach()
//  02-Jul-01   floh    created
//-------------------------------------------------------------------
bool
nChnModulator::Attach(nSceneGraph2* sceneGraph)
{
    if (nVisNode::Attach(sceneGraph))
    {
        this->Compute(sceneGraph);
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
//  Compute()
//  08-Jul-99   floh    created
//-------------------------------------------------------------------
void nChnModulator::Compute(nSceneGraph2 *sceneGraph)
{
    n_assert(sceneGraph);

    // up to parent class
    nVisNode::Compute(sceneGraph);
    
    // get channel context
    nChannelContext* chnContext = sceneGraph->GetChannelContext();
    n_assert(chnContext);
    n_assert(this->defarray);

    // for each channel modulate definition
    int i;
    for (i=0; i<this->num_defs; i++) 
    {
        nChnModDef *d = &(this->defarray[i]);
        
        // read current channel values
        d->val0 = chnContext->GetChannel1f(d->chn0_num);
        d->val1 = chnContext->GetChannel1f(d->chn1_num);

        // modulate channel 0 by channel 1
        d->val0 *= d->val1;

        // write back modulated channel
        chnContext->SetChannel1f(d->chn0_num, d->val0);
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

