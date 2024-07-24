#define N_IMPLEMENTS nWeightTree
//-------------------------------------------------------------------
//  nwtree_main.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelcontext.h"
#include "gfx/nchannelset.h"
#include "node/nweighttree.h"

//-------------------------------------------------------------------
//  nWeightTree()
//  03-Jul-99   floh    created
//-------------------------------------------------------------------
nWeightTree::nWeightTree()
{
    // empty
}

//-------------------------------------------------------------------
//  ~nWeightTree()
//  03-Jul-99   floh    created
//-------------------------------------------------------------------
nWeightTree::~nWeightTree()
{
    nWeightNode *wn;
    while ((wn = (nWeightNode *) this->wlist.RemHead())) delete wn;
}

//-------------------------------------------------------------------
//  AddLeaf()
//  Name der Node entspricht dem Channel, der abgetastet werden
//  soll.
//  03-Jul-99   floh    created
//-------------------------------------------------------------------
bool nWeightTree::AddLeaf(const char *n)
{
    n_assert(n);

    // Name darf noch nicht existieren...
    nWeightNode *wn = (nWeightNode *) this->wlist.Find(n);
    if (wn) {
        n_printf("weight node '%s' already exists!\n",n);
        return false;
    } else {
        nWeightNode *wn = new nWeightNode(n);
        this->wlist.AddTail(wn);
        this->NotifyChannelSetDirty();
        return true;
    }
}

//-------------------------------------------------------------------
//  AddNode()
//  03-Jul-99   floh    created
//-------------------------------------------------------------------
bool nWeightTree::AddNode(const char *n,
                          const char *c0_name,
                          const char *c1_name)
{
    n_assert(n);
    n_assert(c0_name);
    n_assert(c1_name);

    // Node mit diesem Namen darf noch nicht existieren
    nWeightNode *wn = (nWeightNode *) this->wlist.Find(n);
    if (wn) {
        n_printf("weight node '%s' already exists!\n",n);
        return false;
    }

    // Child-Nodes muessen schon existieren
    nWeightNode *c0 = (nWeightNode *) this->wlist.Find(c0_name);
    nWeightNode *c1 = (nWeightNode *) this->wlist.Find(c1_name);
    if (!c0) {
        n_printf("no weight node '%s'!\n",c0_name);
        return false;
    }
    if (!c1) {
        n_printf("no weight node '%s'!\n",c1_name);
        return false;
    }

    wn = new nWeightNode(n);
    this->wlist.AddTail(wn);
    wn->c[0] = c0;
    wn->c[1] = c1;
    c0->parent = wn;
    c1->parent = wn;

    return true;
}

//-------------------------------------------------------------------
//  AttachChannels()
//  Attach the channels we are interested in for reading/writing
//  to the channel set.
//  24-Jun-01   floh    created
//-------------------------------------------------------------------
void nWeightTree::AttachChannels(nChannelSet* chnSet)
{
    n_assert(chnSet);

    // for each leaf node...
    nWeightNode *wn;
    for (wn = (nWeightNode *) this->wlist.GetHead();
         wn;
         wn = (nWeightNode *) wn->GetSucc())
    {
        // break on the first non-leaf-node
        if (NULL != wn->c[0]) break;

        // initialize local channel index from channel name
        wn->chn_num = chnSet->AddChannel(this->refChannel.get(), wn->GetName());
    }
    nVisNode::AttachChannels(chnSet);
}

//-------------------------------------------------------------------
//  initNodeVals()
//  For each leaf node, read current channel value, for each
//  knot-node, set value to 1.0f.
//  03-Jul-99   floh    created
//  24-Jun-01   floh    rewritten for new channel stuff
//-------------------------------------------------------------------
void nWeightTree::initNodeVals(nChannelContext* chnContext)
{
    n_assert(chnContext);

    nWeightNode *wn;
    for (wn = (nWeightNode *) this->wlist.GetHead();
         wn;
         wn = (nWeightNode *) wn->GetSucc())
    {
        if (wn->chn_num >= 0) 
        {
            // a leaf node, read channel value
            wn->val = chnContext->GetChannel1f(wn->chn_num);
        } 
        else 
        {
            // a knot, set value to 1.0
            wn->val = 1.0f;
        }
    }
}

//-------------------------------------------------------------------
//  writeLeafVals()
//  Write the value of each leaf node back to the channel.
//  03-Jul-99   floh    created
//  24-Jun-01   floh    new channel stuff
//-------------------------------------------------------------------
void nWeightTree::writeLeafVals(nChannelContext* chnContext)
{
    n_assert(chnContext);

    nWeightNode *wn;
    for (wn = (nWeightNode *) this->wlist.GetHead();
         wn;
         wn = (nWeightNode *) wn->GetSucc())
    {
        // break on the first non-leaf-node
        if (NULL != wn->c[0]) break;
        else 
        {
            n_assert(wn->chn_num >= 0);
            chnContext->SetChannel1f(wn->chn_num, wn->val);
        }
    }
}

//-------------------------------------------------------------------
//  evalNode()
//  Evaluiert eine einzelne Node:
//  Falls die Summe der Child-Node-Werte groesser ist als
//  der eigene Wert, werden die Child-Nodes angepasst:
//  - falls der Wert der dominierenden Node zu gross ist,
//    wird dieser auf den Wert der Parent-Node gesetzt und
//    die untergeordnete Node auf 0.0 gesetzt
//  - ansonsten wird nur der Wert der untergeordneten
//    Node angepasst
//  - auf jede Node, die veraendert wurde, wird ein evalNode()
//    rekursiv angewendet
//  03-Jul-99   floh    created
//-------------------------------------------------------------------
void nWeightTree::evalNode(nWeightNode *wn)
{
    if (wn->chn_num < 0) {
        float sum = wn->c[0]->val + wn->c[1]->val;
        if (sum > wn->val) {
            // clampe Wert der dominanten Node auf aktuellen Wert
            if (wn->c[0]->val > wn->val) {
                wn->c[0]->val = wn->val;
                this->evalNode(wn->c[0]);
            }
            // passe Wert der untergeordneten Node an
            float diff = wn->val - wn->c[0]->val;
            wn->c[1]->val = diff;
            this->evalNode(wn->c[1]);
        }
        wn->val = sum;
    }
}

//-------------------------------------------------------------------
//  Attach()
//  02-Jul-01   floh    created
//-------------------------------------------------------------------
bool
nWeightTree::Attach(nSceneGraph2* sceneGraph)
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
//
//  Evaluiert den Weight-Tree:
//  Fuer jeden Knoten gilt: die Summe der Werte der Kinder
//  darf nicht groesser sein, als der Wert des Knotens
//  selbst. Initial haben alle Knoten einen Wert von 1.0,
//  liegt die Summe der Werte der Kinder darueber, wird
//  der Wert der dominierten Child-Node so angepasst, dass
//  die Summe wieder stimmt, genauso fuer alle untergeordneten
//  Nodes.
//
//  03-Jul-99   floh    created
//  24-Jun-01   floh    + new channel stuff
//-------------------------------------------------------------------
void 
nWeightTree::Compute(nSceneGraph2* sceneGraph)
{
    nVisNode::Compute(sceneGraph);

    // get channel context
    nChannelContext* chnContext = sceneGraph->GetChannelContext();
    n_assert(chnContext);

    // init tree values (reads channels for leaf nodes)
    this->initNodeVals(chnContext);

    // evaluate the tree
    nWeightNode *wn;
    for (wn = (nWeightNode *) this->wlist.GetHead();
         wn;
         wn = (nWeightNode *) wn->GetSucc())
    {
        this->evalNode(wn);
    }
    
    // write leaf values back to channels
    this->writeLeafVals(chnContext);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

