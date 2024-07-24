#define N_IMPLEMENTS nProfileServer
//------------------------------------------------------------------------------
//  nprofileserver_cmds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "misc/nprofileserver.h"
#include "kernel/nkernelserver.h"

static void n_start(void* slf, nCmd* cmd);
static void n_stop(void* slf, nCmd* cmd);
static void n_toggle(void* slf, nCmd* cmd);
static void n_isprofiling(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nprofileserver
    
    @superclass
    nroot

    @classinfo
    Parent class of profile servers.
*/
void n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_start_v",         'STRT', n_start);
    cl->AddCmd("v_stop_v",          'STOP', n_stop);
    cl->AddCmd("v_toggle_v",        'TGGL', n_toggle);
    cl->AddCmd("b_isprofiling_v",   'ISPR', n_isprofiling);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    start

    @input
    v

    @output
    v

    @info
    Start profiling.
*/
static void n_start(void* slf, nCmd* /*cmd*/)
{
    nProfileServer* self = (nProfileServer*) slf;
    self->Start();
}

//------------------------------------------------------------------------------
/**
    @cmd
    stop

    @input
    v

    @output
    v

    @info
    Stop profiling.
*/
static void n_stop(void* slf, nCmd* /*cmd*/)
{
    nProfileServer* self = (nProfileServer*) slf;
    self->Stop();
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
    Toggle profiling.
*/
static void n_toggle(void* slf, nCmd* /*cmd*/)
{
    nProfileServer* self = (nProfileServer*) slf;
    self->Toggle();
}

//------------------------------------------------------------------------------
/**
    @cmd
    isprofiling

    @input
    v

    @output
    b (IsProfiling)

    @info
    Get current profiling on/off status.
*/
static void n_isprofiling(void* slf, nCmd* /*cmd*/)
{
    nProfileServer* self = (nProfileServer*) slf;
    self->IsProfiling();
}

//------------------------------------------------------------------------------
