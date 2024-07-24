#define N_IMPLEMENTS nOverlayPlane
//-------------------------------------------------------------------
//  novrlay_cmds.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "node/noverlayplane.h"

static void n_setcolor(void *, nCmd *);
static void n_getcolor(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    noverlayplane

    @superclass
    nvisnode

    @classinfo
    Render an alpha overlay plane into the current scene.
    Depends on a nshadernode that provides surface properties.
    The nshadernode should be given srcalpha/invsrcalpha transparency
    and a high foreground render priority, lighting and fogging
    should be disabled.
    noverlayplane objects are usually not used in normal
    nvisnode hierarchies, because they will overwrite each other.
    Instead, they should be used under the nspecialfx server,
    which makes sure that overlay planes are used mutually exclusive.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setcolor_ffff", 'SCOL', n_setcolor);
    cl->AddCmd("ffff_getcolor_v", 'GCOL', n_getcolor);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcolor

    @input
    f(Red), f(Green), f(Blue), f(Alpha) 

    @output
    v

    @info
    Set the plane's color.
*/
static void n_setcolor(void *o, nCmd *cmd)
{
    nOverlayPlane *self = (nOverlayPlane *) o;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    float f3 = cmd->In()->GetF();
    self->SetColor(f0,f1,f2,f3);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcolor

    @input
    v

    @output
    f(Red), f(Green), f(Blue), f(Alpha) 

    @info
    Get the plane's color.
*/
static void n_getcolor(void *o, nCmd *cmd)
{
    nOverlayPlane *self = (nOverlayPlane *) o;
    float f0,f1,f2,f3;
    self->GetColor(f0,f1,f2,f3);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
    cmd->Out()->SetF(f2);
    cmd->Out()->SetF(f3);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------











