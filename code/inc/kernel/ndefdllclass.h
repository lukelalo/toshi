//-------------------------------------------------------------------
//	OVERVIEW
//  DLL export/import related stuff.
//
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------

#ifndef N_SYSTEM_H
#include "kernel/nsystem.h"
#endif

#undef N_DLLCLASS
#ifdef __VC__
#   if defined(N_IMPLEMENTS) && defined(N_DEFINES) && (N_IMPLEMENTS==N_DEFINES)
#       define N_DLLCLASS
#       define N_PUBLIC __declspec(dllexport)
#   else
#       define N_DLLCLASS
#       define N_PUBLIC __declspec(dllimport)
#   endif
#else
#   define N_DLLCLASS
#   define N_PUBLIC
#endif

#ifdef N_INIT
#   define n_init N_INIT
#endif
#ifdef N_FINI
#   define n_fini N_FINI
#endif
#ifdef N_NEW
#   define n_create N_NEW
#endif
#ifdef N_VERSION
#   define n_version N_VERSION
#endif
#ifdef N_INITCMDS
#   define n_initcmds N_INITCMDS
#endif
//-------------------------------------------------------------------

