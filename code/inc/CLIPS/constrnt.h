   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.20  01/31/02            */
   /*                                                     */
   /*                CONSTRAINT HEADER FILE               */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides functions for creating and removing     */
/*   constraint records, adding them to the contraint hash   */
/*   table, and enabling and disabling static and dynamic    */
/*   constraint checking.                                    */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_constrnt
#define _H_constrnt

struct constraintRecord;

#ifndef _H_evaluatn
#include "evaluatn.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _CONSTRNT_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

struct constraintRecord
  {
   unsigned int anyAllowed : 1;
   unsigned int symbolsAllowed : 1;
   unsigned int stringsAllowed : 1;
   unsigned int floatsAllowed : 1;
   unsigned int integersAllowed : 1;
   unsigned int instanceNamesAllowed : 1;
   unsigned int instanceAddressesAllowed : 1;
   unsigned int externalAddressesAllowed : 1;
   unsigned int factAddressesAllowed : 1;
   unsigned int voidAllowed : 1;
   unsigned int anyRestriction : 1;
   unsigned int symbolRestriction : 1;
   unsigned int stringRestriction : 1;
   unsigned int floatRestriction : 1;
   unsigned int integerRestriction : 1;
   unsigned int instanceNameRestriction : 1;
   unsigned int multifieldsAllowed : 1;
   unsigned int singlefieldsAllowed : 1;
   unsigned short bsaveIndex;
   struct expr *restrictionList;
   struct expr *minValue;
   struct expr *maxValue;
   struct expr *minFields;
   struct expr *maxFields;
   struct constraintRecord *multifield;
   struct constraintRecord *next;
   int bucket;
   int count;
  };

typedef struct constraintRecord CONSTRAINT_RECORD;

#define SIZE_CONSTRAINT_HASH  167

#define CONSTRAINT_DATA 43

struct constraintData
  { 
   struct constraintRecord **ConstraintHashtable;
   BOOLEAN StaticConstraintChecking;
   BOOLEAN DynamicConstraintChecking;
#if (BLOAD || BLOAD_ONLY || BLOAD_AND_BSAVE) && (! RUN_TIME)
   struct constraintRecord *ConstraintArray;
   long int NumberOfConstraints;
#endif
  };

#define ConstraintData(theEnv) ((struct constraintData *) GetEnvironmentData(theEnv,CONSTRAINT_DATA))

#if ENVIRONMENT_API_ONLY
#define GetDynamicConstraintChecking(theEnv) EnvGetDynamicConstraintChecking(theEnv)
#define GetStaticConstraintChecking(theEnv) EnvGetStaticConstraintChecking(theEnv)
#define SetDynamicConstraintChecking(theEnv,a) EnvSetDynamicConstraintChecking(theEnv,a)
#define SetStaticConstraintChecking(theEnv,a) EnvSetStaticConstraintChecking(theEnv,a)
#else
#define GetDynamicConstraintChecking() EnvGetDynamicConstraintChecking(GetCurrentEnvironment())
#define GetStaticConstraintChecking() EnvGetStaticConstraintChecking(GetCurrentEnvironment())
#define SetDynamicConstraintChecking(a) EnvSetDynamicConstraintChecking(GetCurrentEnvironment(),a)
#define SetStaticConstraintChecking(a) EnvSetStaticConstraintChecking(GetCurrentEnvironment(),a)
#endif

   LOCALE void                           InitializeConstraints(void *);
   LOCALE int                            GDCCommand(void *);
   LOCALE int                            SDCCommand(void *d);
   LOCALE int                            GSCCommand(void *);
   LOCALE int                            SSCCommand(void *);
   LOCALE BOOLEAN                        EnvSetDynamicConstraintChecking(void *,int);
   LOCALE BOOLEAN                        EnvGetDynamicConstraintChecking(void *);
   LOCALE BOOLEAN                        EnvSetStaticConstraintChecking(void *,int);
   LOCALE BOOLEAN                        EnvGetStaticConstraintChecking(void *);
#if (! BLOAD_ONLY) && (! RUN_TIME)
   LOCALE int                            HashConstraint(struct constraintRecord *);
   LOCALE struct constraintRecord       *AddConstraint(void *,struct constraintRecord *);
#endif
#if (! RUN_TIME)
   LOCALE void                           RemoveConstraint(void *,struct constraintRecord *);
#endif

#endif




