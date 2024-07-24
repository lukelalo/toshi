#define N_IMPLEMENTS nMeshCluster2
//------------------------------------------------------------------------------
//  nmeshcluster2_cmds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "node/nmeshcluster2.h"
#include "kernel/npersistserver.h"

static void n_setreadonly(void* slf, nCmd* cmd);
static void n_getreadonly(void* slf, nCmd* cmd);
static void n_setskinmesh(void* slf, nCmd* cmd);
static void n_getskinmesh(void* slf, nCmd* cmd);
static void n_setrootjoint(void* slf, nCmd* cmd);
static void n_getrootjoint(void* slf, nCmd* cmd);
static void n_setcastshadow(void* slf, nCmd* cmd);
static void n_getcastshadow(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmeshcluster2
    
    @superclass
    nvisnode

    @classinfo
    Replaces the nmeshcluster class, works with njoint2 skeletons
    and is thus somewhat more efficient and logical.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setreadonly_b",   'SRDO', n_setreadonly);
    cl->AddCmd("b_getreadonly_v",   'GRDO', n_getreadonly);
    cl->AddCmd("v_setskinmesh_s",   'SSMS', n_setskinmesh);
    cl->AddCmd("s_getskinmesh_v",   'GSMS', n_getskinmesh);
    cl->AddCmd("v_setrootjoint_s",  'SRTJ', n_setrootjoint);
    cl->AddCmd("s_getrootjoint_v",  'GRTJ', n_getrootjoint);
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
    nMeshCluster2* self = (nMeshCluster2*) slf;
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
    nMeshCluster2* self = (nMeshCluster2*) slf;
    cmd->Out()->SetB(self->GetReadOnly());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setskinmesh
    @input
    s(SkinMeshPath)
    @output
    v
    @info
    Define the path to the object which provides the original
    skin mesh. This is usually a nmeshnode object. The skin mesh
    object must have weight and joint index vertex components
    (usually provided by the Wavefront file).  
*/
static
void
n_setskinmesh(void* slf, nCmd* cmd)
{
    nMeshCluster2* self = (nMeshCluster2*) slf;
    self->SetSkinMesh(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getskinmesh
    @input
    v
    @output
    s(SkinMeshPath)
    @info
    Get the skin mesh object.
*/
static
void
n_getskinmesh(void* slf, nCmd* cmd)
{
    nMeshCluster2* self =(nMeshCluster2*) slf;
    cmd->Out()->SetS(self->GetSkinMesh());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setrootjoint
    @input
    s (RootJointPath)
    @output
    v
    @info
    Define the root joint of the bone skeleton.
*/
static
void
n_setrootjoint(void* slf, nCmd* cmd)
{
    nMeshCluster2* self = (nMeshCluster2*) slf;
    self->SetRootJoint(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrootjoint
    @input
    v
    @output
    s(RootJointPath)
    @info
    Get the root joint of the bone skeleton.
*/
static
void
n_getrootjoint(void* slf, nCmd* cmd)
{
    nMeshCluster2* self =(nMeshCluster2*) slf;
    cmd->Out()->SetS(self->GetRootJoint());
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
    nMeshCluster2* self = (nMeshCluster2*) slf;
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
    nMeshCluster2* self = (nMeshCluster2*) slf;
    cmd->Out()->SetB(self->GetCastShadow());
}

//------------------------------------------------------------------------------
/**
*/
bool
nMeshCluster2::SaveCmds(nPersistServer* fs)
{
    if (nVisNode::SaveCmds(fs))
    {
        nCmd* cmd;

        //--- setreadonly ---
        cmd = fs->GetCmd(this, 'SRDO');
        cmd->In()->SetB(this->GetReadOnly());
        fs->PutCmd(cmd);

        //--- setskinmesh ---
        cmd = fs->GetCmd(this, 'SSMS');
        cmd->In()->SetS(this->GetSkinMesh());
        fs->PutCmd(cmd);

        //--- setrootjoint ---
        cmd = fs->GetCmd(this, 'SRTJ');
        cmd->In()->SetS(this->GetRootJoint());
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
