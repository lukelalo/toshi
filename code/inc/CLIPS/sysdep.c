   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.22  06/15/04            */
   /*                                                     */
   /*               SYSTEM DEPENDENT MODULE               */
   /*******************************************************/

/*************************************************************/
/* Purpose: Isolation of system dependent routines.          */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Donnell                                     */
/*                                                           */
/* Revision History:                                         */
/*      6.23: Modified GenOpen to check the file length      */
/*            against the system constant FILENAME_MAX.      */
/*                                                           */
/*                                                           */
/*************************************************************/

#define _SYSDEP_SOURCE_

#include "setup.h"

#include <stdio.h>
#define _STDIO_INCLUDED_
#include <string.h>

#include <stdlib.h>
#include <time.h>

#if   VAX_VMS
#include timeb
#include <descrip.h>
#include <ssdef.h>
#include <stsdef.h>
#include signal
extern int LIB$SPAWN();
#endif

#if MAC

#if MAC_MCW || MAC_XCD
#include <Carbon/Carbon.h> 
#endif

#define kTwoPower32 (4294967296.0)      /* 2^32 */
#if MAC_MCW || MAC_XCD
#include <strings.h>
#endif
#endif

#if MAC_MCW || IBM_MCW || MAC_XCD 
#include <unistd.h>
#endif

#if IBM_ICB
#include <i32.h>
#include <stk.h>
#include <sys\types.h>
#include <sys\timeb.h>
#include <io.h>
#include <fcntl.h>
#include <limits.h>
#include <process.h>
#endif

#if   IBM_MSC
#include <sys\types.h>
#include <sys\timeb.h>
#include <io.h>
#include <fcntl.h>
#include <limits.h>
#include <process.h>
#endif

#if   IBM_TBC
#include <bios.h>
#include <io.h>
#include <fcntl.h>
#include <limits.h>
#endif

#if IBM_MCW
#include <io.h>
#include <limits.h>
#endif

#if   IBM_ZTC || IBM_SC
#include <time.h>
#include <controlc.h>
#include <io.h>
#include <fcntl.h>
#include <limits.h>
#include <process.h>
#endif

#if   UNIX_7 || IBM_GCC
#include <sys/types.h>
#include <sys/timeb.h>
#include <signal.h>
#endif

#if   UNIX_V
#include <sys/types.h>
#include <sys/times.h>
#include <signal.h>
#endif

#include "argacces.h"
#include "bmathfun.h"
#include "commline.h"
#include "conscomp.h"
#include "constrnt.h"
#include "constrct.h"
#include "cstrcpsr.h"
#include "emathfun.h"
#include "envrnmnt.h"
#include "filecom.h"
#include "iofun.h"
#include "memalloc.h"
#include "miscfun.h"
#include "multifld.h"
#include "multifun.h"
#include "parsefun.h"
#include "prccode.h"
#include "prdctfun.h"
#include "proflfun.h"
#include "prcdrfun.h"
#include "router.h"
#include "sortfun.h"
#include "strngfun.h"
#include "textpro.h"
#include "utility.h"
#include "watch.h"

#include "sysdep.h"

#if DEFFACTS_CONSTRUCT
#include "dffctdef.h"
#endif

#if DEFRULE_CONSTRUCT
#include "ruledef.h"
#endif

#if DEFGENERIC_CONSTRUCT
#include "genrccom.h"
#endif

#if DEFFUNCTION_CONSTRUCT
#include "dffnxfun.h"
#endif

#if DEFGLOBAL_CONSTRUCT
#include "globldef.h"
#endif

#if DEFTEMPLATE_CONSTRUCT
#include "tmpltdef.h"
#endif

#if OBJECT_SYSTEM
#include "classini.h"
#endif

#include "moduldef.h"

#if EMACS_EDITOR
#include "ed.h"
#endif

#if DEVELOPER
#include "developr.h"
#endif

/***************/
/* DEFINITIONS */
/***************/

#define NO_SWITCH         0
#define BATCH_SWITCH      1
#define BATCH_STAR_SWITCH 2
#define LOAD_SWITCH       3

/********************/
/* ENVIRONMENT DATA */
/********************/

#define SYSTEM_DEPENDENT_DATA 58

struct systemDependentData
  { 
   void (*RedrawScreenFunction)(void *);
   void (*PauseEnvFunction)(void *);
   void (*ContinueEnvFunction)(void *,int);
#if ! WINDOW_INTERFACE
#if IBM_TBC
   void interrupt (*OldCtrlC)(void);
   void interrupt (*OldBreak)(void);
#endif
#if IBM_MSC
   void (interrupt *OldCtrlC)(void);
   void (interrupt *OldBreak)(void);
#endif
#if   IBM_ICB
#pragma interrupt (OldCtrlC,OldBreak)
   void (*OldCtrlC)(void);
   void (*OldBreak)(void);
#endif
#endif
/*
#if  MAC
   short int BinaryRefNum;
#endif
*/
#if IBM_TBC || IBM_MSC || IBM_ICB /* || IBM_MCW || IBM_ZTC */
   int BinaryFileHandle;
#endif
#if (! IBM_TBC) && (! IBM_MSC) && (! IBM_ICB) /*  && (! IBM_MCW) && (! IBM_ZTC) */
   FILE *BinaryFP;
#endif
  };

#define SystemDependentData(theEnv) ((struct systemDependentData *) GetEnvironmentData(theEnv,SYSTEM_DEPENDENT_DATA))

/****************************************/
/* GLOBAL EXTERNAL FUNCTION DEFINITIONS */
/****************************************/

   extern void                    UserFunctions(void);
   extern void                    EnvUserFunctions(void *);

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static void                    InitializeSystemDependentData(void *);
   static void                    SystemFunctionDefinitions(void *);
   static void                    InitializeKeywords(void *);
   static void                    InitializeNonportableFeatures(void *);
#if   (VAX_VMS || UNIX_V || UNIX_7 || IBM_GCC) && (! WINDOW_INTERFACE)
   static void                    CatchCtrlC(int);
#endif
#if   (IBM_TBC || IBM_MSC) && (! WINDOW_INTERFACE)
   static void interrupt          CatchCtrlC(void);
   static void                    RestoreInterruptVectors(void);
#endif
#if   IBM_ICB && (! WINDOW_INTERFACE)
#pragma interrupt (CatchCtrlC)
   static void                    CatchCtrlC(void);
   static void                    RestoreInterruptVectors(void);
#endif
#if   (IBM_ZTC || IBM_SC) && (! WINDOW_INTERFACE)
   static void _cdecl             CatchCtrlC(void);
#endif
#if MAC && (! WINDOW_INTERFACE)
   static void                    CallSystemTask(void);
#endif

/********************************************************/
/* InitializeSystemDependentData: Allocates environment */
/*    data for system dependent routines.               */
/********************************************************/
static void InitializeSystemDependentData(
  void *theEnv)
  {
   AllocateEnvironmentData(theEnv,SYSTEM_DEPENDENT_DATA,sizeof(struct systemDependentData),NULL);
  }

/**************************************************/
/* InitializeEnvironment: Performs initialization */
/*   of the KB environment.                       */
/**************************************************/
#if (! ENVIRONMENT_API_ONLY) && ALLOW_ENVIRONMENT_GLOBALS
globle void InitializeEnvironment()
   {
    if (GetCurrentEnvironment() == NULL)
      { CreateEnvironment(); }
   }
#endif

/*****************************************************/
/* EnvInitializeEnvironment: Performs initialization */
/*   of the KB environment.                          */
/*****************************************************/
globle void EnvInitializeEnvironment(
  void *vtheEnvironment)
  {
   struct environmentData *theEnvironment = (struct environmentData *) vtheEnvironment;
   
   /*================================================*/
   /* Don't allow the initialization to occur twice. */
   /*================================================*/

   if (theEnvironment->initialized) return;
     
   /*================================*/
   /* Initialize the memory manager. */
   /*================================*/

   InitializeMemory(theEnvironment);

   /*===================================================*/
   /* Initialize environment data for various features. */
   /*===================================================*/
   
   InitializeCommandLineData(theEnvironment);
#if CONSTRUCT_COMPILER && (! RUN_TIME)
   InitializeConstructCompilerData(theEnvironment);
#endif
   InitializeConstructData(theEnvironment);
   InitializeEvaluationData(theEnvironment);
   InitializeExternalFunctionData(theEnvironment);
   InitializeMultifieldData(theEnvironment);
   InitializePrettyPrintData(theEnvironment);
   InitializePrintUtilityData(theEnvironment);
   InitializeScannerData(theEnvironment);
   InitializeSystemDependentData(theEnvironment);
   InitializeUserDataData(theEnvironment);
   InitializeUtilityData(theEnvironment);
#if DEBUGGING_FUNCTIONS
   InitializeWatchData(theEnvironment);
#endif
   
   /*===============================================*/
   /* Initialize the hash tables for atomic values. */
   /*===============================================*/

   InitializeAtomTables(theEnvironment);

   /*=========================================*/
   /* Initialize file and string I/O routers. */
   /*=========================================*/

   InitializeDefaultRouters(theEnvironment);

   /*=========================================================*/
   /* Initialize some system dependent features such as time. */
   /*=========================================================*/

   InitializeNonportableFeatures(theEnvironment);

   /*=============================================*/
   /* Register system and user defined functions. */
   /*=============================================*/

   SystemFunctionDefinitions(theEnvironment);
   UserFunctions();
   EnvUserFunctions(theEnvironment);

   /*====================================*/
   /* Initialize the constraint manager. */
   /*====================================*/

   InitializeConstraints(theEnvironment);

   /*==========================================*/
   /* Initialize the expression hash table and */
   /* pointers to specific functions.          */
   /*==========================================*/

   InitExpressionData(theEnvironment);

   /*===================================*/
   /* Initialize the construct manager. */
   /*===================================*/

#if ! RUN_TIME
   InitializeConstructs(theEnvironment);
#endif

   /*=====================================*/
   /* Initialize the defmodule construct. */
   /*=====================================*/

   AllocateDefmoduleGlobals(theEnvironment);

   /*===================================*/
   /* Initialize the defrule construct. */
   /*===================================*/

#if DEFRULE_CONSTRUCT
   InitializeDefrules(theEnvironment);
#endif

   /*====================================*/
   /* Initialize the deffacts construct. */
   /*====================================*/

#if DEFFACTS_CONSTRUCT
   InitializeDeffacts(theEnvironment);
#endif

   /*=====================================================*/
   /* Initialize the defgeneric and defmethod constructs. */
   /*=====================================================*/

#if DEFGENERIC_CONSTRUCT
   SetupGenericFunctions(theEnvironment);
#endif

   /*=======================================*/
   /* Initialize the deffunction construct. */
   /*=======================================*/

#if DEFFUNCTION_CONSTRUCT
   SetupDeffunctions(theEnvironment);
#endif

   /*=====================================*/
   /* Initialize the defglobal construct. */
   /*=====================================*/

#if DEFGLOBAL_CONSTRUCT
   InitializeDefglobals(theEnvironment);
#endif

   /*=======================================*/
   /* Initialize the deftemplate construct. */
   /*=======================================*/

#if DEFTEMPLATE_CONSTRUCT
   InitializeDeftemplates(theEnvironment);
#endif

   /*=============================*/
   /* Initialize COOL constructs. */
   /*=============================*/

#if OBJECT_SYSTEM
   SetupObjectSystem(theEnvironment);
#endif

   /*=====================================*/
   /* Initialize the defmodule construct. */
   /*=====================================*/

   InitializeDefmodules(theEnvironment);

   /*======================================================*/
   /* Register commands and functions for development use. */
   /*======================================================*/

#if DEVELOPER
   DeveloperCommands(theEnvironment);
#endif

   /*=========================================*/
   /* Install the special function primitives */
   /* used by procedural code in constructs.  */
   /*=========================================*/

   InstallProcedurePrimitives(theEnvironment);

   /*==============================================*/
   /* Install keywords in the symbol table so that */
   /* they are available for command completion.   */
   /*==============================================*/

   InitializeKeywords(theEnvironment);

   /*========================*/
   /* Issue a clear command. */
   /*========================*/
   
   EnvClear(theEnvironment);

   /*=============================*/
   /* Initialization is complete. */
   /*=============================*/

   theEnvironment->initialized = TRUE;
  }

/******************************************************/
/* SetRedrawFunction: Sets the redraw screen function */
/*   for use with a user interface that may be        */
/*   overwritten by execution of a command.           */
/******************************************************/
globle void SetRedrawFunction(
  void *theEnv,
  void (*theFunction)(void *))
  {
   SystemDependentData(theEnv)->RedrawScreenFunction = theFunction;
  }

/******************************************************/
/* SetPauseEnvFunction: Set the normal state function */
/*   which puts terminal in a normal state.           */
/******************************************************/
globle void SetPauseEnvFunction(
  void *theEnv,
  void (*theFunction)(void *))
  {
   SystemDependentData(theEnv)->PauseEnvFunction = theFunction;
  }

/*********************************************************/
/* SetContinueEnvFunction: Sets the continue environment */
/*   function which returns the terminal to a special    */
/*   screen interface state.                             */
/*********************************************************/
globle void SetContinueEnvFunction(
  void *theEnv,
  void (*theFunction)(void *,int))
  {
   SystemDependentData(theEnv)->ContinueEnvFunction = theFunction;
  }

/*******************************************************/
/* GetRedrawFunction: Gets the redraw screen function. */
/*******************************************************/
globle void (*GetRedrawFunction(void *theEnv))(void *)
  {
   return SystemDependentData(theEnv)->RedrawScreenFunction;
  }

/*****************************************************/
/* GetPauseEnvFunction: Gets the normal state function. */
/*****************************************************/
globle void (*GetPauseEnvFunction(void *theEnv))(void *)
  {
   return SystemDependentData(theEnv)->PauseEnvFunction;
  }

/*********************************************/
/* GetContinueEnvFunction: Gets the continue */
/*   environment function.                   */
/*********************************************/
globle void (*GetContinueEnvFunction(void *theEnv))(void *,int)
  {
   return SystemDependentData(theEnv)->ContinueEnvFunction;
  }

/*************************************************/
/* RerouteStdin: Processes the -f, -f2, and -l   */
/*   options available on machines which support */
/*   argc and arv command line options.          */
/*************************************************/
globle void RerouteStdin(
  void *theEnv,
  int argc,
  char *argv[])
  {
   int i;
   int theSwitch = NO_SWITCH;

   /*======================================*/
   /* If there aren't enough arguments for */
   /* the -f argument, then return.        */
   /*======================================*/

   if (argc < 3)
     { return; }

   /*=====================================*/
   /* If argv was not passed then return. */
   /*=====================================*/

   if (argv == NULL) return;

   /*=============================================*/
   /* Process each of the command line arguments. */
   /*=============================================*/

   for (i = 1 ; i < argc ; i++)
     {
      if (strcmp(argv[i],"-f") == 0) theSwitch = BATCH_SWITCH;
#if ! RUN_TIME
      else if (strcmp(argv[i],"-f2") == 0) theSwitch = BATCH_STAR_SWITCH;
      else if (strcmp(argv[i],"-l") == 0) theSwitch = LOAD_SWITCH;
#endif
      else if (theSwitch == NO_SWITCH)
        {
         PrintErrorID(theEnv,"SYSDEP",2,FALSE);
         EnvPrintRouter(theEnv,WERROR,"Invalid option\n");
        }

      if (i > (argc-1))
        {
         PrintErrorID(theEnv,"SYSDEP",1,FALSE);
         EnvPrintRouter(theEnv,WERROR,"No file found for ");

         switch(theSwitch)
           {
            case BATCH_SWITCH:
               EnvPrintRouter(theEnv,WERROR,"-f");
               break;

            case BATCH_STAR_SWITCH:
               EnvPrintRouter(theEnv,WERROR,"-f2");
               break;

            case LOAD_SWITCH:
               EnvPrintRouter(theEnv,WERROR,"-l");
           }

         EnvPrintRouter(theEnv,WERROR," option\n");
         return;
        }

      switch(theSwitch)
        {
         case BATCH_SWITCH:
            OpenBatch(theEnv,argv[++i],TRUE);
            break;

#if (! RUN_TIME) && (! BLOAD_ONLY)
         case BATCH_STAR_SWITCH:
            EnvBatchStar(theEnv,argv[++i]);
            break;

         case LOAD_SWITCH:
            EnvLoad(theEnv,argv[++i]);
            break;
#endif
        }
     }
  }

/**************************************************/
/* SystemFunctionDefinitions: Sets up definitions */
/*   of system defined functions.                 */
/**************************************************/
static void SystemFunctionDefinitions(
  void *theEnv)
  {
   ProceduralFunctionDefinitions(theEnv);
   MiscFunctionDefinitions(theEnv);

#if BASIC_IO || EXT_IO
   IOFunctionDefinitions(theEnv);
#endif

   PredicateFunctionDefinitions(theEnv);
   BasicMathFunctionDefinitions(theEnv);
   FileCommandDefinitions(theEnv);
   SortFunctionDefinitions(theEnv);

#if DEBUGGING_FUNCTIONS
   WatchFunctionDefinitions(theEnv);
#endif

#if MULTIFIELD_FUNCTIONS
   MultifieldFunctionDefinitions(theEnv);
#endif

#if STRING_FUNCTIONS
   StringFunctionDefinitions(theEnv);
#endif

#if EX_MATH
   ExtendedMathFunctionDefinitions(theEnv);
#endif

#if TEXTPRO_FUNCTIONS || HELP_FUNCTIONS
   HelpFunctionDefinitions(theEnv);
#endif

#if EMACS_EDITOR
   EditorFunctionDefinition(theEnv);
#endif

#if CONSTRUCT_COMPILER && (! RUN_TIME)
   ConstructsToCCommandDefinition(theEnv);
#endif

#if PROFILING_FUNCTIONS
   ConstructProfilingFunctionDefinitions(theEnv);
#endif

   ParseFunctionDefinitions(theEnv);
  }
  
/*********************************************************/
/* gentime: A function to return a floating point number */
/*   which indicates the present time. Used internally   */
/*   for timing rule firings and debugging.              */
/*********************************************************/
globle double gentime()
  {
#if   MAC
   UnsignedWide result;

   Microseconds(&result);

   return(((((double) result.hi) * kTwoPower32) + result.lo) / 1000000.0);
#endif

#if IBM_MCW
   unsigned long int result;

   result = GetTickCount();

   return((double) result / 1000.0);
#endif

#if   IBM_TBC && (! WINDOW_INTERFACE)
   unsigned long int result;

   result = biostime(0,(long int) 0);

   return((double) result / 18.2);
#endif

#if GENERIC || (! MAC)
   return((double) clock() / (double) CLOCKS_PER_SEC);
#endif
  }

/*****************************************************/
/* gensystem: Generic routine for passing a string   */
/*   representing a command to the operating system. */
/*****************************************************/
globle void gensystem(
  void *theEnv)
  {
   char *commandBuffer = NULL;
   int bufferPosition = 0;
   unsigned bufferMaximum = 0;
   int numa, i;
   DATA_OBJECT tempValue;
   char *theString;

   /*===========================================*/
   /* Check for the corret number of arguments. */
   /*===========================================*/

   if ((numa = EnvArgCountCheck(theEnv,"system",AT_LEAST,1)) == -1) return;

   /*============================================================*/
   /* Concatenate the arguments together to form a single string */
   /* containing the command to be sent to the operating system. */
   /*============================================================*/

   for (i = 1 ; i <= numa; i++)
     {
      EnvRtnUnknown(theEnv,i,&tempValue);
      if ((GetType(tempValue) != STRING) &&
          (GetType(tempValue) != SYMBOL))
        {
         SetHaltExecution(theEnv,TRUE);
         SetEvaluationError(theEnv,TRUE);
         ExpectedTypeError2(theEnv,"system",i);
         return;
        }

     theString = DOToString(tempValue);

     commandBuffer = AppendToString(theEnv,theString,commandBuffer,&bufferPosition,&bufferMaximum);
    }

   if (commandBuffer == NULL) return;

   /*=======================================*/
   /* Execute the operating system command. */
   /*=======================================*/

#if VAX_VMS
   if (SystemDependentData(theEnv)->PauseEnvFunction != NULL) (*SystemDependentData(theEnv)->PauseEnvFunction)(theEnv);
   VMSSystem(commandBuffer);
   putchar('\n');
   if (SystemDependentData(theEnv)->ContinueEnvFunction != NULL) (*SystemDependentData(theEnv)->ContinueEnvFunction)(theEnv,1);
   if (SystemDependentData(theEnv)->RedrawScreenFunction != NULL) (*SystemDependentData(theEnv)->RedrawScreenFunction)(theEnv);
#endif

#if   UNIX_7 || UNIX_V || IBM_MSC || IBM_TBC || IBM_ICB || IBM_ZTC || IBM_SC || IBM_MCW || IBM_GCC
   if (SystemDependentData(theEnv)->PauseEnvFunction != NULL) (*SystemDependentData(theEnv)->PauseEnvFunction)(theEnv);
   system(commandBuffer);
   if (SystemDependentData(theEnv)->ContinueEnvFunction != NULL) (*SystemDependentData(theEnv)->ContinueEnvFunction)(theEnv,1);
   if (SystemDependentData(theEnv)->RedrawScreenFunction != NULL) (*SystemDependentData(theEnv)->RedrawScreenFunction)(theEnv);
#else

#if ! VAX_VMS
   EnvPrintRouter(theEnv,WDIALOG,
            "System function not fully defined for this system.\n");
#endif

#endif

   /*==================================================*/
   /* Return the string buffer containing the command. */
   /*==================================================*/

   rm(theEnv,commandBuffer,bufferMaximum);

   return;
  }

#if   VAX_VMS
/*************************************************/
/* VMSSystem: Implements system command for VMS. */
/*************************************************/
globle void VMSSystem(
  char *cmd)
  {
   long status, complcode;
   struct dsc$descriptor_s cmd_desc;

   cmd_desc.dsc$w_length = strlen(cmd);
   cmd_desc.dsc$a_pointer = cmd;
   cmd_desc.dsc$b_class = DSC$K_CLASS_S;
   cmd_desc.dsc$b_dtype = DSC$K_DTYPE_T;

   status = LIB$SPAWN(&cmd_desc,0,0,0,0,0,&complcode,0,0,0);
  }

#endif

/***********************************************************/
/* InitializeNonportableFeatures: Initializes non-portable */
/*   features. Currently, the only non-portable feature    */
/*   requiring initialization is the interrupt handler     */
/*   which allows execution to be halted.                  */
/***********************************************************/
#if IBM_TBC
#pragma argsused
#endif
static void InitializeNonportableFeatures(
  void *theEnv)
  {
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#endif
#if ! WINDOW_INTERFACE

#if MAC
   AddPeriodicFunction("systemtask",CallSystemTask,0);
#endif

#if VAX_VMS || UNIX_V || UNIX_7 || IBM_GCC
   signal(SIGINT,CatchCtrlC);
#endif

#if IBM_TBC
   SystemDependentData(theEnv)->OldCtrlC = getvect(0x23);
   SystemDependentData(theEnv)->OldBreak = getvect(0x1b);
   setvect(0x23,CatchCtrlC);
   setvect(0x1b,CatchCtrlC);
   atexit(RestoreInterruptVectors);
#endif

#if IBM_MSC || IBM_ICB
   SystemDependentData(theEnv)->OldCtrlC = _dos_getvect(0x23);
   SystemDependentData(theEnv)->OldBreak = _dos_getvect(0x1b);
   _dos_setvect(0x23,CatchCtrlC);
   _dos_setvect(0x1b,CatchCtrlC);
   atexit(RestoreInterruptVectors);
#endif

#if IBM_ZTC || IBM_SC
   _controlc_handler = CatchCtrlC;
   controlc_open();
#endif

#endif
  }

/*************************************************************/
/* Functions For Handling Control C Interrupt: The following */
/*   functions handle interrupt processing for several       */
/*   machines. For the Macintosh control-c is not handle,    */
/*   but a function is provided to call periodically which   */
/*   calls SystemTask (allowing periodic tasks to be handled */
/*   by the operating system).                               */
/*************************************************************/

#if ! WINDOW_INTERFACE

#if MAC
/************************************************************/
/* CallSystemTask: Macintosh specific function which allows */
/*   periodic tasks to be handled by the operating system.  */
/************************************************************/
static void CallSystemTask()
  {
   static unsigned long int lastCall;

   if (TickCount() < (lastCall + 10)) return;
   SystemTask();
   lastCall = TickCount();
   return;
  }
#endif

#if   VAX_VMS || UNIX_V || UNIX_7 || IBM_GCC
/**********************************************/
/* CatchCtrlC: VMS and UNIX specific function */
/*   to allow control-c interrupts.           */
/**********************************************/
static void CatchCtrlC(
  int sgnl)
  {
#if ALLOW_ENVIRONMENT_GLOBALS
   SetHaltExecution(GetCurrentEnvironment(),TRUE);
   CloseAllBatchSources(GetCurrentEnvironment());
#endif
   signal(SIGINT,CatchCtrlC);
  }
#endif

#if   IBM_TBC || IBM_MSC
/******************************************************/
/* CatchCtrlC: IBM Microsoft C and Borland Turbo C    */
/*   specific function to allow control-c interrupts. */
/******************************************************/
static void interrupt CatchCtrlC()
  {
#if ALLOW_ENVIRONMENT_GLOBALS
   SetHaltExecution(GetCurrentEnvironment(),TRUE);
   CloseAllBatchSources(GetCurrentEnvironment());
#endif
  }

/**************************************************************/
/* RestoreInterruptVectors: IBM Microsoft C and Borland Turbo */
/*   C specific function for restoring interrupt vectors.     */
/**************************************************************/
static void RestoreInterruptVectors()
  {
#if ALLOW_ENVIRONMENT_GLOBALS
   void *theEnv;
   
   theEnv = GetCurrentEnvironment();

#if IBM_TBC
   setvect(0x23,SystemDependentData(theEnv)->OldCtrlC);
   setvect(0x1b,SystemDependentData(theEnv)->OldBreak);
#else
   _dos_setvect(0x23,SystemDependentData(theEnv)->OldCtrlC);
   _dos_setvect(0x1b,SystemDependentData(theEnv)->OldBreak);
#endif
#endif
  }
#endif

#if IBM_ZTC || IBM_SC
/***********************************************/
/* CatchCtrlC: IBM Zortech C specific function */
/*   to allow control-c interrupts.            */
/***********************************************/
static void _cdecl CatchCtrlC()
  {
#if ALLOW_ENVIRONMENT_GLOBALS
   SetHaltExecution(GetCurrentEnvironment(),TRUE);
   CloseAllBatchSources(GetCurrentEnvironment());
#endif
  }
#endif

#if   IBM_ICB
/*************************************************/
/* CatchCtrlC: IBM Intel C Code Builder specific */
/*   function to allow control-c interrupts.     */
/*************************************************/
static void CatchCtrlC()
  {
#if ALLOW_ENVIRONMENT_GLOBALS
   _XSTACK *sf;                        /* Real-mode interrupt handler stack frame. */

   sf = (_XSTACK *) _get_stk_frame();  /* Get pointer to V86 _XSTACK frame. */
   SetHaltExecution(GetCurrentEnvironment(),TRUE);             /* Terminate execution and */
   CloseAllBatchSources(GetCurrentEnvironment());             /* return to the command prompt.        */
   sf->opts |= _STK_NOINT;             /* Set _ST_NOINT to prevent V86 call. */
#endif
  }

/********************************************************/
/* RestoreInterruptVectors: IBM Intel C Code Builder    */
/*   specific function for restoring interrupt vectors. */
/********************************************************/
static void RestoreInterruptVectors()
  {
#if ALLOW_ENVIRONMENT_GLOBALS
   void *theEnv;
   
   theEnv = GetCurrentEnvironment();

   _dos_setvect(0x23,SystemDependentData(theEnv)->OldCtrlC);
   _dos_setvect(0x1b,SystemDependentData(theEnv)->OldBreak);
#endif
  }
#endif

#endif

/**************************************/
/* GENEXIT:  A generic exit function. */
/**************************************/
globle void genexit(
  int num)
  {
   exit(num);
  }

/******************************************************/
/* genrand: Generic random number generator function. */
/******************************************************/
int genrand()
  {
   return(rand());
  }

/**********************************************************************/
/* genseed: Generic function for seeding the random number generator. */
/**********************************************************************/
globle void genseed(
  int seed)
  {
   srand((unsigned) seed);
  }

/*********************************************/
/* gengetcwd: Generic function for returning */
/*   the current directory.                  */
/*********************************************/
#if IBM_TBC
#pragma argsused
#endif
globle char *gengetcwd(
  char *buffer,
  int buflength)
  {
#if MAC_MCW || IBM_MCW || MAC_XCD
   return(getcwd(buffer,buflength));
#endif

   if (buffer != NULL)
     { buffer[0] = 0; }
   return(buffer);
  }

/****************************************************/
/* genremove: Generic function for removing a file. */
/****************************************************/
globle int genremove(
  char *fileName)
  {
   if (remove(fileName)) return(FALSE);

   return(TRUE);
  }

/****************************************************/
/* genrename: Generic function for renaming a file. */
/****************************************************/
globle int genrename(
  char *oldFileName,
  char *newFileName)
  {
   if (rename(oldFileName,newFileName)) return(FALSE);

   return(TRUE);
  }

/*********************************************/
/* GenOpen: Trap routine for opening a file. */
/*********************************************/
globle FILE *GenOpen(
  char *fileName,
  char *accessType)
  {
   if (strlen(fileName) > FILENAME_MAX)
     { return(NULL); }

   return(fopen(fileName,accessType));
  }
  
/**********************************************/
/* GenClose: Trap routine for closing a file. */
/**********************************************/
globle int GenClose(
  FILE *theFile)
  {
   return(fclose(theFile));
  }
  
/************************************************************/
/* GenOpenReadBinary: Generic and machine specific code for */
/*   opening a file for binary access. Only one file may be */
/*   open at a time when using this function since the file */
/*   pointer is stored in a global variable.                */
/************************************************************/
globle int GenOpenReadBinary(
  void *theEnv,
  char *funcName,
  char *fileName)
  {
/*
#if  MAC
   Str255 tempName;
   OSErr resultCode;
   Str255 volName;
   short int vRefNum;
   long vDirNum;
   FSSpec theFSSpec;
   
   resultCode = HGetVol(volName,&vRefNum,&vDirNum);

   if (resultCode != noErr)
     {
      OpenErrorMessage(theEnv,funcName,fileName);
      return(0);
     }
   strcpy((char *) tempName,fileName);

   CopyCStringToPascal((char *) tempName,tempName);

   resultCode = FSMakeFSSpec(vRefNum,vDirNum,tempName,&theFSSpec);
   
   if (resultCode != noErr)
     {
      OpenErrorMessage(theEnv,funcName,fileName);
      return(FALSE);
     }
   
   resultCode = FSpOpenDF(&theFSSpec,fsCurPerm,&SystemDependentData(theEnv)->BinaryRefNum);

   if (resultCode != noErr)
     {
      OpenErrorMessage(theEnv,funcName,fileName);
      return(FALSE);
     }

#endif
*/
#if IBM_TBC || IBM_MSC || IBM_ICB

   SystemDependentData(theEnv)->BinaryFileHandle = open(fileName,O_RDONLY | O_BINARY);
   if (SystemDependentData(theEnv)->BinaryFileHandle == -1)
     {
      OpenErrorMessage(theEnv,funcName,fileName);
      return(FALSE);
     }
#endif

#if (! IBM_TBC) && (! IBM_MSC) && (! IBM_ICB)

   if ((SystemDependentData(theEnv)->BinaryFP = fopen(fileName,"rb")) == NULL)
     {
      OpenErrorMessage(theEnv,funcName,fileName);
      return(FALSE);
     }
#endif

   return(TRUE);
  }

/***********************************************/
/* GenReadBinary: Generic and machine specific */
/*   code for reading from a file.             */
/***********************************************/
globle void GenReadBinary(
  void *theEnv,
  void *dataPtr,
  unsigned long size)
  {
/*
#if MAC
   long dataSize;

   dataSize = (long) size;
   FSRead(SystemDependentData(theEnv)->BinaryRefNum,&dataSize,dataPtr);
#endif
*/
#if IBM_TBC || IBM_MSC || IBM_ICB /* || IBM_MCW */
   char *tempPtr;

   tempPtr = (char *) dataPtr;
   while (size > INT_MAX)
     {
      read(SystemDependentData(theEnv)->BinaryFileHandle,tempPtr,INT_MAX);
      size -= INT_MAX;
      tempPtr = tempPtr + INT_MAX;
     }

   if (size > 0) 
     { read(SystemDependentData(theEnv)->BinaryFileHandle,tempPtr,(STD_SIZE) size); }
#endif

#if (! IBM_TBC) && (! IBM_MSC) && (! IBM_ICB) /* && (! IBM_MCW) && (! IBM_ZTC) */
   unsigned int temp, number_of_reads, read_size;

   if (sizeof(int) == sizeof(long))
     { read_size = size; }
   else
     { read_size = (1L << (sizeof(int) * 8L)) - 1L ; }
   number_of_reads = size / read_size;
   temp = size - ((long) number_of_reads * (long) read_size);

   while (number_of_reads > 0)
     {
      fread(dataPtr,(STD_SIZE) read_size,1,SystemDependentData(theEnv)->BinaryFP); 
      dataPtr = ((char *) dataPtr) + read_size;
      number_of_reads--;
     }

   fread(dataPtr,(STD_SIZE) temp,1,SystemDependentData(theEnv)->BinaryFP); 
#endif
  }

/***************************************************/
/* GetSeekCurBinary:  Generic and machine specific */
/*   code for seeking a position in a file.        */
/***************************************************/
globle void GetSeekCurBinary(
  void *theEnv,
  long offset)
  {
/*
#if  MAC
   SetFPos(SystemDependentData(theEnv)->BinaryRefNum,fsFromMark,offset);
#endif
*/
#if IBM_TBC || IBM_MSC || IBM_ICB /* || IBM_MCW || IBM_ZTC */
   lseek(SystemDependentData(theEnv)->BinaryFileHandle,offset,SEEK_CUR);
#endif

#if (! IBM_TBC) && (! IBM_MSC) && (! IBM_ICB) /* && (! IBM_MCW) && (! IBM_ZTC) */
   fseek(SystemDependentData(theEnv)->BinaryFP,offset,SEEK_CUR);
#endif
  }
  
/***************************************************/
/* GetSeekSetBinary:  Generic and machine specific */
/*   code for seeking a position in a file.        */
/***************************************************/
globle void GetSeekSetBinary(
  void *theEnv,
  long offset)
  {
/*
#if  MAC
   SetFPos(SystemDependentData(theEnv)->BinaryRefNum,fsFromStart,offset);
#endif
*/
#if IBM_TBC || IBM_MSC || IBM_ICB /* || IBM_MCW || IBM_ZTC */
   lseek(SystemDependentData(theEnv)->BinaryFileHandle,offset,SEEK_SET);
#endif

#if (! IBM_TBC) && (! IBM_MSC) && (! IBM_ICB) /* && (! IBM_MCW) && (! IBM_ZTC) */
   fseek(SystemDependentData(theEnv)->BinaryFP,offset,SEEK_SET);
#endif
  }

/************************************************/
/* GenTellBinary:  Generic and machine specific */
/*   code for telling a position in a file.     */
/************************************************/
globle void GenTellBinary(
  void *theEnv,
  long *offset)
  {
/*
#if  MAC
   GetFPos(SystemDependentData(theEnv)->BinaryRefNum,offset);
#endif
*/
#if IBM_TBC || IBM_MSC || IBM_ICB /* || IBM_MCW || IBM_ZTC */
   *offset = lseek(SystemDependentData(theEnv)->BinaryFileHandle,0,SEEK_CUR);
#endif

#if (! IBM_TBC) && (! IBM_MSC) && (! IBM_ICB) /* && (! IBM_MCW) && (! IBM_ZTC) */
   *offset = ftell(SystemDependentData(theEnv)->BinaryFP);
#endif
  }

/****************************************/
/* GenCloseBinary:  Generic and machine */
/*   specific code for closing a file.  */
/****************************************/
globle void GenCloseBinary(
  void *theEnv)
  {
/*
#if  MAC
   FSClose(SystemDependentData(theEnv)->BinaryRefNum);
#endif
*/
#if IBM_TBC || IBM_MSC || IBM_ICB /* || IBM_MCW */
   close(SystemDependentData(theEnv)->BinaryFileHandle);
#endif

#if (! IBM_TBC) && (! IBM_MSC) && (! IBM_ICB) /* && (! IBM_MCW) */
   fclose(SystemDependentData(theEnv)->BinaryFP);
#endif
  }
  
/***********************************************/
/* GenWrite: Generic routine for writing to a  */
/*   file. No machine specific code as of yet. */
/***********************************************/
globle void GenWrite(
  void *dataPtr,
  unsigned long size,
  FILE *fp)
  {
   if (size == 0) return;
#if UNIX_7
   fwrite(dataPtr,(STD_SIZE) size,1,fp);
#else
   fwrite(dataPtr,(STD_SIZE) size,1,fp);
#endif
  }

/*********************************************/
/* InitializeKeywords: Adds key words to the */
/*   symbol table so that they are available */
/*   for command completion.                 */
/*********************************************/
static void InitializeKeywords(
  void *theEnv)
  {
#if (! RUN_TIME) && WINDOW_INTERFACE
   void *ts;

   /*====================*/
   /* construct keywords */
   /*====================*/

   ts = EnvAddSymbol(theEnv,"defrule");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"defglobal");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"deftemplate");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"deffacts");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"deffunction");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"defmethod");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"defgeneric");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"defclass");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"defmessage-handler");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"definstances");
   IncrementSymbolCount(ts);

   /*=======================*/
   /* set-strategy keywords */
   /*=======================*/

   ts = EnvAddSymbol(theEnv,"depth");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"breadth");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"lex");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"mea");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"simplicity");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"complexity");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"random");
   IncrementSymbolCount(ts);

   /*==================================*/
   /* set-salience-evaluation keywords */
   /*==================================*/

   ts = EnvAddSymbol(theEnv,"when-defined");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"when-activated");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"every-cycle");
   IncrementSymbolCount(ts);

   /*======================*/
   /* deftemplate keywords */
   /*======================*/

   ts = EnvAddSymbol(theEnv,"field");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"multifield");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"default");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"type");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"allowed-symbols");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"allowed-strings");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"allowed-numbers");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"allowed-integers");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"allowed-floats");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"allowed-values");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"min-number-of-elements");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"max-number-of-elements");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"NONE");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"VARIABLE");
   IncrementSymbolCount(ts);

   /*==================*/
   /* defrule keywords */
   /*==================*/

   ts = EnvAddSymbol(theEnv,"declare");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"salience");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"test");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"or");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"and");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"not");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"logical");
   IncrementSymbolCount(ts);

   /*===============*/
   /* COOL keywords */
   /*===============*/

   ts = EnvAddSymbol(theEnv,"is-a");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"role");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"abstract");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"concrete");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"pattern-match");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"reactive");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"non-reactive");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"slot");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"field");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"multiple");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"single");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"storage");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"shared");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"local");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"access");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"read");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"write");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"read-only");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"read-write");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"initialize-only");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"propagation");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"inherit");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"no-inherit");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"source");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"composite");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"exclusive");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"allowed-lexemes");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"allowed-instances");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"around");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"before");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"primary");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"after");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"of");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"self");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"visibility");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"override-message");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"private");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"public");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"create-accessor");
   IncrementSymbolCount(ts);

   /*================*/
   /* watch keywords */
   /*================*/

   ts = EnvAddSymbol(theEnv,"compilations");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"deffunctions");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"globals");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"rules");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"activations");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"statistics");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"facts");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"generic-functions");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"methods");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"instances");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"slots");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"messages");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"message-handlers");
   IncrementSymbolCount(ts);
   ts = EnvAddSymbol(theEnv,"focus");
   IncrementSymbolCount(ts);
#else
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#endif
#endif
  }


