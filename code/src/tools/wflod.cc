//--------------------------------------------------------------------
//  wflod.cc
//
//  Triangle-Reduzierer. Der hereinkommende Wavefront-File muss
//  bereits trianguliert worden sein!
//
//  (C) 1998 A.Weissflog
//--------------------------------------------------------------------
#include "tools/wfobject.h"
#include "tools/wftools.h"

#include "list.h"
#include "vector.h"
#include "progmesh.h"

List<tridata> tri;       // global list of triangles
List<Vector> vert;       // global list of vertices

//--------------------------------------------------------------------
//  reduce()
//  04-Jan-99   floh    created
//  06-Jan-99   floh    + uebernimmt jetzt Textur-Koordinaten
//                        in die Triangles
//  07-Jan-99   floh    + komplett neu
//  25-Oct-00   floh    + rewritten to wfObject
//--------------------------------------------------------------------
void reduce(wfObject& src, wfObject& dst, int face_num)
{
    fprintf(stderr, "translating vertex and triangle data...\n");

    // add vertices
    nArray<wfCoord>::iterator cur_v;
    for (cur_v=src.v_array.Begin(); cur_v!=src.v_array.End(); cur_v++) {
        vert.Add(Vector(cur_v->v.x,cur_v->v.y,cur_v->v.z));
    }

    // add triangles
    nArray<wfFace>::iterator cur_f;
    for (cur_f=src.f_array.Begin(); cur_f!=src.f_array.End(); cur_f++) {
        wfPoint p[3];
        p[0] = cur_f->points.At(0);
        p[1] = cur_f->points.At(1);
        p[2] = cur_f->points.At(2);
        tridata td;
        int i;
        for (i=0; i<3; i++) {
            td.v[i]  = p[i].v_index;
            td.vt[i] = p[i].vt_index;   // may be -1
        }
        tri.Add(td);
    }

    fprintf(stderr, "generating reduced mesh...\n");
    GenMesh(vert,tri,src,dst,face_num);
}        

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    List<tridata> dummy0;
    List<Vector> dummy2;
    
    bool help;
    long retval = 0;
    int abs_v, rel_v, num_v;
    fprintf(stderr,"-> wflod\n");
        
    // get args
    help  = wf_getboolarg(argc,argv,"-help");
    abs_v = wf_getintarg(argc,argv,"-abs",0);
    rel_v = wf_getintarg(argc,argv,"-rel",100);
    
    // show help?
    if (help) {
        fprintf(stderr,"wflod [-help] [-rel] [-abs]\n"
               "Based on lod demo by Stan Melax (melax@cs.ualberta.ca)\n"
               "Triangle reduction tool.\n" 
               "-help -- show help and exit\n"
               "-rel  -- reduce to n percent of original vertice number\n"
               "-abs  -- reduce to abs number of vertices\n");
        return 0;
    }

    wfObject src;
    wfObject dst;

    FILE *in, *out;
    if (!wf_openfiles(argc, argv, in, out)) {
        fprintf(stderr,"file open failed!\n");
        retval = 10; goto ende;
    }

    // load source object
    fprintf(stderr,"loading...\n");
    if (!src.load(in)) {
        fprintf(stderr,"Load failed!\n");
        retval = 10; goto ende;
    }

    if (abs_v > 0) {
        num_v = src.f_array.Size();
        if (abs_v < num_v) num_v = abs_v;
    } else {
        if (rel_v < 0)        rel_v = 0;
        else if (rel_v > 100) rel_v = 100; 
        num_v = (src.f_array.Size() * rel_v) / 100;
    }
        
    fprintf(stderr, "before: %d vertices, %d faces\n", src.v_array.Size(), src.f_array.Size());
    reduce(src,dst,num_v);
    fprintf(stderr, "after: %d vertices, %d faces\n", dst.v_array.Size(), dst.f_array.Size());
    
    // write the result
    fprintf(stderr,"saving...\n");
    dst.save(out);
    
    wf_closefiles(in, out);
    
ende:
    fprintf(stderr,"<- wflod\n");
    return retval;
}  
    
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
