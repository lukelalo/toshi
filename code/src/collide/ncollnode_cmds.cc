#define N_IMPLEMENTS nCollideNode
//-------------------------------------------------------------------
//  ncollnode_cmds.cc
//  (C) RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "collide/ncollidenode.h"

static void n_setfilename(void *, nCmd *);
static void n_getfilename(void *, nCmd *);
static void n_setcollclass(void *, nCmd *);
static void n_getcollclass(void *, nCmd *);

//-------------------------------------------------------------------
/**
    @scriptclass
    ncollidenode

    @superclass
    nvisnode

    @classinfo
    Collision visnode subclass for collision system testing ONLY!
*/
//-------------------------------------------------------------------
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setfilename_s",  'SFIL', n_setfilename);
    cl->AddCmd("s_getfilename_v",  'GFIL', n_getfilename);
    cl->AddCmd("v_setcollclass_s", 'SCCL', n_setcollclass);
    cl->AddCmd("s_getcollclass_v", 'GCCL', n_getcollclass);
    cl->EndCmds();
}

//-------------------------------------------------------------------
/**
    @cmd
    setfilename

    @input
    s (WavefrontFilename)

    @output
    v

    @info
    Define filename where to load collision geometry from.
*/
//-------------------------------------------------------------------
static void n_setfilename(void *o, nCmd *cmd)
{
    nCollideNode *self = (nCollideNode *) o;
    self->SetFilename(cmd->In()->GetS());
}

//-------------------------------------------------------------------
/**
    @cmd
    getfilename

    @input
    v

    @output
    s (WavefrontFilename)

    @info
    Get current geometry description filename.
*/
//-------------------------------------------------------------------
static void n_getfilename(void *o, nCmd *cmd)
{
    nCollideNode *self = (nCollideNode *) o;
    cmd->Out()->SetS(self->GetFilename());
}

//-------------------------------------------------------------------
/**
    @cmd
    setcollclass

    @input
    s (CollClass)

    @output
    v

    @info
    Set the user defined collision class for this object.
*/
//-------------------------------------------------------------------
static void n_setcollclass(void *o, nCmd *cmd)
{
    nCollideNode *self = (nCollideNode *) o;
    self->SetCollClass(cmd->In()->GetS());
}

//-------------------------------------------------------------------
/**
    @cmd
    getcollclass

    @input
    v

    @output
    s (CollClass)

    @info
    Get the user defined collision class for this object.
*/
//-------------------------------------------------------------------
static void n_getcollclass(void *o, nCmd *cmd)
{
    nCollideNode *self = (nCollideNode *) o;
    cmd->Out()->SetS(self->GetCollClass());
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
