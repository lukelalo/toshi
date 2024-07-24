//--------------------------------------------------------------------
//  wfflatten
//
//  Macht ein Wavefront-Objekt "flach":
//      - Zuerst werden saemtliche "shared"-Beziehungen
//        aufgefaltet, jeder Polygon-Eckpunkt bekommt
//        seinen eigenen Vertex, Normale und UV.
//      - Ueber diese Eckpunt-Parameter wird ein
//        Redundanz-Check gemacht.
//      - Zuletzt wird noch ein "illegal Faces"
//        Check durchgefuehrt.
//
//  (C) 1998 A.Weissflog
//--------------------------------------------------------------------
#include "tools/wftoolbox.h"
#include "tools/wftools.h"

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool help;
    long retval = 0;

    fprintf(stderr,"-> wfflatten\n");
    
    // get command line args
    help = wf_getboolarg(argc, argv, "-help");
    if (help) {
        fprintf(stderr, "wfflatten [-help]\n"
                        "(C) 1998 Andre Weissflog\n"
                        "Flatten a wavefront file (unshare v's, vn's, vt's)\n"
                        "-help  -- show help\n");
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

    // load...
    fprintf(stderr,"loading...\n");
    if (!src.load(in)) {
        fprintf(stderr,"Load failed!\n");
        retval = 10; goto ende;
    }
    
    // do the flattening stuff...
    fprintf(stderr,"flattening...\n");
    toolbox.flatten(src,dst);

    // write the result
    fprintf(stderr,"saving...\n");
    dst.save(out);
    
    wf_closefiles(in, out);

ende:
    fprintf(stderr,"<- wfflatten\n");
    return retval;
}  
    
