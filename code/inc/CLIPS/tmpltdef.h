   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.23  01/31/05            */
   /*                                                     */
   /*               DEFTEMPLATE HEADER FILE               */
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
/*      6.23: Added support for templates maintaining their  */
/*            own list of facts.                             */
/*                                                           */
/*************************************************************/

#ifndef _H_tmpltdef
#define _H_tmpltdef

struct deftemplate;
struct templateSlot;
struct deftemplateModule;

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
#ifndef _H_constrnt
#include "constrnt.h"
#endif
#include "factbld.h"
#ifndef _H_factmngr
#include "factmngr.h"
#endif
#ifndef _H_cstrccom
#include "cstrccom.h"
#endif

struct deftemplate
  {
   struct constructHeader header;
   struct templateSlot *slotList;
   unsigned int implied       : 1;
   unsigned int watch         : 1;
   unsigned int inScope       : 1;
   unsigned short numberOfSlots;
   long busyCount;
   struct factPatternNode *patternNetwork;
   struct fact *factList;
   struct fact *lastFact;
  };

struct templateSlot
  {
   struct symbolHashNode *slotName;
   unsigned int multislot : 1;
   unsigned int noDefault : 1;
   unsigned int defaultPresent : 1;
   unsigned int defaultDynamic : 1;
   CONSTRAINT_RECORD *constraints;
   struct expr *defaultList;
   struct templateSlot *next;
  };

struct deftemplateModule
  {
   struct defmoduleItemHeader header;
  };

#define DEFTEMPLATE_DATA 5

struct deftemplateData
  { 
   struct construct *DeftemplateConstruct;
   int DeftemplateModuleIndex;
   struct entityRecord DeftemplatePtrRecord;
#if DEBUGGING_FUNCTIONS
   int DeletedTemplateDebugFlags;
#endif
#if CONSTRUCT_COMPILER && (! RUN_TIME)
   struct CodeGeneratorItem *DeftemplateCodeItem;
#endif
#if (! RUN_TIME) && (! BLOAD_ONLY)
   int DeftemplateError;
#endif
  };

#define EnvGetDeftemplateName(theEnv,x) GetConstructNameString((struct constructHeader *) x)
#define EnvGetDeftemplatePPForm(theEnv,x) GetConstructPPForm(theEnv,(struct constructHeader *) x)
#define EnvDeftemplateModule(theEnv,x) GetConstructModuleName((struct constructHeader *) x)
#define DeftemplateData(theEnv) ((struct deftemplateData *) GetEnvironmentData(theEnv,DEFTEMPLATE_DATA))

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _TMPLTDEF_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#if ENVIRONMENT_API_ONLY
#define FindDeftemplate(theEnv,a) EnvFindDeftemplate(theEnv,a)
#define GetNextDeftemplate(theEnv,a) EnvGetNextDeftemplate(theEnv,a)
#define IsDeftemplateDeletable(theEnv,a) EnvIsDeftemplateDeletable(theEnv,a)
#define GetDeftemplateName(theEnv,x) GetConstructNameString((struct constructHeader *) x)
#define GetDeftemplatePPForm(theEnv,x) GetConstructPPForm(theEnv,(struct constructHeader *) x)
#define GetNextFactInTemplate(theEnv,a,b) EnvGetNextFactInTemplate(theEnv,a,b)
#define DeftemplateModule(theEnv,x) GetConstructModuleName((struct constructHeader *) x)
#else
#define FindDeftemplate(a) EnvFindDeftemplate(GetCurrentEnvironment(),a)
#define GetNextDeftemplate(a) EnvGetNextDeftemplate(GetCurrentEnvironment(),a)
#define IsDeftemplateDeletable(a) EnvIsDeftemplateDeletable(GetCurrentEnvironment(),a)
#define GetDeftemplateName(x) GetConstructNameString((struct constructHeader *) x)
#define GetDeftemplatePPForm(x) GetConstructPPForm(GetCurrentEnvironment(),(struct constructHeader *) x)
#define GetNextFactInTemplate(a,b) EnvGetNextFactInTemplate(GetCurrentEnvironment(),a,b)
#define DeftemplateModule(x) GetConstructModuleName((struct constructHeader *) x)
#endif

   LOCALE void                           InitializeDeftemplates(void *);
   LOCALE void                          *EnvFindDeftemplate(void *,char *);
   LOCALE void                          *EnvGetNextDeftemplate(void *,void *);
   LOCALE BOOLEAN                        EnvIsDeftemplateDeletable(void *,void *);
   LOCALE void                          *EnvGetNextFactInTemplate(void *,void *,void *);
   LOCALE struct deftemplateModule      *GetDeftemplateModuleItem(void *,struct defmodule *);
   LOCALE void                           ReturnSlots(void *,struct templateSlot *);
   LOCALE void                           IncrementDeftemplateBusyCount(void *,void *);
   LOCALE void                           DecrementDeftemplateBusyCount(void *,void *);

#endif


