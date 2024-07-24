#define N_IMPLEMENTS nMeshNode
//-------------------------------------------------------------------
//  nmeshnode_cmds.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "node/nmeshnode.h"
#include "kernel/npersistserver.h"
#include "gfx/ngfxtypes.h"

static void n_setfilename(void*, nCmd*);
static void n_getfilename(void*, nCmd*);
static void n_setreadonly(void*, nCmd*);
static void n_getreadonly(void*, nCmd*);
static void n_setcastshadow(void*, nCmd*);
static void n_getcastshadow(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmeshnode

    @superclass
    nvisnode

    @classinfo
    Describe static mesh object in a visual hierarchy which
    can load itself from a Wavefront file.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setfilename_s",   'SFLN', n_setfilename);
    cl->AddCmd("s_getfilename_v",   'GFLN', n_getfilename);
    cl->AddCmd("v_setreadonly_b",   'SRDO', n_setreadonly);
    cl->AddCmd("b_getreadonly_v",   'GRDO', n_getreadonly);
    cl->AddCmd("v_setcastshadow_b", 'SCSD', n_setcastshadow);
    cl->AddCmd("b_getcastshadow_v", 'GCSD', n_getcastshadow);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfilename

    @input
    s (WavefrontFilename)

    @output
    v

    @info
    Set the filename of the wavefront file which describes the
    mesh geometry.
*/
static void n_setfilename(void *o, nCmd *cmd)
{
    nMeshNode *self = (nMeshNode *) o;
    self->SetFilename(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfilename

    @input
    v

    @output
    s (WavefrontFilename)

    @info
    Get the filename of the wavefront file which describes the
    mesh geometry.
*/
static void n_getfilename(void *o, nCmd *cmd)
{
    nMeshNode *self = (nMeshNode *) o;
    cmd->Out()->SetS(self->GetFilename());
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
    Set this to true if the mesh is a 'readonly' mesh. Readonly
    meshes are never rendered, but can provide source data for
    mesh animator object. The default is 'false'.
*/
static void n_setreadonly(void *o, nCmd *cmd)
{
    nMeshNode *self = (nMeshNode *) o;
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
    Get the value defined by 'setreadonly'.
*/
static void n_getreadonly(void *o, nCmd *cmd)
{
    nMeshNode *self = (nMeshNode *) o;
    cmd->Out()->SetB(self->GetReadOnly());
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
    Turn on/off shadow casting for this object.
*/
static void n_setcastshadow(void *o, nCmd *cmd)
{
    nMeshNode *self = (nMeshNode *) o;
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
    Get shadow casting state of this object.
*/
static void n_getcastshadow(void *o, nCmd *cmd)
{
    nMeshNode *self = (nMeshNode *) o;
    cmd->Out()->SetB(self->GetCastShadow());
}

//-------------------------------------------------------------------
//  SaveCmds()
//  05-Sep-00   floh    created
//-------------------------------------------------------------------
bool nMeshNode::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;
    
        //--- setfilename ---
        cmd = fs->GetCmd(this, 'SFLN');
        cmd->In()->SetS(this->GetFilename());
        fs->PutCmd(cmd);

        //--- setreadonly ---
        if (this->GetReadOnly())
        {
            cmd = fs->GetCmd(this, 'SRDO');
            cmd->In()->SetB(this->GetReadOnly());
            fs->PutCmd(cmd);
        }

        //--- setcastshadow ---
        if (this->GetCastShadow())
        {
            cmd = fs->GetCmd(this, 'SCSD');
            cmd->In()->SetB(this->GetCastShadow());
            fs->PutCmd(cmd);
        }

        retval = true;
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
