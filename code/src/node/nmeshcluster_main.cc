#define N_IMPLEMENTS nMeshCluster
//-------------------------------------------------------------------
//  nmeshcluster_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "node/nmeshcluster.h"
#include "node/njoint.h"
#include "gfx/nscenegraph2.h"
#include "gfx/ngfxserver.h"
#include "shadow/nshadowserver.h"
#include "shadow/nshadowcaster.h"

//-------------------------------------------------------------------
//  ~nMeshCluster()
//  21-Oct-00   floh    created
//-------------------------------------------------------------------
nMeshCluster::~nMeshCluster()
{
    if (this->j_array) 
    {
        delete[] this->j_array;
        this->j_array = NULL;
    }
    if (this->refShadowCaster.isvalid())
    {
        this->refShadowCaster->Release();
    }
}

//-------------------------------------------------------------------
//  count_joints()
//  Recursively count joints.
//  06-Nov-00   floh    created
//-------------------------------------------------------------------
void nMeshCluster::count_joints(nRoot *joint, int& num_joints)
{
    n_assert(joint->IsA(this->njoint_class));
    num_joints++;
    nRoot *child;
    for (child=joint->GetHead(); child; child=child->GetSucc()) 
    {
        if (child->IsA(this->njoint_class)) 
        {
            this->count_joints(child,num_joints);
        }
    }
}

//-------------------------------------------------------------------
//  collect_joints()
//  Build joint palette by recursively collecting the joints
//  in a joint hierarchy.
//  06-Nov-00   floh    created
//-------------------------------------------------------------------
void nMeshCluster::collect_joints(nRoot *joint, int& cur_joint)
{
    n_assert(joint->IsA(this->njoint_class));
    this->set_joint(cur_joint++,(nJoint *)joint);
    nRoot *child;
    for (child=joint->GetHead(); child; child=child->GetSucc()) 
    {
        if (child->IsA(this->njoint_class)) 
        {
            this->collect_joints(child,cur_joint);
        }
    }
}

//-------------------------------------------------------------------
//  SetRootJoint()
//  Defines the root joint and builds the joint palette.
//  06-Nov-00   floh    created
//-------------------------------------------------------------------
void nMeshCluster::SetRootJoint(const char *n)
{
    n_assert(n);

    this->ref_root = n;

    // get the nJoint type handle
    this->njoint_class = ks->FindClass("njoint");

    // pass 1, count joints in joint hierarchy
    int num_joints = 0;
    this->count_joints(this->ref_root.get(), num_joints);

    // pass2, build joint palette
    this->begin_joints(num_joints);
    int cur_joint = 0;
    this->collect_joints(this->ref_root.get(),cur_joint);
    this->end_joints();
    n_assert(num_joints == cur_joint);
}

//-------------------------------------------------------------------
//  eval_skin()
//  Do the actual skinning.
//  23-Oct-00   floh    created
//  06-Nov-00   floh    + rewritten to multiple weights per vertex
//  06-Aug-01   floh    + update shadow caster
//-------------------------------------------------------------------
void nMeshCluster::eval_skin(nVertexBuffer *skin_vb, nSceneGraph2 *sceneGraph)
{
    n_assert(skin_vb && sceneGraph);
    nIndexBuffer* indexBuffer = sceneGraph->GetIndexBuffer();
    n_assert(indexBuffer);

    // get destination vertex buffer
    nVertexBuffer *vb_dest = this->dyn_vb.Begin(
        indexBuffer,
        sceneGraph->GetPixelShader(),
        sceneGraph->GetTextureArray());
    skin_vb->LockVertices();

    // get source and destination vertex component pointers
    float *src_coord = skin_vb->coord_ptr;  
    float *dst_coord = vb_dest->coord_ptr;
    float *src_norm  = skin_vb->norm_ptr;
    float *dst_norm  = vb_dest->norm_ptr;
    ulong *src_color = skin_vb->color_ptr;
    ulong *dst_color = vb_dest->color_ptr;
    
    // FIXME: to keep the inner loop somewhat simple, we assume
    // that the skin vertex buffer has normals
    n_assert(src_coord && src_norm);

    float *src_uv[N_MAXNUM_TEXCOORDS];
    float *dst_uv[N_MAXNUM_TEXCOORDS];
    int i;
    int num_texcoordsets = 0;
    for (i=0; i<N_MAXNUM_TEXCOORDS; i++) 
    {
        src_uv[i] = skin_vb->uv_ptr[i];
        dst_uv[i] = vb_dest->uv_ptr[i];
        if (src_uv[i])
        {
            num_texcoordsets++;
        }
        else break;
    }
    float *src_wgt = skin_vb->w_ptr;
    int *src_jnt   = skin_vb->j_ptr;
    n_assert(skin_vb->GetVertexType() & N_VT_JW);

    int num_v      = skin_vb->GetNumVertices();
    int src_stride = skin_vb->stride4;
    int dst_stride = vb_dest->stride4;

    // handle shadow stuff
    vector3* shadowCoordPtr = 0;
    if (this->refShadowCaster.isvalid() && this->castShadow)
    {
        this->refShadowCaster->Lock();
        shadowCoordPtr = this->refShadowCaster->GetCoordPtr();
    }

    // for each vertex... (-> important to write target buffer continously!)
    vector3 v0,v1,n0,n1;
    for (i=0; i<num_v; i++) 
    {
        // read source coordinate and normal
        v0.set(src_coord[0],src_coord[1],src_coord[2]);
        n0.set(src_norm[0],src_norm[1],src_norm[2]);
        src_coord += src_stride;
        src_norm  += src_stride;

        // compute weighted coordinate and normal
        int j;
        v1.set(0.0f,0.0f,0.0f);
        n1.set(0.0f,0.0f,0.0f);
        for (j=0; j<4; j++) 
        {
            float w = src_wgt[j];
            if (w > 0.0f)
            {
                nJoint *joint = this->j_array[src_jnt[j]];
                v1 += (joint->irm_times_m * v0) * w;
                n1 += (joint->nirm_times_nm * n0) * w;
            }
        }
        src_jnt += src_stride;
        src_wgt += src_stride;

        // write destination coordinate
        dst_coord[0]=v1.x;  dst_coord[1]=v1.y;  dst_coord[2]=v1.z;
        dst_norm[0] =n1.x;  dst_norm[1] =n1.y;  dst_norm[2] =n1.z;
        dst_coord += dst_stride;
        dst_norm  += dst_stride;

        // update shadow caster
        if (shadowCoordPtr)
        {
            shadowCoordPtr->set(v1.x, v1.y, v1.z);
            shadowCoordPtr++;
        }

        // copy color and uv coordinates if necessary
        if (src_color) 
        {
            dst_color[0] = src_color[0];
            src_color += src_stride;
            dst_color += dst_stride;
        }
        int k;
        for (k=0; k<num_texcoordsets; k++) 
        {
            dst_uv[k][0] = src_uv[k][0];
            dst_uv[k][1] = src_uv[k][1];
            src_uv[k] += src_stride;
            dst_uv[k] += dst_stride;
        }
    }

    // instead of keeping track of the exact bounding box,
    // we create a very conservative bounding box (twice
    // the diameter of the original bounding box)
    vector3 bbdiam = skin_vb->GetBBox().vmax - skin_vb->GetBBox().vmin;
    float l = bbdiam.len();
    vector3 vmin(-l,-l,-l);
    vector3 vmax(+l,+l,+l);
    bbox3 bbox(vmin,vmax);
    vb_dest->SetBBox(bbox);

    // unlock and finish
    if (shadowCoordPtr)
    {
        this->refShadowCaster->Unlock();
        this->refShadowServer->AttachCaster(sceneGraph->GetTransform(), this->refShadowCaster.get());
    }
    skin_vb->UnlockVertices();
    this->dyn_vb.End(num_v, indexBuffer->GetNumIndices());

    // if readonly, hand vertex buffer up to whoever wants it
    if (this->dyn_vb.GetReadOnly())
    {
        sceneGraph->SetVertexBuffer(vb_dest);
    }
}

//-------------------------------------------------------------------
//  Attach()
//  23-Oct-00   floh    created
//  24-Jun-01   floh    new scene graph stuff
//-------------------------------------------------------------------
bool nMeshCluster::Attach(nSceneGraph2 *sceneGraph)
{
    n_assert(sceneGraph);
    if (nVisNode::Attach(sceneGraph)) 
    {
        if (this->dyn_vb.GetReadOnly()) 
        {
            // if readonly mode, attach as meshnode
            sceneGraph->AttachMeshNode(this);
        } 
        else 
        {
            // in default mode, attach as complex node
            sceneGraph->AttachVisualNode(this);
        }
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
//  Compute()
//  23-Oct-00   floh    created
//  24-Jun-01   floh    new scene graph stuff, and depend on
//                      njointanim object (attached as depend node)
//                      to update the skeleton animation (no longer
//                      invokes Compute() on the joint hierarchy!)
//-------------------------------------------------------------------
void nMeshCluster::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    // hand up Compute(), so that our depend nodes are triggered
    nVisNode::Compute(sceneGraph);

    // invoke compute on the skin mesh and get pointer to vertex buffer
    this->ref_skin->Compute(sceneGraph);
    nVertexBuffer *skin_vb = sceneGraph->GetVertexBuffer();
    n_assert(skin_vb);
    sceneGraph->SetVertexBuffer(0);

    // create dynamic vertex buffer on demand
    if (!this->dyn_vb.IsValid()) 
    {
        // the destination vertex type is the same as the
        // source vertex type minus any weights and joint indices
        int src_vtype = skin_vb->GetVertexType();
        
        // make sure there are weights and joint indices in
        // the source vertex buffer
        n_assert(src_vtype & N_VT_JW);
        int dest_vtype = src_vtype & ~N_VT_JW;
        this->dyn_vb.Initialize(dest_vtype, skin_vb->GetNumVertices());

        // if shadow casting enabled, also initialize a shadow caster
        // object, the source mesh must have shadow casting enabled
        // as well!
        nShadowCaster* srcCaster = sceneGraph->GetShadowCaster();
        if (this->castShadow && srcCaster)
        {
            this->refShadowCaster = this->refShadowServer->NewCaster(0);
            this->refShadowCaster->Initialize(srcCaster->GetNumCoords(), srcCaster->GetNumEdges());
            this->refShadowCaster->CopyEdges(srcCaster, 0, srcCaster->GetNumEdges());
        }
    };
    sceneGraph->SetShadowCaster(0);

    // make sure that the target vertex buffer is great enough
    // (HACK: this may be a problem if the size of the source skin
    // changes (for instance for things like lod), so in this case
    // it would actually be better to have one meshcluster for each
    // lod)
    n_assert(skin_vb->GetNumVertices() <= this->dyn_vb.GetNumVertices());

    // one-timer: compute the relaxed state of the joint skeleton
    if (this->joints_dirty) 
    {
        this->j_array[0]->EvalRelaxedState(NULL);
        this->joints_dirty = false;
    }

    // evaluate current state joint skeleton
    this->ref_root->EvalCurrentState(NULL);

    // do the actual skinning
    this->eval_skin(skin_vb, sceneGraph);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
