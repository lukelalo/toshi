//--------------------------------------------------------------------
//  wftrans.cc
//
//  Translatiert, rotiert, skaliert Wavefront-File. Alle Normalen
//  werden nach der Transformation generell renormalisiert.
//
//  24-Oct-00   floh    rewritten to wfObject
//
//  (C) 1998 A.Weissflog
//--------------------------------------------------------------------
#include "tools/wftoolbox.h"
#include "tools/wftools.h"
#include "math/nmath.h"

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool help;
    long retval = 0;
    
    float tx,ty,tz;
    float rx,ry,rz;
    float sx,sy,sz;
    float s;
    
    vector3 trans,rot,scale;

    fprintf(stderr,"-> wftrans\n");
    
    // Args auswerten
    help = wf_getboolarg(argc, argv, "-help");
    tx   = wf_getfloatarg(argc, argv, "-tx", 0.0f);
    ty   = wf_getfloatarg(argc, argv, "-ty", 0.0f);
    tz   = wf_getfloatarg(argc, argv, "-tz", 0.0f);
    rx   = wf_getfloatarg(argc, argv, "-rx", 0.0f);
    ry   = wf_getfloatarg(argc, argv, "-ry", 0.0f);
    rz   = wf_getfloatarg(argc, argv, "-rz", 0.0f);
    sx   = wf_getfloatarg(argc, argv, "-sx", 1.0f);
    sy   = wf_getfloatarg(argc, argv, "-sy", 1.0f);
    sz   = wf_getfloatarg(argc, argv, "-sz", 1.0f);
    s    = wf_getfloatarg(argc, argv, "-s",  1.0f);
    if (s != 1.0f) {
        sx = sy = sz = s;
    }
    if (help) {
        fprintf(stderr, "wftrans [-help] [-tx] [-ty] [-tz] "
                        "[-rx] [-ry] [-rz] [-sx] [-sy] [-sz] [-s]\n"
                        "(C) 1998 Andre Weissflog\n"
                        "Transforms a wavefront object.\n"
                        "-help          -- show help\n"
                        "-tx -ty -tz    -- translate params\n"
                        "-rx -ry -rz    -- rotate params in degree\n"
                        "-sx -sy -sz    -- scale params\n"
                        "-s             -- shortcut, overrides -sx,-sy,-sz\n");
        return 0;
    }
    
    wfToolbox toolbox;
    wfObject src;

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

    // generate transformation vectors
    trans.set(tx,ty,tz);
    rot.set(n_deg2rad(rx),n_deg2rad(ry),n_deg2rad(rz));
    scale.set(sx,sy,sz);

    // do the transformation
    toolbox.transform(src,trans,rot,scale);

    // write the result
    fprintf(stderr,"saving...\n");
    src.save(out);
    
    wf_closefiles(in, out);
    
ende:
    fprintf(stderr,"<- wftrans\n");
    return retval;
}  
