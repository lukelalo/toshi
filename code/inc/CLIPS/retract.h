   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.20  01/31/02            */
   /*                                                     */
   /*                RETRACT HEADER FILE                  */
   /*******************************************************/

/*************************************************************/
/* Purpose:  Handles join network activity associated with   */
/*   with the removal of a data entity such as a fact or     */
/*   instance.                                               */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_retract
#define _H_retract

#ifndef _H_match
#include "match.h"
#endif
#ifndef _H_network
#include "network.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _RETRACT_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

struct rdriveinfo
  {
   struct partialMatch *link;
   struct joinNode *jlist;
   struct rdriveinfo *next;
  };

LOCALE void                           NetworkRetract(void *,struct patternMatch *);
LOCALE void                           PosEntryRetract(void *,struct joinNode *,struct alphaMatch *,struct partialMatch *,int,int);
LOCALE void                           ReturnPartialMatch(void *,struct partialMatch *);
LOCALE void                           DestroyPartialMatch(void *,struct partialMatch *);
LOCALE void                           FlushGarbagePartialMatches(void *);
LOCALE void                           NegEntryRetract(void *,struct joinNode *,struct partialMatch *,int);
LOCALE void                           RetractCheckDriveRetractions(void *,struct alphaMatch *,int); 

#endif



