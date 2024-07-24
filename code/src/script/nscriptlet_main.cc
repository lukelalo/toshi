#define N_IMPLEMENTS nScriptlet
//------------------------------------------------------------------------------
//  © 2001 Radon Labs GmbH
//
#include "kernel/npersistserver.h"
#include "kernel/nscriptserver.h"
#include "script/nscriptlet.h"

//------------------------------------------------------------------------------
/**
*/
nScriptlet::nScriptlet() :
    refScript(kernelServer, this),
    refFile(kernelServer, this),
    script(NULL),
    autoRun(false)
{
    this->refScript = "/sys/servers/script";
    this->refFile   = "/sys/servers/file2";
}

//------------------------------------------------------------------------------
/**
*/
nScriptlet::~nScriptlet()
{
    this->FreeScriptBuffer();
}

//------------------------------------------------------------------------------
/**
*/
void
nScriptlet::FreeScriptBuffer()
{
    if (this->script)
    {
        n_free((void*) this->script);
        this->script = 0;
    }
}

//------------------------------------------------------------------------------
/**
    @param  b   true if auto run should be activated
*/
void
nScriptlet::SetAutoRun(bool b)
{
    this->autoRun = b;
}

//------------------------------------------------------------------------------
/**
    @return     the status of the auto run flag
*/
bool
nScriptlet::GetAutoRun()
{
    return this->autoRun;
}

//------------------------------------------------------------------------------
/**
    Set the scriptlet as a string. If AutoRun is active, immediately feed
    it to the script server. In this case, the result of the evaluation
    will be thrown away.

    @param  script      the script to execute as a string
*/
void
nScriptlet::SetScript(const char* str)
{
    n_assert(str);

    // copy script over    
    this->FreeScriptBuffer();
    this->script = n_strdup(str);

    // optionally evaluate the script
    if (this->autoRun)
    {
        this->Run();
    }
}

//------------------------------------------------------------------------------
/**
    @return     the current embedded script, or NULL, if none defined
*/
const char*
nScriptlet::GetScript()
{
    return this->script;
}

//------------------------------------------------------------------------------
/**
    Feeds the current script into the default script server ("/sys/servers/script")
    and return the result.

    @return     the result of evaluating the script, NULL if no scriptlet defined
*/
const char*
nScriptlet::Run()
{
    if (this->script)
    {
        const char* result;
        this->refScript->Run(this->script, result);
        return result;
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Read a script file into the internal scriptlet buffer line by line. 
    Throws away comments and replaces newlines by semicolons to fit the
    whole script into a single line of code.

    Should be overwritten by subclasses to handle language specific stuff.

    @param      file        name of script file, may contain Nebula assigns
    @return                 true if script successfully parsed
*/
bool
nScriptlet::ParseFile(const char*)
{
    n_error("Pure virtual function called!\n");
    return true;
}


//------------------------------------------------------------------------------
