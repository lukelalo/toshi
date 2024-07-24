//--------------------------------------------------------------------
//  wfsmooth [-help] [-angle]
//
//  Recompute face normals, expects a threshold angle which
//  defines the 'hard edge' angle.
//
//  (C) 1998..2000 Andre Weissflog
//--------------------------------------------------------------------
#include "tools/wftoolbox.h"
#include "tools/wftools.h"

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool help;
    long retval = 0;
    float angle;
        
    fprintf(stderr,"-> wfsmooth\n");
    
    // Args auswerten
    help  = wf_getboolarg(argc, argv, "-help");
    angle = wf_getfloatarg(argc, argv, "-angle", 0.0f);
    if (help) {
        fprintf(stderr, "wfsmooth [-help] [-angle]\n"
                        "(C) 1998 Andre Weissflog\n"
                        "Generate face normals.\n"   
                        "-help          -- show help\n"
                        "-angle         -- smooth/hard edge angle\n");
        return 0;
    }

    wfToolbox toolbox;
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

    // do the smoothing...
    toolbox.smooth(src,dst,angle);

    // write the result
    fprintf(stderr,"saving...\n");
    dst.save(out);
    
    wf_closefiles(in, out);

ende:
    fprintf(stderr,"<- wfsmooth\n");
    return retval;
}  
    
    
