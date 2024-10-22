   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.20  01/31/02            */
   /*                                                     */
   /*                DEFFACTS HEADER FILE                 */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Donnell                                     */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_dffctdef
#define _H_dffctdef

#ifndef _H_conscomp
#include "conscomp.h"
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
#ifndef _H_constrct
#include "constrct.h"
#endif
#ifndef _H_moduldef
#include "moduldef.h"
#endif
#ifndef _H_cstrccom
#include "cstrccom.h"
#endif

#define DEFFACTS_DATA 0

struct deffactsData
  { 
   struct construct *DeffactsConstruct;
   int DeffactsModuleIndex;  
#if CONSTRUCT_COMPILER && (! RUN_TIME)
   struct CodeGeneratorItem *DeffactsCodeItem;
#endif
  };
  
struct deffacts
  {
   struct constructHeader header;
   struct expr *assertList;
  };

struct deffactsModule
  {
   struct defmoduleItemHeader header;
  };

#define EnvGetDeffactsName(theEnv,x) GetConstructNameString((struct constructHeader *) x)
#define EnvGetDeffactsPPForm(theEnv,x) GetConstructPPForm(theEnv,(struct constructHeader *) x)
#define EnvDeffactsModule(theEnv,x) GetConstructModuleName((struct constructHeader *) x)
#define DeffactsData(theEnv) ((struct deffactsData *) GetEnvironmentData(theEnv,DEFFACTS_DATA))

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _DFFCTDEF_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#if ENVIRONMENT_API_ONLY
#define DeffactsModule(theEnv,x) GetConstructModuleName((struct constructHeader *) x)
#define FindDeffacts(theEnv,a) EnvFindDeffacts(theEnv,a)
#define GetDeffactsName(theEnv,x) GetConstructNameString((struct constructHeader *) x)
#define GetDeffactsPPForm(theEnv,x) GetConstructPPForm(theEnv,(struct constructHeader *) x)
#define GetNextDeffacts(theEnv,a) EnvGetNextDeffacts(theEnv,a)
#define IsDeffactsDeletable(theEnv,a) EnvIsDeffactsDeletable(theEnv,a)
#else
#define DeffactsModule(x) GetConstructModuleName((struct constructHeader *) x)
#define FindDeffacts(a) EnvFindDeffacts(GetCurrentEnvironment(),a)
#define GetDeffactsName(x) GetConstructNameString((struct constructHeader *) x)
#define GetDeffactsPPForm(x) GetConstructPPForm(GetCurrentEnvironment(),(struct constructHeader *) x)
#define GetNextDeffacts(a) EnvGetNextDeffacts(GetCurrentEnvironment(),a)
#define IsDeffactsDeletable(a) EnvIsDeffactsDeletable(GetCurrentEnvironment(),a)
#endif

   LOCALE void                           InitializeDeffacts(void *);
   LOCALE void                          *EnvFindDeffacts(void *,char *);
   LOCALE void                          *EnvGetNextDeffacts(void *,void *);
   LOCALE void                           CreateInitialFactDeffacts(void);
   LOCALE BOOLEAN                        EnvIsDeffactsDeletable(void *,void *);
   LOCALE struct deffactsModule         *GetDeffactsModuleItem(void *,struct defmodule *);

#endif


