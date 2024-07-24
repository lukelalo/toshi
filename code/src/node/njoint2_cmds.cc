#define N_IMPLEMENTS nJoint2
//------------------------------------------------------------------------------
//  njoint2_cmds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "node/njoint2.h"
#include "kernel/npersistserver.h"

static void n_otxyz(void* slf, nCmd* cmd);
static void n_getot(void* slf, nCmd* cmd);
static void n_osxyz(void* slf, nCmd* cmd);
static void n_getos(void* slf, nCmd* cmd);
static void n_oqxyzw(void* slf, nCmd* cmd);
static void n_getoq(void* slf, nCmd *cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    njoint2
    
    @superclass
    n3dnode

    @classinfo
    Alternative joint class for skinned characters. Used by the
    new Maya-2-Nebula character exporter.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_otxyz_fff",   'OTXZ', n_otxyz);
    cl->AddCmd("fff_getot_v",   'GTOT', n_getot);
    cl->AddCmd("v_osxyz_fff",   'OSXZ', n_osxyz);
    cl->AddCmd("fff_getos_v",   'GTOS', n_getos);
    cl->AddCmd("v_oqxyzw_ffff", 'OQXZ', n_oqxyzw);
    cl->AddCmd("ffff_getoq_v",  'GTOQ', n_getoq);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    otxyz
    @input
    f(TranslateX), f(TranslateY), f(TranlateZ)
    @output
    v
    @info
    Defines the "relaxed pose" translation of the joint.
*/
static
void
n_otxyz(void* slf, nCmd* cmd)
{
    nJoint2* self =(nJoint2*) slf;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    self->OTxyz(f0, f1, f2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getot
    @input
    v
    @output
    f(TranslateX), f(TranslateY), f(TranlateZ)
    @info
    Return the relaxed pose translation.
*/
static
void
n_getot(void* slf, nCmd* cmd)
{
    nJoint2* self = (nJoint2*) slf;
    float f0, f1, f2;
    self->GetOT(f0, f1, f2);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
    cmd->Out()->SetF(f2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    osxyz
    @input
    f(ScaleX), f(ScaleY), f(ScaleZ)
    @output
    v
    @info
    Defines the "relaxed pose" scale of the joint.
*/
static
void
n_osxyz(void* slf, nCmd* cmd)
{
    nJoint2* self =(nJoint2*) slf;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    self->OSxyz(f0, f1, f2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getos
    @input
    v
    @output
    f(ScaleX), f(ScaleY), f(ScaleZ)
    @info
    Return the relaxed pose scale.
*/
static
void
n_getos(void* slf, nCmd* cmd)
{
    nJoint2* self = (nJoint2*) slf;
    float f0, f1, f2;
    self->GetOS(f0, f1, f2);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
    cmd->Out()->SetF(f2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    oqxyzw
    @input
    f(RotateX), f(RotateY), f(RotateZ), f(RotateW)
    @output
    v
    @info
    Defines the "relaxed pose" rotation as quaternion.
*/
static
void
n_oqxyzw(void* slf, nCmd* cmd)
{
    nJoint2* self =(nJoint2*) slf;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    float f3 = cmd->In()->GetF();
    self->OQxyzw(f0, f1, f2, f3);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getoq
    @input
    v
    @output
    f(RotateX), f(RotateY), f(RotateZ), f(RotateW)
    @info
    Return the relaxed pose rotation as quaternion.
*/
static
void
n_getoq(void* slf, nCmd* cmd)
{
    nJoint2* self = (nJoint2*) slf;
    float f0, f1, f2, f3;
    self->GetOQ(f0, f1, f2, f3);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
    cmd->Out()->SetF(f2);
    cmd->Out()->SetF(f3);
}

//------------------------------------------------------------------------------
/**
*/
bool
nJoint2::SaveCmds(nPersistServer* fs)
{
    if (n3DNode::SaveCmds(fs))
    {
        nCmd* cmd;
        float x,y,z,w;

        //--- otxyz ---
        this->GetOT(x, y, z);
        if ((x != 0.0f) || (y != 0.0f) || (z != 0.0f))
        {
            cmd = fs->GetCmd(this, 'OTXZ');
            cmd->In()->SetF(x);
            cmd->In()->SetF(y);
            cmd->In()->SetF(z);
            fs->PutCmd(cmd);
        }

        //--- oqxyzw ---
        this->GetOQ(x, y, z, w);
        if ((x != 0.0f) || (y != 0.0f) || (z != 0.0f) || (w != 1.0f))
        {
            cmd = fs->GetCmd(this, 'OQXZ');
            cmd->In()->SetF(x);
            cmd->In()->SetF(y);
            cmd->In()->SetF(z);
            cmd->In()->SetF(w);
            fs->PutCmd(cmd);
        }

        //--- osxyz ---
        this->GetOS(x, y, z);
        if ((x != 1.0f) || (y != 1.0f) || (z != 1.0f))
        {
            cmd = fs->GetCmd(this, 'OSXZ');
            cmd->In()->SetF(x);
            cmd->In()->SetF(y);
            cmd->In()->SetF(z);
            fs->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
