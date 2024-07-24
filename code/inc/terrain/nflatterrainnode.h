#ifndef N_FLATTERRAINNODE_H
#define N_FLATTERRAINNODE_H
//--------------------------------------------------------------------
/**
    @class nFlatTerrainNode

    @brief An adaptive lod terrain renderer for landscapes.

    Thatcher Ulrich's <tu@tulrich.com> terrain renderer wrapped
    into a Nebula class. All credits go to Mr. Ulrich.
*/
//--------------------------------------------------------------------

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_FLAT_QUADTREE_H
#include "terrain/flat_quadtree.h"
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

#include "terrain/flat_quadtree.h"
#include "terrain/clip.h"

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nFlatTerrainNode
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nGfxServer;
class N_DLLCLASS nFlatTerrainNode : public nVisNode {
    enum {
        NUM_TRIANGLES = 1024,   // number of triangles per vertex buffer
    };
    enum {
        CLIP_LEFT   = (1<<0),
        CLIP_RIGHT  = (1<<1),
        CLIP_TOP    = (1<<2),
        CLIP_BOTTOM = (1<<3),
        CLIP_FAR    = (1<<4),
        CLIP_NEAR   = (1<<5),
    };
    
	nAutoRef<nSceneGraph2> ref_sceneGraph;
    nAutoRef<nGfxServer> ref_gs;
    nAutoRef<nFileServer2> refFile;
    nRsrcPath image_path;       // filename of img height map
    nRsrcPath tree_path;        // filename of tree file map
    bool map_dirty;
    bool first_run;             // true after map reloaded

	nRoot *e_weld;				// temporary storage pointer until the Update() is called
	nRoot *s_weld;				// NULL if no weld has been called.

    float detail;
    float static_error;
    float uv_scale;
    FlatVertInfo root_vertices[4];
    flatquadcornerdata root_corner_data;
    flatquadsquare *root_node;      // root node of terrain's quad tree
    float radius;               // radius of sphere in meters
    float vertex_radius;        // vertex scale radius to convert from 1^16 space to world space

    flatquadsquare *free_quads;     // pointer to linked list of free quads
    flatquadsquare *quad_pool;      // pointer to quad tree pool

    matrix44 vp_matrix;         // combined view-projection matrix for frustum culling

    int num_triangles;          // number of triangles that fit into the vertex buffer
    nRef<nIndexBuffer> ref_ibuf;
    nVertexBuffer *cur_vb;      // the current vertex buffer

    ushort render_pass;
    int cur_vertex;
    int cur_triangle;

    int stride;                 // the vertex buffers stride
    float *coord;               // the current vertex buffers current coord pointer
    float *norm;                // the current vertex buffers current norm pointer
    float *uv[N_MAXNUM_TEXCOORDS];      // the current vertex buffers current uv pointer
    bool in_render;

    int precull_nodes;          // statistics   
    int postcull_nodes;         // statistics

    // DEBUG ONLY!
    line3 iline;                // last line of last intersection
    vector3 iposition;          // position of last intersection
    triangle itriangle;         // triangle of last intersection

    // saving to wavefront file...
    nFile *fp;                   // for saving to Wavefront file...
    int num_tris;               // number of triangles written

    // the dynamic vertex buffer we render to
    nDynVertexBuffer dyn_vb;

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    static nProfiler *p_update;
    static nProfiler *p_render;
	
	// For dynamic welding functions. e, n, w, s
	flatquadcornerdata *neighbors[4];

    nFlatTerrainNode();
    virtual ~nFlatTerrainNode();
    virtual bool Attach(nSceneGraph2 *);
    virtual void Compute(nSceneGraph2 *);
    virtual void Preload();
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
	virtual bool Weld(nRoot *south, nRoot *east);
	virtual float GetHeight(float x, float z);

    void BeginRender(void);
    void EndRender(void);

    bool SaveToWavefront(const char *);
    void BeginRenderToWF(void);
    void EndRenderToWF(void);
    void AddTriangleToWF(flatquadsquare *, nFlatTriangleCorner *, nFlatTriangleCorner *, nFlatTriangleCorner *);

    Clip::Visibility CullBox(const bbox3_qs *bbox);
    bool SaveTree(const char *fname);
    void GetNumNodes(int& precull, int& postcull);

    void RenderBox(bbox3_qs *box);

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
    void nFlatTerrainNode::AddTriangle(flatquadsquare *qs,
                                   nFlatTriangleCorner *tc0,
                                   nFlatTriangleCorner *tc1,
                                   nFlatTriangleCorner *tc2)
    {
        n_assert(this->in_render);
        nFlatTriangleCorner *tc_array[3];
        tc_array[0] = tc0;
        tc_array[1] = tc1;
        tc_array[2] = tc2;
        float r = this->vertex_radius;
        int triangle_base_index = this->cur_triangle*3;

        int i;
        for (i=0; i<3; i++) {
            nFlatTriangleCorner *tc = tc_array[i];

            // does vertex already exist in current vbuffer?
            if (tc->vi->vpass == this->render_pass) {
                // yes, just add index...
                int index = triangle_base_index + i;
                this->ref_ibuf->Index(triangle_base_index + i, tc->vi->vindex);
            } else {
                if (!tc->pos_ok) {
                    tc->pos = vector3(float(tc->x),tc->vi->y,float(tc->z));
                    tc->pos *= r;
                    tc->pos_ok = true;
                }
                float x = tc->pos.x;
                float y = tc->pos.y;
                float z = tc->pos.z;
                this->coord[0]=x; this->coord[1]=y; this->coord[2]=z; this->coord+=this->stride;
                vector3 n(tc->vi->n.unpack());
                float nx = n.x;
                float ny = n.y;
                float nz = n.z;
                this->norm[0]=nx; this->norm[1]=ny; this->norm[2]=nz; this->norm+=this->stride;
//                  this->uv[0][0] = x*this->uv_scale; this->uv[0][1] = z*this->uv_scale; this->uv[0] += this->stride;
 
// compute uv coordinates for texture layer 0
                this->uv[0][0] = float(tc->vi->v) / 256.0f;
                this->uv[0][1] = float(tc->vi->u) / 256.0f;
                this->uv[0] += this->stride;                

                // compute uv coordinates for texture layer 1
                this->uv[1][0] = x*this->uv_scale; 
                this->uv[1][1] = z*this->uv_scale; 
                this->uv[1] += this->stride;

                this->ref_ibuf->Index(triangle_base_index + i, this->cur_vertex);
                tc->vi->vpass  = this->render_pass;
                tc->vi->vindex = this->cur_vertex;
                this->cur_vertex++;
            }
        }

        // increment triangle count and check if vertex buffer full
        this->cur_triangle++;
        if (this->cur_triangle == this->num_triangles) {
            this->swap_vbuffer();
        }
    };

    //-------------------------------------------------------------------
    //  Allocflatquadsquare()
    //  Allocate and initialize a new flatquadsquare. Return index
    //  into quad_pool array.
    //  19-Apr-00   floh    created
    //-------------------------------------------------------------------
    ushort nFlatTerrainNode::AllocQuadSquare(flatquadcornerdata *pcd)
    {
        flatquadsquare *qs = this->free_quads;
        if (qs) {
            this->free_quads = qs->next;
            qs->Init(pcd,this->quad_pool, this);
            return (qs - this->quad_pool);
        } else {
            return 0;
        }
    };

    //-------------------------------------------------------------------
    //  Freeflatquadsquare()
    //  Free a flatquadsquare object.
    //  19-Apr-00   floh    created
    //-------------------------------------------------------------------
    void nFlatTerrainNode::FreeQuadSquare(flatquadsquare *qs)
    {
        qs->next = this->free_quads;
        this->free_quads = qs;
    };

private:
    bool fill_heightmap_from_bmp(const char *, FlatHeightMapInfo&, int& w, int& h);
    bool load_height_map(const char *);
    bool load_tree_file(const char *);
    void init_vbuffer(void);
    void swap_vbuffer(void);
//    void flush_vbuffer(void);
    void visualize(void);
    void free_tree(void);
    void allocate_tree(int num_quads);
    void load_map(void);
	void WeldAux(void);
};
//--------------------------------------------------------------------
#endif
