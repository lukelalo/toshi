#define N_IMPLEMENTS nFlatTerrainNode
//-------------------------------------------------------------------
//  nflatterrain_main.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "terrain/nflatterrainnode.h"

//-------------------------------------------------------------------
/**
    23-Mar-00   floh    created
*/
//-------------------------------------------------------------------
nFlatTerrainNode::nFlatTerrainNode()
: ref_ibuf(this),
  dyn_vb(ks,this),
  ref_gs(ks,this),
  ref_sceneGraph(ks,this),
  refFile(ks, this)
{
	this->ref_sceneGraph = "/sys/servers/sgraph2";
    this->ref_gs = "/sys/servers/gfx";
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

//    this->tmp_sgnode_ptr = NULL;
    this->num_triangles = 0;
    this->cur_vertex = 0;
    this->cur_triangle = 0;
    this->cur_vb = NULL;

    this->stride = 0;
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

    for (i=0; i<4; i++) {
        this->neighbors[i] = NULL;
    }

    this->e_weld = this->s_weld = NULL;
}

//-------------------------------------------------------------------
/**
    23-Mar-00   floh    created
*/
//-------------------------------------------------------------------
nFlatTerrainNode::~nFlatTerrainNode()
{
    if (this->root_node) this->root_node->Release(this);
    if (this->quad_pool) delete[] this->quad_pool;
}

//-------------------------------------------------------------------
/**
    @brief Set the detail threshold value.

    This is a percentage ranging from 0.0 to 100.0 and may be
    modified during runtime without reloading the heightmap data.

    26-Mar-00   floh    created
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::SetDetail(float d)
{
    this->detail = d;
}

//-------------------------------------------------------------------
/**
    @brief Return the detail threshold value.

    26-Mar-00   floh    created
*/
//-------------------------------------------------------------------
float nFlatTerrainNode::GetDetail(void)
{
    return this->detail;
}

//-------------------------------------------------------------------
/**
    @brief Set the static error threshold value used to remove
    triangles in flat areas.

    The default value is 25.

    Changing this value will cause a reload of the heightmap data.

    30-Mar-00   floh    created
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::SetStaticError(float f)
{
    this->static_error = f;
    this->map_dirty = true;
}

//-------------------------------------------------------------------
/**
    @brief Return the static error threshold value.

    30-Mar-00   floh    created
*/
//-------------------------------------------------------------------
float nFlatTerrainNode::GetStaticError(void)
{
    return this->static_error;
}

//-------------------------------------------------------------------
/**
    @brief Set the maximum radius of the terrain in meters.

    Changing this value will cause a reload of the heightmap
    data.

    27-Mar-00   floh    created
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::SetRadius(float r)
{
    this->radius = r;
    this->vertex_radius = this->radius / float(1<<16);
    this->map_dirty = true;
}

//-------------------------------------------------------------------
/**
    @brief Return the maximum radius of the terrain.

    @return Maximum radius of the terrain.

    27-Mar-00   floh    created
*/
//-------------------------------------------------------------------
float nFlatTerrainNode::GetRadius(void)
{
    return this->radius;
}

//-------------------------------------------------------------------
/**
    @brief Set the UV scale factor used in generating the texture
    coordinates.

    @verbatim
    u = x * scale;
    v = z * scale;
    @endverbatim

    27-Mar-00   floh    created
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::SetUvScale(float s)
{
    this->uv_scale = s;
}

//-------------------------------------------------------------------
/**
    @brief Get the UV scale factor used in generating the
    texture coordinates.

    27-Mar-00   floh    created
*/
//-------------------------------------------------------------------
float nFlatTerrainNode::GetUvScale(void)
{
    return this->uv_scale;
}

//-------------------------------------------------------------------
/**
    @brief Set the filename to load the heightmap data from.

    This will be loaded during the next frame.

    27-Mar-00   floh    created
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::SetHeightMap(const char *name)
{
    char buf[N_MAXPATH];
    getcwd(buf,sizeof(buf));
    this->image_path.Set(this->refFile.get(),name,buf);
    this->tree_path.Clear();
    this->map_dirty = true;
}

//-------------------------------------------------------------------
/**
    @brief Get the filename for the heightmap data.

    @return The filename for the heightmap data.

    27-Mar-00   floh    created
*/
//-------------------------------------------------------------------
const char *nFlatTerrainNode::GetHeightMap(void)
{
    return this->image_path.GetPath();
}

//-------------------------------------------------------------------
/**
    @brief Set the filename to load the load the tree file from.

    This will be loaded and used during the next frame.

    26-Apr-00   floh    created
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::SetTreeFile(const char *name)
{
    char buf[N_MAXPATH];
    getcwd(buf,sizeof(buf));
    this->tree_path.Set(this->refFile.get(),name,buf);
    this->image_path.Clear();
    this->map_dirty = true;
}

//-------------------------------------------------------------------
/**
    @brief Get the filename to the tree file.

    @return filename to the tree file.

    26-Apr-00   floh    created
*/
//-------------------------------------------------------------------
const char *nFlatTerrainNode::GetTreeFile(void)
{
    return this->tree_path.GetPath();
}

//-------------------------------------------------------------------
/**
    @brief Get number of nodes in tree before and after static culling
    (for statistics).

    27-Apr-00   floh    created
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::GetNumNodes(int& precull, int& postcull)
{
    precull  = this->precull_nodes;
    postcull = this->postcull_nodes;
}

//-------------------------------------------------------------------
/**
    @brief Attach the terrain to the scenegraph.

    26-Mar-00   floh    created
*/
//-------------------------------------------------------------------
bool nFlatTerrainNode::Attach(nSceneGraph2 *sg)
{
    if (nVisNode::Attach(sg)) {
        sg->AttachVisualNode(this);
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
/**
    @brief Intersect line with terrain.
    
    Line must already have been translated into the terrains local
    coordinate system.

    @return true if intersection occurs and point of intersection.

    20-Apr-00   floh    created
    03-May-00   floh    load map if not happened yet, because
                        intersection may happen at any time
*/
//-------------------------------------------------------------------
bool nFlatTerrainNode::Intersect(line3& l, float& ipos, triangle& itri)
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

void nFlatTerrainNode::Preload()
{
    if (this->map_dirty)
        this->load_map();
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
