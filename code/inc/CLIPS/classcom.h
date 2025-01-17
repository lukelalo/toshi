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

#ifndef _H_classcom
#define _H_classcom

#define CONVENIENCE_MODE  0
#define CONSERVATION_MODE 1

#define EnvGetDefclassName(theEnv,x) GetConstructNameString((struct constructHeader *) x)
#define EnvGetDefclassPPForm(theEnv,x) GetConstructPPForm(theEnv,(struct constructHeader *) x)

#define GetDefclassNamePointer(x) GetConstructNamePointer((struct constructHeader *) x)
#define GetDefclassModule(x) GetConstructModuleItem((struct constructHeader *) x)

#define SetNextDefclass(c,t) SetNextConstruct((struct constructHeader *) c, \
                                              (struct constructHeader *) t)

#define SetDefclassPPForm(c,ppf) SetConstructPPForm(theEnv,(struct constructHeader *) c,ppf)

#define EnvDefclassModule(theEnv,x) GetConstructModuleName((struct constructHeader *) x)

#ifndef _H_cstrccom
#include "cstrccom.h"
#endif
#ifndef _H_moduldef
#include "moduldef.h"
#endif
#ifndef _H_object
#include "object.h"
#endif
#ifndef _H_symbol
#include "symbol.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _CLASSCOM_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#if ENVIRONMENT_API_ONLY
#define DefclassModule(theEnv,x) GetConstructModuleName((struct constructHeader *) x)
#define FindDefclass(theEnv,a) EnvFindDefclass(theEnv,a)
#define GetDefclassList(theEnv,a,b) EnvGetDefclassList(theEnv,a,b)
#define GetDefclassName(theEnv,x) GetConstructNameString((struct constructHeader *) x)
#define GetDefclassPPForm(theEnv,x) GetConstructPPForm(theEnv,(struct constructHeader *) x)
#define GetDefclassWatchInstances(theEnv,a) EnvGetDefclassWatchInstances(theEnv,a)
#define GetDefclassWatchSlots(theEnv,a) EnvGetDefclassWatchSlots(theEnv,a)
#define GetNextDefclass(theEnv,a) EnvGetNextDefclass(theEnv,a)
#define IsDefclassDeletable(theEnv,a) EnvIsDefclassDeletable(theEnv,a)
#define ListDefclasses(theEnv,a,b) EnvListDefclasses(theEnv,a,b)
#define SetDefclassWatchInstances(theEnv,a,b) EnvSetDefclassWatchInstances(theEnv,a,b)
#define SetDefclassWatchSlots(theEnv,a,b) EnvSetDefclassWatchSlots(theEnv,a,b)
#define Undefclass(theEnv,a) EnvUndefclass(theEnv,a)
#define SetClassDefaultsMode(theEnv,a) EnvSetClassDefaultsMode(theEnv,a)
#define GetClassDefaultsMode(theEnv) EnvGetClassDefaultsMode(theEnv)
#else
#define DefclassModule(x) GetConstructModuleName((struct constructHeader *) x)
#define FindDefclass(a) EnvFindDefclass(GetCurrentEnvironment(),a)
#define GetDefclassList(a,b) EnvGetDefclassList(GetCurrentEnvironment(),a,b)
#define GetDefclassName(x) GetConstructNameString((struct constructHeader *) x)
#define GetDefclassPPForm(x) GetConstructPPForm(GetCurrentEnvironment(),(struct constructHeader *) x)
#define GetDefclassWatchInstances(a) EnvGetDefclassWatchInstances(GetCurrentEnvironment(),a)
#define GetDefclassWatchSlots(a) EnvGetDefclassWatchSlots(GetCurrentEnvironment(),a)
#define GetNextDefclass(a) EnvGetNextDefclass(GetCurrentEnvironment(),a)
#define IsDefclassDeletable(a) EnvIsDefclassDeletable(GetCurrentEnvironment(),a)
#define ListDefclasses(a,b) EnvListDefclasses(GetCurrentEnvironment(),a,b)
#define SetDefclassWatchInstances(a,b) EnvSetDefclassWatchInstances(GetCurrentEnvironment(),a,b)
#define SetDefclassWatchSlots(a,b) EnvSetDefclassWatchSlots(GetCurrentEnvironment(),a,b)
#define Undefclass(a) EnvUndefclass(GetCurrentEnvironment(),a)
#define SetClassDefaultsMode(a) EnvSetClassDefaultsMode(GetCurrentEnvironment(),a)
#define GetClassDefaultsMode() EnvGetClassDefaultsMode(GetCurrentEnvironment())
#endif

LOCALE void *EnvFindDefclass(void *,char *);
LOCALE DEFCLASS *LookupDefclassByMdlOrScope(void *,char *);
LOCALE DEFCLASS *LookupDefclassInScope(void *,char *);
LOCALE DEFCLASS *LookupDefclassAnywhere(void *,struct defmodule *,char *);
LOCALE BOOLEAN DefclassInScope(void *,DEFCLASS *,struct defmodule *);
LOCALE void *EnvGetNextDefclass(void *,void *);
LOCALE BOOLEAN EnvIsDefclassDeletable(void *,void *);

LOCALE void UndefclassCommand(void *);
LOCALE unsigned short EnvSetClassDefaultsMode(void *,unsigned short);
LOCALE unsigned short EnvGetClassDefaultsMode(void *);
LOCALE void *GetClassDefaultsModeCommand(void *);
LOCALE void *SetClassDefaultsModeCommand(void *);

#if DEBUGGING_FUNCTIONS
LOCALE void PPDefclassCommand(void *);
LOCALE void ListDefclassesCommand(void *);
LOCALE void EnvListDefclasses(void *,char *,struct defmodule *);
LOCALE unsigned EnvGetDefclassWatchInstances(void *,void *);
LOCALE void EnvSetDefclassWatchInstances(void *,unsigned,void *);
LOCALE unsigned EnvGetDefclassWatchSlots(void *,void *);
LOCALE void EnvSetDefclassWatchSlots(void *,unsigned,void *);
LOCALE unsigned DefclassWatchAccess(void *,int,unsigned,EXPRESSION *);
LOCALE unsigned DefclassWatchPrint(void *,char *,int,EXPRESSION *);
#endif

LOCALE void GetDefclassListFunction(void *,DATA_OBJECT *);
LOCALE void EnvGetDefclassList(void *,DATA_OBJECT *,struct defmodule *);
LOCALE BOOLEAN EnvUndefclass(void *,void *);
LOCALE BOOLEAN HasSuperclass(DEFCLASS *,DEFCLASS *);

LOCALE SYMBOL_HN *CheckClassAndSlot(void *,char *,DEFCLASS **);

#if (! BLOAD_ONLY) && (! RUN_TIME)
LOCALE void SaveDefclasses(void *,void *,char *);
#endif

#endif




