//--------------------------------------------------------------------
//  wflodmerge [-help] [lod-files...]
//
//  Liest mehrere LOD-Levels in Form von Stripe-Files ein,
//  und merged diese in einen einzigen Output-File.
//
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "tools/wavefront.h"
#include "tools/wftools.h"

#define MAXNUM_WF (64)

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int i;
    bool help;
    long retval = 0;
    int num_wf;
    nWavefront *wf_in[MAXNUM_WF];
    float lod[MAXNUM_WF];
    
    fprintf(stderr,"-> wflodmerge\n");
    
    // Args auswerten...
    help = wf_getboolarg(argc,argv,"-help");
    if (help) {
        fprintf(stderr,"wflodmerge [-help] [file:lod ...]\n"
                       "(C) 1999 A.Weissflog\n"
                       "Merge striped detail-levels into one stripe file.\n"
                       "-help: show help and exit\n"
                       "files:lod ...: input file names with LOD weight\n");
        return 0;
    }
    
    // alle Args, die nicht mit '-' anfangen, sind Input-Filenamen...
    memset(wf_in,0,sizeof(wf_in));
    num_wf = 0;
    for (i=1; i<argc; i++) {
        if (argv[i][0] != '-') {
            char buf[256];
            strcpy(buf,argv[i]);
            char *tmp = strchr(buf,':');
            if (tmp) {
                *tmp++ = 0;
                char *fname = buf;
                lod[num_wf] = (float) atof(tmp);
                FILE *fp;
                fprintf(stderr,"loading %s...\n",fname);
                fp = fopen(fname,"r");
                if (fp) {
                    wf_in[num_wf] = new nWavefront;
                    if (!wf_in[num_wf]->Load(fp)) {
                        fprintf(stderr,"Load failed!\n");
                        retval = 10; goto ende;
                    } else {
                        num_wf++;
                    }
                } else {
                    fprintf(stderr,"Could not open file!\n");
                    retval = 10; goto ende;
                }
            } else {
                fprintf(stderr,"':' expected in argument '%s'!\n",argv[i]);
                retval = 10; goto ende;
            }
        }
    }
    fprintf(stderr,"saving...\n");
    for (i=0; i<num_wf; i++) {
        printf("lod %f\n",lod[i]);
        wf_in[i]->SaveV(stdout);
        wf_in[i]->SaveVN(stdout);
        wf_in[i]->SaveVT(stdout);
        wf_in[i]->SaveRgba(stdout);
        wf_in[i]->SaveFaces(stdout);
    }
ende:
    for (i=0; i<num_wf; i++) delete wf_in[i];
    fprintf(stderr,"<- wflodmerge\n");
    return retval;
}

    
    
    
    
    
                
        
