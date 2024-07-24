//------------------------------------------------------------------------------
//  wfwedge.cc
//
//  Generate winged edge information (new).
//
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/wftoolbox.h"
#include "tools/wftools.h"

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    bool help;
    int retval = 0;

    fprintf(stderr, "-> wfedge\n");

    // get args
    help = wf_getboolarg(argc, argv, "-help");
    if (help)
    {
        fprintf(stderr, "wfedge [-help]\n"
                        "(C) 2001 RadonLabs GmbH\n"
                        "Generate winged edge information on a cleaned file.\n"
                        "-help  -- show help\n");
        return 0;
    }

    wfToolbox toolbox;
    wfObject src;

    FILE* in, *out;
    if (!wf_openfiles(argc, argv, in, out))
    {
        fprintf(stderr, "file open failed!\n");
        retval = 10; goto ende;
    }

    // load object...
    fprintf(stderr, "loading...\n");
    if (!src.load(in))
    {
        fprintf(stderr, "load failed!\n");
        retval = 10; goto ende;
    }

    // generate winged edges
    toolbox.genWingedEdges(src);

    // write result
    fprintf(stderr, "saving...\n");
    src.save(out);

    wf_closefiles(in, out);

ende:
    fprintf(stderr, "<- wfedge\n");
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
