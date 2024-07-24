#ifndef N_STACKTRACE_H
#define N_STACKTRACE_H
//------------------------------------------------------------------------------
/* Copyright (c) 2001-2002 Bruce Mitchener, Jr.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------

/**
   @brief This function will print the current stack trace. It is currently
   only functional on Linux.
   @ingroup NebulaKernelUtilities
  
   @param prefix      Prefix each line of the stack trace with the given string.

   @todo Port n_print_stacktrace() to Mac OS X and Windows.
*/
void n_print_stacktrace(char* prefix);
#endif
