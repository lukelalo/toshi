//--------------------------------------------------------------------
//  wftools.cc
//
//  Allgemeine Support-Routinen fuer wf-Tools.
//
//  (C) 1998 A.Weissflog
//--------------------------------------------------------------------
#include "tools/wftools.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <math.h>

//--------------------------------------------------------------------
//  wf_findarg()
//  
//  27-Dec-98   floh    created
//--------------------------------------------------------------------
int wf_findarg(int argc, char *argv[], char *opt)
{
    int i;
    for (i=1; i<argc; i++) {
        char *arg = argv[i];
        if (strcmp(arg,opt)==0) return i;
    }
    return 0;
}    

//--------------------------------------------------------------------
//  wf_getfloatarg()
//  wf_getstrarg()
//  wf_getintarg()
//  wf_getboolarg()
//
//  Suche Argument-Wert definiert durch "opt", wenn nicht
//  existent, returniere "def", wenn Fehler, exit();
//  Die "bool" Funktion returniert true, wenn die Option
//  existiert, false, wenn nicht.
//
//  27-Dec-98   floh    created
//--------------------------------------------------------------------
float wf_getfloatarg(int argc, char *argv[], char *opt, float def)
{
    int i = wf_findarg(argc,argv,opt);
    if (i == 0) return def;
    else if (++i < argc) return (float) atof(argv[i]);
    else {
        fprintf(stderr,"arg error after %s\n", opt);
        exit(10);
    }
    return 0.0f;
}

char *wf_getstrarg(int argc, char *argv[], char *opt, char *def)
{
    int i = wf_findarg(argc,argv,opt);
    if (i == 0) return def;
    else if (++i < argc) return argv[i];
    else {
        fprintf(stderr,"arg error after %s\n", opt);
        exit(10);
    }
    return NULL;
}

int wf_getintarg(int argc, char *argv[], char *opt, int def)
{
    int i = wf_findarg(argc,argv,opt);
    if (i == 0) return def;
    else if (++i < argc) return atoi(argv[i]);
    else {
        fprintf(stderr,"arg error after %s\n", opt);
        exit(10);
    }
    return 0;
}

bool wf_getboolarg(int argc, char *argv[], char *opt)
{
    int i = wf_findarg(argc,argv,opt);
    if (i == 0) return false;
    else        return true;
}

//--------------------------------------------------------------------
//  wf_getstrargs()
//
//  Sucht das uebergebene Keyword und returniert im uebergebenen
//  Buffer alle Args, die nach dem Keyword kommen, bis zum
//  letzten Arg, oder bis ein Arg kommt, welches mit '-'
//  anfaengt.
//
//  17-Aug-99   floh    created
//--------------------------------------------------------------------
char *wf_getstrargs(int argc, char *argv[], char *opt, char *buf, int buf_size)
{
    buf[0] = 0;
    int i = wf_findarg(argc,argv,opt); 
    int l = 1;
    if (i > 0) {
        for (++i; i<argc; i++) {
            // beim naechsten Arg, welches mit '-' anfaengt,
            // abbrechen
            if (argv[i][0] == '-') break;
            l += strlen(argv[i]) + 1;
            if (l < buf_size) {
                strcat(buf,argv[i]);
                strcat(buf," ");
            }
        }
    }
    return buf;
}                

//--------------------------------------------------------------------
//  wf_openfiles()
//  Check command line if in/out files are given, if not,
//  use stdin/stdout.
//  24-Oct-00   floh    created
//--------------------------------------------------------------------
bool wf_openfiles(int argc, char *argv[], FILE *& in, FILE *& out)
{
    char *in_str = wf_getstrarg(argc, argv, "-in", NULL);
    char *out_str = wf_getstrarg(argc, argv, "-out", NULL);

    if (in_str) {
        in = fopen(in_str,"r");
        if (!in) {
            fprintf(stderr, "Could not open '%s' for reading!\n",in_str);
            in = NULL;
            out = NULL;
            return false;
        }
    } else {
        in = stdin;
    }
    if (out_str) {
        out = fopen(out_str,"w");
        if (!out) {
            fprintf(stderr, "Could not open '%s' for writing!\n",out_str);
            in = NULL;
            out = NULL;
            return false;
        }
    } else {
        out = stdout;
    }
    return true;
}

//--------------------------------------------------------------------
//  wf_closefiles()
//  Close file handles opened by wf_openfiles()
//  24-Oct-00   floh    created
//--------------------------------------------------------------------
void wf_closefiles(FILE *in, FILE *out)
{
    if (in) {
        if (in != stdin) fclose(in);
    }
    if (out) {
        if (out != stdout) fclose(out);
    }
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
