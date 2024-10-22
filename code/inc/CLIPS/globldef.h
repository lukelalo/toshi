   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.21  06/15/03            */
   /*                                                     */
   /*                DEFGLOBAL HEADER FILE                */
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

#ifndef _H_globldef
#define _H_globldef

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

#define DEFGLOBAL_DATA 1

struct defglobalData
  { 
   struct construct *DefglobalConstruct;
   int DefglobalModuleIndex;  
   int ChangeToGlobals;   
   BOOLEAN ResetGlobals;
   struct entityRecord GlobalInfo;
   struct entityRecord DefglobalPtrRecord;
   long LastModuleIndex;
   struct defmodule *TheDefmodule;
#if CONSTRUCT_COMPILER && (! RUN_TIME)
   struct CodeGeneratorItem *DefglobalCodeItem;
#endif
  };

struct defglobal
  {
   struct constructHeader header;
   unsigned int watch   : 1;
   unsigned int inScope : 1;
   long busyCount;
   DATA_OBJECT current;
   struct expr *initial;
  };

struct defglobalModule
  {
   struct defmoduleItemHeader header;
  };

#define EnvGetDefglobalName(theEnv,x) GetConstructNameString((struct constructHeader *) x)
#define EnvGetDefglobalPPForm(theEnv,x) GetConstructPPForm(theEnv,(struct constructHeader *) x)
#define EnvDefglobalModule(theEnv,x) GetConstructModuleName((struct constructHeader *) x)

#define DefglobalData(theEnv) ((struct defglobalData *) GetEnvironmentData(theEnv,DEFGLOBAL_DATA))

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _GLOBLDEF_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#if ENVIRONMENT_API_ONLY
#define DefglobalModule(theEnv,x) GetConstructModuleName((struct constructHeader *) x)
#define FindDefglobal(theEnv,a) EnvFindDefglobal(theEnv,a)
#define GetDefglobalName(theEnv,x) GetConstructNameString((struct constructHeader *) x)
#define GetDefglobalPPForm(theEnv,x) GetConstructPPForm(theEnv,(struct constructHeader *) x)
#define GetDefglobalValue(theEnv,a,b) EnvGetDefglobalValue(theEnv,a,b)
#define GetDefglobalValueForm(theEnv,a,b,c) EnvGetDefglobalValueForm(theEnv,a,b,c)
#define GetGlobalsChanged(theEnv) EnvGetGlobalsChanged(theEnv)
#define GetNextDefglobal(theEnv,a) EnvGetNextDefglobal(theEnv,a)
#define IsDefglobalDeletable(theEnv,a) EnvIsDefglobalDeletable(theEnv,a)
#define SetDefglobalValue(theEnv,a,b) EnvSetDefglobalValue(theEnv,a,b)
#define SetGlobalsChanged(theEnv,a) EnvSetGlobalsChanged(theEnv,a)
#else
#define DefglobalModule(x) GetConstructModuleName((struct constructHeader *) x)
#define FindDefglobal(a) EnvFindDefglobal(GetCurrentEnvironment(),a)
#define GetDefglobalName(x) GetConstructNameString((struct constructHeader *) x)
#define GetDefglobalPPForm(x) GetConstructPPForm(GetCurrentEnvironment(),(struct constructHeader *) x)
#define GetDefglobalValue(a,b) EnvGetDefglobalValue(GetCurrentEnvironment(),a,b)
#define GetDefglobalValueForm(a,b,c) EnvGetDefglobalValueForm(GetCurrentEnvironment(),a,b,c)
#define GetGlobalsChanged() EnvGetGlobalsChanged(GetCurrentEnvironment())
#define GetNextDefglobal(a) EnvGetNextDefglobal(GetCurrentEnvironment(),a)
#define IsDefglobalDeletable(a) EnvIsDefglobalDeletable(GetCurrentEnvironment(),a)
#define SetDefglobalValue(a,b) EnvSetDefglobalValue(GetCurrentEnvironment(),a,b)
#define SetGlobalsChanged(a) EnvSetGlobalsChanged(GetCurrentEnvironment(),a)
#endif

   LOCALE void                           InitializeDefglobals(void *);
   LOCALE void                          *EnvFindDefglobal(void *,char *);
   LOCALE void                          *EnvGetNextDefglobal(void *,void *);
   LOCALE void                           CreateInitialFactDefglobal(void);
   LOCALE BOOLEAN                        EnvIsDefglobalDeletable(void *,void *);
   LOCALE struct defglobalModule        *GetDefglobalModuleItem(void *,struct defmodule *);
   LOCALE void                           QSetDefglobalValue(void *,struct defglobal *,DATA_OBJECT_PTR,int);
   LOCALE struct defglobal              *QFindDefglobal(void *,struct symbolHashNode *);
   LOCALE void                           EnvGetDefglobalValueForm(void *,char *,unsigned,void *);
   LOCALE int                            EnvGetGlobalsChanged(void *);
   LOCALE void                           EnvSetGlobalsChanged(void *,int);
   LOCALE BOOLEAN                        EnvGetDefglobalValue(void *,char *,DATA_OBJECT_PTR);
   LOCALE BOOLEAN                        EnvSetDefglobalValue(void *,char *,DATA_OBJECT_PTR);
   LOCALE void                           UpdateDefglobalScope(void *);
   LOCALE void                          *GetNextDefglobalInScope(void *,void *);
   LOCALE int                            QGetDefglobalValue(void *,void *,DATA_OBJECT_PTR);

#ifndef _GLOBLDEF_SOURCE_
#endif

#endif


