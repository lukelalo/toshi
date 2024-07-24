#define N_IMPLEMENTS nScriptServer
#define N_KERNEL
//--------------------------------------------------------------------
//  nscriptrun.cc -- Interaktiv- und Server-Modus des
//                     Scriptservers.
//
//  (C) 1998 Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kernel/nscriptserver.h"

//--------------------------------------------------------------------
/**
     - 31-Oct-98   floh    uebernommen aus nTclServer
*/
//--------------------------------------------------------------------
bool nScriptServer::RunCommand(nCmd *)
{
    n_error("nScriptServer::RunCommand(): pure virtual function called!");
    return FALSE;
}

bool nScriptServer::WriteComment(nFile*, const char*)
{
    n_error("nScriptServer::WriteComment(): pure virtual function called!");
    return false;
}
//--------------------------------------------------------------------
/**
     - 23-Feb-99   floh    created
*/
//--------------------------------------------------------------------
char *nScriptServer::Prompt(char *, int)
{
    n_error("pure virtual function called!");
    return NULL;
}

//--------------------------------------------------------------------
/**
    Trigger should be called in regular intervals, if the ScriptServer
    is not in interactive mode.  This ensures that any events which
    need to be processed will be.
    
    @return true to remain running, false to exit the application.

    - 21-Dec-98   floh    created
    - 31-Aug-99   floh    'quit_requested' wird zurueckgesetzt, wenn
                          es einmal true war
*/
//--------------------------------------------------------------------
bool nScriptServer::Trigger(void)
{
    bool retval = !(this->quitRequested);
    this->quitRequested = false;
    return retval;
}

//--------------------------------------------------------------------
//    EOF
//--------------------------------------------------------------------


