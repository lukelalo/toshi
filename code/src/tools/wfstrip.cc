//--------------------------------------------------------------------
//  wfstrip.cc
//  
//  Entferne Koordinaten-Channels aus einem Wavefront-File.
//
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "tools/wavefront.h"
#include "tools/wftools.h"

//--------------------------------------------------------------------
void strip(nWavefront *wf,
           bool strip_vt,
           bool strip_vn,
           bool strip_rgba)
{
    nFace *f;
    for (f = (nFace *) wf->face_list.GetHead();
         f;
         f = (nFace *) f->GetSucc())
    {
        nPoint *p;
        for (p = (nPoint *) f->points.GetHead();
             p;
             p = (nPoint *) p->GetSucc())
        {
            if (strip_vt && p->vt) {
                p->vt->use_count--;
                if (p->vt->use_count == 0) delete p->vt;
                p->vt = NULL;
            }
            if (strip_vn && p->vn) {
                p->vn->use_count--;
                if (p->vn->use_count == 0) delete p->vn;
                p->vn = NULL;
            }
            if (strip_rgba && p->rgba) {
                p->rgba->use_count--;
                if (p->rgba->use_count == 0) delete p->rgba;
                p->rgba = NULL;
            }
        }
    }
}

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    nWavefront wf;
    bool help;
    long retval = 0;
    
    bool strip_vt   = false;
    bool strip_vn   = false;
    bool strip_rgba = false;

    fprintf(stderr, "-> wfstrip\n");
    
    // Args auswerten
    help       = wf_getboolarg(argc,argv,"-help");
    strip_vt   = wf_getboolarg(argc,argv,"-vt");
    strip_vn   = wf_getboolarg(argc,argv,"-vn");
    strip_rgba = wf_getboolarg(argc,argv,"-rgba");
    if (help) {
            fprintf(stderr,"wfstrip [-help] [-vt] [-vn] [-rgba]\n"
                           "(C) 1999 A.Weissflog\n"
                           "Strip vertex channels from wavefront file\n"
                           "-help -- show this help\n"
                           "-vt   -- strip vt channel\n"
                           "-vn   -- strip vn channel\n"
                           "-rgba -- strip rgba channel\n");
            return 0;
    }
    
    fprintf(stderr,"loading...\n");
    if (!wf.Load(stdin)) {
        fprintf(stderr,"Load failed!\n");
        retval = 10; goto ende;
    }

    fprintf(stderr,"stripping...\n");    
    strip(&wf,strip_vt,strip_vn,strip_rgba);

    fprintf(stderr,"saving...\n");
    if (!wf.Save(stdout)) {
       fprintf(stderr,"Save failed!\n");
       retval = 10; goto ende;
    }
    
ende:
    fprintf(stderr,"<- wfstrip\n");
    return retval;
}  
             
    
    
