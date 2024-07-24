#define N_IMPLEMENTS nMeshIpol
//------------------------------------------------------------------------------
//  nmeshipol_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//------------------------------------------------------------------------------
#include "node/nmeshipol.h"
#include "node/nmeshnode.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nvertexbuffer.h"
#include "gfx/nindexbuffer.h"
#include "gfx/nchannelcontext.h"
#include "mathlib/bbox.h"

//------------------------------------------------------------------------------
/**
*/
nMeshIpol::~nMeshIpol()
{
    if (this->key_array) 
    {
        delete[] this->key_array;
        this->key_array = NULL;
    }
}

//------------------------------------------------------------------------------
/**
    Interpolate between 2 source vertex buffer, render result to target
    vertex buffer. 

    @param  sceneGraph  pointer to scene graph object
    @param  l           lerp value (between 0.0 and 1.0)
    @param  vb0         first source vertex buffer
    @param  vb1         second source vertex buffer
*/  
void 
nMeshIpol::interpolate(nSceneGraph2* sceneGraph, float l, nVertexBuffer *vb0, nVertexBuffer *vb1)
{
    n_assert(sceneGraph && vb0 && vb1);

    // get current index buffer (this is the index buffer from
    // one of the source meshes, all index buffer should be 
    // identical anyway!)
    nIndexBuffer* indexBuffer = sceneGraph->GetIndexBuffer();
    n_assert(indexBuffer);

    // get destination vertex buffer
    nVertexBuffer *vb2 = this->dyn_vb.Begin(
        indexBuffer,
        sceneGraph->GetPixelShader(),
        sceneGraph->GetTextureArray());

    // lock source buffers to gain read access
    vb0->LockVertices();
    vb1->LockVertices();

    // get the required pointers
    float *coord[3];
    float *norm[3];
    ulong *color[3];
    float *uv[N_MAXNUM_TEXCOORDS][3];
    coord[0]=vb0->coord_ptr; coord[1]=vb1->coord_ptr; coord[2]=vb2->coord_ptr;
    norm[0] =vb0->norm_ptr;  norm[1] =vb1->norm_ptr;  norm[2] =vb2->norm_ptr;
    color[0]=vb0->color_ptr; color[1]=vb1->color_ptr; color[2]=vb2->color_ptr;
    int i;
    for (i=0; i<N_MAXNUM_TEXCOORDS; i++) 
    {
        uv[i][0]=vb0->uv_ptr[i]; uv[i][1]=vb1->uv_ptr[i]; uv[i][2]=vb2->uv_ptr[i];
    }

    bool do_coord = this->update_flags & N_UPDATE_COORD ? true : false;
    bool do_norm  = this->update_flags & N_UPDATE_NORM  ? true : false;
    bool do_color = this->update_flags & N_UPDATE_RGBA  ? true : false;
    bool do_uv[N_MAXNUM_TEXCOORDS];
    
    do_uv[0] = this->update_flags & N_UPDATE_UV0 ? true : false;
    do_uv[1] = this->update_flags & N_UPDATE_UV1 ? true : false;
    do_uv[2] = this->update_flags & N_UPDATE_UV2 ? true : false;
    do_uv[3] = this->update_flags & N_UPDATE_UV3 ? true : false;
    
    // get 16 bit fixed point lerp value for color lerping
    short uslerp = (short) (256.0f * l);

    // the destination buffer must be written continously, because
    // it may be placed in uncached memory (AGP or video mem)
    // it may actually be better to keep coords, norms, etc 
    // in separate arrays -> DX8!
    int num_v = vb0->GetNumVertices();
    for (i=0; i<num_v; i++) 
    {
        int k;

        // interpolate 3d coordinates
        if (coord[0]) 
        {
            if (do_coord) 
            {
                coord[2][0] = coord[0][0] + ((coord[1][0]-coord[0][0])*l);
                coord[2][1] = coord[0][1] + ((coord[1][1]-coord[0][1])*l);
                coord[2][2] = coord[0][2] + ((coord[1][2]-coord[0][2])*l);
            } 
            else 
            {
                coord[2][0] = coord[0][0];
                coord[2][1] = coord[0][1];
                coord[2][2] = coord[0][2];
            }                
            coord[0] += vb0->stride4;
            coord[1] += vb1->stride4;
            coord[2] += vb2->stride4;
        }

        // interpolate normals
        if (norm[0]) 
        {
            if (do_norm) 
            {
                norm[2][0] = norm[0][0] + ((norm[1][0]-norm[0][0])*l);
                norm[2][1] = norm[0][1] + ((norm[1][1]-norm[0][1])*l);
                norm[2][2] = norm[0][2] + ((norm[1][2]-norm[0][2])*l);
            } 
            else 
            {
                norm[2][0] = norm[0][0];
                norm[2][1] = norm[0][1];
                norm[2][2] = norm[0][2];
            }
            norm[0] += vb0->stride4;
            norm[1] += vb1->stride4;
            norm[2] += vb2->stride4;
        }

        // interpolate vertex colors
        if (color[0]) 
        {
            if (do_color) 
            {
                // FIXME!!! (optimize!)
                ulong c0 = color[0][0];
                ulong c1 = color[1][0];
                short r0 = short((c0>>24) & 0xff);
                short g0 = short((c0>>16) & 0xff);
                short b0 = short((c0>>8)  & 0xff);
                short a0 = short(c0       & 0xff);
                short r1 = short((c1>>24) & 0xff);
                short g1 = short((c1>>16) & 0xff);
                short b1 = short((c1>>8)  & 0xff);
                short a1 = short(c1       & 0xff);
                color[2][0] = (((r0 + (((r1-r0)*uslerp)>>8)) & 0xff)<<24) |
                              (((g0 + (((g1-g0)*uslerp)>>8)) & 0xff)<<16) |
                              (((b0 + (((b1-b0)*uslerp)>>8)) & 0xff)<<8)  |
                              (((a0 + (((a1-a0)*uslerp)>>8)) & 0xff));
            } 
            else 
            {
                color[2][0] = color[0][0];
            }
            color[0] += vb0->stride4;
            color[1] += vb1->stride4;
            color[2] += vb2->stride4;
        }

        // interpolate texture coords
        for (k=0; k<N_MAXNUM_TEXCOORDS; k++) 
        {
            if (uv[k][0]) 
            {
                if (do_uv[k]) 
                {
                    uv[k][2][0] = uv[k][0][0] + ((uv[k][1][0]-uv[k][0][0])*l);
                    uv[k][2][1] = uv[k][0][1] + ((uv[k][1][1]-uv[k][0][1])*l);
                } 
                else 
                {
                    uv[k][2][0] = uv[k][0][0];
                    uv[k][2][1] = uv[k][0][1];
                }
                uv[k][0] += vb0->stride4;
                uv[k][1] += vb1->stride4;
                uv[k][2] += vb2->stride4;
            }
        }
    }

    // lerp bounding box
    const vector3& vb0_min = vb0->GetBBox().vmin;
    const vector3& vb0_max = vb0->GetBBox().vmax;
    const vector3& vb1_min = vb1->GetBBox().vmin;
    const vector3& vb1_max = vb1->GetBBox().vmax;
    vector3 vb2_min = vb0_min + (vb1_min-vb0_min) * l;
    vector3 vb2_max = vb0_max + (vb1_max-vb0_max) * l;
    bbox3 vb2_bbox(vb2_min, vb2_max);
    vb2->SetBBox(vb2_bbox);

    // unlock source vertex buffers, and render dynamic vertex buffer
    vb1->UnlockVertices();
    vb0->UnlockVertices();
    this->dyn_vb.End(num_v, indexBuffer->GetNumIndices());

    // if we are readonly, attach vertex buffer to current scene graph node
    if (this->dyn_vb.GetReadOnly())
    {
        sceneGraph->SetVertexBuffer(vb2);
    }
    else
    {
        sceneGraph->SetVertexBuffer(0);
    }
}

//------------------------------------------------------------------------------
/**
    Attach object to scene if a non-readonly object (a read only object will
    not be visible and only serves as input mesh for other mesh animators).

    @param  sceneGraph      pointer to scene graph object
*/
bool
nMeshIpol::Attach(nSceneGraph2* sceneGraph)
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

//------------------------------------------------------------------------------
/**
    Update myself and render to dynamic mesh, or set as current vertex
    buffer in scene graph if I'm read only (see nMeshNode).

    @param  sceneGraph      pointer to scene graph object

    24-Jul-01   floh    + some corrections to prevent illegal array accesses
*/
void 
nMeshIpol::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    // let my depend nodes update me
    nVisNode::Compute(sceneGraph);
        
    // find the 2 current keyframes
    float tscale = this->scale;
    float min_t = this->key_array[0].t * tscale;
    float max_t = this->key_array[this->num_keys-1].t * tscale;

    if (max_t > 0.0) {
    
        // get current anim channel value
        nChannelContext* chnContext = sceneGraph->GetChannelContext();
        n_assert(chnContext);
        float t = chnContext->GetChannel1f(this->localChannelIndex);

        // handle loop/oneshot behaviour
        if (this->repType == N_REPTYPE_LOOP) 
        {
            // in loop mode, normalize on loop time
            t = t - float(floor(t/max_t)*max_t);
        } 

        // clamp t to valid range
        if      (t < min_t)  t = min_t;
        else if (t >= max_t) t = max_t - 0.0001f;

        // find the right keys
        int i = 0;
        if ((this->key_array[0].t > 0.0f) && (this->repType == N_REPTYPE_LOOP)) 
        {
            char buf[N_MAXPATH];
            n_error("Object '%s' 1st keyframe > 0.0f!\n",this->GetFullName(buf,sizeof(buf)));
        };
        while ((this->key_array[i].t*tscale) <= t) i++;
        n_assert((i > 0) && (i < this->num_keys));
        nMeshIpolKey *k0 = &(this->key_array[i-1]);
        nMeshIpolKey *k1 = &(this->key_array[i]);
        nVertexBuffer *vb0 = NULL;
        nVertexBuffer *vb1 = NULL;

        // invoke Compute() on both candidates and get vertex buffer pointers
        k0->ref_source->Compute(sceneGraph);
        vb0 = sceneGraph->GetVertexBuffer();
        n_assert(vb0);
        n_assert(vb0->GetVBufType() == N_VBTYPE_READONLY);

        k1->ref_source->Compute(sceneGraph);
        vb1 = sceneGraph->GetVertexBuffer();
        n_assert(vb1);
        n_assert(vb1->GetVBufType() == N_VBTYPE_READONLY);

        // clear scene graph vertex buffer after child nodes wrote to it
        sceneGraph->SetVertexBuffer(0);

        // FIXME: make sure both vertex buffers have identical number of vertices
/*
        if (vb0->GetNumVertices() != vb1->GetNumVertices())
        {
            char buf0[N_MAXPATH];
            char buf1[N_MAXPATH];
            n_error("ERROR: nMeshIpol: num vertices differ '%s' and '%s'\n",
                k0->ref_source->GetFullName(buf0, sizeof(buf0)), 
                k1->ref_source->GetFullName(buf1, sizeof(buf1)));
        }
*/

        // make sure both vertex buffers have identical vertex formats
        if (vb0->GetVertexType() != vb1->GetVertexType())
        {
            char buf0[N_MAXPATH];
            char buf1[N_MAXPATH];
            n_error("ERROR: nMeshIpol: vertex formats differ '%s' and '%s'\n",
                k0->ref_source->GetFullName(buf0, sizeof(buf0)), 
                k1->ref_source->GetFullName(buf1, sizeof(buf1)));
        }

        // initialize dynamic vertex buffer if not happened yet
        if (!this->dyn_vb.IsValid()) 
        {
            this->dyn_vb.Initialize(vb0->GetVertexType(), vb0->GetNumVertices());
        
            // make sure that the vbuffers we get are bigger
            // then the source buffers, that's a compromise
            // we make to not make the code overly difficult
            if (vb0->GetNumVertices() > this->dyn_vb.GetNumVertices()) 
            {
                n_error("ERROR: source vertex buffers are greater then target vertex buffer!\n");
            }
        }

        // compute lerp position
        float l;
        float t0 = k0->t * tscale;
        float t1 = k1->t * tscale;
        if (t1 > t0) l = (float) ((t-t0)/(t1-t0));
        else         l = 1.0f;
            
        // generate and render interpolated vertex buffer
        this->interpolate(sceneGraph, l, vb0, vb1);
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
