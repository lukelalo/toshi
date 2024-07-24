   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*               CLIPS Version 6.23  01/31/05          */
   /*                                                     */
   /*                                                     */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Brian L. Donnell                                     */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*      6.23: Corrected compilation errors for files         */
/*            generated by constructs-to-c. DR0861           */
/*                                                           */
/*************************************************************/

#ifndef _H_insquery
#define _H_insquery

#if INSTANCE_SET_QUERIES

#ifndef _H_object
#include "object.h"
#endif

typedef struct query_class
  {
   DEFCLASS *cls;
   struct defmodule *theModule;
   struct query_class *chain,*nxt;
  } QUERY_CLASS;

typedef struct query_soln
  {
   INSTANCE_TYPE **soln;
   struct query_soln *nxt;
  } QUERY_SOLN;

typedef struct query_core
  {
   INSTANCE_TYPE **solns;
   EXPRESSION *query,*action;
   QUERY_SOLN *soln_set,*soln_bottom;
   unsigned soln_size,soln_cnt;
   DATA_OBJECT *result;
  } QUERY_CORE;

typedef struct query_stack
  {
   QUERY_CORE *core;
   struct query_stack *nxt;
  } QUERY_STACK;

#define INSTANCE_QUERY_DATA 31

struct instanceQueryData
  { 
   SYMBOL_HN *QUERY_DELIMETER_SYMBOL;
   QUERY_CORE *QueryCore;
   QUERY_STACK *QueryCoreStack;
   int AbortQuery;
  };

#define InstanceQueryData(theEnv) ((struct instanceQueryData *) GetEnvironmentData(theEnv,INSTANCE_QUERY_DATA))


#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _INSQUERY_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#define QUERY_DELIMETER_STRING     "(QDS)"

LOCALE void SetupQuery(void *);
LOCALE void *GetQueryInstance(void *);
LOCALE void GetQueryInstanceSlot(void *,DATA_OBJECT *);
LOCALE BOOLEAN AnyInstances(void *);
LOCALE void QueryFindInstance(void *,DATA_OBJECT *);
LOCALE void QueryFindAllInstances(void *,DATA_OBJECT *);
LOCALE void QueryDoForInstance(void *,DATA_OBJECT *);
LOCALE void QueryDoForAllInstances(void *,DATA_OBJECT *);
LOCALE void DelayedQueryDoForAllInstances(void *,DATA_OBJECT *);

#endif

#endif





