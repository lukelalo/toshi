//--------------------------------------------------------------------
//  wfappend [-help]
//
//  Append a Wavefront object to stdin, update face indices.
//
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "tools/wavefront.h"
#include "tools/wftools.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <assert.h>

//--------------------------------------------------------------------
//  wf_map()
//  Map one pointer to another using linear a mapping table.
//  07-Oct-99   floh    created
//--------------------------------------------------------------------
void *wf_map(void **map, void *p, int num)
{
    int i;
    for (i=0; i<num; i++) {
        if (map[2*i] == p) return map[2*i+1];
    }
    return NULL;
}

//--------------------------------------------------------------------
//  wf_append()
//  07-Oct-99   floh    created
//--------------------------------------------------------------------
void wf_append(nWavefront& wf0, nWavefront& wf1)
{
    // alloziere Mapping-Tables
    int num_v    = wf1.NumV();
    int num_vn   = wf1.NumVN();
    int num_vt   = wf1.NumVT();
    int num_rgba = wf1.NumRgba();
    nV  **v_map      = (nV **)  malloc(num_v  * sizeof(void *) * 2);
    nVN **vn_map     = (nVN **) malloc(num_vn * sizeof(void *) * 2);
    nVT **vt_map     = (nVT **) malloc(num_vt * sizeof(void *) * 2);
    nRgba **rgba_map = (nRgba **) malloc(num_rgba * sizeof(void *) * 2);
    assert(v_map && vn_map && vt_map && rgba_map);

    // copy v's, vn's, vt's, rgba's
    nV *v;
    int i;
    for (i=0, v=(nV *) wf1.v_list.GetHead();
         v;
         i++, v=(nV *) v->GetSucc())
    {
        v_map[i*2]   = v;
        v_map[i*2+1] = wf0.AddV(v->x,v->y,v->z);
    }
    nVN *vn;
    for (i=0, vn=(nVN *) wf1.vn_list.GetHead();
         vn;
         i++, vn=(nVN *) vn->GetSucc())
    {
        vn_map[i*2]   = vn;
        vn_map[i*2+1] = wf0.AddVN(vn->nx,vn->ny,vn->nz);
    }
    nVT *vt;
    for (i=0, vt=(nVT *) wf1.vt_list.GetHead();
         vt;
         i++, vt=(nVT *) vt->GetSucc())
    {
        vt_map[i*2]   = vt;
        vt_map[i*2+1] = wf0.AddVT(vt->u,vt->v);
    }
    nRgba *rgba;
    for (i=0, rgba=(nRgba *) wf1.rgba_list.GetHead();
         rgba;
         i++, rgba=(nRgba *) rgba->GetSucc())
    {
        rgba_map[i*2] = rgba;
        rgba_map[i*2+1] = wf0.AddRgba(rgba->r,rgba->g,rgba->b,rgba->a);
    }

    // copy faces
    nFace *f0;
    for (f0 = (nFace *) wf1.face_list.GetHead();
         f0;
         f0 = (nFace *) f0->GetSucc())
    {
        nFace *f1 = wf0.AddFace();
        f1->type = f0->type;
        nPoint *p0;
        for (p0 = (nPoint *) f0->points.GetHead();
             p0;
             p0 = (nPoint *) p0->GetSucc())
        {
            nV *v = NULL;
            nVN *vn = NULL;
            nVT *vt = NULL;
            nRgba *rgba = NULL;
            if (p0->v)    v    = (nV *)    wf_map((void **)v_map,p0->v,num_v);
            if (p0->vn)   vn   = (nVN *)   wf_map((void **)vn_map,p0->vn,num_vn);
            if (p0->vt)   vt   = (nVT *)   wf_map((void **)vt_map,p0->vt,num_vt);
            if (p0->rgba) rgba = (nRgba *) wf_map((void **)rgba_map,p0->rgba,num_rgba);
            f1->AddPoint(v,vn,vt,rgba);
        }
    }

    free(v_map);
    free(vn_map);
    free(vt_map);
    free(rgba_map);
}

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
#define MAXNUM_FNAMES (512)
    bool help;
    long retval = 0;
    nWavefront wf0;
    FILE *f;
    char buf[128*MAXNUM_FNAMES];
    char *fn_array[MAXNUM_FNAMES];
    char *fnames;
    char *fn;
    int num,i;

    fprintf(stderr,"-> wfappend\n");
        
    // Args auswerten
    help   = wf_getboolarg(argc,argv,"-help");
    fnames = wf_getstrargs(argc,argv,"-append",buf,sizeof(buf));
    
    // Hilfe anzeigen?
    if (help) {
        fprintf(stderr,"wfappend [-help] [-append]\n"
               "(C) 1999 Andre Weissflog\n"
               "Append Wavefront files to stream, update face indices.\n"
               "-help:   show help and exit\n"
               "-append: names of file to append\n");
        return 0;
    }
    if (!fnames) {
        fprintf(stderr,"Error: No filenames given!\n");
        return 10;
    }

    fprintf(stderr,"loading from stdin...\n");
    if (!wf0.Load(stdin)) {
        fprintf(stderr,"Load failed!\n");
        retval = 10; goto ende;
    }

    // fill array with file names
    num = 0;
    while ((fn=strtok(fnames," \t")) && (num<MAXNUM_FNAMES)) {
        if (fnames) fnames = NULL;
        fn_array[num++] = fn;
    }
    for (i=0; i<num; i++) {
        fn = fn_array[i];
        fprintf(stderr,"appending %s...\n",fn);
        f = fopen(fn,"r");
        if (f) {
            nWavefront *wf1 = new nWavefront;
            if (wf1->Load(f)) wf_append(wf0,*wf1);
            else fprintf(stderr,"Loading data from '%s' failed!\n",fn);
            delete wf1;
            fclose(f);
        } else {
            fprintf(stderr,"Could not open file '%s'!\n",fn);
        }
    }

    fprintf(stderr,"saving...\n");
    if (!wf0.Save(stdout)) {
       fprintf(stderr,"Save failed!\n");
       retval = 10; goto ende;
    }
    
ende:
    fprintf(stderr,"<- wfappend\n");
    return retval;
}  
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
