/*
clipsMFC.cpp : implementation file for a CLIPS Wrapper class using MFC

Notes:			Allows encapsulation of the 'C' interface for CLIPS. Also adds
				enhanced parameter/error checking, and simplifies the
				housekeeping that is needed in the main application as a lot of
				this is handled by class instance variables. It should also be
				relatively easy to expand this class to cover more of the
				CLIPS interface than I initially used.
			**  there are a lot of definitions in the first portion of the 
			    header file, please review these settings and adjust to suit the
				needs of YOUR project.

Author(s):	Mark Tomlinson, Michael Giordano
			

Version:		3.1

Revision	WHO		Date  		WHY
--------------------------------------------------
1.1			mdt		5/4/96		instance & dynaload
1.5         mdt     6/19/97		Added Build()
1.6			mdt	    9/29/97		added createfact
1.7			mjg		1/21/98		added ODBC Query
1.8         mjg     1/29/98		added UserDefined router support
1.9         mdt     1/31/98		minor bug fixes, mods to UserDefined routers   
2.0			mdt/mjg 2/21/98		minor bug in WriteFactSlot, added UserFunctions
2.2			mdt		7/15/98		Major overhaul of route buffers
2.3         sj      5/12/99     AddResetFunction()
3.0			mdt		5/16/99		First major overhaul in many moons
								fixed many little problems and
								cleaned up code (such as using CLIPS
								pointer validation routines), all
								references to external CString* converted
								to CString &  and added the following:
								PeriodicCleanup()
3.1         mdt     5/18/03     Updated to compile clean with VC++ 7. A version 
								of stdafx.h is required in the directory with the 
								wrappers to pick up the MFC references.

*/
#include "stdafx.h"
#ifndef _DEBUG
#pragma optimize("",off)
#pragma optimize("q",off)
#endif

#define NON_VERBOSE_LOG 1
#define DEF_CB_PRIORITY 1


//stream i/o
#include <iostream>
#include <fstream>
//#include <direct.h>

//#ifndef __AFX_H__
//	#include <afx.h>
//#endif
#ifndef __AFXCOLL_H__
	#include <afxcoll.h>
#endif

#define __defc
//default loop count
int		Defcount;
//running flag
bool	Running;
//error log file
char	ErrorLog[_MAX_PATH] = "";

//macro used to check GPA() return values
#define CHKFNPTR(p,s)	if (p == NULL) { \
	                    char t_buf[100]; \
						sprintf(t_buf,"Unable to locate CLIPS entry point for function: %s",s); \
						::MessageBox(NULL,t_buf,"CLIPSWrap::GetProcAddress",MB_OK); \
						return FALSE; }
//macro to load a given proc address from CLIPS DLL
#define GPA(p,t,n)	p = (t) GetProcAddress(m_hLib,n); \
				    CHKFNPTR(p,n)
/////////////////////////////////////////////////////////////////////////////
// exported functions for DLL suppport/callbacks


//include for all clips defs
extern "C"	{
#include <dynclips.h>
}
//class definition
#include <clipsmfc.h>
#ifdef USE_ODBC
	//Header files for ODBC Query
	#include <afxdb.h>
	#include "rsvarcol.h"	//Class definition for CVarRecordset
#endif

extern "C"	{
/*	some global variables */
//array of pointer to the string buffers for memory routes
//CObjArray
CStringArray*	pRoutes[NUMROUTES];
CString*	    pRteNames[NUMROUTES];
//string arrays to manage lists of file routes
CStringArray*	FileRoutes;
CStringArray*	FileNames;
}

//prototype for generic CB function which can be set to fire periodically
void far       (*CBfunctPtr)(bool);

/*
This function handles out of memory conditions for CLIPS, this is a callback
from CLIPS.
*/ 
extern "C" int DllExport ReportMemoryProblems(unsigned long p)
{
//!!!    (dl_PeriodicCleanup)(TRUE, TRUE);
	return 0;
}

/*
This function captures which routes we will handle, this is a callback
from CLIPS.
*/
extern "C" int DllExport captureQuery(char *router)
{
	/*
	If the router comes from stdout, stdin, wclips, wdisplay, ...,
  then we will accept the output.  Otherwise, pass the buck onto
  someone else. This could be done using a string container class.
	*/
	//return true if we handle this route, and false if not.
	if(IsDefaultRouter(router))	{
		return TRUE;
	}
	return (GetRouteNum(0,router) != -1);
}

/*
This function actually handles what to do with the routed
information. This is a callback from CLIPS.
*/
extern "C" int DllExport capturePrint(char far* router, char far* str)
{
 static CString message;
	CString		hold;
	CString		Route;
	int			rnum = 0, i, n = -1;

    if(strlen(ErrorLog) == 0)	{
		//get Windows directory - this is where we want to put it
		memset(ErrorLog,0,256);
		GetWindowsDirectory(ErrorLog,255);
		strcat(ErrorLog,"\\CLIPSDLL.ERR");
	}

	// Dump all routes to the file CLIPSDLL.ERR
	// if not handled in another way, in this code
	// the "other way" is to copy the string into
	// an array which was initialized elsewhere.
	// since CLIPS does not actually format the route
	// data you could add a routine to collect all the
	// output data on a given route until a CRLF was recieved
	// thus building a single string of output.
	//(ex-) (str-cat ?item1 ?item2 ?item3 ... "some text" ...)
	//      (printout wwhatever ..........)
	bool fHandled = false;

	hold = str;
	// loop in case there is more than 1 buffer/route
	while ((rnum = GetRouteNum(rnum,router)) != -1)	{
		if(pRoutes[rnum] != NULL)	{
			pRoutes[rnum]->Add(str);
			fHandled = true;
		}
		++rnum;
	}
	Route = router;
	if (FileRoutes!=NULL)	{
		for(i = 0; i < FileRoutes->GetSize(); i++)	{
			if(FileRoutes->GetAt(i) == Route)	{
				n = i;
			}
			if(n != -1)	{
				std::ofstream *ofFile;
				ofFile = new std::ofstream(FileNames->GetAt(i),std::ios::out | std::ios::app | std::ios::binary);
				long		i = 0;

				(*ofFile) << hold;
#if VERBOSE_LOG
				(*ofFile) << "\r\n";
#endif
				fHandled = true;
				delete ofFile;
		  	}
		}
	}

	if(!fHandled)	{
		// create the error log for unhandled routes
		std::ofstream *ofError;
				
		ofError = new std::ofstream(ErrorLog, std::ios::out | std::ios::app | std::ios::binary);

        //is it a EOL situation?
		if((strncmp((LPCSTR)hold,"\r\n",2)!= 0)	&&
		   (strncmp((LPCSTR)hold,"\r",1)!= 0) 	&&
		   (strncmp((LPCSTR)hold,"\n",1)!= 0))	{
            if(message.GetLength() > MBUFSIZE) {  //don't let it get too big
    			(*ofError) << message;
#if VERBOSE_LOG
	    		(*ofError) << "\r\n";
#endif
                message.Empty();
            }
            if(message.GetLength() == 0)   {
    			message += "\r\n";
	    		message += "Msg from ";
    			message += (char *)router;
                message += ": ";
            }
            message += hold;
        }
        else    {
			(*ofError) << message;
#if VERBOSE_LOG
			(*ofError) << "\r\n";
#endif
            message.Empty();
		}
		delete ofError;
	}
	return TRUE;
}	//end of extern 'c' declaration block

//return ordinal number of specified route
int GetRouteNum(int start, const char far* route)
{
	int rnum = -1;

	for(int i = start; i < NUMROUTES; i++)	{
		if(pRteNames[i] != NULL)	{
			if(pRteNames[i]->CompareNoCase(route) == 0)	{
				rnum = i;
				break;
			}
		}
	}
	return rnum;
}

//check to see if router is a system defualt router name
bool IsDefaultRouter(const char * route)
{
	if((_stricmp(route, "stdout")==0)		||
	   (_stricmp(route, "werror")==0)		||
	   (_stricmp(route, "wclips")==0)		||
	   (_stricmp(route, "wdisplay")==0)	||
	   (_stricmp(route, "wdialog")==0)	||
	   (_stricmp(route, "wtrace")==0)		||
	   (_stricmp(route, "wagenda")==0)	||
	   (_stricmp(route, "wwarning")==0))	{
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CCLIPSWrap - all class implementation functions

//constructor
//note: construction does NOT load the DLL
CCLIPSWrap::CCLIPSWrap(int count)
	: factPtr (NULL), rulePtr (NULL), modulePtr (NULL), globalPtr (NULL),
	  instancePtr (NULL), classPtr (NULL), m_hLib (NULL)
{
	int i;

	Defcount = count;
	Running = false;
	m_fClipsInit = false;
	CLEARMBUF
	for(i = 0; i < NUMROUTES; i++)	{
		pRoutes[i]   = NULL;
		pRteNames[i] = NULL;
		}
	FileRoutes	  = NULL;
	FileNames	  = NULL;       
	CBfunctPtr    = NULL;
	factPtr       = NULL;
	modulePtr     = NULL;
	rulePtr       = NULL;
	globalPtr     = NULL;
	instancePtr   = NULL;
	classPtr      = NULL;
	activationPtr = NULL;
	agendaPtr     = NULL;
	templatePtr   = NULL;
	insSlotPtr    = NULL;
	tempSlotPtr   = NULL;
	fastLoad      = NULL;
	fastSave      = NULL;
}

//destructor
//cleanup memory
CCLIPSWrap::~CCLIPSWrap(void)
{
	CBfunctPtr    = NULL;
	factPtr       = NULL;
	modulePtr     = NULL;
	rulePtr       = NULL;
	globalPtr     = NULL;
	instancePtr   = NULL;
	classPtr      = NULL;
	activationPtr = NULL;
	agendaPtr     = NULL;
	templatePtr   = NULL;
	insSlotPtr    = NULL;
	tempSlotPtr   = NULL;
	fastLoad      = NULL;
	fastSave      = NULL;
	if(FileRoutes != NULL)	{
		delete FileRoutes;
		FileRoutes = NULL;
		}
	if(FileNames != NULL)	{
		delete FileNames;
		FileNames = NULL;
		}
//the route buffers in memory will not be deleted
//since we did not allocate them in this class
	for(int i = 0; i < NUMROUTES; i++)	{
		if(pRteNames[i] != NULL)	{
			delete pRteNames[i];
			pRteNames[i] = NULL;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// virtual functions

/***************************************************************************
		Function	:	CCLIPSWrap::SetErrorLog(CString& filename)
		Author		:	Mark Tomlinson
		Desc.		:	set filename & path for error route log filename
		Returns		:	none
****************************************************************************/
void CCLIPSWrap::SetErrorLog(CString& filename)
{
	strcpy(ErrorLog,(LPCSTR)(filename));
}

/***************************************************************************
		Function	:	CCLIPSWrap::SetMsgLoopCBfn(void far (*func_ptr)(void))
		Author		:	Mark Tomlinson
		Desc.		:	set callback function which be invoked during msgpump loops
		Returns		:	none
****************************************************************************/
void CCLIPSWrap::SetMsgLoopCBfn(void far (*func_ptr)(bool))
{
	CBfunctPtr = func_ptr;
}

/****************************************************************************
		Function	: 	CCLIPSWrap::CLIPSInit(void)
		Author		: 	Mark Tomlinson
		Desc.		: 	Accessor function to CLIPS InitilizeCLIPS(), also will
						load the DLL into memory, get procedure entry points &
						initialize the default capture router.
		Returns 	: 	true on success, false if it fails.
****************************************************************************/
bool CCLIPSWrap::CLIPSInit(void)
{
	if(m_fClipsInit)
		return true;
//now decide which DLL to load...
	if(m_hLib == NULL)	{
#ifdef WIN32
		m_hLib = LoadLibrary ("clips.dll");
#else
		m_hLib = LoadLibrary ("clipsdll.dll");
#endif
	}
	if(m_hLib == NULL)
		return false;

	if(LoadDllReferences() == false)	{
		//unload the library
		FreeLibrary(m_hLib);
		m_hLib = NULL;
		return false;
	}

	//call init clips
	(dl_InitializeCLIPS)();

	//set message pump callback function to prevent a hang
	//of the system when CLIPS is doing lengthy processing
	//not exactly needed in WIN32 - essential in WIN16
#ifndef WIN32
	(dl_AddPeriodicFunction)("msgpump",MsgLoopProc,1);
#endif
	//set out of memory handler function
	(dl_SetOutOfMemoryFunction)((unsigned long)ReportMemoryProblems);

	//tell CLIPS to be quiet while loading files
	(dl_SetPrintWhileLoading)(false);

	// add router to handle output from DLL functions, (if any), although
	// you are best to assume some output so as not to overwrite the
	// windows desktop with TTY output from the DLL. Then return.
	if((dl_AddRouter)("capture", 20, (int (*)(char *))captureQuery, (int (*)(char *,char *))capturePrint, NULL, NULL, NULL))	{
		m_fClipsInit = true;
		return true;
		}
	//failed
	return false;
}

/***************************************************************************
		Function	:	CCLIPSWrap::CLIPSReset(void)
		Author		:	Mark Tomlinson
		Desc.		:	Accessor function for CLIPS Reset
		Returns		:	true on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSReset(void)
{
	INITCHK(false)
	(dl_Reset)();
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSNextFactString(string* FactString)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS GetNextFact(); places returned fact
					  into string parameter.
		Returns 	: true if it succeeded, and false on failure - or if
					  no more facts exist.
****************************************************************************/
bool CCLIPSWrap::CLIPSNextFactString(CString& FactString)
{
	INITCHK(false)
	CLEARMBUF
	factPtr= (struct fact *)(dl_GetNextFact)(factPtr);
	if(factPtr)	{  //place fact into string buffer
		(dl_GetFactPPForm)(m_buf,MAX_FACT_LEN,factPtr);
		FactString = m_buf;
		return true;
		}
	return false;
}

/****************************************************************************
		Function	:	CCLIPSWrap::CLIPSRun(int numIterations)
		Author		:	Mark Tomlinson
		Desc.		:	Accessor function for CLIPS Run(). Passes in numIterations
						parameter to CLIPS, (-1 by default).
		Returns 	:	Number of iterations performed, (Run() - return value) or
						-1 on error
****************************************************************************/
long int CCLIPSWrap::CLIPSRun(long int numIterations)
{
	long int	ret;

	INITCHK(-1L)
	Running = true;
	ret = (dl_Run)(numIterations);
	Running = false;
	if(CBfunctPtr != NULL)	{
		CBfunctPtr(false);
		}       
	return ret;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSLoad(CString& scriptFile)
		Author		: Mark Tomlinson
		Desc.		: Accessor function to CLIPS Load(), will attempt to load
					  the file specified by scriptFile parameter.
		Returns 	: Enumerated value for success or failure
****************************************************************************/
int CCLIPSWrap::CLIPSLoad(CString& scriptFile)
{
	int	retval,j,i;
	OFSTRUCT	of;

	INITCHK(READ_NOT_INIT);
	if(scriptFile.IsEmpty())	{
		retval = -2;
		}
	else	{
		SETMBUF((LPCSTR)scriptFile)
		if(OpenFile(m_buf,&of,OF_EXIST) == HFILE_ERROR)
			return BAD_LOAD_NAME;
		j = strlen(m_buf);
		//insure we convert backslash to forwardslash
		for(i = 0;i < j;i++)	{
			if(m_buf[i] == '\\')	{
				m_buf[i] = '/';
			}
		}
		retval = (dl_Load)(m_buf);
		}

	switch(retval)	{
		case 0:	{
			return READ_FAIL;
			}
		case 1:	{
			return READ_OK;
			}
		case -1:	{
			return PARSE_FAIL;
			}
		case -2:	{
			return BAD_LOAD_NAME;
			}
		}
	return READ_FAIL;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSGetNumberOfFacts(void)
		Author		: Mark Tomlinson
		Desc.		: Accessor function to CLIPS GetNumberOfFacts()
		Returns 	: The number of facts in the workspace, -1 on error
****************************************************************************/
long int CCLIPSWrap::CLIPSGetNumberOfFacts(void)
{
	INITCHK(-1L)
	return (dl_GetNumberOfFacts)();
}

/****************************************************************************
		Function	: 	CCLIPSWrap::CLIPSExit(void)
		Author		: 	Mark Tomlinson
		Desc.		: 	Accessor function to CLIPS ExitCLIPS(), this will end
						CLIPS and unload the DLL. To continue using the same
						class instance after this would require a call to
						CLIPSInit() again. [any version of the DLL dated before
						5/96 - this will also terminate the calling process]
		Returns 	: 	none
****************************************************************************/
void CCLIPSWrap::CLIPSExit(int retVal)
{
	(dl_ExitCLIPS)(retVal);
	//CLIPS may have already unloaded itself...
	FreeLibrary(m_hLib);
	m_hLib       = NULL;
	m_fClipsInit = false;
	return;
}

/****************************************************************************
		Function	: 	CCLIPSWrap::CLIPSClear(void)
		Author		: 	Mark Tomlinson
		Desc.		: 	Accessor function to CLIPS Clear(), this will clear
						CLIPS workspace.
		Returns 	: 	true on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSClear(void)
{
	INITCHK(false)
	(dl_Clear)();
	return true;
}

/****************************************************************************
		Function	: 	CCLIPSWrap::CLIPSBSave(CString& FileName)
		Author		: 	Mark Tomlinson
		Desc.		: 	Accessor function to CLIPS BSave(), this will save
						a binary image of the CLIPS environment.
		Returns 	: 	true on success, false on error
****************************************************************************/
int CCLIPSWrap::CLIPSBSave(CString& BinName)
{
	int	retval,i;

	INITCHK(SAVE_NOT_INIT);
	if(BinName.IsEmpty())	{
		retval = -2;
		}
	else	{
		SETMBUF((LPCSTR)BinName)
		//insure we convert backslash to forwardslash
		for(i = 0;i < (int)strlen(m_buf);i++)	{
			if(m_buf[i] == '\\')	{
				m_buf[i] = '/';
			}
		}
		retval = (dl_Bsave)(m_buf);
		}

	switch(retval)	{
		case 0:	{
			return SAVE_FAIL;
			}
		case 1:	{
			return SAVE_OK;
			}
		case -2:	{
			return BAD_SAVE_NAME;
			}
		}
	return SAVE_FAIL;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSBLoad(CString& BinFile)
		Author		: Mark Tomlinson
		Desc.		: Accessor function to CLIPS Bload(), will attempt to load
					  a binary image of the CLIPS environment. [with the 16 bit
					  DLL, due to a Borland compiler bug, do not attempt to Bload
					  a file in which any object set is greater than 64k]
		Returns 	: Enumerated value for success or failure
****************************************************************************/
int CCLIPSWrap::CLIPSBLoad(CString& BinFile)
{
	int			retval;
	int			i;
	OFSTRUCT	of;


	INITCHK(READ_NOT_INIT);
	if(BinFile.IsEmpty())	{
		retval = -2;
		}
	else	{
		SETMBUF((LPCSTR)BinFile)
		if(OpenFile(m_buf,&of,OF_EXIST) == HFILE_ERROR)
			return BAD_LOAD_NAME;
		//insure we convert backslash to forwardslash
		for(i = 0;i < (int)strlen(m_buf);i++)	{
			if(m_buf[i] == '\\')	{
				m_buf[i] = '/';
			}
		}
		retval = (dl_Bload)(m_buf);
		}

	switch(retval)	{
		case 0:	{
			return READ_FAIL;
			}
		case 1:	{
			return READ_OK;
			}
		case -2:	{
			return BAD_LOAD_NAME;
			}
		}
	return READ_FAIL;
}

/***************************************************************************
		Function	:	CCLIPSWrap::CLIPSRetract(void *Fact)
		Author		:	Mark Tomlinson
		Desc.		:	Accessor function for CLIPS Retract command, will retract
						a fact in the CLIPS environment.
		Returns		:	true on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSRetract(void *Fact)
{
	INITCHK(false)
	if((Fact == NULL)	&&
		(factPtr == NULL))	{
		return false;
		}
	if(Fact == NULL)	{
		return BOOLCHK((dl_Retract)(factPtr));
		}
	return BOOLCHK((dl_Retract)(Fact));
}

/***************************************************************************
		Function	:	CCLIPSWrap::CLIPSWatch(CString& Item)
		Author		:	Mark Tomlinson
		Desc.		:	Accessor function for CLIPS Watch command, will set a
						watch on Item.
		Returns		:	true on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSWatch(CString& Item)
{
	if((!m_fClipsInit) ||
	   (Item.IsEmpty()))	{
		return false;
		}
	SETMBUF((LPCSTR)Item)
	return BOOLCHK((dl_Watch)(m_buf));
}

/***************************************************************************
		Function	:	CCLIPSWrap::CLIPSUnWatch(CString& Item)
		Author		:	Mark Tomlinson
		Desc.		:	Accessor function for CLIPS Unwatch command, will remove
						a watch on Item.
		Returns		:	true on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSUnWatch(CString& Item)
{
	if((!m_fClipsInit) ||
	   (Item.IsEmpty()))	{
		return false;
		}
	SETMBUF((LPCSTR)Item)
	return BOOLCHK((dl_Unwatch)(m_buf));
}

/***************************************************************************
		Function	:	CCLIPSWrap::CLIPSMatches(void *Rule)
		Author		:	Mark Tomlinson
		Desc.		:	Accessor function for CLIPS Matches command, this appears
						to always return CLIPS_true, as the optput from this command
						is routed to WDISPLAY or WERROR. We return false if CLIPS is
						not initalized or if the Rule pointer is NULL.
		Returns		:	true on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSMatches(void *Rule)
{
	if((!m_fClipsInit)  ||
		((Rule == NULL) &&
		 (rulePtr == NULL)))	{
		return false;
		}
	if(Rule != NULL)
		return BOOLCHK((dl_Matches)(Rule, "WDISPLAY"));
	else
		return BOOLCHK((dl_Matches)(rulePtr, "WDISPLAY"));
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSSaveFacts(CString& FileName, bool Visable)
		Author		: Mark Tomlinson
		Desc.		: Accessor function to CLIPS SaveFacts(), will save
					  either LOCAL facts or VISIBLE facts based upon Visable
					  parameter
		Returns 	: Enumerated value for success or failure
****************************************************************************/
int CCLIPSWrap::CLIPSSaveFacts(CString& FileName, bool Visable)
{
	int	retval,j,i;

	INITCHK(SAVE_NOT_INIT);
	if(FileName.IsEmpty())	{
		retval = -2;
		}
	else	{
		SETMBUF((LPCSTR)FileName)
		j = strlen(m_buf);
		//insure we convert backslash to forwardslash
		for(i = 0;i < j;i++)	{
			if(m_buf[i] == '\\')	{
				m_buf[i] = '/';
			}
		}
		if(Visable)	{
			retval = (dl_SaveFacts)(m_buf,VISIBLE_SAVE,NULL);
			}
		else	{
			retval = (dl_SaveFacts)(m_buf,LOCAL_SAVE,NULL);
			}
		}

	switch(retval)	{
		case 0:	{
			return SAVE_FAIL;
			}
		case 1:	{
			return SAVE_OK;
			}
		case -2:	{
			return BAD_SAVE_NAME;
			}
		}
	return SAVE_FAIL;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSLoadFacts(CString& FileName)
		Author		: Mark Tomlinson
		Desc.		: Accessor function to CLIPS LoadFacts, will attempt to load
					  a set of Facts into CLIPS
		Returns 	: Enumerated value for success or failure
****************************************************************************/
int CCLIPSWrap::CLIPSLoadFacts(CString& FileName)
{
	int	retval,j,i;
	OFSTRUCT	of;

	INITCHK(READ_NOT_INIT);
	if(FileName.IsEmpty())	{
		retval = -2;
	}
	else	{
		SETMBUF((LPCSTR)FileName)
		if(OpenFile(m_buf,&of,OF_EXIST) == HFILE_ERROR)
			return BAD_LOAD_NAME;
		j = strlen(m_buf);
		//insure we convert backslash to forwardslash
		for(i = 0;i < j;i++)	{
			if(m_buf[i] == '\\')	{
				m_buf[i] = '/';
			}
		}
		retval = (dl_LoadFacts)(m_buf);
	}

	switch(retval)	{
		case 0:	{
			return READ_FAIL;
			}
		case 1:	{
			return READ_OK;
			}
		case -2:	{
			return BAD_LOAD_NAME;
			}
		}
	return READ_FAIL;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSNextFact(void)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS GetNextFact(); places returned fact
					  into factPtr
		Returns 	: true if it succeeded, and false on failure - or if
					  no more facts exist.
****************************************************************************/
bool CCLIPSWrap::CLIPSNextFact(void)
{
	INITCHK(false)
	factPtr = (struct fact *)(dl_GetNextFact)(factPtr);
	if(factPtr)	{
		return true;
		}
	return false;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSFactString(CString& Data)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS GetFactPPForm(); places factPtr Data
					  into string parameter.
		Returns 	: true if it succeeded, and false on failure
****************************************************************************/
bool CCLIPSWrap::CLIPSFactString(CString& Data)
{
	INITCHK(false)
	CLEARMBUF
	if(factPtr)	{  //place fact into string buffer
		(dl_GetFactPPForm)(m_buf,MAX_FACT_LEN,factPtr);
		Data = m_buf;
		return true;
		}
	return false;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSSetFactDup(int Value)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS SetFactDuplication();
		Returns 	: return value of SetFactDuplication, appears to be old
					  FactDuplication value. -1 if an error.
****************************************************************************/
int	CCLIPSWrap::CLIPSSetFactDup(int Value)
{
	INITCHK(-1)
	return (dl_SetFactDuplication)(Value);
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSGetFactDup(void)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS SetFactDuplication();
		Returns 	: return value of GetFactDuplication. -1 on error
****************************************************************************/
bool CCLIPSWrap::CLIPSGetFactDup(void)
{
	INITCHK(false)
	return BOOLCHK((dl_GetFactDuplication)());
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSDribble(CString& FileName, bool Switch)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS DribbleOn() and DribbleOff(); if Switch
					  is true it executes DribbleOn if Switch is false it
					  executes DribbleOff.
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSDribble(CString& FileName, bool Switch)
{
	INITCHK(false)
	if((Switch)	&&
		(FileName.IsEmpty()))	{
		return false;
		}
	if(Switch)	{
		SETMBUF((LPCSTR)FileName)
		return BOOLCHK((dl_DribbleOn)(m_buf));
		}
	else	{
		return BOOLCHK((dl_DribbleOff)());
		}
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSAssert(CString& String)
		Author		: Mark Tomlinson
		Desc.		: first form for CLIPSAssert();
					  Accessor for CLIPS AssertString function. pointer to the
					  asserted fact is stored in factPtr.
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSAssert(CString& String)
{
	INITCHK(false)
	if(String.IsEmpty())	{
		return false;
		}
	SETMBUF((LPCSTR)String)
	factPtr = (struct fact *)(dl_AssertString)(m_buf);
	if(factPtr == NULL)	{
		return false;
		}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSAssert(void* Fact)
		Author		: Mark Tomlinson
		Desc.		: second form for CLIPSAssert(); (third form is implied)
					  Accessor for CLIPS Assert function. pointer to the
					  asserted fact is stored in factPtr. If the Fact parameter
					  is NULL then factPtr will be asserted unless it is NULL.
		Returns 	: true on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSAssert(void* Fact)
{
	INITCHK(false)
	if(Fact == NULL)	{
		if(factPtr != NULL)	{
			factPtr = (struct fact *)(dl_Assert)(factPtr);
			}
		else	{
			return false;
			}
		}
	else	{
		factPtr = (fact *)(dl_Assert)(Fact);
		}
	if(factPtr == NULL)	{
		return false;
		}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSIncrementFactCtr(void* Fact)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS IncrementFactCount. if Fact parameter
					  is NULL then factPtr is assumed to be the fact for which
					  the count is to be incremented.
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSIncrementFactCtr(void* Fact)
{
	INITCHK(false)
	if(Fact == NULL)	{
		if(factPtr != NULL)	{
			(dl_IncrementFactCount)(factPtr);
			}
		else	{
			return false;
			}
		}
	else	{
		(dl_IncrementFactCount)(Fact);
		}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSDecrementFactCtr(void* Fact)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS DecrementFactCount. if Fact parameter
					  is NULL then factPtr is assumed to be the fact for which
					  the count is to be decremented.
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSDecrementFactCtr(void* Fact)
{
	INITCHK(false)
	if(Fact == NULL)	{
		if(factPtr != NULL)	{
			(dl_DecrementFactCount)(factPtr);
			}
		else	{
			return false;
			}
		}
	else	{
		(dl_DecrementFactCount)(Fact);
		}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSGetFactListChanged(void)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS GetFactListChanged.
		Returns 	: return value of GetFactListChanged()
****************************************************************************/
bool CCLIPSWrap::CLIPSGetFactListChanged(void)
{
	INITCHK(false)
	return BOOLCHK((dl_GetFactListChanged)());
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSSetFactListChanged(int Value)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS SetFactListChanged.
		Returns 	: true on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSSetFactListChanged(bool Value)
{
	INITCHK(false)
	(dl_SetFactListChanged)(Value);
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSGetNextInstance(int Which, void* Class)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS functions:
					  GetNextInstance()
					  GetNextInstanceInScope()
					  GetNextInstanceInClass() <if Class is NULL classPtr is used>
					  determined by Which parameter. updates instancePtr.
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSGetNextInstance(int Which, void* Class)
{
	INITCHK(false)
	if((Which == I_CLASS) &&
		(Class == NULL)	 &&
		(classPtr == NULL))	{
		return false;
		}

	switch(Which)	{
		case I_PERIOD:
			instancePtr = (struct instance *) (dl_GetNextInstance)(instancePtr);
			break;
		case I_SCOPE:
			instancePtr = (struct instance *) (dl_GetNextInstanceInScope)(instancePtr);
			break;
		case I_CLASS:	{
			if(Class == NULL)	{
				instancePtr = (struct instance *) (dl_GetNextInstanceInClass)(instancePtr, classPtr);
				}
			else	{
				instancePtr = (struct instance *) (dl_GetNextInstanceInClass)(instancePtr, Class);
				}
			}
			break;
		}
	if(instancePtr == NULL)	{
		return false;
		}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSGetInstanceClass(void* Instance)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function	GetInstanceClass():
					  updates classPtr. If Instance is NULL uses instancePtr.
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSGetInstanceClass(void* Instance)
{
	INITCHK(false)
	if((Instance == NULL)	 &&
		(instancePtr == NULL))	{
		return false;
	}
	if(Instance == NULL)	{
		classPtr = (struct defclass *) (dl_GetInstanceClass)(instancePtr);
	}
	else	{
		if(CLIPSValidInstance(Instance))	{
			classPtr = (struct defclass *) (dl_GetInstanceClass)(Instance);
		}
		else	{
			return false;
		}
	}

	if(classPtr == NULL)	{
		return false;
		}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSGetInstanceName(CString& Data,void* Instance)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function	GetInstanceName():
					  If Instance is NULL uses instancePtr. Name is returned
					  in string class parameter Data.
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSGetInstanceName(CString& Data,void* Instance)
{
	char far *temp;

	INITCHK(false)
	if((Instance == NULL)	 &&
		(instancePtr == NULL))	{
		return false;
		}
	if(Instance == NULL)	{
		temp = (dl_GetInstanceName)(instancePtr);
		}
	else	{
		temp = (dl_GetInstanceName)(Instance);
		}
	if(temp == NULL)	{
		return false;
		}
	Data = temp;
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSGetInstanceData(CString& Data,void* Instance)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function	GetInstancePPForm():
					  If Instance is NULL uses instancePtr. Data is returned
					  in string class parameter Data.
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSGetInstanceData(CString& Data,void* Instance)
{
	INITCHK(false)
	if((Instance == NULL)	 &&
	   (instancePtr == NULL))	{
		return false;
		}
	CLEARMBUF
	if(Instance == NULL)	{
		(dl_GetInstancePPForm)(m_buf,255,instancePtr);
		}
	else	{
		(dl_GetInstancePPForm)(m_buf,255,Instance);
		}
	Data = m_buf;
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSCreateRawInstance(CString& Name)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function	CreateRawInstance():
					  uses classPtr which must already be set to desired class
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSCreateRawInstance(CString& Data)
{
	INITCHK(false)
	if(classPtr == NULL)	{
		return false;
		}
	if(Data.IsEmpty())	{
		return false;
		}
	SETMBUF((LPCSTR)Data)
	instancePtr = (struct instance *) (dl_CreateRawInstance)(classPtr,m_buf);
	if(instancePtr == NULL)	{
		return false;
		}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSValidInstance(void* Instance)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function ValidInstanceAddress():
					  uses instancePtr if Instance parameter is NULL.
		Returns 	: true if valid, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSValidInstance(void* Instance)
{
	int	retVal;

	INITCHK(false)
	if((Instance == NULL)	&&
		(instancePtr == NULL))	{
		return false;
		}
	if(Instance == NULL)	{
		retVal = (dl_ValidInstanceAddress)(instancePtr);
		}
	else	{
		retVal = (dl_ValidInstanceAddress)(Instance);
		}
	if(retVal)	{
		return true;
		}
	return false;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSSaveInstances(CString& FileName, bool Visable)
		Author		: Mark Tomlinson
		Desc.		: Accessor function to CLIPS SaveInstances(), will save
					  either LOCAL Instances or VISIBLE based upon Visable
					  parameter. ClassList is set to NULL. Inherit flag is true.
		Returns 	: number of instances saved, or -1 on error
****************************************************************************/
long int CCLIPSWrap::CLIPSSaveInstances(CString& FileName, bool Visable)
{
	long int retval;

	INITCHK(-1L)
	if(FileName.IsEmpty())	{
		return -1;
		}
	else	{
		SETMBUF((LPCSTR)FileName)
		if(Visable)	{
			retval = (dl_SaveInstances)(m_buf,VISIBLE_SAVE,NULL,true);
			}
		else	{
			retval = (dl_SaveInstances)(m_buf,LOCAL_SAVE,NULL,true);
			}
		}
	return retval;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSLoadInstances(CString& FileName)
		Author		: Mark Tomlinson
		Desc.		: Accessor function to CLIPS LoadInstances, will attempt to load
					  a set of Instances into CLIPS
		Returns 	: number of instances loaded, or -1 on error
****************************************************************************/
long int CCLIPSWrap::CLIPSLoadInstances(CString& FileName)
{
	long int	retval,j,i;
	OFSTRUCT	of;

	INITCHK(-1L)
	if(FileName.IsEmpty())	{
		return -1;
	}
	else	{
		SETMBUF((LPCSTR)FileName)
		if(OpenFile(m_buf,&of,OF_EXIST) == HFILE_ERROR)
			return BAD_LOAD_NAME;
		j = strlen(m_buf);
		//insure we convert backslash to forwardslash
		for(i = 0;i < j;i++)	{
			if(m_buf[i] == '\\')	{
				m_buf[i] = '/';
			}
		}
		retval = (dl_LoadInstances)(m_buf);
	}
	return retval;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSSetStrategy(long int data)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function	SetStrategy():
		Returns 	: old strategy value or -1 on error
****************************************************************************/
long int	CCLIPSWrap::CLIPSSetStrategy(long int data)
{
	INITCHK(-1L)
	return (dl_SetStrategy)((int)data);
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSGetStrategy(void)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function	GetStrategy():
		Returns 	: GetStrategy return value or -1 on error
****************************************************************************/
long int	CCLIPSWrap::CLIPSGetStrategy(void)
{
	INITCHK(-1L)
	return (dl_GetStrategy)();
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSAddSymbol(CString& Symbol)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function	AddSymbol():
		Returns 	: pointer to symbol, NULL on error
****************************************************************************/
void*	CCLIPSWrap::CLIPSAddSymbol(CString& Symbol)
{
	INITCHK(NULL)
	if(Symbol.IsEmpty())	{
		return NULL;
		}
	SETMBUF((LPCSTR)Symbol)
	return (dl_AddSymbol)(m_buf);
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSAddLong(long int lValue)
		Author		: Michael Giordano
		Desc.		: Accessor for CLIPS function AddLong()
		Arguments   : lValue = Value to add to Multifield
		Returns 	: Pointer to new long int Multifield Value
****************************************************************************/
void *CCLIPSWrap::CLIPSAddLong(long int lValue)
{
	INITCHK(NULL)
	return(dl_AddLong)(lValue);
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSAddDouble(Double dValue)
		Author		: Michael Giordano
		Desc.		: Accessor for CLIPS function AddDouble()
		Arguments   : dValue = Value to add to Multifield
		Returns 	: Pointer to new double Multifield Value
****************************************************************************/
void *CCLIPSWrap::CLIPSAddDouble(double dValue)
{
	INITCHK(NULL)
	return(dl_AddDouble)(dValue);
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSMemoryUsed(void)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function	MemUsed():
		Returns 	: return value from MemUsed() or -1 on error
****************************************************************************/
long int	CCLIPSWrap::CLIPSMemoryUsed(void)
{
	INITCHK(false)
	return (dl_MemUsed)();
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSMemoryRequests(void)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function	MemRequests():
		Returns 	: return value from MemRequests() or -1 on error
****************************************************************************/
long int	CCLIPSWrap::CLIPSMemoryRequests(void)
{
	INITCHK(-1L)
	return (dl_MemRequests)();
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSGetFocus(void)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function WRGetFocus(), updates modulePtr
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSGetFocus(void)
{
	INITCHK(false)
	modulePtr = (struct defmodule *) (dl_WRGetFocus)();
	if(modulePtr == NULL)	{
		return false;
		}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSRemoveAllFacts(void)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function RemoveAllFacts(),
					  updates factPtr
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSRemoveAllFacts(void)
{
	INITCHK(false)
	(dl_RemoveAllFacts)();
	factPtr = NULL;
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSAddFactArray(CStringArray* List)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function RemoveAllFacts(),
					  updates factPtr
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::AddFactArray(CStringArray& List, int NumFacts)
{
	int		i;
	CString cTemp;

	INITCHK(false)
	if(NumFacts == 0)
		NumFacts = (int) List.GetSize();
	if((!m_fClipsInit) ||
		(!NumFacts))		{  //insure CLIPS is ready
		return false;
		}
	
	for(i = 0; i < NumFacts; i++)	{
		cTemp = List.GetAt(i);	   		//get string from array
		SETMBUF((LPCSTR)cTemp)	//copy string to buffer
		if(!CLIPSAssert(m_buf))	{  		//assert string, bail if fail
			return false;
			}
		}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::SetRouteBuffer(CStringArray* pBuffer,CString& pRoute, bool remove)
		Author		: Mark Tomlinson
		Desc.		: assigns a string arrary pointer for a route name to
					  a given array pointer, calling app is responsible for
					  all maintenance of these string arrays. (FORM 1)
		Returns 	: Old Route buffer pointer. You can use this to de-assign 
					  a route to an array also, by passing in the original
					  buffer pointer and setting the remove flag to true.
****************************************************************************/
CStringArray* CCLIPSWrap::SetRouteBuffer(CStringArray* pBuffer, CString &Route, bool remove)
{
	CStringArray* old = NULL;
	int i;

	if(remove)	{
		for(i = 0;i < NUMROUTES;i++)	{
			if(pRoutes[i] == pBuffer)	{
				old = pRoutes[i];
				pRoutes[i] = NULL;
				if(pRteNames[i] != NULL)	{
					delete pRteNames[i];
				}
				pRteNames[i] = NULL;
				break;
			}
		}
	}
	else	{
		for(i = 0; i < NUMROUTES; i++)	{
			if(pRoutes[i] == NULL)	{
				old = pRoutes[i];
				pRoutes[i]   = pBuffer;
				pRteNames[i] = new CString(Route);
				break;
			}
		}
	}
	return old;
}

/****************************************************************************
		Function	: CCLIPSWrap::SetRouteBuffer(CStringArray* pBuffer,const char *Route, bool remove)
		Author		: Mark Tomlinson
		Desc.		: assigns a string arrary pointer for a route name to 
					  a given array pointer, calling app is responsible for
					  all maintenance of these string arrays. (FORM 3)
		Returns 	: Old buffer pointer
****************************************************************************/
CStringArray* CCLIPSWrap::SetRouteBuffer(CStringArray* pBuffer, const char *Route, bool remove)
{
	return SetRouteBuffer(pBuffer, CString(Route), remove);
}

/****************************************************************************
		Function	: CCLIPSWrap::SetRouteFile(CString& RouteName,CString& FileName) form -1
		Author		: Mark Tomlinson
		Desc.		: Assigns the output from a specified route to a
					  specified filespec.
		Returns 	: true on success, false on error
****************************************************************************/
bool CCLIPSWrap::SetRouteFile(CString& Route, CString& FileName)
{
	return SetRouteFile(Route, (LPCSTR)FileName);
}

/****************************************************************************
		Function	: CCLIPSWrap::SetRouteFile(CString& RouteName,char far* FileName) form -2
		Author		: Mark Tomlinson
		Desc.		: Assigns the output from a specified route to a
					  specified filespec. this form allows 'de-routing'
					  by use of a NULL pointer for FileName
		Returns 	: true on success, false on error
****************************************************************************/
bool CCLIPSWrap::SetRouteFile(CString& Route, const char far* FileName)
{
	int	i, n = -1;

	if(FileRoutes == NULL)	{
		if(FileName != NULL)	{
			FileRoutes = new CStringArray();
			FileNames  = new CStringArray();
			if(FileRoutes == NULL)	{
				return false;
				}
			}
		}

	for(i = 0;i < FileRoutes->GetSize(); i++)	{
		if(FileRoutes->GetAt(i) == Route)	{
			n = i;
			break;
		}
	}
	//a safety check
	if(n != -1)	{
		if(FileName != NULL)	{
			(*FileNames)[n] = FileName;
			}
		else	{
			FileNames->RemoveAt(n);
			FileRoutes->RemoveAt(n);
			}
	}
	else	{
		if(FileName != NULL)	{
			FileNames->Add(FileName);
			FileRoutes->Add(Route);
			}
		else	{
			return false;
			}
		}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::GetGlobal(CString& name) 
		Author		: Mark Tomlinson
		Desc.		: Rtreive the value of a global
		Returns 	: pointer to char string containing value
****************************************************************************/
char far* CCLIPSWrap::GetGlobal(CString& name)
{
	SETMBUF((LPCSTR)name)
	globalPtr = (struct defglobal *)(dl_FindDefglobal)(m_buf);
	if(globalPtr == NULL)	{	
		return NULL;
	}
	CLEARMBUF
	(dl_GetDefglobalValueForm)(m_buf,MBUFSIZE,globalPtr);
	return (strstr(m_buf, "=") + 1);
}

/****************************************************************************
		Function	: CCLIPSWrap::GetDefglobalFloat(CString& name)
		Author		: Mark Tomlinson
		Desc.		: returns the floating point value of a named Defglobal
					  casts the return type. Be sure that you have the right
					  type defined!.
		Returns 	: cast value, or -9999.99 on error
****************************************************************************/
float	CCLIPSWrap::GetDefglobalFloat(CString& name)
{
	char*			ptc;
	float			ival = (float)-9999.99;
					
	ptc = GetGlobal(name);
	if(ptc!=NULL)	{
		++ptc;
		//read 'm in  
		ival = (float) atof(ptc);
		}
	//send it back
	return (ival);
}

/****************************************************************************
		Function	: CCLIPSWrap::GetDefglobalInt(CString& name)
		Author		: Mark Tomlinson
		Desc.		: returns the int value of a named Defglobal
					  casts the return type. Be sure that you have the right
					  type defined!.
		Returns 	: cast value, or -9999 on error
****************************************************************************/
int	CCLIPSWrap::GetDefglobalInt(CString& name)
{
	char*		ptc;
	int			ival = -9999;

	ptc = GetGlobal(name);
	if(ptc!=NULL)	{
		++ptc;
		//read 'm in
		ival = atoi(ptc);
		}
	return ival;
}

/****************************************************************************
		Function	: CCLIPSWrap::GetDefglobalLong(CString& name)
		Author		: Mark Tomlinson
		Desc.		: returns the long value of a named Defglobal
					  casts the return type. Be sure that you have the right
					  type defined!.
		Returns 	: cast value, or -999999 on error
****************************************************************************/
long	CCLIPSWrap::GetDefglobalLong(CString& name)
{
	char*			ptc;
	long int		ival = -999999L;

	ptc = GetGlobal(name);
	if(ptc!=NULL)	{
		++ptc;
		//read 'm in
		ival = atol(ptc);
		}
	return ival;
}

/****************************************************************************
		Function	: CCLIPSWrap::GetDefglobalString(CString& name)
		Author		: Mark Tomlinson
		Desc.		: returns the string value of a named Defglobal
					  casts the return type. Be sure that you have the right
					  type defined!.
		Returns 	: cast value, or NULL on error
****************************************************************************/
const char far* CCLIPSWrap::GetDefglobalString(CString& name)
{
	char *		ptc;
	CString		Temp;

	ptc = GetGlobal(name);
	if(ptc!=NULL)	{
		++ptc;
		//read 'm in
		Temp = ptc;
		}
	SETMBUF((LPCSTR)Temp)
	//send it back
	return m_buf;
}

/****************************************************************************
		Function	: CCLIPSWrap::GetDefglobalAddress(CString& name)
		Author		: Mark Tomlinson
		Desc.		: returns the address value of a named Defglobal
					  casts the return type. Be sure that you have the right
					  type defined!.
		Returns 	: cast value, or NULL on error
****************************************************************************/
void*	CCLIPSWrap::GetDefglobalAddress(CString& name)
{
	char		var[256];
	DATA_OBJECT theVar;

	INITCHK(NULL)
	memset(var,0,256);
	memset(&theVar,0,sizeof(DATA_OBJECT));
	strcpy(var,(LPCSTR)(name));	//this is necessary because CLIPS is not defined
									//as using a CONST CHAR *
	if(!(dl_GetDefglobalValue)(var,&theVar))	{
		return NULL;
		}
	return theVar.value;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSGetNextDefglobal(void)
		Author		: Mark Tomlinson
		Desc.		: updates globalPtr
		Returns 	: true  on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSGetNextDefglobal(void)
{
	INITCHK(false)
	globalPtr = (struct defglobal *) (dl_GetNextDefglobal)(globalPtr);
	if(globalPtr == NULL)
		return false;
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSFindDefglobal(CString& theVar)
		Author		: Mark Tomlinson
		Desc.		: updates globalPtr
		Returns 	: true  on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSFindDefglobal(CString& theVar)
{
	INITCHK(false)
	SETMBUF((LPCSTR)theVar)
	globalPtr = (struct defglobal *) (dl_FindDefglobal)(m_buf);
	if(globalPtr == NULL)
		return false;
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSUndefglobal(void)
		Author		: Mark Tomlinson
		Desc.		: uses globalPtr, updates globalPtr
		Returns 	: true  on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSUndefglobal(void* Defglobal)
{
	INITCHK(false)
	if(Defglobal == NULL)	{
		if((dl_Undefglobal)(globalPtr))	{
			globalPtr = NULL;
			return true;
		}
	}
	else	{
		if((dl_Undefglobal)(Defglobal))	{
			return true;
		}
	}
	return false;
}

/****************************************************************************
		Function	: CCLIPSWrap::SetConstruct(CString& theVar)
		Author		: Mark Tomlinson
		Desc.		: sets the construct:
			ex. "(defglobal ?*x* = 3)"
		Returns 	: true  on success, false on Error
****************************************************************************/
bool CCLIPSWrap::SetConstruct(CString& theVar)
{
	INITCHK(false)
	SETMBUF((LPCSTR)theVar)
	if((dl_OpenStringSource)("load-construct",m_buf,0) == 0)
		return false;
	if((dl_LoadConstructsFromLogicalName)("load-construct") == 0)	{
		(dl_CloseStringSource)("load-construct");
		return false;
		}
	(dl_CloseStringSource)("load-construct");
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::GetAllFacts(CStringArray& buffer)
		Author		: Mark Tomlinson
		Desc.		: returns all facts in buffer
		Returns 	: true on success, false on error or if no facts retrieved
****************************************************************************/
bool CCLIPSWrap::GetAllFacts(CStringArray& buffer)
{
	CString	temp;

	//reset factPtr to initial fact
	factPtr = (struct fact *) (dl_GetNextFact)(NULL);
	//start looping
	while(CLIPSNextFactString(temp))	{
		buffer.Add(temp);
		}
	if(buffer.GetSize() == 0)	{
		return false;
		}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSMemUsed(void)
		Author		: Mark Tomlinson
		Desc.		: returns amount of memory CLIPS has
		Returns 	: memused
****************************************************************************/
long int CCLIPSWrap::CLIPSMemUsed(void)
{
	return (dl_MemUsed)();
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSBatch(void)
		Author		: Mark Tomlinson
		Desc.		: access the Batch Command
		Returns 	: memused
****************************************************************************/
bool CCLIPSWrap::CLIPSBatch(CString& FileName)
{
	char	var[256];
	int i, j;

	memset(var,0,256);
	strcpy(var,(LPCSTR)(*FileName));
	j = strlen(var);
	//insure we convert backslash to forwardslash
	for(i = 0;i < j;i++)	{
		if(var[i] == '\\')	{
			var[i] = '/';
		}
	}
	return BOOLCHK((dl_Batch)(var));
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSBatchStar(void)
		Author		: Mark Tomlinson
		Desc.		: access the Batch* Command
		Returns 	: memused
****************************************************************************/
bool CCLIPSWrap::CLIPSBatchStar(CString& FileName)
{
	char	var[256];
	int i, j;
	OFSTRUCT	of;

	memset(var,0,256);
	strcpy(var,(LPCSTR)(FileName));
	j = strlen(var);
	//insure we convert backslash to forwardslash
	for(i = 0;i < j;i++)	{
		if(var[i] == '\\')	{
			var[i] = '/';
		}
	}
//	check it out...
	if(OpenFile(var,&of,OF_EXIST) == HFILE_ERROR)	{
		return BOOLCHK((dl_BatchStar)(var));
	}
	else	{
		return false;
	}
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSFreeMem(void)
		Author		: Mark Tomlinson
		Desc.		: attempts to free all of clips memory
		Returns 	: memused
****************************************************************************/
void CCLIPSWrap::CLIPSFreeMem()
{    
	 (dl_Reset)();
	 (dl_Clear)();
	 (dl_FreeAtomicValueStorage)();
	 (dl_ReleaseMem)(-1L, CLIPS_TRUE);
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSSetHaltExecution(void)
		Author		: Mark Tomlinson
		Desc.		: sets the CLIPS halt flag
		Returns 	: memused
****************************************************************************/
void CCLIPSWrap::CLIPSSetHaltExecution(bool mode)
{
	if(mode)	{
		(dl_SetHaltExecution)(CLIPS_TRUE);
	}
	else	{
		(dl_SetHaltExecution)(CLIPS_FALSE);
	}
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSMakeInstance(CString& Data)
		Author		: Mark Tomlinson
		Desc.		: creates a new COOL object, sets instancePtr to new instance
					  (ex. "(Test of MINE)", assuming a definstance of MINE)
		Returns 	: true on success, otherwise false
****************************************************************************/
bool CCLIPSWrap::CLIPSMakeInstance(CString& Data)
{
	INITCHK(false)
	SETMBUF((LPCSTR)Data)
	//attempt the MakeInstance call
	if((instancePtr = (struct instance *) (dl_MakeInstance)(m_buf)) == NULL)	{
		return false;
	}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSFindInstance(CString& Name = NULL)
		Author	    : Mark Tomlinson
		Desc.		: locates a COOL object, sets instancePtr to found obj.
					   (ex. "Test", given the above example)
		Returns 	: true on success, otherwise false
****************************************************************************/
bool CCLIPSWrap::CLIPSFindInstance(CString& Name, bool SearchImports)
{
	INITCHK(false)
	SETMBUF((LPCSTR)Name)
	if((instancePtr = (struct instance *) (dl_FindInstance)(modulePtr,m_buf,SearchImports)) == NULL)	{
		return false;
	}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSDeleteInstance()
		Author		: Mark Tomlinson
		Desc.		: deletes current instance
		Returns 	: true on success, otherwise false
****************************************************************************/
bool CCLIPSWrap::CLIPSDeleteInstance(void* Instance)
{
	bool	ret = false;

	INITCHK(false)
	if((instancePtr == NULL) && 
		(Instance == NULL))	{
		return false;
	}
	if(CLIPSValidInstance(Instance))	{
		if(Instance == NULL)	{
			ret = (dl_DeleteInstance)(instancePtr) ? true : false;
			instancePtr = NULL;
		}
		else	{
			ret = (dl_DeleteInstance)(Instance) ? true : false;
		}
	}
	return ret;
}

/****************************************************************************
		Function	: CCLIPSWrap::DeleteAllInstances()
		Author		: Mark Tomlinson
		Desc.		: deletes all instances
		Returns 	: true on success, otherwise false
****************************************************************************/
bool CCLIPSWrap::DeleteAllInstances(void)
{
	INITCHK(false)
	instancePtr = NULL;
	return BOOLCHK((dl_DeleteInstance)(NULL));
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSGetNextInstanceInClass(void)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function	GetNextInstanceInClass():
					  uses classPtr. updates instancePtr.
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSGetNextInstanceInClass(void *pClass)
{
	INITCHK(false)
//!!!
	instancePtr = (struct instance *) (dl_GetNextInstanceInClass)(classPtr,instancePtr);
	if(instancePtr == NULL)	{
		return false;
	}
	return true;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSGetInstancePPForm(CString& Data)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function	GetInstancePPForm():
					  uses instancePtr.
		Returns 	: true on success, false on error
****************************************************************************/
bool	CCLIPSWrap::CLIPSGetInstancePPForm(CString& Data)
{
	INITCHK(false)
	if(instancePtr == NULL)
		return false;
	CLEARMBUF
	(dl_GetInstancePPForm)(m_buf,1023,instancePtr);
	if(strlen(m_buf) > 0)	{
		Data = m_buf;
		return true;
		}
	return false;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSGetGlobalNumberOfInstances()
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function	GetGlobalNumberOfInstances():
		Returns 	: total number of instances, all modules
****************************************************************************/
unsigned long CCLIPSWrap::CLIPSGetGlobalNumberOfInstances(void)
{
	INITCHK(false)
	return (dl_GetGlobalNumberOfInstances)();
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSDirectGetSlot(CString& Slot, CString& Value)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function DirectGetSlot()
						(slot names are case sensitive)
		Returns 	: true on success, false on Error
****************************************************************************/
bool CCLIPSWrap::CLIPSDirectGetSlot(CString& Class, CString& Slot, CString& Value)
{

	char	slot[256];
	struct	dataObject value;
	long	temp_l = 0;
	double	temp_f = 0.0;

	memset(slot,0,256);
	INITCHK(false)
	if(!Class.IsEmpty())	{
		SETMBUF((LPCSTR)Class)
		instancePtr = (struct instance *) (dl_FindInstance)(modulePtr,m_buf,0);
	}
	if((instancePtr == NULL)	||
	   (Slot.IsEmpty()))
		return false;
	strcpy(slot,(LPCSTR)(Slot));
	(dl_DirectGetSlot)(instancePtr,slot,&value);
	switch (value.type)	{
		case INTEGER:
			temp_l = ValueToLong(GetValue(value));
			sprintf(m_buf,"%ld",temp_l);
			break;
		case FLOAT:
			temp_f = ValueToDouble(GetValue(value));
			sprintf(m_buf,"%lf",temp_f);
			break;
		case SYMBOL:
		case STRING:
		default:
			SETMBUF((char *)ValueToString(GetValue(value)))
			break;
	}
	Value = m_buf;
	if (Value.GetLength() > 0)
		return true;
	return false;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSDirectPutSlot(CString& Slot, CString& Value, int type)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function DirectPutSlot()
					  (slot names are case sensitive)
		Returns 	: true on success, false on Error
****************************************************************************/
bool CCLIPSWrap::CLIPSDirectPutSlot(CString& Class, CString& Slot, CString& Value, int type)
{
	char	slot[256];
	struct	dataObject value;
	long	temp_l = 0;
	double	temp_f = 0.0;

	memset(slot,0,256);
	INITCHK(false)
	if(!Class.IsEmpty())	{
		SETMBUF((LPCSTR)Class)
		instancePtr = (struct instance *) (dl_FindInstance)(modulePtr,m_buf,0);
	}
		
	if((instancePtr == NULL)	||
		(Slot.IsEmpty()))
		return false;
	strcpy(slot,(LPCSTR)(Slot));
	SETMBUF((LPCSTR)Value)
	SetType(value,type);
	switch(type)	{
	case INTEGER:
		sscanf(m_buf,"%ld",&temp_l);
		SetValue(value,(dl_AddLong)(temp_l));
		break;
	case FLOAT:
		sscanf(m_buf,"%lf",&temp_f);
		SetValue(value,(dl_AddDouble)(temp_f));
		break;
	case SYMBOL:
	case STRING:
		SetValue(value,(dl_AddSymbol)(m_buf));
	}
	return BOOLCHK((dl_DirectPutSlot)(instancePtr,slot,&value));
}

//not implemented
bool CCLIPSWrap::SetSlotValue(CString& Class, CString& Slot, CString& Value)
{
	return 0;
}

//not implemented
bool CCLIPSWrap::GetSlotValue(CString& Class, CString& Slot, CString& Value)
{
	return 0;
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSRouteCommand(CString& Command, 
		                                            CString& route, int do_print)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function RouteCommand()
					  takes a fully qualified CLIPS command line and executes 
					  it, just as it you entered it in the interactive CLIPS.
		Arguments   : Command = the CLIPS command to execute
		              route   = the std route for the output to be displayed on
					  do_print= set to non-zero to actually display output
		Returns 	: true on success, false on Error
****************************************************************************/
bool CCLIPSWrap::CLIPSRouteCommand(CString& Command, CString &route, int do_print)
{
	char	tbuf[MAX_ITEM_SIZE];

	SETMBUF((LPCSTR)Command)
      if(do_print)	{
		if(route.GetLength() < MAX_ITEM_SIZE)	{
			strcpy(tbuf, (LPCSTR)route);
			return BOOLCHK((dl_RouteCommand)(m_buf,1,tbuf));
		}
   	}
	else	{
		return BOOLCHK((dl_RouteCommand)(m_buf,0,NULL));
	}
	return false;
}


/****************************************************************************
		Function	: CCLIPSWrap::CLIPSBuild(CString& Command)
		Author		: Mark Tomlinson
		Desc.		: Accessor for CLIPS function Build()
		Returns 	: true on success, false on Error
****************************************************************************/
bool CCLIPSWrap::CLIPSBuild(CString& Command)
{
	SETMBUF((LPCSTR)Command)
	return BOOLCHK((dl_Build)(m_buf));
}

/****************************************************************************
		Function	: CCLIPSWrap::ReadFactSlot(CString& Slot, CString& Data)
		Author		: Mark Tomlinson
		Desc.		: Read the data from a slot on the default fact, (factPtr)
		Returns 	: true on success, false on Error
****************************************************************************/
bool CCLIPSWrap::ReadFactSlot(CString& Slot, CString& Data)
{
struct dataObject *theValue;
struct dataObject question_slot;
long   temp_l = 0;
double temp_f = 0.0;

if (factPtr) {
	SETMBUF((LPCSTR)Slot)
    if ((dl_GetFactSlot)(factPtr,m_buf,&question_slot) == 1) {
		theValue = (struct dataObject *)GetValue (question_slot);
		switch (theValue->type)	{
			case INTEGER:
				temp_l = ValueToLong(theValue);
				sprintf(m_buf,"%ld",temp_l);
				break;
			case FLOAT:
				temp_f = ValueToDouble(theValue);
				sprintf(m_buf,"%lf",temp_f);
				break;
			case SYMBOL:
			case STRING:
			default:
				SETMBUF((char *)ValueToString(theValue))
				break;
			}
		Data = m_buf;
		return 1;
		}
	}
return 0;
}

/****************************************************************************
		Function	: CCLIPSWrap::WriteFactSlot(CString& Slot, CString& Data)
		Author		: Mark Tomlinson
		Desc.		: Write a vaule to a slot on the default fact, (factPtr)
		Returns 	: true on success, false on Error
****************************************************************************/
bool CCLIPSWrap::WriteFactSlot(CString& Slot, CString& Data, int type, void *pFact)
{
	//!!!!
	DATA_OBJECT value;
	char		temps[MAX_ITEM_SIZE];
	long		temp_l = 0;
	double		temp_f = 0.0;

	if(Slot.IsEmpty()	||
	   Data.IsEmpty())
	   return false;
	memset(temps,0,MAX_ITEM_SIZE);
	strcpy(temps,(LPCSTR)(Data));
	SetType(value,type);
	switch(type)	{
	case INTEGER:
		sscanf(temps,"%ld",&temp_l);
		SetValue(value,(dl_AddLong)(temp_l));
		break;
	case FLOAT:
		sscanf(temps,"%lf",&temp_f);
		SetValue(value,(dl_AddDouble)(temp_f));
		break;
	case SYMBOL:
	case STRING:
		SetValue(value,(dl_AddSymbol)(temps));
	}
	if (factPtr) {
		SETMBUF((LPCSTR)Slot)
		if ((dl_PutFactSlot)(factPtr,m_buf,&value) == 1) {
			return true;
			}
		}
	return false;
}

/***************************************************************************
		Function	:	CLIPSFindDeftemplate(CString& name)
		Author		:	Mark Tomlinson
		Desc.		:	locates the specified deftemplate and updates
		                templatePtr.
		Returns		:	true if deftemplate found, false otherwise
****************************************************************************/
bool CCLIPSWrap::CLIPSFindDeftemplate(CString& name)
{
	SETMBUF((LPCSTR)name);
	templatePtr = (struct deftemplate *)(dl_FindDeftemplate)(m_buf);
	if(templatePtr == NULL)	{
		return false;
	}
	return true;
}

/***************************************************************************
		Function	:	CLIPSCreateFact(void* tPtr)
		Author		:	Mark Tomlinson
		Desc.		:	creates a fact using the deftemplate pointed to
		                by tPtr, (or templatePtr if parameter is NULL).
						factPtr is updated on return.
		Returns		:	true on success, false otherwise
****************************************************************************/
bool CCLIPSWrap::CLIPSCreateFact(void* tPtr)
{
	if(tPtr != NULL)	{
		factPtr = (struct fact *) (dl_CreateFact)(tPtr);
	}
	else	{
		factPtr = (struct fact *) (dl_CreateFact)(templatePtr);
	}
	if(factPtr == NULL)	{
		return false;
	}
	return true;
}

/***************************************************************************
		Function	:	AssignFactSlotDefaults(void *theFact)
		Author		:	Mark Tomlinson
		Desc.		:	Assigns default values to fact slots. If theFact
		                is NULL, uses factPtr.
		Returns		:	true on success, false otherwise
****************************************************************************/
bool CCLIPSWrap::CLIPSAssignFactSlotDefaults(void *theFact)
{
	if(theFact != NULL)	{
		return BOOLCHK((dl_AssignFactSlotDefaults)(theFact));
	}
	else	{
		return BOOLCHK((dl_AssignFactSlotDefaults)(factPtr));
	}
}


int CCLIPSWrap::Version()
{
    return (dl_VERSION)(0);
}

/***************************************************************************
		Function	:	CLIPSParseDefglobal(CString *Source)
		Author		:	Mark Tomlinson
		Desc.		:	defglobal form: (defglobal module_name ?*global_name* = value)
		Returns		:	true on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSParseDefglobal(CString &Source)
{
	int  retval;
	char name[] = "command";

	if(Source.GetLength() > 0)	{
		SETMBUF((LPCSTR)Source)
	}
	else	{
		return false;
	}
    (dl_OpenStringSource)(name,m_buf,0);
    retval = (dl_ParseDefglobal)(name);
	(dl_CloseStringSource)(name);
	return BOOLCHK(retval);
}

#if USE_ODBC
/***************************************************************************
		Function : CLIPSODBCQuery(CString& strQuery, 
								  CString& strCredentials,
								  CString& strDeftemplateName,
								  bool bImplode)
		Author   : Michael Giordano
		Desc.	 : Form 1. Function that will process an ODBC query
				   and ASSERT the resulting rows as FACTS in CLIPS.
		Arguments: strQuery           = the SQL Query for the database
		           strCredentials     = ODBC connection information
				   strDeftemplateName = the deftemplate used to create the facts
				   bImplode           = implode flag
		Returns	 : Integer Error Code
****************************************************************************/
int CCLIPSWrap::CLIPSODBCQuery(CString& strQuery, CString& strCredentials, CString& strDeftemplateName, bool bImplode)
{
	RETCODE nRetCode;	
	CDatabase m_dbODBC;
	CString strAssert;

	ASSERT(strQuery.GetLength() > 0 && strCredentials.GetLength() > 0 && strDeftemplateName.GetLength() > 0);

	//If there is no App running then terminate as we cannot guarantee the
	//	state of MFC
	if(AfxGetApp() == NULL)
	{
		return(-253);
	}

	//Attempt to Open the database.
	try
	{
		m_dbODBC.OpenEx( strCredentials, CDatabase::openReadOnly | CDatabase::noOdbcDialog );
	}

	//If there is a problem, get the return code and exit
	catch(CDBException* e)
	{
		nRetCode = e->m_nRetCode;
		e->Delete();
		return(nRetCode);
	}

	//Open and populate the recordset
	CVarRecordset rsODBC(&m_dbODBC);
	if(rsODBC.Open(rsODBC.forwardOnly, strQuery, rsODBC.readOnly) == 0)
		return(-254);

	//Assert all facts
	if(!IssueODBCQuery(rsODBC, strDeftemplateName, strAssert, bImplode))
		return(-255);
	
	//Close all recordset before closing the database.
	m_dbODBC.Close();

	return(0);
}

/***************************************************************************
		Function : CLIPSODBCQuery(CString& strQuery, 
								  CString& strCredentials,
								  CString& strDeftemplateName,
								  bool bImplode,
								  CString& strError)
		Author   : Michael Giordano
		Desc.	 : Form 2. Same as Form 1 except returns bool and uses strError
				   to report errors
		Arguments: strQuery           = the SQL Query for the database
		           strCredentials     = ODBC connection information
				   strDeftemplateName = the deftemplate used to create the facts
				   bImplode           = implode flag
				   strError           = Error String
		Returns	 : true on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSODBCQuery(CString& strQuery, CString& strCredentials, CString& strDeftemplateName, bool bImplode, CString& strError)
{
	CDatabase m_dbODBC;
	CString strAssert;

	ASSERT(strQuery.GetLength() > 0 && strCredentials.GetLength() > 0 && strDeftemplateName.GetLength() > 0);

	//If there is no App running then terminate as we cannot guarantee the
	//	state of MFC
	if(AfxGetApp() == NULL)
	{
		strError = "No initialized CWinApp Object could be found";
		return false;
	}

	//Attempt to Open the database.
	try
	{
		m_dbODBC.OpenEx( strCredentials, CDatabase::openReadOnly | CDatabase::noOdbcDialog );
	}

	//If there is a problem, get the return code and exit
	catch(CDBException* e)
	{
		strError = e->m_strStateNativeOrigin;
		strError += e->m_strError;
		e->Delete();
		return false;
	}

	//Open and populate the recordset
	CVarRecordset rsODBC(&m_dbODBC);
	try
	{
		rsODBC.Open(rsODBC.forwardOnly, strQuery, rsODBC.readOnly);
	}

	//If there is a problem, get the return code and exit
	catch(CDBException* e)
	{
		strError = e->m_strStateNativeOrigin;
		strError += e->m_strError;
		e->Delete();
		return false;
	}

	//Assert all facts
	if(!IssueODBCQuery(rsODBC, strDeftemplateName, strAssert, bImplode))
	{
		strError = "Fact assertion failed while trying to assert : ";
		strError += strAssert; 
		return false;
	}

	//Close all recordset before closing the database.
	m_dbODBC.Close();

	strError = "OK";
	return true;
}

/***************************************************************************
		Function : CLIPSODBCQuery(CString& strQuery, 
								  CDatabase& dbDataSource,
								  CString& strDeftemplateName,
								  bool bImplode,
								  CString& strError)
		Author   : Michael Giordano
		Desc.	 : Form 3. Same as Form 2 except and uses an intialized
				   CDatabase object. This allows the calling program to better
				   manage ODBC resources for better performance
		Arguments: strQuery           = the SQL Query for the database
		           dbDataSource       = Open database
				   strDeftemplateName = the deftemplate used to create the facts
				   bImplode           = implode flag
				   strError           = Error String
		Returns	 : true on success, false on error
****************************************************************************/
bool CCLIPSWrap::CLIPSODBCQuery(CString& strQuery, CDatabase& dbDataSource, CString& strDeftemplateName, bool bImplode, CString& strError)
{
	CString strAssert, strTemp;

	ASSERT(strQuery.GetLength() > 0 && dbDataSource.IsOpen != false && strDeftemplateName.GetLength() > 0);

	//If there is no App running then terminate as we cannot guarantee the
	//	state of MFC
	if(AfxGetApp() == NULL)
	{
		strError = "No initialized CWinApp Object could be found";
		return false;
	}

	//If the dbDataSource is not open, return an error message saying so
	if(dbDataSource.IsOpen() == false)
	{
		strError = "The ODBC Data Source " + dbDataSource.GetConnect() + " is not open";
		return false;
	}

	//Open and populate the recordset
	CVarRecordset rsODBC(&dbDataSource);
	try
	{
		rsODBC.Open(rsODBC.forwardOnly, strQuery, rsODBC.readOnly);
	}

	//If there is a problem, get the return code and exit
	catch(CDBException* e)
	{
		strTemp = e->m_strStateNativeOrigin;
		strTemp += e->m_strError;
		strError = strTemp;
		e->Delete();
		return false;
	}

	//Assert all facts
	if(!IssueODBCQuery(rsODBC, strDeftemplateName, strAssert, bImplode))
	{
		strError = "Fact assertion failed while trying to assert : ";
		strError += strAssert; 
		return false;
	}

	strError = "OK";
	return true;
}

/***************************************************************************
		Function : IssueODBCQuery(CVarRecordset& rsODBC
								  CString& strDeftemplateName)
		Author   : Michael Giordano
		Desc.	 : Steps through an open CVarRecordset and asserts fatcs
		Arguments: rsODBC             = Recordset containing records to be asserted
				   strDeftemplateName = the deftemplate used to create the facts
		Returns	 : true on success, false on error
****************************************************************************/
bool CCLIPSWrap::IssueODBCQuery(CVarRecordset& rsODBC, CString& strDeftemplateName, CString& strAssert, bool bImplode)
{
	int i;
	CString strTemp;

	//Process until EOF
	while(rsODBC.IsEOF() == false)
	{
		//Skip any deleted records
		if(rsODBC.IsDeleted() == true)
		{
			rsODBC.MoveNext();
			continue;
		}

		//Deftemplate setup
		strAssert = "(" + strDeftemplateName + " ";

		//Loop through columns and add only those that have a value
		for(i = 0; i < rsODBC.GetNumCols(); i++)
		{
			if(rsODBC.GetColResult(i).GetLength() > 0)
			{
				strTemp = rsODBC.GetColName(i);
				strTemp.MakeLower();

				//To Implode or not to Implode, that is the question
				if(bImplode)
				{
					strAssert += "(" + strTemp + " \"" + rsODBC.GetColResult(i) + "\")";
				}
				else
				{
					strAssert += "(" + strTemp + " " + rsODBC.GetColResult(i) + ")";
				}
			}
		}
		
		strAssert += ")";
		
		//Try to assert the fact into CLIPS, return if unsuccessful
		if(!CLIPSAssert(strAssert))
		{
   			return false;
		}

		//Get the next record
		rsODBC.MoveNext();
	}

	//Close the Recordset
	rsODBC.Close();

	return true;
}
#endif

/****************************************************************************
	Function	: AddFunction(char *szCLIPSFunctionName, char cReturnType, 
	                          int (*)(void) pFunction, char *szFunctionName, 
							  char *szParameterTypes)
	Author		: Michael Giordano
	Desc.		: Adds a User defined function
	Arguments   : char *szCLIPSFunctionName = the name of the function in the CLIPS program
				: char cReturnType = the type of parameter it returns
				: int (*)(void) pFunction = the address of the function
				: char *szFunctionName = the actual name of the function
				: char *szParameterTypes = parameter string for CLIPS
	Returns 	: true on success, false on Error
****************************************************************************/
bool CCLIPSWrap::AddFunction(const char *szCLIPSFunctionName, char cReturnType, int (*pFunction)(void), const char *szFunctionName, const char *szParameterTypes)
{
	char temp1[MAX_ITEM_SIZE];
	char temp2[MAX_ITEM_SIZE];

	//safety valve
	if ((szCLIPSFunctionName == NULL)		|| 
		(strlen(szCLIPSFunctionName) == 0)	||
		(pFunction == NULL)					||
		(szFunctionName == NULL)			||
		(strlen(szFunctionName) == 0)		||
		(szParameterTypes == NULL))	{
		return 0;
	}
	SETMBUF(szCLIPSFunctionName)
	strcpy(temp1,szFunctionName);
	strcpy(temp2,szParameterTypes);
	return BOOLCHK((dl_DefineFunction2)(m_buf,cReturnType,PTIF pFunction,temp1,temp2));
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSRtnArgCount(void)
		Author		: Michael Giordano
		Desc.		: Accessor for CLIPS function RtnArgCount()
		Arguments   : N/A
		Returns 	: The number of arguments passed to the function
****************************************************************************/
int CCLIPSWrap::CLIPSRtnArgCount(void)
{
	return (dl_RtnArgCount)();
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSArgCountCheck(CString& FunctionName,
												     int iRestriction,
												     int iCount)
		Author		: Michael Giordano
		Desc.		: Accessor for CLIPS function ArgCountCheck()
		Arguments   : FunctionName = Name of the function printed if the function
									 is not satisfied
					  iRestriction = NO_MORE_THAN, EXACTLY, AT_LEAST
					  iCount       =Number of arguments in the restriction test
		Returns 	: The number of arguments passed to the function or -1 if unsuccessful
****************************************************************************/
int CCLIPSWrap::CLIPSArgCountCheck(CString& FunctionName, int iRestriction, int iCount)
{
	if (FunctionName.GetLength() == 0)	{
		return 0;
	}
	SETMBUF((LPCSTR)FunctionName)
	return (dl_ArgCountCheck)(m_buf, iRestriction, iCount);
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSArgTypeCheck(CString& FunctionName,
												     int iPos,
												     int iType,
													 void *pDataObject)
		Author		: Michael Giordano
		Desc.		: Accessor for CLIPS function ArgTypeCheck()
		Arguments   : FunctionName = Name of the function printed if the function
									 is not satisfied
					  iPos         = The position of the parameter to be checked
					  iType        = Acceptable values are :
									 FLOAT, INTEGER, SYMBOL, STRING, MULTIFIELD,
									 EXTERNAL_ADDRESS, INSTANCE_ADDRESS, INSTANCE_NAME,
									 INTEGER_OR_FLOAT, SYMBOL_OR_STRING, INSTANCE_OR_INSTANCE_NAME
					  pDataObject  = Pointer to the unknown parameter.
		Returns 	: true if successful, false otherwise
****************************************************************************/
bool CCLIPSWrap::CLIPSArgTypeCheck(CString& FunctionName, int iPos, int iType, void *pDataObject)
{
	if ((FunctionName.GetLength() == 0)	||
		(pDataObject == NULL))	{
		return false;
	}
	SETMBUF((LPCSTR)FunctionName)
	return BOOLCHK((dl_ArgTypeCheck)(m_buf, iPos, iType, pDataObject));
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSRtnLexeme(int iPos)
		Author		: Michael Giordano
		Desc.		: Accessor for CLIPS function RtnLexeme()
		Arguments   : iPos = The position of the desired parameter
		Returns 	: Pointer from either a symbol, string, or instance name data type 
		             (NULL is returned if the type is not SYMBOL, STRING, or INSTANCE_NAME)
****************************************************************************/
char *CCLIPSWrap::CLIPSRtnLexeme(int iPos)
{
	return (dl_RtnLexeme)(iPos);
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSRtnDouble(int iPos)
		Author		: Michael Giordano
		Desc.		: Accessor for CLIPS function RtnDouble()
		Arguments   : iPos = The position of the desired parameter
		Returns 	: Floating-point value of desired parameter
****************************************************************************/
double CCLIPSWrap::CLIPSRtnDouble(int iPos)
{
	return (dl_RtnDouble)(iPos);
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSRtnLong(int iPos)
		Author		: Michael Giordano
		Desc.		: Accessor for CLIPS function RtnLong()
		Arguments   : iPos = The position of the desired parameter
		Returns 	: Long interger value of desired parameter
****************************************************************************/
long CCLIPSWrap::CLIPSRtnLong(int iPos)
{
	return (dl_RtnLong)(iPos);
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSRtnUnknown(int iPos, void *pDataObject)
		Author		: Michael Giordano
		Desc.		: Accessor for CLIPS function RtnUnknown()
		Arguments   : iPos       = The position of the desired parameter
					  DataObject = Temporary Storage spot for data
		Returns 	: Pointer to DataObject
****************************************************************************/
void *CCLIPSWrap::CLIPSRtnUnknown(int iPos, void *pDataObject)
{
	if(pDataObject == NULL)	{
		return NULL;
	}
	return (dl_RtnUnknown)(iPos, pDataObject);
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSCreateMultifield(int iSize)
		Author		: Michael Giordano
		Desc.		: Accessor for CLIPS function AddSymbol()
		Arguments   : iSize = Number of fields in Multifield
		Returns 	: Pointer to new Multifield
****************************************************************************/
void *CCLIPSWrap::CLIPSCreateMultifield(int iSize)
{
	return(dl_CreateMultifield)(iSize);
}

/****************************************************************************
		Function	: CCLIPSWrap::CLIPSSetMultifieldErrorValue(void *Multifield)
		Author		: Michael Giordano
		Desc.		: Accessor for CLIPS function AddSymbol()
		Arguments   : Multifield = Pointer to Multifield object in error
		Returns 	: void
****************************************************************************/
void CCLIPSWrap::CLIPSSetMultifieldErrorValue(void *pMultifield)
{
	if (pMultifield == NULL)	{
		return;
	}
	(dl_SetMultifieldErrorValue)(pMultifield);
}

/***************************************************************************
		Function	:	CLIPSAgenda(CString& RouteName)
		Author		:	Mark Tomlinson
		Desc.		:	Print the Agenda to a specified RouteName
		Returns		:	true if it worked...
****************************************************************************/
bool CCLIPSWrap::CLIPSAgenda(CString& RouteName)
{
//!!!
   struct defmodule *theModule = NULL;
   bool				foundRtr   = false;

   theModule = (struct defmodule *) (dl_GetCurrentModule)();
   if (theModule == NULL)	{
	   return false;
   }
//   for(int i = 0; i < UserRouters->GetSize(); i++)	{
//		if(UserRouters->GetAt(i) == RouteName)	{
//			foundRtr = true;
//			break;
//		}
//   }
   if(!foundRtr)	{
	   if(GetRouteNum(0,(LPCSTR)(RouteName)) != -1)	{
			foundRtr = true;
		}
   }
   if(!foundRtr)	{
	   return false;
   }
   CLEARMBUF
   SETMBUF((LPCSTR)RouteName)
   (dl_Agenda)(m_buf,theModule);
   CLEARMBUF
   return true;
}

/****************************************************************************
	Function	: CCLIPSWrap::CLIPSGetCurrentModule(void)
	Author		: Mark Tomlinson
	Desc.		: updates modulePtr with Current Module pointer
	Returns 	: true on success, false on Error
****************************************************************************/
bool CCLIPSWrap::CLIPSGetCurrentModule(void)
{
   modulePtr = (struct defmodule *) (dl_GetCurrentModule)();
   if(modulePtr == NULL)	{
	   return false;
   }
   else	{
	   return true;
   }
}

/****************************************************************************
	Function	: CCLIPSWrap::CLIPSGetNextActivation(void)
	Author		: Mark Tomlinson
	Desc.		: updates activationPtr
	Returns 	: true on success, false on Error
****************************************************************************/
bool CCLIPSWrap::CLIPSGetNextActivation(void* pAct)
{
	//!!!
   activationPtr = (struct activation *) (dl_GetNextActivation)(activationPtr);
   if(activationPtr == NULL)	{
	   return false;
   }
   else	{
	   return true;
   }
}

/****************************************************************************
	Function	: CCLIPSWrap::CLIPSGetNextDefclass(void)
	Author		: Mark Tomlinson
	Desc.		: updates classPtr
	Returns 	: true on success, false on Error
****************************************************************************/
bool CCLIPSWrap::CLIPSGetNextDefclass(void* pClass)
{
	//!!!
   classPtr = (struct defclass *) (dl_GetNextDefclass)(classPtr);
   if(classPtr == NULL)	{
	   return false;
   }
   else	{
	   return true;
   }
}

/****************************************************************************
	Function	: CCLIPSWrap::CLIPSSend(CString &Msg, CString &Args, CString &InsName, CString &RetStr)
	Author		: Mark Tomlinson
	Desc.		: Sends a message to a specified Instance, or to instancePtr
				  if there is no instance specified
	Returns 	: true on success, false on Error
****************************************************************************/
bool CCLIPSWrap::CLIPSSend(CString &Msg, CString &Args, CString &InsName, CString &RetStr)	
{
   DATA_OBJECT insdata, rtn;
   VOID *myInstancePtr;
   char	ArgBuf[MAX_ITEM_SIZE];


   if((InsName.GetLength() == 0) && (instancePtr != NULL))	{
	   myInstancePtr = instancePtr;
   }
   else	{
	   return false;
   }
   SETMBUF((LPCSTR)Msg)
   memset(ArgBuf,0,MAX_ITEM_SIZE);
   strcpy(ArgBuf,(LPCSTR)Args);
   SetType(insdata,INSTANCE_ADDRESS);
   SetValue(insdata,myInstancePtr);
   dl_Send(&insdata,m_buf,ArgBuf,&rtn);
   RetStr = DOToString(rtn);  
   return true;
}

/***************************************************************************
Function	: CLIPSAddResetFunction(char *szCLIPSFunctionName,
                                    void (*)(void) pFunction, 
                                    int iPriorityValue)
Author	: Steve Jernigan
Desc.		: Adds a User defined reset function
Arguments   : char *szCLIPSFunctionName = the name of the function in the
              CLIPS program
            : void (*)(void) pFunction = the address of the function
		: int iPriorityValue = Priority ov the reset item (higher first), Values
              between -2K and 2k should only be used internally by clips
Returns 	: true on success, false on Error
***************************************************************************/
bool CCLIPSWrap::CLIPSAddResetFunction(const char *szCLIPSFunctionName, void(*pFunction)(void), int iPriorityValue)
{
	//safety valve
	if ((szCLIPSFunctionName == NULL)		||
		(strlen(szCLIPSFunctionName) == 0)	||
		(pFunction == NULL))	{
		return 0;
	}
	SETMBUF(szCLIPSFunctionName)
	return BOOLCHK((dl_AddResetFunction)(m_buf,(void (__cdecl *)(void)) pFunction,iPriorityValue));
}

/****************************************************************************
	Function	: CCLIPSWrap::LoadDllReferences(void)
	Author		: Mark Tomlinson
	Desc.		: Get the procedure entry points for all CLIPS functions
				  used in this class. Uses 2 macros defined earlier. You
				  could shift to loading the entry points as needed rather
				  than taking the hit during initialization.
	Returns 	: true on success, false on Error
****************************************************************************/
bool CCLIPSWrap::LoadDllReferences()
{
	GPA(dl_InitializeCLIPS,FORM_01,"InitializeCLIPS")
	GPA(dl_AddPeriodicFunction,FORM_18,"AddPeriodicFunction")
	GPA(dl_SetOutOfMemoryFunction,FORM_31,"SetOutOfMemoryFunction")
	GPA(dl_SetPrintWhileLoading,FORM_02,"SetPrintWhileLoading")
	GPA(dl_AddRouter,ADDROUTER,"AddRouter")
	GPA(dl_DeleteRouter,FORM_11,"DeleteRouter")
	GPA(dl_ExitCLIPS,FORM_02,"ExitCLIPS")
	GPA(dl_Reset,FORM_01,"Reset")
	GPA(dl_GetFactPPForm,FORM_04,"GetFactPPForm")
	GPA(dl_GetNextFact,FORM_30,"GetNextFact")
	GPA(dl_Run,FORM_40,"Run")
	GPA(dl_Load,FORM_11,"Load")
	GPA(dl_GetNumberOfFacts,FORM_41,"GetNumberOfFacts")
	GPA(dl_Clear,FORM_01,"Clear")
	GPA(dl_Bsave,FORM_11,"Bsave")
	GPA(dl_Bload,FORM_11,"Bload")
	GPA(dl_Retract,FORM_14,"Retract")
	GPA(dl_Watch,FORM_11,"Watch")
	GPA(dl_Unwatch,FORM_11,"Unwatch")
	GPA(dl_Matches,FORM_34,"Matches")
	GPA(dl_SaveFacts,FORM_12,"SaveFacts")
	GPA(dl_LoadFacts,FORM_11,"LoadFacts")
	GPA(dl_SetFactDuplication,FORM_10,"SetFactDuplication")
	GPA(dl_GetFactDuplication,FORM_13,"GetFactDuplication")
	GPA(dl_DribbleOff,FORM_13,"DribbleOff")
	GPA(dl_DribbleOn,FORM_11,"DribbleOn")
	GPA(dl_AssertString,FORM_32,"AssertString")
	GPA(dl_Assert,FORM_30,"Assert")
	GPA(dl_IncrementFactCount,FORM_03,"IncrementFactCount")
	GPA(dl_DecrementFactCount,FORM_03,"DecrementFactCount")
	GPA(dl_GetFactListChanged,FORM_13,"GetFactListChanged")
	GPA(dl_SetFactListChanged,FORM_02,"SetFactListChanged")
	GPA(dl_GetNextInstanceInClass,FORM_33,"GetNextInstanceInClass")
	GPA(dl_GetNextInstanceInScope,FORM_30,"GetNextInstanceInScope")
	GPA(dl_GetNextInstance,FORM_30,"GetNextInstance")
	GPA(dl_GetInstanceClass,FORM_30,"GetInstanceClass")
	GPA(dl_GetInstanceName,FORM_50,"GetInstanceName")
	GPA(dl_GetInstancePPForm,FORM_04,"GetInstancePPForm")
	GPA(dl_CreateRawInstance,FORM_34,"CreateRawInstance")
	GPA(dl_ValidInstanceAddress,FORM_14,"ValidInstanceAddress")
	GPA(dl_SaveInstances,FORM_42,"SaveInstances")
	GPA(dl_LoadInstances,FORM_43,"LoadInstances")
	GPA(dl_SetStrategy,FORM_10,"SetStrategy")
	GPA(dl_GetStrategy,FORM_13,"GetStrategy")
	GPA(dl_AddLong,FORM_37,"AddLong")
	GPA(dl_AddDouble,FORM_38,"AddDouble")
	GPA(dl_AddSymbol,FORM_32,"AddSymbol")
	GPA(dl_MemUsed,FORM_41,"MemUsed")
	GPA(dl_MemRequests,FORM_41,"MemRequests")
	GPA(dl_WRGetFocus,FORM_35,"WRGetFocus")
	GPA(dl_RemoveAllFacts,FORM_01,"RemoveAllFacts")
	GPA(dl_GetDefglobalValueForm,FORM_04,"GetDefglobalValueForm")
	GPA(dl_FindDefglobal,FORM_32,"FindDefglobal")
	GPA(dl_GetDefglobalValue,FORM_17,"GetDefglobalValue")
	GPA(dl_GetNextDefglobal,FORM_30,"GetNextDefglobal")
	GPA(dl_Undefglobal,FORM_14,"Undefglobal")
	GPA(dl_CloseStringSource,FORM_11,"CloseStringSource")
	GPA(dl_LoadConstructsFromLogicalName,FORM_11,"LoadConstructsFromLogicalName")
	GPA(dl_OpenStringSource,FORM_15,"OpenStringSource")
	GPA(dl_Batch,FORM_11,"Batch")
	GPA(dl_BatchStar,FORM_11,"BatchStar")
	GPA(dl_ReleaseMem,FORM_44,"ReleaseMem")
	GPA(dl_FreeAtomicValueStorage,FORM_01,"FreeAtomicValueStorage")
	GPA(dl_SetHaltExecution,FORM_02,"SetHaltExecution")
	GPA(dl_MakeInstance,FORM_32,"MakeInstance")
	GPA(dl_FindInstance,FORM_36,"FindInstance")
	GPA(dl_DeleteInstance,FORM_14,"DeleteInstance")
	GPA(dl_GetGlobalNumberOfInstances,FORM_60,"GetGlobalNumberOfInstances")
	GPA(dl_DirectGetSlot,FORM_06,"DirectGetSlot")
	GPA(dl_DirectPutSlot,FORM_16,"DirectPutSlot")
	GPA(dl_RouteCommand,FORM_21,"RouteCommand")
	GPA(dl_GetFactSlot,FORM_16,"GetFactSlot")
	GPA(dl_PutFactSlot,FORM_16,"PutFactSlot")
	GPA(dl_Build,FORM_11,"Build")
	GPA(dl_VERSION,FORM_100,"CLIPSVERSION")
	GPA(dl_AssignFactSlotDefaults,FORM_14,"AssignFactSlotDefaults")
	GPA(dl_CreateFact,FORM_30,"CreateFact")
	GPA(dl_FindDeftemplate,FORM_32,"FindDeftemplate")
	GPA(dl_ParseDefglobal,FORM_11,"ParseDefglobal")
	GPA(dl_DefineFunction2,FORM_61,"DefineFunction2")
	GPA(dl_RtnArgCount,FORM_13,"RtnArgCount");
	GPA(dl_ArgCountCheck,FORM_19,"ArgCountCheck");
	GPA(dl_ArgRangeCheck,FORM_19,"ArgRangeCheck");
	GPA(dl_RtnLexeme,FORM_51,"RtnLexeme");
	GPA(dl_RtnDouble,FORM_70,"RtnDouble");
	GPA(dl_RtnLong,FORM_45,"RtnLong");
	GPA(dl_RtnUnknown,FORM_3A,"RtnUnknown");
	GPA(dl_ArgTypeCheck,FORM_20,"ArgTypeCheck");
	GPA(dl_AddSymbol,FORM_32,"AddSymbol");
	GPA(dl_CreateMultifield,FORM_39,"CreateMultifield");
	GPA(dl_SetMultifieldErrorValue,FORM_03,"SetMultifieldErrorValue");
	GPA(dl_Agenda,FORM_05,"Agenda");
	GPA(dl_GetCurrentModule,FORM_35,"GetCurrentModule");
	GPA(dl_GetNextActivation,FORM_30,"GetNextActivation");
	GPA(dl_GetNextDefclass,FORM_30,"GetNextDefclass");
	GPA(dl_Send,FORM_07,"Send");
	GPA(dl_AddResetFunction,FORM_18,"AddResetFunction");
	GPA(dl_PeriodicCleanup,FORM_08,"PeriodicCleanup");
	return true;	
}

#ifndef _DEBUG
#pragma optimize("",on)
#pragma optimize("q",on)
#endif

