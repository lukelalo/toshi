#define N_IMPLEMENTS nMeshEmitter
//-------------------------------------------------------------------
//  nmeshemitter_cmds.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "particle/nmeshemitter.h"

static void n_setmeshnode(void *, nCmd *);
static void n_getmeshnode(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmeshemitter

    @superclass
    npservemitter

    @classinfo
    Emit particles from meshnode vertices.
*/
void n_initcmds(nClass *cl)
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
    Define relative path to object which should provide the
    vertex buffer used for emitting particles. Set this
    object to inactive to prevent it from rendering its own
    vertex buffer.
*/
static void n_setmeshnode(void *o, nCmd *cmd)
{
    nMeshEmitter *self = (nMeshEmitter *) o;
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
    Return relative path to object providing vertex buffer.
*/
static void n_getmeshnode(void *o, nCmd *cmd)
{
    nMeshEmitter *self = (nMeshEmitter *) o;
    cmd->Out()->SetS(self->GetMeshNode());
}

//-------------------------------------------------------------------
//  SaveCmds()
//  01-Nov-00   floh    created
//-------------------------------------------------------------------
bool nMeshEmitter::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nPEmitter::SaveCmds(fs)) {
        nCmd *cmd;

        //--- setmeshnode ---
        cmd = fs->GetCmd(this,'SMSN');
        cmd->In()->SetS(this->GetMeshNode());
        fs->PutCmd(cmd);
        
        retval = true;
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
