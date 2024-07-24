#define N_IMPLEMENTS nLightNode
//-------------------------------------------------------------------
//  node/nlight_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "node/nlightnode.h"
#include "kernel/npersistserver.h"

static void n_settype(void *, nCmd *);
static void n_setcolor(void *, nCmd *);
static void n_setr(void *, nCmd *);
static void n_setg(void *, nCmd *);
static void n_setb(void *, nCmd *);
static void n_seta(void *, nCmd *);
static void n_setattenuation(void *, nCmd *);
static void n_setspot(void *, nCmd *);
static void n_gettype(void *, nCmd *);
static void n_getcolor(void *, nCmd *);
static void n_getattenuation(void *, nCmd *);
static void n_getspot(void *, nCmd *);
static void n_setcastshadows(void*, nCmd*);
static void n_getcastshadows(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nlightnode

    @superclass
    nvisnode

    @classinfo
    Defines "actual lightsource attributes" for the parent n3dnode.
    Position and orientation will be provided by the n3dnode if it 
    is required by the lightsource.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_settype_s",           'STYP', n_settype);
    cl->AddCmd("v_setcolor_ffff",       'SCLR', n_setcolor);
    cl->AddCmd("v_setr_f",              'SETR', n_setr);
    cl->AddCmd("v_setg_f",              'SETG', n_setg);
    cl->AddCmd("v_setb_f",              'SETB', n_setb);
    cl->AddCmd("v_seta_f",              'SETA', n_seta);
    cl->AddCmd("v_setattenuation_fff",  'SATT', n_setattenuation);
    cl->AddCmd("v_setspot_ff",          'SSPT', n_setspot);
    cl->AddCmd("s_gettype_v",           'GTYP', n_gettype);
    cl->AddCmd("ffff_getcolor_v",       'GCLR', n_getcolor);
    cl->AddCmd("fff_getattenuation_v",  'GATT', n_getattenuation);
    cl->AddCmd("ff_getspot_v",          'GSPT', n_getspot);
    cl->AddCmd("v_setcastshadows_b",    'SCSH', n_setcastshadows);
    cl->AddCmd("b_getcastshadows_v",    'GCSH', n_getcastshadows);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    settype

    @input
    s (Type = [ambient point spot directional parallelpoint])

    @output
    v

    @info
    Sets the type of the lightsource:
    ambient -- ambient-lightsource. There should exist only one 
               per scene, since the ambient lightsource can't be 
               combined (they overwrite each other)
               To an ambient lightsource only "setcolor" is a 
               usefull operation.
    point   -- point-lightsource. Evaluates the actual position.
               "setcolor" sets color and intensity, "setattenuation"
               the distance related fade of light.
    spot    -- spot-lightsource. Evaluates the actual position 
               and orientation (the ray of light goes along the 
               negative z-axis of the parent n3dnode).
               setcolor, setattenuation and
               setspot work like expected.
    directional -- directional-lightsource. The orientation of the 
               parent n3dnode defines the direction (along the 
               negative z-axis) of the light.setcolor sets
               color and intensity. Position of the n3dnode,
               setattenuation, setspot have no meaning in this context.
*/
static void n_settype(void *o, nCmd *cmd)
{
    nLightNode *self = (nLightNode *) o;
    const char *ts = cmd->In()->GetS();
    if      (strcmp(ts,"ambient")==0)       self->SetType(N_LIGHT_AMBIENT);
    else if (strcmp(ts,"point")==0)         self->SetType(N_LIGHT_POINT);
    else if (strcmp(ts,"spot")==0)          self->SetType(N_LIGHT_SPOT);
    else if (strcmp(ts,"directional")==0)   self->SetType(N_LIGHT_DIRECTIONAL);
    else {
        n_printf("invalid light type, must be [ambient point spot directional parallelpoint]\n");
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcolor

    @input
    f (Red), f (Green), f (Blue), f (Alpha)

    @output
    v

    @info
    Defines color and intensity of the lightsource. Normal values range
    between -1.0 and +1.0, but you can also use higher/lower values.
    It is not possible to set the ambient/diffuse/specular components one
    by one. OpenGL allows this but not Direct3D. Therefor all non-ambient
    lightsources have an ambient value of 0,0,0,0 and diffuse/specular
    values as set by "setcolor". Notice that on material properties you 
    can set every component (ambient/diffuse/specular) one by one in the
    reflexion properties.
*/
static void n_setcolor(void *o, nCmd *cmd)
{
    nLightNode *self = (nLightNode *) o;
    float r = cmd->In()->GetF();
    float g = cmd->In()->GetF();
    float b = cmd->In()->GetF();
    float a = cmd->In()->GetF();
    self->SetColor(r,g,b,a);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setr
    setg
    setb
    seta

    @input
    f (Red,Green,Blue,Alpha)

    @output
    v

    @info
    Set the color components independently.
*/
static void n_setr(void *o, nCmd *cmd)
{
    nLightNode *self = (nLightNode *) o;
    self->SetR(cmd->In()->GetF());
}

static void n_setg(void *o, nCmd *cmd)
{
    nLightNode *self = (nLightNode *) o;
    self->SetG(cmd->In()->GetF());
}

static void n_setb(void *o, nCmd *cmd)
{
    nLightNode *self = (nLightNode *) o;
    self->SetB(cmd->In()->GetF());
}

static void n_seta(void *o, nCmd *cmd)
{
    nLightNode *self = (nLightNode *) o;
    self->SetA(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setattenuation

    @input
    v

    @output
    f (Constant), f (Linear), f (Quadratic)

    @info
    Sets the 3 distance dependant lightsource fade parameters.
    Somewhere in the OpenGl specs you can find a formula for sure...
    until that:
    1, 0, 0     -> lightsource stays constant to infinity
    0, 0.2, 0   -> good for tests, smaller values for Linear
                   result in a weak fade of light
                   ergeben schwaechere Abnahme
    Is only evaluated for lights of type "point" and "spot"
*/
static void n_setattenuation(void *o, nCmd *cmd)
{
    nLightNode *self = (nLightNode *) o;
    float c = cmd->In()->GetF();
    float l = cmd->In()->GetF();
    float q = cmd->In()->GetF();
    self->SetAttenuation(c,l,q);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setspot

    @input
    f (Cutoff), f (Exponent)

    @output
    v

    @info
    Sets the spot parameters opening-angle (cutoff) and angle dependent
    light intensity fade (exponent). Cutoff makes only sense for angles
    smaller than 90 degrees. The exponent is between 0 and 128, the higher
    the values, the more focussed is the spot. On exponent 0 the light
    intensity is spread evenly along the whole opening angle of the spot.
*/
static void n_setspot(void *o, nCmd *cmd)
{
    nLightNode *self = (nLightNode *) o;
    float c = cmd->In()->GetF();
    float e = cmd->In()->GetF();
    self->SetSpot(c,e);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettype

    @input
    v

    @output
    s (Type = [ambient point spot directional parallelpoint])

    @info
    Returns the type of the lightsource.
*/
static void n_gettype(void *o, nCmd *cmd)
{
    nLightNode *self = (nLightNode *) o;
    nLightType lt = self->GetType();
    switch(lt) {
        case N_LIGHT_AMBIENT:       cmd->Out()->SetS("ambient"); break;
        case N_LIGHT_POINT:         cmd->Out()->SetS("point"); break;
        case N_LIGHT_SPOT:          cmd->Out()->SetS("spot"); break;
        case N_LIGHT_DIRECTIONAL:   cmd->Out()->SetS("directional"); break;
        default:                    cmd->Out()->SetS("<error>"); break;
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcolor

    @input
    v

    @output
    f (Red), f (Green), f (Blue), f (Alpha)

    @info
    Returns the color of the lightsource.
*/
static void n_getcolor(void *o, nCmd *cmd)
{
    nLightNode *self = (nLightNode *) o;
    float r,g,b,a;
    self->GetColor(r,g,b,a);
    cmd->Out()->SetF(r);
    cmd->Out()->SetF(g);
    cmd->Out()->SetF(b);
    cmd->Out()->SetF(a);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getattenuation

    @input
    v

    @output
    f (Constant), f (Linear), f (Quadratic)

    @info
    Returns the light intensity fade parameter.
*/
static void n_getattenuation(void *o, nCmd *cmd)
{
    nLightNode *self = (nLightNode *) o;
    float c,l,q;
    self->GetAttenuation(c,l,q);
    cmd->Out()->SetF(c);
    cmd->Out()->SetF(l);
    cmd->Out()->SetF(q);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getspot

    @input
    v

    @output
    f (Cutoff), f (Exponent)

    @info
    Returns the opening angle parameter.
*/
static void n_getspot(void *o, nCmd *cmd)
{
    nLightNode *self = (nLightNode *) o;
    float c,e;
    self->GetSpot(c,e);
    cmd->Out()->SetF(c);
    cmd->Out()->SetF(e);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcastshadows

    @input
    b (CastShadows)

    @output
    v

    @info
    Set to true if this light should be considered as a light
    source in the shadow casting system.
*/
static void n_setcastshadows(void* o, nCmd* cmd)
{
    nLightNode* self = (nLightNode*) o;
    self->SetCastShadows(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcastshadows

    @input
    v

    @output
    b (CastShadows)

    @info
    Return shadow casting state of this light.
*/
static void n_getcastshadows(void* o, nCmd* cmd)
{
    nLightNode* self = (nLightNode*) o;
    cmd->Out()->SetB(self->GetCastShadows());
}

//-------------------------------------------------------------------
//  SaveCmds()
//  02-Jan-99   floh    machine generated
//  06-Aug-01   floh    shadow casting state
//-------------------------------------------------------------------
bool nLightNode::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;
        float r,g,b,a,c,l,q,e;

        //--- settype ---
        cmd = fs->GetCmd(this, 'STYP');
        nLightType lt = this->GetType();
        switch(lt) {
            case N_LIGHT_AMBIENT:       cmd->In()->SetS("ambient"); break;
            case N_LIGHT_POINT:         cmd->In()->SetS("point"); break;
            case N_LIGHT_SPOT:          cmd->In()->SetS("spot"); break;
            case N_LIGHT_DIRECTIONAL:   cmd->In()->SetS("directional"); break;
            default:                    cmd->In()->SetS("<error>"); break;
        }
        fs->PutCmd(cmd);
        
        //--- setcolor ---
        cmd = fs->GetCmd(this, 'SCLR');
        this->GetColor(r,g,b,a);
        cmd->In()->SetF(r);
        cmd->In()->SetF(g);
        cmd->In()->SetF(b);
        cmd->In()->SetF(a);
        fs->PutCmd(cmd);
        
        //--- setattenuation ---
        cmd = fs->GetCmd(this, 'SATT');
        this->GetAttenuation(c,l,q);
        cmd->In()->SetF(c);
        cmd->In()->SetF(l);
        cmd->In()->SetF(q);
        fs->PutCmd(cmd);
        
        //--- setspot ---
        cmd = fs->GetCmd(this, 'SSPT');
        this->GetSpot(c,e);
        cmd->In()->SetF(c);
        cmd->In()->SetF(e);
        fs->PutCmd(cmd);

        //--- setcastshadows ---
        cmd = fs->GetCmd(this, 'SCSH');
        cmd->In()->SetB(this->GetCastShadows());
        fs->PutCmd(cmd);
        
        retval = true;
    }
    return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
