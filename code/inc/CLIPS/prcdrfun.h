   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.20  01/31/02            */
   /*                                                     */
   /*          PROCEDURAL FUNCTIONS HEADER FILE           */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*      Brian L. Donnell                                     */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_prcdrfun

#define _H_prcdrfun

#ifndef _H_evaluatn
#include "evaluatn.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _PRCDRFUN_SOURCE
#define LOCALE
#else
#define LOCALE extern
#endif

typedef struct loopCounterStack
  {
   long loopCounter;
   struct loopCounterStack *nxt;
  } LOOP_COUNTER_STACK;

#define PRCDRFUN_DATA 13

struct procedureFunctionData
  { 
   int ReturnFlag;
   int BreakFlag;
   LOOP_COUNTER_STACK *LoopCounterStack;
   struct dataObject *BindList;
  };

#define ProcedureFunctionData(theEnv) ((struct procedureFunctionData *) GetEnvironmentData(theEnv,PRCDRFUN_DATA))

   LOCALE void                           ProceduralFunctionDefinitions(void *);
   LOCALE void                           WhileFunction(void *,DATA_OBJECT_PTR);
   LOCALE void                           LoopForCountFunction(void *,DATA_OBJECT_PTR);
   LOCALE long                           GetLoopCount(void *);
   LOCALE void                           IfFunction(void *,DATA_OBJECT_PTR);
   LOCALE void                           BindFunction(void *,DATA_OBJECT_PTR);
   LOCALE void                           PrognFunction(void *,DATA_OBJECT_PTR);
   LOCALE void                           ReturnFunction(void *,DATA_OBJECT_PTR);
   LOCALE void                           BreakFunction(void *);
   LOCALE void                           SwitchFunction(void *,DATA_OBJECT_PTR);
   LOCALE BOOLEAN                        GetBoundVariable(void *,struct dataObject *,struct symbolHashNode *);
   LOCALE void                           FlushBindList(void *);

#endif






