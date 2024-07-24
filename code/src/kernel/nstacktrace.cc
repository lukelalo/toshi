//------------------------------------------------------------------------------
/* Copyright (c) 2001-2002 Bruce Mitchener, Jr.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------
/*
   This #ifdef __LINUX__ block *MUST* be first and before
   any other includes, otherwise, Linux build breaks.
*/
#ifdef __LINUX__
#define HAVE_BACKTRACE 1
#define HAVE_DLADDR 1
#define HAVE_DLFCN_H 1
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

// Source files in the kernel should define N_KERNEL
// so that the function EXPORT/IMPORT works on win32
#define N_KERNEL
#include "kernel/ntypes.h"
#include <stdio.h>

#ifdef __WIN32__
#include <malloc.h>
#endif

static void
n_symbol_info (void *address,
               char **file_name,
               void **base_address,
               char **symbol_name,
               void **symbol_address,
               void **offset);
static int
n_crawl_stack(void ***ptrarray, int length);

#if HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#if HAVE_BACKTRACE 
#include <execinfo.h>
#endif

static int
n_crawl_stack(void ***ptrarray, int length)
{
#if HAVE_BACKTRACE
    return backtrace(*ptrarray, length);
#else
    return 0;
#endif
}

#if HAVE_DLADDR
static void
n_symbol_info (void *address,
               char **file_name,
               void **base_address,
               char **symbol_name,
               void **symbol_address,
               void **offset)
{
    Dl_info sym_info;
    int ret;

    ret = dladdr(address, &sym_info);
    if (ret)
    {     
        *file_name = (char*)sym_info.dli_fname;
        *base_address = sym_info.dli_fbase;                               
        *symbol_name = (char*)sym_info.dli_sname;
        *symbol_address = sym_info.dli_saddr;                               
        *offset = (void*)((long)address - (long)*symbol_address);
    }
    else
    {
        *file_name = "????";
        *base_address = NULL;
        *symbol_name = "????";
        *symbol_address = NULL;
        *offset = NULL;
    }
}
#else /* HAVE_DLADDR */
static void
n_symbol_info (void *address,
               char **file_name,
               void **base_address,
               char **symbol_name,
               void **symbol_address,
               void **offset)
{
    *file_name = "????";
    *base_address = NULL;
    *symbol_name = "????";
    *symbol_address = NULL;
    *offset = NULL;
}
#endif /* HAVE_DLADDR */

N_EXPORT void n_print_stacktrace(char* prefix)
{
    void **trace, *address, *base_address, *symbol_address, *offset;
    char *file_name, *symbol_name;
    int count, i;


    trace = (void**)alloca(sizeof(void*) * 100);
    count = n_crawl_stack((void***)&trace, 100);

    for (i = 0; i < count; i++)
    {
        address = trace[i];
        n_symbol_info(address, &file_name, &base_address, &symbol_name,
                       &symbol_address, &offset);

        fprintf(stderr, "%s %p %s+%p (%s)\n", prefix, base_address, 
                symbol_name, offset, file_name);
    }
}

