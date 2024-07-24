#define N_IMPLEMENTS nThreshNode
//-------------------------------------------------------------------
//  nthreshnode_cmds.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "node/nthreshnode.h"
#include "kernel/npersistserver.h"

static void n_setlowerbound(void *, nCmd *);
static void n_getlowerbound(void *, nCmd *);
static void n_setupperbound(void *, nCmd *);
static void n_getupperbound(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nthreshnode

    @superclass
    nanimnode

    @classinfo
    Disable/enable trigger stream to child objects based on
    current channel value.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setlowerbound_f", 'SLWB', n_setlowerbound);
    cl->AddCmd("f_getlowerbound_v", 'GLWB', n_getlowerbound);
    cl->AddCmd("v_setupperbound_f", 'SUPB', n_setupperbound);
    cl->AddCmd("f_getupperbound_v", 'GUPB', n_getupperbound);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlowerbound

    @input
    f (LowerBound)

    @output
    v

    @info
    Set the lower bound of the channel range from which
    child objects should be triggered. Default value is 0.0 .
*/
static void n_setlowerbound(void *o, nCmd *cmd)
{
    nThreshNode *self = (nThreshNode *) o;
    self->SetLowerBound(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlowerbound

    @input
    v

    @output
    f (LowerBound)

    @info
    Get the current lower bound value.
*/
static void n_getlowerbound(void *o, nCmd *cmd)
{
    nThreshNode *self = (nThreshNode *) o;
    cmd->Out()->SetF(self->GetLowerBound());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setupperbound

    @input
    f (UpperBound)

    @output
    v

    @info
    Set the upper bound of the channel range to which
    child objects should be triggered. Default value is 1.0 .
*/
static void n_setupperbound(void *o, nCmd *cmd)
{
    nThreshNode *self = (nThreshNode *) o;
    self->SetUpperBound(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getupperbound

    @input
    v

    @output
    f (UpperBound)

    @info
    Get the current upper bound value.
*/
static void n_getupperbound(void *o, nCmd *cmd)
{
    nThreshNode *self = (nThreshNode *) o;
    cmd->Out()->SetF(self->GetUpperBound());
}

//-------------------------------------------------------------------
//  SaveCmds()
//  17-Oct-00   floh    created
//-------------------------------------------------------------------
bool nThreshNode::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nAnimNode::SaveCmds(fs)) {
        nCmd *cmd;

        //--- setlowerbound ---
        cmd = fs->GetCmd(this,'SLWB');
        cmd->In()->SetF(this->GetLowerBound());
        fs->PutCmd(cmd);

        //--- setupperbound ---
        cmd = fs->GetCmd(this,'SUPB');
        cmd->In()->SetF(this->GetUpperBound());
        fs->PutCmd(cmd);

        retval = true;
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
