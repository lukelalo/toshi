#define N_IMPLEMENTS nTerrainNode
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
#include <stdio.h>
#include <math.h>
#include "terrain/quadtree.h"
#include "terrain/nterrainnode.h"

//-------------------------------------------------------------------
//  quadsquare()
//  19-Apr-00   floh    created
//-------------------------------------------------------------------
quadsquare::quadsquare()
{
    this->next = NULL;
}

//-------------------------------------------------------------------
//  Init()
//  Initialize quad for usage (pseudo constructor).
//  19-Apr-00   floh    created  
//-------------------------------------------------------------------
void quadsquare::Init(quadcornerdata* pcd, quadsquare *qsbase)
{
    int i;
    pcd->square   = this;
    this->qs_base = qsbase;

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

    // update the quad's cartesian coords and bounding box...
    this->UpdateCartesians(*pcd);
    this->UpdateBBox(*pcd);

    // Compute lighting...
    this->InterpolateNormals(*pcd);
}

//-------------------------------------------------------------------
//  Release()
//  Release the quad and all of its children through
//  nTerrainNode::FreeQuadSquare().
//  19-Apr-00   floh    created
//-------------------------------------------------------------------
void quadsquare::Release(nTerrainNode *tn)
{
    // Recursively delete sub-trees.
    int	i;
    for (i=0; i<4; i++) {
	    if (this->child[i]) this->GetChild(i)->Release(tn);
	    this->child[i] = 0;
    }
    tn->FreeQuadSquare(this);
}

//-------------------------------------------------------------------
//  CreateChild()
// Creates a child square at the specified index.
//-------------------------------------------------------------------
void quadsquare::CreateChild(nTerrainNode *tn, int index, quadcornerdata& cd) 
{
    if (0 == this->child[index]) {
	    quadcornerdata q;
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
void quadsquare::SetStatic(quadcornerdata& cd)
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
int	quadsquare::CountNodes()
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
void quadsquare::UpdateNormal2(VertInfo& vi, 
                               int tx0, int tz0, float y0,
                               int tx1, int tz1, float y1,
                               int tx2, int tz2, float y2)
{
    if (tx0 == 0) {
        vector3 n(0.0f, 1.0f, 0.0f);
        vi.n.pack(n);
    } else if (tx0 == 65536) {
        vector3 n(0.0f, -1.0f, 0.0f);
        vi.n.pack(n);
    } else {

        // convert the 3 polar points to cartesian points
        vector3 v0(this->GetCartesian(tx0,tz0,y0));
        vector3 v1(this->GetCartesian(tx1,tz1,y1));
        vector3 v2(this->GetCartesian(tx2,tz2,y2));

        // create the normal from the 3 vectors
        vector3 n((v2-v0) * (v1-v0));
        n.norm();
        vi.n.pack(n);
    }
}

//-------------------------------------------------------------------
//  InterpolateNormals()
//  Generate approximated normals of (usually) non-static nodes from
//  quadcorner normals.
//  30-Mar-00   floh    created
//-------------------------------------------------------------------
void quadsquare::InterpolateNormals(quadcornerdata& cd)
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
ushort quadsquare::RecomputeError(quadcornerdata& cd)
{
    int	i;

    // Measure error of center and edge vertices.
    float maxerror = 0.0f;

    // update the quads cartesian coordinates
    this->UpdateCartesians(cd);
    vector3 v[9];
    v[0] = this->vertex[0].cart;
    v[1] = this->vertex[1].cart;
    v[2] = cd.verts[0]->cart;
    v[3] = this->vertex[2].cart;
    v[4] = cd.verts[1]->cart;
    v[5] = this->vertex[3].cart;
    v[6] = cd.verts[2]->cart;
    v[7] = this->vertex[4].cart;
    v[8] = cd.verts[3]->cart;

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
        quadcornerdata q;
        if (this->child[i]) {
            this->SetupCornerData(&q,cd,i);
            quadsquare *ch = this->GetChild(i);
            this->error[i+2] = ch->RecomputeError(q);
        } else {
            // Compute difference between bilinear average at child center, and diagonal edge approximation.
            //this->error[i+2] = 0;
            this->error[i+2] = (ushort) this->GetError4(v[0], v[i*2+1], v[i*2+2], v[(i*2+3)&7]);
        }
        if (this->error[i+2] > maxerror) maxerror = this->error[i+2];
    }

    // update the quads bounding box 
    // (must be called after recursing to children)
    this->UpdateBBox(cd);

    // The error this node and descendants is correct now.
    this->flags &= ~DIRTY;
    return (ushort) maxerror;
}

//-------------------------------------------------------------------
//  ResetTree()
// Clear all enabled flags, and delete all non-static child nodes.
//-------------------------------------------------------------------
void quadsquare::ResetTree(nTerrainNode *tn)
{
    int	i;
    for (i = 0; i < 4; i++) {
        if (this->child[i]) {
            quadsquare *ch = this->GetChild(i);
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
void quadsquare::StaticCullData(nTerrainNode *tn, quadcornerdata& cd, float threshold_detail)
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
void quadsquare::StaticCullAux(nTerrainNode *tn, quadcornerdata& cd, float threshold_detail, int target_level)
{
    int	i,j;
    quadcornerdata q;
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
        quadsquare *s = this->GetNeighbor(0, cd);
        if ((s==NULL) || ((0==s->child[1]) && (0==s->child[2]))) {

            // Force vertex height to the edge value.
            int half  = 1<<cd.level;
            int whole = half<<1;
            ushort y = ushort((cd.verts[0]->y + cd.verts[3]->y) * 0.5f);
            this->vertex[1].y = y;
            vector3 v0 = this->GetCartesian(cd.xorg+whole,cd.zorg+half,y);
            vector3 v1 = this->GetCartesian(cd.xorg+whole,cd.zorg,cd.verts[0]->y);
            vector3 v2 = this->GetCartesian(cd.xorg+whole,cd.zorg+whole,cd.verts[3]->y);
            this->error[0] = (ushort) this->GetError3(v0,v1,v2);

            // Force alias vertex to match (not if alias vertex is north pole)
            if (s && (!((cd.xorg + whole) == 65536))) {
                s->vertex[3].y = y;
                s->vertex[3].n = this->vertex[1].n;
            }
            this->flags |= DIRTY;
        }
    }

    if ((0==this->child[2]) && (0==this->child[3]) && ((this->error[1]*threshold_detail)<size)) {
        quadsquare *s = this->GetNeighbor(3,cd);
        if ((s==NULL) || ((0==s->child[0]) && (0==s->child[1]))) {

            int half  = 1<<cd.level;
            int whole = half<<1;
            ushort y = ushort((cd.verts[2]->y + cd.verts[3]->y) * 0.5f);
            this->vertex[4].y = y;

            vector3 v0 = this->GetCartesian(cd.xorg+half,cd.zorg+whole,y);
            vector3 v1 = this->GetCartesian(cd.xorg,cd.zorg+whole,cd.verts[2]->y);
            vector3 v2 = this->GetCartesian(cd.xorg+whole,cd.zorg+whole,cd.verts[3]->y);
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
            
            // FIXME: should be calculated in cartesian space?
            float diff = (float) fabs(this->vertex[i+1].y - (cd.verts[i]->y + cd.verts[(i+3)&3]->y) * 0.5f);
            if (diff > 0.00001f) {
                necessary_edges = true;
            }
        }

        if (!necessary_edges) {
            size *= 1.414213562f; // sqrt(2), because diagonal is longer than side.
            if ((cd.parent->square->error[2 + cd.child_index] * threshold_detail) < size) {
                quadsquare *ch = cd.parent->square->GetChild(cd.child_index);
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
void quadsquare::Update(nTerrainNode *tn, quadcornerdata& cd, vector3& viewer, float detail)
{
	UpdateAux(tn,cd,viewer,0,detail);
}

//-------------------------------------------------------------------
//  UpdateAux()
// Does the actual work of updating enabled states and tree growing/shrinking.
//-------------------------------------------------------------------
void quadsquare::UpdateAux(nTerrainNode *tn, quadcornerdata& cd, vector3& viewer, float center_error,float detail)
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
        quadcornerdata q;
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
	
    // Test for disabling.  East, South, and center.
    if ((this->enabled_flags & 1) && 
        (this->sub_enabled_count[0] == 0) && 
        (this->VertexTest(detail,cd.xorg+whole,cd.zorg+half,this->vertex[1].y,this->error[0],viewer)==false)) 
    {
        this->enabled_flags &= ~1;
        quadsquare *s = this->GetNeighbor(0,cd);
        if (s) s->enabled_flags &= ~4;
    }
    if ((this->enabled_flags & 8) && 
        (this->sub_enabled_count[1] == 0) && 
        (this->VertexTest(detail,cd.xorg+half,cd.zorg+whole,this->vertex[4].y,this->error[1],viewer)==false)) 
    {
        this->enabled_flags &= ~8;
        quadsquare*	s = this->GetNeighbor(3,cd);
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
//  AlignBorders()
//  Fix east and south borders to match heights of west and north 
//  borders. Should be called after AddHeightMap() and before
//  error values are recomputed.
//  13-Apr-00   floh    created
//-------------------------------------------------------------------
void quadsquare::AlignBorders(quadcornerdata& cd)
{
    this->AlignSouthBordersAux(cd);
}

//-------------------------------------------------------------------
//  AlignSouthBordersAux()
//  Recursively travel through tree and align heights of
//  southern border quads with western and northern border
//  quads.
//  13-Apr-00   floh    created
//  14-Apr-00   floh    align normals as well
//-------------------------------------------------------------------
void quadsquare::AlignSouthBordersAux(quadcornerdata& cd)
{
    // make sure we are on a southern border quad
    int whole = 2<<cd.level;
    if (cd.zorg+whole != 65536) return;
    
    // get height of south vertex and copy to north vertex
    // of opposite quad. If we are at root level, we are our
    // own neighbor...
    quadsquare *n;
    n = this->GetNeighbor(3,cd);
    n->vertex[2].y    = this->vertex[4].y;
    n->vertex[2].n    = this->vertex[4].n;
    n->vertex[2].cart = this->vertex[4].cart;
    n->flags |= DIRTY;

    // recurse into sw quads
    if (this->child[2]) {
        quadcornerdata q;
        this->SetupCornerData(&q,cd,2);
        this->GetChild(2)->AlignSouthBordersAux(q);
    }

    // recurse into se quads
    if (this->child[3]) {
        quadcornerdata q;
        this->SetupCornerData(&q,cd,3);
        this->GetChild(3)->AlignSouthBordersAux(q);
    }
}

//-------------------------------------------------------------------
//  AddHeightMap()
// Sets the height of all samples within the specified rectangular
// region using the given array of floats.  Extends the tree to the
// level of detail defined by (1 << hm.Scale) as necessary.
//-------------------------------------------------------------------
void quadsquare::AddHeightMap(nTerrainNode *tn, quadcornerdata& cd, HeightMapInfo& hm)
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
        quadcornerdata q;
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
int	quadsquare::Render(nTerrainNode *tn, quadcornerdata& cd)
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
//  26-Mar-00   floh    rewritten to use nTerrainNode rendering API
//  28-Mar-00   floh    + exchanged quad view frustum clipping stuff...
//-------------------------------------------------------------------
void quadsquare::RenderAux(quadcornerdata& cd, Clip::Visibility vis, nTerrainNode *tn)
{
    // FIXME!
    // if (cd.level == 10) tn->RenderBox(this->bbox);

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
    quadcornerdata	q;
    for (i=0; i<4; i++, mask<<=1) {
        if (this->enabled_flags & (16<<i)) {
            this->SetupCornerData(&q,cd,i);
            if (this->child[i]) this->GetChild(i)->RenderAux(q,vis,tn);
        } else {
            flags |= mask;
        }
    }

    if (flags == 0) return;

    VertInfo *vi[9];
    vi[0] = &(this->vertex[0]);
    vi[1] = &(this->vertex[1]);
    vi[2] = cd.verts[0];
    vi[3] = &(this->vertex[2]);
    vi[4] = cd.verts[1];
    vi[5] = &(this->vertex[3]);
    vi[6] = cd.verts[2];
    vi[7] = &(this->vertex[4]);
    vi[8] = cd.verts[3];

#define tri(a,b,c) (tn->AddTriangle(this,vi[c],vi[b],vi[a]))

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
void quadsquare::RenderToWF(nTerrainNode *tn, quadcornerdata& cd)
{
    tn->BeginRenderToWF();
    this->RenderToWFAux(cd,tn);
    tn->EndRenderToWF();
}

//-------------------------------------------------------------------
//  RenderToWFAux()
//  20-Jul-00   floh    created
//-------------------------------------------------------------------
void quadsquare::RenderToWFAux(quadcornerdata& cd, nTerrainNode *tn)
{
    // recursively go down to child node
    int	i;
    int	flags = 0;
    int	mask = 1;
    quadcornerdata q;
    for (i=0; i<4; i++, mask<<=1) {
        if (this->enabled_flags & (16<<i)) {
            this->SetupCornerData(&q,cd,i);
            if (this->child[i]) this->GetChild(i)->RenderToWFAux(q,tn);
        } else {
            flags |= mask;
        }
    }

    if (flags == 0) return;

    VertInfo *vi[9];
    vi[0] = &(this->vertex[0]);
    vi[1] = &(this->vertex[1]);
    vi[2] = cd.verts[0];
    vi[3] = &(this->vertex[2]);
    vi[4] = cd.verts[1];
    vi[5] = &(this->vertex[3]);
    vi[6] = cd.verts[2];
    vi[7] = &(this->vertex[4]);
    vi[8] = cd.verts[3];

#undef tri
#define tri(a,b,c) (tn->AddTriangleToWF(this,vi[c],vi[b],vi[a]))

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
bool quadsquare::Intersect(quadcornerdata& cd, line3& l, float& ipos, triangle& itri)
{
    // Check if intersection with this bounding box. For level 15 and 14,
    // we dont check, because those 2 bounding boxes are not correct
    // in cartesian space
    if (this->bbox.intersect(l)) {

        // intersection with box, hand down request for children,
        // or check for intersection with triangle
        int i;
        bool child_isect = false;
        for (i=0; i<4; i++) {
            quadcornerdata q;
            this->SetupCornerData(&q,cd,i);
            if (this->child[i]) {
                // hand down intersection to child node
                child_isect |= this->GetChild(i)->Intersect(q,l,ipos,itri);
            } else {
                // build the 2 triangles making up the quad
                vector3& v0 = q.verts[0]->cart;
                vector3& v1 = q.verts[1]->cart;
                vector3& v2 = q.verts[2]->cart;
                vector3& v3 = q.verts[3]->cart;
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
            }
        }
        return child_isect;
    } else {
        // no intersection...
        return false;
    }
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
void quadsquare::Save(nTerrainNode *tn, quadcornerdata& cd, nFile *fp)
{
    // make sure tree is initialized!
    this->ResetTree(tn);
    this->RecomputeError(cd);
    int num_quads = this->CountNodes();
    fp->Write(&num_quads,sizeof(num_quads));
    this->SaveAux(fp);
}

//-------------------------------------------------------------------
//  SaveAux()
//  FIXME: BYTE ORDERING!!!
//  26-Apr-00   floh    created
//-------------------------------------------------------------------
void quadsquare::SaveAux(nFile *fp)
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
void quadsquare::Load(nTerrainNode *tn, quadcornerdata& cd, nFile *fp)
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
            quadcornerdata q;
            this->SetupCornerData(&q,cd,i);
            this->child[i] = tn->AllocQuadSquare(&q);
            if (this->child[i]) {
                this->GetChild(i)->Load(tn,q,fp);
            }
        }
    }

    // update cartesian coords and bounding box (after recursing into child!)
    this->UpdateCartesians(cd);
    this->UpdateBBox(cd);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------



