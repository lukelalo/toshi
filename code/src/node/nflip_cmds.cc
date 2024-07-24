#define N_IMPLEMENTS nFlipFlop
//-------------------------------------------------------------------
//  node/nflip_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include "kernel/npersistserver.h"
#include "node/nflipflop.h"

static void n_addkey(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nflipflop

    @superclass
    nanimnode

    @classinfo
    The nFlipFlop class selects one of its child nodes based on keyframes
    on a channel. This can be used to switch nodes at runtime, for 
    example to flip through a series of texture arrays, or to select a
    particular mesh based on a 'damage' channel. For each node you wish
    to flip to, add a keyframe with the channel value to select that node.
    The node will remain active until the next keyframe.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("b_addkey_fs",'AKEY',n_addkey);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addkey

    @input
    f (Time) s (Objectname)

    @output
    b (Success)

    @info
    Appends a new Keyframe to the nflipflop object.
    You provide a point in time on the timeline in seconds
    and the name of the subobject to be switched to active
    at the given time. A subobject with the passed name
    must already exist, otherwise false is returned.
*/
static void n_addkey(void *o, nCmd *cmd)
{
    nFlipFlop *self = (nFlipFlop *) o;
    {
        float t = cmd->In()->GetF();
        const char *n = cmd->In()->GetS();
        cmd->Out()->SetB(self->AddKey(t,n));
    }
}

//-------------------------------------------------------------------
//  SaveCmds()
//  02-Jan-99   floh    machine generated
//-------------------------------------------------------------------
bool nFlipFlop::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nAnimNode::SaveCmds(fs)) {
        nCmd *cmd;
        //--- addkey ---
        int i;
        for (i=0; i<this->num_keys; i++) {
            nObjectKey *k = &(this->keyarray[i]);
            cmd = fs->GetCmd(this, 'AKEY');
            cmd->In()->SetF(k->t);
            cmd->In()->SetS(k->o->GetName());
            fs->PutCmd(cmd);
        }
        retval = true;
    }
    return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------


