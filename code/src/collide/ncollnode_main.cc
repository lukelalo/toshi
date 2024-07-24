#define N_IMPLEMENTS nCollideNode
//------------------------------------------------------------------------------
//  ncollnode_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//------------------------------------------------------------------------------
#include "gfx/ngfxserver.h"
#include "gfx/nscenegraph2.h"
#include "collide/ncollideobject.h"
#include "collide/ncollidenode.h"

nNebulaScriptClass(nCollideNode, "nvisnode");

//------------------------------------------------------------------------------
/**
*/
nCollideNode::~nCollideNode()
{
    if (this->collClass) 
    {
        n_free((void *)this->collClass);
        this->collClass = 0;
    }

    if (this->refCollServer.isvalid()) 
    {
        // release collide object
        if (this->collContext)
        {
            if (this->collObject)
            {
                this->collContext->ReleaseObject(this->collObject);
                this->collObject = 0;
            }
        }

        // release collide shape
        if (this->collShape)
        {
            this->refCollServer->ReleaseShape(this->collShape);
            this->collShape = 0;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Attach this object as visual node to the scene graph, the
    scene graph will simply call our Compute() method when its time
    for us to render.
*/
bool 
nCollideNode::Attach(nSceneGraph2* sceneGraph)
{
    if (nVisNode::Attach(sceneGraph)) 
    {
        sceneGraph->AttachVisualNode(this);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
nCollideNode::Compute(nSceneGraph2* sceneGraph)
{
    nVisNode::Compute(sceneGraph);

    // collision objects initialized yet?
    nCollideServer *cs = this->refCollServer.get();
    if (!this->collContext) 
    {
        n_assert(this->collClass);
        this->collContext = cs->GetDefaultContext();
        this->collObject  = this->collContext->NewObject();
        this->collObject->SetCollClass(cs->QueryCollClass(this->collClass));
        this->collContext->AddObject(this->collObject);
    }

    if (this->fileDirty) 
    {
        const char *fname = this->rsrcPath.GetAbsPath();
        
        // release old shape?
        if (this->collShape) 
        {
            cs->ReleaseShape(this->collShape);
        }

        // get a new shape, and load geometry if necessary,
        // (shapes can be shared)
        this->collShape = cs->NewShape(fname);
        if (!this->collShape->IsInitialized()) 
        {
            this->collShape->Load(this->refFileServer.get(), fname);
        }

        // set new shape
        this->collObject->SetShape(this->collShape);
        
        // clean file_dirty flag, so we won't reload next frame
        this->fileDirty = false;
    }

    // get our collision matrix
    matrix44 m(sceneGraph->GetTransform());
    matrix44 vm;
    this->refGfxServer->GetMatrix(N_MXM_VIEWER, vm);
    m.mult_simple(vm);
    this->collObject->Transform(0.0f, m);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

