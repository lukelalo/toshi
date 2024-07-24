#define N_IMPLEMENTS nShadowControl
//------------------------------------------------------------------------------
//  nshadowcontrol_cmds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shadow/nshadowcontrol.h"
#include "kernel/npersistserver.h"

static void n_setcolor(void*, nCmd*);
static void n_getcolor(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nshadowcontrol
    
    @superclass
    nvisnode

    @classinfo
    Control global attributes of shadow server (like shadow color).
*/
void
n_initcmds(nClass* cl)
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
    f(Red), f(Blue), f(Green), f(Alpha)

    @output
    v

    @info
    Set global shadow color (alpha defines transparency).
*/
static void n_setcolor(void* slf, nCmd* cmd)
{
    nShadowControl* self = (nShadowControl*) slf;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    float f3 = cmd->In()->GetF();
    self->SetColor(f0, f1, f2, f3);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcolor

    @input
    v

    @output
    f(Red), f(Blue), f(Green), f(Alpha)

    @info
    Get global shadow color
*/
static void n_getcolor(void* slf, nCmd* cmd)
{
    nShadowControl* self = (nShadowControl*) slf;
    float f0, f1, f2, f3;
    self->GetColor(f0, f1, f2, f3);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
    cmd->Out()->SetF(f2);
    cmd->Out()->SetF(f3);
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowControl::SaveCmds(nPersistServer* fs)
{
    if (nVisNode::SaveCmds(fs))
    {
        nCmd* cmd;
        float f0, f1, f2, f3;

        //--- setcolor ---
        cmd = fs->GetCmd(this, 'SCOL');
        this->GetColor(f0, f1, f2, f3);
        cmd->In()->SetF(f0);
        cmd->In()->SetF(f1);
        cmd->In()->SetF(f2);
        cmd->In()->SetF(f3);
        fs->PutCmd(cmd);
    
        return true;        
    }
    return false;
}

//------------------------------------------------------------------------------
