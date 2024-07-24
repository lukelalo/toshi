#define N_IMPLEMENTS nMeshCluster
//-------------------------------------------------------------------
//  nmeshcluster_cmds.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "node/nmeshcluster.h"
#include "kernel/npersistserver.h"

static void n_setreadonly(void *, nCmd *);
static void n_getreadonly(void *, nCmd *);
static void n_setskinmesh(void *, nCmd *);
static void n_getskinmesh(void *, nCmd *);
static void n_setrootjoint(void *, nCmd *);
static void n_getrootjoint(void *, nCmd *);
static void n_setcastshadow(void*, nCmd*);
static void n_getcastshadow(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmeshcluster

    @superclass
    nvisnode

    @classinfo
    Perform weighted vertex skinning from a static skin mesh
    and a joint hierarchy.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setreadonly_b",   'SRDO', n_setreadonly);
    cl->AddCmd("b_getreadonly_v",   'GRDO', n_getreadonly);
    cl->AddCmd("v_setskinmesh_s",   'SSKM', n_setskinmesh);
    cl->AddCmd("s_getskinmesh_v",   'GSKM', n_getskinmesh);
    cl->AddCmd("v_setrootjoint_s",  'SRJT', n_setrootjoint);
    cl->AddCmd("s_getrootjoint_v",  'GRJT', n_getrootjoint);
    cl->AddCmd("v_setcastshadow_b", 'SCSH', n_setcastshadow);
    cl->AddCmd("b_getcastshadow_v", 'GCSH', n_getcastshadow);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setreadonly

    @input
    b (ReadOnly)

    @output
    v

    @info
    Must be set to true if the meshcluster is the source for another
    dynamic vertex buffer class. Otherwise (if its going to be rendered),
    set it to false. Default is false.
*/
static void n_setreadonly(void *o, nCmd *cmd)
{
    nMeshCluster *self = (nMeshCluster *) o;
    self->SetReadOnly(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getreadonly

    @input
    v

    @output
    b (ReadOnly)

    @info
    Return the status of the readonly flag.
*/
static void n_getreadonly(void *o, nCmd *cmd)
{
    nMeshCluster *self = (nMeshCluster *) o;
    cmd->Out()->SetB(self->GetReadOnly());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setskinmesh

    @input
    s (SkinMeshProvider)

    @output
    v

    @info
    Define the path to the object which provides the original
    skin mesh. This is usually a nmeshnode object. The skin mesh
    object must have weight and joint index vertex components
    (usually provided by the Wavefront file).
*/
static void n_setskinmesh(void *o, nCmd *cmd)
{
    nMeshCluster *self = (nMeshCluster *) o;
    self->SetSkinMesh(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getskinmesh

    @input
    v

    @output
    s (SkinMeshProvider)

    @info
    Get the path to the skin mesh.
*/
static void n_getskinmesh(void *o, nCmd *cmd)
{
    nMeshCluster *self = (nMeshCluster *) o;
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
    Define the relative path to the root object of the joint
    hierarchy.
*/
static void n_setrootjoint(void *o, nCmd *cmd)
{
    nMeshCluster *self = (nMeshCluster *) o;
    self->SetRootJoint(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrootjoint

    @input
    v

    @output
    s (RootJointPath)

    @info
    Return the relative path to the root joint.
*/
static void n_getrootjoint(void *o, nCmd *cmd)
{
    nMeshCluster *self = (nMeshCluster *) o;
    cmd->Out()->SetS(self->GetRootJoint());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcastshadow

    @input
    b (CastShadow)

    @output
    v

    @info
    Turn shadow casting on/off for this object.
*/
static void n_setcastshadow(void* o, nCmd* cmd)
{
    nMeshCluster* self = (nMeshCluster*) o;
    self->SetCastShadow(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcastshadow

    @input
    v

    @output
    b (CastShadow)

    @info
    Get shadow casting state.
*/
static void n_getcastshadow(void* o, nCmd* cmd)
{
    nMeshCluster* self = (nMeshCluster*) o;
    cmd->Out()->SetB(self->GetCastShadow());
}

//-------------------------------------------------------------------
//  SaveCmds()
//  23-Oct-00   floh    created
//  06-Nov-00   floh    + beginjoints/setjoint/endjoints killed,
//                      + added setrootjoint
//-------------------------------------------------------------------
bool nMeshCluster::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;

        //--- setreadonly ---
        cmd = fs->GetCmd(this,'SRDO');
        cmd->In()->SetB(this->GetReadOnly());
        fs->PutCmd(cmd);

        //--- setskinmesh ---
        cmd = fs->GetCmd(this,'SSKM');
        cmd->In()->SetS(this->GetSkinMesh());
        fs->PutCmd(cmd);

        //--- setrootjoint ---
        cmd = fs->GetCmd(this,'SRJT');
        cmd->In()->SetS(this->GetRootJoint());
        fs->PutCmd(cmd);

        //--- setcastshadow ---
        cmd = fs->GetCmd(this, 'SCSH');
        cmd->In()->SetB(this->GetCastShadow());
        fs->PutCmd(cmd);
        
        retval = true;
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
