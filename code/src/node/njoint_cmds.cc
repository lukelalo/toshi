#define N_IMPLEMENTS nJoint
//-------------------------------------------------------------------
//  node/njoint_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/njoint.h"

static void n_oxyz(void *, nCmd *);
static void n_geto(void *, nCmd *);
static void n_oqxyzw(void *, nCmd *);
static void n_getoq(void *, nCmd *);
static void n_nxyz(void *, nCmd *);
static void n_getn(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    njoint

    @superclass
    n3dnode

    @classinfo
    Defines a joint in a hierarchical skeleton of bones. The njoint
    class is derived from n3dnode class, thus one can invoke
    the usual transformations on it (translate, rotate, scale).
    However, njoint has 2 additional attributes, its relaxed
    orientation and translation. A relaxed skeleton is a skeleton
    where all n3dnode transformation are on their default values
    (zeros translation/rotation, scale one). How the bones are
    actually positioned and oriented in the relaxed skeleton
    is defined by the default orientation and translation alone.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_oxyz_fff",    'OXYZ', n_oxyz);
    cl->AddCmd("fff_geto_v",    'GETO', n_geto);
    cl->AddCmd("v_oqxyzw_ffff", 'OQUA', n_oqxyzw);
    cl->AddCmd("ffff_getoq_v",  'GEOQ', n_getoq);
    cl->AddCmd("v_nxyz_fff",    'NXYZ', n_nxyz);
    cl->AddCmd("fff_getn_v",    'GETN', n_getn);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    oxyz

    @input
    f (OrientX), f (OrientY), f (OrientZ)

    @output
    v

    @info
    Define default orientation of the joint as 3 euler angles 
    (in degree).
*/
static void n_oxyz(void *o, nCmd *cmd)
{
    nJoint *self = (nJoint *) o;
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->Oxyz(x,y,z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    geto

    @input
    v

    @output
    f (OrientX), f (OrientY), f (OrientZ)

    @info
    Return the default orientation of the joint.
*/
static void n_geto(void *o, nCmd *cmd)
{
    nJoint *self = (nJoint *) o;
    float x,y,z;
    self->GetO(x,y,z);
    cmd->Out()->SetF(x);
    cmd->Out()->SetF(y);
    cmd->Out()->SetF(z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    oqxyzw

    @input
    f (Quat X), f (Quat Y), f (Quat Z) f (Quat Axis w)

    @output
    v

    @info
    Define default orientation of joint as quaternion. This
    is an alternative to the euler angle method.
*/
static void n_oqxyzw(void *o, nCmd *cmd)
{
    nJoint *self = (nJoint *) o;
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    float w = cmd->In()->GetF();
    self->OQxyzw(x,y,z,w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getoq

    @input
    v

    @output
    f (Quat X), f (Quat Y), f (Quat Z) f (Quat Axis w)

    @info
    Return the default orientation as set by 'oqxyzw'.
*/
static void n_getoq(void *o, nCmd *cmd)
{
    nJoint *self = (nJoint *) o;
    float x,y,z,w;
    self->GetOQ(x,y,z,w);
    cmd->Out()->SetF(x);
    cmd->Out()->SetF(y);
    cmd->Out()->SetF(z);
    cmd->Out()->SetF(w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    nxyz

    @input
    f (NormalX), f (NormalY), f (NormalZ)

    @output
    v

    @info
    Define the default translation of the joint.
*/
static void n_nxyz(void *o, nCmd *cmd)
{
    nJoint *self = (nJoint *) o;
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->Nxyz(x,y,z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getn

    @input
    v

    @output
    f (NormalX), f (NormalY), f (NormalZ)

    @info
    Return the default translation of the joint.
*/
static void n_getn(void *o, nCmd *cmd)
{
    nJoint *self = (nJoint *) o;
    float x,y,z;
    self->GetN(x,y,z);
    cmd->Out()->SetF(x);
    cmd->Out()->SetF(y);
    cmd->Out()->SetF(z);
}

//-------------------------------------------------------------------
//  SaveCmds()
//  02-Jan-99   floh    machine generated
//-------------------------------------------------------------------
bool nJoint::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (n3DNode::SaveCmds(fs)) {
        nCmd *cmd;
        float x,y,z,w;

        if (get(USEQUAT)) {
            //--- oqxyzw ---
            cmd = fs->GetCmd(this,'OQUA');
            this->GetOQ(x,y,z,w);
            cmd->In()->SetF(x);
            cmd->In()->SetF(y);
            cmd->In()->SetF(z);
            cmd->In()->SetF(w);
            fs->PutCmd(cmd);
        } else {
            //--- oxyz ---
            cmd = fs->GetCmd(this,'OXYZ');
            this->GetO(x,y,z);
            cmd->In()->SetF(x);
            cmd->In()->SetF(y);
            cmd->In()->SetF(z);
            fs->PutCmd(cmd);
        }

        //--- nxyz ---
        cmd = fs->GetCmd(this,'NXYZ');
        this->GetN(x,y,z);
        cmd->In()->SetF(x);
        cmd->In()->SetF(y);
        cmd->In()->SetF(z);
        fs->PutCmd(cmd);

        retval = true;
    }
    return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
