//------------------------------------------------------------------------------
/* Copyright (c) 2003 Leaf Garland.
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#define N_IMPLEMENTS nAreaEmitter
//-------------------------------------------------------------------
//  nareaemit_cmds.cc
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "particle/nareaemitter.h"

static void n_setareasize(void *, nCmd *);
static void n_getareasize(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nareaemitter

    @superclass
    npservemitter

    @classinfo
    Emit particles from a definable area.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setareasize_fff",'SEAZ',n_setareasize);
    cl->AddCmd("fff_getareasize_v",'GEAZ',n_getareasize);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setareasize

    @input
    f (width), f (height), f (depth)

    @output
    v

    @info
    Defines the area that particles can be emitted from. The area
    is centered around the node's position.
*/
static void n_setareasize(void *o, nCmd *cmd)
{
    nAreaEmitter *self = (nAreaEmitter *) o;
    float w = cmd->In()->GetF();
    float h = cmd->In()->GetF();
    float d = cmd->In()->GetF();
    self->SetAreaSize(w,h,d);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getareasize

    @input
    v

    @output
    f (width), f (height), f (depth)

    @info
    Returns the area that particles can be emitted from.
*/
static void n_getareasize(void *o, nCmd *cmd)
{
    nAreaEmitter *self = (nAreaEmitter *) o;
    float w,h,d;
    self->GetAreaSize(w,h,d);
    cmd->Out()->SetF(w);
    cmd->Out()->SetF(h);
    cmd->Out()->SetF(d);
}

//------------------------------------------------------------------------------
/**
    Emit commands to make this object persistent.

    @param  fs      file server which makes the object persistent
    @return         true if all ok
*/
bool nAreaEmitter::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nPEmitter::SaveCmds(fs)) {
        nCmd *cmd;

        //--- setareasize ---
        float w,h,d;
        this->GetAreaSize(w,h,d);
        cmd = fs->GetCmd(this,'SEAZ');
        cmd->Out()->SetF(w);
        cmd->Out()->SetF(h);
        cmd->Out()->SetF(d);
        fs->PutCmd(cmd);
        
        retval = true;
    }
    return retval;
}
