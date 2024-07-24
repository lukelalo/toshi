#define N_IMPLEMENTS nLinkNode
//-------------------------------------------------------------------
//  node/nlink_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/nlinknode.h"

static void n_settarget(void *, nCmd *);
static void n_gettarget(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nlinknode

    @superclass
    nvisnode

    @classinfo
    Defines a variable 'Link' to another VisNode object which
    can be changed at runtime. Can be integrated in a visual
    hierarchy as a 'placeholder' object if the targetobject must
    change or will be created at runtime.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_settarget_s", 'STGT', n_settarget);
    cl->AddCmd("s_gettarget_v", 'GTGT', n_gettarget);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    settarget

    @input
    s (TargetObjectPath)

    @output
    v

    @info
    Defines the target nvisnode object to be triggered instead
    of the nlinknode. The target object does not need to exist at
    the time the script command is called. nlinknode uses nDynAutoRef
    which can resolve itself later (usually when the nlinknode is
    first rendered).
*/
static void n_settarget(void *o, nCmd *cmd)
{
    nLinkNode *self = (nLinkNode *) o;
    self->SetTarget(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettarget

    @input
    v

    @output
    s (TargetObjectPath)

    @info
    Return the via 'settarget' defined target object.
*/
static void n_gettarget(void *o, nCmd *cmd)
{
    nLinkNode *self = (nLinkNode *) o;
    cmd->Out()->SetS(self->GetTarget());
}

//-------------------------------------------------------------------
//  SaveCmds()
//  29-Sep-00   floh    created
//-------------------------------------------------------------------
bool nLinkNode::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;
        
        //--- settarget ---
        const char *s = this->GetTarget();
        if (s) {
            cmd = fs->GetCmd(this,'STGT');
            cmd->In()->SetS(s);
            fs->PutCmd(cmd);
        }
        retval = true;
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
