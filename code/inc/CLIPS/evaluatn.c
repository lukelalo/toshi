   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.23  01/31/05            */
   /*                                                     */
   /*                  EVALUATION MODULE                  */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides routines for evaluating expressions.    */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Donnell                                     */
/*                                                           */
/* Revision History:                                         */
/*      6.23: Correction for FalseSymbol/TrueSymbol. DR0859  */
/*                                                           */
/*************************************************************/

#define _EVALUATN_SOURCE_

#include <stdio.h>
#define _STDIO_INCLUDED_
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "setup.h"

#include "commline.h"
#include "constant.h"
#include "envrnmnt.h"
#include "memalloc.h"
#include "router.h"
#include "extnfunc.h"
#include "prcdrfun.h"
#include "multifld.h"
#include "factmngr.h"
#include "prntutil.h"
#include "exprnpsr.h"
#include "utility.h"
#include "proflfun.h"
#include "sysdep.h"

#if DEFFUNCTION_CONSTRUCT
#include "dffnxfun.h"
#endif

#if DEFGENERIC_CONSTRUCT
#include "genrccom.h"
#endif

#if OBJECT_SYSTEM
#include "object.h"
#endif

#include "evaluatn.h"

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static void                    PropagateReturnAtom(void *,int,void *);

/**************************************************/
/* InitializeEvaluationData: Allocates environment */
/*    data for expression evaluation.             */
/**************************************************/
globle void InitializeEvaluationData(
  void *theEnv)
  {
   AllocateEnvironmentData(theEnv,EVALUATION_DATA,sizeof(struct evaluationData),NULL);
  }

/**************************************************************/
/* EvaluateExpression: Evaluates an expression. Returns FALSE */
/*   if no errors occurred during evaluation, otherwise TRUE. */
/**************************************************************/
globle int EvaluateExpression(
  void *theEnv,
  struct expr *problem,
  DATA_OBJECT_PTR returnValue)
  {
   struct expr *oldArgument;
   struct FunctionDefinition *fptr;
#if PROFILING_FUNCTIONS
   struct profileFrameInfo profileFrame;
#endif

   if (problem == NULL)
     {
      returnValue->type = SYMBOL;
      returnValue->value = EnvFalseSymbol(theEnv);
      return(EvaluationData(theEnv)->EvaluationError);
     }

   switch (problem->type)
     {
      case STRING:
      case SYMBOL:
      case FLOAT:
      case INTEGER:
#if OBJECT_SYSTEM
      case INSTANCE_NAME:
      case INSTANCE_ADDRESS:
#endif
      case EXTERNAL_ADDRESS:
        returnValue->type = problem->type;
        returnValue->value = problem->value;
        break;

      case FCALL:
        {
         fptr = (struct FunctionDefinition *) problem->value;

#if PROFILING_FUNCTIONS   
         StartProfile(theEnv,&profileFrame,
                      &fptr->usrData,
                      ProfileFunctionData(theEnv)->ProfileUserFunctions);
#endif

         oldArgument = EvaluationData(theEnv)->CurrentExpression;
         EvaluationData(theEnv)->CurrentExpression = problem;

         switch(fptr->returnValueType)
           {
            case 'v' :
              if (fptr->environmentAware)
                { (* (void (*)(void *)) fptr->functionPointer)(theEnv); }
              else
                { (* (void (*)(void)) fptr->functionPointer)(); }
              returnValue->type = RVOID;
              returnValue->value = EnvFalseSymbol(theEnv);
              break;
            case 'b' :
              returnValue->type = SYMBOL;
              if (fptr->environmentAware)
                {
                 if ((* (int (*)(void *)) fptr->functionPointer)(theEnv))
                   returnValue->value = EnvTrueSymbol(theEnv);
                 else
                   returnValue->value = EnvFalseSymbol(theEnv);
                }
              else
                {
                 if ((* (int (*)(void)) fptr->functionPointer)())
                   returnValue->value = EnvTrueSymbol(theEnv);
                 else
                   returnValue->value = EnvFalseSymbol(theEnv);
                }
              break;
            case 'a' :
              returnValue->type = EXTERNAL_ADDRESS;
              if (fptr->environmentAware)
                {
                 returnValue->value =
                                (* (void *(*)(void *)) fptr->functionPointer)(theEnv);
                }
              else
                {
                 returnValue->value =
                                (* (void *(*)(void)) fptr->functionPointer)();
                }
              break;
            case 'i' :
              returnValue->type = INTEGER;
              if (fptr->environmentAware)
                {
                 returnValue->value = (void *)
                   EnvAddLong(theEnv,(long) (* (int (*)(void *)) fptr->functionPointer)(theEnv));
                }
              else
                {
                 returnValue->value = (void *)
                   EnvAddLong(theEnv,(long) (* (int (*)(void)) fptr->functionPointer)());
                }
              break;
            case 'l' :
              returnValue->type = INTEGER;
              if (fptr->environmentAware)
                {
                 returnValue->value = (void *)
                    EnvAddLong(theEnv,(* (long int (*)(void *)) fptr->functionPointer)(theEnv));
                }
              else
                {
                 returnValue->value = (void *)
                    EnvAddLong(theEnv,(* (long int (*)(void)) fptr->functionPointer)());
                }
              break;
            case 'f' :
              returnValue->type = FLOAT;
              if (fptr->environmentAware)
                {
                 returnValue->value = (void *)
                    EnvAddDouble(theEnv,(double) (* (float (*)(void *)) fptr->functionPointer)(theEnv));
                }
              else
                {
                 returnValue->value = (void *)
                    EnvAddDouble(theEnv,(double) (* (float (*)(void)) fptr->functionPointer)());
                }
              break;
            case 'd' :
              returnValue->type = FLOAT;
              if (fptr->environmentAware)
                {
                 returnValue->value = (void *)
                    EnvAddDouble(theEnv,(* (double (*)(void *)) fptr->functionPointer)(theEnv));
                }
              else
                {
                 returnValue->value = (void *)
                    EnvAddDouble(theEnv,(* (double (*)(void)) fptr->functionPointer)());
                }
              break;
            case 's' :
              returnValue->type = STRING;
              if (fptr->environmentAware)
                {
                 returnValue->value = (void *)
                   (* (SYMBOL_HN *(*)(void *)) fptr->functionPointer)(theEnv);
                }
              else
                {
                 returnValue->value = (void *)
                   (* (SYMBOL_HN *(*)(void)) fptr->functionPointer)();
                }
              break;
            case 'w' :
              returnValue->type = SYMBOL;
              if (fptr->environmentAware)
                {
                 returnValue->value = (void *)
                   (* (SYMBOL_HN *(*)(void *)) fptr->functionPointer)(theEnv);
                }
              else
                {
                 returnValue->value = (void *)
                   (* (SYMBOL_HN *(*)(void)) fptr->functionPointer)();
                }
              break;
#if OBJECT_SYSTEM
            case 'x' :
              returnValue->type = INSTANCE_ADDRESS;
              if (fptr->environmentAware)
                {
                 returnValue->value =
                                (* (void *(*)(void *)) fptr->functionPointer)(theEnv);
                }
              else
                {
                 returnValue->value =
                                (* (void *(*)(void)) fptr->functionPointer)();
                }
              break;
            case 'o' :
              returnValue->type = INSTANCE_NAME;
              if (fptr->environmentAware)
                {
                 returnValue->value = (void *)
                   (* (SYMBOL_HN *(*)(void *)) fptr->functionPointer)(theEnv);
                }
              else
                {
                 returnValue->value = (void *)
                   (* (SYMBOL_HN *(*)(void)) fptr->functionPointer)();
                }
              break;
#endif
            case 'c' :
              {
               char cbuff[2];
               if (fptr->environmentAware)
                 {
                  cbuff[0] = (* (char (*)(void *)) fptr->functionPointer)(theEnv);
                 }
               else
                 {
                  cbuff[0] = (* (char (*)(void)) fptr->functionPointer)();
                 }
               cbuff[1] = EOS;
               returnValue->type = SYMBOL;
               returnValue->value = (void *) EnvAddSymbol(theEnv,cbuff);
               break;
              }

            case 'j' :
            case 'k' :
            case 'm' :
            case 'n' :
            case 'u' :
               if (fptr->environmentAware)
                 {
                  (* (void (*)(void *,DATA_OBJECT_PTR)) fptr->functionPointer)(theEnv,returnValue);
                 }
               else
                 {
                  (* (void (*)(DATA_OBJECT_PTR)) fptr->functionPointer)(returnValue);
                 }
              break;

            default :
               SystemError(theEnv,"EVALUATN",2);
               EnvExitRouter(theEnv,EXIT_FAILURE);
               break;
            }

#if PROFILING_FUNCTIONS 
        EndProfile(theEnv,&profileFrame);
#endif

        EvaluationData(theEnv)->CurrentExpression = oldArgument;
        break;
        }

     case MULTIFIELD:
        returnValue->type = MULTIFIELD;
        returnValue->value = ((DATA_OBJECT_PTR) (problem->value))->value;
        returnValue->begin = ((DATA_OBJECT_PTR) (problem->value))->begin;
        returnValue->end = ((DATA_OBJECT_PTR) (problem->value))->end;
        break;

     case MF_VARIABLE:
     case SF_VARIABLE:
        if (GetBoundVariable(theEnv,returnValue,(SYMBOL_HN *) problem->value) == FALSE)
          {
           PrintErrorID(theEnv,"EVALUATN",1,FALSE);
           EnvPrintRouter(theEnv,WERROR,"Variable ");
           EnvPrintRouter(theEnv,WERROR,ValueToString(problem->value));
           EnvPrintRouter(theEnv,WERROR," is unbound\n");
           returnValue->type = SYMBOL;
           returnValue->value = EnvFalseSymbol(theEnv);
           SetEvaluationError(theEnv,TRUE);
          }
        break;

      default:
        if (EvaluationData(theEnv)->PrimitivesArray[problem->type] == NULL)
          {
           SystemError(theEnv,"EVALUATN",3);
           EnvExitRouter(theEnv,EXIT_FAILURE);
          }

        if (EvaluationData(theEnv)->PrimitivesArray[problem->type]->copyToEvaluate)
          {
           returnValue->type = problem->type;
           returnValue->value = problem->value;
           break;
          }

        if (EvaluationData(theEnv)->PrimitivesArray[problem->type]->evaluateFunction == NULL)
          {
           SystemError(theEnv,"EVALUATN",4);
           EnvExitRouter(theEnv,EXIT_FAILURE);
          }

        oldArgument = EvaluationData(theEnv)->CurrentExpression;
        EvaluationData(theEnv)->CurrentExpression = problem;

#if PROFILING_FUNCTIONS 
        StartProfile(theEnv,&profileFrame,
                     &EvaluationData(theEnv)->PrimitivesArray[problem->type]->usrData,
                     ProfileFunctionData(theEnv)->ProfileUserFunctions);
#endif

        (*EvaluationData(theEnv)->PrimitivesArray[problem->type]->evaluateFunction)(theEnv,problem->value,returnValue);

#if PROFILING_FUNCTIONS
        EndProfile(theEnv,&profileFrame);
#endif

        EvaluationData(theEnv)->CurrentExpression = oldArgument;
        break;
     }

   PropagateReturnValue(theEnv,returnValue);
   return(EvaluationData(theEnv)->EvaluationError);
  }

/******************************************/
/* InstallPrimitive: Installs a primitive */
/*   data type in the primitives array.   */
/******************************************/
globle void InstallPrimitive(
  void *theEnv,
  struct entityRecord *thePrimitive,
  int whichPosition)
  {
   if (EvaluationData(theEnv)->PrimitivesArray[whichPosition] != NULL)
     {
      SystemError(theEnv,"EVALUATN",5);
      EnvExitRouter(theEnv,EXIT_FAILURE);
     }

   EvaluationData(theEnv)->PrimitivesArray[whichPosition] = thePrimitive;
  }

/******************************************************/
/* SetEvaluationError: Sets the EvaluationError flag. */
/******************************************************/
globle void SetEvaluationError(
  void *theEnv,
  int value)
  {
   EvaluationData(theEnv)->EvaluationError = value;
   if (value == TRUE) 
     { EvaluationData(theEnv)->HaltExecution = TRUE; }
  }

/*********************************************************/
/* GetEvaluationError: Returns the EvaluationError flag. */
/*********************************************************/
globle int GetEvaluationError(
  void *theEnv)
  {
   return(EvaluationData(theEnv)->EvaluationError);
  }

/**************************************************/
/* SetHaltExecution: Sets the HaltExecution flag. */
/**************************************************/
globle void SetHaltExecution(
  void *theEnv,
  int value)
  { 
   EvaluationData(theEnv)->HaltExecution = value; 
  }

/*****************************************************/
/* GetHaltExecution: Returns the HaltExecution flag. */
/*****************************************************/
globle int GetHaltExecution(
  void *theEnv)
  {
   return(EvaluationData(theEnv)->HaltExecution);
  }

/******************************************************/
/* ReturnValues: Returns a linked list of DATA_OBJECT */
/*   structures to the pool of free memory.           */
/******************************************************/
globle void ReturnValues(
  void *theEnv,
  DATA_OBJECT_PTR garbagePtr)
  {
   DATA_OBJECT_PTR nextPtr;

   while (garbagePtr != NULL)
     {
      nextPtr = garbagePtr->next;
      ValueDeinstall(theEnv,garbagePtr);
      rtn_struct(theEnv,dataObject,garbagePtr);
      garbagePtr = nextPtr;
     }
  }

/***************************************************/
/* PrintDataObject: Prints a DATA_OBJECT structure */
/*   to the specified logical name.                */
/***************************************************/
globle void PrintDataObject(
  void *theEnv,
  char *fileid,
  DATA_OBJECT_PTR argPtr)
  {
   switch(argPtr->type)
     {
      case RVOID:
      case SYMBOL:
      case STRING:
      case INTEGER:
      case FLOAT:
      case EXTERNAL_ADDRESS:
      case FACT_ADDRESS:
#if OBJECT_SYSTEM
      case INSTANCE_NAME:
      case INSTANCE_ADDRESS:
#endif
        PrintAtom(theEnv,fileid,argPtr->type,argPtr->value);
        break;
      case MULTIFIELD:
        PrintMultifield(theEnv,fileid,(struct multifield *) argPtr->value,
                        argPtr->begin,argPtr->end,TRUE);
        break;

      default:
        if (EvaluationData(theEnv)->PrimitivesArray[argPtr->type] != NULL)
          {
           if (EvaluationData(theEnv)->PrimitivesArray[argPtr->type]->longPrintFunction)
             {
              (*EvaluationData(theEnv)->PrimitivesArray[argPtr->type]->longPrintFunction)(theEnv,fileid,argPtr->value);
              break;
             }
           else if (EvaluationData(theEnv)->PrimitivesArray[argPtr->type]->shortPrintFunction)
             {
              (*EvaluationData(theEnv)->PrimitivesArray[argPtr->type]->shortPrintFunction)(theEnv,fileid,argPtr->value);
              break;
             }
          }

        EnvPrintRouter(theEnv,fileid,"<UnknownPrintType");
        PrintLongInteger(theEnv,fileid,(long int) argPtr->type);
        EnvPrintRouter(theEnv,fileid,">");
        SetHaltExecution(theEnv,TRUE);
        SetEvaluationError(theEnv,TRUE);
        break;
     }
  }

/****************************************************/
/* EnvSetMultifieldErrorValue: Creates a multifield */
/*   value of length zero for error returns.        */
/****************************************************/
globle void EnvSetMultifieldErrorValue(
  void *theEnv,
  DATA_OBJECT_PTR returnValue)
  {
   returnValue->type = MULTIFIELD;
   returnValue->value = EnvCreateMultifield(theEnv,0L);
   returnValue->begin = 1;
   returnValue->end = 0;
  }

/**************************************************/
/* ValueInstall: Increments the appropriate count */
/*   (in use) values for a DATA_OBJECT structure. */
/**************************************************/
globle void ValueInstall(
  void *theEnv,
  DATA_OBJECT *vPtr)
  {
   if (vPtr->type == MULTIFIELD) MultifieldInstall(theEnv,(struct multifield *) vPtr->value);
   else AtomInstall(theEnv,vPtr->type,vPtr->value);
  }

/****************************************************/
/* ValueDeinstall: Decrements the appropriate count */
/*   (in use) values for a DATA_OBJECT structure.   */
/****************************************************/
globle void ValueDeinstall(
  void *theEnv,
  DATA_OBJECT *vPtr)
  {
   if (vPtr->type == MULTIFIELD) MultifieldDeinstall(theEnv,(struct multifield *) vPtr->value);
   else AtomDeinstall(theEnv,vPtr->type,vPtr->value);
  }

/*****************************************/
/* AtomInstall: Increments the reference */
/*   count of an atomic data type.       */
/*****************************************/
globle void AtomInstall(
  void *theEnv,
  int type,
  void *vPtr)
  {
   switch (type)
     {
      case SYMBOL:
      case STRING:
#if DEFGLOBAL_CONSTRUCT
      case GBL_VARIABLE:
#endif
#if OBJECT_SYSTEM
      case INSTANCE_NAME:
#endif
        IncrementSymbolCount(vPtr);
        break;

      case FLOAT:
        IncrementFloatCount(vPtr);
        break;

      case INTEGER:
        IncrementIntegerCount(vPtr);
        break;

      case MULTIFIELD:
        MultifieldInstall(theEnv,(struct multifield *) vPtr);
        break;

      case RVOID:
        break;

      default:
        if (EvaluationData(theEnv)->PrimitivesArray[type] == NULL) break;
        if (EvaluationData(theEnv)->PrimitivesArray[type]->bitMap) IncrementBitMapCount(vPtr);
        else if (EvaluationData(theEnv)->PrimitivesArray[type]->incrementBusyCount)
          { (*EvaluationData(theEnv)->PrimitivesArray[type]->incrementBusyCount)(theEnv,vPtr); }
        break;
     }
  }

/*******************************************/
/* AtomDeinstall: Decrements the reference */
/*   count of an atomic data type.         */
/*******************************************/
globle void AtomDeinstall(
  void *theEnv,
  int type,
  void *vPtr)
  {
   switch (type)
     {
      case SYMBOL:
      case STRING:
#if DEFGLOBAL_CONSTRUCT
      case GBL_VARIABLE:
#endif
#if OBJECT_SYSTEM
      case INSTANCE_NAME:
#endif
        DecrementSymbolCount(theEnv,(SYMBOL_HN *) vPtr);
        break;

      case FLOAT:
        DecrementFloatCount(theEnv,(FLOAT_HN *) vPtr);
        break;

      case INTEGER:
        DecrementIntegerCount(theEnv,(INTEGER_HN *) vPtr);
        break;

      case MULTIFIELD:
        MultifieldDeinstall(theEnv,(struct multifield *) vPtr);
        break;

      case RVOID:
        break;

      default:
        if (EvaluationData(theEnv)->PrimitivesArray[type] == NULL) break;
        if (EvaluationData(theEnv)->PrimitivesArray[type]->bitMap) DecrementBitMapCount(theEnv,(BITMAP_HN *) vPtr);
        else if (EvaluationData(theEnv)->PrimitivesArray[type]->decrementBusyCount)
          { (*EvaluationData(theEnv)->PrimitivesArray[type]->decrementBusyCount)(theEnv,vPtr); }
     }
  }

/*********************************************************************/
/* PropagateReturnValue: Decrements the associated depth for a value */
/*   stored in a DATA_OBJECT structure. In effect, the values        */
/*   returned by certain evaluations (such as a deffunction call)    */
/*   are passed up to the previous depth of evaluation. The return   */
/*   value's depth is decremented so that it will not be garbage     */
/*   collected along with other items that are no longer needed from */
/*   the evaluation that generated the return value.                 */
/*********************************************************************/
globle void PropagateReturnValue(
  void *theEnv,
  DATA_OBJECT *vPtr)
  {
   unsigned long i; /* 6.04 Bug Fix */
   struct multifield *theSegment;
   struct field *theMultifield;

   if (vPtr->type != MULTIFIELD)
     { PropagateReturnAtom(theEnv,vPtr->type,vPtr->value); }
   else
     {
      theSegment = (struct multifield *) vPtr->value;

      if (theSegment->depth > EvaluationData(theEnv)->CurrentEvaluationDepth)
        theSegment->depth = (short) EvaluationData(theEnv)->CurrentEvaluationDepth;

      theMultifield = theSegment->theFields;

      for (i = 0; i < theSegment->multifieldLength; i++)
        { PropagateReturnAtom(theEnv,theMultifield[i].type,theMultifield[i].value); }
     }
  }

/*****************************************/
/* PropagateReturnAtom: Support function */
/*   for PropagateReturnValue.           */
/*****************************************/
static void PropagateReturnAtom(
  void *theEnv,
  int type,
  void *value)
  {
   switch (type)
     {
      case INTEGER         :
      case FLOAT           :
      case SYMBOL          :
      case STRING          :
#if OBJECT_SYSTEM
      case INSTANCE_NAME   :
#endif
        if (((SYMBOL_HN *) value)->depth > EvaluationData(theEnv)->CurrentEvaluationDepth)
          { ((SYMBOL_HN *) value)->depth = EvaluationData(theEnv)->CurrentEvaluationDepth; }
        break;

#if OBJECT_SYSTEM
      case INSTANCE_ADDRESS :
        if (((INSTANCE_TYPE *) value)->depth > EvaluationData(theEnv)->CurrentEvaluationDepth)
          { ((INSTANCE_TYPE *) value)->depth = EvaluationData(theEnv)->CurrentEvaluationDepth; }
        break;
#endif
      case FACT_ADDRESS :
        if (((int) ((struct fact *) value)->depth) > EvaluationData(theEnv)->CurrentEvaluationDepth)
          { ((struct fact *) value)->depth = (unsigned) EvaluationData(theEnv)->CurrentEvaluationDepth; }
        break;
     }
  }

#if DEFFUNCTION_CONSTRUCT || DEFGENERIC_CONSTRUCT

/********************************************/
/* EnvFunctionCall: Allows Deffunctions and */
/*   Generic Functions to be called from C. */
/*   Allows only constants as arguments.    */
/********************************************/
globle int EnvFunctionCall(
  void *theEnv,
  char *name,
  char *args,
  DATA_OBJECT *result)
  {
   FUNCTION_REFERENCE theReference;

   /*=======================================*/
   /* Call the function if it can be found. */
   /*=======================================*/

   if (GetFunctionReference(theEnv,name,&theReference))
     { return(FunctionCall2(theEnv,&theReference,args,result)); }

   /*=========================================================*/
   /* Otherwise signal an error if a deffunction, defgeneric, */
   /* or user defined function doesn't exist that matches     */
   /* the specified function name.                            */
   /*=========================================================*/

   PrintErrorID(theEnv,"EVALUATN",2,FALSE);
   EnvPrintRouter(theEnv,WERROR,"No function, generic function or deffunction of name ");
   EnvPrintRouter(theEnv,WERROR,name);
   EnvPrintRouter(theEnv,WERROR," exists for external call.\n");
   return(TRUE);
  }

/********************************************/
/* FunctionCall2: Allows Deffunctions and    */
/*   Generic Functions to be called from C. */
/*   Allows only constants as arguments.    */
/********************************************/
globle int FunctionCall2(
  void *theEnv,
  FUNCTION_REFERENCE *theReference,
  char *args,
  DATA_OBJECT *result)
  {
   EXPRESSION *argexps;
   int error = FALSE;

   /*=============================================*/
   /* Force periodic cleanup if the function call */
   /* was executed from an embedded application.  */
   /*=============================================*/

   if ((EvaluationData(theEnv)->CurrentEvaluationDepth == 0) && (! CommandLineData(theEnv)->EvaluatingTopLevelCommand) &&
       (EvaluationData(theEnv)->CurrentExpression == NULL))
     { PeriodicCleanup(theEnv,TRUE,FALSE); }

   /*========================*/
   /* Reset the error state. */
   /*========================*/

   if (EvaluationData(theEnv)->CurrentEvaluationDepth == 0) SetHaltExecution(theEnv,FALSE);
   EvaluationData(theEnv)->EvaluationError = FALSE;

   /*======================================*/
   /* Initialize the default return value. */
   /*======================================*/

   result->type = SYMBOL;
   result->value = EnvFalseSymbol(theEnv);

   /*============================*/
   /* Parse the argument string. */
   /*============================*/

   argexps = ParseConstantArguments(theEnv,args,&error);
   if (error == TRUE) return(TRUE);

   /*====================*/
   /* Call the function. */
   /*====================*/

   theReference->argList = argexps;
   error = EvaluateExpression(theEnv,theReference,result);

   /*========================*/
   /* Return the expression. */
   /*========================*/

   ReturnExpression(theEnv,argexps);
   theReference->argList = NULL;

   /*==========================*/
   /* Return the error status. */
   /*==========================*/

   return(error);
  }

#endif

/***************************************************/
/* CopyDataObject: Copies the values from a source */
/*   DATA_OBJECT to a destination DATA_OBJECT.     */
/***************************************************/
globle void CopyDataObject(
  void *theEnv,
  DATA_OBJECT *dst,
  DATA_OBJECT *src,
  int garbageMultifield)
  {
   if (src->type != MULTIFIELD)
     {
      dst->type = src->type;
      dst->value = src->value;
     }
   else
     {
      DuplicateMultifield(theEnv,dst,src);
      if (garbageMultifield)
        { AddToMultifieldList(theEnv,(struct multifield *) dst->value); }
     }
  }

/***********************************************/
/* TransferDataObjectValues: Copies the values */
/*   directly from a source DATA_OBJECT to a   */
/*   destination DATA_OBJECT.                  */
/***********************************************/
globle void TransferDataObjectValues(
  DATA_OBJECT *dst,
  DATA_OBJECT *src)
  {
   dst->type = src->type;
   dst->value = src->value;
   dst->begin = src->begin;
   dst->end = src->end;
   dst->supplementalInfo = src->supplementalInfo;
   dst->next = src->next;
  }

/************************************************************************/
/* ConvertValueToExpression: Converts the value stored in a data object */
/*   into an expression. For multifield values, a chain of expressions  */
/*   is generated and the chain is linked by the nextArg field. For a   */
/*   single field value, a single expression is created.                */
/************************************************************************/
globle struct expr *ConvertValueToExpression(
  void *theEnv,
  DATA_OBJECT *theValue)
  {
   long i;
   struct expr *head = NULL, *last = NULL, *newItem;

   if (GetpType(theValue) != MULTIFIELD)
     { return(GenConstant(theEnv,GetpType(theValue),GetpValue(theValue))); }

   for (i = GetpDOBegin(theValue); i <= GetpDOEnd(theValue); i++)
     {
      newItem = GenConstant(theEnv,GetMFType(GetpValue(theValue),i),
                        GetMFValue(GetpValue(theValue),i));
      if (last == NULL) head = newItem;
      else last->nextArg = newItem;
      last = newItem;
     }

   if (head == NULL)
     return(GenConstant(theEnv,FCALL,(void *) FindFunction(theEnv,"create$")));

   return(head);
  }

/****************************************/
/* GetAtomicHashValue: Returns the hash */
/*   value for an atomic data type.     */
/****************************************/
unsigned int GetAtomicHashValue(
  unsigned short type,
  void *value,
  int position)
  {
   unsigned int tvalue;
   union
     {
      double fv;
      unsigned int liv;
     } fis;

   switch (type)
     {
      case FLOAT:
        fis.fv = ValueToDouble(value);
        tvalue = fis.liv;
        break;

      case INTEGER:
        tvalue = (unsigned int) ValueToLong(value);
        break;

      case FACT_ADDRESS:
#if OBJECT_SYSTEM
      case INSTANCE_ADDRESS:
#endif
      case EXTERNAL_ADDRESS:
         tvalue = (unsigned int) value;
         break;

      case STRING:
#if OBJECT_SYSTEM
      case INSTANCE_NAME:
#endif
      case SYMBOL:
        tvalue = ((SYMBOL_HN *) value)->bucket;
        break;

      default:
        tvalue = type;
     }

   if (position < 0) return(tvalue);

   return((unsigned int) (tvalue * (position + 29)));
  }

/***********************************************************/
/* FunctionReferenceExpression: Returns an expression with */
/*   an appropriate expression reference to the specified  */
/*   name if it is the name of a deffunction, defgeneric,  */
/*   or user/system defined function.                      */
/***********************************************************/
globle struct expr *FunctionReferenceExpression(
  void *theEnv,
  char *name)
  {
#if DEFGENERIC_CONSTRUCT
   void *gfunc;
#endif
#if DEFFUNCTION_CONSTRUCT
   void *dptr;
#endif
   struct FunctionDefinition *fptr;

   /*=====================================================*/
   /* Check to see if the function call is a deffunction. */
   /*=====================================================*/

#if DEFFUNCTION_CONSTRUCT
   if ((dptr = (void *) LookupDeffunctionInScope(theEnv,name)) != NULL)
     { return(GenConstant(theEnv,PCALL,dptr)); }
#endif

   /*====================================================*/
   /* Check to see if the function call is a defgeneric. */
   /*====================================================*/

#if DEFGENERIC_CONSTRUCT
   if ((gfunc = (void *) LookupDefgenericInScope(theEnv,name)) != NULL)
     { return(GenConstant(theEnv,GCALL,gfunc)); }
#endif

   /*======================================*/
   /* Check to see if the function call is */
   /* a system or user defined function.   */
   /*======================================*/

   if ((fptr = FindFunction(theEnv,name)) != NULL)
     { return(GenConstant(theEnv,FCALL,fptr)); }

   /*===================================================*/
   /* The specified function name is not a deffunction, */
   /* defgeneric, or user/system defined function.      */
   /*===================================================*/

   return(NULL);
  }

/******************************************************************/
/* GetFunctionReference: Fills an expression with an appropriate  */
/*   expression reference to the specified name if it is the      */
/*   name of a deffunction, defgeneric, or user/system defined    */
/*   function.                                                    */
/******************************************************************/
globle BOOLEAN GetFunctionReference(
  void *theEnv,
  char *name,
  FUNCTION_REFERENCE *theReference)
  {
#if DEFGENERIC_CONSTRUCT
   void *gfunc;
#endif
#if DEFFUNCTION_CONSTRUCT
   void *dptr;
#endif
   struct FunctionDefinition *fptr;

   theReference->nextArg = NULL;
   theReference->argList = NULL;
   theReference->type = RVOID;
   theReference->value = NULL;

   /*=====================================================*/
   /* Check to see if the function call is a deffunction. */
   /*=====================================================*/

#if DEFFUNCTION_CONSTRUCT
   if ((dptr = (void *) LookupDeffunctionInScope(theEnv,name)) != NULL)
     {
      theReference->type = PCALL;
      theReference->value = dptr;
      return(TRUE);
     }
#endif

   /*====================================================*/
   /* Check to see if the function call is a defgeneric. */
   /*====================================================*/

#if DEFGENERIC_CONSTRUCT
   if ((gfunc = (void *) LookupDefgenericInScope(theEnv,name)) != NULL)
     {
      theReference->type = GCALL;
      theReference->value = gfunc;
      return(TRUE);
     }
#endif

   /*======================================*/
   /* Check to see if the function call is */
   /* a system or user defined function.   */
   /*======================================*/

   if ((fptr = FindFunction(theEnv,name)) != NULL)
     {
      theReference->type = FCALL;
      theReference->value = fptr;
      return(TRUE);
     }

   /*===================================================*/
   /* The specified function name is not a deffunction, */
   /* defgeneric, or user/system defined function.      */
   /*===================================================*/

   return(FALSE);
  }

/*******************************************************/
/* DOsEqual: Determines if two DATA_OBJECTS are equal. */
/*******************************************************/
globle BOOLEAN DOsEqual(
  DATA_OBJECT_PTR dobj1,
  DATA_OBJECT_PTR dobj2)
  {
   if (GetpType(dobj1) != GetpType(dobj2))
     { return(FALSE); }

   if (GetpType(dobj1) == MULTIFIELD)
     {
      if (MultifieldDOsEqual(dobj1,dobj2) == FALSE)
        { return(FALSE); }
     }
   else if (GetpValue(dobj1) != GetpValue(dobj2))
     { return(FALSE); }

   return(TRUE);
  }


