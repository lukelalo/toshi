/**
    wfspherical.cc

    Read a wavefront file, remap uv coordinates by doing a simple spherical
    mapping.

    (C) 2001 RadonLabs GmbH -- A.Weissflog
*/
#include "tools/wftoolbox.h"
#include "tools/wftools.h"

//-----------------------------------------------------------------------------
/**
    Da main function!
*/
int main(int argc, char *argv[])
{
    bool help;
    long retval = 0;

    fprintf(stderr, "-> wfspherical\n");

    // read args
    help = wf_getboolarg(argc, argv, "-help");
    if (help)
    {
        fprintf(stderr, "wfspherical [-help]\n"
                        "(C) 2001 Andre Weissflog\n"
                        "Generate uv coords by doing spherical mapping.\n"
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

    // load source object
    fprintf(stderr,"loading...\n");
    if (!src.load(in)) {
        fprintf(stderr,"Load failed!\n");
        retval = 10; goto ende;
    }
    
    // do the spherical mapping
    toolbox.map_spherical(src, dst);

    // write the result
    fprintf(stderr,"saving...\n");
    dst.save(out);
    
    wf_closefiles(in, out);

ende:
    fprintf(stderr,"<- wfclean\n");
    return retval;
}  
