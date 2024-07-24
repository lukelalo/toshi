#define N_IMPLEMENTS nOverlayPlane
//-------------------------------------------------------------------
//  novrlay_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "gfx/ngfxserver.h"
#include "gfx/nscenegraph2.h"
#include "node/noverlayplane.h"
#include "misc/nspecialfxserver.h"

nNebulaScriptClass(nOverlayPlane, "nvisnode");

//-------------------------------------------------------------------
//  ~nOverlayPlane()
//  09-May-00   floh    created
//-------------------------------------------------------------------
nOverlayPlane::~nOverlayPlane()
{
    if (this->ref_ibuf.isvalid()) 
    {
        this->ref_ibuf->Release();
        this->ref_ibuf.invalidate();
    };
}

//-------------------------------------------------------------------
//  init_vbuffer()
//  09-May-00   floh    created
//  18-Oct-00   floh    + extended to work with ndynvertexbuffer
//-------------------------------------------------------------------
void nOverlayPlane::init_vbuffer(void)
{
    n_assert(!this->ref_ibuf.isvalid());

    // initialize dynamic vertex buffer
    this->dyn_vb.Initialize((N_VT_COORD|N_VT_RGBA|N_VT_UV0),0);

    // make sure the dynamic vertex buffer is big enough
    n_assert(this->dyn_vb.GetNumVertices() >= 4);

    // initialize index buffer
    nIndexBuffer *ibuf = this->ref_gs->FindIndexBuffer("noverlayplane_ibuf");
    if (!ibuf) {
        ibuf = this->ref_gs->NewIndexBuffer("noverlayplane_ibuf");
        ibuf->Begin(N_IBTYPE_STATIC, N_PTYPE_TRIANGLE_LIST, 6);
        ibuf->Index(0, 0);
        ibuf->Index(1, 1);
        ibuf->Index(2, 2);
        ibuf->Index(3, 2);
        ibuf->Index(4, 3);
        ibuf->Index(5, 0);
        ibuf->End();
    }
    n_assert(ibuf);
    this->ref_ibuf = ibuf;
}

//-------------------------------------------------------------------
//  Attach()
//  09-May-00   floh    created
//  18-Jun-01   floh    changed for new scene graph stuff
//-------------------------------------------------------------------
bool 
nOverlayPlane::Attach(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);
    if (nVisNode::Attach(sceneGraph)) 
    {
        sceneGraph->AttachVisualNode(this);
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
//  Compute()
//  09-May-00   floh    created
//  18-Jun-01   floh    changed for new scene graph stuff
//-------------------------------------------------------------------
void
nOverlayPlane::Compute(nSceneGraph2* sceneGraph)
{
    nVisNode::Compute(sceneGraph);  
    
    nGfxServer *gs = this->ref_gs.get();                

    // demand-initialize dynamic vertex buffer
    if (!this->dyn_vb.IsValid()) 
    {
        this->init_vbuffer();
    }

    // get the current overlay color from specialfx server
    float r,g,b,a;
    this->ref_sfx->GetOverlay(r,g,b,a);

    // add internal color (more or less a hack)
    r += color.x;
    g += color.y;
    b += color.z;
    a += color.w;

    // get ulong-color
    ulong c;
    if (this->dyn_vb.GetColorFormat() == N_COLOR_RGBA) 
    {
        c = n_f2rgba(r,g,b,a);
    } 
    else 
    {
        c = n_f2bgra(r,g,b,a);
    }

    if (c != 0) {

        // set modelview and projection matrix to identity
        gs->PushMatrix(N_MXM_MODELVIEW);
        gs->PushMatrix(N_MXM_PROJECTION);
        gs->SetMatrix(N_MXM_MODELVIEW,this->identity);
        gs->SetMatrix(N_MXM_PROJECTION,this->identity);

        // fill the dynamic vertex buffer (continously!)
        nVertexBuffer *vb = this->dyn_vb.Begin(
            this->ref_ibuf.get(),
            sceneGraph->GetPixelShader(),
            sceneGraph->GetTextureArray());

        float *coord = vb->coord_ptr;
        ulong *color = vb->color_ptr;
        float *uv    = vb->uv_ptr[0];
        int s = vb->stride4;

        // topleft
        coord[0]=-1.0f; coord[1]=-1.0f; coord[2]=0.0f; coord+=s;
        color[0]=c; color+=s;
        uv[0]=0.0f; uv[1]=0.0f; uv+=s;

        // topright
        coord[0]=+1.0f; coord[1]=-1.0f; coord[2]=0.0f; coord+=s;
        color[0]=c; color+=s;
        uv[0]=1.0f; uv[1]=0.0f; uv+=s;
        
        // bottomright
        coord[0]=+1.0f; coord[1]=+1.0f; coord[2]=0.0f; coord+=s;
        color[0]=c; color+=s;
        uv[0]=1.0f; uv[1]=1.0f; uv+=s;

        // bottomleft
        coord[0]=-1.0f; coord[1]=+1.0f; coord[2]=0.0f; coord+=s;
        color[0]=c; color+=s;
        uv[0]=0.0f; uv[1]=1.0f; uv+=s;

        this->dyn_vb.End(4,6);

        gs->PopMatrix(N_MXM_MODELVIEW);
        gs->PopMatrix(N_MXM_PROJECTION);
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
