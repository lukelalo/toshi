   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.20  01/31/02            */
   /*                                                     */
   /*                  MULTIFIELD MODULE                  */
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

#define _MULTIFLD_SOURCE_

#include <stdio.h>
#define _STDIO_INCLUDED_

#include "setup.h"

#include "constant.h"
#include "memalloc.h"
#include "envrnmnt.h"
#include "evaluatn.h"
#include "scanner.h"
#include "router.h"
#include "strngrtr.h"
#include "utility.h"
#include "multifld.h"

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static void                    DeallocateMultifieldData(void *);

/***************************************************/
/* InitializeMultifieldData: Allocates environment */
/*    data for multifield values.                  */
/***************************************************/
globle void InitializeMultifieldData(
  void *theEnv)
  {
   AllocateEnvironmentData(theEnv,MULTIFIELD_DATA,sizeof(struct multifieldData),DeallocateMultifieldData);
  }

/*****************************************************/
/* DeallocateMultifieldData: Deallocates environment */
/*    data for multifield values.                    */
/*****************************************************/
static void DeallocateMultifieldData(
  void *theEnv)
  {
   struct multifield *tmpPtr, *nextPtr; 
   
   tmpPtr = MultifieldData(theEnv)->ListOfMultifields;
   while (tmpPtr != NULL)
     {
      nextPtr = tmpPtr->next;
      ReturnMultifield(theEnv,tmpPtr);
      tmpPtr = nextPtr;
     }
  }

/***********************************************************/
/* CreateMultifield2:       */
/***********************************************************/
globle void *CreateMultifield2(
  void *theEnv,
  unsigned long size)
  {
   struct multifield *theSegment;
   unsigned long newSize = size;

   if (size <= 0) newSize = 1;

   theSegment = get_var_struct2(theEnv,multifield,(long) sizeof(struct field) * (newSize - 1L));

   theSegment->multifieldLength = size;
   theSegment->depth = (short) EvaluationData(theEnv)->CurrentEvaluationDepth;
   theSegment->busyCount = 0;
   theSegment->next = NULL;

   return((void *) theSegment);
  }

/*****************************************************************/
/* ReturnMultifield:                                             */
/*****************************************************************/
globle void ReturnMultifield(
  void *theEnv,
  struct multifield *theSegment)
  {
   unsigned long newSize;

   if (theSegment == NULL) return;

   if (theSegment->multifieldLength == 0) newSize = 1;
   else newSize = theSegment->multifieldLength;

   rtn_var_struct2(theEnv,multifield,sizeof(struct field) * (newSize - 1),theSegment);
  }

/******************************/
/* MultifieldInstall:            */
/******************************/
globle void MultifieldInstall(
  void *theEnv,
  struct multifield *theSegment)
  {
   unsigned long length, i;
   struct field *theFields;

   if (theSegment == NULL) return;

   length = theSegment->multifieldLength;

   theSegment->busyCount++;
   theFields = theSegment->theFields;

   for (i = 0 ; i < length ; i++)
     { AtomInstall(theEnv,theFields[i].type,theFields[i].value); }
  }

/******************************/
/* MultifieldDeinstall:       */
/******************************/
globle void MultifieldDeinstall(
  void *theEnv,
  struct multifield *theSegment)
  {
   unsigned long length, i;
   struct field *theFields;

   if (theSegment == NULL) return;

   length = theSegment->multifieldLength;
   theSegment->busyCount--;
   theFields = theSegment->theFields;

   for (i = 0 ; i < length ; i++)
     { AtomDeinstall(theEnv,theFields[i].type,theFields[i].value); }
  }

/*******************************************************/
/* StringToMultifield:  Returns a multifield structure */
/*    that represents the string sent as the argument. */
/*******************************************************/
globle struct multifield *StringToMultifield(
  void *theEnv,
  char *theString)
  {
   struct token theToken;
   struct multifield *theSegment;
   struct field *theFields;
   unsigned long numberOfFields = 0;
   struct expr *topAtom = NULL, *lastAtom = NULL, *theAtom;

   /*====================================================*/
   /* Open the string as an input source and read in the */
   /* list of values to be stored in the multifield.     */
   /*====================================================*/

   OpenStringSource(theEnv,"multifield-str",theString,0);

   GetToken(theEnv,"multifield-str",&theToken);
   while (theToken.type != STOP)
     {
      if ((theToken.type == SYMBOL) || (theToken.type == STRING) ||
          (theToken.type == FLOAT) || (theToken.type == INTEGER) ||
          (theToken.type == INSTANCE_NAME))
        { theAtom = GenConstant(theEnv,theToken.type,theToken.value); }
      else
        { theAtom = GenConstant(theEnv,STRING,EnvAddSymbol(theEnv,theToken.printForm)); }

      numberOfFields++;
      if (topAtom == NULL) topAtom = theAtom;
      else lastAtom->nextArg = theAtom;

      lastAtom = theAtom;
      GetToken(theEnv,"multifield-str",&theToken);
     }

   CloseStringSource(theEnv,"multifield-str");

   /*====================================================================*/
   /* Create a multifield of the appropriate size for the values parsed. */
   /*====================================================================*/

   theSegment = (struct multifield *) EnvCreateMultifield(theEnv,numberOfFields);
   theFields = theSegment->theFields;

   /*====================================*/
   /* Copy the values to the multifield. */
   /*====================================*/

   theAtom = topAtom;
   numberOfFields = 0;
   while (theAtom != NULL)
     {
      theFields[numberOfFields].type = theAtom->type;
      theFields[numberOfFields].value = theAtom->value;
      numberOfFields++;
      theAtom = theAtom->nextArg;
     }

   /*===========================*/
   /* Return the parsed values. */
   /*===========================*/

   ReturnExpression(theEnv,topAtom);

   /*============================*/
   /* Return the new multifield. */
   /*============================*/

   return(theSegment);
  }
  
/**************************************************************/
/* EnvCreateMultifield: Creates a multifield of the specified */
/*   size and adds it to the list of segments.                */
/**************************************************************/
globle void *EnvCreateMultifield(
  void *theEnv,
  unsigned long size)
  {
   struct multifield *theSegment;
   unsigned long newSize;

   if (size <= 0) newSize = 1;
   else newSize = size;

   theSegment = get_var_struct2(theEnv,multifield,(long) sizeof(struct field) * (newSize - 1L));

   theSegment->multifieldLength = size;
   theSegment->depth = (short) EvaluationData(theEnv)->CurrentEvaluationDepth;
   theSegment->busyCount = 0;
   theSegment->next = NULL;

   theSegment->next = MultifieldData(theEnv)->ListOfMultifields;
   MultifieldData(theEnv)->ListOfMultifields = theSegment;

   UtilityData(theEnv)->EphemeralItemCount++;
   UtilityData(theEnv)->EphemeralItemSize += sizeof(struct multifield) + (sizeof(struct field) * newSize);

   return((void *) theSegment);
  }

/*********************************************************************/
/* DOToMultifield:    */
/*********************************************************************/
globle void *DOToMultifield(
  void *theEnv,
  DATA_OBJECT *theValue)
  {
   struct multifield *dst, *src;

   if (theValue->type != MULTIFIELD) return(NULL);

   dst = (struct multifield *) CreateMultifield2(theEnv,(unsigned long) GetpDOLength(theValue));

   src = (struct multifield *) theValue->value;
   GenCopyMemory(struct field,dst->multifieldLength,
              &(dst->theFields[0]),&(src->theFields[GetpDOBegin(theValue) - 1]));

   return((void *) dst);
  }

/***********************************************************/
/* AddToMultifieldList:                                       */
/***********************************************************/
globle void AddToMultifieldList(
  void *theEnv,
  struct multifield *theSegment)
  {
   theSegment->depth = (short) EvaluationData(theEnv)->CurrentEvaluationDepth;
   theSegment->next = MultifieldData(theEnv)->ListOfMultifields;
   MultifieldData(theEnv)->ListOfMultifields = theSegment;

   UtilityData(theEnv)->EphemeralItemCount++;
   UtilityData(theEnv)->EphemeralItemSize += sizeof(struct multifield) + (sizeof(struct field) * theSegment->multifieldLength);
  }

/***********************************************************/
/* FlushMultifields:                                         */
/***********************************************************/
globle void FlushMultifields(
  void *theEnv)
  {
   struct multifield *theSegment, *nextPtr, *lastPtr = NULL;
   unsigned long newSize;

   theSegment = MultifieldData(theEnv)->ListOfMultifields;
   while (theSegment != NULL)
     {
      nextPtr = theSegment->next;
      if ((theSegment->depth > EvaluationData(theEnv)->CurrentEvaluationDepth) && (theSegment->busyCount == 0))
        {
         UtilityData(theEnv)->EphemeralItemCount--;
         UtilityData(theEnv)->EphemeralItemSize -= sizeof(struct multifield) +
                              (sizeof(struct field) * theSegment->multifieldLength);
         if (theSegment->multifieldLength == 0) newSize = 1;
         else newSize = theSegment->multifieldLength;
         rtn_var_struct2(theEnv,multifield,sizeof(struct field) * (newSize - 1),theSegment);
         if (lastPtr == NULL) MultifieldData(theEnv)->ListOfMultifields = nextPtr;
         else lastPtr->next = nextPtr;
        }
      else
        { lastPtr = theSegment; }

      theSegment = nextPtr;
     }
  }

/*********************************************************************/
/* DuplicateMultifield: Allocates a new segment and copies results from */
/*                  old value to new - NOT put on ListOfMultifields!!   */
/*********************************************************************/
globle void DuplicateMultifield(
  void *theEnv,
  DATA_OBJECT_PTR dst,
  DATA_OBJECT_PTR src)
  {
   dst->type = MULTIFIELD;
   dst->begin = 0;
   dst->end = src->end - src->begin;
   dst->value = (void *) CreateMultifield2(theEnv,(unsigned long) dst->end + 1);
   GenCopyMemory(struct field,dst->end + 1,&((struct multifield *) dst->value)->theFields[0],
                                        &((struct multifield *) src->value)->theFields[src->begin]);
  }

/*********************************************************************/
/* CopyMultifield:    */
/*********************************************************************/
globle void *CopyMultifield(
  void *theEnv,
  struct multifield *src)
  {
   struct multifield *dst;

   dst = (struct multifield *) CreateMultifield2(theEnv,src->multifieldLength);
   GenCopyMemory(struct field,src->multifieldLength,&(dst->theFields[0]),&(src->theFields[0]));
   return((void *) dst);
  }

/**********************************************************/
/* PrintMultifield: Prints out a multifield               */
/**********************************************************/
globle void PrintMultifield(
  void *theEnv,
  char *fileid,
  struct multifield *segment,
  long begin,
  long end,
  int printParens)
  {
   struct field *theMultifield;
   int i;

   theMultifield = segment->theFields;
   if (printParens)
     EnvPrintRouter(theEnv,fileid,"(");
   i = begin;
   while (i <= end)
     {
      PrintAtom(theEnv,fileid,theMultifield[i].type,theMultifield[i].value);
      i++;
      if (i <= end) EnvPrintRouter(theEnv,fileid," ");
     }
   if (printParens)
     EnvPrintRouter(theEnv,fileid,")");
  }

/*****************************************************/
/* StoreInMultifield:  Append function for segments. */
/*****************************************************/
globle void StoreInMultifield(
  void *theEnv,
  DATA_OBJECT *returnValue,
  EXPRESSION *expptr,
  int garbageSegment)
  {
   DATA_OBJECT val_ptr;
   DATA_OBJECT *val_arr;
   struct multifield *theMultifield;
   struct multifield *orig_ptr;
   long start, end, i,j, k, argCount;
   unsigned long seg_size;

   argCount = CountArguments(expptr);

   /*=========================================*/
   /* If no arguments are given return a NULL */
   /* multifield of length zero.              */
   /*=========================================*/

   if (argCount == 0)
     {
      SetpType(returnValue,MULTIFIELD);
      SetpDOBegin(returnValue,1);
      SetpDOEnd(returnValue,0);
      if (garbageSegment) theMultifield = (struct multifield *) EnvCreateMultifield(theEnv,0L);
      else theMultifield = (struct multifield *) CreateMultifield2(theEnv,0L);
      SetpValue(returnValue,(void *) theMultifield);
      return;
     }

   else
     {
      /*========================================*/
      /* Get a new segment with length equal to */
      /* the total length of all the arguments. */
      /*========================================*/

      val_arr = (DATA_OBJECT *) gm3(theEnv,(long) sizeof(DATA_OBJECT) * argCount);
      seg_size = 0;
      for(i = 1 ; i <= argCount ; i++ , expptr = expptr->nextArg)
        {
         EvaluateExpression(theEnv,expptr,&val_ptr);
         if (EvaluationData(theEnv)->EvaluationError)
           {
            SetpType(returnValue,MULTIFIELD);
            SetpDOBegin(returnValue,1);
            SetpDOEnd(returnValue,0);
            if (garbageSegment)
              { theMultifield = (struct multifield *) EnvCreateMultifield(theEnv,0L); }
            else theMultifield = (struct multifield *) CreateMultifield2(theEnv,0L);
            SetpValue(returnValue,(void *) theMultifield);
            rm3(theEnv,val_arr,(long) sizeof(DATA_OBJECT) * argCount);
            return;
           }
         SetpType(val_arr+i-1,GetType(val_ptr));
         if (GetType(val_ptr) == MULTIFIELD)
           {
            SetpValue(val_arr+i-1,GetpValue(&val_ptr));
            start = GetDOBegin(val_ptr);
            end = GetDOEnd(val_ptr);
           }
         else if (GetType(val_ptr) == RVOID)
           {
            SetpValue(val_arr+i-1,GetValue(val_ptr));
            start = 1;
            end = 0;
           }
         else
           {
            SetpValue(val_arr+i-1,GetValue(val_ptr));
            start = end = -1;
           }

         seg_size += (unsigned long) (end - start + 1);
         SetpDOBegin(val_arr+i-1,start);
         SetpDOEnd(val_arr+i-1,end);
        }

      if (garbageSegment)
        { theMultifield = (struct multifield *) EnvCreateMultifield(theEnv,seg_size); }
      else theMultifield = (struct multifield *) CreateMultifield2(theEnv,seg_size);

      /*========================================*/
      /* Copy each argument into new segment.  */
      /*========================================*/

      for(k=0,j=1; k < argCount;k++)
        {
         if (GetpType(val_arr+k) == MULTIFIELD)
           {
            start = GetpDOBegin(val_arr+k);
            end = GetpDOEnd(val_arr+k);
            orig_ptr = (struct multifield *) GetpValue(val_arr+k);
            for(i=start; i< end + 1; i++,j++)
              {
               SetMFType(theMultifield,j,(GetMFType(orig_ptr,i)));
               SetMFValue(theMultifield,j,(GetMFValue(orig_ptr,i)));
              }
           }
         else if (GetpType(val_arr+k) != MULTIFIELD)
           {
            SetMFType(theMultifield,j,(short) (GetpType(val_arr+k)));
            SetMFValue(theMultifield,j,(GetpValue(val_arr+k)));
            j++;
           }
        }

      /*=========================*/
      /* Return the new segment. */
      /*=========================*/

      SetpType(returnValue,MULTIFIELD);
      SetpDOBegin(returnValue,1);
      SetpDOEnd(returnValue,(long) seg_size);
      SetpValue(returnValue,(void *) theMultifield);
      rm3(theEnv,val_arr,(long) sizeof(DATA_OBJECT) * argCount);
      return;
     }
  }

/*************************************************************/
/* MultifieldDOsEqual: determines if two segments are equal. */
/*************************************************************/
globle BOOLEAN MultifieldDOsEqual(
  DATA_OBJECT_PTR dobj1,
  DATA_OBJECT_PTR dobj2)
  {
   long extent1,extent2; /* 6.04 Bug Fix */
   FIELD_PTR e1,e2;

   extent1 = GetpDOLength(dobj1);
   extent2 = GetpDOLength(dobj2);
   if (extent1 != extent2)
     { return(FALSE); }

   e1 = (FIELD_PTR) GetMFPtr(GetpValue(dobj1),GetpDOBegin(dobj1));
   e2 = (FIELD_PTR) GetMFPtr(GetpValue(dobj2),GetpDOBegin(dobj2));
   while (extent1 != 0)
     {
      if (e1->type != e2->type)
        { return(FALSE); }

      if (e1->value != e2->value)
        { return(FALSE); }

      extent1--;

      if (extent1 > 0)
        {
         e1++;
         e2++;
        }
     }
   return(TRUE);
  }

/******************************************************************/
/* MultifieldsEqual: Determines if two multifields are identical. */
/******************************************************************/
globle int MultifieldsEqual(
  struct multifield *segment1,
  struct multifield *segment2)
  {
   struct field *elem1;
   struct field *elem2;
   unsigned long length, i = 0;

   length = segment1->multifieldLength;
   if (length != (int) segment2->multifieldLength)
     { return(FALSE); }

   elem1 = segment1->theFields;
   elem2 = segment2->theFields;

   /*==================================================*/
   /* Compare each field of both facts until the facts */
   /* match completely or the facts mismatch.          */
   /*==================================================*/

   while (i < length)
     {
      if (elem1[i].type != elem2[i].type)
        { return(FALSE); }

      if (elem1[i].type == MULTIFIELD)
        {
         if (MultifieldsEqual((struct multifield *) elem1[i].value,
                              (struct multifield *) elem2[i].value) == FALSE)
          { return(FALSE); }
        }
      else if (elem1[i].value != elem2[i].value)
        { return(FALSE); }

      i++;
     }
   return(TRUE);
  }

/************************************************************/
/* HashMultifield: Returns the hash value for a multifield. */
/************************************************************/
unsigned HashMultifield(
  struct multifield *theSegment,
  unsigned theRange)
  {
   unsigned long length, i;
   unsigned int tvalue;
   unsigned int count;
   struct field *fieldPtr;
   union
     {
      double fv;
      unsigned int liv;
     } fis;

   /*================================================*/
   /* Initialize variables for computing hash value. */
   /*================================================*/

   count = 0;
   length = theSegment->multifieldLength;
   fieldPtr = theSegment->theFields;

   /*====================================================*/
   /* Loop through each value in the multifield, compute */
   /* its hash value, and add it to the running total.   */
   /*====================================================*/

   for (i = 0;
        i < length;
        i++)
     {
      switch(fieldPtr[i].type)
         {
          case MULTIFIELD:
            count += HashMultifield((struct multifield *) fieldPtr[i].value,theRange);
            break;

          case FLOAT:
            fis.fv = ValueToDouble(fieldPtr[i].value);
            count += (fis.liv * (i + 29))  +
                     (int) ValueToDouble(fieldPtr[i].value);
            break;

          case INTEGER:
            count += (((int) ValueToLong(fieldPtr[i].value)) * (i + 29)) +
                             ValueToLong(fieldPtr[i].value);
            break;

          case FACT_ADDRESS:
          case EXTERNAL_ADDRESS:
#if OBJECT_SYSTEM
          case INSTANCE_ADDRESS:
#endif
            count += (unsigned int) (((int) fieldPtr[i].value) * (i + 29));
            break;

          case SYMBOL:
          case STRING:
#if OBJECT_SYSTEM
          case INSTANCE_NAME:
#endif
            tvalue = (unsigned) HashSymbol(ValueToString(fieldPtr[i].value),theRange);
            count += (unsigned) (tvalue * (i + 29));
            break;
         }
     }

   /*========================*/
   /* Return the hash value. */
   /*========================*/

   return(count);
  }

/**********************/
/* GetMultifieldList: */
/**********************/
globle struct multifield *GetMultifieldList(
  void *theEnv)
  {
   return(MultifieldData(theEnv)->ListOfMultifields);
  }





