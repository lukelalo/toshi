#define N_IMPLEMENTS nConServer
//-------------------------------------------------------------------
//  misc/ncsrv_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "misc/nconserver.h"

static void n_open(void *, nCmd *);
static void n_close(void *, nCmd *);
static void n_toggle(void *, nCmd *);
static void n_watch(void *, nCmd *);
static void n_unwatch(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nconserver

    @superclass
    nroot

    @classinfo
    The console server can be found at "/sys/servers/console", it 
    implements an interactive console. 
    For the console server to be functioning, the following objects 
    have to exist: 
    /sys/servers/input 
    /sys/servers/gfx 
    /sys/servers/script 
    
    The console server is activated via the "esc (escape)" key and 
    from there on it intercepts all key events. If activated the complete 
    key input will be interpreted as command line input and the 
    resulting commands will be passed to the script server. 
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("b_open_v",'OCON',n_open);
    cl->AddCmd("b_close_v",'CCON',n_close);
    cl->AddCmd("v_toggle_v",'TCON',n_toggle);
    cl->AddCmd("v_watch_s",'WTCH',n_watch);
    cl->AddCmd("v_unwatch_v",'UWTC',n_unwatch);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    open

    @input
    v

    @output
    b (Success)

    @info
    Activates the console, can be also done with 
    a key press (escape). 
*/
static void n_open(void *o, nCmd *cmd)
{
    nConServer *self = (nConServer *) o;
    cmd->Out()->SetB(self->Open());
}

//------------------------------------------------------------------------------
/**
    @cmd
    close

    @input
    v

    @output
    b (Success)

    @info
    Closes the console. 
*/
static void n_close(void *o, nCmd *cmd)
{
    nConServer *self = (nConServer *) o;
    cmd->Out()->SetB(self->Close());
}

//------------------------------------------------------------------------------
/**
    @cmd
    toggle

    @input
    v

    @output
    v

    @info
    Toggles the console state. 
*/
static void n_toggle(void *o, nCmd *)
{
    nConServer *self = (nConServer *) o;
    self->Toggle();
}

//------------------------------------------------------------------------------
/**
    @cmd
    watch

    @input
    s (Pattern)

    @output
    v

    @info
    Allows you to monitor the value of any status variables
    under '/sys/var'. You can use pattern matching to 
    decide which variables will be shown. The watch values
    are only shown when the console is closed.
*/
static void n_watch(void *o, nCmd *cmd)
{
    nConServer *self = (nConServer *) o;
    self->Watch(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    unwatch

    @input
    v

    @output
    v

    @info
    Stop watching any variables.
*/
static void n_unwatch(void *o, nCmd *)
{
    nConServer *self = (nConServer *) o;
    self->Unwatch();
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
