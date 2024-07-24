#define N_IMPLEMENTS n3DNode
//-------------------------------------------------------------------
//  node/n3dn_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/n3dnode.h"

static void n_setactive(void*, nCmd*);
static void n_getactive(void*, nCmd*);
static void n_setmaxlod(void*, nCmd*);
static void n_getmaxlod(void*, nCmd*);
static void n_sethighdetail(void*, nCmd*);
static void n_gethighdetail(void*, nCmd*);
static void n_setsprite(void *, nCmd *);
static void n_getsprite(void *, nCmd *);
static void n_setbillboard(void *, nCmd *);
static void n_getbillboard(void *, nCmd *);
static void n_setlockviewer(void *, nCmd *);
static void n_getlockviewer(void *, nCmd *);
static void n_setviewspace(void*, nCmd*);
static void n_getviewspace(void*, nCmd*);
static void n_txyz(void *, nCmd *);
static void n_tx(void *, nCmd *);
static void n_ty(void *, nCmd *);
static void n_tz(void *, nCmd *);
static void n_rxyz(void *, nCmd *);
static void n_rx(void *, nCmd *);
static void n_ry(void *, nCmd *);
static void n_rz(void *, nCmd *);
static void n_sxyz(void *, nCmd *);
static void n_sx(void *, nCmd *);
static void n_sy(void *, nCmd *);
static void n_sz(void *, nCmd *);
static void n_qxyzw(void *, nCmd *);
static void n_gett(void *, nCmd *);
static void n_getr(void *, nCmd *);
static void n_gets(void *, nCmd *);
static void n_getq(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    n3dnode

    @superclass
    nvisnode

    @classinfo
    A n3dnode object positions and groups its subobjects into
    space. n3dnode objects can be translated, scaled and rotated
    to describe position and orientation relative to a parent
    3d node (or in world space if the 3d node object is at the root
    level). Usually, a 3d node object has at least a nmeshnode, a 
    ntexarraynode and a nshadernode as child object to describe a 
    vanilla visual object. Such a tree of visual objects is also
    called a visual hierarchy.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setactive_b",     'SACT', n_setactive);
    cl->AddCmd("b_getactive_v",     'GACT', n_getactive);
    cl->AddCmd("v_setmaxlod_f",     'SMLD', n_setmaxlod);
    cl->AddCmd("f_getmaxlod_v",     'GMLD', n_getmaxlod);
    cl->AddCmd("v_sethighdetail_b", 'SHDT', n_sethighdetail);
    cl->AddCmd("b_gethighdetail_v", 'GHDT', n_gethighdetail);
    cl->AddCmd("v_setsprite_b",     'SSPR', n_setsprite);
    cl->AddCmd("b_getsprite_v",     'GSPR', n_getsprite);
    cl->AddCmd("v_setbillboard_b",  'SBLB', n_setbillboard);
    cl->AddCmd("b_getbillboard_v",  'GBLB', n_getbillboard);
    cl->AddCmd("v_setlockviewer_b", 'SLVW', n_setlockviewer);
    cl->AddCmd("b_getlockviewer_v", 'GLVW', n_getlockviewer);
    cl->AddCmd("v_setviewspace_b",  'SVWS', n_setviewspace);
    cl->AddCmd("b_getviewspace_v",  'GVWS', n_getviewspace);
    cl->AddCmd("v_txyz_fff",        'TXYZ', n_txyz);
    cl->AddCmd("v_tx_f",            'TX__', n_tx);
    cl->AddCmd("v_ty_f",            'TY__', n_ty);
    cl->AddCmd("v_tz_f",            'TZ__', n_tz);
    cl->AddCmd("v_rxyz_fff",        'RXYZ', n_rxyz);
    cl->AddCmd("v_rx_f",            'RX__', n_rx);
    cl->AddCmd("v_ry_f",            'RY__', n_ry);
    cl->AddCmd("v_rz_f",            'RZ__', n_rz);
    cl->AddCmd("v_qxyzw_ffff",      'QXYZ', n_qxyzw);
    cl->AddCmd("v_sxyz_fff",        'SXYZ', n_sxyz);
    cl->AddCmd("v_sx_f",            'SX__', n_sx);
    cl->AddCmd("v_sy_f",            'SY__', n_sy);
    cl->AddCmd("v_sz_f",            'SZ__', n_sz);
    cl->AddCmd("fff_gett_v",        'GETT', n_gett);
    cl->AddCmd("fff_getr_v",        'GETR', n_getr);
    cl->AddCmd("fff_gets_v",        'GETS', n_gets);
    cl->AddCmd("ffff_getq_v",       'GETQ', n_getq);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setactive

    @input
    b (ActiveFlag)

    @output
    v

    @info
    Used to activate/deactivate parts of a visual hierarchy.
    This makes everything under (and including) the n3dnode object
    invisible (or visible). Default state is of course 'active'.
*/
static void n_setactive(void* o, nCmd* cmd)
{
    n3DNode* self = (n3DNode*) o;
    self->SetActive(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getactive

    @input
    v

    @output
    b (ActiveFlag)

    @info
*/
static void n_getactive(void* o, nCmd* cmd)
{
    n3DNode* self = (n3DNode*) o;
    cmd->Out()->SetB(self->GetActive());
}

//-------------------------------------------------------------------
/**
    @cmd
    sethighdetail

    @input
    b (HighDetailFlag)

    @output
    v

    @info
    Set to true if this is a high detail node (default is true)
*/
//-------------------------------------------------------------------
static void n_sethighdetail(void* o, nCmd* cmd)
{
    n3DNode* self = (n3DNode*) o;
    self->SetHighDetail(cmd->In()->GetB());
}

//-------------------------------------------------------------------
/**
    @cmd
    gethighdetail

    @input
    v

    @output
    b (ActiveFlag)

    @info
    Get the high detail flag.
*/
//-------------------------------------------------------------------
static void n_gethighdetail(void* o, nCmd* cmd)
{
    n3DNode* self = (n3DNode*) o;
    cmd->Out()->SetB(self->GetHighDetail());
}

//-------------------------------------------------------------------
/**
    @cmd
    setmaxlod

    @input
    f (MinimalLodValue)

    @output
    v

    @info
    Sets the minimum lod value at which this object is visible.
    If the object is below that lod value, it will be invisible
    (the same result as if the object would be "setactive false").
    The default value is 0.0 (always visible).
*/
//-------------------------------------------------------------------
static void n_setmaxlod(void* o, nCmd* cmd)
{
    n3DNode* self = (n3DNode*) o;
    self->SetMaxLod(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    getmaxlod

    @input
    v

    @output
    f (MinimalLodValue)

    @info
    Get the current minimal lod value for this object.
*/
//-------------------------------------------------------------------
static void n_getmaxlod(void* o, nCmd* cmd)
{
    n3DNode* self = (n3DNode*) o;
    cmd->Out()->SetF(self->GetMaxLod());
}

//------------------------------------------------------------------------------
/**
    @cmd
    txyz

    @input
    f (TranslateX), f (TranslateY), f (TranslateZ)

    @output
    v

    @info
    Translate object to [x,y,z].
*/
static void n_txyz(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->Txyz(x,y,z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    tx

    @input
    f (TranslateX)

    @output
    v

    @info
    Translate object to [x].
*/
static void n_tx(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    self->Tx(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    ty

    @input
    f (TranslateY)

    @output
    v

    @info
    Translate object to [y].
*/
static void n_ty(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    self->Ty(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    tz

    @input
    f (TranslateZ)

    @output
    v

    @info
    Translate object to [z].
*/
static void n_tz(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    self->Tz(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    rxyz

    @input
    f (RotateX), f (RotateY), f (RotateZ)

    @output
    v

    @info
    Rotate object around [xyz] (in degree). Rotation order
    is always x->y->z. 
*/
static void n_rxyz(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->Rxyz(x,y,z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    rx

    @input
    f (RotateX)

    @output
    v

    @info
    Rotate object around [x].
*/
static void n_rx(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    self->Rx(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    ry

    @input
    f (RotateY)

    @output
    v

    @info
    Rotate object around [y].
*/
static void n_ry(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    self->Ry(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    rz

    @input
    f (RotateZ)

    @output
    v

    @info
    Rotate object around [z].
*/
static void n_rz(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    self->Rz(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    sxyz

    @input
    f (ScaleX), f (ScaleY), f (ScaleZ)

    @output
    v

    @info
    Scale object to [xyz].  Enabling scaling on an object will cause various
    other parts of Nebula to not work as expected.

     - Lighting will be incorrect unless setnormalizenormals is
       enabled on the shadernode.
     - Ray picking via nOpcodeServer will not recognize the scaled
       size of the object, instead using the original mesh size.
*/
static void n_sxyz(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->Sxyz(x,y,z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    sx

    @input
    f (ScaleX)

    @output
    v

    @info
    Scale object along [x]. See notes about scaling in the documentation
    for sxyz.
*/
static void n_sx(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    self->Sx(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    sy

    @input
    f (ScaleY)

    @output
    v

    @info
    Scale object along [y]. See notes about scaling in the documentation
    for sxyz.
*/
static void n_sy(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    self->Sy(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    sz

    @input
    f (ScaleZ)

    @output
    v

    @info
    Scale object along [z]. See notes about scaling in the documentation
    for sxyz.
*/
static void n_sz(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    self->Sz(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    qxyzw

    @input
    x(QuatX), y(QuatY), z(QuatZ), w(QuatW)

    @output
    v

    @info
    Set the quaternion defining orientation in space. This
    is an alternative method to rxyz (setting rxyz will disable
    qxyzw, and setting qxyzw will disable rxyz).
*/
static void n_qxyzw(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    float w = cmd->In()->GetF();
    self->Qxyzw(x,y,z,w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setsprite

    @input
    b (IsSprite)

    @output
    v

    @info
    Turn sprite behaviour on (true) or off (false). In sprite mode
    the 3d node will always orient its negative z axis towards
    the viewer, and the local y axis is aligned with the 
    viewers y axis.
*/
static void n_setsprite(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    self->SetSprite(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getsprite

    @input
    v

    @output
    b (IsSprite)

    @info
    Returns the sprite behaviour flag.
*/
static void n_getsprite(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    cmd->Out()->SetB(self->GetSprite());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setbillboard

    @input
    b (IsBillboard)

    @output
    v

    @info
    Turn billboard behavious on (true) or off (false). In billboard
    mode will orient its negative z axis towards the viewer, but with
    a locked y axis (thus, the object will rotate around its y axis).
*/
static void n_setbillboard(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    self->SetBillboard(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getbillboard

    @input
    v

    @output
    b (IsBillboard)

    @info
    Returns the billboard behaviour flag.
*/
static void n_getbillboard(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    cmd->Out()->SetB(self->GetBillboard());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlockviewer

    @input
    b (LockViewer)

    @output
    v

    @info
    Set or clear the 'lock to viewer' behaviour. In 'lock viewer' 
    mode the 3d node will always position itself to the
    current viewer position.
*/
static void n_setlockviewer(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    self->SetLockViewer(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlockviewer

    @input
    v

    @output
    b (LockViewer)

    @info
    Return the current 'lock viewer' flag.
*/
static void n_getlockviewer(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    cmd->Out()->SetB(self->GetLockViewer());
}

//-------------------------------------------------------------------
/**
    @cmd
    setviewspace

    @input
    b (ViewSpaceFlag)

    @output
    v

    @info
    Set to true if this node lives in view space.
*/
//-------------------------------------------------------------------
static void n_setviewspace(void* slf, nCmd* cmd)
{
    n3DNode* self = (n3DNode*) slf;
    self->SetViewSpace(cmd->In()->GetB());
}

//-------------------------------------------------------------------
/**
    @cmd
    getviewspace

    @input
    v

    @output
    b (ViewSpaceFlag)

    @info
    Get view space flag.
*/
//-------------------------------------------------------------------
static void n_getviewspace(void* slf, nCmd* cmd)
{
    n3DNode* self = (n3DNode*) slf;
    cmd->Out()->SetB(self->GetViewSpace());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gett

    @input
    v

    @output
    f (TranslateX), f (TranslateY), f (TranslateZ)

    @info
    Return current translation of the object.
*/
static void n_gett(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    float x,y,z;
    self->GetT(x,y,z);
    cmd->Out()->SetF(x);
    cmd->Out()->SetF(y);
    cmd->Out()->SetF(z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getr

    @input
    v

    @output
    f (RotateX), f (RotateY), f (RotateZ)

    @info
    Return current rotation of the object.
*/
static void n_getr(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    float x,y,z;
    self->GetR(x,y,z);
    cmd->Out()->SetF(x);
    cmd->Out()->SetF(y);
    cmd->Out()->SetF(z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gets

    @input
    v

    @output
    f (ScaleX), f (ScaleY), f (ScaleZ)

    @info
    Return current scaling of the object.
*/
static void n_gets(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    float x,y,z;
    self->GetS(x,y,z);
    cmd->Out()->SetF(x);
    cmd->Out()->SetF(y);
    cmd->Out()->SetF(z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getq

    @input
    v

    @output
    x(QuatX), y(QuatY), z(QuatZ), w(QuatW)

    @info
    Get the currently set quaternion.
*/
static void n_getq(void *o, nCmd *cmd)
{
    n3DNode *self = (n3DNode *) o;
    float x,y,z,w;
    self->GetQ(x,y,z,w);
    cmd->Out()->SetF(x);
    cmd->Out()->SetF(y);
    cmd->Out()->SetF(z);
    cmd->Out()->SetF(w);
}

//------------------------------------------------------------------------------
/**
    Emit commands to make this object persistent.

    @param  fs      file server which makes the object persistent
    @return         true if all ok
*/
bool n3DNode::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;
        bool b;
        float x,y,z,w;

        //--- setactive ---
        if (!(b = this->GetActive()))
        {
            cmd = fs->GetCmd(this, 'SACT');
            cmd->In()->SetB(this->GetActive());
            fs->PutCmd(cmd);
        }

        //--- setmaxlod ---
        if (this->GetMaxLod() > 0.0f)
        {
            cmd = fs->GetCmd(this, 'SMLD');
            cmd->In()->SetF(this->GetMaxLod());
            fs->PutCmd(cmd);
        }

        //--- txyz ---
        this->GetT(x,y,z);
        if ((x != 0.0f) || (y != 0.0f) || (z != 0.0f))
        {
            cmd = fs->GetCmd(this,'TXYZ');
            cmd->In()->SetF(x);
            cmd->In()->SetF(y);
            cmd->In()->SetF(z);
            fs->PutCmd(cmd);
        }

        //--- rxyz, qxyzw ---
        if (this->get(USEQUAT)) 
        {
            this->GetQ(x,y,z,w);
            if ((x != 0.0f) || (y != 0.0f) || (z != 0.0f) || (w != 1.0f))
            {
                cmd = fs->GetCmd(this,'QXYZ');
                cmd->In()->SetF(x);
                cmd->In()->SetF(y);
                cmd->In()->SetF(z);
                cmd->In()->SetF(w);
                fs->PutCmd(cmd);
            }
        } 
        else 
        {
            this->GetR(x,y,z);
            if ((x != 0.0f) || (y != 0.0f) || (z != 0.0f))
            {
                cmd = fs->GetCmd(this,'RXYZ');
                cmd->In()->SetF(x);
                cmd->In()->SetF(y);
                cmd->In()->SetF(z);
                fs->PutCmd(cmd);
            }
        }

        //--- sxyz ---
        this->GetS(x,y,z);
        if ((x != 1.0f) || (y != 1.0f) || (z != 1.0f))
        {
            cmd = fs->GetCmd(this,'SXYZ');
            cmd->In()->SetF(x);
            cmd->In()->SetF(y);
            cmd->In()->SetF(z);
            fs->PutCmd(cmd);
        }

        //--- setsprite ---
        if ((b = this->GetSprite())) 
        {
            cmd = fs->GetCmd(this,'SSPR');
            cmd->In()->SetB(b);
            fs->PutCmd(cmd);
        }

        //--- setbillboard ---
        if ((b = this->GetBillboard())) 
        {
            cmd = fs->GetCmd(this,'SBLB');
            cmd->In()->SetB(b);
            fs->PutCmd(cmd);
        }

        //--- setlockviewer ---
        if ((b = this->GetLockViewer())) 
        {
            cmd = fs->GetCmd(this,'SLVW');
            cmd->In()->SetB(b);
            fs->PutCmd(cmd);
        }
    
        //--- sethighdetail ---
        if (!(b = this->GetHighDetail()))
        {
            cmd = fs->GetCmd(this, 'SHDT');
            cmd->In()->SetB(b);
            fs->PutCmd(cmd);
        }

        //--- setviewspace ---
        if ((b = this->GetViewSpace()))
        {
            cmd = fs->GetCmd(this, 'SVWS');
            cmd->In()->SetB(b);
            fs->PutCmd(cmd);
        }

        retval = true;
    }
    return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
