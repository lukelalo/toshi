#define N_IMPLEMENTS nMeshCluster2
//------------------------------------------------------------------------------
//  nmeshcluster2_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "node/nmeshcluster2.h"
#include "node/njoint2.h"
#include "gfx/nscenegraph2.h"
#include "gfx/ngfxserver.h"
#include "shadow/nshadowserver.h"
#include "shadow/nshadowcaster.h"

nNebulaScriptClass(nMeshCluster2, "nvisnode");

//------------------------------------------------------------------------------
/**
*/
nMeshCluster2::nMeshCluster2() :
    refGfx(kernelServer, this),
    refShadowServer(kernelServer, this),
    refSkin(kernelServer, this),
    refBones(kernelServer, this),
    njoint2Class(0),
    numJoints(0),
    jointPalette(0),
    jointsDirty(false),
    dynVBuf(kernelServer, this),
    castShadow(false),
    refShadowCaster(this)
{
    this->refShadowServer = "/sys/servers/shadow";
    this->SetFlags(N_FLAG_SAVEUPSIDEDOWN);
}

//------------------------------------------------------------------------------
/**
*/
nMeshCluster2::~nMeshCluster2()
{
    if (this->jointPalette)
    {
        delete[] this->jointPalette;
        this->jointPalette = 0;
    }
    if (this->refShadowCaster.isvalid())
    {
        this->refShadowCaster->Release();
    }
}

//------------------------------------------------------------------------------
/**
    Recursively count number of joints under given object.
*/
int
nMeshCluster2::CountJoints(nRoot* joint, int num)
{
    n_assert(this->njoint2Class);

    nRoot* child;
    for (child = joint->GetHead(); child; child = child->GetSucc())
    {
        if (child->IsInstanceOf(this->njoint2Class))
        {
            num++;
            num = this->CountJoints(child, num);
        }
    }
    return num;
}

//------------------------------------------------------------------------------
/**
    Recursively add joints to joint palette.
*/
int
nMeshCluster2::CollectJoints(nRoot* joint, int curJointIndex)
{
    n_assert(this->njoint2Class);

    nRoot* child;
    for (child = joint->GetHead(); child; child = child->GetSucc())
    {
        if (child->IsInstanceOf(this->njoint2Class))
        {
            this->jointPalette[curJointIndex++] = (nJoint2*) child;
            curJointIndex = this->CollectJoints(child, curJointIndex);
        }
    }
    return curJointIndex;
}

//------------------------------------------------------------------------------
/**
    Build the joint palette by recursively scanning one level below the
    root joint. 
*/
void
nMeshCluster2::BuildJointPalette()
{
    n_assert(this->refBones.isvalid());
    
    // get njoint2 class handle
    if (0 == this->njoint2Class)
    {
        this->njoint2Class = kernelServer->FindClass("njoint2");
        n_assert(this->njoint2Class);
    }

    // count joints
    this->numJoints = this->CountJoints(this->refBones.get(), 0);

    // allocate joint palette
    this->jointPalette = new nJoint2*[this->numJoints];

    // fill joint palette
    this->CollectJoints(this->refBones.get(), 0);

    // clear dirty flag
    this->jointsDirty = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nMeshCluster2::SetRootJoint(const char* n)
{
    n_assert(n);
    this->refBones = n;
    this->BuildJointPalette();
}

//------------------------------------------------------------------------------
/**
*/
bool
nMeshCluster2::Attach(nSceneGraph2* sceneGraph)
{
    if (nVisNode::Attach(sceneGraph))
    {
        if (this->dynVBuf.GetReadOnly())
        {
            // in readonly mode, attach as mesh node
            sceneGraph->AttachMeshNode(this);
        }
        else
        {
            // render mode: attach as complex visual node
            sceneGraph->AttachVisualNode(this);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Evaluates and renders the skin.
*/
void
nMeshCluster2::RenderSkin(nSceneGraph2* sceneGraph, nVertexBuffer* srcSkin)
{
    n_assert(sceneGraph && srcSkin);
    nIndexBuffer* indexBuffer = sceneGraph->GetIndexBuffer();
    n_assert(indexBuffer);

    // get render vertex buffer and lock for writing
    nVertexBuffer* dstSkin = this->dynVBuf.Begin(
        indexBuffer,
        sceneGraph->GetPixelShader(),
        sceneGraph->GetTextureArray());
    srcSkin->LockVertices();

    // get src and dst vertex component pointers
    float* srcCoord = srcSkin->coord_ptr;
    float* dstCoord = dstSkin->coord_ptr;
    float* srcNorm  = srcSkin->norm_ptr;
    float* dstNorm  = dstSkin->norm_ptr;
    ulong* srcColor = srcSkin->color_ptr;
    ulong* dstColor = dstSkin->color_ptr;

    // we assume that the source vertex buffer has at least coords and normals
    n_assert(srcCoord && srcNorm);

    float *srcUV[N_MAXNUM_TEXCOORDS];
    float *dstUV[N_MAXNUM_TEXCOORDS];
    int i;
    int numTexCoordSets = 0;
    for (i = 0; i < N_MAXNUM_TEXCOORDS; i++) 
    {
        srcUV[i] = srcSkin->uv_ptr[i];
        dstUV[i] = dstSkin->uv_ptr[i];
        if (srcUV[i])
        {
            numTexCoordSets++;
        }
        else break;
    }
    float *srcWgt = srcSkin->w_ptr;
    int *srcJoint = srcSkin->j_ptr;

    int numVertices  = srcSkin->GetNumVertices();
    int srcStride = srcSkin->stride4;
    int dstStride = dstSkin->stride4;

    // shadow caster coordinate pointer
    vector3* shadowCoordPtr = 0;
    if (this->refShadowCaster.isvalid() && this->castShadow)
    {
        this->refShadowCaster->Lock();
        shadowCoordPtr = this->refShadowCaster->GetCoordPtr();
    }

    // for each vertex... (-> important to write target buffer continously!)
    vector3 v0,v1,n0,n1;
    for (i = 0; i < numVertices; i++)
    {
        // read source coordinate and normal
        v0.set(srcCoord[0], srcCoord[1], srcCoord[2]);
        n0.set(srcNorm[0], srcNorm[1], srcNorm[2]);
        srcCoord += srcStride;
        srcNorm  += srcStride;

        // compute weighted coord and normal
        int j;
        v1.set(0.0f,0.0f,0.0f);
        n1.set(0.0f,0.0f,0.0f);
        for (j = 0; j < 4; j++)
        {
            float w = srcWgt[j];
            if (w > 0.0f)
            {
                nJoint2* joint = this->jointPalette[srcJoint[j]];
                v1 += (joint->GetSkinMatrix44() * v0) * w;
                n1 += (joint->GetSkinMatrix33() * n0) * w;
            }
        }
        srcJoint += srcStride;
        srcWgt   += srcStride;

        // write dst coord
        dstCoord[0] = v1.x;
        dstCoord[1] = v1.y;
        dstCoord[2] = v1.z;
        dstCoord += dstStride;

        // write dst norm
        dstNorm[0] = n1.x;
        dstNorm[1] = n1.y;
        dstNorm[2] = n1.z;
        dstNorm += dstStride;

        // update shadow caster coordinate
        if (shadowCoordPtr)
        {
            shadowCoordPtr->set(v1);
            shadowCoordPtr++;
        }

        // copy color and uv coordinates if necessary
        if (srcColor) 
        {
            dstColor[0] = srcColor[0];
            srcColor += srcStride;
            dstColor += dstStride;
        }
        int k;
        for (k = 0; k < numTexCoordSets; k++) 
        {
            dstUV[k][0] = srcUV[k][0];
            dstUV[k][1] = srcUV[k][1];
            srcUV[k] += srcStride;
            dstUV[k] += dstStride;
        }
    }

    // instead of keeping track of the exact bounding box,
    // we create a very conservative bounding box (twice
    // the diameter of the original bounding box)
    vector3 bbdiam = srcSkin->GetBBox().vmax - srcSkin->GetBBox().vmin;
    float l = 2.0f * bbdiam.len();
    vector3 vmin(-l, -l, -l);
    vector3 vmax(+l, +l, +l);
    bbox3 bbox(vmin, vmax);
    dstSkin->SetBBox(bbox);

    // unlock and finish
    if (shadowCoordPtr)
    {
        this->refShadowCaster->Unlock();
        this->refShadowServer->AttachCaster(sceneGraph->GetTransform(), this->refShadowCaster.get());
    }
    srcSkin->UnlockVertices();
    this->dynVBuf.End(numVertices, indexBuffer->GetNumIndices());

    // if readonly, hand vertex buffer up to whoever wants it
    if (this->dynVBuf.GetReadOnly())
    {
        sceneGraph->SetVertexBuffer(dstSkin);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nMeshCluster2::Compute(nSceneGraph2* sceneGraph)
{
    nVisNode::Compute(sceneGraph);

    // invoke Compute() on skin mesh and get pointer to source vertex buffer
    this->refSkin->Compute(sceneGraph);
    nVertexBuffer* srcSkin = sceneGraph->GetVertexBuffer();
    n_assert(srcSkin);
    sceneGraph->SetVertexBuffer(0);

    // initialize dynamic vertex buffer if not yet happened
    if (!this->dynVBuf.IsValid())
    {
        // the destination vertex type is the same as the
        // source vertex type minus any weights and joint indices
        int srcVType = srcSkin->GetVertexType();
        n_assert(srcVType & N_VT_JW);
        int dstVType = srcVType & ~N_VT_JW;
        this->dynVBuf.Initialize(dstVType, srcSkin->GetNumVertices());
    
        // if shadow casting enabled, also initialize a shadow caster
        // (NOTE: the source mesh must be a shadow caster as well!)
        nShadowCaster* srcCaster = sceneGraph->GetShadowCaster();
        if (this->castShadow && srcCaster)
        {
            this->refShadowCaster = this->refShadowServer->NewCaster(0);
            this->refShadowCaster->Initialize(srcCaster->GetNumCoords(), srcCaster->GetNumEdges());
            this->refShadowCaster->CopyEdges(srcCaster, 0, srcCaster->GetNumEdges());
        }
    }
    sceneGraph->SetShadowCaster(0);

    // make sure dest vertex buffer is large enough
    n_assert(srcSkin->GetNumVertices() <= this->dynVBuf.GetNumVertices());

    // compute current state of joint skeleton
//    this->refBones->Compute(sceneGraph);

    // render the skin
    this->RenderSkin(sceneGraph, srcSkin);
}

//------------------------------------------------------------------------------
