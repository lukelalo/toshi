#ifndef N_WFTOOLS_H
#define N_WFTOOLS_H
//--------------------------------------------------------------------
//  OVERVIEW
//      Prototype-Defs fuer src/tools/wftools.cc
//
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

//--------------------------------------------------------------------
extern float wf_getfloatarg(int argc, char *argv[], char *opt, float def);
extern char *wf_getstrarg(int argc, char *argv[], char *opt, char *def);
extern int wf_getintarg(int argc, char *argv[], char *opt, int def);
extern bool wf_getboolarg(int argc, char *argv[], char *opt);
extern char *wf_getstrargs(int argc, char *argv[], char *opt, char *buf, int buf_size);
extern bool wf_openfiles(int argc, char *argv[], FILE *& in, FILE *& out);
extern void wf_closefiles(FILE *in, FILE *out);
//--------------------------------------------------------------------
#endif



