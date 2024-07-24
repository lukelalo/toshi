#ifndef N_DEBUG_H
#define N_DEBUG_H
/*!
  \file
*/
//--------------------------------------------------------------------
//  OVERVIEW
//  n_assert()  - the vanilla assert() Macro
//  n_error()   - print message with source and line number, then abort
//  n_warn()    - print message with source and line number, then continue
//
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>

/*! \def n_assert(exp)
    \brief The vanilla assert() Macro
*/
/*! \def n_error(x)
    \brief Print message with source and line number, then abort
*/
/*! \def n_warn(x)
    \brief Print message with source and line number, then continue
*/

#ifdef __NEBULA_NO_ASSERT__
#define n_assert(exp)
#else
#define n_assert(exp) { if (!(exp)) n_barf(#exp,__FILE__,__LINE__); }
#endif

#define n_warn(x) do { n_printf("WARN %s/%d: %s\n", __FILE__, __LINE__, x); } while(0)
//--------------------------------------------------------------------
#endif
