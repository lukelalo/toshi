#ifndef N_TERRAINNODE_H
#define N_TERRAINNODE_H
//--------------------------------------------------------------------
/**
    @class nTerrainNode

    @brief An adaptive lod terrain renderer for spherical terrains.

    Thatcher Ulrich's <tu@tulrich.com> terrain renderer wrapped
    into a Nebula class. All credits go to Mr. Ulrich.
*/
//--------------------------------------------------------------------

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef QUADTREE_H
#include "terrain/quadtree.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_RSRCPATH_H
#include "misc/nrsrcpath.h"
#endif

#ifndef N_BBOX_H
#include "mathlib/bbox.h"
#endif

#ifndef N_LINE_H
#include "mathlib/line.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#include "terrain/quadtree.h"
#include "terrain/clip.h"

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nTerrainNode
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nGfxServer;
class nProfiler;
class nSceneGraph2;
class N_PUBLIC nTerrainNode : public nVisNode {
    enum {
        CLIP_LEFT   = (1<<0),
        CLIP_RIGHT  = (1<<1),
        CLIP_TOP    = (1<<2),
        CLIP_BOTTOM = (1<<3),
        CLIP_FAR    = (1<<4),
        CLIP_NEAR   = (1<<5),
    };
    
    nAutoRef<nGfxServer>    ref_gs;
    nAutoRef<nSceneGraph2>  refSceneGraph;
    nAutoRef<nFileServer2>  refFile;
    nRsrcPath image_path;       // filename of bmp height map
    nRsrcPath tree_path;        // filename of tree file map
    bool map_dirty;
    bool first_run;             // true after map reloaded
    
    float detail;
    float static_error;
    float uv_scale;
    VertInfo root_vertices[4];
    quadcornerdata root_corner_data;
    quadsquare *root_node;      // root node of terrain's quad tree
    float radius;               // radius of sphere in meters
    float vertex_radius;        // vertex scale radius to convert from 1^16 space to world space

    quadsquare *free_quads;     // pointer to linked list of free quads
    quadsquare *quad_pool;      // pointer to quad tree pool

    matrix44 vp_matrix;         // combined view-projection matrix for frustum culling

    ushort render_pass;
    int cur_vertex;
    int cur_triangle;

    int num_triangles;          // number of triangles that fit into the vertex buffer
    nRef<nIndexBuffer> ref_ibuf;
    nVertexBuffer *cur_vb;      // the current vertex buffer
    int stride4;                        // vertex format stride
    float *coord;                       // the current vertex buffers current coord pointer
    float *norm;                        // the current vertex buffers current norm pointer
    float *uv[N_MAXNUM_TEXCOORDS];      // the current vertex buffers current uv pointer
    bool in_render;

    int precull_nodes;          // statistics   
    int postcull_nodes;         // statistics

    // DEBUG ONLY!
    line3 iline;                // last line of last intersection
    vector3 iposition;          // position of last intersection
    triangle itriangle;         // triangle of last intersection

    // saving to wavefront file...
    nFile* fp;                   // for saving to Wavefront file...
    int num_tris;               // number of triangles written

    // the dynamic vertex buffer we render to
    nDynVertexBuffer dyn_vb;

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    static nProfiler *p_update;
    static nProfiler *p_render;

    nTerrainNode();
    virtual ~nTerrainNode();
    virtual bool Attach(nSceneGraph2*);
    virtual void Compute(nSceneGraph2*);
    virtual bool SaveCmds(nPersistServer *);

    virtual void  SetDetail(float);
    virtual float GetDetail(void);
    virtual void  SetStaticError(float);
    virtual float GetStaticError(void);
    virtual void  SetUvScale(float);
    virtual float GetUvScale(void);
    virtual void  SetHeightMap(const char *);
    virtual const char *GetHeightMap(void);
    virtual void  SetTreeFile(const char *);
    virtual const char *GetTreeFile(void);
    virtual void  SetRadius(float r);
    virtual float GetRadius(void);
    virtual bool Intersect(line3& l, float& ipos, triangle& itri);

    void BeginRender(void);
    void EndRender(void);

    bool SaveToWavefront(const char *);
    void BeginRenderToWF(void);
    void EndRenderToWF(void);
    void AddTriangleToWF(quadsquare *, VertInfo *, VertInfo *, VertInfo *);

    Clip::Visibility CullBox(const bbox3& bbox);
    bool SaveTree(const char *fname);
    void GetNumNodes(int& precull, int& postcull);

    void RenderBox(bbox3& box);

    // set height values directly from array of float pointers
    void SetFloatArrayHeightMap(int width, int height, float *floatPointer);

    //-------------------------------------------------------------------
    //  AddTriangle()
    //  Add a triangle to vertex buffer, reuse vertex coordinates
    //  if already exists in buffer, otherwise add a new vertex.
    //  27-Mar-00   floh    created
    //  05-Apr-00   floh    + support for y-mirroring
    //  18-Apr-00   floh    + some optimization (removed the 
    //                        virtual function calls on vbuffer to add
    //                        vertices
    //  12-May-00   floh    + some optimization
    //-------------------------------------------------------------------
    void nTerrainNode::AddTriangle(quadsquare *, VertInfo *v0, VertInfo *v1, VertInfo *v2)
    {
        n_assert(this->in_render);
        int triangle_base_index = this->cur_triangle*3;
        VertInfo *v_array[3];
        v_array[0]=v0; v_array[1]=v1; v_array[2]=v2;

        int i;
        for (i=0; i<3; i++) {
            VertInfo *vi = v_array[i];

            // does vertex already exist in current vbuffer?
            if (vi->vpass == this->render_pass) {
                // yes, just add index...
                this->ref_ibuf->Index(triangle_base_index + i, vi->vindex);
            } else {
                float x = vi->cart.x * this->vertex_radius;
                float y = vi->cart.y * this->vertex_radius;
                float z = vi->cart.z * this->vertex_radius;
                this->coord[0]=x; this->coord[1]=y; this->coord[2]=z; this->coord+=this->stride4;
                vector3 n(vi->n.unpack());
                this->norm[0]=n.x; this->norm[1]=n.y; this->norm[2]=n.z; this->norm+=this->stride4;

                // compute uv coordinates for texture layer 0
                this->uv[0][0] = float(vi->v) / 256.0f;
                this->uv[0][1] = float(vi->u) / 256.0f;
                this->uv[0] += this->stride4;                

                // compute uv coordinates for texture layer 1
                this->uv[1][0] = x*this->uv_scale; 
                this->uv[1][1] = z*this->uv_scale; 
                this->uv[1] += this->stride4;

                this->ref_ibuf->Index(triangle_base_index + i, this->cur_vertex);
                vi->vpass  = this->render_pass;
                vi->vindex = this->cur_vertex;
                this->cur_vertex++;
            }
        }

        // increment triangle count and check if vertex buffer full
        this->cur_triangle++;
        if (this->cur_triangle == this->num_triangles) {
            this->swap_buffers();
        }
    };

    //-------------------------------------------------------------------
    //  AllocQuadSquare()
    //  Allocate and initialize a new quadsquare. Return index
    //  into quad_pool array.
    //  19-Apr-00   floh    created
    //-------------------------------------------------------------------
    ushort nTerrainNode::AllocQuadSquare(quadcornerdata *pcd)
    {
        quadsquare *qs = this->free_quads;
        if (qs) {
            this->free_quads = qs->next;
            qs->Init(pcd,this->quad_pool);
            return (qs - this->quad_pool);
        } else {
            return 0;
        }
    };

    //-------------------------------------------------------------------
    //  FreeQuadSquare()
    //  Free a quadsquare object.
    //  19-Apr-00   floh    created
    //-------------------------------------------------------------------
    void nTerrainNode::FreeQuadSquare(quadsquare *qs)
    {
        qs->next = this->free_quads;
        this->free_quads = qs;
    };

private:
    void init_buffers(void);
    void swap_buffers(void);
    bool fill_heightmap_from_bmp(const char *, HeightMapInfo&, int& w, int& h);
    void fillHeightMapFromFloatArray(int width, int height, float* floatPointer, HeightMapInfo& heightMapInfo);
    bool load_height_map(const char *);
    bool load_tree_file(const char *);
    void visualize(void);
    void free_tree(void);
    void allocate_tree(int num_quads);
    void load_map(void);
};
//--------------------------------------------------------------------
#endif
