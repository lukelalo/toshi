#define N_IMPLEMENTS nFlipFlop
//-------------------------------------------------------------------
//  nflip_main.cc
//  (C) 1998 Andre Weissflog
//-------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelcontext.h"
#include "node/nflipflop.h"

//-------------------------------------------------------------------
//  nFlipFlop()
//  09-Dec-98   floh    created
//-------------------------------------------------------------------
nFlipFlop::nFlipFlop()
{
    this->keyarray_size = 8;
    this->num_keys = 0;
    this->keyarray = new nObjectKey[this->keyarray_size];
    if (!this->keyarray) n_error("Out of mem!");
    // beim Speichern muessen zuerst die Child-Objekte, DANN
    // unser eigener Status gespeichert werden
    this->SetFlags(N_FLAG_SAVEUPSIDEDOWN);
}

//-------------------------------------------------------------------
//  ~nFlipFlop()
//  09-Dec-98   floh    created
//  24-Apr-99   floh    + keine Loeschschutz mehr fuer Subobjekte
//-------------------------------------------------------------------
nFlipFlop::~nFlipFlop()
{
    if (this->keyarray) delete[] this->keyarray;
}

//-------------------------------------------------------------------
//  getKey()
//  09-Dec-98   floh    created
//-------------------------------------------------------------------
nObjectKey *nFlipFlop::getKey(void)
{
    if (this->num_keys < this->keyarray_size) {
        return &(this->keyarray[this->num_keys++]);
    } else {
        // Keyarray muss reallokiert werden
        ulong new_size = this->keyarray_size * 2;
        nObjectKey *new_array = new nObjectKey[new_size];
        if (new_array) {
            memcpy(new_array,this->keyarray,this->keyarray_size*sizeof(nObjectKey));
            delete[] this->keyarray;
            this->keyarray      = new_array;
            this->keyarray_size = new_size;
            return &(this->keyarray[this->num_keys++]);
        } else {
            n_error("Out of mem!");
            return NULL;
        }
    }
}

//-------------------------------------------------------------------
//  AddKey()
//  09-Dec-98   floh    created
//  24-Apr-99   floh    kein Loeschschutz mehr fuer
//                      Sub-Objekte
//-------------------------------------------------------------------
bool nFlipFlop::AddKey(float time, const char *name)
{
    n_assert(name);
    nVisNode *c;
    c = (nVisNode *) this->Find(name);
    if (c) {
        nObjectKey *k = this->getKey();
        if (k) {
            k->Set(time,c);
            return true;
        }
    } else {
        n_printf("nFlipFlop::AddKey(): Child object %s not found!\n",name);
    }
    return false;
}

//-------------------------------------------------------------------
//  Attach()
//  Attach() wird nur an das per Timestamp gueltige
//  Objekt weitergegeben.
//  20-Apr-99   floh    created
//  25-May-99   floh    + nicht mehr auf GlobalTime hardgecodet
//                      + TimeScale-Support
//  30-Sep-99   floh    + clampt im OneShot-Modus t auf den gueltigen
//                        Wertebereich
//  27-Jun-01   floh    + new scene graph stuff
//  24-Jul-01   floh    + some corrections to prevent illegal array accesses
//-------------------------------------------------------------------
bool nFlipFlop::Attach(nSceneGraph2 *sceneGraph)
{
    n_assert(sceneGraph);

    // selectively route Attach() to subobjects
    if (this->num_keys > 1) 
    {
        // WE DO NOT CALL nVisNode::Attach() BECAUSE WE NEED TO
        // SELECTIVELY ROUTE Attach() TO OUR CHILDREN, THAT'S WHY
        // SOME OF THE nVisNode::Attach() FUNCTIONALITY NEEDS
        // TO BE "EMULATED"
        float tscale = this->scale;
        float min_t = this->keyarray[0].t * tscale;
        float max_t = this->keyarray[this->num_keys-1].t * tscale;
        if (max_t > 0.0) 
        {
            nChannelContext* chnContext = sceneGraph->GetChannelContext();
            n_assert(chnContext);
            float t = chnContext->GetChannel1f(this->localChannelIndex);

            if (this->repType == N_REPTYPE_LOOP) 
            {
                // im Loop-Modus Zeit auf Durchlaufzeit normalisieren
                t = t - (((float)floor(t/max_t)) * max_t);
            } 

            // biege t auf den gueltigen Wertebereich
            float correction = (max_t - min_t) / 10000.0f;
            if      (t < min_t)  t = min_t;
            else if (t >= max_t) t = max_t - correction;
        
            int i = 0;
            if (this->keyarray[0].t > 0.0f) 
            {
                char buf[N_MAXPATH];
                n_error("Object '%s' 1st keyframe > 0.0f!\n",this->GetFullName(buf,sizeof(buf)));
            };
            while ((this->keyarray[i].t * tscale) <= t) i++;
            n_assert((i > 0) && (i < this->num_keys));
            nObjectKey *k = &(this->keyarray[i-1]);
            k->o->Attach(sceneGraph);
        }
    }
    return true;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
