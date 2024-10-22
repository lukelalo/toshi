#define N_IMPLEMENTS nMapNode
//-----------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo.
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#include "map/nmapnode.h"
#include "kernel/npersistserver.h"

static void n_setmap(void* slf, nCmd* cmd);
static void n_getmap(void* slf, nCmd* cmd);
static void n_setblocksize(void* slf, nCmd* cmd);
static void n_getblocksize(void* slf, nCmd* cmd);
static void n_seterror(void* slf, nCmd* cmd);
static void n_geterror(void* slf, nCmd* cmd);
static void n_setdetailsize(void* slf, nCmd* cmd);
static void n_getdetailsize(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmapnode
    
    @superclass
    nvisnode

    @classinfo
    The nmap provides an alternative implementation of a terrain.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setmap_s", 'SMAP', n_setmap);
    clazz->AddCmd("s_getmap_v", 'GMAP', n_getmap);
    clazz->AddCmd("v_setblocksize_i", 'SBLK', n_setblocksize);
    clazz->AddCmd("i_getblocksize_v", 'GBLK', n_getblocksize);
    clazz->AddCmd("v_seterror_i", 'SERR', n_seterror);
    clazz->AddCmd("i_geterror_v", 'GERR', n_geterror);
    clazz->AddCmd("v_setdetailsize_f", 'SDTS', n_setdetailsize);
    clazz->AddCmd("f_getdetailsize_v", 'GDTS', n_getdetailsize);
    clazz->EndCmds();
}

//-------------------------------------------------------------------
/**
    @cmd
    setmap

    @input
    s(MapSource)

    @output
    v

    @info
    Set the node to obtain heightmap from.
*/
//-------------------------------------------------------------------
void n_setmap(void *o, nCmd *cmd)
{
    nMapNode *self = (nMapNode *) o;
    self->SetMapPath(cmd->In()->GetS());
}

//-------------------------------------------------------------------
/**
    @cmd
    getmap

    @input
    v

    @output
    s (MapSource)

    @info
    Return the current map source.
*/
//-------------------------------------------------------------------
void n_getmap(void *o, nCmd *cmd)
{
    nMapNode *self = (nMapNode *) o;
    cmd->Out()->SetS(self->GetMapPath());
}

//-------------------------------------------------------------------
/**
    @cmd
    setblocksize

    @input
    i (TerrainBlockSize)

    @output
    v

    @info
    Set the terrain block size which is used to divide up terrain.
    This must be an odd number.
*/
//-------------------------------------------------------------------
void n_setblocksize(void* o, nCmd *cmd)
{
    nMapNode* self = (nMapNode*)o;
    self->SetBlockSize(cmd->In()->GetI());
}

//-------------------------------------------------------------------
/**
    @cmd
    getblocksize

    @input
    v

    @output
    i (TerrainBlockSize)

    @info
    Set the terrain block size.
*/
//-------------------------------------------------------------------
void n_getblocksize(void* o, nCmd* cmd)
{
    nMapNode* self = (nMapNode*)o;
    cmd->Out()->SetI(self->GetBlockSize());
}

//-------------------------------------------------------------------
/**
    @cmd
    seterror

    @input
    i (Error)

    @output
    v

    @info
    Set the pixel pop error, causes a recalculation of the terrain.
*/
//-------------------------------------------------------------------
void n_seterror(void* o, nCmd *cmd)
{
    nMapNode* self = (nMapNode*)o;
    self->SetError(cmd->In()->GetI());
}

//-------------------------------------------------------------------
/**
    @cmd
    geterror

    @input
    v

    @output
    i (Error)

    @info
    Get the pixel pop error
*/
//-------------------------------------------------------------------
void n_geterror(void* o, nCmd* cmd)
{
    nMapNode* self = (nMapNode*)o;
    cmd->Out()->SetI(self->GetError());
}

//-------------------------------------------------------------------
/**
    @cmd
    setdetailsize

    @input
    f (DetailSize)

    @output
    v

    @info
    Sets the size of the detail texture in metres.  The detail
    texture is then repeated across the entire terrain.
*/
//-------------------------------------------------------------------
void n_setdetailsize(void *o, nCmd *cmd)
{
    nMapNode *self = (nMapNode*)o;
    self->SetDetailSize(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    getdetailsize

    @input
    v

    @output
    f (DetailSize)

    @info
    Gets the size of the detail texture in metres.
*/
//-------------------------------------------------------------------
void n_getdetailsize(void *o, nCmd *cmd)
{
    nMapNode *self = (nMapNode*)o;
    cmd->Out()->SetF(self->GetDetailSize());
}

//------------------------------------------------------------------------------
/**
    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nMapNode::SaveCmds(nPersistServer* ps)
{
    if (nVisNode::SaveCmds(ps))
    {
        nCmd* cmd = NULL;

        // setmap
        cmd = ps->GetCmd(this, 'SMAP');
        if (NULL != cmd)
        {
            cmd->In()->SetS(GetMapPath());
            ps->PutCmd(cmd);
        }

        // setblocksize
        cmd = ps->GetCmd(this, 'SBLK');
        if (NULL != cmd)
        {
            cmd->In()->SetI(GetBlockSize());
            ps->PutCmd(cmd);
        }

        // setdetailsize
        cmd = ps->GetCmd(this, 'SDTS');
        if (NULL != cmd)
        {
            cmd->In()->SetF(GetDetailSize());
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
