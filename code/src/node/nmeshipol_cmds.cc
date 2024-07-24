#define N_IMPLEMENTS nMeshIpol
//-------------------------------------------------------------------
//  nmeshipol_cmds.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "node/nmeshipol.h"
#include "node/nmeshnode.h"

static void n_setreadonly(void *, nCmd *);
static void n_getreadonly(void *, nCmd *);
static void n_beginkeys(void *, nCmd *);
static void n_setkey(void *, nCmd *);
static void n_endkeys(void *, nCmd *);
static void n_getnumkeys(void *, nCmd *);
static void n_getkey(void *, nCmd *);
static void n_setupdatecoord(void *, nCmd *);
static void n_getupdatecoord(void *, nCmd *);
static void n_setupdatenorm(void *, nCmd *);
static void n_getupdatenorm(void *, nCmd *);
static void n_setupdatecolor(void *, nCmd *);
static void n_getupdatecolor(void *, nCmd *);
static void n_setupdateuv0(void *, nCmd *);
static void n_getupdateuv0(void *, nCmd *);
static void n_setupdateuv1(void *, nCmd *);
static void n_getupdateuv1(void *, nCmd *);
static void n_setupdateuv2(void *, nCmd *);
static void n_getupdateuv2(void *, nCmd *);
static void n_setupdateuv3(void *, nCmd *);
static void n_getupdateuv3(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmeshipol

    @superclass
    nanimnode

    @classinfo
    Interpolate between any number of meshes. Mesh structure
    (number of vertices, and how they are connected to primitives)
    must be identical. Only the values of the vertex components
    may differ.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setreadonly_b",       'SRDO', n_setreadonly);
    cl->AddCmd("b_getreadonly_v",       'GRDO', n_getreadonly);
    cl->AddCmd("v_beginkeys_i",         'BKEY', n_beginkeys);
    cl->AddCmd("v_setkey_ifs",          'SKEY', n_setkey);
    cl->AddCmd("v_endkeys_v",           'EKEY', n_endkeys);
    cl->AddCmd("i_getnumkeys_v",        'GNKS', n_getnumkeys);
    cl->AddCmd("fs_getkey_i",           'GKEY', n_getkey);
    cl->AddCmd("v_setupdatecoord_b",    'SUCD', n_setupdatecoord);
    cl->AddCmd("b_getupdatecoord_v",    'GUCD', n_getupdatecoord);
    cl->AddCmd("v_setupdatenorm_b",     'SUNM', n_setupdatenorm);
    cl->AddCmd("b_getupdatenorm_v",     'GUNM', n_getupdatenorm);
    cl->AddCmd("v_setupdatecolor_b",    'SUCL', n_setupdatecolor);
    cl->AddCmd("b_getupdatecolor_v",    'GUCL', n_getupdatecolor);
    cl->AddCmd("v_setupdateuv0_b",      'SUV0', n_setupdateuv0);
    cl->AddCmd("b_getupdateuv0_v",      'GUV0', n_getupdateuv0);
    cl->AddCmd("v_setupdateuv1_b",      'SUV1', n_setupdateuv1);
    cl->AddCmd("b_getupdateuv1_v",      'GUV1', n_getupdateuv1);
    cl->AddCmd("v_setupdateuv2_b",      'SUV2', n_setupdateuv2);
    cl->AddCmd("b_getupdateuv2_v",      'GUV2', n_getupdateuv2);
    cl->AddCmd("v_setupdateuv3_b",      'SUV3', n_setupdateuv3);
    cl->AddCmd("b_getupdateuv3_v",      'GUV3', n_getupdateuv3);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setreadonly

    @input
    b (ReadOnly)

    @output
    v

    @info
    Set this flag to true if the object serves as a source for
    another mesh animation object. This is necessary because 
    meshes which are optimized for rendering have very poor
    read access, while meshes which are optimized for read/write
    cannot be rendered.
*/
static void n_setreadonly(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    self->SetReadOnly(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getreadonly

    @input
    v

    @output
    b (ReadOnly)

    @info
    Get the status of the readonly flag.
*/
static void n_getreadonly(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    cmd->Out()->SetB(self->GetReadOnly());
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginkeys

    @input
    i (NumKeys)

    @output
    v

    @info
    Begin defining keyframes. Expects number of keyframes.
*/
static void n_beginkeys(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    self->BeginKeys(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setkey

    @input
    i (KeyIndex), f (TimeStamp), s (ObjectPath)

    @output
    v

    @info
    Set a keyframe. The object must be of class nmeshnode.
*/
static void n_setkey(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    int i   = cmd->In()->GetI();
    float f = cmd->In()->GetF();
    const char *s = cmd->In()->GetS();
    self->SetKey(i,f,s);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endkeys

    @input
    v

    @output
    v

    @info
    End defining key frames.
*/
static void n_endkeys(void *o, nCmd *)
{
    nMeshIpol *self = (nMeshIpol *) o;
    self->EndKeys();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumkeys

    @input
    v

    @output
    i (NumKeys)

    @info
    Get number of keyframes.
*/
static void n_getnumkeys(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    cmd->Out()->SetI(self->GetNumKeys());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkey

    @input
    i (KeyIndex)

    @output
    f (TimeStamp), s (ObjectPath)

    @info
    Get a keyframe definition.
*/
static void n_getkey(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    float f;
    const char *s;
    self->GetKey(cmd->In()->GetI(),f,s);
    cmd->Out()->SetF(f);
    cmd->Out()->SetS(s);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setupdatecoord
    setupdatenorm
    setupdatecolor
    setupdateuv0
    setupdateuv1
    setupdateuv2
    setupdateuv3

    @input
    b (Update)

    @output
    v

    @info
    Explicitly enable/disable interpolation of vertex components.
    By default, all vertex components are disabled. Disabling
    components that don't need to be updated is more efficient.
*/
static void n_setupdatecoord(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    self->SetUpdateCoord(cmd->In()->GetB());
}
static void n_setupdatenorm(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    self->SetUpdateNorm(cmd->In()->GetB());
}
static void n_setupdatecolor(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    self->SetUpdateColor(cmd->In()->GetB());
}
static void n_setupdateuv0(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    self->SetUpdateUv0(cmd->In()->GetB());
}
static void n_setupdateuv1(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    self->SetUpdateUv1(cmd->In()->GetB());
}
static void n_setupdateuv2(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    self->SetUpdateUv2(cmd->In()->GetB());
}
static void n_setupdateuv3(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    self->SetUpdateUv3(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getupdatecoord
    getupdatenorm
    getupdatecolor
    getupdateuv0
    getupdateuv1
    getupdateuv2
    getupdateuv3

    @input
    v

    @output
    b (Update)

    @info
    Get the enable/disable state of vertex component interpolation.
*/
static void n_getupdatecoord(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    cmd->Out()->SetB(self->GetUpdateCoord());
}
static void n_getupdatenorm(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    cmd->Out()->SetB(self->GetUpdateNorm());
}
static void n_getupdatecolor(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    cmd->Out()->SetB(self->GetUpdateColor());
}
static void n_getupdateuv0(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    cmd->Out()->SetB(self->GetUpdateUv0());
}
static void n_getupdateuv1(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    cmd->Out()->SetB(self->GetUpdateUv1());
}
static void n_getupdateuv2(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    cmd->Out()->SetB(self->GetUpdateUv2());
}
static void n_getupdateuv3(void *o, nCmd *cmd)
{
    nMeshIpol *self = (nMeshIpol *) o;
    cmd->Out()->SetB(self->GetUpdateUv3());
}

//-------------------------------------------------------------------
//  SaveCmds()
//  21-Sep-00   floh    created
//-------------------------------------------------------------------
bool nMeshIpol::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nAnimNode::SaveCmds(fs)) {
        nCmd *cmd;

        //--- beginkeys,setkey,endkeys ---
        int num = this->GetNumKeys();
        if (num > 0) {
            if ((cmd = fs->GetCmd(this,'BKEY'))) {
                cmd->In()->SetI(num);
                fs->PutCmd(cmd);
            }
            int i;
            for (i=0; i<num; i++) {
                if ((cmd = fs->GetCmd(this,'SKEY'))) {
                    float f;
                    const char *s;
                    this->GetKey(i,f,s);
                    cmd->In()->SetI(i);
                    cmd->In()->SetF(f);
                    cmd->In()->SetS(s);
                    fs->PutCmd(cmd);
                }
            }
            if ((cmd = fs->GetCmd(this,'EKEY'))) {
                fs->PutCmd(cmd);
            }
        }

        //--- setreadonly ---
        cmd = fs->GetCmd(this,'SRDO');
        cmd->In()->SetB(this->GetReadOnly());
        fs->PutCmd(cmd);

        //--- setupdatecoord ---
        if ((cmd = fs->GetCmd(this,'SUCD'))) {
            cmd->In()->SetB(this->GetUpdateCoord());
            fs->PutCmd(cmd);
        }

        //--- setupdatenorm ---
        if ((cmd = fs->GetCmd(this,'SUNM'))) {
            cmd->In()->SetB(this->GetUpdateNorm());
            fs->PutCmd(cmd);
        }

        //--- setupdatecolor ---
        if ((cmd = fs->GetCmd(this,'SUCL'))) {
            cmd->In()->SetB(this->GetUpdateColor());
            fs->PutCmd(cmd);
        }

        //--- setupdateuv0 ---
        if ((cmd = fs->GetCmd(this,'SUV0'))) {
            cmd->In()->SetB(this->GetUpdateUv0());
            fs->PutCmd(cmd);
        }

        //--- setupdateuv1 ---
        if ((cmd = fs->GetCmd(this,'SUV1'))) {
            cmd->In()->SetB(this->GetUpdateUv1());
            fs->PutCmd(cmd);
        }

        //--- setupdateuv2 ---
        if ((cmd = fs->GetCmd(this,'SUV2'))) {
            cmd->In()->SetB(this->GetUpdateUv2());
            fs->PutCmd(cmd);
        }

        //--- setupdateuv3 ---
        if ((cmd = fs->GetCmd(this,'SUV3'))) {
            cmd->In()->SetB(this->GetUpdateUv3());
            fs->PutCmd(cmd);
        }
        retval = true;
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
