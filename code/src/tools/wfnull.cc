//--------------------------------------------------------------------
//  wfnull [-help]
//
//  Liest einen Wavefront-File und schreibt ihn wieder raus.
//  
//  24-Oct-00   floh    rewritten to wfObject 
//
//  (C) 1998 A.Weissflog
//--------------------------------------------------------------------
#include "tools/wfobject.h"
#include "tools/wftools.h"

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool help;
    long retval = 0;
    fprintf(stderr,"-> wfnull\n");
        
    // read args
    help = wf_getboolarg(argc,argv,"-help");
    
    // show help?
    if (help) {
        fprintf(stderr,"wfnull [-help]\n"
               "(C) 1998 Andre Weissflog\n"
               "Load and save wavefront file, removing fluff.\n"
               "-help: show help and exit\n");
        return 0;
    }
    
    wfObject src;
    FILE *in, *out;
    if (!wf_openfiles(argc, argv, in, out)) {
        fprintf(stderr,"file open failed!\n");
        retval = 10; goto ende;
    }

    fprintf(stderr,"loading...\n");
    if (!src.load(in)) {
        fprintf(stderr,"Load failed!\n");
        retval = 10; goto ende;
    }

    // print stats
    fprintf(stderr,"-> num v:    %d\n",src.v_array.Size());
    fprintf(stderr,"-> num vn:   %d\n",src.vn_array.Size());
    fprintf(stderr,"-> num vt:   %d\n",src.vt_array.Size());
    fprintf(stderr,"-> num rgba: %d\n",src.c_array.Size());
    fprintf(stderr,"-> num f:    %d\n",src.f_array.Size());

    fprintf(stderr,"saving...\n");
    src.save(out);

    wf_closefiles(in,out);
    
ende:
    fprintf(stderr,"<- wfnull\n");
    return retval;
}  
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
