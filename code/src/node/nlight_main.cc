#define N_IMPLEMENTS nLightNode
//-------------------------------------------------------------------
//  nln_main.cc
//  (C) 1998 A.Weissflog
//-------------------------------------------------------------------
#include "node/nlightnode.h"
#include "gfx/nscenegraph2.h"

nNebulaScriptClass(nLightNode, "nvisnode");

//-------------------------------------------------------------------
//  Attach()
//  20-Apr-99   floh    created
//-------------------------------------------------------------------
bool 
nLightNode::Attach(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);
    if (nVisNode::Attach(sceneGraph)) 
    {
        sceneGraph->AttachLightNode(this);
        return true;
    } 
    return false;
}

//-------------------------------------------------------------------
//  Compute()
//  19-Apr-99   floh    created
//  27-Jun-99   floh    + setzt n->light zuerst korrekt auf NULL
//-------------------------------------------------------------------
void 
nLightNode::Compute(nSceneGraph2* sceneGraph)
{   
    n_assert(sceneGraph);
    nVisNode::Compute(sceneGraph);

    // update the light's modelview matrix
    this->light.SetModelView(sceneGraph->GetTransform());

    // attach the light to the scene graph
    sceneGraph->SetLight(&(this->light));
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

