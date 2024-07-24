//--------------------------------------------------------------------
//  wftriang.cc
//  Triangulate a wavefront file.
//
//  24-Oct-00   floh    rewritten to wfObject
//
//  (C) 1998 A.Weissflog
//--------------------------------------------------------------------
#include "tools/wftoolbox.h"
#include "tools/wftools.h"

/*
//--------------------------------------------------------------------
void copy_arrays(nWavefront *wf0, nWavefront *wf1)
{
    nV *v;
    nVN *vn;
    nVT *vt;
    
    for (v = (nV *) wf0->v_list.GetHead();
         v;
         v = (nV *) v->GetSucc())
    {
        wf1->AddV(v->x,v->y,v->z);
    }
    for (vn = (nVN *) wf0->vn_list.GetHead();
         vn;
         vn = (nVN *) vn->GetSucc())
    {
        wf1->AddVN(vn->nx,vn->ny,vn->nz);
    }
    for (vt = (nVT *) wf0->vt_list.GetHead();
         vt;
         vt = (nVT *) vt->GetSucc())
    {
        wf1->AddVT(vt->u,vt->v);
    }
}

//--------------------------------------------------------------------
//  clone_v()
//  clone_vn()
//  clone_vt()
//  clone_rgba()
//
//  Findet jeweils das passende v,vn,vt aus wf0 in wf1.
//
//  29-Dec-98   floh    created
//--------------------------------------------------------------------
nV *clone_v(nWavefront *wf0, nWavefront *wf1, nV *v0)
{
    nV *v;
    if (v0) v = (nV *) wf1->v_list.NodeAt(wf0->v_list.IndexOf(v0));
    else    v = NULL;
    return v;
}
nVN *clone_vn(nWavefront *wf0, nWavefront *wf1, nVN *vn0)
{
    nVN *vn;
    if (vn0) vn = (nVN *) wf1->vn_list.NodeAt(wf0->vn_list.IndexOf(vn0));
    else     vn = NULL;
    return vn;
}
nVT *clone_vt(nWavefront *wf0, nWavefront *wf1, nVT *vt0)
{
    nVT *vt;
    if (vt0) vt = (nVT *) wf1->vt_list.NodeAt(wf0->vt_list.IndexOf(vt0));
    else     vt = NULL;
    return vt;
}
nRgba *clone_rgba(nWavefront *wf0, nWavefront *wf1, nRgba *rgba0)
{
    nRgba *rgba;
    if (rgba0) rgba = (nRgba *) wf1->rgba_list.NodeAt(wf0->rgba_list.IndexOf(rgba0));
    else       rgba = NULL;
    return rgba;
}

//--------------------------------------------------------------------
void triang(nWavefront *wf0, nWavefront *wf1)
{
    // kopiere jede gueltige Flaeche von wf0 nach wf1, entferne
    // ungueltige Flaechen, trianguliere Flaechen mit
    // mehr als 3 Eckpunkten als Triangle-Fans
    fprintf(stderr, "triangulating...\n");
    
    nFace *f0;
    for (f0 = (nFace *) wf0->face_list.GetHead();
         f0;
         f0 = (nFace *) f0->GetSucc())
    {
        long num_p = f0->NumPoints();
        if (num_p > 2) {
            nPoint *p0 = (nPoint *) f0->points.GetHead();
            nPoint *p1 = (nPoint *) p0->GetSucc();
            nPoint *p2 = (nPoint *) p1->GetSucc();
            long i;
            for (i=0; i<(num_p-2); i++) {
                nFace *f1 = wf1->AddFace();
                nV *v;
                nVN *vn;
                nVT *vt;
                nRgba *rgba;
                v  = clone_v(wf0, wf1, p0->v);
                vn = clone_vn(wf0, wf1, p0->vn);
                vt = clone_vt(wf0, wf1, p0->vt);
                rgba = clone_rgba(wf0, wf1, p0->rgba);
                f1->AddPoint(v,vn,vt,rgba);
                v  = clone_v(wf0, wf1, p1->v);
                vn = clone_vn(wf0, wf1, p1->vn);
                vt = clone_vt(wf0, wf1, p1->vt);
                rgba = clone_rgba(wf0, wf1, p1->rgba);
                f1->AddPoint(v,vn,vt,rgba);
                v  = clone_v(wf0, wf1, p2->v);
                vn = clone_vn(wf0, wf1, p2->vn);
                vt = clone_vt(wf0, wf1, p2->vt);
                rgba = clone_rgba(wf0, wf1, p2->rgba);
                f1->AddPoint(v,vn,vt,rgba);
                p1 = p2;
                p2 = (nPoint *) p2->GetSucc();
            }
        }
    }
    fprintf(stderr, "%d -> %d faces\n",(int)wf0->NumFaces(), (int)wf1->NumFaces());
} 
*/               

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool help;
    long retval = 0;
    
    fprintf(stderr,"-> wftriang\n");
    
    // Args auswerten
    help = wf_getboolarg(argc, argv, "-help");

    if (help) {
        fprintf(stderr, "wftriang [-help]\n"
                        "(C) 1998 Andre Weissflog\n"
                        "Triangulate wavefront object.\n"
                        "-help          -- show help\n");
        return 0;
    }

    wfToolbox toolbox;
    wfObject src,dst;

    FILE *in, *out;
    if (!wf_openfiles(argc, argv, in, out)) {
        fprintf(stderr,"file open failed!\n");
        retval = 10; goto ende;
    }

    // load...
    fprintf(stderr,"loading...\n");
    if (!src.load(in)) {
        fprintf(stderr,"Load failed!\n");
        retval = 10; goto ende;
    }

    // triangulate...
    toolbox.triangulate(src,dst);

    // save...
    fprintf(stderr,"saving...\n");
    dst.save(out);
    
    wf_closefiles(in, out);
    
ende:
    fprintf(stderr,"<- wftriang\n");
    return retval;
}  
    

    

