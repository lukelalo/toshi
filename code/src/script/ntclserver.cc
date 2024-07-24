#define N_IMPLEMENTS nTclServer
//--------------------------------------------------------------------
//  ntclserver.cc
//  (C) 1998-2000 Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

#include "kernel/nkernelserver.h"
#include "kernel/npersistserver.h"
#include "script/ntclserver.h"
#include "kernel/nfile.h"
#include "kernel/nfileserver2.h"

nNebulaClass(nTclServer, "nscriptserver");

extern Tcl_ObjCmdProc tclcmd_New;
extern Tcl_ObjCmdProc tclcmd_Delete;
extern Tcl_ObjCmdProc tclcmd_Sel;
extern Tcl_ObjCmdProc tclcmd_Psel;
extern Tcl_ObjCmdProc tclcmd_Get;
extern Tcl_ObjCmdProc tclcmd_Exit;
extern Tcl_ObjCmdProc tclcmd_Unknown;
extern Tcl_ObjCmdProc tclcmd_Puts;
extern Tcl_ObjCmdProc tclcmd_Dir;
extern Tcl_ObjCmdProc tclcmd_Exists;

//--------------------------------------------------------------------
//  link_to_interp()
//  Link Nebula to an existing Tcl interpreter.
//  27-Nov-00   floh    created
//--------------------------------------------------------------------
void nTclServer::link_to_interp(Tcl_Interp *interp, bool is_standalone)
{
    n_assert(interp);

    if (is_standalone) {
        // initialize tcl_library variable
        #ifndef __MACOSX__
        char buf[N_MAXPATH];
        kernelServer->GetFileServer2()->ManglePath("home:bin/tcl/tcl8.4",buf,sizeof(buf));
        #ifdef __WIN32__
            // under Windows let's be nice with backslashes
            char *tmp;
            while ((tmp = strchr(buf,'/'))) *tmp='\\';
        #endif

        n_printf("Setting tcl_library to '%s'\n",buf);
        Tcl_SetVar(this->interp,"tcl_library",buf,0);
        #endif
        Tcl_Init(this->interp);    

        // puts is only overriden in standalone mode, since Tcl debuggers
        // (at least the TclPro debugger) don't like it when puts is modified
        Tcl_EvalEx(this->interp,"rename puts tcl_puts",-1,TCL_EVAL_DIRECT);
        Tcl_CreateObjCommand(this->interp,"puts",tclcmd_Puts,(ClientData)this,NULL);

    }

    Tcl_EvalEx(this->interp,"rename unknown tcl_unknown",-1,TCL_EVAL_DIRECT);
    Tcl_CreateObjCommand(this->interp,"unknown",tclcmd_Unknown,(ClientData)this,NULL);

    Tcl_CreateObjCommand(this->interp, "new",           tclcmd_New,         (ClientData)this, NULL);
    Tcl_CreateObjCommand(this->interp, "delete",        tclcmd_Delete,      (ClientData)this, NULL);
    Tcl_CreateObjCommand(this->interp, "sel",           tclcmd_Sel,         (ClientData)this, NULL);
    Tcl_CreateObjCommand(this->interp, "psel",          tclcmd_Psel,        (ClientData)this, NULL);
    Tcl_CreateObjCommand(this->interp, "get",           tclcmd_Get,         (ClientData)this, NULL);
    Tcl_CreateObjCommand(this->interp, "exit",          tclcmd_Exit,        (ClientData)this, NULL);
    Tcl_CreateObjCommand(this->interp, "dir",           tclcmd_Dir,         (ClientData)this, NULL);
    Tcl_CreateObjCommand(this->interp, "exists",        tclcmd_Exists,      (ClientData)this, NULL);
}

//--------------------------------------------------------------------
//  unlink_from_interp()
//  Unlink Nebula from an Tcl interpreter.
//  27-Nov-00   floh    created
//--------------------------------------------------------------------
void nTclServer::unlink_from_interp(Tcl_Interp *interp, bool is_standalone)
{
    n_assert(interp);

    Tcl_DeleteCommand(this->interp, "exists");
    Tcl_DeleteCommand(this->interp, "dir");
    Tcl_DeleteCommand(this->interp, "unknown");
    Tcl_DeleteCommand(this->interp, "exit");
    Tcl_DeleteCommand(this->interp, "get");
    Tcl_DeleteCommand(this->interp, "psel");
    Tcl_DeleteCommand(this->interp, "sel");
    Tcl_DeleteCommand(this->interp, "delete");
    Tcl_DeleteCommand(this->interp, "new");

    if (is_standalone) {
        Tcl_DeleteCommand(this->interp,"puts");
        Tcl_EvalEx(this->interp,"rename tcl_puts puts",-1,TCL_EVAL_DIRECT);
    }    

    Tcl_EvalEx(this->interp,"rename tcl_unknown unknown",-1,TCL_EVAL_DIRECT);
}

//--------------------------------------------------------------------
//  nTclServer()
//  10-Oct-98   floh    created
//  19-Oct-98   floh    eingebaute Kommandos umbenannt:
//                          nnew     -> new
//                          nrelease -> delete
//                          ncd      -> sel
//                          npwd     -> psel
//                          nopen    -> get
//                          nexit    -> exit
//                          ncmd     -> do
//                          nconnect -> connect
//  18-Dec-98   floh    neues Kommando: ondo, macht ein atomares
//                      select-do-unselect
//                      + neue Kommando: clone
//  12-Jan-99   floh    + unknown Command-Handling
//  23-Feb-99   floh    + puts wird renamed nach tcl_puts, puts
//                        wird dann abgefangen, und ueber n_printf()
//                        geleitet
//  11-May-99   floh    + neues Kommando: loadpackage
//  24-May-99   floh    + do, ondo und clone raus
//  25-May-99   floh    + log [off,stdout,stderr,fname]
//  16-Jun-99   floh    + ruft Tcl_Init() auf, welches hauptsaechlich
//                        init.tcl sourced
//  28-Dec-99   floh    + neu: 'dir'.
//  14-Jan-00   floh    + exists
//  03-Feb-00   floh    + converted to Tcl_CreateObjCommand
//                      + now properly initializes tcl_library
//  27-Nov-00   floh    + Standalone <-> Extension mode
//--------------------------------------------------------------------
nTclServer::nTclServer() :
    refFileServer(kernelServer, this)
{
    this->refFileServer     = "/sys/servers/file2";
    this->print_error       = false;
    this->is_standalone_tcl = true;
    this->indent_level      = 0;
    this->indent_buf[0]     = 0;

    // initialize data internal to TCL to make encodings work.
    char buf[N_MAXPATH];
    kernelServer->GetFileServer2()->ManglePath("home:bin/tcl/tcl8.4",buf,sizeof(buf));
    #ifdef __WIN32__
        // under Windows let's be nice with backslashes
        char *tmp;
        while ((tmp = strchr(buf,'/'))) *tmp='\\';
    #endif
    Tcl_FindExecutable(buf);

    // create interpreter
    this->interp  = Tcl_CreateInterp();
    n_assert(this->interp);

    this->link_to_interp(this->interp, this->is_standalone_tcl);
}

//--------------------------------------------------------------------
//  ~nTclServer()
//  10-Oct-98   floh    created
//  18-Dec-98   floh    + neues Kommando 'ondo'
//                      + 'clone'
//  12-Jan-99   floh    + 'unknown'
//  23-Feb-99   floh    + 'puts'
//  11-May-99   floh    + 'loadpackage'
//  24-May-99   floh    + do, ondo und clone raus
//  25-May-99   floh    + log
//  28-Dec-99   floh    + dir
//  14-Jan-00   floh    + exists
//--------------------------------------------------------------------
nTclServer::~nTclServer()
{
    n_assert(this->interp);
    this->unlink_from_interp(this->interp, this->is_standalone_tcl);    
    if (this->is_standalone_tcl) Tcl_DeleteInterp(this->interp);
}

//--------------------------------------------------------------------
//  InitAsExtension()
//  Migrate from standalone mode to tcl extension mode. Our "owned"
//  Tcl intrepreter created in the nTclServer constructor will
//  be killed, and Nebula will be linked to the provided
//  Tcl interpreter.
//  27-Nov-00   floh    created
//--------------------------------------------------------------------
void nTclServer::InitAsExtension(Tcl_Interp *ext_interp)
{
    n_assert(ext_interp);
    n_assert(this->interp);

    // unlink from previous interpreter
    this->unlink_from_interp(this->interp,this->is_standalone_tcl);

    // if we created the previous interpreter, kill it
    if (this->is_standalone_tcl) {
        Tcl_DeleteInterp(this->interp);
        this->interp = NULL;
    }

    // link to the external interpreter
    this->interp = ext_interp;
    this->is_standalone_tcl = false;
    this->link_to_interp(this->interp, this->is_standalone_tcl);
}

//------------------------------------------------------------------------------
/**
    Begin writing a persistent object.
*/
nFile* 
nTclServer::BeginWrite(const char* filename, nRoot* obj)
{
    n_assert(filename);
    n_assert(obj);

    this->indent_level = 0;

    nFile* file = this->refFileServer->NewFileObject();
    n_assert(file);
    if (file->Open(filename, "w"))
    {
        char buf[N_MAXPATH];
        sprintf(buf, "# $parser:ntclserver$ $class:%s$\n", obj->GetClass()->GetName());

        file->PutS("# ---\n");
        file->PutS(buf);
        file->PutS("# ---\n");
        return file;
    }
    else
    {
        n_printf("nTclServer::WriteBegin(): failed to open file '%s' for writing!\n", filename);
        delete file;
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Finish writing a persistent object.
*/
bool 
nTclServer::EndWrite(nFile* file)
{
    n_assert(file);
 
    file->PutS("# ---\n");
    file->PutS("# Eof\n");
    
    file->Close();
    delete file;
    return (this->indent_level == 0);
}

//--------------------------------------------------------------------
//  _indent()
//  04-Nov-98   floh    created
//--------------------------------------------------------------------
static void _indent(long i, char *buf)
{
    long j;
    buf[0] = 0;
    for (j=0; j<i; j++) strcat(buf,"\t");
}

//--------------------------------------------------------------------
//  WriteComment()
//  04-Nov-98   floh    created
//--------------------------------------------------------------------
bool nTclServer::WriteComment(nFile *file, const char *str)
{
    n_assert(file);
    n_assert(str);
    file->PutS("# ");
    file->PutS(str);
    file->PutS("\n");
    return true;
}

//--------------------------------------------------------------------
//  write_select_statement()
//  Write the statement to select an object after its creation
//  statement.
//  05-Oct-00   floh    created
//--------------------------------------------------------------------
void nTclServer::write_select_statement(nFile* file, nRoot *o, nRoot *owner)
{
    switch (this->GetSelectMethod()) {

        case SELCOMMAND:
            // get relative path from owner to o and write select statement
            char relpath[N_MAXPATH];
            _indent(++this->indent_level, this->indent_buf);
            owner->GetRelPath(o, relpath, sizeof(relpath));
            
            file->PutS(this->indent_buf);
            file->PutS("sel ");
            file->PutS(relpath);
            file->PutS("\n");
            break;

        case NOSELCOMMAND:
            break;
    }
}

//--------------------------------------------------------------------
//  WriteBeginNewObject()
//  Write start of persistent object with default constructor.
//  06-Mar-00   floh    created
//  05-Sep-00   floh    + support for optional enter/leave syntax
//--------------------------------------------------------------------
bool nTclServer::WriteBeginNewObject(nFile* file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);
    const char *o_name  = o->GetName();

    // write generic 'new' statement
    const char *o_class = o->GetClass()->GetName();
    _indent(this->indent_level,this->indent_buf);

    file->PutS(this->indent_buf);
    file->PutS("new ");
    file->PutS(o_class);
    file->PutS(" ");
    file->PutS(o_name);
    file->PutS("\n");

    // write select object statement
    this->write_select_statement(file, o, owner);
    return true;
}

//--------------------------------------------------------------------
//  WriteBeginNewObjectCmd()
//  Write start of persistent object with custom constructor
//  defined by command.
//  06-Mar-00   floh    created
//--------------------------------------------------------------------
bool nTclServer::WriteBeginNewObjectCmd(nFile* file, nRoot *o, nRoot *owner, nCmd *cmd)
{
    n_assert(file);
    n_assert(o);
    n_assert(cmd);

    // write constructor statement
    this->WriteCmd(file, cmd);

    // write select object statement
    this->write_select_statement(file, o, owner);
    return true;
}

//--------------------------------------------------------------------
//  WriteBeginSelObject()
//  Write start of persisting object without constructor, only
//  write the select statement.
//  06-Mar-00   floh    created
//--------------------------------------------------------------------
bool nTclServer::WriteBeginSelObject(nFile* file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);

    // write select object statement
    this->write_select_statement(file, o, owner);
    return true;
}

//--------------------------------------------------------------------
//  WriteEndObject()
//  04-Nov-98   floh    created
//  06-Mar-00   floh    + write relative path from o to owner
//                        into select statement, instead of
//                        hardwiring to 'sel ..'
//  05-Oct-00   floh    + write unselect statement
//--------------------------------------------------------------------
bool nTclServer::WriteEndObject(nFile* file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);

    // get relative path from owner to o and write select statement
    char relpath[N_MAXPATH];
    _indent(--this->indent_level, this->indent_buf);
    o->GetRelPath(owner, relpath, sizeof(relpath));

    file->PutS(this->indent_buf);
    file->PutS("sel ");
    file->PutS(relpath);
    file->PutS("\n");

    return true;
}

//--------------------------------------------------------------------
//  WriteCmd()
//  04-Nov-98   floh    created
//  12-Jan-99   floh    verwendet jetzt den "neuen" Syntax, nicht
//                      mehr "do".
//  08-Aug-99   floh    nCmd-Objekte kennen nicht mehr unmittelbar
//                      ihren Namen, haben aber einen Pointer auf
//                      ihren nCmdProto eingebettet
//  12-Oct-99   floh    - String-Args werden mit einem umschliessenden
//                        " " geschrieben
//                      - Support fuer ARGTYPE_OBJECT
//  09-Nov-99   floh    + Support fuer "null" Objects
//--------------------------------------------------------------------
bool nTclServer::WriteCmd(nFile* file, nCmd *cmd)
{
    n_assert(file);
    n_assert(cmd);
    const char *name = cmd->GetProto()->GetName();
    n_assert(name);
    nArg *arg;

    // write the command name
    file->PutS(this->indent_buf);
    file->PutS(".");
    file->PutS(name);

    // write command arguments
    cmd->Rewind();
    int num_args = cmd->GetNumInArgs();

    const char* strPtr;
    ushort strLen;
    ushort bufLen;

    int i;
    for (i=0; i<num_args; i++) 
    {
        char buf[N_MAXPATH];
        arg=cmd->In();

        switch(arg->GetType()) {

            case nArg::ARGTYPE_INT:
                sprintf(buf, " %d", arg->GetI());
                break;

            case nArg::ARGTYPE_FLOAT:
                sprintf(buf, " %.6f", arg->GetF());
                break;

            case nArg::ARGTYPE_STRING:
                strPtr = arg->GetS();
                strLen = strlen(strPtr);
                bufLen = sizeof(buf)-1;
            
                file->PutS(" \"");
                if (strLen > bufLen-1) 
                {
                    buf[bufLen] = 0; // Null terminator
                    for (int j=0; j<strLen-2; j+=bufLen)
                    {
                        memcpy((void*)&buf[0], strPtr, bufLen);
                        file->PutS(buf);
                        strPtr += bufLen;     
                    }
                strPtr += bufLen;
                }
                sprintf(buf, "%s\"", strPtr);
                break;

            case nArg::ARGTYPE_CODE:
                sprintf(buf, " { %s }", arg->GetC());
                break;

            case nArg::ARGTYPE_BOOL:
                sprintf(buf," %s",(arg->GetB() ? "true" : "false"));
                break;

            case nArg::ARGTYPE_OBJECT:
                {
                    nRoot *o = (nRoot *) arg->GetO();
                    if (o) {
                        char buf[N_MAXPATH];
                        sprintf(buf, " %s", o->GetFullName(buf, sizeof(buf)));
                    } 
                    else 
                    {
                        sprintf(buf, " null");
                    }
                }
                break;

            default:
                sprintf(buf, " ???");
                break;
        }
        file->PutS(buf);
    }
    return file->PutS("\n");
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------





