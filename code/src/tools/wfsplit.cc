//--------------------------------------------------------------------
//  wfsplit [-help] [-file] [-keys]
//
//  Splittet einen oder mehrere durch Keywords definierte
//  Wavefront-Channels aus Stdin. Die herausgenommenen
//  Channels werden nach -file geschrieben, der Rest geht
//  nach Stdout.
//  
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "tools/wftools.h"

#include <stdlib.h>
#include <string.h>

#define MAXNUM_KEYS (16)

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool help;
    char key_buf[N_MAXPATH];
    char *keys[MAXNUM_KEYS];
    char *file;
    int retval = 0;
    int num_keys;

    fprintf(stderr,"-> wfsplit\n");
        
    // Args auswerten
    help = wf_getboolarg(argc,argv,"-help");
    file = wf_getstrarg(argc,argv,"-file","split.tmp");
    wf_getstrargs(argc,argv,"-keys",key_buf,sizeof(key_buf));
        
    // Hilfe anzeigen?
    if (help) {
        fprintf(stderr,"wfsplit [-help] [-file] [-keys]\n"
               "(C) 1999 Andre Weissflog\n"
               "Split lines from Wavefront file to extra output file.\n"
               "-help: show help and exit\n"
               "-file: defines output filename for filtered lines\n"
               "-keys: Wavefront keywords of filtered lines\n");
        return 0;
    }

    // splitte Keywords in 'key_buf'
    char *tmp = key_buf;
    for (num_keys=0; num_keys<MAXNUM_KEYS; num_keys++) {
        keys[num_keys] = strtok(tmp," ");
        if (tmp) tmp=NULL;
    }
    
    // lese Zeilen aus stdin und schreibe nach stdout oder fp
    FILE *fp = fopen(file,"w");
    if (fp) {
        char lb[N_MAXPATH];
        char *line;
        while ((line = fgets(lb,sizeof(lb),stdin))) {
            char line_buf[N_MAXPATH];
            n_strncpy2(line_buf,line,sizeof(line_buf));
            char *kw = strtok(line_buf," \t\n");
            
            // Leer- und Kommentarzeilen werden geschluckt
            if (kw && (kw[0] != '#')) {
                int i;
                bool match = false;
                for (i=0; (i<num_keys) && (keys[i]); i++) {
                    if (strcmp(kw,keys[i])==0) {
                        match=true;
                        break;
                    }
                }
                if (match) fputs(line,fp);
                else       fputs(line,stdout);
            }
        }
        fclose(fp);
    }
    fprintf(stderr,"<- wfsplit\n");
    return retval;
}  

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
        
        
                
                
                
                    
                
                        
                
                 
