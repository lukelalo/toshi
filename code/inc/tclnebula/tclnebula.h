#ifndef N_TCLNEBULA_H
#define N_TCLNEBULA_H
//-------------------------------------------------------------------
//  tclnebula.h
//
//  Header file for the Tcl extension dll, which makes Nebula
//  a proper Tcl extension.
//
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#include "tcl.h"

//-------------------------------------------------------------------
//  Code taken from:
// 
//  A sample dynamically loadable extension for Tcl 8.0
//
//  by Scott Stanton w/ help from Ray Johnson
//  scott.stanton@eng.sun.com
//  ray.johnson@eng.sun.com
//-------------------------------------------------------------------

#if defined(__WIN32__)
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   undef WIN32_LEAN_AND_MEAN
#endif

extern "C" __declspec(dllexport) int tclnebula_Init(Tcl_Interp *interp);
extern "C" __declspec(dllexport) int tclnebula_SafeInit(Tcl_Interp *interp);
//-------------------------------------------------------------------
#endif
