#define N_IMPLEMENTS nLinkNode
//-------------------------------------------------------------------
//  nlink_main.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "node/nlinknode.h"

//-------------------------------------------------------------------
//  ~nLinkNode()
//  14-Oct-99   floh    created
//-------------------------------------------------------------------
nLinkNode::~nLinkNode()
{ }

//-------------------------------------------------------------------
/**
    @brief Ask target if it's channel set is dirty, do NOT hand up to
    nVisNode class.

     - 22-Jun-01   floh    created
*/
//-------------------------------------------------------------------
bool
nLinkNode::IsChannelSetDirty()
{
    if (this->ref_target.isvalid())
    {
        return this->ref_target->IsChannelSetDirty();
    }
    return false;
}

//-------------------------------------------------------------------
/**
    @brief Simply reroute to target object, do NOT hand up to nVisNode
    class.

     - 22-Jun-01   floh    created
*/
//-------------------------------------------------------------------
void
nLinkNode::AttachChannels(nChannelSet* chnSet)
{
    if (this->ref_target.isvalid())
    {
        this->ref_target->AttachChannels(chnSet);
    }
}

//-------------------------------------------------------------------
/**
    @brief Route the preload method to the target object.
*/
//-------------------------------------------------------------------
void
nLinkNode::Preload()
{
    if (this->ref_target.isvalid())
    {
        this->ref_target->Preload();
    }
    nVisNode::Preload();
}

//-------------------------------------------------------------------
/**
    @brief Attach the target object to the scene graph.

     - 14-Oct-99   floh    created
*/
//-------------------------------------------------------------------
bool nLinkNode::Attach(nSceneGraph2 *sceneGraph)
{
    if (this->ref_target.isvalid())
    {
        return this->ref_target->Attach(sceneGraph);
    }
    return true;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

