//--------------------------------------------------------------------
//  wavefront.cc
//
//  Implementiert nWavefront Klasse.
//  03-May-99   floh    kommt jetzt auch mit Tristrips (t/q)
//                      zurecht
//
//  (C) 1998 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/ntypes.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "tools/wavefront.h"
#include "mathlib/vector.h"

//====================================================================
//  nIList Implementierung
//====================================================================

//--------------------------------------------------------------------
//  NodeAt()
//  23-Dec-98   floh    created  
//--------------------------------------------------------------------
nNode *nIList::NodeAt(long index)
{
    long i = 0;
    nNode *nd;
    for (nd=this->GetHead(); nd; nd=nd->GetSucc()) {
        if (i == index) return nd;
        i++;
    }
    return NULL;
}

//--------------------------------------------------------------------
//  IndexOf()
//  23-Dec-98   floh    created  
//--------------------------------------------------------------------
long nIList::IndexOf(nNode *n)
{
    long i = 0;
    nNode *nd;
    for (nd=this->GetHead(); nd; nd=nd->GetSucc()) {
        if (n==nd) return i;
        i++;
    }
    return -1;
} 

//====================================================================
//  nPoint Implementierung
//====================================================================

//--------------------------------------------------------------------
//  nPoint()
//  23-Dec-98   floh    created
//  02-Feb-99   floh    + Support fuer Rgba
//--------------------------------------------------------------------
nPoint::nPoint(nWavefront *_wf,
               nFace *_f,
               nV *_v,
               nVN *_vn,
               nVT *_vt,
               nRgba *_rgba)
{
    wf = _wf;
    f  = _f;
    v  = _v;
    vn = _vn;
    vt = _vt;
    rgba = _rgba;
    if (v)    v->use_count++;
    if (vn)   vn->use_count++;
    if (vt)   vt->use_count++;
    if (rgba) rgba->use_count++;
    _f->points.AddTail(this);
}

//--------------------------------------------------------------------
//  ~nPoint()
//  23-Dec-98   floh    created
//  28-Dec-98   floh    falls v,vn,vt, auf welche der Point
//                      verwiesen hat, einen UseCount 0 haben,
//                      werden sie gleich mit gekillt (weil 
//                      eh niemand mehr auf sie verweist)
//  02-Feb-99   floh    + Support fuer Eckpunktfarben
//--------------------------------------------------------------------
nPoint::~nPoint()
{
    if (v) {
        v->use_count--;
        if (v->use_count == 0) delete v;
    }
    if (vn) {
        vn->use_count--;
        if (vn->use_count == 0) delete vn;
    }
    if (vt) {
        vt->use_count--;
        if (vt->use_count == 0) delete vt;
    }
    if (rgba) {
        rgba->use_count--;
        if (rgba->use_count == 0) delete rgba;
    }
    this->Remove();
}    

//--------------------------------------------------------------------
//  nPoint::Print()
//  23-Dec-98   floh    created
//--------------------------------------------------------------------
bool nPoint::Print(FILE *fp)
{
    if (v) {
        fprintf(fp,"%d",(int)wf->v_list.IndexOf((nNode *)v)+1);
        if (vt) fprintf(fp,"/%d",(int)wf->vt_list.IndexOf((nNode *)vt)+1);
        else if (vn || rgba) fprintf(fp,"/");
        if (vn) fprintf(fp,"/%d",(int)wf->vn_list.IndexOf((nNode *)vn)+1);
        else if (rgba) fprintf(fp,"/");
        if (rgba) fprintf(fp,"/%d",(int)wf->rgba_list.IndexOf((nNode *)rgba)+1);
    }
    return true;
}

//--------------------------------------------------------------------
//  nPoint::PrintAsTriStrip()
//  03-May-99   floh    created
//--------------------------------------------------------------------
bool nPoint::PrintAsTriStrip(FILE *fp)
{
    if (v) fprintf(fp,"%d",(int)wf->v_list.IndexOf((nNode *)v)+1);
    return true;
}

//====================================================================
//  nFace Implementierung
//====================================================================

//--------------------------------------------------------------------
//  nFace()
//  23-Dec-98   floh    created
//--------------------------------------------------------------------
nFace::nFace(nWavefront *_wf)
{
    type = N_FACE;
    wf = _wf;
    nx = ny = nz = 0.0f;
    wf->face_list.AddTail(this);
}

//--------------------------------------------------------------------
//  ~nFace()
//  23-Dec-98   floh    created
//--------------------------------------------------------------------
nFace::~nFace()
{
    // alle Points killen
    nPoint *p;
    while ((p = (nPoint *) points.GetHead())) delete p;
    this->Remove();
}

//--------------------------------------------------------------------
//  SetType()
//  03-May-99   floh    created
//--------------------------------------------------------------------
void nFace::SetType(nFaceType t)
{
    this->type = t;
}

//--------------------------------------------------------------------
//  AddPoint()
//  23-Dec-98   floh    created
//  02-Feb-99   floh    Unterstuetzung fuer Eckpunktfarben 
//--------------------------------------------------------------------
void nFace::AddPoint(nV *v, nVN *vn, nVT *vt, nRgba *rgba)
{
    new nPoint(wf,this,v,vn,vt,rgba);
}

//--------------------------------------------------------------------
//  NumPoints()
//  23-Dec-98   floh    created
//--------------------------------------------------------------------
long nFace::NumPoints(void)
{
    long i=0;
    nPoint *p;
    for (p = (nPoint *) points.GetHead();
         p;
         p = (nPoint *) p->GetSucc())
    {
        i++;
    }
    return i;
}

//--------------------------------------------------------------------
//  SharedPoints()
//  Returniert, wieviele Punkte 2 Flaechen sharen, wenn das
//  mehr 2 sind, ist dass ein Problem
//  23-Dec-98   floh    created
//--------------------------------------------------------------------
long nFace::SharedPoints(nFace *f)
{
    nPoint *p0;
    long num_shared = 0;
    // teste jeden meiner Punkte mit jedem Punkt
    // der anderen Flaeche
    for (p0 = (nPoint *) points.GetHead();
         p0;
         p0 = (nPoint *) p0->GetSucc())
    {
        nPoint *p1;
        for (p1 = (nPoint *) f->points.GetHead();
             p1;
             p1 = (nPoint *) p1->GetSucc())
        {
            if (p0->IsEqual(p1)) num_shared++;
        }
    }
    return num_shared;
}

//--------------------------------------------------------------------
//  Print()
//  23-Dec-98   floh    created
//  03-May-99   floh    kann jetzt auch TriangleStrips saven
//--------------------------------------------------------------------
bool nFace::Print(FILE *fp)
{
    nPoint *p;
    if (this->type == N_TRISTRIP) {
        fprintf(fp,"t ");
        for (p=(nPoint *)points.GetHead(); p; p=(nPoint *)p->GetSucc()) {
            p->PrintAsTriStrip(fp);
            fprintf(fp," ");
        }
    } else {
        fprintf(fp,"f ");
        for (p=(nPoint *)points.GetHead(); p; p=(nPoint *) p->GetSucc()) {
            p->Print(fp);
            fprintf(fp," ");
        }
    }
    fprintf(fp,"\n");
    return true;
}

//--------------------------------------------------------------------
//  PrintAsFN()
//  20-Aug-99   floh    created
//--------------------------------------------------------------------
bool nFace::PrintAsFN(FILE *fp)
{
    nPoint *p0 = (nPoint *) points.GetHead();
    nPoint *p1 = (nPoint *) p0->GetSucc();
    nPoint *p2 = (nPoint *) p1->GetSucc();
    fprintf(fp,"fn %d %d %d %f %f %f\n",
            (int)wf->v_list.IndexOf(p0->v),
            (int)wf->v_list.IndexOf(p1->v),
            (int)wf->v_list.IndexOf(p2->v),
            nx, ny, nz);
    return true;
}

//====================================================================
//  nWingedEdge Implementierung
//====================================================================

//-------------------------------------------------------------------
//  nWingedEdge::nWingedEdge()
//  16-Aug-99   floh    created
//-------------------------------------------------------------------
nWingedEdge::nWingedEdge(nWavefront *_wf, nIList *l, nV *_v0, nV *_v1, 
                         nFace *_f0, nFace *_f1)
{
    n_assert(_wf && l && _v0 && _v1 && _f0 && _f1);
    v0 = _v0;
    v1 = _v1;
    f0 = _f0;
    f1 = _f1;
    v0->use_count++;
    v1->use_count++;
    wf = _wf;
    l->AddTail(this);
}

//-------------------------------------------------------------------
//  nWingedEdge::~nWingedEdge()
//  16-Aug-99   floh    created
//-------------------------------------------------------------------
nWingedEdge::~nWingedEdge()
{
    v0->use_count--;
    v1->use_count--;
    this->Remove();
}

//-------------------------------------------------------------------
//  nWingedEdge::Print()
//  16-Aug-99   floh    created
//-------------------------------------------------------------------
bool nWingedEdge::Print(FILE *fp)
{
    fprintf(fp,"we %d %d %d %d\n",
            (int)wf->face_list.IndexOf(f0),
            (int)wf->face_list.IndexOf(f1),
            (int)wf->v_list.IndexOf(v0),
            (int)wf->v_list.IndexOf(v1));
    return true;
};

//====================================================================
//  nWavefront Implementierung
//====================================================================

//--------------------------------------------------------------------
//  nWavefront()
//  23-Dec-98   floh    created
//--------------------------------------------------------------------
nWavefront::nWavefront()
{ }

//--------------------------------------------------------------------
//  ~nWavefront()
//  23-Dec-98   floh    created
//  02-Feb-99   floh    Support fuer Eckpunktfarben
//  16-Aug-99   floh    + Facenormals, Edge-Liste
//--------------------------------------------------------------------
nWavefront::~nWavefront()
{
    nV *v;
    nVT *vt;
    nVN *vn;
    nRgba *rgba;
    nFace *f;
    nWingedEdge *we;
    
    // zuerst alle Faces und WingedEdges killen, wegen Vertex-Usecounts
    while ((f  = (nFace *) face_list.GetHead())) delete f;
    while ((we = (nWingedEdge *) we_list.GetHead())) delete we;
    while ((v  = (nV *) v_list.GetHead()))   delete v;
    while ((vn = (nVN *) vn_list.GetHead())) delete vn;
    while ((vt = (nVT *) vt_list.GetHead())) delete vt;
    while ((rgba = (nRgba *) rgba_list.GetHead())) delete rgba;
}

//--------------------------------------------------------------------
//  AddV()
//  AddVN()
//  AddVT()
//  AddRgba()
//  AddFace()
//  AddWingedEdge()
//  23-Dec-98   floh    created
//  02-Feb-99   floh    Support fuer Eckpunktfarben
//  16-Aug-99   floh    + Winged-Face/Edges
//--------------------------------------------------------------------
nV *nWavefront::AddV(float x, float y, float z)
{
    return new nV(&v_list,x,y,z);
}
nVN *nWavefront::AddVN(float nx, float ny, float nz)
{
    return new nVN(&vn_list,nx,ny,nz);
}
nVT *nWavefront::AddVT(float u, float v)
{
    return new nVT(&vt_list,u,v);
}
nRgba *nWavefront::AddRgba(float r, float g, float b, float a)
{
    return new nRgba(&rgba_list,r,g,b,a);
}
nFace *nWavefront::AddFace(void)
{
    return new nFace(this);
}
nWingedEdge *nWavefront::AddWingedEdge(nV *v0, nV *v1, nFace *f0, nFace *f1)
{
    return new nWingedEdge(this,&(we_list),v0,v1,f0,f1);
}

//--------------------------------------------------------------------
//  GetFirstPoint()
//  GetNextPoint()
//  
//  Methoden zum Face-unabhaengigen durchsteppen aller
//  nPoints in einem nWavefront-Objekt.
//
//  28-Dec-98   floh    created
//--------------------------------------------------------------------
nPoint *nWavefront::GetFirstPoint(void)
{
    nFace *f = (nFace *) this->face_list.GetHead();
    nPoint *p = NULL;
    while (f && (!(p = (nPoint *) f->points.GetHead()))) {
        f = (nFace *) f->GetSucc();
    }
    return p;
}
nPoint *nWavefront::GetNextPoint(nPoint *p)
{
    nPoint *next_p = (nPoint *) p->GetSucc();
    if (!next_p) {
        nFace *next_f = (nFace *) p->f->GetSucc();
        while (next_f && (!(next_p=(nPoint *)next_f->points.GetHead()))) {
            next_f = (nFace *) next_f->GetSucc();
        }
    }
    return next_p;
}

//--------------------------------------------------------------------
//  NextPointV()
//  NextPointVN()
//  NextPointVT()
//  NextPointRgba()
//  Sucht naechsten nPoint, der auf V,VN,VT zeigt.
//  23-Dec-98   floh    created
//  02-Feb-99   floh    Support fuer Eckpunktfarben
//--------------------------------------------------------------------
nPoint *nWavefront::NextPointV(nV *v, nPoint *p)
{
    if (!p) {
        p = this->GetFirstPoint();
        if (p->v == v) return p;
    }
    while ((p = this->GetNextPoint(p))) {
        if (p->v == v) return p;
    }
    return NULL;
}
nPoint *nWavefront::NextPointVN(nVN *vn, nPoint *p)
{
    if (!p) {
        p = this->GetFirstPoint();
        if (p->vn == vn) return p;
    }
    while ((p = this->GetNextPoint(p))) {
        if (p->vn == vn) return p;
    }
    return NULL;
}
nPoint *nWavefront::NextPointVT(nVT *vt, nPoint *p)
{
    if (!p) {
        p = this->GetFirstPoint();
        if (p->vt == vt) return p;
    }
    while ((p = this->GetNextPoint(p))) {
        if (p->vt == vt) return p;
    }
    return NULL;
}
nPoint *nWavefront::NextPointRgba(nRgba *rgba, nPoint *p)
{
    if (!p) {
        p = this->GetFirstPoint();
        if (p->rgba == rgba) return p;
    }
    while ((p = this->GetNextPoint(p))) {
        if (p->rgba == rgba) return p;
    }
    return NULL;
}

//--------------------------------------------------------------------
//  NumV()
//  NumVN()
//  NumVT()
//  NumRgba()
//  NumFaces()
//  NumFNormals()
//  NumWingedEdge()
//  23-Dec-98   floh    created
//  02-Feb-99   floh    + NumRgba()
//  16-Aug-99   floh    + Facenormals und WingedEdges
//--------------------------------------------------------------------
long nWavefront::NumV(void)
{
    nV *v;
    long i=0;
    for (v=(nV *)v_list.GetHead(); v; v =(nV *)v->GetSucc())
    {
        i++;
    }
    return i;
}
long nWavefront::NumVN(void)
{
    nVN *vn;
    long i=0;
    for (vn=(nVN *)vn_list.GetHead(); vn; vn=(nVN *)vn->GetSucc())
    {
        i++;
    }
    return i;
}
long nWavefront::NumVT(void)
{
    nVT *vt;
    long i=0;
    for (vt=(nVT *)vt_list.GetHead(); vt; vt=(nVT *)vt->GetSucc())
    {
        i++;
    }
    return i;
}
long nWavefront::NumRgba(void)
{
    nRgba *rgba;
    long i=0;
    for (rgba=(nRgba *)rgba_list.GetHead(); rgba; rgba=(nRgba *)rgba->GetSucc())
    {
        i++;
    }
    return i;
}
long nWavefront::NumFaces(void)
{
    nFace *f;
    long i=0;
    for (f=(nFace *)face_list.GetHead(); f; f=(nFace *)f->GetSucc())
    {
        i++;
    }
    return i;
}
long nWavefront::NumWingedEdges(void)
{
    nWingedEdge *we;
    long i=0;
    for (we=(nWingedEdge *)we_list.GetHead(); we; we=(nWingedEdge *)we->GetSucc())
    {
        i++;
    }
    return i;
}

//--------------------------------------------------------------------
//  Load()
//  23-Dec-98   floh    created
//  02-Feb-99   floh    + Support fuer Eckpunktfarben
//  03-May-99   floh    + kann jetzt auch t/q Definitionen lesen
//  16-Aug-99   floh    + FaceNormals und WingedEdges
//  20-Aug-99   floh    + Facenormals und WingedEdges werden NICHT
//                        mehr geladen
//--------------------------------------------------------------------
bool nWavefront::Load(FILE *fp)
{
    char line[1024];
    int act_line = 0;
    nFace *act_face = NULL;
    while (fgets(line,sizeof(line),fp)) {
        act_line++;
        char *kw = strtok(line," \t\n");
        if (kw) {
            if (strcmp(kw,"v")==0) {
                char *xs = strtok(NULL," \t\n");
                char *ys = strtok(NULL," \t\n");
                char *zs = strtok(NULL," \t\n");
                if (xs && ys && zs) {
                    float x = (float) atof(xs);
                    float y = (float) atof(ys);
                    float z = (float) atof(zs);
                    this->AddV(x,y,z);
				} else {
					fprintf(stderr,"line %d: broken 'v' statement\n",act_line);
					fclose(fp); return false;
				}
            } else if (strcmp(kw,"vn")==0) {
                char *nxs = strtok(NULL," \t\n");
                char *nys = strtok(NULL," \t\n");
                char *nzs = strtok(NULL," \t\n");
                if (nxs && nys && nzs) {
                    float nx = (float) atof(nxs);
                    float ny = (float) atof(nys);
                    float nz = (float) atof(nzs);
                    this->AddVN(nx,ny,nz);
                } else {
					fprintf(stderr,"line %d: broken 'vn' statement\n",act_line);
					fclose(fp); return false;
                }
            } else if (strcmp(kw,"vt")==0) {
                char *us = strtok(NULL," \t\n");
                char *vs = strtok(NULL," \t\n");
                if (us && vs) {
                    float u = (float) atof(us);
                    float v = (float) atof(vs);
                    this->AddVT(u,v);
                } else {
					fprintf(stderr,"line %d: broken 'vt' statement\n",act_line);
					fclose(fp); return false;
                }
            } else if (strcmp(kw,"rgba")==0) {
                char *rs = strtok(NULL, " \t\n");
                char *gs = strtok(NULL, " \t\n");
                char *bs = strtok(NULL, " \t\n");
                char *as = strtok(NULL, " \t\n");
                if (rs && gs && bs && as) {
                    float r = (float) atof(rs);
                    float g = (float) atof(gs);
                    float b = (float) atof(bs);
                    float a = (float) atof(as);
                    this->AddRgba(r,g,b,a);
                } else {
                    fprintf(stderr,"line %d: broken 'rgba' statement\n",act_line);
                    fclose(fp); return false;
                }
			} else if (strcmp(kw,"f")==0) {
                char *p_str;
                // fuer jeden Punkt-String...
                nFace *f = this->AddFace();
                f->SetType(nFace::N_FACE);
                while ((p_str = strtok(NULL," \t\n"))) {
                    char *slash;
                    nV *v       = NULL;
                    nVN *vn     = NULL;
                    nVT *vt     = NULL;
                    nRgba *rgba = NULL;
                    long act_i  = 0;
                    long i[4];
                    memset(i,0,sizeof(i));
                    do {
                        slash = strchr(p_str,'/');
                        if (slash) *slash=0;
                        if (*p_str) i[act_i] = atoi(p_str)-1;
                        act_i++;
                        p_str = slash+1;
                    } while (slash && (act_i<4));
                    if (i[0]>=0) v = (nV *)   this->v_list.NodeAt(i[0]);
                    if (i[1]>=0) vt = (nVT *) this->vt_list.NodeAt(i[1]);
                    if (i[2]>=0) vn = (nVN *) this->vn_list.NodeAt(i[2]);
                    if (i[3]>=0) rgba = (nRgba *) this->rgba_list.NodeAt(i[3]); 
                    f->AddPoint(v,vn,vt,rgba);
                }
            } else if (strcmp(kw,"t")==0) {
                // Anfang einer Triangle-Strip-Definition
                char *p_str;
                act_face = this->AddFace();
                act_face->SetType(nFace::N_TRISTRIP);
                while ((p_str = strtok(NULL," \t\n"))) {
                    int i   = atoi(p_str)-1;
                    nV *v   =   (nV *)  this->v_list.NodeAt(i);
                    nVT *vt = (nVT *) this->vt_list.NodeAt(i);
                    nVN *vn = (nVN *) this->vn_list.NodeAt(i);
                    nRgba *rgba = (nRgba *) this->rgba_list.NodeAt(i);
                    act_face->AddPoint(v,vn,vt,rgba);
                }
            } else if (strcmp(kw,"q")==0) {
                // Fortfuehrung einer Triangle-Strip-Definition
                char *p_str;
                while ((p_str = strtok(NULL," \t\n"))) {
                    int i   = atoi(p_str)-1;
                    nV *v   = (nV *) this->v_list.NodeAt(i);
                    nVT *vt = (nVT *) this->vt_list.NodeAt(i);
                    nVN *vn = (nVN *) this->vn_list.NodeAt(i);
                    nRgba *rgba = (nRgba *) this->rgba_list.NodeAt(i);
                    act_face->AddPoint(v,vn,vt,rgba);
                }
            }
        }
    }
    return true;
}

//--------------------------------------------------------------------
//  SaveV()
//  03-May-99   floh    created
//--------------------------------------------------------------------
bool nWavefront::SaveV(FILE *fp)
{
    nV *v;
    for (v=(nV *)v_list.GetHead(); v; v=(nV *)v->GetSucc()) {
        if (!v->Print(fp)) return false;
    }
    return true;
}

//--------------------------------------------------------------------
//  SaveVN()
//  03-May-99   floh    created
//--------------------------------------------------------------------
bool nWavefront::SaveVN(FILE *fp)
{
    nVN *vn;
    for (vn=(nVN *)vn_list.GetHead(); vn; vn=(nVN *)vn->GetSucc()) {
        if (!vn->Print(fp)) return false;
    }
    return true;
}

//--------------------------------------------------------------------
//  SaveVT()
//  03-May-99   floh    created
//--------------------------------------------------------------------
bool nWavefront::SaveVT(FILE *fp)
{
    nVT *vt;
    for (vt=(nVT *)vt_list.GetHead(); vt; vt=(nVT *)vt->GetSucc()) {
        if (!vt->Print(fp)) return false;
    }
    return true;
}

//--------------------------------------------------------------------
//  SaveRgba()
//  03-May-99   floh    created
//--------------------------------------------------------------------
bool nWavefront::SaveRgba(FILE *fp)
{
    nRgba *rgba;
    for (rgba=(nRgba *)rgba_list.GetHead(); rgba; rgba=(nRgba *)rgba->GetSucc()) {
        if (!rgba->Print(fp)) return false;
    }
    return true;
}

//--------------------------------------------------------------------
//  SaveFaces()
//  03-May-99   floh    created
//--------------------------------------------------------------------
bool nWavefront::SaveFaces(FILE *fp)
{
    nFace *f;
    for (f=(nFace *)face_list.GetHead(); f; f=(nFace *)f->GetSucc()) {
        if (!f->Print(fp)) return false;
    }
    return true;
}                

//--------------------------------------------------------------------
//  SaveFacesAsFN()
//  16-Aug-99   floh    created
//--------------------------------------------------------------------
bool nWavefront::SaveFacesAsFN(FILE *fp)
{
    nFace *f;
    for (f=(nFace *)face_list.GetHead(); f; f=(nFace *)f->GetSucc()) {
        if (!f->PrintAsFN(fp)) return false;
    }
    return true;
}

//--------------------------------------------------------------------
//  SaveWingedEdges()
//  16-Aug-99   floh    created
//--------------------------------------------------------------------
bool nWavefront::SaveWingedEdges(FILE *fp)
{
    nWingedEdge *we;
    for (we=(nWingedEdge *)we_list.GetHead(); we; we=(nWingedEdge *)we->GetSucc()) {
        if (!we->Print(fp)) return false;
    }
    return true;
}

//--------------------------------------------------------------------
//  Save()
//  23-Dec-98   floh    created
//  16-Aug-99   floh    Facenormals und WingedEdges
//--------------------------------------------------------------------
bool nWavefront::Save(FILE *fp)
{
    bool retval;
    retval  = this->SaveV(fp);
    retval &= this->SaveVN(fp);
    retval &= this->SaveVT(fp);
    retval &= this->SaveRgba(fp);
    retval &= this->SaveFaces(fp);
    if (!this->we_list.IsEmpty()) {
        retval &= this->SaveFacesAsFN(fp);
        retval &= this->SaveWingedEdges(fp);
    }
    return retval;
}

//--------------------------------------------------------------------
//  GenFaceNormals()
//  30-Dec-98   floh    created
//  22-Aug-99   floh    versucht, das genaueste Ergebnis zu finden...
//--------------------------------------------------------------------
void nWavefront::GenFaceNormals()
{
    nFace *f = (nFace *) this->face_list.GetHead();
    if (f) do {
        nFace *fnext = (nFace *) f->GetSucc();
        nPoint *p0 = (nPoint *) f->points.GetHead();
        nPoint *p1 = (nPoint *) p0->GetSucc();
        nPoint *p2 = (nPoint *) p1->GetSucc();
        vector3 v0(p1->v->x - p0->v->x,
                   p1->v->y - p0->v->y,
                   p1->v->z - p0->v->z);
        vector3 v1(p2->v->x - p0->v->x,
                   p2->v->y - p0->v->y,
                   p2->v->z - p0->v->z);
        vector3 v2(p2->v->x - p1->v->x,
                   p2->v->y - p1->v->y,
                   p2->v->z - p1->v->z);

        vector3 n0 = v0*v1;
        vector3 n1 = v1*v2;
        vector3 n2 = v0*v2;
        vector3 n;
        float n0_len = n0.len();
        float n1_len = n1.len();
        float n2_len = n2.len();
        if ((n0_len==0.0f) || (n1_len==0.0f) || (n2_len==0.0f)) {
            fprintf(stderr, "illegal face removed!\n");
            delete f;
        } else {
            if ((n0_len > n1_len) && (n0_len > n2_len)) n = n0;
            else if (n1_len > n2_len)                   n = n1;
            else                                        n = n2;
            n.norm();
            f->nx = n.x;
            f->ny = n.y;
            f->nz = n.z;
        }
        f = fnext;
    } while (f);
}        
             
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
