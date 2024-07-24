#define N_IMPLEMENTS nMeshMixer
//-------------------------------------------------------------------
//  nmeshmixer_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "gfx/ngfxtypes.h"
#include "gfx/nvertexbuffer.h"
#include "gfx/nindexbuffer.h"
#include "gfx/nchannelset.h"
#include "gfx/nchannelcontext.h"
#include "gfx/nscenegraph2.h"
#include "node/nmeshmixer.h"
#include "node/nmeshnode.h"

nNebulaScriptClass(nMeshMixer, "nvisnode");

//-------------------------------------------------------------------
//  ~nMeshMixer()
//  27-Sep-00   floh    created
//-------------------------------------------------------------------
nMeshMixer::~nMeshMixer()
{ }

//-------------------------------------------------------------------
//	AttachChannels()
//	Attach the weight channels to the channel set.
//	25-Jun-01	floh	created
//-------------------------------------------------------------------
void nMeshMixer::AttachChannels(nChannelSet* chnSet)
{
	n_assert(chnSet);
	int i;
	for (i = 0; i < this->num_sources; i++)
	{
		nMixerSource* curSource = &(this->sources[i]);
		int chnIndex = chnSet->AddChannel(this->refChannel.get(), curSource->GetChannelName());
		curSource->SetChannelIndex(chnIndex);

        // ask source to attach its channels as well
        curSource->GetSource()->AttachChannels(chnSet);
	}
	nVisNode::AttachChannels(chnSet);
}

//-------------------------------------------------------------------
//  mix()
//  27-Sep-00   floh    created
//-------------------------------------------------------------------
void nMeshMixer::mix(nVertexBuffer **vb_src,
                     float *w,
                     int num,
					 nSceneGraph2* sceneGraph)
{
    n_assert(vb_src && w && sceneGraph);

	// get index buffer 
	nIndexBuffer* indexBuffer = sceneGraph->GetIndexBuffer();
	n_assert(indexBuffer);

    // get destination vertex buffer
    nVertexBuffer *vb_dest = this->dyn_vb.Begin(
		indexBuffer,
		sceneGraph->GetPixelShader(),
		sceneGraph->GetTextureArray());

    // get pointers to source vertex components
    float *coord[N_MAXSOURCES+1];
    float *norm[N_MAXSOURCES+1];
    ulong *color[N_MAXSOURCES+1];
    float *uv[N_MAXNUM_TEXCOORDS][N_MAXSOURCES+1];
    int i,j,k;
    for (i=0; i<num; i++) 
	{
        vb_src[i]->LockVertices();
        coord[i] = vb_src[i]->coord_ptr;
        norm[i]  = vb_src[i]->norm_ptr;
        color[i] = vb_src[i]->color_ptr;
        for (j=0; j<N_MAXNUM_TEXCOORDS; j++) {
            uv[j][i] = vb_src[i]->uv_ptr[j];
        }
    }
    coord[i] = vb_dest->coord_ptr;
    norm[i]  = vb_dest->norm_ptr;
    color[i] = vb_dest->color_ptr;
    for (j=0; j<N_MAXNUM_TEXCOORDS; j++) 
	{
        uv[j][i] = vb_dest->uv_ptr[j];
    }

    // get update bools
    bool do_coord = this->GetUpdateCoord();
    bool do_norm  = this->GetUpdateNorm();
    bool do_color = this->GetUpdateColor();
    bool do_uv[N_MAXNUM_TEXCOORDS];
    do_uv[0] = this->GetUpdateUv0();
    do_uv[1] = this->GetUpdateUv1();
    do_uv[2] = this->GetUpdateUv2();
    do_uv[3] = this->GetUpdateUv3();

    // get 16 bit fixed point weight values for color lerping
    ushort usweight[N_MAXSOURCES];
    for (i=0; i<num; i++) 
	{
        usweight[i] = (short) (256.0f * w[i]);
    }

    // the destination buffer must be written continously, because
    // it may be placed in uncached memory (AGP or video mem)
    // it may actually be better to keep coords, norms, etc 
    // in separate arrays -> DX8!
    int num_v = vb_src[0]->GetNumVertices();
    int dest = num;
    int src_stride  = vb_src[0]->stride4;
    int dest_stride = vb_dest->stride4;
    for (i=0; i<num_v; i++) 
	{
        if (coord[0]) 
		{
            if (do_coord) 
			{
                register float x = coord[0][0] * w[0];
                register float y = coord[0][1] * w[0];
                register float z = coord[0][2] * w[0];
                for (j=1; j<num; j++) 
				{
                    x += coord[j][0] * w[j];
                    y += coord[j][1] * w[j];
                    z += coord[j][2] * w[j];
                }
                coord[dest][0] = x;
                coord[dest][1] = y;
                coord[dest][2] = z;
            } 
			else 
			{
                coord[dest][0] = coord[0][0];
                coord[dest][1] = coord[0][1];
                coord[dest][2] = coord[0][2];
            }
            for (j=0; j<num; j++) 
			{
                coord[j] += src_stride;
            }
            coord[dest] += dest_stride;
        }

        if (norm[0]) 
		{
            if (do_norm) 
			{
                register float x = norm[0][0] * w[0];
                register float y = norm[0][1] * w[0];
                register float z = norm[0][2] * w[0];
                for (j=1; j<num; j++) 
				{
                    x += norm[j][0] * w[j];
                    y += norm[j][1] * w[j];
                    z += norm[j][2] * w[j];
                }
                norm[dest][0] = x;
                norm[dest][1] = y;
                norm[dest][2] = z;
            } 
			else 
			{
                norm[dest][0] = norm[0][0];
                norm[dest][1] = norm[0][1];
                norm[dest][2] = norm[0][2];
            }
            for (j=0; j<num; j++) 
			{
                norm[j] += src_stride;
            }
            norm[dest] += dest_stride;
        }

        if (color[0]) 
		{
            if (do_color) 
			{
                ulong c = color[0][0];
                ushort r = (ushort((c>>24) & 0xff) * usweight[0]) >> 8;
                ushort g = (ushort((c>>16) & 0xff) * usweight[0]) >> 8;
                ushort b = (ushort((c>>8)  & 0xff) * usweight[0]) >> 8;
                ushort a = (ushort(c       & 0xff) * usweight[0]) >> 8;
                for (j=1; j<num; j++) 
				{
                    c = color[j][0];
                    r += (ushort((c>>24) & 0xff) * usweight[j]) >> 8;
                    g += (ushort((c>>16) & 0xff) * usweight[j]) >> 8;
                    b += (ushort((c>>8)  & 0xff) * usweight[j]) >> 8;
                    a += (ushort(c       & 0xff) * usweight[j]) >> 8;
                }
                r&=0xff; g&=0xff; b&=0xff; a&=0xff;
                color[dest][0] = (r<<24)|(g<<16)|(b<<8)|(a);
            } 
			else 
			{
                color[dest][0] = color[0][0];
            }
            for (j=0; j<num; j++) 
			{
                color[j] += src_stride;
            }
            color[dest] += dest_stride;
        }

        for (k=0; k<N_MAXNUM_TEXCOORDS; k++) 
		{
            if (uv[k][0]) 
			{
                if (do_uv[k]) 
				{
                    register float u = uv[k][0][0] * w[0];
                    register float v = uv[k][0][1] * w[0];
                    for (j=1; j<num; j++) {
                        u += uv[k][j][0] * w[j];
                        v += uv[k][j][1] * w[j];
                    }
                    uv[k][dest][0] = u;
                    uv[k][dest][1] = v;
                } 
				else 
				{
                    uv[k][dest][0] = uv[k][0][0];
                    uv[k][dest][1] = uv[k][0][1];
                }
                for (j=0; j<num; j++) 
				{
                    uv[k][j] += src_stride;
                }
                uv[k][dest] += dest_stride;
            }
        }
    }

    // weight bounding box
    const bbox3 *src_bb = &(vb_src[0]->GetBBox());
    bbox3 dest_bb;
    dest_bb.vmin = src_bb->vmin * w[0];
    dest_bb.vmax = src_bb->vmax * w[0];
    for (j=1; j<num; j++) 
	{
        src_bb = &(vb_src[j]->GetBBox());
        dest_bb.vmin += src_bb->vmin * w[j];
        dest_bb.vmax += src_bb->vmax * w[j];
    }
    vb_dest->SetBBox(dest_bb);

    // unlock everything
    for (j=0; j<num; j++) 
	{
        vb_src[j]->UnlockVertices();
    }
    this->dyn_vb.End(num_v, indexBuffer->GetNumIndices());

    // if readonly, hand 
    if (this->dyn_vb.GetReadOnly())
	{
		sceneGraph->SetVertexBuffer(vb_dest);
	}
    else
	{
		sceneGraph->SetVertexBuffer(0);
	}
}

//-------------------------------------------------------------------
//  Attach()
//  17-Oct-00   floh    created
//	25-Jun-01	floh	new scene graph stuff
//-------------------------------------------------------------------
bool nMeshMixer::Attach(nSceneGraph2* sceneGraph)
{
	n_assert(sceneGraph);
    if (nVisNode::Attach(sceneGraph)) 
	{
		// don't attach to scene if we are a readonly mesh
		if (!this->dyn_vb.GetReadOnly())
		{
			sceneGraph->AttachVisualNode(this);
		}
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
//  Compute()
//  27-Sep-00   floh    created
//  17-Oct-00   floh    + can now optionally read weights directly
//                        from channels
//  18-Oct-00   floh    + no longer derived from nDynMeshNode, instead
//                        uses embedded nDynVertexBuffer object
//  01-Dec-00   floh    + normalization code was wrong
//                        thanx to Benedek Balazs for the fix.
//-------------------------------------------------------------------
void nMeshMixer::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);
	
	// hand up Compute(), so that our depend nodes are triggered
	nVisNode::Compute(sceneGraph);

	// get current channel context
	nChannelContext* chnContext = sceneGraph->GetChannelContext();
	n_assert(chnContext);

	// read weights and invoke Compute() on those sources whose
	// weights are greater 0.0
    float weights[N_MAXSOURCES];
    nVertexBuffer *vxbufs[N_MAXSOURCES];
    int num = 0;
    float w_sum = 0.0f;
    int i;
	for (i = 0; i < this->num_sources; i++) 
	{
        nMixerSource* curSource = &(this->sources[i]);
		float w = chnContext->GetChannel1f(curSource->GetChannelIndex());
		if (w > 0.0f)
		{
			// store weight
			weights[num] = w;
			w_sum += w;

			// invoke compute on source object
			nVisNode* obj = curSource->GetSource();
			obj->Compute(sceneGraph);
			n_assert(obj);
			
			// get vertex buffer of source object
			vxbufs[num] = sceneGraph->GetVertexBuffer();
		    n_assert(vxbufs[num]);
			sceneGraph->SetVertexBuffer(0);

			num++;
		}
	}

    if (num > 0) {

        // allocate writeonly vbuffers on demand...
        if (!this->dyn_vb.IsValid()) 
		{
            this->dyn_vb.Initialize(vxbufs[0]->GetVertexType(),vxbufs[0]->GetNumVertices());
            
            // make sure that the vbuffers we get are bigger
            // then the source buffers, that's a compromise
            // we make to not make the code overly difficult
            if (vxbufs[0]->GetNumVertices() > this->dyn_vb.GetNumVertices()) 
			{
                n_error("ERROR: source vertex buffers are greater then target vertex buffer!\n");
            }
        }

        // normalize weights if sum of weight is different from one
        if ((w_sum>=TINY) && (fabs(1.0f-w_sum)>TINY) && this->GetNormalize()) 
		{
            for (i=0; i<num; i++) weights[i] /= w_sum;
        }

        // generate weighted target vbuffer
        this->mix(vxbufs, weights, num, sceneGraph);
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
