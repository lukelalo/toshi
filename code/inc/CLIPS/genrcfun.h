   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*               CLIPS Version 6.20  01/31/02          */
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
/*                                                           */
/*************************************************************/

#ifndef _H_genrcfun
#define _H_genrcfun

typedef struct defgenericModule DEFGENERIC_MODULE;
typedef struct restriction RESTRICTION;
typedef struct method DEFMETHOD;
typedef struct defgeneric DEFGENERIC;

#ifndef _STDIO_INCLUDED_
#define _STDIO_INCLUDED_
#include <stdio.h>
#endif

#ifndef _H_conscomp
#include "conscomp.h"
#endif
#ifndef _H_constrct
#include "constrct.h"
#endif
#ifndef _H_moduldef
#include "moduldef.h"
#endif
#ifndef _H_symbol
#include "symbol.h"
#endif
#ifndef _H_expressn
#include "expressn.h"
#endif
#ifndef _H_evaluatn
#include "evaluatn.h"
#endif

#if OBJECT_SYSTEM
#ifndef _H_object
#include "object.h"
#endif
#endif

struct defgenericModule
  {
   struct defmoduleItemHeader header;
  };

struct restriction
  {
   void **types;
   EXPRESSION *query;
   unsigned tcnt;
  };

struct method
  {
   unsigned index,busy;
   int restrictionCount,
       minRestrictions,maxRestrictions,
       localVarCount;
   unsigned system : 1;
   unsigned trace : 1;
   RESTRICTION *restrictions;
   EXPRESSION *actions;
   char *ppForm;
   struct userData *usrData;
  };

struct defgeneric
  {
   struct constructHeader header;
   unsigned busy,trace;
   DEFMETHOD *methods;
   unsigned mcnt,new_index;
  };

#define DEFGENERIC_DATA 27

struct defgenericData
  { 
   struct construct *DefgenericConstruct;
   int DefgenericModuleIndex;
   ENTITY_RECORD GenericEntityRecord;
#if DEBUGGING_FUNCTIONS
   unsigned WatchGenerics;
   unsigned WatchMethods;
#endif
   DEFGENERIC *CurrentGeneric;
   DEFMETHOD *CurrentMethod;
   DATA_OBJECT *GenericCurrentArgument;
#if (! RUN_TIME) && (! BLOAD_ONLY)
   unsigned OldGenericBusySave;
#endif
#if CONSTRUCT_COMPILER && (! RUN_TIME)
   struct CodeGeneratorItem *DefgenericCodeItem;
#endif
#if (! BLOAD_ONLY) && (! RUN_TIME)
   struct token GenericInputToken;
#endif
  };

#define DefgenericData(theEnv) ((struct defgenericData *) GetEnvironmentData(theEnv,DEFGENERIC_DATA))
#define SaveBusyCount(gfunc)    (DefgenericData(theEnv)->OldGenericBusySave = gfunc->busy)
#define RestoreBusyCount(gfunc) (gfunc->busy = DefgenericData(theEnv)->OldGenericBusySave)

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _GENRCFUN_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#if ! RUN_TIME
LOCALE BOOLEAN ClearDefgenericsReady(void *);
LOCALE void *AllocateDefgenericModule(void *);
LOCALE void FreeDefgenericModule(void *,void *);
#endif

#if (! BLOAD_ONLY) && (! RUN_TIME)

LOCALE int ClearDefmethods(void *);
LOCALE int RemoveAllExplicitMethods(void *,DEFGENERIC *);
LOCALE void RemoveDefgeneric(void *,void *);
LOCALE int ClearDefgenerics(void *);
LOCALE void MethodAlterError(void *,DEFGENERIC *);
LOCALE void DeleteMethodInfo(void *,DEFGENERIC *,DEFMETHOD *);
LOCALE void DestroyMethodInfo(void *,DEFGENERIC *,DEFMETHOD *);
LOCALE int MethodsExecuting(DEFGENERIC *);
#endif
#if ! OBJECT_SYSTEM
LOCALE BOOLEAN SubsumeType(int,int);
#endif

LOCALE int FindMethodByIndex(DEFGENERIC *,unsigned);
#if DEBUGGING_FUNCTIONS
LOCALE void PreviewGeneric(void *);
LOCALE void PrintMethod(void *,char *,int,DEFMETHOD *);
#endif
LOCALE DEFGENERIC *CheckGenericExists(void *,char *,char *);
LOCALE int CheckMethodExists(void *,char *,DEFGENERIC *,int);

#if ! OBJECT_SYSTEM
LOCALE char *TypeName(void *,int);
#endif

LOCALE void PrintGenericName(void *,char *,DEFGENERIC *);

#endif





