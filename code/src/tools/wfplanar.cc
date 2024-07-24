//------------------------------------------------------------------------------
/**
    Planar projection wavefront tool. Generates a new texture coordinate
    set.
*/
#include "tools/wftoolbox.h"
#include "tools/wftools.h"

//-----------------------------------------------------------------------------
/**
    Da main function!
*/
int main(int argc, char* argv[])
{
    bool help;
    int retval = 0;

    vector4 uParams;
    vector4 vParams;
    int texLayer;

    fprintf(stderr, "-> wfplanar\n");

    // read args
    help     = wf_getboolarg(argc, argv, "-help");
    texLayer = wf_getintarg(argc, argv, "-layer", 0);
    uParams.x = wf_getfloatarg(argc, argv, "-ux", 0.0f);
    uParams.y = wf_getfloatarg(argc, argv, "-uy", 0.0f);
    uParams.z = wf_getfloatarg(argc, argv, "-uz", 0.0f);
    uParams.w = wf_getfloatarg(argc, argv, "-uw", 0.0f);
    vParams.x = wf_getfloatarg(argc, argv, "-vx", 0.0f);
    vParams.y = wf_getfloatarg(argc, argv, "-vy", 0.0f);
    vParams.z = wf_getfloatarg(argc, argv, "-vz", 0.0f);
    vParams.w = wf_getfloatarg(argc, argv, "-vw", 0.0f);

    // show help???
    if (help)
    {
        fprintf(stderr, "wfplanar [-help] [-layer] [-ux] [-uy] [-uz] [-uw] [-vx] [-vy] [-vz] [-vw]\n"
                        "(C) 2001 Andre Weissflog\n"
                        "Generate a new set of uv coords by doing planar mapping.\n"
                        "-help              -- show help\n"
                        "-layer             -- the texture layer to generate [0..3]\n"
                        "-ux, -uy, -uz, -uw -- the u parameters (see below)\n"
                        "-vx, -vy, -vz, -vw -- the v parameters (see below)\n"
                        "\n"
                        "uv coordinates are computed as follows:\n"
                        "u = ox * ux + oy * uy + oz * uz + uw\n"
                        "v = ox * vx + oy * vy + oz * vz + vw\n"
                        "Where [ox,oy,oz] are the 3d coordinates of the object.\n");
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
    
    // do the planar mapping
    toolbox.mapPlanar(src, dst, texLayer, uParams, vParams);

    // write the result
    fprintf(stderr,"saving...\n");
    dst.save(out);
    
    wf_closefiles(in, out);

ende:
    fprintf(stderr,"<- wfplanar\n");
    return retval;
}  
