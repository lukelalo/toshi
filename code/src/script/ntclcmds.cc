#define N_IMPLEMENTS nTclServer
//--------------------------------------------------------------------
//  ntclcmds.cc --  implementiere Tcl-Erweiterungs-Kommandos
//                  
//  (C) 1998 Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include "script/ntclserver.h"
#include "kernel/nkernelserver.h"
#include "kernel/nroot.h"

//--------------------------------------------------------------------
/**
    - 25-Feb-99   floh    created
*/
//--------------------------------------------------------------------
static void tcl_staticError(Tcl_Interp *interp, bool print_error, const char *msg)
{
    Tcl_SetResult(interp, (char *)msg, TCL_STATIC);
    if (print_error) {
        n_printf(msg);
        n_printf("\n");
    }
}

//--------------------------------------------------------------------
/**
    - 25-Feb-99   floh    created
*/
//--------------------------------------------------------------------
static void tcl_volatileError(Tcl_Interp *interp, bool print_error, const char *msg)
{
    Tcl_SetResult(interp, (char *)msg, TCL_VOLATILE);
    if (print_error) {
        n_printf(msg);
        n_printf("\n");
    }
}

//--------------------------------------------------------------------
/**
    - 25-Feb-99   floh    created
*/
//--------------------------------------------------------------------
static void tcl_unknownObject(Tcl_Interp *interp, bool print_error, const char *obj_name)
{
    char buf[256];
    sprintf(buf, "Object '%s' does not exist.", obj_name);
    tcl_volatileError(interp, print_error, buf);
}

//--------------------------------------------------------------------
/**
    Execute a replaced command with the argument list
    of another command.

    - 04-Feb-00   floh    created
    - 24-Nov-00   floh    + the reference incr/decr was completely broken,
                            this left unused objects aroung when
                            tcl_pipe_command was executed, doh...
    - 04-Mar-03   dmitri  + Tcl_NewStringObj() returns an object with a zero
                            refcount, causing crashes in the TCL debugger,
                            but not in stand-alone Nebula.
*/
//--------------------------------------------------------------------
static int tcl_pipe_command(Tcl_Interp *interp, const char *cmd, int objc, Tcl_Obj *CONST *objv)
{
#define MAX_ARGS (64)
    n_assert(objc < MAX_ARGS);
    int i;
    Tcl_Obj *new_objv[MAX_ARGS];
    int res = TCL_OK;

    // increment refcount of original args
    for (i=1; i<objc; i++) {
        new_objv[i] = objv[i];
        Tcl_IncrRefCount(new_objv[i]);
    }
    new_objv[0] = Tcl_NewStringObj(cmd,strlen(cmd));
    Tcl_IncrRefCount(new_objv[0]);
    res = Tcl_EvalObjv(interp,objc,new_objv,TCL_EVAL_DIRECT);

    // decrement refcount of newly created command, this will effectively release it
    Tcl_DecrRefCount(new_objv[0]);

    // decrement refcount of args
    for (i=1; i<objc; i++) {
        new_objv[i] = objv[i];
        Tcl_DecrRefCount(new_objv[i]);
    }
    return res;
}

//--------------------------------------------------------------------
/**
    - 10-Oct-98   floh    created
    - 19-Oct-98   floh    neuer Syntax
    - 01-Nov-98   floh    Client-Funktionalitaet
    - 23-Oct-99   floh    should no longer crash when object could not
                          be created.
    - 03-Feb-00   floh    + converted to tcl objects 
*/
//--------------------------------------------------------------------
int tclcmd_New(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST *objv)
{
    int retval = TCL_ERROR;
    nTclServer *tcl = (nTclServer *)cdata;
    if (objc != 3) tcl_staticError(interp, tcl->print_error, "Syntax is 'name = new class name'");
    else {
        char *cl_name = Tcl_GetString(objv[1]);
        char *o_name  = Tcl_GetString(objv[2]);
        nRoot *o = nTclServer::kernelServer->NewNoFail(cl_name,o_name);
        if (o) {
            char buf[512];
            Tcl_SetResult(interp, o->GetFullName(buf,sizeof(buf)), TCL_VOLATILE);
            retval = TCL_OK;
        }
    }
    return retval;
}

//--------------------------------------------------------------------
/**
    - 10-Oct-98   floh    created
    - 19-Oct-98   floh    neuer Syntax
    - 01-Nov-98   floh    Client-Funktionalitaet
    - 11-Dec-98   floh    + die Routine weigert sich jetzt, ein Objekt
                            zu loeschen, welches im aktuellen Pfad liegt
                          + oops, Tcl-Result-String wurde als TCL_STATIC,
                            nicht TCL_VOLATILE zurueckgegeben.
    - 03-Feb-00   floh    + converted to Tcl objects
                          + removed the cwd deletion check, the
                            kernel does its own check anyway
*/
//--------------------------------------------------------------------
int tclcmd_Delete(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST *objv)
{
    int retval = TCL_ERROR;
    nTclServer *tcl = (nTclServer *)cdata;
    if (objc != 2) tcl_staticError(interp, tcl->print_error, "Syntax is 'delete name'");
    else {
        nRoot *o = nTclServer::kernelServer->Lookup(Tcl_GetString(objv[1]));
        if (o) {
            o->Release();
            retval = TCL_OK;
        } else tcl_unknownObject(interp,tcl->print_error,Tcl_GetString(objv[1]));
    }
    return retval;
}

//--------------------------------------------------------------------
/**
    - 10-Oct-98   floh    created
    - 19-Oct-98   floh    neuer Syntax
    - 01-Nov-98   floh    Client-Funktionalitaet
    - 03-Feb-00   floh    + converted to Tcl objects
*/
//--------------------------------------------------------------------
int tclcmd_Sel(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST *objv)
{
    int retval = TCL_ERROR;
    nTclServer *tcl = (nTclServer *)cdata;
    if (objc != 2) tcl_staticError(interp, tcl->print_error, "Syntax is 'name = sel name'");
    else {
        char *o_name = Tcl_GetString(objv[1]);
        nRoot *o = nTclServer::kernelServer->Lookup(o_name);
        if (o) {
            nTclServer::kernelServer->SetCwd(o);
            char buf[N_MAXPATH];
            Tcl_SetResult(interp, o->GetFullName(buf,sizeof(buf)), TCL_VOLATILE);
            retval = TCL_OK;
        } else tcl_unknownObject(interp,tcl->print_error,Tcl_GetString(objv[1]));
    }
    return retval;
}

//--------------------------------------------------------------------
//  tclcmd_Psel()
//  10-Oct-98   floh    created
//  19-Oct-98   floh    neuer Syntax
//  01-Nov-98   floh    Client-Funktionalitaet
//  03-Feb-00   floh    converted to Tcl objects
//--------------------------------------------------------------------
int tclcmd_Psel(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST *objv)
{
    int retval = TCL_ERROR;
    nTclServer *tcl = (nTclServer *)cdata;
    if (objc != 1) tcl_staticError(interp, tcl->print_error, "Syntax is 'name = psel'");
    else {
        nRoot *o = nTclServer::kernelServer->GetCwd();
        if (o) {
            char buf[N_MAXPATH];
            Tcl_SetResult(interp, o->GetFullName(buf,sizeof(buf)), TCL_VOLATILE);
            retval = TCL_OK;
        } else tcl_staticError(interp, tcl->print_error, "No object selected.");
    }    
    return retval;
}

//--------------------------------------------------------------------
/**
    - 28-Dec-99   floh    created
    - 03-Feb-00   floh    converted to Tcl objects
*/
//--------------------------------------------------------------------
int tclcmd_Dir(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST *objv)
{
    int retval = TCL_ERROR;
    nTclServer *tcl = (nTclServer *)cdata;
    Tcl_ResetResult(interp);
    if (objc != 1) tcl_staticError(interp, tcl->print_error, "Syntax is 'dir'");
    else {
        nRoot *cwd = nTclServer::kernelServer->GetCwd();
        if (cwd) {
            nRoot *o;
            Tcl_ResetResult(interp);
            Tcl_Obj *res = Tcl_GetObjResult(interp);
            for (o=cwd->GetHead(); o; o=o->GetSucc()) {
                const char *n = o->GetName();
                Tcl_AppendToObj(res,(char *)n,strlen(n));
                Tcl_AppendToObj(res," ",1);
            }
            retval = TCL_OK;
        } else tcl_staticError(interp, tcl->print_error, "No selected object.");
    }
    return retval;
}

//--------------------------------------------------------------------
/**
    - 10-Oct-98   floh    created
    - 19-Oct-98   floh    neuer Syntax
    - 01-Nov-98   floh    Client-Funktionalitaet
    - 03-Feb-00   floh    converted to Tcl objects
*/
//--------------------------------------------------------------------
int tclcmd_Get(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST *objv)
{
    int retval = TCL_ERROR;
    nTclServer *tcl = (nTclServer *)cdata;
    if (objc != 2) tcl_staticError(interp, tcl->print_error, "Syntax is 'name = get filename'");
    else {
        char *o_name = Tcl_GetString(objv[1]);
        nRoot *o = nTclServer::kernelServer->Load(o_name);
        if (o) {
            char buf[512];
            Tcl_SetResult(interp, o->GetFullName(buf,sizeof(buf)), TCL_VOLATILE);
            retval = TCL_OK;
        } else {
            char buf[256];
            sprintf(buf, "Could not load object [name=%s].", Tcl_GetString(objv[1]));
            tcl_volatileError(interp, tcl->print_error, buf);
        }
    }
    return retval;
}

//--------------------------------------------------------------------
/**
    - 11-Oct-98   floh    created
    - 18-Dec-98   floh    argc und argv muessen jetzt auf das 1.Arg
                          zeigen
    - 21-Jun-99   floh    Support fuer ARGTYPE_OBJECT
    - 08-Aug-99   floh    bissel aufgeraeumt
    - 25-Oct-99   floh    unterstuetzt jetzt 'null' als Objekt-Pointer
    - 03-Feb-00   floh    + converted to Tcl objects
    - 02-Aug-00   floh    + support fuer 'null' strings
    - 24-Apr-01   floh    + support for 'code' data type (has different
                            delimiter)
*/
//--------------------------------------------------------------------
static bool _getInArgs(Tcl_Interp *interp, nCmd *cmd, int objc, Tcl_Obj *CONST *objv)
{
    long num_args;
        
    num_args = cmd->GetNumInArgs();
    if (num_args == objc) {
 
        // read out arguments
        int i;
        nArg *arg;
        cmd->Rewind();
        for (i=0; i<num_args; i++) {
            bool arg_ok = false;
            arg = cmd->In();
            switch(arg->GetType()) {
                case nArg::ARGTYPE_INT:
                {
                    int n;
                    if (Tcl_GetIntFromObj(interp,objv[i],&n)==TCL_OK) {
                        arg->SetI(n);                                
                        arg_ok = true;
                    }
                } 
                break;

                case nArg::ARGTYPE_FLOAT:
                {
                    double d;
                    if (Tcl_GetDoubleFromObj(interp,objv[i],&d)==TCL_OK) {
                        float f = (float) d;
                        arg->SetF(f);
                        arg_ok = true;
                    }
                }
                break;


                case nArg::ARGTYPE_STRING:
                {
                    const char *str = Tcl_GetString(objv[i]);
                    if (strcmp(":null:",str)==0) {
                        str = NULL;
                    }
                    arg->SetS(str);
                    arg_ok = true;
                }
                break;    

                case nArg::ARGTYPE_CODE:
                {
                    const char *str = Tcl_GetString(objv[i]);
                    if (strcmp("",str)==0) {
                        str = NULL;
                    }
                    arg->SetC(str);
                    arg_ok = true;
                }
                break;    

                case nArg::ARGTYPE_BOOL:
                {
                    int bi;
                    if (Tcl_GetBooleanFromObj(interp,objv[i],&bi)==TCL_OK) {
                        bool b = bi ? true : false;
                        arg->SetB(b);
                        arg_ok = true;
                    }
                }
                break;

                case nArg::ARGTYPE_OBJECT:
                    {
                        nRoot *o;
                        char *o_name = Tcl_GetString(objv[i]);
                        if (strcmp("null",o_name)==0) {
                            o = NULL;
                            arg_ok = true;
                        } else {
                            o = nTclServer::kernelServer->Lookup(o_name);
                            if (o) arg_ok = true;
                            else   n_printf("could not lookup '%s' as object!\n",o_name);
                        }
                        arg->SetO(o);
                    }
                    break;
 
                case nArg::ARGTYPE_VOID:
                    break;

                case nArg::ARGTYPE_LIST:
                    n_warn("List values aren't acceptable in arguments.");
                    arg_ok = false;
                    break;
            }
            if (!arg_ok) return false;
        }
        return true;
    }
    return false;
}

//--------------------------------------------------------------------
/**
    utility function for nArg of type ARGTYPE_LIST
*/
static Tcl_Obj* _putOutListArg(Tcl_Interp *interp, nArg *listArg)
{
    n_assert(nArg::ARGTYPE_LIST == listArg->GetType());
    
    nArg* args;
    int num_args = listArg->GetL(args);

    Tcl_Obj* res = Tcl_NewListObj(0,0);

    nArg* arg = args;
    for (int i=0; i<num_args; i++) {
        switch(arg->GetType()) {
            case nArg::ARGTYPE_INT:
                {
                    Tcl_Obj *io = Tcl_NewIntObj(arg->GetI());
                    Tcl_ListObjAppendElement(interp,res,io);
                }
                break;

            case nArg::ARGTYPE_FLOAT:
                {
                    Tcl_Obj *fo = Tcl_NewDoubleObj((double)arg->GetF());
                    Tcl_ListObjAppendElement(interp,res,fo);
                }
                break;

            case nArg::ARGTYPE_STRING:
                {
                    const char *s = arg->GetS();
                    if (!s) {
                        s = ":null:";
                    }
                    Tcl_Obj *so = Tcl_NewStringObj((char *)s,strlen(s));
                    Tcl_ListObjAppendElement(interp,res,so);
                }
                break;

            case nArg::ARGTYPE_CODE:
                {
                    const char *s = arg->GetC();
                    if (!s) {
                        s = "";
                    }
                    Tcl_Obj *so = Tcl_NewStringObj((char *)s,strlen(s));
                    Tcl_ListObjAppendElement(interp,res,so);
                }
                break;

            case nArg::ARGTYPE_BOOL:
                {
                    const char *s = arg->GetB() ? "true" : "false";
                    Tcl_Obj *so = Tcl_NewStringObj((char *)s,strlen(s));
                    Tcl_ListObjAppendElement(interp,res,so);
                }
                break;

            case nArg::ARGTYPE_OBJECT:
                {
                    char buf[N_MAXPATH];
                    const char *s;
                    nRoot *o = (nRoot *) arg->GetO();
                    if (o) {
                        o->GetFullName(buf,sizeof(buf));
                        s = buf;
                    } else {
                        s = "null";
                    }
                    Tcl_Obj *so = Tcl_NewStringObj(s,strlen(s));
                    Tcl_ListObjAppendElement(interp,res,so);
                }
                break;

            case nArg::ARGTYPE_LIST:
                Tcl_ListObjAppendElement(interp,res,_putOutListArg(interp,arg));
                break;

            case nArg::ARGTYPE_VOID:
                break;
        }
        arg++;
    }

    return res;
}

//--------------------------------------------------------------------
/**
    *** TODO ***
    BOOL Objekte auch als Tcl-Bool-Objekte behandeln, nicht als
    String-Objekte (dafuer muessen aber die existieren
    Tcl-Objekte angepasst werden!)

    - 11-Oct-98   floh    created
    - 27-Oct-98   floh    oops, Tcl_AppendResult() macht offensichtlich
                          keine Leerzeichen
    - 21-Jun-99   floh    + Support fuer ARGTYPE_OBJECT
    - 08-Aug-99   floh    bissel aufgeraeumt
    - 20-Dec-99   floh    + hat am Anfang Tcl-Result nicht neu initialisiert,
                            sondern gleich mit Append angefangen...
    - 03-Feb-00   floh    + converted to Tcl objects
    - 02-Aug-00   floh    + support for 'null' strings
    - 24-Apr-01   floh    + support for 'code chunk' datatype
*/
//--------------------------------------------------------------------
static void _putOutArgs(Tcl_Interp *interp, nCmd *cmd)
{
    nArg *arg;
    int num_args = cmd->GetNumOutArgs();
    cmd->Rewind();

    Tcl_ResetResult(interp);
    Tcl_Obj *res = Tcl_GetObjResult(interp);

    // handle single return args (no need to create list)
    if (1 == num_args) {
        arg = cmd->Out();
        switch (arg->GetType()) {

            case nArg::ARGTYPE_INT:
                Tcl_SetIntObj(res,arg->GetI());
                break;

            case nArg::ARGTYPE_FLOAT:
                Tcl_SetDoubleObj(res,arg->GetF());
                break;

            case nArg::ARGTYPE_STRING:
                {
                    const char *s = arg->GetS();
                    if (!s) {
                        s = ":null:";
                    }
                    Tcl_SetStringObj(res,(char *)s,strlen(s));
                }
                break;

            case nArg::ARGTYPE_CODE:
                {
                    const char *s = arg->GetC();
                    if (!s) {
                        s = "";
                    }
                    Tcl_SetStringObj(res,(char *)s,strlen(s));
                }
                break;

            case nArg::ARGTYPE_BOOL:
                {
                    const char *s = arg->GetB() ? "true" : "false";
                    Tcl_SetStringObj(res,(char *)s,strlen(s));
                }
                break;

            case nArg::ARGTYPE_OBJECT:
                {
                    char buf[N_MAXPATH];
                    const char *s;
                    nRoot *o = (nRoot *) arg->GetO();
                    if (o) {
                        o->GetFullName(buf,sizeof(buf));
                        s = buf;
                    } else {
                        s = "null";
                    }
                    Tcl_SetStringObj(res,(char *)s,strlen(s));
                }
                break;

            case nArg::ARGTYPE_LIST:
                Tcl_ListObjAppendList(interp,res,_putOutListArg(interp,arg));
                break;

            case nArg::ARGTYPE_VOID:
                break;
        }
    } else {

        // more then one output arg, create a list
        int i;
        for (i=0; i<num_args; i++) {
            arg = cmd->Out();
            switch(arg->GetType()) {

                case nArg::ARGTYPE_INT:
                    {
                        Tcl_Obj *io = Tcl_NewIntObj(arg->GetI());
                        Tcl_ListObjAppendElement(interp,res,io);
                    }
                    break;

                case nArg::ARGTYPE_FLOAT:
                    {
                        Tcl_Obj *fo = Tcl_NewDoubleObj((double)arg->GetF());
                        Tcl_ListObjAppendElement(interp,res,fo);
                    }
                    break;

                case nArg::ARGTYPE_STRING:
                    {
                        const char *s = arg->GetS();
                        if (!s) {
                            s = ":null:";
                        }
                        Tcl_Obj *so = Tcl_NewStringObj((char *)s,strlen(s));
                        Tcl_ListObjAppendElement(interp,res,so);
                    }
                    break;

                case nArg::ARGTYPE_CODE:
                    {
                        const char *s = arg->GetC();
                        if (!s) {
                            s = "";
                        }
                        Tcl_Obj *so = Tcl_NewStringObj((char *)s,strlen(s));
                        Tcl_ListObjAppendElement(interp,res,so);
                    }
                    break;

                case nArg::ARGTYPE_BOOL:
                    {
                        const char *s = arg->GetB() ? "true" : "false";
                        Tcl_Obj *so = Tcl_NewStringObj((char *)s,strlen(s));
                        Tcl_ListObjAppendElement(interp,res,so);
                    }
                    break;

                case nArg::ARGTYPE_OBJECT:
                    {
                        char buf[N_MAXPATH];
                        const char *s;
                        nRoot *o = (nRoot *) arg->GetO();
                        if (o) {
                            o->GetFullName(buf,sizeof(buf));
                            s = buf;
                        } else {
                            s = "null";
                        }
                        Tcl_Obj *so = Tcl_NewStringObj(s,strlen(s));
                        Tcl_ListObjAppendElement(interp,res,so);
                    }
                    break;

                case nArg::ARGTYPE_LIST:
                    Tcl_ListObjAppendElement(interp,res,_putOutListArg(interp,arg));
                    break;

                case nArg::ARGTYPE_VOID:
                    break;
            }
        }
    }
}         

//--------------------------------------------------------------------
/**
    @brief Print a error message involving an object and a command.

    - 03-Feb-00   floh    created
*/
//--------------------------------------------------------------------
static void tcl_objcmderror(Tcl_Interp *interp,
                            nTclServer *tcl,
                            const char *msg,    // message, must contain 2 '%s'
                            nRoot *o,           // name is 1st '%s'
                            char *cmd_name)     // 2nd '%s'
{
    char error_buf[1024];
    char name_buf[N_MAXPATH];
    sprintf(error_buf,msg,o->GetFullName(name_buf,sizeof(name_buf)),cmd_name);
    tcl_volatileError(interp, tcl->print_error, error_buf);
}

//--------------------------------------------------------------------
/**
    Wird vom Tcl-Interpreter aufgerufen, wenn ein unbekanntes
    Kommando empfangen wird.
    Die Routine versucht den String so zu interpretieren:
    obj_name.command
    Also, auf "obj_name" wird das durch Punkt getrennte Kommando
    angewendet.
    Fehlt der obj_name, oder faengt das Kommando mit einem
    Punkt an, wird "command" auf das momentan ausgewaehlte
    Objekt angewendet.
    Wenn was schiefgeht, wird das Kommando an das originale
    Unknown-Kommando weitergereicht.

    - 12-Jan-99   floh    created
    - 30-Sep-99   floh    crashing bug, beim Anwenden eines Kommandos
                          wurde Cwd gecacht, wenn das Cwd durch die
                          Kommando-Anwendung irgendwie gekillt wurde,
                          gab es damit ein ungueltiges Cwd im Kernel.
    - 03-Feb-00   floh    if selected object doesn't accept command,
                          the original 'unknown' procedure is invoked
*/
//--------------------------------------------------------------------
int tclcmd_Unknown(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST *objv)
{
    int retval = TCL_ERROR;
    nTclServer *tcl = (nTclServer *) cdata;
        char *obj_name;
        char *cmd_name;
        char *dot;
        char cmd[N_MAXPATH];
        nRoot *o;
        bool has_dot = false;

        // extract object name and cmd name
        n_strncpy2(cmd,Tcl_GetString(objv[1]),sizeof(cmd));
        dot = strchr(cmd,'.');

        // special case handle path components
        while (dot && ((dot[1] == '.')||(dot[1] == '/'))) dot=strchr(++dot,'.'); 
        if (dot) {
            has_dot = true;
            *dot = 0;
            obj_name = cmd;
            if (obj_name == dot) obj_name = NULL;
            cmd_name = dot+1;
        } else {
            obj_name = NULL;
            cmd_name = cmd;
        }
        if (*cmd_name == 0) cmd_name = NULL;

        // find object to invoke command on
        if (obj_name) o = nTclServer::kernelServer->Lookup(obj_name);
        else          o = nTclServer::kernelServer->GetCwd();
        if (!o) {
            tcl_unknownObject(interp, tcl->print_error, obj_name);
            return TCL_ERROR;
        }
        if (!cmd_name) {
            tcl_staticError(interp, tcl->print_error, "No command.");
            return TCL_ERROR;
        }

        // invoke command
        nClass *cl = o->GetClass();
        nCmdProto *cmd_proto = (nCmdProto *) cl->FindCmdByName(cmd_name);
        if (cmd_proto) {
            
            nCmd *cmd = cmd_proto->NewCmd();
            n_assert(cmd);

            // retrieve input args (skip the 'unknown' and cmd statement)            
            if (!_getInArgs(interp,cmd,objc-2,objv+2)) {
                tcl_objcmderror(interp,tcl,"Broken input args, object '%s', command '%s'",o,cmd_name);
                cmd_proto->RelCmd(cmd);
                return TCL_ERROR;
            }

            // let object handle the command
            if (o->Dispatch(cmd)) {
                retval = TCL_OK;
                _putOutArgs(interp,cmd);
            } else {
                tcl_objcmderror(interp,tcl,"Dispatch error, object '%s', command '%s'",o,cmd_name);
            }
            cmd_proto->RelCmd(cmd);
            
        } else {

            // exception, the object doesn't know about the command!
            // if there was a dot in the command name, we know that
            // it was supposed to be a Nebula command statement,
            // so we will just barf.
            if (has_dot) {
                // this is not very critical...
                tcl_objcmderror(interp,tcl,"Object '%s' doesn't accept command '%s'",o,cmd_name);
                retval = TCL_OK;
            } else {
                // otherwise, let's see if the original unknown command
                // knows what to do...
                retval = tcl_pipe_command(interp,"tcl_unknown",objc,objv);
            }
        }
    return retval;
}

//--------------------------------------------------------------------
/**
    - 10-Oct-98   floh    created
    - 19-Oct-98   floh    neuer Syntax
    - 01-Nov-98   floh    Client-Funktionalitaet
    - 23-Feb-99   floh    setzt das Quit-Requested-Flag
    - 03-Feb-00   floh    + converted to Tcl objects
    - 24-Nov-00   floh    + calls Tcl_Exit() if in extension mode
*/
//--------------------------------------------------------------------
int tclcmd_Exit(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST *objv)
{
    int retval = TCL_ERROR;
    nTclServer *tcl = (nTclServer *) cdata;
    if (objc != 1) tcl_staticError(interp, tcl->print_error, "Syntax is 'exit'");
    else {
        // turn off the interactive mode and set the quit requested flag,
        // if the Tcl server is in standalone mode (thus a Nebula application
        // wraps has loaded Tcl), this is the signal for the application
        // that the script server requested to quit the application. 
        // In extension mode however, Nebula has been loaded
        // into an existing Tcl interpreter. In this case, we simply call
        // Tcl_Exit() which does the basic cleanup stuff, and then exits
        // the process, Nebula then gets unloaded in the detach handler
        // of the extension dll.

        tcl->SetQuitRequested(true);
        if (!tcl->is_standalone_tcl) {
            Tcl_Exit(0);
        }
        retval = TCL_OK;
    }
    return retval;
}

//--------------------------------------------------------------------
/**
    - 23-Feb-99   floh    created
    - 25-May-99   floh    wenn das puts an Stdout geht, wird es nicht
                          mehr an das originale Puts weitergeleitet.
    - 03-Feb-00   floh    + converted to Tcl objects
*/
//--------------------------------------------------------------------
int tclcmd_Puts(ClientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST *objv)
{
    int retval     = TCL_OK;
    bool is_stdout = true;

    // emulate puts behaviour but reroute output into Nebula kernel
    if (objc > 1) {
        bool newline = true;
        char *str = Tcl_GetString(objv[objc-1]);
        int i;
        for (i=1; i<(objc-1); i++) {
            char *s = Tcl_GetString(objv[i]);
            if (strcmp(s,"-nonewline")==0) newline = false;
            else if (strcmp(s,"stdout")!=0) is_stdout = false;
        }
        if (is_stdout) {
            nTclServer::kernelServer->Print(str);
            if (newline) nTclServer::kernelServer->Print("\n");
        }
    }
    // hand control to original puts command
    if (!is_stdout) {
        retval = tcl_pipe_command(interp,"tcl_puts",objc,objv);
    }
    return retval;
}

//--------------------------------------------------------------------
/**
    - 14-Jan-00   floh    created
    - 03-Feb-00   floh    + converted to Tcl objects
*/
//--------------------------------------------------------------------
int tclcmd_Exists(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *CONST *objv)
{
    int retval = TCL_ERROR;
    nTclServer *tcl = (nTclServer *)cdata;
    if (objc != 2) tcl_staticError(interp, tcl->print_error, "Syntax is 'exists name'");
    else {
        nRoot *o = nTclServer::kernelServer->Lookup(Tcl_GetString(objv[1]));
        if (o) Tcl_SetResult(interp, "1", TCL_STATIC);
        else   Tcl_SetResult(interp, "0", TCL_STATIC);
        retval = TCL_OK;
    }
    return retval;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

