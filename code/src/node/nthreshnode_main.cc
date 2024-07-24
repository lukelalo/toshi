#define N_IMPLEMENTS nThreshNode
//-------------------------------------------------------------------
//  nthreshnode_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "node/nthreshnode.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelcontext.h"

nNebulaScriptClass(nThreshNode, "nanimnode");

//-------------------------------------------------------------------
//  ~nThreshNode()
//  17-Oct-00   floh    created
//-------------------------------------------------------------------
nThreshNode::~nThreshNode()
{ }

//-------------------------------------------------------------------
/**
    @brief Attach() will only be routed to the child objects if
    the channel value is within the lower and upper bound.

    17-Oct-00   floh    created
*/
//-------------------------------------------------------------------
bool nThreshNode::Attach(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);
    nChannelContext* chnContext = sceneGraph->GetChannelContext();
    n_assert(chnContext);
    float c = chnContext->GetChannel1f(this->localChannelIndex);
    if ((c >= this->lower_bound) && (c < this->upper_bound)) 
    {
        return nVisNode::Attach(sceneGraph);
    } 
    else 
    {
        return true;
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
