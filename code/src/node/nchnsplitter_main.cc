#define N_IMPLEMENTS nChnSplitter
//-------------------------------------------------------------------
//  nchnsplitter_main.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelset.h"
#include "gfx/nchannelcontext.h"
#include "node/nchnsplitter.h"

//-------------------------------------------------------------------
//  nChnSplitter()
//  07-Jul-99   floh    created
//-------------------------------------------------------------------
nChnSplitter::nChnSplitter()
{
    this->num_keys = 0;
    this->keyarray_size = 0;
    this->keyarray = NULL;
}

//-------------------------------------------------------------------
//  ~nChnSplitter()
//  07-Jul-99   floh    created
//-------------------------------------------------------------------
nChnSplitter::~nChnSplitter()
{
    if (this->keyarray) delete[] this->keyarray;
}

//-------------------------------------------------------------------
//  getKey()
//  Allokiert bei Bedarf ein neues Key-Array.
//  07-Jul-99   floh    created
//-------------------------------------------------------------------
nChnSplitterKey *nChnSplitter::getKey(void)
{
    if (this->num_keys < this->keyarray_size) {
        return &(this->keyarray[this->num_keys++]);
    } else {
        // Keyarray muss reallokiert werden
        ulong new_size = this->keyarray_size * 2;
        if (new_size == 0) new_size = 8;
        nChnSplitterKey *new_array = new nChnSplitterKey[new_size];
        if (new_array) {
            // altes Array uebernehmen
            if (this->keyarray) {
                memcpy(new_array,this->keyarray,this->keyarray_size*sizeof(nChnSplitterKey));
                delete[] this->keyarray;
            }
            this->keyarray      = new_array;
            this->keyarray_size = new_size;
            return &(this->keyarray[this->num_keys++]);
        } else return NULL;
    }
}

//-------------------------------------------------------------------
//  AddKey()
//  07-Jul-99   floh    created
//-------------------------------------------------------------------
void nChnSplitter::AddKey(float t, const char *chn_name)
{
    nChnSplitterKey *k = this->getKey();
    k->Set(t,chn_name);
    this->NotifyChannelSetDirty();
}

//-------------------------------------------------------------------
//  BeginKeys()
//  07-Jul-99   floh    created
//-------------------------------------------------------------------
void nChnSplitter::BeginKeys(int num)
{
    if (this->keyarray) {
        delete[] this->keyarray;
        this->keyarray = NULL;
    }
    this->keyarray_size = num;
    this->num_keys      = num;
    this->keyarray = new nChnSplitterKey[this->num_keys];
    n_assert(this->keyarray);
}

//-------------------------------------------------------------------
//  SetKey()
//  07-Jul-99   floh    created
//-------------------------------------------------------------------
void nChnSplitter::SetKey(int i, float t, const char *chn_name)
{
    n_assert(i >= 0);
    if (i < this->num_keys) {
        nChnSplitterKey *k = &(this->keyarray[i]);
        k->Set(t,chn_name);
    } else {
        n_printf("nChnSplitter::SetKey(): key index '%d' too big\n",i);
    }
}

//-------------------------------------------------------------------
//  EndKeys()
//  07-Jul-99   floh    created
//-------------------------------------------------------------------
void nChnSplitter::EndKeys(void)
{
    this->NotifyChannelSetDirty();
}

//-------------------------------------------------------------------
//  GetKeyInfo()
//  07-Jul-99   floh    created
//-------------------------------------------------------------------
void nChnSplitter::GetKeyInfo(int& num)
{
    num = this->num_keys;
}

//-------------------------------------------------------------------
//  GetKey()
//  07-Jul-99   floh    created
//-------------------------------------------------------------------
void nChnSplitter::GetKey(int i, float& t, const char *&chn_name)
{
    n_assert(i >= 0);
    if (i < this->num_keys) {
        nChnSplitterKey *k = &(this->keyarray[i]);
        t = k->t;
        chn_name = (const char *) &(k->chn_name[0]);
    } else {
        n_printf("nChnSplitter::GetKey(): key index '%d' too big\n",i);
        t = 0.0f;
        chn_name = NULL;
    }
}

//-------------------------------------------------------------------
//  AttachChannel()
//  Attach channels to channel set which we are interested in for
//  reading and writing.
//  24-Jun-01   floh    created
//-------------------------------------------------------------------
void
nChnSplitter::AttachChannels(nChannelSet* chnSet)
{
    n_assert(chnSet);
    n_assert(this->keyarray);
    int i;
    for (i = 0; i < num_keys; i++)
    {
        nChnSplitterKey* key = &(this->keyarray[i]);
        key->chn_num = chnSet->AddChannel(this->refChannel.get(), key->chn_name);
    }
    nAnimNode::AttachChannels(chnSet);
}

//-------------------------------------------------------------------
//  Attach()
//  02-Jul-01   floh    created
//-------------------------------------------------------------------
bool
nChnSplitter::Attach(nSceneGraph2* sceneGraph)
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
//  07-Jul-99   floh    created
//  30-Sep-99   floh    clampt im OneShot Modus t auf den
//                      gueltigen Wertebereich
//  24-Jun-01   floh    + new scene graph and channel writing
//  24-Jul-01   floh    + some corrections to prevent illegal array accesses
//-------------------------------------------------------------------
void nChnSplitter::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    // up to parent class
    nVisNode::Compute(sceneGraph);

    if (this->num_keys > 0) 
    {
        float tscale = this->scale;
        float min_t = this->keyarray[0].t * tscale;
        float max_t = this->keyarray[this->num_keys-1].t * tscale;

        // get our channel context
        nChannelContext* chnContext = sceneGraph->GetChannelContext();
        n_assert(chnContext);

        // reset all output channels to 0
        int i;
        for (i=0; i<this->num_keys; i++) 
        {
            nChnSplitterKey *k = &(this->keyarray[i]);
            k->chn_val = 0.0f;
        }

        if (max_t > 0.0) 
        {
            // get current time
            float t = chnContext->GetChannel1f(this->localChannelIndex);
            if (this->repType == N_REPTYPE_LOOP) 
            {
                // in loop mode, wrap time to loop interval
                t = t - (((float)floor(t/max_t)) * max_t);
            } 

            // clamp time to range
            if      (t < min_t)  t = min_t;
            else if (t >= max_t) t = max_t - 0.0001f;

            // find the right key and interpolate
            i = 0;
            if (this->keyarray[0].t > 0.0f) 
            {
                char buf[N_MAXPATH];
                n_error("Object '%s' 1st keyframe > 0.0f!\n",this->GetFullName(buf,sizeof(buf)));
            };
            while ((this->keyarray[i].t*tscale) <= t) i++;
            n_assert((i > 0) && (i < this->num_keys));
            nChnSplitterKey *k0 = &(this->keyarray[i-1]);
            nChnSplitterKey *k1 = &(this->keyarray[i]);
            float t0 = k0->t * tscale;
            float t1 = k1->t * tscale;
            float l;
            if (t1 > t0) l = (float) ((t-t0)/(t1-t0));
            else         l = 1.0f;
            k0->chn_val = 1.0f - l;
            k1->chn_val = l;
        }

        // write channels
        for (i=0; i<this->num_keys; i++) 
        {
            nChnSplitterKey *k = &(this->keyarray[i]);
            chnContext->SetChannel1f(k->chn_num, k->chn_val);
        }
    }
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
