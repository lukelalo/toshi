#define N_IMPLEMENTS nTerrainNode
//-------------------------------------------------------------------
//  nterrain_render.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/ngfxserver.h"
#include "misc/nquickwatch.h"
#include "terrain/nterrainnode.h"
#include "terrain/quadtree.h"
#include "kernel/ntimeserver.h"
#include "gfx/nvertexbuffer.h"
#include "gfx/nindexbuffer.h"
#include "gfx/nscenegraph2.h"
#include "gfx/npixelshader.h"

//-------------------------------------------------------------------
//  init_indexbuffer()
//  Initialize Index buffer and nDynVertexBuffer stuff.
//  07-Oct-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::init_buffers(void)
{
    n_assert(!this->ref_ibuf.isvalid());
    
    this->dyn_vb.Initialize((N_VT_COORD|N_VT_NORM|N_VT_UV0|N_VT_UV1),0);

    int num_indices = 0; 
    nIndexBuffer *ibuf = this->ref_gs->FindIndexBuffer("nterrainnode_ibuf");
    if (ibuf) {
        num_indices = ibuf->GetNumIndices();
    } else {
        ibuf = this->ref_gs->NewIndexBuffer("nterrainnode_ibuf");
        num_indices = this->dyn_vb.GetNumVertices();
        
        // round down to multiple of 3
        num_indices = (num_indices / 3) * 3;
        ibuf->Begin(N_IBTYPE_WRITEONLY, N_PTYPE_TRIANGLE_LIST, num_indices);
        int i;
        for (i=0; i<num_indices; i++) {
            ibuf->Index(i, i);
        }
        ibuf->End();
    }

    this->num_triangles = num_indices / 3;
    n_assert(this->num_triangles > 0);
     
    n_assert(ibuf);
    this->ref_ibuf = ibuf;
}

//-------------------------------------------------------------------
//  swap_buffers()
//  Render current vbuffer, swap current vbuffer, initialize 
//  new current vbuffer, increment render pass ID.
//  27-Mar-00   floh    created
//  07-Oct-00   floh    rewritten for ndynmeshnode
//  18-Oct-00   floh    + rewritten to ndynvertexbuffer
//-------------------------------------------------------------------
void nTerrainNode::swap_buffers(void)
{
    // render the current vbuffer, swap to other vbuffer
    this->ref_ibuf->Unlock();
    this->cur_vb = this->dyn_vb.Swap(this->cur_vertex,this->cur_triangle*3);
    this->ref_ibuf->Lock();
    
    // prepare for new render pass
    this->render_pass++;
    if (this->render_pass==0) this->render_pass=1;
    this->cur_vertex   = 0;
    this->cur_triangle = 0;
    this->coord = this->cur_vb->coord_ptr;
    this->norm  = this->cur_vb->norm_ptr;
    this->stride4 = this->cur_vb->stride4;
    int i;
    for (i=0; i<N_MAXNUM_TEXCOORDS; i++) {
        this->uv[i] = this->cur_vb->uv_ptr[i];
    }
}

//-------------------------------------------------------------------
//  BeginRender()
//  27-Mar-00   floh    created
//  05-Apr-00   floh    if clip_enable, set up a clipping plane
//  11-Apr-00   floh    removed clip plane
//  06-Sep-00   floh    + rewritten for ndynmeshnode
//  18-Oct-00   floh    + rewritten for ndynvertexbuffer
//-------------------------------------------------------------------
void nTerrainNode::BeginRender(void)
{
    this->render_pass++;
    if (this->render_pass==0) this->render_pass=1;
    
    nSceneGraph2* sceneGraph = this->refSceneGraph.get();
    this->cur_vb = this->dyn_vb.Begin(this->ref_ibuf.get(), 
                                      sceneGraph->GetPixelShader(),
                                      sceneGraph->GetTextureArray());
    this->ref_ibuf->Lock();

    this->cur_vertex = 0;
    this->cur_triangle = 0;
    this->coord  = (float *) this->cur_vb->coord_ptr;
    this->norm   = (float *) this->cur_vb->norm_ptr;
    this->stride4 = this->cur_vb->stride4;
    int i;
    for (i=0; i<N_MAXNUM_TEXCOORDS; i++) {
        this->uv[i] = this->cur_vb->uv_ptr[i];
    }
    this->in_render = true;
}

//-------------------------------------------------------------------
//  EndRender()
//  27-Mar-00   floh    created
//  05-Apr-00   floh    if clip_enable, turn off clip plane
//  17-Oct-00   floh    + rewritten to ndynvertexbuffer
//-------------------------------------------------------------------
void nTerrainNode::EndRender(void)
{
    this->dyn_vb.End(this->cur_vertex,this->cur_triangle*3);
    this->ref_ibuf->Unlock();
    this->in_render = false;
}

//-------------------------------------------------------------------
//  CullBox()
//  Perform view frustum culling for a bounding box. Expects
//  this->vp_matrix to be valid (updated inside Compute()).
//  28-Mar-00   floh    created
//-------------------------------------------------------------------
Clip::Visibility nTerrainNode::CullBox(const bbox3& bbox)
{
    int or_code  = 0;
    int and_code = ~0;
    int i;
    float min_x = bbox.vmin.x * this->vertex_radius;
    float min_y = bbox.vmin.y * this->vertex_radius;
    float min_z = bbox.vmin.z * this->vertex_radius;
    float max_x = bbox.vmax.x * this->vertex_radius;
    float max_y = bbox.vmax.y * this->vertex_radius;
    float max_z = bbox.vmax.z * this->vertex_radius;
    vector4 v0(0.0f,0.0f,0.0f,1.0f);
    vector4 v1(0.0f,0.0f,0.0f,1.0f);
    for (i=0; i<8; i++) {
        v0.w = 1.0f;
        if (i & 1) v0.x = max_x;
        else       v0.x = min_x;
        if (i & 2) v0.y = max_y;
        else       v0.y = min_y;
        if (i & 4) v0.z = max_z;
        else       v0.z = min_z;
        v1 = this->vp_matrix * v0;
        int clip = 0;
        if      (v1.x < -v1.w) clip |= CLIP_LEFT;
        else if (v1.x >  v1.w) clip |= CLIP_RIGHT;
        if      (v1.y < -v1.w) clip |= CLIP_TOP;
        else if (v1.y >  v1.w) clip |= CLIP_BOTTOM;
        if      (v1.z < -v1.w) clip |= CLIP_NEAR;
        else if (v1.z >  v1.w) clip |= CLIP_FAR;
        and_code &= clip;
        or_code  |= clip;
    }
    if (and_code != 0) {
        return Clip::NOT_VISIBLE;
    } else if (or_code == 0) {
        return Clip::NO_CLIP;
    } else {
        return Clip::SOME_CLIP;
    }
}

//-------------------------------------------------------------------
//  RenderBox()
//  18-Apr-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::RenderBox(bbox3& box)
{
    nRState rs;
    nGfxServer *gs = this->ref_gs.get();

    rs.Set(N_RS_LIGHTING,N_FALSE); gs->SetState(rs);
    
    vector3 v0(box.vmin * this->vertex_radius);
    vector3 v1(box.vmax * this->vertex_radius);
    gs->Rgba(0.0f,1.0f,0.0f,1.0f);
    gs->Begin(N_PTYPE_LINE_LIST);
    gs->Coord(v0.x,v0.y,v0.z); gs->Coord(v1.x,v0.y,v0.z);
    gs->Coord(v1.x,v0.y,v0.z); gs->Coord(v1.x,v1.y,v0.z);
    gs->Coord(v1.x,v1.y,v0.z); gs->Coord(v0.x,v1.y,v0.z);
    gs->Coord(v0.x,v1.y,v0.z); gs->Coord(v0.x,v0.y,v0.z);
    gs->Coord(v0.x,v0.y,v1.z); gs->Coord(v1.x,v0.y,v1.z);
    gs->Coord(v1.x,v0.y,v1.z); gs->Coord(v1.x,v1.y,v1.z);
    gs->Coord(v1.x,v1.y,v1.z); gs->Coord(v0.x,v1.y,v1.z);
    gs->Coord(v0.x,v1.y,v1.z); gs->Coord(v0.x,v0.y,v1.z);
    gs->Coord(v0.x,v0.y,v0.z); gs->Coord(v0.x,v0.y,v1.z);
    gs->Coord(v1.x,v0.y,v0.z); gs->Coord(v1.x,v0.y,v1.z);
    gs->Coord(v1.x,v1.y,v0.z); gs->Coord(v1.x,v1.y,v1.z);
    gs->Coord(v0.x,v1.y,v0.z); gs->Coord(v0.x,v1.y,v1.z);
    gs->End();

    rs.Set(N_RS_LIGHTING,N_TRUE); gs->SetState(rs);
}

//-------------------------------------------------------------------
//  visualize()
//  26-Apr-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::visualize(void)
{
    nGfxServer *gs = this->ref_gs.get();
    nRState rs;
    rs.Set(N_RS_LIGHTING,N_FALSE); gs->SetState(rs);

    // visualize last line of intersection
    vector3 v0 = this->iline.start();
    vector3 v1 = this->iline.end();
    gs->Begin(N_PTYPE_LINE_LIST);

    gs->Rgba(1.0f,0.0f,0.0f,1.0f);
    gs->Coord(v0.x,v0.y,v0.z); gs->Coord(v1.x,v1.y,v1.z);

    // visualize last point of intersection
    vector3 v2 = this->iposition;
    gs->Rgba(0.0f,0.0f,1.0f,1.0f);
    gs->Coord(v2.x-1.0f, v2.y, v2.z); gs->Coord(v2.x+1.0f, v2.y, v2.z);
    gs->Coord(v2.x, v2.y-1.0f, v2.z); gs->Coord(v2.x, v2.y+1.0f, v2.z);
    gs->Coord(v2.x, v2.y, v2.z-1.0f); gs->Coord(v2.x, v2.y, v2.z+1.0f);

    // visualize normal of intersection
    v0 = this->iposition;
    v1 = v0 + this->itriangle.normal();
    gs->Rgba(1.0f,1.0f,0.0f,1.0f);
    gs->Coord(v0.x,v0.y,v0.z); gs->Coord(v1.x,v1.y,v1.z);
    
    gs->End();

    rs.Set(N_RS_LIGHTING,N_TRUE); gs->SetState(rs);
}


//-------------------------------------------------------------------
//  Compute()
//  26-Mar-00   floh    created
//  28-Mar-00   floh    + compute vp_matrix
//  06-Jun-00   floh    + additional bounding box culling check
//                        before Update() is culled.
//-------------------------------------------------------------------
void 
nTerrainNode::Compute(nSceneGraph2 *sceneGraph)
{
    n_assert(sceneGraph);

    nVisNode::Compute(sceneGraph);

    nGfxServer *gs = this->ref_gs.get();

    //  compute viewer position in local quadtree coordinate space
    vector3 v0;
    matrix44 imv;
    gs->GetMatrix(N_MXM_MODELVIEW,imv);
    imv.invert_simple();
    v0.set(imv.M41,imv.M42,imv.M43);
    v0.x /= this->vertex_radius;
    v0.y /= this->vertex_radius;
    v0.z /= this->vertex_radius;
    vector3 vwr_loc(v0.x,v0.y,v0.z);

    // update vp matrix (used in CullBox())
    matrix44 mvm;
    matrix44 pm;
    gs->GetMatrix(N_MXM_MODELVIEW,mvm);
    gs->GetMatrix(N_MXM_PROJECTION,pm);
    this->vp_matrix = mvm * pm;

    // demand create vertex buffers
    if (!this->dyn_vb.IsValid()) {
        this->init_buffers();
    }

    // (re)load height map?
    if (this->map_dirty) {
        // make sure everything is loaded...
        if (this->map_dirty) this->load_map();
    }

    if (this->first_run) {
        // Run the update function a few times before we start rendering
        // to disable unnecessary quadsquares, so the first frame won't
        // be overloaded with tons of triangles.
        int i;
        for (i = 0; i < 10; i++) {
    	    this->root_node->Update(this,this->root_corner_data,vwr_loc,this->detail);
        }
        this->first_run = false;
    }

    // cull nodes and render
    Clip::Visibility vis;
    vis = this->CullBox(this->root_node->bbox);
    if (vis != Clip::NOT_VISIBLE) {
        this->p_update->Start();
        this->root_node->Update(this,this->root_corner_data,vwr_loc,this->detail);
        this->p_update->Stop();
        this->p_render->Start();

        // backup the pixel shader's layer 0 texture so that we can
        // restore it after rendering
        this->root_node->Render(this,this->root_corner_data);
        this->p_render->Stop();
    }

    // visualize stuff
    // this->visualize();
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
