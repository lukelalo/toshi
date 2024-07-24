//--------------------------------------------------------------------
//  wfradius.cc
//
//  Get radius of wavefront file, and write it to an ascii file.
//  The original Wavefront-File goes to standard out
//  as usual.
//
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//--------------------------------------------------------------------
#include "tools/wavefront.h"
#include "tools/wftools.h"
#include "mathlib/vector.h"

#ifdef __WIN32__
#define putenv _putenv
#endif

//--------------------------------------------------------------------
//  wf_getradius()
//  Get radius from Wavefront object and write to extra file.
//  22-Aug-00   floh    created
//--------------------------------------------------------------------
void wf_getradius(nWavefront& wf, const char *file)
{
    float max_len = 0.0f;
    nV *v;
    for (v=(nV *)wf.v_list.GetHead(); v; v=(nV *) v->GetSucc()) {
        vector3 cv(v->x,v->y,v->z);
        float l = cv.len();
        if (l > max_len) max_len = l;
    }
    FILE *fp = fopen(file,"w");
    if (fp) {
        fprintf(fp,"%f",max_len);
        fclose(fp);
    }
}

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool help;
    const char *file;
    long retval = 0;
    nWavefront wf;
    fprintf(stderr,"-> wfradius\n");
        
    // get arguments
    help = wf_getboolarg(argc,argv,"-help");
    file = wf_getstrarg(argc,argv,"-file","radius.tmp");
    
    // show help?
    if (help) {
        fprintf(stderr,"wfradius [-help] [-file]\n"
               "(C) 2000 RadonLabs GmbH -- A.Weissflog\n"
               "Get the radius of a wavefront file and write to ascii file.\n"
               "-help: show help and exit\n"
               "-file: name of file to write radius to\n");
        return 0;
    }
    
    fprintf(stderr,"loading...\n");
    if (!wf.Load(stdin)) {
        fprintf(stderr,"Load failed!\n");
        retval = 10; goto ende;
    }
    wf_getradius(wf,file);
    fprintf(stderr,"saving...\n");
    if (!wf.Save(stdout)) {
       fprintf(stderr,"Save failed!\n");
       retval = 10; goto ende;
    }
    
ende:
    fprintf(stderr,"<- wfradius\n");
    return retval;
}  
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
