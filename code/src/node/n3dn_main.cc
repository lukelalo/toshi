#define N_IMPLEMENTS n3DNode
//------------------------------------------------------------------------------
//  n3dn_main.cc
//  (C) 1998..2001 Andre Weissflog
//------------------------------------------------------------------------------
#include "node/n3dnode.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelserver.h"
#include "gfx/ngfxserver.h"
#include "gfx/nchannelset.h"
#include "gfx/nchannelcontext.h"

nNebulaScriptClass(n3DNode, "nvisnode");

//------------------------------------------------------------------------------
/**
*/
n3DNode::n3DNode() :
    s(1.0f, 1.0f, 1.0f),
    n3dFlags(ACTIVE | MXDIRTY | HIGHDETAIL),
    maxLod(0.0f),
    lodChn(-1),
    detailChn(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    n3DNode requires write access to the 1-dimensional lod channel.

    @param  channelSet      the nChannelSet object to update
*/
void
n3DNode::AttachChannels(nChannelSet* channelSet)
{
    n_assert(channelSet);    
    this->lodChn = channelSet->AddChannel(this->refChannel.get(), "lod");
    if (!this->GetHighDetail())
    {
        this->detailChn = channelSet->AddChannel(this->refChannel.get(), "detail");
    }
    nVisNode::AttachChannels(channelSet);
}

//------------------------------------------------------------------------------
/**
    Update the embedded matrix if the dirty flag is set. The matrix
    is either updated from the (t,r,s) vectors, or if the quaternion
    interface is used, from (t,q,s).
*/
void
n3DNode::updateMatrix()
{
    if ((this->get(MXDIRTY)) && (!(this->get(OVERRIDE)))) 
    {
        if (this->get(USEQUAT)) 
        {
            // use quaternion
            this->mx.ident();
            this->mx.scale(this->s);
            this->mx.mult_simple(matrix44(this->q));
            this->mx.translate(this->t);
        } else {
            // use rotation
            this->mx.ident();
            this->mx.scale(this->s);
            this->mx.rotate_x(this->r.x);
            this->mx.rotate_y(this->r.y);
            this->mx.rotate_z(this->r.z);
            this->mx.translate(this->t);
        }
        // unset the dirty flag
        this->unset(MXDIRTY);
    }
}

//------------------------------------------------------------------------------
/**
    Attach the object to the scene. This begins a new scene graph node,
    may allocate a new channel context (if there is none in the parent
    scene graph node), and then calls nSceneGraph2::AttachTransformNode().
    This will immediately call our own Compute() method, where we update
    our embedded matrix and then call nSceneGraph2::SetTransform().
    Finally, Attach() is invoked on our children, and the scene graph
    node is finished.

    @param  sceneGraph      pointer to our scene graph object
    @return                 true if successful

    11-Apr-02   floh    + read externally provided lod channel and decide
                          whether we should hand attach down to our children
    12-Apr-02   floh    + only attach to scenegraph not LOD-clipped
*/
bool
n3DNode::Attach(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    // do nothing if not active
    bool retval = true;
    if (this->GetActive())
    {
        // get parent's channel context
        nChannelContext* chnContext = sceneGraph->GetChannelContext();
        
        // begin a new scene graph node
        if (sceneGraph->BeginNode())
        {
            // the new node inherits our parent's channel context,
            // however, if the parent has no channel context we
            // are at the top of a visual hierarchy and need
            // to allocate one
            if (!chnContext)
            {
                nChannelSet* chnSet = this->GetChannelSet();
                chnContext = this->refChannel->GetContext(chnSet);
                n_assert(chnContext);
            }
            sceneGraph->SetChannelContext(chnContext);

            // compare curLod against maxLod and decide if we should
            // attach ourselves and our children to the scene
            float curLod = chnContext->GetChannel1f(this->lodChn);
            float curDetail = 1.0f;
            if (!this->GetHighDetail())
            {
                n_assert(-1 != this->detailChn);
                curDetail = chnContext->GetChannel1f(this->detailChn);
            }

            if (((this->maxLod == 0.0f) || (curLod < this->maxLod)) && (curDetail > 0.5f))
            {
                // attach self to scene
                sceneGraph->AttachTransformNode(this);
            
                // attach children to scene
                retval = nVisNode::Attach(sceneGraph);
            }

            // finally, finish the scene graph node
            sceneGraph->EndNode();
        }
        else
        {
            // oops, overflow error in scene graph
            retval = false;
        }
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Updates the internal matrix and computes lod value, finally sets
    the transformation for the current scene graph node.

    @param  sceneGraph      the scene graph who wants us to render
*/
void
n3DNode::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    // let nVisNode call my depend node (NOTE: WE MUST DO THIS even if
    // we are in OVERRIDE mode, see nCurveArrayNode as an example
    // why this is necessary)
    nVisNode::Compute(sceneGraph);

    matrix44 resMatrix;
    if (this->get(OVERRIDE))
    {
        // if we are in override mode, directly set the matrix
        // which has been set per n3DNode::M() (this is useful
        // for game objects who want to copy their matrix efficiently
        // into a n3DNode object).
        resMatrix = this->mx;
    }
    else
    {
        // otherwise the canonical case: update internal matrix
        this->updateMatrix();
        resMatrix = this->mx;

        // if we are in LockViewer mode, copy the current viewer
        // position into the result matrix
        if (this->get(LOCKVWR))
        {
            matrix44 viewMatrix;
            this->refGfx->GetMatrix(N_MXM_VIEWER, viewMatrix);
            resMatrix.M41 = viewMatrix.M41;
            resMatrix.M42 = viewMatrix.M42;
            resMatrix.M43 = viewMatrix.M43;
        }
    }

    if (!this->get(VIEWSPACE))
    {
        // multiply with the parent matrix to flatten the matrix hierarchy
        resMatrix.mult_simple(sceneGraph->GetParentTransform());
    }


    // handle sprite and billboard behaviour
    if (this->get(SPRITE)) 
    {
        // sprite behaviour
        vector3 to(0.0f, 0.0f, 0.0f);
        vector3 up(0.0f, 1.0f, 0.0f);
        resMatrix.lookat(to,up);
    } 
    else if (this->get(BILLBOARD)) 
    {
        // billboard behaviour
        vector3 to(0.0f, 0.0f, 0.0f);
        vector3 up(resMatrix.y_component());
        resMatrix.billboard(to, up);
    }

    // write the resulting transformation into the current scene graph node
    sceneGraph->SetTransform(resMatrix);

    /* Obsolete - Ling
    // FIXME: the lod ranges are hardwired for some sort
    // of backward compatibility, in the future, the lod
    // should simply be "distance to viewer in meters"!!!)
    const float nearLod = 1.0f;
    const float farLod  = 25000.0f;
    float curLod;
    vector3 distVec(resMatrix.pos_component());
    float dist = distVec.len();
    if (dist <= nearLod)        curLod = 1.0f;
    else if (dist >= farLod)    curLod = 0.0f;
    else
    {
        curLod = 1.0f - ((dist - nearLod) / (farLod - nearLod));
    }
    sceneGraph->SetLodValue(curLod);
    */
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
