#define N_IMPLEMENTS nCharacterServer
//------------------------------------------------------------------------------
//  ncharskinrenderer.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "character/ncharskinrenderer.h"
#include "shadow/nshadowcaster.h"
#include "shadow/nshadowserver.h"
#include "character/ncharskeleton.h"
#include "character/ncharjoint.h"

//------------------------------------------------------------------------------
/**
*/
nCharSkinRenderer::nCharSkinRenderer(nKernelServer* kernelServer, nRoot* owner) :
    dynVBuf(kernelServer, owner)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nCharSkinRenderer::~nCharSkinRenderer()
{
    if (this->refShadowCaster.isvalid())
    {
        this->refShadowCaster->Release();
        this->refShadowCaster.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCharSkinRenderer::Initialize(nShadowServer* shadowServer,
                              nVertexBuffer* srcVertexBuffer,
                              nShadowCaster* srcShadowCaster,
                              bool readOnly)
{
    n_assert(!this->IsValid());
    n_assert(shadowServer);
    n_assert(srcVertexBuffer);

    // release old shadow caster if exists
    if (this->refShadowCaster.isvalid())
    {
        this->refShadowCaster->Release();
        this->refShadowCaster.invalidate();
    }

    // initialize the dynamic vertex buffer
    int vertexType = srcVertexBuffer->GetVertexType();
    n_assert(vertexType & N_VT_JW);
    vertexType &= ~N_VT_JW;
    int numVertices = srcVertexBuffer->GetNumVertices();
    this->dynVBuf.SetReadOnly(readOnly);
    this->dynVBuf.Initialize(vertexType, numVertices);

    // clone optional shadow caster
    if (srcShadowCaster)
    {
        int numCoords = srcShadowCaster->GetNumCoords();
        int numEdges  = srcShadowCaster->GetNumEdges();
        this->refShadowCaster = shadowServer->NewCaster(0);
        this->refShadowCaster->Initialize(numCoords, numEdges);
        this->refShadowCaster->CopyEdges(srcShadowCaster, 0, numEdges);
    }

    // make sure dest vertex buffer is large enough
    n_assert(srcVertexBuffer->GetNumVertices() <= this->dynVBuf.GetNumVertices());
}

//------------------------------------------------------------------------------
/**
*/
bool
nCharSkinRenderer::IsValid()
{
    return this->dynVBuf.IsValid();
}

//------------------------------------------------------------------------------
/**
*/
void
nCharSkinRenderer::Render(const nCharSkeleton* charSkeleton, 
                          nIndexBuffer* indexBuffer, 
                          nVertexBuffer* srcSkin,
                          nPixelShader* pixelShader,
                          nTextureArray* textureArray)
{
    n_assert(charSkeleton && indexBuffer && srcSkin);
    n_assert(this->dynVBuf.IsValid());

    // prepare dynamic vertex buffer for writing
    nVertexBuffer* dstSkin = this->dynVBuf.Begin(indexBuffer, pixelShader, textureArray);
    
    // lock source vertex buffer for reading
    srcSkin->LockVertices();

    // get src and dst vertex component pointers
    float* srcCoord = srcSkin->coord_ptr;
    float* dstCoord = dstSkin->coord_ptr;
    float* srcNorm  = srcSkin->norm_ptr;
    float* dstNorm  = dstSkin->norm_ptr;
    ulong* srcColor = srcSkin->color_ptr;
    ulong* dstColor = dstSkin->color_ptr;
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

    // we assume that the source vertex buffer has at least coords and normals
    n_assert(srcCoord && srcNorm);

    // shadow caster coordinate pointer
    vector3* shadowCoordPtr = 0;
    if (this->refShadowCaster.isvalid())
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
                const nCharJoint& joint = charSkeleton->GetJoint(srcJoint[j]);
                v1 += (joint.GetSkinMatrix44() * v0) * w;
                n1 += (joint.GetSkinMatrix33() * n0) * w;
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
    }
    srcSkin->UnlockVertices();
    this->dynVBuf.End(numVertices, indexBuffer->GetNumIndices());
}

//------------------------------------------------------------------------------
/**
*/
nShadowCaster*
nCharSkinRenderer::GetShadowCaster() const
{
    if (this->refShadowCaster.isvalid())
    {
        return this->refShadowCaster.get();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
