   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.20  01/31/02            */
   /*                                                     */
   /*       PROCEDURAL FUNCTIONS PARSER HEADER FILE       */
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

#ifndef _H_prcdrpsr

#define _H_prcdrpsr

#ifndef _H_constrnt
#include "constrnt.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _PRCDRPSR_SOURCE
#define LOCALE
#else
#define LOCALE extern
#endif

struct BindInfo
  {
   struct symbolHashNode *name;
   CONSTRAINT_RECORD *constraints;
   struct BindInfo *next;
  };

#if (! RUN_TIME)
   LOCALE void                           ProceduralFunctionParsers(void *);
   LOCALE struct BindInfo               *GetParsedBindNames(void *);
   LOCALE void                           SetParsedBindNames(void *,struct BindInfo *);
   LOCALE void                           ClearParsedBindNames(void *);
   LOCALE BOOLEAN                        ParsedBindNamesEmpty(void *);
#endif
#if (! BLOAD_ONLY) && (! RUN_TIME)
   LOCALE int                            SearchParsedBindNames(void *,struct symbolHashNode *);
   LOCALE int                            CountParsedBindNames(void *);
   LOCALE void                           RemoveParsedBindName(void *,struct symbolHashNode *);
   LOCALE struct constraintRecord       *FindBindConstraints(void *,struct symbolHashNode *);
#endif

#endif




