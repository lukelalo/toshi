   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.20  01/31/02            */
   /*                                                     */
   /*                 ROUTER HEADER FILE                  */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides a centralized mechanism for handling    */
/*   input and output requests.                              */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_router
#define _H_router

#ifndef _H_prntutil
#include "prntutil.h"
#endif

#ifndef _STDIO_INCLUDED_
#define _STDIO_INCLUDED_
#include <stdio.h>
#endif

#define WWARNING "wwarning"
#define WERROR "werror"
#define WTRACE "wtrace"
#define WDIALOG "wdialog"
#define WPROMPT  WPROMPT_STRING
#define WDISPLAY "wdisplay"

#define ROUTER_DATA 46

struct router
  {
   char *name;
   int active;
   int priority;
   short int environmentAware;
   int (*query)(void *,char *);
   int (*printer)(void *,char *,char *);
   int (*exiter)(void *,int);
   int (*charget)(void *,char *);
   int (*charunget)(void *,int,char *);
   struct router *next;
  };

struct routerData
  { 
   int CommandBufferInputCount;
   char *LineCountRouter;
   char *FastCharGetRouter;
   char *FastCharGetString;
   long FastCharGetIndex;
   struct router *ListOfRouters;
   FILE *FastLoadFilePtr;
   FILE *FastSaveFilePtr;
   int Abort;
  };

#define RouterData(theEnv) ((struct routerData *) GetEnvironmentData(theEnv,ROUTER_DATA))

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _ROUTER_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#if ENVIRONMENT_API_ONLY
#define ExitRouter(theEnv,a) EnvExitRouter(theEnv,a)
#define GetcRouter(theEnv,a) EnvGetcRouter(theEnv,a)
#define PrintRouter(theEnv,a,b) EnvPrintRouter(theEnv,a,b)
#define UngetcRouter(theEnv,a,b) EnvUngetcRouter(theEnv,a,b)
#define ActivateRouter(theEnv,a) EnvActivateRouter(theEnv,a)
#define DeactivateRouter(theEnv,a) EnvDeactivateRouter(theEnv,a)
#define DeleteRouter(theEnv,a) EnvDeleteRouter(theEnv,a)
#else
#define ExitRouter(a) EnvExitRouter(GetCurrentEnvironment(),a)
#define GetcRouter(a) EnvGetcRouter(GetCurrentEnvironment(),a)
#define PrintRouter(a,b) EnvPrintRouter(GetCurrentEnvironment(),a,b)
#define UngetcRouter(a,b) EnvUngetcRouter(GetCurrentEnvironment(),a,b)
#define ActivateRouter(a) EnvActivateRouter(GetCurrentEnvironment(),a)
#define DeactivateRouter(a) EnvDeactivateRouter(GetCurrentEnvironment(),a)
#define DeleteRouter(a) EnvDeleteRouter(GetCurrentEnvironment(),a)
#endif

   LOCALE void                           InitializeDefaultRouters(void *);
   LOCALE int                            EnvPrintRouter(void *,char *,char *);
   LOCALE int                            EnvGetcRouter(void *,char *);
   LOCALE int                            EnvUngetcRouter(void *,int,char *);
   LOCALE void                           EnvExitRouter(void *,int);
   LOCALE void                           AbortExit(void *);
   LOCALE BOOLEAN                        EnvAddRouter(void *,
                                                   char *,int,
                                                   int (*)(void *,char *),
                                                   int (*)(void *,char *,char *),
                                                   int (*)(void *,char *),
                                                   int (*)(void *,int,char *),
                                                   int (*)(void *,int));
   LOCALE BOOLEAN                        AddRouter(char *,int,
                                                   int (*)(char *),
                                                   int (*)(char *,char *),
                                                   int (*)(char *),
                                                   int (*)(int,char *),
                                                   int (*)(int));
   LOCALE int                            EnvDeleteRouter(void *,char *);
   LOCALE int                            QueryRouters(void *,char *);
   LOCALE int                            EnvDeactivateRouter(void *,char *);
   LOCALE int                            EnvActivateRouter(void *,char *);
   LOCALE void                           SetFastLoad(void *,FILE *);
   LOCALE void                           SetFastSave(void *,FILE *);
   LOCALE FILE                          *GetFastLoad(void *);
   LOCALE FILE                          *GetFastSave(void *);
   LOCALE void                           UnrecognizedRouterMessage(void *,char *);
   LOCALE void                           ExitCommand(void *);
   LOCALE int                            PrintNRouter(void *,char *,char *,unsigned long);

#endif


