//--------------------------------------------------------------------
//  wfmeshify.cc
//
//  Triangle-Stripper, basierend auf dem "meshifier" Code von
//
//  Brad Grantham, grantham@asd.sgi.com, http://www.alt.net/~grantham/
//
//  Hereinkommender sollte durch wfclean und wftriang und MUSS
//  durch wfflatten gegangen sein und darf Koordinaten, Normalen,
//  Eckpunktfarben und/oder Textur-Koordinaten enthalten.
//
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "tools/wavefront.h"
#include "tools/wftools.h"
#include "mesh.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>

#define MAXNUM_STRIPS   (1<<16)
#define MAXNUM_VERTICES (1<<16)

int act_strip;                          // aktueller Strip
int act_index;
int strip_start[MAXNUM_STRIPS];         // Index in index_buffer 
int strip_num[MAXNUM_STRIPS];           // Anzahl Indices im Strip
int index_buffer[MAXNUM_VERTICES];      // Sammlung der Indices

//--------------------------------------------------------------------
void beginStrip(int v1, int v2)
{
    strip_start[act_strip] = act_index;
    strip_num[act_strip]   = 2; 
    index_buffer[act_index++] = v1;
    index_buffer[act_index++] = v2;
}
//--------------------------------------------------------------------
void contStrip(int v1)
{
    index_buffer[act_index++] = v1;
    strip_num[act_strip]++;
}
//--------------------------------------------------------------------
void endStrip(void)
{
    act_strip++;
}

//--------------------------------------------------------------------
//  cmp_i3()
//  Vergleicht 2 int[3]-Arrays, kommt true zurueck, wenn sie
//  identisch sind.
//  30-Jun-99   floh    created
//--------------------------------------------------------------------
bool cmp_i3(int *i0, int *i1)
{
    if ((i0[0]==i1[0]) && (i0[1]==i1[1]) && (i0[2]==i1[2])) return true;
    else return false;
}

//--------------------------------------------------------------------
//  rotate_i3()
//  Rotiert eine int[3]-Array um eine Position
//  30-Jun-99   floh    created
//--------------------------------------------------------------------
void rotate_i3(int *i) {
    int tmp = i[2];
    i[2] = i[1];
    i[1] = i[0];
    i[0] = tmp;
}

//--------------------------------------------------------------------
//  flip_strip()
//  Flippe einen Vertex-Strip, indem er einfach in umgekehrter
//  Reihenfolge notiert wird.
//  30-Jun-99   floh    created
//--------------------------------------------------------------------
void flip_strip(int *start, int num)
{
    fprintf(stderr,"flip_strip(%d,%d)\n",(int)start,num);
    int i;

    fprintf(stderr,"before: ");
    for (i=0; i<num; i++) {
        fprintf(stderr,"%d ",start[i]+1);
    }
    fprintf(stderr,"\n");
    
    for (i=0; i<(num/2); i++) {
        int tmp = start[num-1-i];
        start[num-1-i] = start[i];
        start[i] = tmp;
    }

    fprintf(stderr,"after:  ");
    for (i=0; i<num; i++) {
        fprintf(stderr,"%d ",start[i]+1);
    }
    fprintf(stderr,"\n");
}

//--------------------------------------------------------------------
//  fix_orientations()
//  Testet das erste Dreieck jedes Strips, ob es dieselbe
//  Orientation hat, wie das "Original-Dreieck", wenn nicht,
//  wird der Strip geflippt.
//  30-Jun-99   floh    created
//--------------------------------------------------------------------
void fix_orientations(nWavefront *wf)
{
    fprintf(stderr,"-> fixing strip orientations...\n");
    int i;
    for (i=0; i<act_strip; i++) {

        int si[3];
        int start = strip_start[i];
        si[0] = index_buffer[start];
        si[1] = index_buffer[start+1];
        si[2] = index_buffer[start+2];

        // suche Dreieck mit denselben Vertices im Original
        nFace *f;
        bool found = false;
        for (f = (nFace *) wf->face_list.GetHead();
             f;
             f = (nFace *) f->GetSucc())
        {
            nPoint *p0 = (nPoint *) f->points.GetHead();
            nPoint *p1 = (nPoint *) p0->GetSucc();
            nPoint *p2 = (nPoint *) p1->GetSucc();
            int oi[3];
            oi[0] = wf->v_list.IndexOf(p0->v);
            oi[1] = wf->v_list.IndexOf(p1->v);
            oi[2] = wf->v_list.IndexOf(p2->v);

            // identische Vertex-Indices?
            int j;
            if (((oi[0]==si[0]) || (oi[0]==si[1]) || (oi[0]==si[2])) &&
                ((oi[1]==si[0]) || (oi[1]==si[1]) || (oi[1]==si[2])) &&
                ((oi[2]==si[0]) || (oi[2]==si[1]) || (oi[2]==si[2])))
            {
                // Treffer! Versuche durch Rotation eine Uebereinstimmung
                // zu erreichen, wenn das schiefgeht, ist das Dreieck
                // geflippt.
                bool needs_flip = true;
                for (j=0; j<3; j++) {
                    if (cmp_i3(&(si[0]),&(oi[0]))) {
                        needs_flip=false;
                        break;
                    }
                    rotate_i3(&(si[0]));
                }
                found = true;
                if (needs_flip) {
                    fprintf(stderr,"-> flipping strip #%d\n",i);
                    int start = strip_start[i];
                    int num   = strip_num[i];
                    flip_strip(&(index_buffer[start]),num);
                }
            }
        }
        if (!found) {
            fprintf(stderr,"ERROR: could not find triangle for strip #%d\n",i);
        }
    }
}

//--------------------------------------------------------------------
bool gen_mesh(nWavefront *wf)
{
    act_strip = 0;
    act_index = 0;
    memset(strip_start,0,sizeof(strip_start));
    memset(strip_num,0,sizeof(strip_num));
    memset(index_buffer,0,sizeof(index_buffer));

    // fuettert Inhalt des Wavefront-Objects an den
    // meshifier
    bool all_ok = true;
    long num_v, num_vn, num_rgba, num_vt;

    // checken, ob geflattened
    num_v    = wf->NumV();
    num_vn   = wf->NumVN();
    num_vt   = wf->NumVT();
    num_rgba = wf->NumRgba();
    all_ok &= (num_vn > 0)   ? (num_v == num_vn)   : true;
    all_ok &= (num_vt > 0)   ? (num_v == num_vt)   : true;
    all_ok &= (num_rgba > 0) ? (num_v == num_rgba) : true;
    if (!all_ok) {
        fprintf(stderr,"Source file has inconsistent number of vertices!\n");
        return false;
    }

    // Vertices anhaengen
    meshVec3 mv;
    meshVec3 mn;
    meshVec4 mc;
    meshVec3 mt;
    nV    *v    = (nV *)    wf->v_list.GetHead();
    nVN   *vn   = (nVN *)   wf->vn_list.GetHead();
    nVT   *vt   = (nVT *)   wf->vt_list.GetHead();
    nRgba *rgba = (nRgba *) wf->rgba_list.GetHead();
    if (v) do {

        memset(&mv,0,sizeof(mv));
        memset(&mn,0,sizeof(mn));
        memset(&mc,0,sizeof(mc));
        memset(&mt,0,sizeof(mt));

        mv[X]=v->x; mv[Y]=v->y; mv[Z]=v->z;

        /*
        if (vn) {
            mn[X]=vn->nx; mn[Y]=vn->ny; mn[Z]=vn->nz;
        }
        if (vt) {
            mt[R]=vt->u; mt[S]=vt->v;
        }
        if (rgba) {
            mc[R]=rgba->r; mc[G]=rgba->g; mc[B]=rgba->b; mc[A]=rgba->a;
        }
        */

        meshAddVertex(mv,mn,mc,mt);
        if (vn)   vn   = (nVN *)   vn->GetSucc();
        if (vt)   vt   = (nVT *)   vt->GetSucc();
        if (rgba) rgba = (nRgba *) rgba->GetSucc();
    } while ((v = (nV *) v->GetSucc()));

    // Triangles anhaengen
    nFace *f;
    for (f = (nFace *) wf->face_list.GetHead();
         f;
         f = (nFace *) f->GetSucc())
    {
        nPoint *p0 = (nPoint *) f->points.GetHead();
        nPoint *p1 = (nPoint *) p0->GetSucc();
        nPoint *p2 = (nPoint *) p1->GetSucc();
        long i0 = wf->v_list.IndexOf(p0->v);
        long i1 = wf->v_list.IndexOf(p1->v);
        long i2 = wf->v_list.IndexOf(p2->v);
        meshAddTriangle(i0,i1,i2);
    }

    // Callback-Hooks setzen
    meshSetProcs(NULL,NULL,NULL,beginStrip,contStrip,endStrip);
    meshCreateStrips();
    fprintf(stderr, "-> %d strips generated\n", act_strip); 
    return true;
}

//--------------------------------------------------------------------
void write_mesh(void)
{
    int i;
    for (i=0; i<act_strip; i++) {
        printf("t ");
        int j;
        for (j=0; j<strip_num[i]; j++) {
            printf("%d ",1+index_buffer[strip_start[i]+j]);
        }
        printf("\n");
    }
}

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool help;
    long retval = 0;
    nWavefront wf;
    
    fprintf(stderr,"-> wfmeshify\n");
    
    // Args auswerten
    help = wf_getboolarg(argc, argv, "-help");
    if (help) {
        fprintf(stderr, "wfmeshify [-help]\n"
                        "based on mesh.c by Brad Grantham, grantham@asd.sgi.com\n"
                        "Generates triangle strips from wavefront obj.\n"
                        "-help          -- show help\n");
        return 0;
    }
    
    // und los...
    fprintf(stderr,"loading...\n");
    if (!wf.Load(stdin)) {
        fprintf(stderr,"Load failed!\n");
        retval = 10; goto ende;
    }
    gen_mesh(&wf);
    fix_orientations(&wf);
    wf.SaveV(stdout);
    wf.SaveVN(stdout);
    wf.SaveVT(stdout);
    write_mesh();

ende:
    fprintf(stderr,"<- wfmeshify\n");
    return retval;
}
//--------------------------------------------------------------------

