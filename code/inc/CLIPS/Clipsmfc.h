#ifndef	_CLIPSWrap
	#define _CLIPSWrap
/////////////////////////////////////////////////////////////////////////////
// CCLIPSWrap defintion

//CLIPS core code version
#define CLIPS_BUILD_VER 610
//define number of routes to handle
#define NUMROUTES		100
//define number of user routes to handle
// #define NUM_U_ROUTES	100
//define the maximum length of a fact string
#define MAX_FACT_LEN	640
//define max length of an item
#define MAX_ITEM_SIZE	640
//define max length of a list
#define MAX_LIST_SIZE	32000
//define number of iterations between msg pump calls
#define MAX_MSG_PUMP	1
//size of temp buffer variable
#define MBUFSIZE		1024

#ifndef __AFX_H__
	#include <afx.h>
#endif
#ifndef __AFXCOLL_H__
	#include <afxcoll.h>
#endif

#include <dynclips.h>
#include <dynaload.h>

#define DllExport	__declspec( dllexport )

//set this to 1 if you want ODBC support...
//remember to include rsvarcol.cpp into yor project as well
#define USE_ODBC 0
 #ifdef USE_ODBC
	#ifndef __AFXDB_H__
		#include <afxdb.h>
	#endif
	#ifndef _RSVARCOL_H_
		#include <rsvarcol.h>
	#endif
#endif

//prototype for route ordinal function
int GetRouteNum(int start, const char far* route);
//default router check function
bool IsDefaultRouter(const char *route);

//prototype for message pump callback
extern "C"	{
void DllExport MsgLoopProc(void);
}
#ifndef __defc
extern int Defcount;
#endif

//a handy macro to check for CLIPSInit and return something on fail
#define INITCHK(p)	if(!m_fClipsInit)	 {	\
						return p;}
//macro to clear the temp buffer
#define CLEARMBUF	memset(m_buf,0,MBUFSIZE);
#define SETMBUF(s)  CLEARMBUF strcpy(m_buf,s);
#define BOOLCHK(s)  (s) ? true : false

// just in case the CLIPS headers are not in
// the preprocessor stack for this pass
#ifndef FACT_ADDRESS
	#define FACT_ADDRESS		6
#endif
#ifndef INSTANCE_ADDRESS
	#define INSTANCE_ADDRESS	7
#endif

//the wrapper class
class CCLIPSWrap
{
	//declarations for the FN pointers to CLIPS functions
	FORM_02		dl_ExitCLIPS;
	FORM_01		dl_Reset;
	FORM_04		dl_GetFactPPForm;
	FORM_30		dl_GetNextFact;
	FORM_40		dl_Run;
	FORM_11		dl_Load;
	FORM_41		dl_GetNumberOfFacts;
	FORM_01		dl_Clear;
	FORM_11		dl_BatchStar;
	FORM_11		dl_Bsave;
	FORM_11		dl_Bload;
	FORM_11     dl_Build;
	FORM_14		dl_Retract;
	FORM_11		dl_Watch;
	FORM_11		dl_Unwatch;
	FORM_34		dl_Matches;
	FORM_12		dl_SaveFacts;
	FORM_11		dl_LoadFacts;
	FORM_10		dl_SetFactDuplication;
	FORM_13		dl_GetFactDuplication;
	FORM_13		dl_DribbleOff;
	FORM_11		dl_DribbleOn;
	FORM_32		dl_AssertString;
	FORM_30		dl_Assert;
	FORM_03		dl_IncrementFactCount;
	FORM_03		dl_DecrementFactCount;
	FORM_13		dl_GetFactListChanged;
	FORM_02		dl_SetFactListChanged;
	FORM_33		dl_GetNextInstanceInClass;
	FORM_30		dl_GetNextInstanceInScope;
	FORM_30		dl_GetNextInstance;
	FORM_30		dl_GetInstanceClass;
	FORM_50		dl_GetInstanceName;
	FORM_04		dl_GetInstancePPForm;
	FORM_34		dl_CreateRawInstance;
	FORM_14		dl_ValidInstanceAddress;
	FORM_42		dl_SaveInstances;
	FORM_43		dl_LoadInstances;
	FORM_10		dl_SetStrategy;
	FORM_13		dl_GetStrategy;
	FORM_32		dl_AddSymbol;
	FORM_37		dl_AddLong;
	FORM_38		dl_AddDouble;
	FORM_41		dl_MemUsed;
	FORM_41		dl_MemRequests;
	FORM_35		dl_WRGetFocus;
	FORM_01		dl_RemoveAllFacts;
	FORM_04		dl_GetDefglobalValueForm;
	FORM_32		dl_FindDefglobal;
	FORM_17		dl_GetDefglobalValue;
	FORM_30		dl_GetNextDefglobal;
	FORM_14		dl_Undefglobal;
	FORM_11		dl_CloseStringSource;
	FORM_11		dl_LoadConstructsFromLogicalName;
	FORM_15		dl_OpenStringSource;
	FORM_11		dl_Batch;
	FORM_44		dl_ReleaseMem;
	FORM_01		dl_FreeAtomicValueStorage;
	FORM_02		dl_SetHaltExecution;
	FORM_32		dl_MakeInstance;
	FORM_36		dl_FindInstance;
	FORM_14		dl_DeleteInstance;
	FORM_60		dl_GetGlobalNumberOfInstances;
	FORM_06		dl_DirectGetSlot;
	FORM_16		dl_DirectPutSlot;
	FORM_21		dl_RouteCommand;
	FORM_16		dl_GetFactSlot;
	FORM_16		dl_PutFactSlot;
	FORM_01		dl_InitializeCLIPS;
	FORM_18		dl_AddPeriodicFunction;
	FORM_31		dl_SetOutOfMemoryFunction;
	FORM_02		dl_SetPrintWhileLoading;
	FORM_100	dl_VERSION;
	FORM_14     dl_AssignFactSlotDefaults;
	FORM_30		dl_CreateFact;
	FORM_32     dl_FindDeftemplate;
	FORM_11     dl_ParseDefglobal;
	ADDROUTER	dl_AddRouter;
	FORM_05		dl_Agenda;
	FORM_35		dl_GetCurrentModule;
	FORM_30     dl_GetNextActivation;
	FORM_30		dl_GetNextDefclass;
	FORM_07		dl_Send;
	//new 2.0
	FORM_11		dl_DeleteRouter;
	FORM_61		dl_DefineFunction2;
	FORM_13		dl_RtnArgCount;
	FORM_19		dl_ArgCountCheck;
	FORM_19		dl_ArgRangeCheck;
	FORM_51		dl_RtnLexeme;
	FORM_70		dl_RtnDouble;
	FORM_45		dl_RtnLong;
	FORM_3A		dl_RtnUnknown;
	FORM_20		dl_ArgTypeCheck;
	FORM_39		dl_CreateMultifield;
	FORM_03		dl_SetMultifieldErrorValue;
	//new 3.0
	FORM_08		dl_PeriodicCleanup;
	FORM_18     dl_AddResetFunction;


	HINSTANCE	m_hLib;

	//CLIPS exception object
	class CLIPSException	{
		CString	Why;
	public:	//construction & destruction
		CLIPSException() {
			Why = "UnKnown";
		}
		CLIPSException(CString &Text)	{
			Why = Text;
		}
		CLIPSException(const char *szText)	{
			Why = szText;
		}
		virtual ~CLIPSException() {}

	public:	//methods
		void SetWhy(const char *szText)	{
			Why = szText;
		}
		void SetWhy(CString &Text)	{
			Why = Text;
		}
		const char *GetWhy()	{
			return (LPCSTR)Why;
		}
	};

public:
	//constructor and destructor are public
	CCLIPSWrap(int count = -1);
	virtual ~CCLIPSWrap(void);

//UserFunction return types
enum UDF_ReturnValues
{
UDFReturn_External_Address = (unsigned) 'a',
UDFReturn_Boolean = (unsigned) 'b',
UDFReturn_Character = (unsigned) 'c',
UDFReturn_Double_Prec_Float = (unsigned) 'd',
UDFReturn_Single_Prec_Float = (unsigned) 'f',
UDFReturn_Integer = (unsigned) 'i',
UDFReturn_Symbol_String_or_InstanceName = (unsigned) 'j',
UDFReturn_Symbol_or_String = (unsigned) 'k',
UDFReturn_Long_Integer = (unsigned) 'l',
UDFReturn_Multifield = (unsigned) 'm',
UDFReturn_Integer_or_Float = (unsigned) 'n',
UDFReturn_Instance_Name = (unsigned) 'o',
UDFReturn_String = (unsigned) 's',
UDFReturn_Any = (unsigned) 'u',
UDFReturn_Void = (unsigned) 'v',
UDFReturn_Symbol = (unsigned) 'w',
UDFReturn_Instance_Address = (unsigned) 'x'
};

//Load file return codes
enum LoadStatus	{
	READ_FAIL,
	PARSE_FAIL,
	READ_OK,
	BAD_LOAD_NAME,
	READ_NOT_INIT,
	};

//Save file return codes
enum SaveStatus	{
	SAVE_FAIL,
	SAVE_OK,
	BAD_SAVE_NAME,
	SAVE_NOT_INIT,
	};

//class instance cases
enum InstanceCase	{
	I_PERIOD,
	I_SCOPE,
	I_CLASS
	};

// Attributes
protected:
	//CLIPS struct pointers
	//these are now (3.0) explicitly declared as the
	//correct pointer types rather than void *
	struct fact			*factPtr;
	struct defmodule	*modulePtr;
	struct defrule		*rulePtr;
	struct defglobal	*globalPtr;
	struct instance		*instancePtr;
	struct defclass		*classPtr;
	struct activation	*activationPtr;
	struct deftemplate	*templatePtr;
	struct instanceSlot *insSlotPtr;
	struct templateSlot *tempSlotPtr;
	//other internal stuff
	void* agendaPtr;
	FILE* fastLoad;
	FILE* fastSave;
	bool  m_fClipsInit;
	char  m_buf[MBUFSIZE];

// Operations
public:
	//this is our public interface to the rest of the CPP world
	void  SetMsgLoopCount(int ct)	{ Defcount = ct; }
	void  SetMsgLoopCBfn(void far (*func_ptr)(bool));
	//all of the member pointer accessors are now inline
	void  SetFactPtr(void *ptr = NULL)        { factPtr       = (struct fact *)ptr; }
	void  SetRulePtr(void *ptr = NULL)        { rulePtr       = (struct defrule*)ptr; }
	void  SetModulePtr(void *ptr = NULL)      { modulePtr     = (struct defmodule*)ptr; }
	void  SetGlobalPtr(void *ptr = NULL)      { globalPtr     = (struct defglobal*)ptr; }
	void  SetInstancePtr(void *ptr = NULL)    { instancePtr   = (struct instance*)ptr; }
	void  SetClassPtr(void *ptr = NULL)       { classPtr      = (struct defclass*)ptr; }
	void  SetAgendaPtr(void *ptr = NULL)      { agendaPtr     = ptr; }
	void  SetTemplatePtr(void *ptr = NULL)    { templatePtr   = (struct deftemplate*)ptr; }
	void  SetActivationPtr(void *ptr = NULL)  { activationPtr = (struct activation*)ptr; }
	void  SetErrorLog(CString & filename);
	void* GetFactPtr(void)       { return (void *)factPtr; }
	void* GetRulePtr(void)       { return (void *)rulePtr; }
	void* GetModulePtr(void)     { return (void *)modulePtr; }
	void* GetGlobalPtr(void)     { return (void *)globalPtr; }
	void* GetInstancePtr(void)   { return (void *)instancePtr; }
	void* GetClassPtr(void)      { return (void *)classPtr; }
	void* GetAgendaPtr(void)     { return agendaPtr; }
	void* GetActivationPtr(void) { return (void *)activationPtr; }
	void* GetTemplatePtr(void)   { return (void *)templatePtr; }
	//defglobal functions
	bool		CLIPSGetNextDefglobal(void);
	bool		CLIPSFindDefglobal(CString & theVar);
	bool		CLIPSUndefglobal(void* Defglobal = NULL);
	bool 		SetConstruct(CString &theVar);
	float		GetDefglobalFloat(CString &name);
	int			GetDefglobalInt(CString &name);
	long int	GetDefglobalLong(CString &name);
const char far*	GetDefglobalString(CString &name);
	void*		GetDefglobalAddress(CString &name);
	void		GetFactIdentifier(CString &name, void* fct);
    bool        CLIPSParseDefglobal(CString &Source);
	//fact functions
 	bool		  CLIPSFindDeftemplate(CString &name);
	bool		  GetAllFacts(CStringArray &buffer);
	bool  		  CLIPSNextFactString(CString &FactString);
	long int 	  CLIPSGetNumberOfFacts(void);
	bool		  CLIPSNextFact(void);
	bool		  CLIPSFactString(CString &Data);
	int			  CLIPSSetFactDup(int value = 0);
	bool		  CLIPSGetFactDup(void);
	bool		  CLIPSAssert(CString &String);
	bool		  CLIPSAssert(void* Fact = NULL);
	bool		  CLIPSIncrementFactCtr(void* Fact = NULL);
	bool		  CLIPSDecrementFactCtr(void* Fact = NULL);
	bool		  CLIPSGetFactListChanged(void);
	bool		  CLIPSSetFactListChanged(bool Value = false);
	bool		  AddFactArray(CStringArray& List, int NumFacts = 0);
	bool		  CLIPSCreateFact(void* tPtr = NULL);
	bool		  CLIPSAssignFactSlotDefaults(void *theFact = NULL);
	//misc
	CStringArray *SetRouteBuffer(CStringArray* pBuffer, CString &Route, bool remove = false);
	CStringArray *SetRouteBuffer(CStringArray* pBuffer, const char *pRoute, bool remove = false);
	bool		  SetRouteFile(CString& RouteName, CString& FileName);
	bool		  SetRouteFile(CString& RouteName, const char far* FileName);
	bool 		  CLIPSAgenda(CString& Routename);
	bool		  CLIPSInit(void);
	bool 		  CLIPSReset(void);
	void 		  CLIPSExit(int retVal = 1);
	long int 	  CLIPSRun(long int numIterations = -1);
	bool 		  CLIPSClear(void);
	bool		  CLIPSRetract(void *Fact = NULL);
	bool		  CLIPSWatch(CString &Item);
	bool		  CLIPSUnWatch(CString &Item);
	bool		  CLIPSMatches(void *Rule = NULL);
	bool 		  CLIPSGetCurrentModule(void);
	//file functions
	int 		  CLIPSLoad(CString &scriptFile);
	int			  CLIPSSave(CString &Filename);
	int 		  CLIPSBSave(CString &BinName);
	int 		  CLIPSBLoad(CString &BinName);
	int			  CLIPSSaveFacts(CString &FileName, bool Visable = TRUE);
	int			  CLIPSLoadFacts(CString &FileName);
	bool		  CLIPSDribble(CString &FileName, bool Switch = FALSE);
	FILE*         CLIPSSetFastSave(FILE* fp = NULL);
	FILE*         CLIPSGetFastSave(void);
	FILE*         CLIPSSetFastLoad(FILE* fp = NULL);
	FILE*         CLIPSGetFastLoad(void);
	//instance functions
	bool		  CLIPSGetNextInstance(int Which = 0,void* Class = NULL);
	bool		  CLIPSMakeInstance(CString &Data);
	void*		  GetNamedInstance(CString &Name, bool SearchImports);
	bool		  CLIPSFindInstance(CString &Name, bool SearchImports = FALSE);
	bool		  CLIPSDeleteInstance(void* Instance = NULL);
	bool 		  DeleteAllInstances(void);
	bool		  CLIPSQuashInstance(void *pIn = NULL);
	bool		  CLIPSGetInstanceClass(void* Instance = NULL);
	bool		  CLIPSGetNextInstanceInClass(void *pClass = NULL);
	bool		  CLIPSGetInstanceName(CString &Data, void* Instance = NULL);
	bool		  CLIPSGetInstanceData(CString &Data, void* Instance = NULL);
	bool		  CLIPSGetInstancePPForm(CString &Data);
	bool		  CLIPSCreateRawInstance(CString &Name);
	bool		  CLIPSValidInstance(void* Instance = NULL);
	bool		  CLIPSDirectGetSlot(CString &Class, CString &Slot, CString &Value);
	bool		  CLIPSDirectPutSlot(CString &Class, CString &Slot, CString &Value, int type = -1);
unsigned long int CLIPSGetGlobalNumberOfInstances(void);
	long int	  CLIPSLoadInstances(CString &FileName);
	long int	  CLIPSSaveInstances(CString &FileName, bool Visable = FALSE);
	bool 		  SetSlotValue(CString &Class, CString &Slot, CString &Value);
	bool 		  GetSlotValue(CString &Class, CString &Slot, CString &Value);
	bool		  ReadFactSlot(CString &Slot, CString &Data);
	bool		  WriteFactSlot(CString&Slot, CString &Data, int type, void *pFact = NULL);
	bool	 	  CLIPSRouteCommand(CString &Command, CString &route, int do_print = 1);
	bool		  DoCLIPSFunctionCall(CString &name, CString &args, CString &result);
	long int	  CLIPSGetStrategy(void);
	long int	  CLIPSSetStrategy(long int data = 0);
	void*		  CLIPSAddSymbol(CString &Symbol);
	void*		  CLIPSAddLong(long int lValue);
	void*		  CLIPSAddDouble(double dValue);
	long int	  CLIPSMemoryUsed(void);
	long int	  CLIPSMemoryRequests(void);
	bool		  CLIPSGetFocus(void);
	bool		  CLIPSRemoveAllFacts(void);
	long int	  CLIPSMemUsed(void);
	bool		  CLIPSBatch(CString &FileName);
	bool		  CLIPSBatchStar(CString &FileName);
	void		  CLIPSFreeMem(void);
	void		  CLIPSSetHaltExecution(bool);
	bool		  CLIPSGetHaltExecution(void);
    int           Version(void);
	bool		  CLIPSGetNextDefclass(void* pClass = NULL);
	bool		  CLIPSGetNextActivation(void* pAct = NULL);
	bool		  CLIPSSend(CString &Msg, CString &Args, CString &InsName, CString &RetStr);
	bool		  CLIPSSend(CString &Msg, CString &Args, void *InsP, CString &RetStr);
	bool		  CLIPSBuild(CString& Command);
	bool		  CLIPSAddResetFunction(const char *szCLIPSFunctionName, void(*pFunction)(void), int iPriorityValue);
#if USE_ODBC
	int					CLIPSODBCQuery(CString& Query, CString& Credentials, CString& DeftemplateName, bool bImplode);
	BOOL				CLIPSODBCQuery(CString& Query, CString& Credentials, CString& DeftemplateName, bool bImplode, CString& strError);
	BOOL				CLIPSODBCQuery(CString& Query, CDatabase& DataSource, CString& DeftemplateName, bool bImplode, CString& strError);
#endif
	bool				AddFunction(const char *szCLIPSFunctionName, char cReturnType, int (*pFunction)(void), const char *szFunctionName, const char *szParameterTypes);
	int					CLIPSRtnArgCount(void);
	int					CLIPSArgCountCheck(CString& FunctionName, int iRestriction, int iCount);
	bool				CLIPSArgTypeCheck(CString& FunctionName, int iPos, int iType, void *pDataObject);
	char			   *CLIPSRtnLexeme(int iPos);
	double				CLIPSRtnDouble(int iPos);
	long int			CLIPSRtnLong(int iPos);
	void			   *CLIPSRtnUnknown(int iPos, void *DataObject);
	void			   *CLIPSCreateMultifield(int iSize);
	void				CLIPSSetMultifieldErrorValue(void *pMultifield);
	bool				CLIPSExecutingConstruct(void);
	bool				CLIPSPopFocus(void);
	bool				CLIPSLoadFactsFromString(CString &theString);
	bool				CLIPSFactRelation(CString &Name, void *pFact);
	bool				CLIPSFactSlotNames(CString &Names, void *pFact);
	void               *GetFactByIndex(long Idx);
	long int			GetSlotCount(void *pFact, bool IsFact = true);
	bool				IsDribbleOn(void);
	bool				IsBatchActive(void);
	bool				CLIPSOpenStringBatch(CString &Name, CString &theStr);
	long int			CLIPSRestoreInstancesFromString(CString &theStr);
	long int			CLIPSLoadInstancesFromString(CString &theStr);
	bool				CLIPSGetIncrementalReset(void);
	bool				CLIPSSetIncrementalReset(bool value);
	bool				CLIPSGetInstancesChanged(void);
	bool				CLIPSSetInstancesChanged(bool value);
	long int			LineCount(void);
	bool				CLIPSSetGlobalsChanged(bool value);
	bool				CLIPSGetGlobalsChanged(void);
	long int			ModuleItemCount(void);
	long int			CLIPSListFocusStack(CString &theString);
	long int			GetFactIndex(void *pFact = NULL);
	//developer functions
	long int			GetFactCount(void *pFact = NULL);
	long int			GetSymbolTblCount(void);
	long int			GetIntegerTblCount(void);
	long int			GetFloatTblCount(void);
	long int			GetBitmapTblCount(void);
	bool				FactIsGarbage(void *pFact = NULL);
	long int			GetTimeTag(void *pIn = NULL, int Typ = FACT_ADDRESS);
	bool				GetItemModuleName(CString &theStr, int Typ = FACT_ADDRESS, void *pIn = NULL);
	long int			GetFactDepth(void *pFact = NULL);
	long int			GetAgendaString(CString &theString);
	const char *		GetClassName(void *pClass = NULL);
	const char *		GetModuleName(void *pModule = NULL);
	long				GetInstanceDepth(void *pInstance = NULL);
	long				GetInstanceBusyCnt(void *pInstance = NULL);
	bool				IsInstanceSync(void *pInstance = NULL);
	bool				InstanceInitInProg(void *pInstance = NULL);
	bool				InstanceSlotsInit(void *pInstance = NULL);
	bool				IsInstanceGarbage(void *pInstance = NULL);
	bool				IsInstanceInstalled(void *pInstance = NULL);
	long				GetClassBusy(void *pClass = NULL);
	long				GetClassHndlrCnt(void *pClass = NULL);
	short				GetClassID(void *pClass = NULL);
	bool				IsClassSystem(void *pClass = NULL);
	bool				IsClassInstalled(void *pClass = NULL);
	bool				IsClassReactive(void *pClass = NULL);
	bool				IsClassConcrete(void *pClass = NULL);
	void *				GetClass(CString &theClass);
	bool				FindClass(CString &theClass);
	long				GetClassSlotCount(void *pClass = NULL);
	void *				GetSlotAddress(int slotNum, bool isfact, void *factORinstance = NULL);
	void *				GetSlotAddress(CString &slotName, bool isfact, void *factORinstance = NULL);
	long				GetFactSlotCount(void *pFact = NULL);
	void *              FindFact(CString &theFact);
	void *				FindFactByIndex(long Idx);
	bool				GetFactSlotNames(CString &names, void *pFact = NULL);
	bool				FactSlotValue(CString &data, const char *slotName = NULL, void *pFact = NULL);
	void *				GetFactSlotName(CString &name, long slotNum, void *pFact = NULL);
	void *				FindFactSlot(CString &name, void *pFact = NULL);
	bool				GetDefglobalVariant(VARIANT &value, CString &name);
	bool				GetDefglobalVariant(VARIANT &value, void *pGlobal = NULL);
	bool				SetDefglobalVariant(const VARIANT &value, void *pGlobal = NULL);
	int					GetDefglobalType(CString &name);
	int					GetDefglobalType(void *pGlobal = NULL);
	bool				GetDefglobaName(CString &name, void *pGlobal = NULL);
	bool				FactSlotVariant(VARIANT &data, const char *slotName = NULL, void *pFact = NULL);
	long				GetInstanceSlotConstraints(void *pSlot);
	void *				GetInstanceSlot(CString &name, void *pInstance = NULL);
	void *				GetInstanceSlot(long SlotNum, void *pInstance = NULL);
	bool				GetInstanceSlotName(CString &name, void *pSlot = NULL);
	long				GetInstanceSlotType(void *pSlot = NULL);
	bool				GetTemplateName(CString &Name, void *pTemplate = NULL);
	bool				GetTemplateModuleName(CString &Name, void *pTemplate = NULL);
	long                CLIPSNumberOfDefmodules(long num = -1);
	bool				SetModuleFocus(CString &ModName);
	bool				GetMatches(void *rule, CString &RouteName);
	bool				GetInstanceSlotValue(VARIANT &va, CString &SlotName, void *pInst = NULL);
	bool				SetInstanceSlotValue(const VARIANT &va, CString &SlotName, void *pInst = NULL);
    bool				GetInstanceSlotShared(void *pSlot = NULL);
    bool				GetInstanceSlotComposite(void *pSlot = NULL);
    bool				GetInstanceSlotInheritable(void *pSlot = NULL);
    bool				GetInstanceSlotReadOnly(void *pSlot = NULL);
    bool				GetInstanceSlotReactive(void *pSlot = NULL);
	bool				GetInstanceSlotPublic(void *pSlot = NULL);
	bool				GetInstanceSlotNoDefault(void *pSlot = NULL);
	bool				GetInstanceSlotMultiple(void *pSlot = NULL);
	bool				GetInstanceSlotInitOnly(void *pSlot = NULL);
	long				GetTemplateSlotConstraints(void *pSlot = NULL);
	long				GetTemplateSlotCount(void *pTemplate = NULL);
	bool				GetTemplateImplied(void *pTemplate = NULL);
	bool				GetTemplateInScope(void *pTemplate);
	bool                GetTemplateSlotMulti(void *pSlot);
	bool                GetTemplateSlotNoDefault(void *pSlot);
	struct deftemplate *GetFactTemplate(void *pFact = NULL);
	bool				IsSuperClass(void *superClass, void *baseClass = NULL);
	bool				IsSuperClass(CString &superClass, CString &baseClass);
	bool				IsSubClass(void *subClass, void *baseClass = NULL);
	bool				IsSubClass(CString &subClass, CString &baseClass);
	void				AddMessageBox(void);
private:
	char *    GetGlobal(CString & name);
	bool	  LoadDllReferences(void);
	void	  GetFactName(void* fct);
//!!! - remove this pragma when VC++ supports this syntax
#pragma warning( disable : 4290 )
	struct instance       *ValidateInstancePtr(void *pIns) throw (CLIPSException);
	struct slotDescriptor *ValidateInstanceSlotDescriptor(void *pSlot) throw (CLIPSException);
	struct deftemplate    *ValidateTemplatePtr(void *pTemplate) throw (CLIPSException);
	struct defglobal      *ValidateGlobalPtr(void *pGlobal) throw (CLIPSException);
	struct defrule        *ValidateRulePtr(void *rule) throw (CLIPSException);
	struct templateSlot   *ValidateTemplateSlot(void *pSlot) throw (CLIPSException);
	struct fact			  *ValidateFactPtr(void *pFact) throw (CLIPSException);
	struct defclass       *ValidateClassPtr(void *pClass) throw (CLIPSException);
#if USE_ODBC
	bool	  IssueODBCQuery(CVarRecordset& rsODBC, CString& strDeftemplateName, CString& strFact, bool bImplode);
#endif

};

/////////////////////////////////////////////////////////////////////////////
#endif
