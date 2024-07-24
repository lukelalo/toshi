#define N_IMPLEMENTS nScriptlet
//------------------------------------------------------------------------------
//  © 2001 Radon Labs GmbH

#include "script/nscriptlet.h"
#include "kernel/npersistserver.h"

static void n_setautorun(void*, nCmd*);
static void n_getautorun(void*, nCmd*);
static void n_parsefile(void*, nCmd*);
static void n_setscript(void*, nCmd*);
static void n_getscript(void*, nCmd*);
static void n_run(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nscriptlet

    @superclass
    nroot

    @classinfo
    Embeds a script inside a Nebula object. Dont use this class directly,
    instead use the language specific subclasses (e.g. ntclscriptlet)
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setautorun_b", 'SARN', n_setautorun);
    cl->AddCmd("b_getautorun_v", 'GARN', n_getautorun);
    cl->AddCmd("b_parsefile_s",  'PRSF', n_parsefile);
    cl->AddCmd("v_setscript_c",  'SSCR', n_setscript);
    cl->AddCmd("c_getscript_v",  'GSCR', n_getscript);
    cl->AddCmd("s_run_v",        'RUN_', n_run);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setautorun

    @input
    b (AutoRun)

    @output
    v

    @info
    If AutoRun is true, the script will immediatly be evaluated by the
    default script server (/sys/servers/script) when the 'setscript'
    command is issued. This is useful when the script defines only
    procs (which are precompiled on first parse by the Tcl interpreter
    and added to the proc list). The '.run' command will still work.
*/
static
void
n_setautorun(void* slf, nCmd* cmd)
{
    nScriptlet* self = (nScriptlet*) slf;
    self->SetAutoRun(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getautorun

    @input
    v

    @output
    b (AutoRun)

    @info
    Return the current status of the AutoRun flag.
*/
static
void
n_getautorun(void* slf, nCmd* cmd)
{
    nScriptlet* self = (nScriptlet*) slf;
    cmd->Out()->SetB(self->GetAutoRun());
}

//------------------------------------------------------------------------------
/**
    @cmd
    parsefile

    @input
    s (ScriptFile)

    @output
    b (Success)

    @info
    Read a (Tcl) script file, and converts it into the embedded 
    representation (this basically makes it no longer editable by hand,
    since comments and newlines will be removed, the whole script
    will be concatenated into a single line). The file name can 
    contain Nebula assigns. When saving the nscriptlet object,
    it will save out the parsed file contents through the command
    .setscript instead of the .parsefile command!
*/
static
void
n_parsefile(void* slf, nCmd* cmd)
{
    nScriptlet* self = (nScriptlet*) slf;
    cmd->Out()->SetB(self->ParseFile(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setscript

    @input
    s (Script)

    @output
    v

    @info
    Set a script as a string. For real world stuff, use the .parsefile command
    to read a tcl script file. If .setautorun has been set to true, the
    script will immediately be evaluated.
*/
static
void
n_setscript(void* slf, nCmd* cmd)
{
    nScriptlet* self = (nScriptlet*) slf;
    self->SetScript(cmd->In()->GetC());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getscript

    @input
    v

    @output
    s (Script)

    @info
    Get the script defined by .setscript. Beware, the returned string can be 
    verylong.
*/
static
void
n_getscript(void* slf, nCmd* cmd)
{
    nScriptlet* self = (nScriptlet*) slf;
    cmd->Out()->SetC(self->GetScript());
}

//------------------------------------------------------------------------------
/**
    @cmd
    run

    @input
    v

    @output
    s (Result)

    @info
    Run the embedded script through the default script server 
    (/sys/servers/script).
*/
static
void
n_run(void* slf, nCmd* cmd)
{
    nScriptlet* self = (nScriptlet*) slf;
    cmd->Out()->SetS(self->Run());
}

//------------------------------------------------------------------------------
/**
    Fills nCmd object with the appropriate persistent data and passes the
    nCmd object on to the nPersistServer for output to a file.

    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nScriptlet::SaveCmds(nPersistServer* fs)
{
    if (nRoot::SaveCmds(fs))
    {
        nCmd* cmd;

        //--- setautorun ---
        cmd = fs->GetCmd(this, 'SARN');
        cmd->In()->SetB(this->GetAutoRun());
        fs->PutCmd(cmd);

        //--- setscript ---
        const char* s = this->GetScript();
        if (s)
        {
            cmd = fs->GetCmd(this, 'SSCR');
            cmd->In()->SetC(s);
            fs->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
