#define N_IMPLEMENTS nFlatTerrainNode
//-------------------------------------------------------------------
// quadtree.cpp	-thatcher 9/15/1999 Copyright 1999-2000 Thatcher Ulrich
//
// Code for quadtree terrain manipulation, meshing, and display.
//
// This code may be freely modified and redistributed.  I make no
// warrantees about it; use at your own risk.  If you do incorporate
// this code into a project, I'd appreciate a mention in the credits.
//
// Thatcher Ulrich <tu@tulrich.com>
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// flat_quadtree.cc -wgold 8/22/2000
//
// Reengineered back into a flat terrain renderer from the example in
// use as the Nomad's spherical terrain renderer.  8/15/00
//
// Weld edges of neighboring trees together by aligning vertex height
// and normals. Not seamless yet, still trying. 8/18/00
//
// Per Thatcher's suggestion, seamless welding of trees was added by
// allowing the GetNeighbor() and EnableEdgeVertex() calls to return 
// the tree that is their neighbor - and generate quads to put in the
// gaps.  
//
// Since there was currently no way to sense who our neighbors were,
// there was a choice to be made.  Either store 4 new pointers to our
// neighboring flatquadtree structure, or put those pointers somewhere
// accessible.  The tradeoff I chose was to give each quadtree a 
// pointer to it's master nFlatTerrainNode who would store the references
// in the new public nFlatTerrainNode *neighbors[] array.
// This caused a 4 byte increase in quad size, making it 96 bytes per quad.  
// Optimization would be nice, but right now a 64x64 quadtree (loaded as a 
// binary tree, not a bitmap) takes <=> 220K
// 
//-------------------------------------------------------------------


#include <stdio.h>
#include <math.h>
#include "terrain/flat_quadtree.h"
#include "terrain/nflatterrainnode.h"

//-------------------------------------------------------------------
//  flatquadsquare()
//  19-Apr-00   floh    created
//-------------------------------------------------------------------
flatquadsquare::flatquadsquare()
{
    this->next = NULL;
	this->master = NULL;
}

//-------------------------------------------------------------------
//  Init()
//  Initialize quad for usage (pseudo constructor).
//  19-Apr-00   floh    created  
//-------------------------------------------------------------------
void flatquadsquare::Init(flatquadcornerdata* pcd, flatquadsquare *qsbase, nFlatTerrainNode *master)
{
    int i;
    pcd->square   = this;
    this->qs_base = qsbase;
	this->master = master;
	this->bbox = new bbox3_qs;
    // Set default vertex positions by interpolating from given corners.
    // Just bilinear interpolation.

    // set default vertex position by interpolating from given corners
    // and add spherical displacement
    this->vertex[0].y = ushort(0.25f * (pcd->verts[0]->y + pcd->verts[1]->y + pcd->verts[2]->y + pcd->verts[3]->y));
    this->vertex[1].y = ushort(0.5f * (pcd->verts[3]->y + pcd->verts[0]->y));
    this->vertex[2].y = ushort(0.5f * (pcd->verts[0]->y + pcd->verts[1]->y));
    this->vertex[3].y = ushort(0.5f * (pcd->verts[1]->y + pcd->verts[2]->y));
    this->vertex[4].y = ushort(0.5f * (pcd->verts[2]->y + pcd->verts[3]->y));
    for (i=0; i<5; i++) {
        this->vertex[i].vpass  = 0;
        this->vertex[i].vindex = 0;
    }

    // Set static to true if/when this node contains real data, and
    // not just interpolated values.  When static == false, a node
    // can be deleted by the Update() function if none of its
    // vertices or children are enabled.
    this->flags = 0;
    this->enabled_flags = 0;

    for (i=0; i<4; i++) {
        this->child[i] = 0;
    }
    for (i=0; i<2; i++) {
	    this->sub_enabled_count[i] = 0;
    }
    for (i=0; i<2; i++) {
        this->error[i] = 0;
    }

    for (i=0; i<4; i++) {
        // FIXME: normally, a GetError4() was computed here. Leaving
        // this out got rid of the excessive allocation of dynamic
        // normals, so what :)
        this->error[i+2] = 0;
    }

    // update the quad's bounding box...
    this->UpdateBBox(*pcd);

    // Compute lighting...
    this->InterpolateNormals(*pcd);
}

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
flatquadsquare* flatquadsquare::GetNeighbor(int dir, flatquadcornerdata& cd) {
    // If we don't have a parent, then we don't have a neighbor.
    // (Actually, we could have inter-tree connectivity at this level
    // for connecting separate trees together.)


    // Find the parent and the child-index of the square we want to locate or create.
	flatquadcornerdata* q = NULL;
    flatquadsquare*	p = NULL;



    int index = cd.child_index ^ 1 ^ ((dir & 1) << 1);
    bool same_parent = ((dir - cd.child_index) & 2) ? true : false;

    if (NULL == cd.parent ) 
	{
		// If we are neighbored by another, our parent nFlatTerrainNode will know about it
		q = this->master->neighbors[ dir ];
		return ( q == NULL ? NULL : q->square);
	} else if (same_parent) {
        p = cd.parent->square;
    } else {
        p = cd.parent->square->GetNeighbor(dir, *cd.parent);
        if (p == 0) return 0;
    }

    flatquadsquare*	n = p->GetChild(index);
    return n;
};

//-------------------------------------------------------------------
//  EnableEdgeVertex()
// Enable the specified edge vertex.  Indices go { e, n, w, s }.
// Increments the appropriate reference-count if IncrementCount is true.
//-------------------------------------------------------------------
void flatquadsquare::EnableEdgeVertex(nFlatTerrainNode * /* unused tn */,
                                      int index,
                                      bool increment_count,
                                      flatquadcornerdata& cd) {
    if ((this->enabled_flags & (1<<index)) && (increment_count == false))
        return;

    // Turn on flag and deal with reference count.
    this->enabled_flags |= (1 << index);
    if ((increment_count==true) && ((index==0) || (index==3))) {
        this->sub_enabled_count[index & 1]++;
    }

    // Now we need to enable the opposite edge vertex of the adjacent square (i.e. the alias vertex).

    // This is a little tricky, since the desired neighbor node may not exist, in which
    // case we have to create it, in order to prevent cracks.  Creating it may in turn cause
    // further edge vertices to be enabled, propagating updates through the tree.

    // The sticking point is the flatquadcornerdata list, which
    // conceptually is just a linked list of activation structures.
    // In this function, however, we will introduce branching into
    // the "list", making it in actuality a tree.  This is all kind
    // of obscure and hard to explain in words, but basically what
    // it means is that our implementation has to be properly
    // recursive.


    // Travel upwards through the tree, looking for the parent in common with our desired neighbor.
    // Remember the path through the tree, so we can travel down the complementary path to get to the neighbor.
    flatquadsquare* p = this;

    flatquadcornerdata *pcd = &cd;

    int ct = 0;

    int stack[32];

    for (;;) {

        if (pcd->parent == NULL || pcd->parent->square == NULL) {

		   // If there's no neighbor - there's no neighbor, simple as that.
		   // Otherwise - if we've been told to "weld" to another neighbor
		   // their corner data will be held inside our master's neighbors[]
		   // array.  WG 8/19/00
		   if ( !master || !master->neighbors[ index ] )
			   return;
		
		   p = master->neighbors[ index ]->square;
		   pcd = master->neighbors[ index ];
		   break;

		   // neighbor is outside tree, simply wrap around on the other side since
           // we live on a sphere! -- no longer true - WG 
		   //                if (ct == 0) return;    // top level no neighbor
		   //                break;
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

	if (ct)
	    p = p->EnableDescendant(p->master,ct,stack,*pcd);

	

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
//  Release()
//  Release the quad and all of its children through
//  nFlatTerrainNode::Freeflatquadsquare().
//  19-Apr-00   floh    created
//-------------------------------------------------------------------
void flatquadsquare::Release(nFlatTerrainNode *tn)
{
    // Recursively delete sub-trees.
    int	i;
    for (i=0; i<4; i++) {
	    if (this->child[i]) this->GetChild(i)->Release(tn);
	    this->child[i] = 0;
    }
    tn->FreeQuadSquare(this);
	delete this->bbox;

}

//-------------------------------------------------------------------
//  CreateChild()
// Creates a child square at the specified index.
//-------------------------------------------------------------------
void flatquadsquare::CreateChild(nFlatTerrainNode *tn, int index, flatquadcornerdata& cd) 
{
    if (0 == this->child[index]) {
	    flatquadcornerdata q;
	    this->SetupCornerData(&q,cd,index);
	    this->child[index] = tn->AllocQuadSquare(&q);
    }
}

//-------------------------------------------------------------------
//  SetStatic()
// Sets this node's static flag to true.  If static == true, then the
// node or its children is considered to contain significant height data
// and shouldn't be deleted.
//-------------------------------------------------------------------
void flatquadsquare::SetStatic(flatquadcornerdata& cd)
{
    if (!(this->flags & STATIC)) {
	    this->flags |= STATIC;
	    
	    // Propagate static status to ancestor nodes.
	    if (cd.parent && cd.parent->square) {
		    cd.parent->square->SetStatic(*cd.parent);
	    }
    }
}

//-------------------------------------------------------------------
//  CountNodes()
// Debugging function.  Counts the number of nodes in this subtree.
//-------------------------------------------------------------------
int	flatquadsquare::CountNodes()
{
    int	count = 1;	// Count ourself.

    // Count descendants.
    int i;
    for (i=0; i<4; i++) {
	    if (this->child[i]) count += this->GetChild(i)->CountNodes();
    }

    return count;
}

//-------------------------------------------------------------------
//  UpdateNormal2()
//  Update the normal in a VertInfo.
//  29-Mar-00   floh    created
//  05-Apr-00   floh    + invert-Flag
//-------------------------------------------------------------------
void flatquadsquare::UpdateNormal2(FlatVertInfo& vi, 
                               int tx0, int tz0, float y0,
                               int tx1, int tz1, float y1,
                               int tx2, int tz2, float y2)
{
//    if (tx0 == 0) {
//        vector3 n(0.0f, 1.0f, 0.0f);
//        vi.n.pack(n);
//    } else if (tx0 == 65536) {
//        vector3 n(0.0f, 1.0f, 0.0f);
//        vi.n.pack(n);
//    } else {

        // convert the 3 vertex points into world-space coordinates

		// NOTE: This doesn't feel right.. For some reason
		// I couldn't get my head around the way normals were 
		// being generated on the spherical terrain so I was 
		// inable to get the normals to look right.  
		// I threw out the old method and went to a more
		// brute force normalizing.
		// Please fix this if you know a better and more 
		// efficient way! (Note - Happens ONCE at load time)
		// 
		// WG 8/23/00

		vector3 v0( float(tx1 - tx0), y1 - y0, (float(tz1 - tz0)) );
		vector3 v1( float(tx2 - tx1), y2 - y1, (float(tz2 - tz1)) );

		vector3 v2(v0.y*v1.z-v0.z*v1.y,
				   v0.z*v1.x-v0.x*v1.z,
	               v0.x*v1.y-v0.y*v1.x);

//		vector3 v0(-float(tx0), y0, -tz0);
//      vector3 v1(-float(tx1), y1, -tz1);
//      vector3 v2(-float(tx2), y2, -tz2);

      // create the normal from the 3 vectors
//        vector3 n((v2-v0) * (v1-v0));
        v2.norm();
        vi.n.pack(v2);
//    }
}

//-------------------------------------------------------------------
//  InterpolateNormals()
//  Generate approximated normals of (usually) non-static nodes from
//  quadcorner normals.
//  30-Mar-00   floh    created
//-------------------------------------------------------------------
void flatquadsquare::InterpolateNormals(flatquadcornerdata& cd)
{
    vector3 cv0(cd.verts[0]->n.unpack());
    vector3 cv1(cd.verts[1]->n.unpack());
    vector3 cv2(cd.verts[2]->n.unpack());
    vector3 cv3(cd.verts[3]->n.unpack());

    vector3 v0(cv0+cv1+cv2+cv3);
    v0.norm();
    this->vertex[0].n.pack(v0);

    vector3 v1(cv3+cv0);
    v1.norm();
    this->vertex[1].n.pack(v1);

    vector3 v2(cv0+cv1);
    v2.norm();
    this->vertex[2].n.pack(v2);

    vector3 v3(cv1+cv2);
    v3.norm();
    this->vertex[3].n.pack(v3);

    vector3 v4(cv2+cv3);
    v4.norm();
    this->vertex[4].n.pack(v4);
}

//-------------------------------------------------------------------
//  RecomputeError()
//  Recomputes the error values for this tree.  Returns the
//  max error.
//  29-Mar-00   floh    added normal computation
//  19-Apr-00   floh    updates cartesian space bbox as well
//  11-May-00   floh    Miny/Maxy gekillt
//-------------------------------------------------------------------
ushort flatquadsquare::RecomputeError(flatquadcornerdata& cd)
{
    int	i;

    // Measure error of center and edge vertices.
    float maxerror = 0.0f;

    // compute coordinates in cartesian space
    int half  = 1<<cd.level;
    int whole = half<<1;
    vector3 v[9];
    float x0 = float(cd.xorg);
    float x1 = float(cd.xorg + half);
    float x2 = float(cd.xorg + whole);
    float z0 = float(cd.zorg);
    float z1 = float(cd.zorg + half);
    float z2 = float(cd.zorg + whole);
    
    v[0] = vector3(x1, z1, this->vertex[0].y);
    v[1] = vector3(x2, z1, this->vertex[1].y);
    v[2] = vector3(x2, z0, cd.verts[0]->y);   
    v[3] = vector3(x1, z0, this->vertex[2].y);
    v[4] = vector3(x0, z0, cd.verts[1]->y);   
    v[5] = vector3(x0, z1, this->vertex[3].y);
    v[6] = vector3(x0, z2, cd.verts[2]->y);   
    v[7] = vector3(x1, z2, this->vertex[4].y);
    v[8] = vector3(x2, z2, cd.verts[3]->y);   

    // Compute error of center vert.
    float e;
    if (cd.child_index & 1) {
        e = this->GetError3(v[0], v[4], v[8]);
    } else {
        e = this->GetError3(v[0], v[2], v[6]);
    }
    if (e > maxerror) maxerror = e;

    // Edge verts.
    e = this->GetError3(v[1], v[2], v[8]);
    if (e > maxerror) maxerror = e;
    this->error[0] = (ushort) e;

    e = this->GetError3(v[7], v[6], v[8]);
    if (e > maxerror) maxerror = e;
    this->error[1] = (ushort) e;

    // Check child squares.
    for (i=0; i<4; i++) {
        flatquadcornerdata q;
        if (this->child[i]) {
            this->SetupCornerData(&q,cd,i);
            flatquadsquare *ch = this->GetChild(i);
            this->error[i+2] = ch->RecomputeError(q);
        } else {
            // Compute difference between bilinear average at child center, and diagonal edge approximation.
            //this->error[i+2] = 0;
            this->error[i+2] = (ushort) this->GetError4(v[0], v[i*2+1], v[i*2+2], v[(i*2+3)&7]);
            //this->error[i+2] = (ushort) this->GetError4(cd, &this->vertex[0], cd.verts[i],
            //                   &this->vertex[i+1], &this->vertex[((i+1)&3)+1]);
        }
        if (this->error[i+2] > maxerror) maxerror = this->error[i+2];
    }

    // update the quads bounding box (must be called after recursing to children)
    this->UpdateBBox(cd);

    // The error this node and descendants is correct now.
    this->flags &= ~DIRTY;
    return (ushort) maxerror;
}

//-------------------------------------------------------------------
//  ResetTree()
// Clear all enabled flags, and delete all non-static child nodes.
//-------------------------------------------------------------------
void flatquadsquare::ResetTree(nFlatTerrainNode *tn)
{
    int	i;
    for (i = 0; i < 4; i++) {
        if (this->child[i]) {
            flatquadsquare *ch = this->GetChild(i);
            ch->ResetTree(tn);
            if (!(ch->flags & STATIC)) {
                ch->Release(tn);
                this->child[i] = 0;
            }
        }
    }
    this->enabled_flags = 0;
    this->sub_enabled_count[0] = 0;
    this->sub_enabled_count[1] = 0;
    this->flags |= DIRTY;
}

//-------------------------------------------------------------------
//  StaticCullData()
// Examine the tree and remove nodes which don't contain necessary
// detail.  Necessary detail is defined as vertex data with a
// edge-length to height ratio less than ThresholdDetail.
//-------------------------------------------------------------------
void flatquadsquare::StaticCullData(nFlatTerrainNode *tn, flatquadcornerdata& cd, float threshold_detail)
{
    // First, clean non-static nodes out of the tree.
    this->ResetTree(tn);

    // Make sure error values are up-to-date.
    if (this->flags & DIRTY) this->RecomputeError(cd);

    // Recursively check all the nodes and do necessary removal.
    // We must start at the bottom of the tree, and do one level of
    // the tree at a time, to ensure the dependencies are accounted
    // for properly.
    int	level;
    for (level=0; level<15; level++) {
        this->StaticCullAux(tn, cd, threshold_detail, level);
    }
}

//-------------------------------------------------------------------
//  StaticCullAux()
// Check this node and its descendents, and remove nodes which don't contain
// necessary detail.
//-------------------------------------------------------------------
void flatquadsquare::StaticCullAux(nFlatTerrainNode *tn, flatquadcornerdata& cd, float threshold_detail, int target_level)
{
    int	i,j;
    flatquadcornerdata q;
    if (cd.level > target_level) {
        // Just recurse to child nodes.
        for (j=0; j<4; j++) {
            if (j<2) i=1-j;
            else     i=j;
            if (this->child[i]) {
                this->SetupCornerData(&q,cd,i);
                this->GetChild(i)->StaticCullAux(tn,q,threshold_detail,target_level);
            }
        }
        return;
    }

    // We're at the target level.  Check this node to see if it's OK to delete it.

    // Check edge vertices to see if they're necessary.
    float size = (float) (2 << cd.level); // Edge length.
    if ((0==this->child[0]) && (0==this->child[3]) && ((this->error[0]*threshold_detail)<size)) {
        flatquadsquare *s = this->GetNeighbor(0, cd);
        if ((s==NULL) || ((0==s->child[1]) && (0==s->child[2]))) {

            // Force vertex height to the edge value.
            int half  = 1<<cd.level;
            int whole = half<<1;
            ushort y = ushort((cd.verts[0]->y + cd.verts[3]->y) * 0.5f);
            this->vertex[1].y = y;
            vector3 v0 = vector3(float(cd.xorg+whole),float(cd.zorg+half),y);
            vector3 v1 = vector3(float(cd.xorg+whole),float(cd.zorg),cd.verts[0]->y);
            vector3 v2 = vector3(float(cd.xorg+whole),float(cd.zorg+whole),cd.verts[3]->y);
            this->error[0] = (ushort) this->GetError3(v0,v1,v2);

            // Force alias vertex to match.
            if (s) {
                s->vertex[3].y = y;
                s->vertex[3].n = this->vertex[1].n;
            }
            this->flags |= DIRTY;
        }
    }

    if ((0==this->child[2]) && (0==this->child[3]) && ((this->error[1]*threshold_detail)<size)) {
        flatquadsquare *s = this->GetNeighbor(3,cd);
        if ((s==NULL) || ((0==s->child[0]) && (0==s->child[1]))) {

            int half  = 1<<cd.level;
            int whole = half<<1;
            ushort y = ushort((cd.verts[2]->y + cd.verts[3]->y) * 0.5f);
            this->vertex[4].y = y;

            vector3 v0 = vector3(float(cd.xorg+half),float(cd.zorg+whole),y);
            vector3 v1 = vector3(float(cd.xorg),float(cd.zorg+whole),cd.verts[2]->y);
            vector3 v2 = vector3(float(cd.xorg+whole),float(cd.zorg+whole),cd.verts[3]->y);
            this->error[1] = (ushort) this->GetError3(v0,v1,v2);

            // Force alias vertex to match.
            if (s) {
                s->vertex[2].y = y;
                s->vertex[2].n = this->vertex[4].n;
            }
            this->flags |= DIRTY;
        }
    }

	// See if we have child nodes.
    bool static_children = false;
    for (i=0; i<4; i++) {
        if (this->child[i]) {
            static_children = true;
            if (this->GetChild(i)->flags & DIRTY) this->flags |= DIRTY;
        }
    }

    // If we have no children and no necessary edges, then see if we can delete ourself.
    if ((static_children==false) && (cd.parent!=NULL)) {
        bool necessary_edges = false;


        for (i=0; i<4; i++) {
			// See if vertex deviates from edge between corners.
        
			// Only allow those to be culled that are not REALLY close to an 
			// edge.. Those really close should be left alone and the inside
			// of the terrain should then be smoothed.  This allows the
			// terrain welding code to work nicer by having more points to 
			// weld together. WG
			//
			// NOTE: There's probably a slicker way to do this - but it only
			// happens once in a tree's lifetime, generally binary trees are
			// preferred and skip this step.
			float diff = (float) fabs(this->vertex[i+1].y - (cd.verts[i]->y + cd.verts[(i+3)&3]->y) * 0.5f);
			if (diff > 0.00001f ||  cd.xorg == 0  || cd.zorg == 0 ||
				cd.xorg > 60000 || cd.zorg > 60000 ) {
				necessary_edges = true;
			}
		}
        if (!necessary_edges) {
            size *= 1.414213562f; // sqrt(2), because diagonal is longer than side.
            if ((cd.parent->square->error[2 + cd.child_index] * threshold_detail) < size) {
                flatquadsquare *ch = cd.parent->square->GetChild(cd.child_index);
                ch->Release(tn);                                // Delete this.
                cd.parent->square->child[cd.child_index] = 0;   // Clear the pointer.
            }
        }
    }
}

//-------------------------------------------------------------------
//  Update()
// Refresh the vertex enabled states in the tree, according to the
// location of the viewer.  May force creation or deletion of qsquares
// in areas which need to be interpolated.
//-------------------------------------------------------------------
void flatquadsquare::Update(nFlatTerrainNode *tn, flatquadcornerdata& cd, vector3& viewer, float detail)
{
	UpdateAux(tn,cd,viewer,0,detail);
}

//-------------------------------------------------------------------
//  UpdateAux()
// Does the actual work of updating enabled states and tree growing/shrinking.
//-------------------------------------------------------------------
void flatquadsquare::UpdateAux(nFlatTerrainNode *tn, flatquadcornerdata& cd, vector3& viewer, float center_error,float detail)
{
    // make sure error values are current.
    if (this->flags & DIRTY) {
        this->RecomputeError(cd);
    }

    int	half = 1 << cd.level;
    int	whole = half << 1;

    // See about enabling child verts.
    if ((!(this->enabled_flags&1)) && this->VertexTest(detail,cd.xorg+whole,cd.zorg+half,this->vertex[1].y,this->error[0],viewer)) 
    {
        this->EnableEdgeVertex(tn, 0, false, cd); // East vert.
    }    
    if ((!(this->enabled_flags&8)) && this->VertexTest(detail,cd.xorg+half,cd.zorg+whole,this->vertex[4].y,this->error[1],viewer))
    {
        this->EnableEdgeVertex(tn, 3, false, cd); // South vert.
    }
    if (cd.level > 0) {
        if (!(this->enabled_flags & 32)) {
            if (this->BoxTest(detail,this->error[3],viewer)) {
                this->EnableChild(tn, 1, cd); // nw child.er
            }
        }
        if (!(this->enabled_flags & 16)) {
            if (this->BoxTest(detail,this->error[2],viewer)) {
                this->EnableChild(tn, 0, cd);   // ne child.
            }
        }
        if (!(this->enabled_flags & 64)) {
            if (this->BoxTest(detail,this->error[4],viewer)) {
                this->EnableChild(tn, 2, cd);   // sw child.
            }
        }
        if (!(this->enabled_flags & 128)) {
            if (this->BoxTest(detail,this->error[5],viewer)) {
                this->EnableChild(tn, 3, cd);   // se child.
            }
        }
		
        // Recurse into child quadrants as necessary.
        flatquadcornerdata q;
        if (this->enabled_flags & 32) {
            if (this->child[1]) {
                this->SetupCornerData(&q,cd,1);
                this->GetChild(1)->UpdateAux(tn,q,viewer,this->error[3],detail);
            }
        }
        if (this->enabled_flags & 16) {
            if (this->child[0]) {
                this->SetupCornerData(&q,cd,0);
                this->GetChild(0)->UpdateAux(tn,q,viewer,this->error[2],detail);
            }
        }
        if (this->enabled_flags & 64) {
            if (this->child[2]) {
                this->SetupCornerData(&q,cd,2);
                this->GetChild(2)->UpdateAux(tn,q,viewer,this->error[4],detail);
            }
        }
        if (this->enabled_flags & 128) {
            if (this->child[3]) {
                this->SetupCornerData(&q,cd,3);
                this->GetChild(3)->UpdateAux(tn,q,viewer,this->error[5],detail);
            }
        }
    }
	if ( cd.zorg + whole == 65536 && cd.xorg + whole == 65536 )
	{
		/* int x = 1; */
	}

	// Ok.. I did it but...
	// This was dumb - caused WAY too many old quads to be left around after a seam was
	// traversed.. I turned on the bounding boxes to see what was up and WOAH... This
	// was in an attempt to keep the edges from ever unseeming - but instead it caused
	// polygons galore in the pipeline... I'll sacrifice some long distance cracks for 
	// 2X performance.
	// wg 8.25.00
	//
    // Test for disabling.  East, South, and center.
	//	if ( ( cd.level < 8 && cd.xorg + whole != 65536 && cd.zorg + whole != 65536 ) && ( cd.xorg != 0  || cd.zorg != 0 ) )
	//	{
    if ((this->enabled_flags & 1) && 
		(this->sub_enabled_count[0] == 0) && 
		(this->VertexTest(detail,cd.xorg+whole,cd.zorg+half,this->vertex[1].y,this->error[0],viewer)==false)) 
	{
		this->enabled_flags &= ~1;
		flatquadsquare *s = this->GetNeighbor(0,cd);
		if (s) s->enabled_flags &= ~4;
	}
	if ((this->enabled_flags & 8) && 
		(this->sub_enabled_count[1] == 0) && 
		(this->VertexTest(detail,cd.xorg+half,cd.zorg+whole,this->vertex[4].y,this->error[1],viewer)==false)) 
	{
		this->enabled_flags &= ~8;
		flatquadsquare*	s = this->GetNeighbor(3,cd);
		if (s) s->enabled_flags &= ~2;
	}
	if ((this->enabled_flags == 0) && (cd.parent != NULL) &&
		(this->BoxTest(detail,center_error,viewer) == false))
	{
		// Disable ourself.
		cd.parent->square->NotifyChildDisable(tn,*cd.parent,cd.child_index);   // nb: possibly deletes 'this'.
	}
}


//-------------------------------------------------------------------
//  AlignSouthBorders()
//  Fix our south borders to match the heights of the given quad's north 
//  borders. Can be called nearly any time.
//  13-Apr-00   floh    created
//-------------------------------------------------------------------
void flatquadsquare::AlignSouthBorders(flatquadcornerdata& cd, flatquadcornerdata& s)
{
    this->AlignSouthBordersAux(cd, s);
	this->RecomputeError( cd );
	this->RecomputeError( s );
}

//-------------------------------------------------------------------
//  AlignBorders()
//  Fix east and south borders to match heights of west and north 
//  borders. Should be called after AddHeightMap() and before
//  error values are recomputed.
//  13-Apr-00   floh    created
//-------------------------------------------------------------------
void flatquadsquare::AlignEastBorders(flatquadcornerdata& cd, flatquadcornerdata& e)
{
	this->AlignEastBordersAux(cd, e);
	this->RecomputeError( cd );
	this->RecomputeError( e );
}

//-------------------------------------------------------------------
//  AlignSouthBordersAux()
//  Recursively travel through tree and align heights of
//  southern border quads with supplied northern border
//  quads.
//  13-Apr-00   floh    created
//  14-Apr-00   floh    align normals as well
//  21-Aug-00   wgold   changed to use neighboring terrain as guide
//						instead of self. GetNeighbor() supports it now
//-------------------------------------------------------------------
void flatquadsquare::AlignSouthBordersAux(flatquadcornerdata& cd, flatquadcornerdata& s)
{
	n_assert( cd.level == s.level );

	// overkill perhaps.. - but it makes somewhat dissimilar terrains almost look
	// like they were meant to be together... It has flaws afterwards if the distances
    // are too great
	int half = (s.verts[1]->y + cd.verts[2]->y) / 2;
	s.verts[1]->y = cd.verts[2]->y = half;
	s.verts[1]->n = cd.verts[2]->n;

	half = (s.verts[0]->y + cd.verts[3]->y) / 2;
	s.verts[0]->y = cd.verts[3]->y = half;
	s.verts[0]->n = cd.verts[3]->n;

 	half = (s.square->vertex[2].y + cd.square->vertex[4].y) / 2;
    s.square->vertex[2].y = cd.square->vertex[4].y = half;	
	s.square->vertex[2].n = cd.square->vertex[4].n;	

	// Let's keep this one around
	cd.square->flags |= STATIC & DIRTY;
	s.square->flags  |= STATIC & DIRTY;
	
	// nw
    if (this->child[3] && s.square->child[0]) {
		flatquadcornerdata r, q;
		this->SetupCornerData(&r,cd,3);
		s.square->SetupCornerData( &q, s, 0 );
		this->GetChild(3)->AlignSouthBordersAux(r, q);
	}
    
	// recurse into ne quads
    if (this->child[2] && s.square->child[1]) {
        flatquadcornerdata q, r;
        this->SetupCornerData(&q,cd,2);
        s.square->SetupCornerData(&r,s,1);
        this->GetChild(2)->AlignSouthBordersAux(q, r);
	}
}

//-------------------------------------------------------------------
//  AlignEastBordersAux()
//  Recursively travel through tree and align heights of
//  Eastern border quads with supplied wester border
//  quads.
//  22-Aug-00	wgold	Created to use a neighboring quad for 
//						relative vertex information, traversing
//						both trees and aligning vertices and normals
//-------------------------------------------------------------------
void flatquadsquare::AlignEastBordersAux(flatquadcornerdata& cd, flatquadcornerdata& e)
{
	n_assert( cd.level == e.level );

	// overkill perhaps.. - but it makes somewhat dissimilar terrains almost look
	// like they were meant to be together... It has flaws afterwards if the distances
    // are too great
	int half = (e.verts[1]->y + cd.verts[0]->y) / 2;
	e.verts[1]->y = cd.verts[0]->y = half;
	e.verts[1]->n = cd.verts[0]->n;

	half = (e.verts[2]->y + cd.verts[3]->y) / 2;
	e.verts[2]->y = cd.verts[3]->y = half;
	e.verts[2]->n = cd.verts[3]->n;

 	half = (e.square->vertex[3].y + cd.square->vertex[1].y) / 2;
    e.square->vertex[3].y = cd.square->vertex[1].y = half;	
	e.square->vertex[3].n = cd.square->vertex[1].n;	

	// Let's keep this one around
	cd.square->flags |= STATIC & DIRTY;
	e.square->flags  |= STATIC & DIRTY;
	
	// nw
    if (this->child[0] && e.square->child[1]) {
		flatquadcornerdata r, q;
		this->SetupCornerData(&r,cd,0);
		e.square->SetupCornerData( &q, e, 1 );
		this->GetChild(0)->AlignEastBordersAux(r, q);
	}
    
	// recurse into ne quads
    if (this->child[3] && e.square->child[2]) {
        flatquadcornerdata q, r;
        this->SetupCornerData(&q,cd,3);
        e.square->SetupCornerData(&r,e,2);
        this->GetChild(3)->AlignEastBordersAux(q, r);
	}
}


//-------------------------------------------------------------------
//  AddHeightMap()
// Sets the height of all samples within the specified rectangular
// region using the given array of floats.  Extends the tree to the
// level of detail defined by (1 << hm.Scale) as necessary.
//-------------------------------------------------------------------
void flatquadsquare::AddHeightMap(nFlatTerrainNode *tn, flatquadcornerdata& cd, FlatHeightMapInfo& hm)
{
    // If block is outside rectangle, then don't bother.
    int	block_size = 2 << cd.level;
    if ((cd.xorg > (hm.x_origin + ((hm.x_size + 2) << hm.scale)))  ||
        (cd.zorg > (hm.z_origin + ((hm.z_size + 2) << hm.scale)))  ||
        ((cd.xorg + block_size) < (hm.x_origin - (1 << hm.scale))) ||
        ((cd.zorg + block_size) < (hm.z_origin - (1 << hm.scale))))
    {
        // This square does not touch the given height array area; no need to modify this square or descendants.
        return;
    }

    if (cd.parent && cd.parent->square) {
        // causes parent edge verts to be enabled, possibly causing neighbor blocks to be created.
        cd.parent->square->EnableChild(tn, cd.child_index, *cd.parent);
    }
	
    int	i;
    int	half = 1 << cd.level;

    // Create and update child nodes.
    for (i=0; i<4; i++) {
        flatquadcornerdata q;
        this->SetupCornerData(&q,cd,i);
        if ((0==this->child[i]) && (cd.level>hm.scale)) {
            // Create child node w/ current (unmodified) values for corner verts.
            this->child[i] = tn->AllocQuadSquare(&q);
        }
		
        // Recurse.
        if (this->child[i]) {
            this->GetChild(i)->AddHeightMap(tn,q,hm);
        }
    }

	// Deviate vertex heights based on data sampled from heightmap.
    int is[5][2] = {
        { cd.xorg+half,   cd.zorg+half },
        { cd.xorg+half*2, cd.zorg+half },
        { cd.xorg+half,   cd.zorg },
        { cd.xorg,        cd.zorg+half },
        { cd.xorg+half,   cd.zorg+half*2 },
    };
    int step_size = (1<<hm.scale);
    for (i=0; i<5; i++) {
        // Modify the vertex heights if necessary, and set the dirty
        // flag if any modifications occur, so that we know we need to
        // recompute error data later.
        int tx = is[i][0];
        int tz = is[i][1];
        float s = hm.Sample(tx,tz);
        this->flags |= DIRTY;
        this->vertex[i].y = (ushort) s;

        // sample 4 neighbor heights and compute normal...
        float nh[4];
        // right...
        nh[0] = hm.Sample(tx+step_size, tz);
        // top...
        nh[1] = hm.Sample(tx, tz+step_size);
        // left...
        nh[2] = hm.Sample(tx-step_size, tz);
        // bottom...
        nh[3] = hm.Sample(tx, tz-step_size);

        int tx1 = tx-step_size;
        int tz1 = tz-step_size;
        if (tx1 < 0) tx1 += 65536;
        if (tz1 < 0) tz1 += 65536;
        this->UpdateNormal2(this->vertex[i],
                            tx,  tz, s,
                            tx, tz1, s+(nh[3]-nh[1])*0.5f,
                            tx1, tz, s+(nh[2]-nh[0])*0.5f);
    }

    if (!(this->flags & DIRTY)) {
        // Check to see if any child nodes are dirty, and set the dirty flag if so.
        for (i=0; i<4; i++) {
            if (this->child[i] && (this->GetChild(i)->flags & DIRTY)) {
                this->flags |= DIRTY;
                break;
            }
        }
    }
    if (this->flags & DIRTY) this->SetStatic(cd);
}

//-------------------------------------------------------------------
//  Render()
//  Draws the heightfield represented by this tree.
//  Returns the number of triangles rendered.
//  26-Mar-00   floh    rewritten to render to Nebula vertex buffer
//-------------------------------------------------------------------
int	flatquadsquare::Render(nFlatTerrainNode *tn, flatquadcornerdata& cd)
{
    tn->BeginRender();
	this->RenderAux(cd,Clip::SOME_CLIP,tn);
    tn->EndRender();
	return 0;
}

//-------------------------------------------------------------------
//  RenderAux()
//  Does the work of rendering this square.  Uses the enabled vertices only.
//  Recurses as necessary.
//  26-Mar-00   floh    created
//  26-Mar-00   floh    rewritten to use nFlatTerrainNode rendering API
//  28-Mar-00   floh    + exchanged quad view frustum clipping stuff...
//-------------------------------------------------------------------
void flatquadsquare::RenderAux(flatquadcornerdata& cd, Clip::Visibility vis, nFlatTerrainNode *tn)
{
    int	half = 1 << cd.level;
    int	whole = 2 << cd.level;
	
    // FIXME!
	// if (cd.level <= 8) 
	//	tn->RenderBox(this->bbox);

    // If this square is outside the frustum, then don't render it.
    vis = tn->CullBox(this->bbox);
    if (vis == Clip::NOT_VISIBLE) {
        // outside view frustum
        return;
    }

    // recursively go down to child node
    int	i;
    int	flags = 0;
    int	mask = 1;
    flatquadcornerdata	q;
    for (i=0; i<4; i++, mask<<=1) {
        if (this->enabled_flags & (16<<i)) {
            this->SetupCornerData(&q,cd,i);
            if (this->child[i]) this->GetChild(i)->RenderAux(q,vis,tn);
        } else {
            flags |= mask;
        }
    }

    if (flags == 0) return;

    nFlatTriangleCorner tc[9];
    int x0 = cd.xorg;
    int x1 = cd.xorg + half;
    int x2 = cd.xorg + whole;
    int z0 = cd.zorg;
    int z1 = cd.zorg + half;
    int z2 = cd.zorg + whole;
    
    tc[0].x=x1; tc[0].z=z1; tc[0].vi=&(this->vertex[0]);
    tc[1].x=x2; tc[1].z=z1; tc[1].vi=&(this->vertex[1]);
    tc[2].x=x2; tc[2].z=z0; tc[2].vi=cd.verts[0];
    tc[3].x=x1; tc[3].z=z0; tc[3].vi=&(this->vertex[2]);
    tc[4].x=x0; tc[4].z=z0; tc[4].vi=cd.verts[1];
    tc[5].x=x0; tc[5].z=z1; tc[5].vi=&(this->vertex[3]);
    tc[6].x=x0; tc[6].z=z2; tc[6].vi=cd.verts[2];
    tc[7].x=x1; tc[7].z=z2; tc[7].vi=&(this->vertex[4]);
    tc[8].x=x2; tc[8].z=z2; tc[8].vi=cd.verts[3];

#define tri(a,b,c) (tn->AddTriangle(this,&(tc[c]),&(tc[b]),&(tc[a])))

    // Make the list of triangles to draw.
    if ((this->enabled_flags & 1) == 0) tri(0, 8, 2);
    else {
        if (flags & 8) tri(0, 8, 1);
        if (flags & 1) tri(0, 1, 2);
    }
    if ((this->enabled_flags & 2) == 0) tri(0, 2, 4);
    else {
        if (flags & 1) tri(0, 2, 3);
        if (flags & 2) tri(0, 3, 4);
    }
    if ((this->enabled_flags & 4) == 0) tri(0, 4, 6);
    else {
        if (flags & 2) tri(0, 4, 5);
        if (flags & 4) tri(0, 5, 6);
    }
    if ((this->enabled_flags & 8) == 0) tri(0, 6, 8);
    else {
        if (flags & 4) tri(0, 6, 7);
        if (flags & 8) tri(0, 7, 8);
    }
}

//-------------------------------------------------------------------
//  RenderToWF()
//  Render quadtree to a wavefront file. You should run a few
//  Update()'s over the tree with an appropriate detail error
//  and the viewer positioned at (0,0,0) before calling
//  RenderToWF().
//  20-Jul-00   floh    created
//-------------------------------------------------------------------
void flatquadsquare::RenderToWF(nFlatTerrainNode *tn, flatquadcornerdata& cd)
{
    tn->BeginRenderToWF();
    this->RenderToWFAux(cd,tn);
    tn->EndRenderToWF();
}

//-------------------------------------------------------------------
//  RenderToWFAux()
//  20-Jul-00   floh    created
//-------------------------------------------------------------------
void flatquadsquare::RenderToWFAux(flatquadcornerdata& cd, nFlatTerrainNode *tn)
{
    int	half = 1 << cd.level;
    int	whole = 2 << cd.level;
	
    // recursively go down to child node
    int	i;
    int	flags = 0;
    int	mask = 1;
    flatquadcornerdata q;
    for (i=0; i<4; i++, mask<<=1) {
        if (this->enabled_flags & (16<<i)) {
            this->SetupCornerData(&q,cd,i);
            if (this->child[i]) this->GetChild(i)->RenderToWFAux(q,tn);
        } else {
            flags |= mask;
        }
    }

    if (flags == 0) return;

    nFlatTriangleCorner tc[9];
    int x0 = cd.xorg;
    int x1 = cd.xorg + half;
    int x2 = cd.xorg + whole;
    int z0 = cd.zorg;
    int z1 = cd.zorg + half;
    int z2 = cd.zorg + whole;
    
    tc[0].x=x1; tc[0].z=z1; tc[0].vi=&(this->vertex[0]);
    tc[1].x=x2; tc[1].z=z1; tc[1].vi=&(this->vertex[1]);
    tc[2].x=x2; tc[2].z=z0; tc[2].vi=cd.verts[0];
    tc[3].x=x1; tc[3].z=z0; tc[3].vi=&(this->vertex[2]);
    tc[4].x=x0; tc[4].z=z0; tc[4].vi=cd.verts[1];
    tc[5].x=x0; tc[5].z=z1; tc[5].vi=&(this->vertex[3]);
    tc[6].x=x0; tc[6].z=z2; tc[6].vi=cd.verts[2];
    tc[7].x=x1; tc[7].z=z2; tc[7].vi=&(this->vertex[4]);
    tc[8].x=x2; tc[8].z=z2; tc[8].vi=cd.verts[3];

#undef tri
#define tri(a,b,c) (tn->AddTriangleToWF(this,&(tc[c]),&(tc[b]),&(tc[a])))

    // Make the list of triangles to draw.
    if ((this->enabled_flags & 1) == 0) tri(0, 8, 2);
    else {
        if (flags & 8) tri(0, 8, 1);
        if (flags & 1) tri(0, 1, 2);
    }
    if ((this->enabled_flags & 2) == 0) tri(0, 2, 4);
    else {
        if (flags & 1) tri(0, 2, 3);
        if (flags & 2) tri(0, 3, 4);
    }
    if ((this->enabled_flags & 4) == 0) tri(0, 4, 6);
    else {
        if (flags & 2) tri(0, 4, 5);
        if (flags & 4) tri(0, 5, 6);
    }
    if ((this->enabled_flags & 8) == 0) tri(0, 6, 8);
    else {
        if (flags & 4) tri(0, 6, 7);
        if (flags & 8) tri(0, 7, 8);
    }
}

//-------------------------------------------------------------------
//  Intersect()
//  Check for intersection of line with terrain, recursively check
//  for intersection for bounding boxes until leaf node intersection
//  has been found, do triangle intersection at leaf node.
//  20-Apr-00   floh    created
//
//  FIXME! ONLY CHECK STATIC NODES FOR INTERSECTION!
//-------------------------------------------------------------------
bool flatquadsquare::Intersect(flatquadcornerdata& cd, line3& l, float& ipos, triangle& itri)
{
    // Check if intersection with this bounding box. For level 15 and 14,
    // we dont check, because those 2 bounding boxes are not correct
    // in cartesian space
    if (this->bbox->intersect(l)) {

        // intersection with box, hand down request for children,
        // or check for intersection with triangle
        int i;
        bool child_isect = false;
        for (i=0; i<4; i++) {
            flatquadcornerdata q;
            this->SetupCornerData(&q,cd,i);
            if (this->child[i]) {
                // hand down intersection to child node
                child_isect |= this->GetChild(i)->Intersect(q,l,ipos,itri);
            } else {
                // build the 2 triangles making up the quad
                int whole  = 2<<q.level;
                float x0 = float(q.xorg);
                float x1 = float(q.xorg + whole);
                float z0 = float(q.zorg);
                float z1 = float(q.zorg + whole);
                vector3 v0(vector3(x1,z0,q.verts[0]->y));
                vector3 v1(vector3(x0,z0,q.verts[1]->y));
                vector3 v2(vector3(x0,z1,q.verts[2]->y));
                vector3 v3(vector3(x1,z1,q.verts[3]->y));
                triangle t[2];
                switch (i) {
                    case 0:
                    case 3:
                        t[0].set(v2,v1,v0); t[1].set(v3,v2,v0);
                        break;
                    case 1:
                    case 2:
                        t[0].set(v3,v1,v0); t[1].set(v3,v2,v1);
                        break;
                }

                // check for intersection of line with the 2 triangles
                float ti;
                int j;
                for (j=0; j<2; j++) {
                    if (t[j].intersect(l,ti)) {
                        if (ti<ipos) {
                            ipos = ti;
                            itri = t[j];
                            child_isect = true;
                            break;
                        }
                    }
                }
                //if (!child_isect) {
                //    n_printf("*** NO TRIANGLE INTERSECTION! ***\n");
                //}
            }
        }
        return child_isect;
    } else {
        // no intersection...
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Returns the height of the heightfield at the specified x,z coordinates.
// 8/22/00		wgold	reimplemented from quadtree.cpp (t.u.)
/////////////////////////////////////////////////////////////////////////////
float	flatquadsquare::GetHeight(flatquadcornerdata& cd, float x, float z)
{
	int	half = 1 << cd.level;

	float	lx = (x - cd.xorg) / float(half);
	float	lz = (z - cd.zorg) / float(half);

	int	ix = int(floor(lx));
	int	iz = int(floor(lz));

	// Clamp.
	if (ix < 0) ix = 0;
	if (ix > 1) ix = 1;
	if (iz < 0) iz = 0;
	if (iz > 1) iz = 1;

	int	index = ix ^ (iz ^ 1) + (iz << 1);
	if (this->child[index]){ // && Child[index]->Static) {
		// Pass the query down to the child which contains it.
		flatquadcornerdata	q;
		SetupCornerData(&q, cd, index);
		return this->GetChild(index)->GetHeight(q, x, z);
	}

	// Bilinear interpolation.
	lx -= ix;
	if (lx < 0) lx = 0;
	if (lx > 1) lx = 1;
	
	lz -= iz;
	if (lx < 0) lz = 0;
	if (lz > 1) lz = 1;

	float	s00, s01, s10, s11;
	switch (index) {
	default:
	case 0:
		s00 = this->vertex[2].y;
		s01 = cd.verts[0]->y;
		s10 = this->vertex[0].y;
		s11 = this->vertex[1].y;
		break;
	case 1:
		s00 = cd.verts[1]->y;
		s01 = this->vertex[2].y;
		s10 = this->vertex[3].y;
		s11 = this->vertex[0].y;
		break;
	case 2:
		s00 = this->vertex[3].y;
		s01 = this->vertex[0].y;
		s10 = cd.verts[2]->y;
		s11 = this->vertex[4].y;
		break;
	case 3:
		s00 = vertex[0].y;
		s01 = vertex[1].y;
		s10 = vertex[4].y;
		s11 = cd.verts[3]->y;
		break;
	}

	return (s00 * (1-lx) + s01 * lx) * (1 - lz) + (s10 * (1-lx) + s11 * lx) * lz;
}

//-------------------------------------------------------------------
//  Save()
//  Save (preprocessed) quadtree structure into file,
//  only save static nodes. Allows to store statically culled
//  quadtree into file to avoid preprocessing of entire quadtree
//  at runtime.
//  26-Apr-00   floh    created
//  27-Apr-00   floh    + count number of quads in tree and
//                        write to beginning of file
//-------------------------------------------------------------------
void flatquadsquare::Save(nFlatTerrainNode * /* unused tn */,
                          flatquadcornerdata& /* unused cd */, nFile *fp)
{
    // make sure tree is initialized!
    // this->ResetTree(tn);
    // this->RecomputeError(cd);
    int num_quads = this->CountNodes();
    fp->Write(&num_quads,sizeof(num_quads));
    this->SaveAux(fp);
}

//-------------------------------------------------------------------
//  SaveAux()
//  FIXME: BYTE ORDERING!!!
//  26-Apr-00   floh    created
//-------------------------------------------------------------------
void flatquadsquare::SaveAux(nFile *fp)
{
    // format is:
    //  ubyte 01 (child follows)
    //      ushort y[5]
    //      ushort n[5]
    //      ushort error[6]
    //
    //  ubyte 00 (no child)
    
    int i;
    uchar head = 1;
    fp->Write(&head,sizeof(head));

    // vertex y
    for (i=0; i<5; i++) {
        ushort y = (ushort) this->vertex[i].y;
        fp->Write(&y,sizeof(y));
    }
    // packed normal
    for (i=0; i<5; i++) {
        ushort n = (ushort) this->vertex[i].n.getpacked();
        fp->Write(&n,sizeof(n));
    }
    // errors
    for (i=0; i<6; i++) {
        ushort e = (ushort) this->error[i];
        fp->Write(&e,sizeof(e));
    }

    // recurse into children
    for (i=0; i<4; i++) {
        if (this->child[i]) this->GetChild(i)->SaveAux(fp);
        else {
            // 'no child' head byte
            head = 0;
            fp->Write(&head,sizeof(head));
        }
    }
}

//-------------------------------------------------------------------
//  Load()
//  Load quadtree structure from file. Filepointer needs to be
//  positioned after the lead byte of the quad definition
//  26-Apr-00   floh    created
//  12-May-00   floh    rewritten...
//-------------------------------------------------------------------
void flatquadsquare::Load(nFlatTerrainNode *tn, flatquadcornerdata& cd, nFile *fp)
{
    int i;

    // read vertex y
    for (i=0; i<5; i++) {
        ushort y;
        fp->Read(&y,sizeof(y));
        this->vertex[i].y = y;
    }

    // read packed normal
    for (i=0; i<5; i++) {
        ushort n;
        fp->Read(&n,sizeof(n));
        this->vertex[i].n.setpacked(n);
    }

    // read error values
    for (i=0; i<6; i++) {
        ushort e;
        fp->Read(&e,sizeof(e));
        this->error[i] = e;
    }

    this->flags = STATIC;

    // create children and recurse as necessary
    for (i=0; i<4; i++) {
        // read head byte, if it is 0, then there is no child at that position
        uchar head;
        fp->Read(&head,sizeof(head));
        if (head!=0) {
            flatquadcornerdata q;
            this->SetupCornerData(&q,cd,i);
            this->child[i] = tn->AllocQuadSquare(&q);
            if (this->child[i]) {
                this->GetChild(i)->Load(tn,q,fp);
            }
        }
    }

    // update bounding box (after recursing into child!)
    this->UpdateBBox(cd);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------



