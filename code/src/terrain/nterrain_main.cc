#define N_IMPLEMENTS nTerrainNode
//-------------------------------------------------------------------
//  nterrain_main.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "terrain/nterrainnode.h"

//-------------------------------------------------------------------
//  nTerrainNode()
//  23-Mar-00   floh    created
//  06-Oct-00   floh    rewritten to ndynmeshnode
//  17-Oct-00   floh    rewritten again to ndynvertexbuffer
//-------------------------------------------------------------------
nTerrainNode::nTerrainNode()
: ref_ibuf(this),
  dyn_vb(ks,this),
  ref_gs(ks,this),
  refSceneGraph(ks,this),
  refFile(ks, this)
{
    this->ref_gs = "/sys/servers/gfx";
    this->refSceneGraph = "/sys/servers/sgraph2";
    this->refFile = "/sys/servers/file2";

    this->map_dirty = false;
    this->first_run = false;
    this->detail = 100.0f;
    this->static_error = 8.0f;
    this->uv_scale = 1.0f;
    this->radius = 10.0f;
    this->quad_pool   = NULL;
    this->free_quads  = NULL;
    this->render_pass = 0;

    this->num_triangles = 0;
    this->stride4 = 0;
    this->cur_vertex = 0;
    this->cur_triangle = 0;
    this->cur_vb = NULL;
    this->coord = NULL;
    this->norm  = NULL;
    memset(this->uv,0,sizeof(this->uv));

    this->in_render = false;

    this->precull_nodes  = 0;
    this->postcull_nodes = 0;

    // initialize root-corner-data
    int i;
    for (i=0; i<4; i++) {
        this->root_vertices[i].y  = 0;
    }
    this->root_corner_data.parent = NULL;
    this->root_corner_data.square = NULL;
    this->root_corner_data.child_index = 0;
    this->root_corner_data.level = 15;
    this->root_corner_data.xorg = 0;
    this->root_corner_data.zorg = 0;
    for (i=0; i<4; i++) {
        this->root_corner_data.verts[i] = &(this->root_vertices[i]);
    }
    this->root_node = NULL;
    this->fp = NULL;
    this->num_tris = 0;
}

//-------------------------------------------------------------------
//  ~TerrainNode()
//  23-Mar-00   floh    created
//-------------------------------------------------------------------
nTerrainNode::~nTerrainNode()
{
    if (this->root_node)    this->root_node->Release(this);
    if (this->quad_pool)    delete[] this->quad_pool;
}

//-------------------------------------------------------------------
//  SetDetail()
//  26-Mar-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::SetDetail(float d)
{
    this->detail = d;
}

//-------------------------------------------------------------------
//  GetDetail()
//  26-Mar-00   floh    created
//-------------------------------------------------------------------
float nTerrainNode::GetDetail(void)
{
    return this->detail;
}

//-------------------------------------------------------------------
//  SetStaticError()
//  30-Mar-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::SetStaticError(float f)
{
    this->static_error = f;
    this->map_dirty = true;
}

//-------------------------------------------------------------------
//  GetStaticError()
//  30-Mar-00   floh    created
//-------------------------------------------------------------------
float nTerrainNode::GetStaticError(void)
{
    return this->static_error;
}

//-------------------------------------------------------------------
//  SetRadius()
//  27-Mar-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::SetRadius(float r)
{
    this->radius = r;
    this->vertex_radius = this->radius / float(1<<16);
    this->map_dirty = true;
}

//-------------------------------------------------------------------
//  GetRadius()
//  27-Mar-00   floh    created
//-------------------------------------------------------------------
float nTerrainNode::GetRadius(void)
{
    return this->radius;
}

//-------------------------------------------------------------------
//  SetUvScale()
//  27-Mar-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::SetUvScale(float s)
{
    this->uv_scale = s;
}

//-------------------------------------------------------------------
//  GetUvScale()
//  27-Mar-00   floh    created
//-------------------------------------------------------------------
float nTerrainNode::GetUvScale(void)
{
    return this->uv_scale;
}

//-------------------------------------------------------------------
//  SetHeightMap()
//  27-Mar-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::SetHeightMap(const char *name)
{
    char buf[N_MAXPATH];
    getcwd(buf,sizeof(buf));
    this->image_path.Set(this->refFile.get(),name,buf);
    this->tree_path.Clear();
    this->map_dirty = true;
}

//-------------------------------------------------------------------
//  GetHeightMap()
//  27-Mar-00   floh    created
//-------------------------------------------------------------------
const char *nTerrainNode::GetHeightMap(void)
{
    return this->image_path.GetPath();
}

//-------------------------------------------------------------------
//  SetTreeFile()
//  26-Apr-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::SetTreeFile(const char *name)
{
    char buf[N_MAXPATH];
    getcwd(buf,sizeof(buf));
    this->tree_path.Set(this->refFile.get(),name,buf);
    this->image_path.Clear();
    this->map_dirty = true;
}

//-------------------------------------------------------------------
//  GetTreeFile()
//  26-Apr-00   floh    created
//-------------------------------------------------------------------
const char *nTerrainNode::GetTreeFile(void)
{
    return this->tree_path.GetPath();
}

//-------------------------------------------------------------------
//  GetNumNodes()
//  Get number of nodes in tree before
//  and after static culling (for statistics).
//  27-Apr-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::GetNumNodes(int& precull, int& postcull)
{
    precull  = this->precull_nodes;
    postcull = this->postcull_nodes;
}

//-------------------------------------------------------------------
//  Attach()
//  26-Mar-00   floh    created
//-------------------------------------------------------------------
bool nTerrainNode::Attach(nSceneGraph2* sceneGraph)
{
    if (nVisNode::Attach(sceneGraph)) 
    {
        sceneGraph->AttachVisualNode(this);
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
//  Intersect()
//  Intersect line with terrain, return true if intersection
//  occurs and point of intersection. Line must already have been
//  translated into the terrains local coordinate system.
//  20-Apr-00   floh    created
//  03-May-00   floh    load map if not happened yet, because
//                      intersection may happen at any time
//-------------------------------------------------------------------
bool nTerrainNode::Intersect(line3& l, float& ipos, triangle& itri)
{
    // make sure everything is loaded...
    if (this->map_dirty) this->load_map();

    // store line for visualization
    this->iline = l;

    // scale line into terrain's number range
    line3 l0(l.start()/this->vertex_radius, l.end()/this->vertex_radius);
    
    // call the quadtree's intersect method
    ipos = 2.0f;    // initialize with outside-bounds-value
    if (this->root_node->Intersect(this->root_corner_data,l0,ipos,itri)) {
        n_assert((ipos>=0.0f) && (ipos<=1.0f));
        this->iposition = l.ipol(ipos);
        this->itriangle = itri;
        return true;
    } else {
        this->iposition.set(0.0f,0.0f,0.0f);
        return false;
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
