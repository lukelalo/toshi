#define N_IMPLEMENTS nShadowControl
//------------------------------------------------------------------------------
//  nshadowcontrol_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shadow/nshadowcontrol.h"
#include "shadow/nshadowserver.h"

nNebulaScriptClass(nShadowControl, "nvisnode");

//------------------------------------------------------------------------------
/**
*/
nShadowControl::nShadowControl() :
    refShadowServer(kernelServer, this)
{
    this->refShadowServer = "/sys/servers/shadow";
}

//------------------------------------------------------------------------------
/**
*/
nShadowControl::~nShadowControl()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowControl::Initialize()
{
    // attach to parent object as depend node, so that it
    // will call Compute() on me
    nVisNode* p = (nVisNode*) this->GetParent();
    if (p && p->IsA(kernelServer->FindClass("nvisnode")))
    {
        p->DependsOn(this);
    }
    nVisNode::Initialize();
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowControl::SetColor(float r, float g, float b, float a)
{
    this->color.set(r, g, b, a);
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowControl::GetColor(float& r, float& g, float& b, float& a)
{
    r = this->color.x;
    g = this->color.y;
    b = this->color.z;
    a = this->color.w;
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowControl::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    // trigger depend nodes
    nVisNode::Compute(sceneGraph);

    // update shadow color
    this->refShadowServer->SetShadowColor(this->color.x, 
                                          this->color.y, 
                                          this->color.z, 
                                          this->color.w);
}

//------------------------------------------------------------------------------




