#define N_IMPLEMENTS nMeshEffect
//------------------------------------------------------------------------------
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "node/nmesheffect.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelcontext.h"
#include "node/nmeshnode.h"

nNebulaScriptClass(nMeshEffect, "nanimnode");

//------------------------------------------------------------------------------
/**
*/
nMeshEffect::nMeshEffect() : 
	refMesh(kernelServer, this),
	dynVBuf(kernelServer, this)
{
		lasttime = 0.0f;

		amplitude = 1.0f;
		frequency = 1.0f;

		twinktime = 1.0f;
        twink = false;
}

//------------------------------------------------------------------------------
/**
*/
nMeshEffect::~nMeshEffect()
{

}

//------------------------------------------------------------------------------
/**
*/
bool
nMeshEffect::Attach(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    if (nVisNode::Attach(sceneGraph))
    {
        // don't attach to scene graph if we are a read only mesh
        if (!this->GetReadOnly())
        {
            sceneGraph->AttachVisualNode(this);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
#ifdef __VC__
#pragma optimize("agt", on)
#endif

void
nMeshEffect::render(nSceneGraph2* sceneGraph, nVertexBuffer* vbSrc, float time)
{
    n_assert(sceneGraph && vbSrc);
    nIndexBuffer* indexBuffer = sceneGraph->GetIndexBuffer();
    n_assert(indexBuffer);

    // get target vertex buffer
    nVertexBuffer* vbDst = this->dynVBuf.Begin(
        indexBuffer,
        sceneGraph->GetPixelShader(),
        sceneGraph->GetTextureArray());

    // lock source buffer for reading
    vbSrc->LockVertices();

    // get source and destination vertex component pointers
    float* srcCoord = vbSrc->coord_ptr;
    float* dstCoord = vbDst->coord_ptr;
    float* srcNorm  = vbSrc->norm_ptr;
    float* dstNorm  = vbDst->norm_ptr;
    ulong* srcColor = vbSrc->color_ptr;
    ulong* dstColor = vbDst->color_ptr;
    n_assert(srcCoord);

    float *srcUv[N_MAXNUM_TEXCOORDS];
    float *dstUv[N_MAXNUM_TEXCOORDS];
    int i;
    int numTexCoordSets = 0;
    for (i = 0; i < N_MAXNUM_TEXCOORDS; i++) 
    {
        srcUv[i] = vbSrc->uv_ptr[i];
        dstUv[i] = vbDst->uv_ptr[i];
        if (srcUv[i])
        {
            numTexCoordSets++;
        }
        else break;
    }

    int numVertices = vbSrc->GetNumVertices();
    int srcStride   = vbSrc->stride4;
    int dstStride   = vbDst->stride4;

	// ready for zucking ?
	int temp = 0;

	if (twink)
	{
		int temptime = int(time * 1000000000);
		if ( (temptime % 150) == 0)
		{
			// temp value for different positions at twinking
			temp = rand()%10 + 1;
			if (temp > 5)
				temp = 5 - temp;

			lasttime = time;
		}
	}

    // process each vertex...
    for (i = 0; i < numVertices; i++)
    {
		float x = float(sin(srcCoord[1] + (time * frequency))) * amplitude;

		dstCoord[0] = srcCoord[0] + x + temp;
		dstCoord[1] = srcCoord[1] + temp;
		dstCoord[2] = srcCoord[2] + temp;

		srcCoord += srcStride;
        dstCoord += dstStride;

        // copy normal
        if (srcNorm)
        {
            dstNorm[0] = srcNorm[0];  
            dstNorm[1] = srcNorm[1];  
            dstNorm[2] = srcNorm[2];
            dstNorm += dstStride;
            srcNorm += srcStride;
        }

        // copy color
        if (srcColor)
        {
            dstColor[0] = srcColor[0];
            dstColor += dstStride;
            srcColor += srcStride;
        }

        // copy tex coords
        int k;
        for (k = 0; k < numTexCoordSets; k++)
        {
            dstUv[k][0] = srcUv[k][0];
            dstUv[k][1] = srcUv[k][1];
            srcUv[k] += srcStride;
            dstUv[k] += dstStride;
        }
    }

    // update target bounding box
    vbDst->SetBBox(vbSrc->GetBBox());

    // unlock and finish
    vbSrc->UnlockVertices();
    this->dynVBuf.End(numVertices, indexBuffer->GetNumIndices());

    // if readonly, hand vertex buffer up to whoever wants it
    if (this->dynVBuf.GetReadOnly())
    {
        sceneGraph->SetVertexBuffer(vbDst);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nMeshEffect::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    nVisNode::Compute(sceneGraph);

    // invoke compute on source mesh
    this->refMesh->Compute(sceneGraph);
    nVertexBuffer* vertexBuffer = sceneGraph->GetVertexBuffer();
    n_assert(vertexBuffer);
    sceneGraph->SetVertexBuffer(0);

    // initialize dynamic vertex buffer on demand
    if (!this->dynVBuf.IsValid())
    {
        this->dynVBuf.Initialize(vertexBuffer->GetVertexType(), vertexBuffer->GetNumVertices());
    }

    // make sure that the target vertex buffer is great enough
    // (HACK: this may be a problem if the size of the source skin
    // changes (for instance for things like lod), so in this case
    // it would actually be better to have one meshcluster for each
    // lod)
    n_assert(vertexBuffer->GetNumVertices() <= this->dynVBuf.GetNumVertices());

    // get current anim channel value
    nChannelContext* chnContext = sceneGraph->GetChannelContext();
    n_assert(chnContext);
    float t = chnContext->GetChannel1f(this->localChannelIndex);

    // fill the dynamic vertex buffer
    this->render(sceneGraph, vertexBuffer, t);
}

//------------------------------------------------------------------------------

