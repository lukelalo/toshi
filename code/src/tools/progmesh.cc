/*
 *  Progressive Mesh type Polygon Reduction Algorithm
 *  by Stan Melax (c) 1998
 *  Permission to use any of this code wherever you want is granted..
 *  Although, please do acknowledge authorship if appropriate.
 *
 *  See the header file progmesh.h for a description of this module
 */

// #include "kernel/ntypes.h"

#include <stdlib.h>
#include <stdio.h>  
#include <math.h>
#include <assert.h>

#include "vector.h"
#include "list.h"
#include "progmesh.h"

#include "tools/wfobject.h"

#ifndef min
static float min(float a, float b)
{
    return (a<b) ? a : b;
}
#endif
#ifndef max
static float max(float a, float b)
{
    return (a>b) ? a : b;
}
#endif

/*
 *  For the polygon reduction algorithm we use data structures
 *  that contain a little bit more information than the usual
 *  indexed face set type of data structure.
 *  From a vertex we wish to be able to quickly get the
 *  neighboring faces and vertices.
 */
class Triangle;
class Vertex;

class Triangle {
  public:
	Vertex *         vertex[3]; // the 3 points that make this tri
	Vector           normal;    // unit vector othogonal to this face
	int              vt[3];     // Textur-Koordinaten-Indices
	                 Triangle(Vertex *v0,Vertex *v1,Vertex *v2);
	                 ~Triangle();
	void             ComputeNormal();
	void             ReplaceVertex(Vertex *vold,Vertex *vnew);
	int              HasVertex(Vertex *v);
};
class Vertex {
  public:
	Vector           position; // location of point in euclidean space
	int              id;       // place of vertex in original list
	List<Vertex *>   neighbor; // adjacent vertices
	List<Triangle *> face;     // adjacent triangles
	float            objdist;  // cached cost of collapsing edge
	Vertex *         collapse; // candidate vertex for collapse
	                 Vertex(Vector v,int _id);
	                 ~Vertex();
	void             RemoveIfNonNeighbor(Vertex *n);
};
List<Vertex *>   vertices;
List<Triangle *> triangles;


Triangle::Triangle(Vertex *v0,Vertex *v1,Vertex *v2){
	assert(v0!=v1 && v1!=v2 && v2!=v0);
	vertex[0]=v0;
	vertex[1]=v1;
	vertex[2]=v2;
	ComputeNormal();
	triangles.Add(this);
	for(int i=0;i<3;i++) {
		vertex[i]->face.Add(this);
		for(int j=0;j<3;j++) if(i!=j) {
			vertex[i]->neighbor.AddUnique(vertex[j]);
		}
	}
}
Triangle::~Triangle(){
	int i;
	triangles.Remove(this);
	for(i=0;i<3;i++) {
		if(vertex[i]) vertex[i]->face.Remove(this);
	}
	for(i=0;i<3;i++) {
		int i2 = (i+1)%3;
		if(!vertex[i] || !vertex[i2]) continue;
		vertex[i ]->RemoveIfNonNeighbor(vertex[i2]);
		vertex[i2]->RemoveIfNonNeighbor(vertex[i ]);
	}
}
int Triangle::HasVertex(Vertex *v) {
	return (v==vertex[0] ||v==vertex[1] || v==vertex[2]);
}
void Triangle::ComputeNormal(){
	Vector v0=vertex[0]->position;
	Vector v1=vertex[1]->position;
	Vector v2=vertex[2]->position;
	normal = (v1-v0)*(v2-v1);
	if(magnitude(normal)==0)return;
	normal = normalize(normal);
}
void Triangle::ReplaceVertex(Vertex *vold,Vertex *vnew) {
	assert(vold && vnew);
	assert(vold==vertex[0] || vold==vertex[1] || vold==vertex[2]);
	assert(vnew!=vertex[0] && vnew!=vertex[1] && vnew!=vertex[2]);
	if(vold==vertex[0]){
		vertex[0]=vnew;
	}
	else if(vold==vertex[1]){
		vertex[1]=vnew;
	}
	else {
		assert(vold==vertex[2]);
		vertex[2]=vnew;
	}
	int i;
	vold->face.Remove(this);
	assert(!vnew->face.Contains(this));
	vnew->face.Add(this);
	for(i=0;i<3;i++) {
		vold->RemoveIfNonNeighbor(vertex[i]);
		vertex[i]->RemoveIfNonNeighbor(vold);
	}
	for(i=0;i<3;i++) {
		assert(vertex[i]->face.Contains(this)==1);
		for(int j=0;j<3;j++) if(i!=j) {
			vertex[i]->neighbor.AddUnique(vertex[j]);
		}
	}
	ComputeNormal();
}

Vertex::Vertex(Vector v,int _id) {
	position =v;
	id=_id;
	vertices.Add(this);
}

Vertex::~Vertex(){
	assert(face.num==0);
	while(neighbor.num) {
		neighbor[0]->neighbor.Remove(this);
		neighbor.Remove(neighbor[0]);
	}
	vertices.Remove(this);
}
void Vertex::RemoveIfNonNeighbor(Vertex *n) {
	// removes n from neighbor list if n isn't a neighbor.
	if(!neighbor.Contains(n)) return;
	for(int i=0;i<face.num;i++) {
		if(face[i]->HasVertex(n)) return;
	}
	neighbor.Remove(n);
}

//-----------------------------------------------------------------------------
float ComputeEdgeCollapseCost(Vertex *u,Vertex *v) {
	// if we collapse edge uv by moving u to v then how 
	// much different will the model change, i.e. how much "error".
	// Texture, vertex normal, and border vertex code was removed
	// to keep this demo as simple as possible.
	// The method of determining cost was designed in order 
	// to exploit small and coplanar regions for
	// effective polygon reduction.
	// Is is possible to add some checks here to see if "folds"
	// would be generated.  i.e. normal of a remaining face gets
	// flipped.  I never seemed to run into this problem and
	// therefore never added code to detect this case.

	int i;
	float edgelength = magnitude(v->position - u->position);
	float curvature=0;
    float cost;
    
	// find the "sides" triangles that are on the edge uv
	List<Triangle *> sides;
	int num_faces = 0;
	for(i=0;i<u->face.num;i++) {
		if(u->face[i]->HasVertex(v)){
			sides.Add(u->face[i]);
			num_faces++;
		}
	}

    //----------------------------------------------------------------
    //  Falls es sich um eine Aussenflaeche handelt, wird 
    //  als Sonderfall -1 returniert.
    //----------------------------------------------------------------
    if (num_faces == 1) {
        // Eine Aussenkante (nicht ganz astrein, aber mal sehen,
        // ob das reicht). Wir machen Aussenkanten erstmal 100
        // mal teurer als normal
        return -1.0f;        
    }
	
	// use the triangle facing most away from the sides 
	// to determine our curvature term
	for(i=0;i<u->face.num;i++) {
		float mincurv=1; // curve for face i and closer side to it
		for(int j=0;j<sides.num;j++) {
			// use dot product of face normals. '^' defined in vector
			float dotprod = u->face[i]->normal ^ sides[j]->normal;
			mincurv = min(mincurv,(1-dotprod)/2.0f);
		}
		curvature = max(curvature,mincurv);
	}
	// the more coplanar the lower the curvature term   
	cost = edgelength * curvature;
    //printf("# cost for %lx->%lx: %f\n", (long int)u, (long int)v, cost);
    return cost;
}

//-----------------------------------------------------------------------------
void ComputeEdgeCostAtVertex(Vertex *v) {
	// compute the edge collapse cost for all edges that start
	// from vertex v.  Since we are only interested in reducing
	// the object by selecting the min cost edge at each step, we
	// only cache the cost of the least cost edge at this vertex
	// (in member variable collapse) as well as the value of the 
	// cost (in member variable objdist).
	if(v->neighbor.num==0) {
		// v doesn't have neighbors so it costs nothing to collapse
		v->collapse=NULL;
		v->objdist=-0.01f;
		return;
	}
	v->objdist = 1000000;
	v->collapse=NULL;
	// search all neighboring edges for "least cost" edge
    bool is_border = false;
	for(int i=0;i<v->neighbor.num;i++) {
		float dist;
		dist = ComputeEdgeCollapseCost(v,v->neighbor[i]);
        if (dist == -1.0f) {
            // Sonderfall, an diesem Vertex endet eine "Aussenkante",
            // deshalb sollte der Vertex nicht verrueckt werden, weil
            // sonst Loecher entstehen wuerden, wir setzen hier einfach
            // ein Flag, damit der "richtige" Ziel-Vertex einfach
            // die Kosten extrem hochsetzen kann 
            is_border = true;
        }
        if(dist<v->objdist) {
			v->collapse=v->neighbor[i];   // candidate for edge collapse
			if (is_border) v->objdist = 1000.0;
            else           v->objdist = dist; // cost of the collapse
		}
	}
    //printf("##### vertex %lx collapses to %lx with cost %f\n",(long int)v,(long int)v->collapse,v->objdist);
}
//-----------------------------------------------------------------------------
void ComputeAllEdgeCollapseCosts() {
	// For all the edges, compute the difference it would make
	// to the model if it was collapsed.  The least of these
	// per vertex is cached in each vertex object.
	for(int i=0;i<vertices.num;i++) {
		ComputeEdgeCostAtVertex(vertices[i]);
	}
}
//-----------------------------------------------------------------------------
void Collapse(Vertex *u, Vertex *v){
	// Collapse the edge uv by moving vertex u onto v
	// Actually remove tris on uv, then update tris that
	// have u to have v, and then remove u.
    //
	if(!v) {
		// u is a vertex all by itself so just delete it
		delete u;
		return;
	}
	int i;
	List<Vertex *>tmp;
    printf("# collapsing v %d -> %d\n", u->id, v->id); 
            
	// make tmp a list of all the neighbors of u
	for(i=0;i<u->neighbor.num;i++) {
		tmp.Add(u->neighbor[i]);
	}
	// delete triangles on edge uv ... und nehme Textur-Index-
    // Ersetzungen vor...
	for(i=u->face.num-1;i>=0;i--) {
		if(u->face[i]->HasVertex(v)) {
            // an der aktuellen Flaeche, ermittle die Textur-Indices
            // an der u->v Kante
            int u_tmp, v_tmp;
            int u_index, v_index;
            for (u_tmp=0; u_tmp<3; u_tmp++) {
                if (u->face[i]->vertex[u_tmp]==u) break;
            }
            for (v_tmp=0; v_tmp<3; v_tmp++) {
                if (u->face[i]->vertex[v_tmp]==v) break;
            }
            u_index = u->face[i]->vt[u_tmp];
            v_index = u->face[i]->vt[v_tmp];
            
            // alle noch existierenden u-Indices muessen auf v gebogen
            // werden
            printf("#### vt: mapping %d -> %d\n", u_index, v_index);
            int k;
            int vt_hits = 0;
            for (k=0; k<triangles.num; k++) {
                int l;
                for (l=0; l<3; l++) {
                    if (triangles[k]->vt[l] == u_index) {
                        triangles[k]->vt[l] = v_index;
                        vt_hits++;
                    }
                }
            }
            printf("### %d hits\n", vt_hits);
            delete(u->face[i]);
		}
	}
	// update remaining triangles to have v instead of u
	for(i=u->face.num-1;i>=0;i--) {
		u->face[i]->ReplaceVertex(u,v);
	}
	delete u;
    
	// recompute the edge collapse costs for neighboring vertices
	for(i=0;i<tmp.num;i++) {
		ComputeEdgeCostAtVertex(tmp[i]);
	}
}
//-----------------------------------------------------------------------------
void AddVertex(List<Vector> &vert){
	for(int i=0;i<vert.num;i++) {
		new Vertex(vert[i],i);
	}
}
//-----------------------------------------------------------------------------
void AddFaces(List<tridata> &tri){
	for(int i=0;i<tri.num;i++) {
		Triangle *t = new Triangle(vertices[tri[i].v[0]],
				                   vertices[tri[i].v[1]],
				                   vertices[tri[i].v[2]] );
        // Texture-Informationen uebernehmen
        int j;
        for (j=0; j<3; j++) t->vt[j] = tri[i].vt[j];
	}
}
//-----------------------------------------------------------------------------
Vertex *MinimumCostEdge(){
	// Find the edge that when collapsed will affect model the least.
	// This funtion actually returns a Vertex, the second vertex
	// of the edge (collapse candidate) is stored in the vertex data.
	// Serious optimization opportunity here: this function currently
	// does a sequential search through an unsorted list :-(
	// Our algorithm could be O(n*lg(n)) instead of O(n*n)
	Vertex *mn=vertices[0];
	for(int i=0;i<vertices.num;i++) {
		if(vertices[i]->objdist < mn->objdist) {
			mn = vertices[i];
		}
	}
	return mn;
}

//-----------------------------------------------------------------------------
void GenWfObject(wfObject& src, wfObject& dst)
{
    // simply go through all remaining triangles and write them out
    // someone should do a wfclean afterwards
    int i;
    bool has_vt = (src.vt_array.Size() > 0);
    int v_index = 0;
    for (i=0; i<triangles.num; i++) {
        int j;
        wfFace f_dst;
        for (j=0; j<3; j++) {
            wfPoint p_dst;

            // append the vertex coordinate to the destination object
            Vertex *v = triangles[i]->vertex[j];
            wfCoord vc(v->position.x,v->position.y,v->position.z);
            dst.v_array.PushBack(vc);
            p_dst.v_index = v_index;
            
            if (has_vt) {
                // if texture coordinates are present, append the texture 
                // coordinate to the destination object
                vector2 vt(src.vt_array.At(triangles[i]->vt[j]));
                dst.vt_array.PushBack(vt);
                p_dst.vt_index = v_index;    
            }
            v_index++;

            // append the point to the destination face
            f_dst.points.PushBack(p_dst);
        }
        // append the new triangle to the destination object
        dst.f_array.PushBack(f_dst);
    }
}

//-----------------------------------------------------------------------------
void GenMesh(List<Vector> &vert,        // Original-Liste aller Vertices
             List<tridata> &tri,        // Original-Liste aller Dreiecke
             wfObject& src,             // Original-Wavefront-Objekt
             wfObject& dst,             // Ziel-Wavefront-Objekt
             int face_num)              // gewuenschte Anzahl Flaechen
{
    AddVertex(vert);  // put input data into our data structures
	AddFaces(tri);
	ComputeAllEdgeCollapseCosts(); // cache all edge collapse costs
    
    // reduzieren bis gewuenschte Flaechenanzahl erreicht
    while (triangles.num > face_num) {
        // naechster Vertex zum entfernen
        Vertex *mn = MinimumCostEdge();
        Collapse(mn,mn->collapse);
    }
    
    // Ergebnis nach wf1 schreiben
    GenWfObject(src,dst);
}

//-----------------------------------------------------------------------------
