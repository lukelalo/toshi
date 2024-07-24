//-------------------------------------------------------------------
//  wfnormalize 
//  
//  Normalize a wavefront object so that it fits into a unit sphere.
//
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "tools/wavefront.h"
#include "tools/wftools.h"
#include "mathlib/vector.h"

//-------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool help;
    int retval = 0;
    nWavefront wf;
    nV *v;
    float max_len;
    int i;

    fprintf(stderr,"-> wfnormalize\n");

    // get command line args
    help = wf_getboolarg(argc, argv, "-help");
    if (help) {
        fprintf(stderr, "wfnormalize [-help]\n"
                        "(C) 2000 RadonLabs GmbH -- A.Weissflog\n"
                        "Normalize a Wavefront object so it fits into a unit sphere.\n"
                        "-help          -- show help\n");
        return 0;
    }

    // und los...
    fprintf(stderr,"loading...\n");
    if (!wf.Load(stdin)) {
        fprintf(stderr,"Load failed!\n");
        retval = 10; goto ende;
    }

    // get the maximum radius
    max_len = 0.0f;
    fprintf(stderr,"normalizing...\n");
    for (v=(nV *)wf.v_list.GetHead(),i=0; v; v=(nV *) v->GetSucc(),i++) {
        vector3 cv(v->x,v->y,v->z);
        float l = cv.len();
        if (l > max_len) max_len = l;
    }
    
    // divide all vertices by max_len
    if (max_len > 0.0f) {
        for (v=(nV *)wf.v_list.GetHead(); v; v=(nV *)v->GetSucc()) {
            v->x /= max_len;
            v->y /= max_len;
            v->z /= max_len;
        }
    }

    // write result...
    fprintf(stderr,"saving...\n");
    if (!wf.Save(stdout)) {
       fprintf(stderr,"Save failed!\n");
       retval = 10; goto ende;
    }

ende:
    fprintf(stderr,"<- wfnormalize\n");
    return retval;
}  

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
