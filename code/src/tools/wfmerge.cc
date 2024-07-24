//--------------------------------------------------------------------
//  wfmerge [-help] [-file] [-append]
//
//  Merged ein mit wfsplit abgespaltetes Wavefront-Fragment
//  wieder zurueck, einfach indem das Fragment appended wird.
//
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "tools/wftools.h"

//--------------------------------------------------------------------
void copy_stdin(void)
{
    char line_buf[N_MAXPATH];
    char *line;
    while ((line=fgets(line_buf,sizeof(line_buf),stdin))) {
        // Kommentare wegwerfen
        if (line[0] != '#') fputs(line,stdout);
    }
}
//--------------------------------------------------------------------
void copy_file(char *file)
{
    char line_buf[N_MAXPATH];
    char *line;
    FILE *fp = fopen(file,"r");
    if (fp) {
        while ((line=fgets(line_buf,sizeof(line_buf),fp))) {
            fputs(line,stdout);
        }
        fclose(fp);
    }
}    
//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool append;
    bool help;
    char *file;
    int retval = 0;
    
    fprintf(stderr,"-> wfmerge\n");
    
    help   = wf_getboolarg(argc,argv,"-help");
    file   = wf_getstrarg(argc,argv,"-file","split.tmp");
    append = wf_getboolarg(argc,argv,"-append");
    
    if (help) {
        fprintf(stderr,"wfmerge [-help] [-file]\n"
               "(C) 1999 Andre Weissflog\n"
               "Merge Wavefront file fragment back\n"
               "-help: show help and exit\n"
               "-file: defines output filename for filtered lines\n"
               "-append: append fragment to stdin\n");
        return 0;
    }

    if (append) {
        copy_stdin();
        fputc('\n',stdout);
        copy_file(file);
    } else {
        copy_file(file);
        fputc('\n',stdout);
        copy_stdin();
    }

    fprintf(stderr,"<- wfmerge\n");
    return retval;
}
//--------------------------------------------------------------------

    
                
