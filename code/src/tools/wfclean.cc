//--------------------------------------------------------------------
//  wfclean.cc
//
//  Testet Wavefront-Objekt auf identische v's, vt's und vn's 
//  (entsprechend einer gegebenen Toleranz) und entfernt die
//  redundanten Elemente.
//
//  (C) 1998..2000 A.Weissflog
//--------------------------------------------------------------------
#include "tools/wftoolbox.h"
#include "tools/wftools.h"

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool help;
    long retval = 0;

    float vtol, vntol, vttol, rgbatol;

    fprintf(stderr,"-> wfclean\n");
    
    // Args auswerten
    help     = wf_getboolarg(argc, argv, "-help");
    vtol     = wf_getfloatarg(argc, argv, "-vtol",    0.0000001f);
    vntol    = wf_getfloatarg(argc, argv, "-vntol",   0.0000001f);
    vttol    = wf_getfloatarg(argc, argv, "-vttol",   0.0000001f);
    rgbatol  = wf_getfloatarg(argc, argv, "-rgbatol", 0.0000001f);
    if (help) {
        fprintf(stderr, "wfclean [-help] [-vtol] [-vntol] [-vttol]\n"
                        "(C) 1998 Andre Weissflog\n"
                        "Remove redundant v's, vn's, vt's, rgba's dependent on tolerance.\n"
                        "-help  -- show help\n"
                        "-vtol  -- tolerance for v's (def 0.0)\n"
                        "-vntol -- tolerance for vn's (def 0.0)\n"
                        "-vttol -- tolerance for vt's (def 0.0)\n"
                        "-rgbatol -- tolerance for rgba's (def 0.0)\n");
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

    // do the cleanup stuff
    toolbox.clean(src,dst,vtol,vntol,vttol,rgbatol);

    // write the result
    fprintf(stderr,"saving...\n");
    dst.save(out);
    
    wf_closefiles(in, out);

ende:
    fprintf(stderr,"<- wfclean\n");
    return retval;
}  

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
