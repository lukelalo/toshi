#define N_IMPLEMENTS nBumpMeshNode
//------------------------------------------------------------------------------
/* Copyright (c) 2002 Dirk Ulbricht of Insane Software
 *
 * See the file "nbump_license.txt" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------
#include "node/nbumpmeshnode.h"
#include "kernel/npersistserver.h"

static void n_setreadonly(void* slf, nCmd* cmd);
static void n_getreadonly(void* slf, nCmd* cmd);
static void n_setmesh(void* slf, nCmd* cmd);
static void n_getmesh(void* slf, nCmd* cmd);
static void n_setlight(void* slf, nCmd* cmd);
static void n_getlight(void* slf, nCmd* cmd);
static void n_setcastshadow(void* slf, nCmd* cmd);
static void n_getcastshadow(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nBumpMeshNode
    
    @superclass
    nvisnode

    @classinfo
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setreadonly_b",   'SRDO', n_setreadonly);
    cl->AddCmd("b_getreadonly_v",   'GRDO', n_getreadonly);
    cl->AddCmd("v_setmesh_s",       'SSMS', n_setmesh);
    cl->AddCmd("s_getmesh_v",       'GSMS', n_getmesh);
    cl->AddCmd("v_setlight_s",      'SLMS', n_setlight);
    cl->AddCmd("s_getlight_v",      'GLMS', n_getlight);
    cl->AddCmd("v_setcastshadow_b", 'SCTS', n_setcastshadow);
    cl->AddCmd("b_getcastshadow_v", 'GCTS', n_getcastshadow);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setreadonly
    @input
    b(ReadOnly)
    @output
    v
    @info
    Must be set to true if the meshcluster is the source for another
    dynamic vertex buffer class. Otherwise (if its going to be rendered),
    set it to false. Default is false.
*/
static
void
n_setreadonly(void* slf, nCmd* cmd)
{
    nBumpMeshNode* self = (nBumpMeshNode*) slf;
    self->SetReadOnly(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getreadonly
    @input
    v
    @output
    b(ReadOnly)
    @info
    Get the read-only status.
*/
static
void
n_getreadonly(void* slf, nCmd* cmd)
{
    nBumpMeshNode* self = (nBumpMeshNode*) slf;
    cmd->Out()->SetB(self->GetReadOnly());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmesh
    @input
    s(MeshPath)
    @output
    v
    @info
    Define the path to the object which provides the original
    mesh. This is usually a nmeshnode object. The mesh object
    must have color components.
*/
static
void
n_setmesh(void* slf, nCmd* cmd)
{
    nBumpMeshNode* self = (nBumpMeshNode*) slf;
    self->SetMesh(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmesh
    @input
    v
    @output
    s(MeshPath)
    @info
    Get the mesh object.
*/
static
void
n_getmesh(void* slf, nCmd* cmd)
{
    nBumpMeshNode* self =(nBumpMeshNode*) slf;
    cmd->Out()->SetS(self->GetMesh());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlight
    @input
    s(LightPath)
    @output
    v
    @info
    Define the path to the object which provides the original
    mesh. This is usually a nmeshnode object. The mesh object
    must have color components.
*/
static
void
n_setlight(void* slf, nCmd* cmd)
{
    nBumpMeshNode* self = (nBumpMeshNode*) slf;
    self->SetLight(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlike
    @input
    v
    @output
    s(LightPath)
    @info
    Get the mesh object.
*/
static
void
n_getlight(void* slf, nCmd* cmd)
{
    nBumpMeshNode* self =(nBumpMeshNode*) slf;
    cmd->Out()->SetS(self->GetLight());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcastshadow
    @input
    b(CastShadow)
    @output
    v
    @info
    Set to true if this object should cast shadows. Please note that the
    skin mesh must also be marked as shadow caster!
*/
static
void
n_setcastshadow(void* slf, nCmd* cmd)
{
    nBumpMeshNode* self = (nBumpMeshNode*) slf;
    self->SetCastShadow(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcastshadow
    @input
    v
    @output
    b(CastShadow)
    @info
    Get the shadow caster stuff.
*/
static
void
n_getcastshadow(void* slf, nCmd* cmd)
{
    nBumpMeshNode* self = (nBumpMeshNode*) slf;
    cmd->Out()->SetB(self->GetCastShadow());
}

//------------------------------------------------------------------------------
/**
*/
bool
nBumpMeshNode::SaveCmds(nPersistServer* fs)
{
    if (nVisNode::SaveCmds(fs))
    {
        nCmd* cmd;

        //--- setreadonly ---
        cmd = fs->GetCmd(this, 'SRDO');
        cmd->In()->SetB(this->GetReadOnly());
        fs->PutCmd(cmd);

        //--- setmesh ---
        cmd = fs->GetCmd(this, 'SSMS');
        cmd->In()->SetS(this->GetMesh());
        fs->PutCmd(cmd);

        //--- setlight ---
        cmd = fs->GetCmd(this, 'SLMS');
        cmd->In()->SetS(this->GetLight());
        fs->PutCmd(cmd);

        //--- setcastshadow ---
        cmd = fs->GetCmd(this, 'SCTS');
        cmd->In()->SetB(this->GetCastShadow());
        fs->PutCmd(cmd);
        
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
