#define N_IMPLEMENTS nTclServer
//--------------------------------------------------------------------
//  ntclrun.cc -- Kommandos auswerten und weiterleiten
//
//  (C) 1998 Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

#include "kernel/nfileserver2.h"
#include "script/ntclserver.h"
#include "kernel/narg.h"
//--------------------------------------------------------------------
/**
    - 23-Feb-99   floh    created
*/
//--------------------------------------------------------------------
char *nTclServer::Prompt(char *buf, int size)
{
    char buffer[1024];
    const char* res;
    buffer[0] = 0;
    this->Run("psel", res);
    strcat(buffer, res);
    strcat(buffer, ">");
    n_strncpy2(buf, buffer, size);
    return buf;
}

//--------------------------------------------------------------------
/**
    - 10-Oct-98   floh    created
    - 03-Feb-00   floh    + now uses Tcl_EvalEx()
    - 14-Oct-01   floh    + checks error code
*/
//--------------------------------------------------------------------
bool nTclServer::Run(const char *cmd_str, const char*& result)
{
    result = 0;
    int errCode = Tcl_EvalEx(this->interp, (char *) cmd_str, -1, TCL_EVAL_DIRECT);
    Tcl_Obj *res = Tcl_GetObjResult(interp);
    result = Tcl_GetString(res);
    if (errCode == TCL_ERROR) {
        n_printf("*** Tcl error '%s'\n", Tcl_GetString(res));
        if (this->GetFailOnError()) {
            n_error("Aborting.");
        }
        return false;
    }
    return true;
}
Tcl_Obj * NArg2TclObj(nArg *a) 
{
    switch (a->GetType()) {
        case nArg::ARGTYPE_VOID:
            return Tcl_NewStringObj("(null)", 6);
        case nArg::ARGTYPE_INT:
            return Tcl_NewIntObj(a->GetI());
        case nArg::ARGTYPE_FLOAT:
            return Tcl_NewDoubleObj(a->GetF());
        case nArg::ARGTYPE_STRING:
            return Tcl_NewStringObj(a->GetS(), -1);
        case nArg::ARGTYPE_BOOL:
            return Tcl_NewBooleanObj(a->GetB());
        case nArg::ARGTYPE_OBJECT:
            return (Tcl_Obj *)a->GetO();
        case nArg::ARGTYPE_CODE:
            return Tcl_NewStringObj(a->GetC(), -1);
    }
    return NULL;
}

bool nTclServer::RunCommand(nCmd *c)
{
    c->Rewind();
    int len = c->GetNumInArgs();
    Tcl_Obj *tclCommand = Tcl_NewStringObj(c->In()->GetS(), -1);
    for (int i = 0; i < len; i++) {
        Tcl_AppendToObj(tclCommand, " ", 1);
        Tcl_AppendObjToObj(tclCommand, NArg2TclObj(c->In()));
    }
    int errCode = Tcl_EvalObjEx(this->interp, tclCommand,  TCL_EVAL_DIRECT);
    Tcl_Obj *res = Tcl_GetObjResult(interp);
    if (errCode == TCL_ERROR) {
        n_printf("*** Tcl error '%s'\n", Tcl_GetString(res));
        if (this->GetFailOnError()) {
            n_error("Aborting.");
        }
        return false;
    }
    return true;
}

//--------------------------------------------------------------------
/**
    - 10-Oct-98   floh    created
    - 25-Feb-99   floh    Error-Message
    - 30-Mar-99   floh    + schickt den Filename zuerst durch
                            nFileServer::ManglePath(), um evtl
                            Assigns aufzuloesen
*/
//--------------------------------------------------------------------
bool nTclServer::RunScript(const char *fname, const char*& result)
{
    char buf[N_MAXPATH];
    result = 0;
    kernelServer->GetFileServer2()->ManglePath(fname,buf,sizeof(buf));
    this->print_error = true;
    int errCode = Tcl_EvalFile(this->interp, buf);
    this->print_error = false;
    Tcl_Obj *res = Tcl_GetObjResult(interp);
    result = Tcl_GetString(res);
    if (errCode == TCL_ERROR) 
    {
        n_printf("*** Tcl error '%s' in file %s line %d.\n", result, fname, this->interp->errorLine);
        if (this->GetFailOnError())
        {
            n_error("Aborting.");
        }
        return false;
    }             
    return true;
}

//--------------------------------------------------------------------
/**
    @brief Guarantee that the TCL events are processed.

    - 21-Dec-98   floh    created
    - 23-Feb-99   floh    jetzt mit Return-Val, wenn Quit requested.
*/
//--------------------------------------------------------------------
bool nTclServer::Trigger(void)
{
    while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT));
    return nScriptServer::Trigger();
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

