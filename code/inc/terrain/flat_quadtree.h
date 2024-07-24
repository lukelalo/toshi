#ifndef N_FLAT_QUADTREE_H
#define N_FLAT_QUADTREE_H
//-------------------------------------------------------------------
// quadtree.hpp	-thatcher 9/15/1999 Copyright 1999-2000 Thatcher Ulrich
//
// Data structures for quadtree terrain storage.
//
// This code may be freely modified and redistributed.  I make no
// warrantees about it; use at your own risk.  If you do incorporate
// this code into a project, I'd appreciate a mention in the credits.
//
// Thatcher Ulrich <tu@tulrich.com>
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_FILESERVER2_H
#include "kernel/nfileserver2.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_BBOX_QS_H
#include "mathlib/bbox_qs.h"
#endif

#ifndef N_TRIANGLE_H
#include "mathlib/triangle.h"
#endif

#ifndef N_PKNORM_H
#include "mathlib/pknorm.h"
#endif
#ifndef N_LINE_H
#include "mathlib/line.h"
#endif
#include "terrain/clip.h"
//-------------------------------------------------------------------
class FlatHeightMapInfo {
public:
   float *data;
   int x_origin,z_origin;
   int	x_size, z_size;
   int	row_width;
   int	scale;
    
public:
    FlatHeightMapInfo() {
        this->data = NULL;
    };
    FlatHeightMapInfo(int xorig, int zorig, int xsize, int zsize, int s) {
        this->x_origin       = xorig;
        this->z_origin       = zorig;
        this->x_size         = xsize;
        this->z_size         = zsize;
        this->scale          = s;
        this->row_width = xsize;
        this->data = new float[this->x_size*this->z_size];
    };
    ~FlatHeightMapInfo() {
        if (this->data) delete[] this->data;
    };
    //---------------------------------------------------------------
    //  Set()
    //---------------------------------------------------------------
    void Set(int xorig, int zorig, int xsize, int zsize, int s) {
        n_assert(NULL == this->data);
        this->x_origin       = xorig;
        this->z_origin       = zorig;
        this->x_size         = xsize;
        this->z_size         = zsize;
        this->scale          = s;
        this->row_width = xsize;
        this->data = new float[this->x_size*this->z_size];
    };

    //---------------------------------------------------------------
    //  SetHeight()
    //  Set height at a specified coordinate.
    //---------------------------------------------------------------
    void SetHeight(int x, int z, float h) {
        n_assert(data);
        n_assert((x>=0) && (x<x_size));
        n_assert((z>=0) && (z<z_size));
        data[z*row_width + x] = h;
    };

    //---------------------------------------------------------------
    //  Sample()
    // Returns the height (y-value) of a point in this heightmap.  The given (x,z) are in
    // world coordinates.  Heights outside this heightmap are considered to be 0.  Heights
    // between sample points are bilinearly interpolated from surrounding points.
    // xxx deal with edges: either force to 0 or over-size the query region....
    //---------------------------------------------------------------
    float Sample(int x, int z) {
	    n_assert(data);
        
        // Break coordinates into grid-relative coords (ix,iz) and remainder (rx,rz).
        // and wrap around if coords are invalid.
        int	ix0 = (x - this->x_origin) >> this->scale;
        int	iz0 = (z - this->z_origin) >> this->scale;
        if (ix0<0) ix0=0;
        if (iz0<0) iz0=0;
        if (ix0>=this->x_size) ix0=this->x_size-1;
        if (iz0>=this->z_size) iz0=this->z_size-1;
        int ix1 = ix0+1;
        int iz1 = iz0+1;
        if (ix1<0) ix1=this->x_size-1;
        if (iz1<0) iz1=this->z_size-1;
        if (ix1>=this->x_size) ix1=0;
        if (iz1>=this->z_size) iz1=0;

        int mask = (1 << this->scale) - 1;
        int rx = (x - this->x_origin) & mask;
        int rz = (z - this->z_origin) & mask;
        float fx = float(rx) / float(mask+1);
        float fz = float(rz) / float(mask+1);
        float s00 = this->data[ix0 + iz0 * this->row_width];
        float s01 = this->data[ix1 + iz1 * this->row_width];
        float s10 = this->data[ix0 + iz1 * this->row_width];
        float s11 = this->data[ix1 + iz1 * this->row_width];
        return (s00 * (1.0f-fx) + s01 * fx) * (1.0f-fz) + (s10 * (1.0f-fx) + s11 * fx) * fz;
    };
};

//-------------------------------------------------------------------
//  FlatVertInfo
//  Packed into 8 bytes.
//-------------------------------------------------------------------
struct	FlatVertInfo {
    
    ushort y;   // the vertex' y coordinate
    pknorm3 n;  // the vertex normal for lighting packed into 16 bits
    ushort vpass, vindex;// information for vertex buffer index reuseage
    ushort u,v;             // the vertices uv coordinate
};

//-------------------------------------------------------------------
//  A structure used during recursive traversal of the tree to hold
//  relevant but transitory data.
//  27-Mar-00   floh    FlatVertInfo contains pointers, not copies
//-------------------------------------------------------------------
class flatquadsquare;
struct flatquadcornerdata {
    flatquadcornerdata *parent;
    flatquadsquare *square;
    int	child_index;
    int	level;
    int xorg, zorg;
    FlatVertInfo *verts[4];  // ne, nw, sw, se
};

//-------------------------------------------------------------------
//  Structure that describes one triangle for rendering through
//  double buffered indexed vertex buffers with vertex reuse.
//-------------------------------------------------------------------
class nFlatTriangleCorner {
public:
    int x,z;            // polar x/z 
    FlatVertInfo *vi;       // vertex info containing y coord, normal and index reuse data
    vector3 pos;        // the triangle's corner position in model space
    bool pos_ok;        // false: pos noch nicht updated    
    nFlatTriangleCorner() {
        pos_ok = false;
    };
};

//-------------------------------------------------------------------
//  flatquadsquare
//  FIXME: size optimization!
//-------------------------------------------------------------------
class nFlatTerrainNode;
class flatquadsquare {
public:
    union {
        ushort child[4];        // index of child quads into qs_base array (0 is reserved for root node!)
        flatquadsquare *next;       // this is for linkage into the quad allocator list only
    };
    FlatVertInfo vertex[5];         // center, e, n, w, s
    ushort error[6];            // e, s, children: ne, nw, sw, se
    bbox3_qs *bbox;
    unsigned char enabled_flags;    // bits 0-7: e, n, w, s, ne, nw, sw, se
    unsigned char sub_enabled_count[2]; // e, s enabled reference counts.
    unsigned char flags;
    enum {
        STATIC = (1<<0),            // static node (may not be deleted)
        DIRTY  = (1<<1),            // Set when vertex data has changed, but error/enabled data has not been recalculated.
    };
    flatquadsquare *qs_base;            // base pointer to array of flatquadsquare nodes
	// UGLY - but there's no other way to keep the
	// welds that I can see since the nTerrainNode
	// holds the 4 pointers (ENWS) to the
    // possible joined edges - I can either mirror
	// those here in EACH quadtree - or give each
	// quadtree 1 pointer to the master nTerrainNode
	// that will have the information.. I'd rather
	// have a utility function to let me lookup my
	// master node - so I can avoid this, but it's 
	// not feasible.
	//
	// It blows the OO of this object, but it gains
	// the ability for each tree to detect and line
	// up vertices based on their neighbors.
	nFlatTerrainNode *master;

    flatquadsquare();
    void  Init(flatquadcornerdata *pcd, flatquadsquare *, nFlatTerrainNode *master);
    void  Release(nFlatTerrainNode *tn);
    void  AddHeightMap(nFlatTerrainNode *tn, flatquadcornerdata& cd, FlatHeightMapInfo& hm);
	void  StaticCullData(nFlatTerrainNode *tn, flatquadcornerdata& cd, float threshold_detail);	
	ushort RecomputeError(flatquadcornerdata& cd);
	int	  CountNodes();
    void  AlignBorders(flatquadcornerdata&, nFlatTerrainNode *south, nFlatTerrainNode *east);
    void  AlignSouthBorders(flatquadcornerdata&, flatquadcornerdata&);
    void  AlignEastBorders(flatquadcornerdata&, flatquadcornerdata&);
	void  ClearWestBorder( flatquadcornerdata& );
    void  CreateChild(nFlatTerrainNode *tn, int index, flatquadcornerdata& cd); 

	void  Update(nFlatTerrainNode *tn, flatquadcornerdata& cd, vector3& vwr, float detail);
    int	  Render(nFlatTerrainNode *tn, flatquadcornerdata& cd);
    void  RenderToWF(nFlatTerrainNode *tn, flatquadcornerdata& cd);
    void  RenderToWFAux(flatquadcornerdata& cd, nFlatTerrainNode *tn);

	float GetHeight(flatquadcornerdata& cd, float x, float z);
    bool  Intersect(flatquadcornerdata& cd, line3& l, float& ipos, triangle& itri);
    
    void Save(nFlatTerrainNode *, flatquadcornerdata& cd, nFile *);      // save flatquadsquare into file
    void Load(nFlatTerrainNode *tn, flatquadcornerdata& cd, nFile *fp);  // load flatquadsquare from file

private:
	void ResetTree(nFlatTerrainNode *tn);
	void StaticCullAux(nFlatTerrainNode *tn, flatquadcornerdata& cd, float ThresholdDetail, int TargetLevel);

	void UpdateAux(nFlatTerrainNode *tn, flatquadcornerdata& cd, vector3& vwr, float CenterError, float Detail);
    void RenderAux(flatquadcornerdata& cd, Clip::Visibility vis, nFlatTerrainNode *tn);
	void SetStatic(flatquadcornerdata& cd);

    void UpdateNormal2(FlatVertInfo& vi, int tx0, int tz0, float y0, int tx1, int tz1, float y1, int tx2, int tz2, float y2);
    void InterpolateNormals(flatquadcornerdata& cd);

    void AlignSouthBordersAux(flatquadcornerdata&, flatquadcornerdata&);
    void AlignEastBordersAux(flatquadcornerdata&, flatquadcornerdata&);
    void AlignEastBorders2( nFlatTerrainNode *source, nFlatTerrainNode *dest);
    void AlignWestBordersAux(flatquadcornerdata&, flatquadcornerdata&);

    void SaveAux(nFile *);
    //-------------------------------------------------------------------
    //  GetNeighbor()
    // Traverses the tree in search of the flatquadsquare neighboring this square to the
    // specified direction.  0-3 --> { E, N, W, S }.
    // Returns NULL if the neighbor is outside the bounds of the tree.
    //
    //  12-Apr-00   floh    + wraps around if outside tree, may
    //                        still return NULL, if the quad doesn't
    //                        have a neighbor on the same level as
    //                        the original quad
    //-------------------------------------------------------------------
    flatquadsquare* flatquadsquare::GetNeighbor(int dir, flatquadcornerdata& cd); 

    
	//-------------------------------------------------------------------
    //  SetupCornerData()
    // Fills the given structure with the appropriate corner values for the
    // specified child block, given our own vertex data and our corner
    // vertex data from cd.
    //
    // ChildIndex mapping:
    // +-+-+
    // |1|0|
    // +-+-+
    // |2|3|
    // +-+-+
    //
    // Verts mapping:
    // 1-0
    // | |
    // 2-3
    //
    // Vertex mapping:
    // +-2-+
    // | | |
    // 3-0-1
    // | | |
    // +-4-+
    //-------------------------------------------------------------------
    void flatquadsquare::SetupCornerData(flatquadcornerdata *q, flatquadcornerdata& cd, int child_index) {
        int	half = 1 << cd.level;

        q->parent = &cd;
        q->square = this->GetChild(child_index);
        q->level = cd.level - 1;
        q->child_index = child_index;

        switch (child_index) {
            default:
            case 0:
                q->xorg = cd.xorg + half;
                q->zorg = cd.zorg;
                q->verts[0] = cd.verts[0];
                q->verts[1] = &(this->vertex[2]);
                q->verts[2] = &(this->vertex[0]);
                q->verts[3] = &(this->vertex[1]);
                break;

            case 1:
                q->xorg = cd.xorg;
                q->zorg = cd.zorg;
                q->verts[0] = &(this->vertex[2]);
                q->verts[1] = cd.verts[1];
                q->verts[2] = &(this->vertex[3]);
                q->verts[3] = &(this->vertex[0]);
                break;

            case 2:
                q->xorg = cd.xorg;
                q->zorg = cd.zorg + half;
                q->verts[0] = &(this->vertex[0]);
                q->verts[1] = &(this->vertex[3]);
                q->verts[2] = cd.verts[2];
                q->verts[3] = &(this->vertex[4]);
                break;

            case 3:
                q->xorg = cd.xorg + half;
                q->zorg = cd.zorg + half;
                q->verts[0] = &(this->vertex[1]);
                q->verts[1] = &(this->vertex[0]);
                q->verts[2] = &(this->vertex[4]);
                q->verts[3] = cd.verts[3];
                break;
        }	
    };

    //-------------------------------------------------------------------
    //  GetError3()
    //  Recompute error for 3 given vertices. Take spherical error
    //  into account.
    //
    //           v0
    //            |-> Error
    //  v1-----------------v2
    //
    //  12-Apr-00   floh    created
    //  11-May-00   floh    + now computes proper error in cartesian
    //                        space...
    //-------------------------------------------------------------------
    float GetError3(vector3& v0, vector3& v1, vector3& v2) {
        line3 l(v1,v2);
        return l.distance(v0);
    };
    
    //-------------------------------------------------------------------
    //  GetError4()
    //  Compute difference between bilinear average at child center, 
    //  and diagonal edge approximation.
    //  12-Apr-00   floh    created
    //-------------------------------------------------------------------
    float GetError4(vector3& v0, vector3& v1, vector3& v2, vector3& v3) {
        line3 l(v1,v3);
        vector3 v4((v0+v2)*0.5f);
        return l.distance(v4);
    };
    
    //-------------------------------------------------------------------
    //  GetChild()
    //  27-Apr-00   floh    created
    //-------------------------------------------------------------------
    flatquadsquare *GetChild(int child_index) {
        ushort i = this->child[child_index];
        if (i==0) return NULL;
        else      return &(this->qs_base[i]);
    };

    //-------------------------------------------------------------------
    //  UpdateBBox()
    //  Update bounding box of quad in cartesian space. Do not recurse
    //  down. Usually called from RecomputeError() after recursion
    //  to child nodes has been done.
    //  18-Apr-00   floh    created
    //-------------------------------------------------------------------
    void UpdateBBox(flatquadcornerdata& cd) {
        // invalidate own bounding box...
        this->bbox->begin_grow();

        // check child status
        int i;
        for (i=0; i<4; i++) {
            flatquadsquare *c = this->GetChild(i);
            if (c) {
                // child, include its bounding box
                // into our own one...
                this->bbox->grow( c->bbox );
            } else {
                // no static child, use 4 child vertices for check...
                flatquadcornerdata q;
                this->SetupCornerData(&q,cd,i);

                // build the 4 vertices making up the quad
                int whole  = 2<<q.level;
                vector3 v[4];
                int x0 = q.xorg;
                int x1 = q.xorg + whole;
                int z0 = q.zorg;
                int z1 = q.zorg + whole;
//                if (65536==z1) z1=0; // 08.24.00 -- oops! Not a sphere anymore
                v[0] = vector3(float(x1),q.verts[0]->y, float(z0));
                v[1] = vector3(float(x0),q.verts[1]->y, float(z0));
                v[2] = vector3(float(x0),q.verts[2]->y, float(z1));
                v[3] = vector3(float(x1),q.verts[3]->y, float(z1));
                this->bbox->grow(v,4);
            }
        }
    };

    //-------------------------------------------------------------------
    //  VertexTest()
    // Returns true if the vertex at (theta,rho,h) with the given world-space error between
    // its interpolated location and its true location, should be enabled, given that
    // the viewpoint is located at Viewer[].
    //-------------------------------------------------------------------
    bool flatquadsquare::VertexTest(float detail_threshold, int tx, int tz, float h, float error, vector3& viewer) {
        vector3 v((float)tx, h, (float)tz );
        float dx = (float) fabs(v.x - viewer.x);
        float dy = (float) fabs(v.y - viewer.y);
        float dz = (float) fabs(v.z - viewer.z);
        float d = dx;
        if (dy > d) d = dy;
        if (dz > d) d = dz;
        return (error * detail_threshold) > d;
    }

    //-------------------------------------------------------------------
    //  BoxTest()
    // Returns true if any vertex within the specified box (origin at x,z,
    // edges of length size) with the given error value could be enabled
    // based on the given viewer location.
    //-------------------------------------------------------------------
    bool flatquadsquare::BoxTest(float detail_threshold, float error, vector3& viewer) {
        // Find the minimum distance to the quads bounding box in cartesian space
/*        float xm = ((bbox->vmin.x+bbox->vmax.x)*0.5f);
        float ym = ((bbox->vmin.y+bbox->vmax.y)*0.5f);
        float zm = ((bbox->vmin.z+bbox->vmax.z)*0.5f);
        float dxhalf = (float) fabs(bbox->vmax.x-bbox->vmin.x)*0.5f;
        float dyhalf = (float) fabs(bbox->vmax.y-bbox->vmin.y)*0.5f;
        float dzhalf = (float) fabs(bbox->vmax.z-bbox->vmin.z)*0.5f;
*/
        float dx = (float) fabs( bbox->xm - viewer.x) - bbox->dxhalf;
        float dy = (float) fabs( bbox->ym - viewer.y) - bbox->dyhalf;
        float dz = (float) fabs( bbox->zm - viewer.z) - bbox->dzhalf;
        float d = dx;
        if (dy > d) d = dy;
        if (dz > d) d = dz;
        return (error * detail_threshold) > d;
    }

    //-------------------------------------------------------------------
    //  EnableEdgeVertex()
    // Enable the specified edge vertex.  Indices go { e, n, w, s }.
    // Increments the appropriate reference-count if IncrementCount is true.
    //-------------------------------------------------------------------
    void flatquadsquare::EnableEdgeVertex(nFlatTerrainNode *tn, int index, bool increment_count, flatquadcornerdata& cd);

   //-------------------------------------------------------------------
    //  EnableDescendant()
    // This function enables the descendant node 'count' generations below
    // us, located by following the list of child indices in path[].
    // Creates the node if necessary, and returns a pointer to it.
    //-------------------------------------------------------------------
    flatquadsquare *flatquadsquare::EnableDescendant(nFlatTerrainNode *tn, int count, int path[], flatquadcornerdata& cd) {
        count--;
        int	child_index = path[count];
        if ((this->enabled_flags & (16<<child_index)) == 0) {
            this->EnableChild(tn,child_index,cd);
        }

        if (count > 0) {
            flatquadcornerdata q;
            this->SetupCornerData((flatquadcornerdata *) &q,cd,child_index);
            if (this->GetChild(child_index)) {
                return this->GetChild(child_index)->EnableDescendant(tn,count,path,q);
            } else return NULL;
        } else {
            return this->GetChild(child_index);
        }
    }

    //-------------------------------------------------------------------
    //  EnableChild()
    // Enable the indexed child node.  { ne, nw, sw, se }
    // Causes dependent edge vertices to be enabled.
    //-------------------------------------------------------------------
    void flatquadsquare::EnableChild(nFlatTerrainNode *tn, int index, flatquadcornerdata& cd) {
        if ((this->enabled_flags & (16<<index)) == 0) {
	        if (this->child[index] == 0) {
    	        this->CreateChild(tn, index, cd);
	        }
            if (this->child[index]) {
	            this->enabled_flags |= (16 << index);
                this->EnableEdgeVertex(tn, index, true, cd);
	            this->EnableEdgeVertex(tn, (index + 1) & 3, true, cd);
            }
        }
    }

    //-------------------------------------------------------------------
    //  NotifyChildDisable()
    // Marks the indexed child quadrant as disabled.  Deletes the child node
    // if it isn't static.
    //-------------------------------------------------------------------
    void flatquadsquare::NotifyChildDisable(nFlatTerrainNode *tn, flatquadcornerdata& cd, int index) {
        // Clear enabled flag for the child.
        this->enabled_flags &= ~(16 << index);

        // Update child enabled counts for the affected edge verts.
        flatquadsquare*	s;
        if (index & 2) s = this;
        else s = this->GetNeighbor(1, cd);
        if (s) {
            s->sub_enabled_count[1]--;
        }
	    
        if ((index==1) || (index==2)) s = this->GetNeighbor(2, cd);
        else s = this;
        if (s) {
            s->sub_enabled_count[0]--;
        }
	    
        flatquadsquare *ch = this->GetChild(index);
        if (!(ch->flags & STATIC)) {
            ch->Release(tn);
            this->child[index] = 0;
        }
    }

    //-------------------------------------------------------------------
    //  NotifyChildDisable()
    // Marks the indexed child quadrant as disabled.  Deletes the child node
    // even if it isn't static.
    //-------------------------------------------------------------------
    void flatquadsquare::NotifyChildDisableAux(nFlatTerrainNode *tn, flatquadcornerdata& cd, int index) {
        // Clear enabled flag for the child.
        this->enabled_flags &= ~(16 << index);

        // Update child enabled counts for the affected edge verts.
        flatquadsquare*	s;
        if (index & 2) s = this;
        else s = this->GetNeighbor(1, cd);
        if (s) {
            s->sub_enabled_count[1]--;
        }
	    
        if ((index==1) || (index==2)) s = this->GetNeighbor(2, cd);
        else s = this;
        if (s) {
            s->sub_enabled_count[0]--;
        }
	    
        flatquadsquare *ch = this->GetChild(index);
        ch->Release(tn);
        this->child[index] = 0;
    }
};

//-------------------------------------------------------------------
#endif // QUADTREE_H
