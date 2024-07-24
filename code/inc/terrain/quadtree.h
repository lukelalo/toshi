#ifndef N_QUADTREE_H
#define N_QUADTREE_H
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

#ifndef N_BBOX_H
#include "mathlib/bbox.h"
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
class HeightMapInfo {
public:
    float *data;
    int x_origin,z_origin;
    int	x_size, z_size;
    int	row_width;
    int	scale;
    
public:
    HeightMapInfo() {
        this->data = NULL;
    };
    HeightMapInfo(int xorig, int zorig, int xsize, int zsize, int s) {
        this->x_origin       = xorig;
        this->z_origin       = zorig;
        this->x_size         = xsize;
        this->z_size         = zsize;
        this->scale          = s;
        this->row_width = xsize;
        this->data = new float[this->x_size*this->z_size];
    };
    ~HeightMapInfo() {
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

        // wrap around only the z dimension (around the equator)
        if (ix1 < 0) ix1 = 0;
        if (iz1 < 0) iz1 = this->z_size - 1;
        if (ix1 >= this->x_size) ix1 = this->x_size - 1;
        if (iz1 >= this->z_size) iz1 = 0;

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
//  VertInfo
//  Packed into 8 bytes.
//-------------------------------------------------------------------
struct VertInfo {
    ushort y;               // the vertex' y coordinate
    pknorm3 n;              // the vertex normal for lighting packed into 16 bits
    ushort vpass,vindex;    // information for vertex buffer index reuseage
    vector3 cart;           // the vertice's cached cartesian coordinate
    ushort u,v;             // the vertices uv coordinate
};

//-------------------------------------------------------------------
//  A structure used during recursive traversal of the tree to hold
//  relevant but transitory data.
//  27-Mar-00   floh    VertInfo contains pointers, not copies
//-------------------------------------------------------------------
class quadsquare;
struct quadcornerdata {
    quadcornerdata *parent;
    quadsquare *square;
    int	child_index;
    int	level;
    int xorg, zorg;
    VertInfo *verts[4];  // ne, nw, sw, se
};

//-------------------------------------------------------------------
//  quadsquare
//  FIXME: size optimization!
//-------------------------------------------------------------------
class nTerrainNode;
class quadsquare {
public:
    union {
        ushort child[4];        // index of child quads into qs_base array (0 is reserved for root node!)
        quadsquare *next;       // this is for linkage into the quad allocator list only
    };
    VertInfo vertex[5];         // center, e, n, w, s
    ushort error[6];            // e, s, children: ne, nw, sw, se
    bbox3 bbox;
    unsigned char enabled_flags;    // bits 0-7: e, n, w, s, ne, nw, sw, se
    unsigned char sub_enabled_count[2]; // e, s enabled reference counts.
    unsigned char flags;
    enum {
        STATIC = (1<<0),            // static node (may not be deleted)
        DIRTY  = (1<<1),            // Set when vertex data has changed, but error/enabled data has not been recalculated.
    };
    quadsquare *qs_base;            // base pointer to array of quadsquare nodes

    static float *sin_lookup;       // sinus-lookup-table
    static float *cos_lookup;       // cosinus-lookup-table
    enum {
        LOOKUP_SHIFT = 13,
        LOOKUP_ENTRIES = (1<<LOOKUP_SHIFT),
    };

    quadsquare();
    void  Init(quadcornerdata *pcd, quadsquare *);
    void  Release(nTerrainNode *tn);
    void  AddHeightMap(nTerrainNode *tn, quadcornerdata& cd, HeightMapInfo& hm);
	void  StaticCullData(nTerrainNode *tn, quadcornerdata& cd, float threshold_detail);	
	ushort RecomputeError(quadcornerdata& cd);
	int	  CountNodes();
    void  AlignBorders(quadcornerdata&);
    void  CreateChild(nTerrainNode *tn, int index, quadcornerdata& cd); 

	void  Update(nTerrainNode *tn, quadcornerdata& cd, vector3& vwr, float detail);
    int	  Render(nTerrainNode *tn, quadcornerdata& cd);
    void  RenderToWF(nTerrainNode *tn, quadcornerdata& cd);
    void  RenderToWFAux(quadcornerdata& cd, nTerrainNode *tn);

	float GetHeight(quadcornerdata& cd, float x, float z);
    bool  Intersect(quadcornerdata& cd, line3& l, float& ipos, triangle& itri);
    
    void Save(nTerrainNode *, quadcornerdata& cd, nFile *);      // save quadsquare into file
    void Load(nTerrainNode *tn, quadcornerdata& cd, nFile *fp);  // load quadsquare from file

    //-------------------------------------------------------------------
    //  GetCartesian()
    //  Get cartesian coordinates from terrain x/z coordinates
    //  and heights. x,z and height range from 0..65536. The
    //  output cartesion coordinates range from -65536.0f to +65536.0f.
    //  05-Apr-00   floh    created
    //-------------------------------------------------------------------
    void GetCartesian(unsigned int tx, unsigned int tz, float h, vector3& v)
    {
        if      (tz == 65536) tz=0;
        if      (tx == 0)     tz=0;
        else if (tx == 65536) tz=0;
        int stx = tx >> (17-LOOKUP_SHIFT);      // range from 0..180 degree
        int stz = tz >> (16-LOOKUP_SHIFT);      // range from 0..360 degree
        float sin_theta = this->sin_lookup[stx];
        float cos_theta = this->cos_lookup[stx];
        float sin_rho   = this->sin_lookup[stz];
        float cos_rho   = this->cos_lookup[stz];
        v.set((sin_theta*sin_rho)*h, cos_theta*h, (sin_theta*cos_rho)*h);
    };
    vector3 GetCartesian(unsigned int tx, unsigned int tz, float h)
    {
        if      (tz == 65536) tz=0;
        if      (tx == 0)     tz=0;
        else if (tx == 65536) tz=0;
        int stx = tx >> (17-LOOKUP_SHIFT);      // range from 0..180 degree
        int stz = tz >> (16-LOOKUP_SHIFT);      // range from 0..360 degree
        float sin_theta = this->sin_lookup[stx];
        float cos_theta = this->cos_lookup[stx];
        float sin_rho   = this->sin_lookup[stz];
        float cos_rho   = this->cos_lookup[stz];

        vector3 v((sin_theta*sin_rho)*h, cos_theta*h, (sin_theta*cos_rho)*h);

        return v;
    };

private:
	void ResetTree(nTerrainNode *tn);
	void StaticCullAux(nTerrainNode *tn, quadcornerdata& cd, float ThresholdDetail, int TargetLevel);

	void UpdateAux(nTerrainNode *tn, quadcornerdata& cd, vector3& vwr, float CenterError, float Detail);
    void RenderAux(quadcornerdata& cd, Clip::Visibility vis, nTerrainNode *tn);
	void SetStatic(quadcornerdata& cd);

    void UpdateNormal2(VertInfo& vi, int tx0, int tz0, float y0, int tx1, int tz1, float y1, int tx2, int tz2, float y2);
    void InterpolateNormals(quadcornerdata& cd);

    void AlignSouthBordersAux(quadcornerdata&);
    void AlignEastBordersAux(quadcornerdata&);
    void AlignWestBordersAux(quadcornerdata&);

    void SaveAux(nFile *);

    //-------------------------------------------------------------------
    //  GetNeighbor()
    // Traverses the tree in search of the quadsquare neighboring this square to the
    // specified direction.  0-3 --> { E, N, W, S }.
    // Returns NULL if the neighbor is outside the bounds of the tree.
    //
    //  12-Apr-00   floh    + wraps around if outside tree, may
    //                        still return NULL, if the quad doesn't
    //                        have a neighbor on the same level as
    //                        the original quad
    //-------------------------------------------------------------------
    quadsquare*	quadsquare::GetNeighbor(int dir, quadcornerdata& cd) {
        // If we don't have a parent, then we don't have a neighbor.
        // (Actually, we could have inter-tree connectivity at this level
        // for connecting separate trees together.)
        // if (cd.parent == NULL) return NULL;

        // Find the parent and the child-index of the square we want to locate or create.
        quadsquare*	p = NULL;

        int index = cd.child_index ^ 1 ^ ((dir & 1) << 1);
        bool same_parent = ((dir - cd.child_index) & 2) ? true : false;

        if (NULL == cd.parent) {
            return cd.square;
        } else if (same_parent) {
            p = cd.parent->square;
        } else {
            p = cd.parent->square->GetNeighbor(dir, *cd.parent);
            if (p == 0) return 0;
        }

        quadsquare*	n = p->GetChild(index);
        return n;
    };

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
    void quadsquare::SetupCornerData(quadcornerdata *q, quadcornerdata& cd, int child_index) {
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
    quadsquare *GetChild(int child_index) {
        ushort i = this->child[child_index];
        if (i==0) return NULL;
        else      return &(this->qs_base[i]);
    };

    //-------------------------------------------------------------------
    //  UpdateCartesians()
    //  Update the cartesian coordinates of the quad, and the cached
    //  spherical coordinates of the quad.
    //  18-Aug-00   floh    created
    //-------------------------------------------------------------------
    void UpdateCartesians(quadcornerdata& cd) {
        int half  = 1<<cd.level;
        int whole = half<<1;
        int x0 = cd.xorg;
        int x1 = cd.xorg + half;
        int x2 = cd.xorg + whole;
        int z0 = cd.zorg;
        int z1 = cd.zorg + half;
        int z2 = cd.zorg + whole;
        // update the cartesian coordinates
        this->GetCartesian(x1,z1,this->vertex[0].y,this->vertex[0].cart);
        this->GetCartesian(x2,z1,this->vertex[1].y,this->vertex[1].cart);
        this->GetCartesian(x1,z0,this->vertex[2].y,this->vertex[2].cart);
        this->GetCartesian(x0,z1,this->vertex[3].y,this->vertex[3].cart);
        this->GetCartesian(x1,z2,this->vertex[4].y,this->vertex[4].cart);
        // update the uv coords
        x0>>=8; x1>>=8; x2>>=8;
        z0>>=8; z1>>=8; z2>>=8;
        this->vertex[0].u=x1; this->vertex[0].v=z1;
        this->vertex[1].u=x2; this->vertex[1].v=z1;
        this->vertex[2].u=x1; this->vertex[2].v=z0;
        this->vertex[3].u=x0; this->vertex[3].v=z1;
        this->vertex[4].u=x1; this->vertex[4].v=z2;
    }; 

    //-------------------------------------------------------------------
    //  UpdateBBox()
    //  Update bounding box of quad in cartesian space. Do not recurse
    //  down. Usually called from RecomputeError() after recursion
    //  to child nodes has been done.
    //  18-Apr-00   floh    created
    //-------------------------------------------------------------------
    void UpdateBBox(quadcornerdata& cd) {
        // invalidate own bounding box...
        this->bbox.begin_grow();

        // check child status
        int i;
        for (i=0; i<4; i++) {
            quadsquare *c = this->GetChild(i);
            if (c) {
                // child, include its bounding box
                // into our own one...
                this->bbox.grow(c->bbox);
            } else {
                // no static child, use 4 child vertices for check...
                quadcornerdata q;
                this->SetupCornerData(&q,cd,i);

                // build the 4 cartesian space vertices making up the quad
                int whole = 2<<q.level;
                int x0 = q.xorg;
                int x1 = q.xorg + whole;
                int z0 = q.zorg;
                int z1 = q.zorg + whole;
                if (65536==z1) z1=0;
                vector3 v[4];
                this->GetCartesian(x1,z0,q.verts[0]->y,v[0]);
                this->GetCartesian(x0,z0,q.verts[1]->y,v[1]);
                this->GetCartesian(x0,z1,q.verts[2]->y,v[2]);
                this->GetCartesian(x1,z1,q.verts[3]->y,v[3]);
                this->bbox.grow(v,4);
            }
        }
    };

    //-------------------------------------------------------------------
    //  VertexTest()
    // Returns true if the vertex at (theta,rho,h) with the given world-space error between
    // its interpolated location and its true location, should be enabled, given that
    // the viewpoint is located at Viewer[].
    //-------------------------------------------------------------------
    bool quadsquare::VertexTest(float detail_threshold, int tx, int tz, float h, float error, vector3& viewer) {
        vector3 v(this->GetCartesian(tx,tz,h));
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
    bool quadsquare::BoxTest(float detail_threshold, float error, vector3& viewer) {
        // Find the minimum distance to the quads bounding box in cartesian space
        float xm = ((bbox.vmin.x+bbox.vmax.x)*0.5f);
        float ym = ((bbox.vmin.y+bbox.vmax.y)*0.5f);
        float zm = ((bbox.vmin.z+bbox.vmax.z)*0.5f);
        float dxhalf = (float) fabs(bbox.vmax.x-bbox.vmin.x)*0.5f;
        float dyhalf = (float) fabs(bbox.vmax.y-bbox.vmin.y)*0.5f;
        float dzhalf = (float) fabs(bbox.vmax.z-bbox.vmin.z)*0.5f;
        float dx = (float) fabs(xm - viewer.x) - dxhalf;
        float dy = (float) fabs(ym - viewer.y) - dyhalf;
        float dz = (float) fabs(zm - viewer.z) - dzhalf;
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
    void quadsquare::EnableEdgeVertex(nTerrainNode *tn, int index, bool increment_count, quadcornerdata& cd) {
        if ((this->enabled_flags & (1<<index)) && (increment_count == false)) return;

        // Turn on flag and deal with reference count.
        this->enabled_flags |= (1 << index);
        if ((increment_count==true) && ((index==0) || (index==3))) {
            this->sub_enabled_count[index & 1]++;
        }

        // Now we need to enable the opposite edge vertex of the adjacent square (i.e. the alias vertex).

        // This is a little tricky, since the desired neighbor node may not exist, in which
        // case we have to create it, in order to prevent cracks.  Creating it may in turn cause
        // further edge vertices to be enabled, propagating updates through the tree.

        // The sticking point is the quadcornerdata list, which
        // conceptually is just a linked list of activation structures.
        // In this function, however, we will introduce branching into
        // the "list", making it in actuality a tree.  This is all kind
        // of obscure and hard to explain in words, but basically what
        // it means is that our implementation has to be properly
        // recursive.

        // Travel upwards through the tree, looking for the parent in common with our desired neighbor.
        // Remember the path through the tree, so we can travel down the complementary path to get to the neighbor.
        quadsquare* p = this;
        quadcornerdata *pcd = &cd;
        int ct = 0;
        int stack[32];
        for (;;) {
            if (pcd->parent == NULL || pcd->parent->square == NULL) {
                // Neighbor doesn't exist (it's outside the tree), so there's no alias vertex to enable.
                // return;

                // neighbor is outside tree, simply wrap around on the other side since
                // we live on a sphere!
                if (ct == 0) return;    // top level no neighbor
                break;
            }
            int	ci = pcd->child_index;
            bool same_parent = ((index - ci) & 2) ? true : false;

            ci = ci ^ 1 ^ ((index & 1) << 1);	// Child index of neighbor node.

            stack[ct] = ci;
            ct++;

            pcd = pcd->parent;
            p = pcd->square;
            if (same_parent) break;
        }

        // Get a pointer to our neighbor (create if necessary), by walking down
        // the quadtree from our shared ancestor.
        p = p->EnableDescendant(tn,ct,stack,*pcd);
	    
        // Finally: enable the vertex on the opposite edge of our neighbor, the alias of the original vertex.
        if (p) {
            int opp_index = index ^ 2;
            p->enabled_flags |= (1 << opp_index);
            if ((increment_count==true) && ((opp_index==0) || (opp_index==3))) {
                p->sub_enabled_count[opp_index & 1]++;
            }
        }
    }

    //-------------------------------------------------------------------
    //  EnableDescendant()
    // This function enables the descendant node 'count' generations below
    // us, located by following the list of child indices in path[].
    // Creates the node if necessary, and returns a pointer to it.
    //-------------------------------------------------------------------
    quadsquare *quadsquare::EnableDescendant(nTerrainNode *tn, int count, int path[], quadcornerdata& cd) {
        count--;
        int	child_index = path[count];
        if ((this->enabled_flags & (16<<child_index)) == 0) {
            this->EnableChild(tn,child_index,cd);
        }

        if (count > 0) {
            quadcornerdata q;
            this->SetupCornerData((quadcornerdata *) &q,cd,child_index);
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
    void quadsquare::EnableChild(nTerrainNode *tn, int index, quadcornerdata& cd) {
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
    void quadsquare::NotifyChildDisable(nTerrainNode *tn, quadcornerdata& cd, int index) {
        // Clear enabled flag for the child.
        this->enabled_flags &= ~(16 << index);

        // Update child enabled counts for the affected edge verts.
        quadsquare*	s;
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
	    
        quadsquare *ch = this->GetChild(index);
        if (!(ch->flags & STATIC)) {
            ch->Release(tn);
            this->child[index] = 0;
        }
    }
};

//-------------------------------------------------------------------
#endif // QUADTREE_H
