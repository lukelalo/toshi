#define N_IMPLEMENTS nStaticMeshEmitter
//------------------------------------------------------------------------------
//  nstaticmeshemitter_cmds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "particle/nstaticmeshemitter.h"
#include "kernel/npersistserver.h"

static void n_setmeshnode(void*, nCmd*);
static void n_getmeshnode(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nstaticmeshemitter

    @superclass
    npemitter

    @classinfo
    Emit static particles from mesh vertices.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setmeshnode_s", 'SMSN', n_setmeshnode);
    cl->AddCmd("s_getmeshnode_v", 'GMSN', n_getmeshnode);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmeshnode

    @input
    s (MeshNodePath)

    @output
    v

    @info
    Defines the source mesh which provides the particle positions, must be
    a direct child of the nstaticmeshemitter, must be 'readonly'.
*/
static void
n_setmeshnode(void* slf, nCmd* cmd)
{
    nStaticMeshEmitter* self = (nStaticMeshEmitter*) slf;
    self->SetMeshNode(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmeshnode

    @input
    v

    @output
    s (MeshNodePath)

    @info
    Get the mesh node as defined by 'setmeshnode'.
*/
static void
n_getmeshnode(void* slf, nCmd* cmd)
{
    nStaticMeshEmitter* self = (nStaticMeshEmitter*) slf;
    cmd->Out()->SetS(self->GetMeshNode());
}

//------------------------------------------------------------------------------
/**
*/
bool
nStaticMeshEmitter::SaveCmds(nPersistServer* fs)
{
    bool retval = false;
    
    if (nPEmitter::SaveCmds(fs))
    {
        nCmd* cmd;

        //--- setmeshnode ---
        cmd = fs->GetCmd(this, 'SMSN');
        cmd->In()->SetS(this->GetMeshNode());
        fs->PutCmd(cmd);

        retval = true;
    }
    return retval;
}

//------------------------------------------------------------------------------





























