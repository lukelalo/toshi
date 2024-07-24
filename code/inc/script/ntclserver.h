#ifndef N_TCLSERVER_H
#define N_TCLSERVER_H
//--------------------------------------------------------------------
/** 
    @class nTclServer
    @ingroup ScriptServices

    @brief wraps around Tcl interpreter

    Implements an nScriptServer that understands Tcl, extended
    by a few Nebula specific Tcl commands and the ability
    to route script cmds to Nebula C++ objects.
*/
//--------------------------------------------------------------------
#include <tcl.h>

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

#ifndef N_SCRIPTSERVER_H
#include "kernel/nscriptserver.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nTclServer
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nFileServer2;
class nTclServer : public nScriptServer {
public:
    static nClass *local_cl;
    static nKernelServer *kernelServer;
        
    nAutoRef<nFileServer2> refFileServer;
    Tcl_Interp *interp;
    long indent_level;
    char indent_buf[N_MAXPATH];
    bool print_error;
    bool is_standalone_tcl;

public:
    nTclServer();
    virtual ~nTclServer();
    
    virtual bool Run(const char *, const char*&);
    virtual bool RunScript(const char *, const char*&);
    virtual bool RunCommand(nCmd *);
    virtual char *Prompt(char *, int);
    
    virtual nFile* BeginWrite(const char* filename, nRoot* obj);  
    virtual bool WriteComment(nFile *, const char *);
    virtual bool WriteBeginNewObject(nFile *, nRoot *, nRoot *);
    virtual bool WriteBeginNewObjectCmd(nFile *, nRoot *, nRoot *, nCmd *);
    virtual bool WriteBeginSelObject(nFile *, nRoot *, nRoot *);
    virtual bool WriteCmd(nFile *, nCmd *);
    virtual bool WriteEndObject(nFile *, nRoot *, nRoot *);
    virtual bool EndWrite(nFile *);
    
    virtual bool Trigger(void);

    virtual void InitAsExtension(Tcl_Interp *);

private:
    void write_select_statement(nFile *, nRoot *, nRoot *);
    void link_to_interp(Tcl_Interp *, bool);
    void unlink_from_interp(Tcl_Interp *, bool);
};
//--------------------------------------------------------------------
#endif
