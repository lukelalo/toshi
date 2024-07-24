#define N_IMPLEMENTS nENetServer
//------------------------------------------------------------------------------
/* Copyright (c) 2003 Bruce Mitchener, Jr.
 *
 * See the file "nenet_license.txt" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------
#include "nenet/nenetserver.h"
#include "kernel/npersistserver.h"

static void n_createhost(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nenetserver

    @superclass
    nroot

    @classinfo
    ...
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("o_createhost_s",        'CRHS', n_createhost);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    createhost

    @input
    s (Name for host object)

    @output
    o (The host)

    @info
*/
static void n_createhost(void *slf, nCmd *cmd)
{
    nENetServer *self = (nENetServer *)slf;
    cmd->Out()->SetO(self->CreateHost(cmd->In()->GetS()));
}

