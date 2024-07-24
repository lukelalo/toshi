#define N_IMPLEMENTS nSceneGraph2
//------------------------------------------------------------------------------
//  nscenegraph2_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/nscenegraph2.h"

static void n_setenablemiplod(void*, nCmd*);
static void n_getenablemiplod(void*, nCmd*);
static void n_beginmiplod(void*, nCmd*);
static void n_setmiplod(void*, nCmd*);
static void n_endmiplod(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nscenegraph2

    @superclass
    nroot

    @classinfo
    Scenegraph management and rendering class.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setenablemiplod_b", 'SEML', n_setenablemiplod);
    cl->AddCmd("b_getenablemiplod_v", 'GEML', n_getenablemiplod);
    cl->AddCmd("v_beginmiplod_i",     'BGML', n_beginmiplod);
    cl->AddCmd("v_setmiplod_if",      'STML', n_setmiplod);
    cl->AddCmd("v_endmiplod_v",       'EDML', n_endmiplod);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setenablemiplod
    @input
    b(MipLODEnabled)
    @output
    v
    @info
    Enable/Disable mipmap LOD handling. Enabling mipmap lod handling may
    reduce video memory useage, because mipmap levels are uploaded based
    on the object's distance to viewer.
*/
static
void
n_setenablemiplod(void* slf, nCmd* cmd)
{
    nSceneGraph2* self = (nSceneGraph2*) slf;
    self->SetEnableMipLod(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getenablemiplod
    @input
    v
    @output
    b(MipLODEnabled)
    @info
    Get the enabled/disabled state of mipmap lod handling.
*/
static
void
n_getenablemiplod(void* slf, nCmd* cmd)
{
    nSceneGraph2* self = (nSceneGraph2*) slf;
    cmd->Out()->SetB(self->GetEnableMipLod());
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginmiplod
    @input
    i(NumTableEntries)
    @output
    v
    @info
    Begin defining the mipmap LOD table. This is a table of distances where
    the more detailed mipmap levels are dropped.
*/
static
void
n_beginmiplod(void* slf, nCmd* cmd)
{
    nSceneGraph2* self = (nSceneGraph2*) slf;
    self->BeginMipLod(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmiplod
    @input
    i(TableIndex), f(Distance)
    @output
    v
    @info
    Set the distance for in the mipmap lod table.
*/
static
void
n_setmiplod(void* slf, nCmd* cmd)
{
    nSceneGraph2* self = (nSceneGraph2*) slf;
    int i0   = cmd->In()->GetI();
    float f0 = cmd->In()->GetF();
    self->SetMipLod(i0, f0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endmiplod
    @input
    v
    @output
    v
    @info
    Finish defining the mipmap lod table
*/
static
void
n_endmiplod(void* slf, nCmd* /*cmd*/)
{
    nSceneGraph2* self = (nSceneGraph2*) slf;
    self->EndMipLod();
}

//------------------------------------------------------------------------------
