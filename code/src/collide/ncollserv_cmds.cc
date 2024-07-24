#define N_IMPLEMENTS nCollideServer
//-------------------------------------------------------------------
//  ncollserv_cmds.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "collide/ncollideserver.h"

static void n_begincollclasses(void *, nCmd *);
static void n_addcollclass(void *, nCmd *);
static void n_endcollclasses(void *, nCmd *);
static void n_begincolltypes(void *, nCmd *);
static void n_addcolltype(void *, nCmd *);
static void n_endcolltypes(void *, nCmd *);
static void n_querycolltype(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ncollideserver

    @superclass
    nroot

    @classinfo
    Superclass for collision servers. A collision server is the
    central object for the collision system and has to be
    subclassed to link to a specific collision system.
*/
void n_initcmds(nClass *cl) {
    cl->BeginCmds();
    cl->AddCmd("v_begincollclasses_v", 'BGCC', n_begincollclasses);
    cl->AddCmd("v_addcollclass_s",     'ADCC', n_addcollclass);
    cl->AddCmd("v_endcollclasses_v",   'ENCC', n_endcollclasses);
    cl->AddCmd("v_begincolltypes_v",   'BGCT', n_begincolltypes);
    cl->AddCmd("v_addcolltype_sss",    'ADCT', n_addcolltype);
    cl->AddCmd("v_endcolltypes_v",     'ENCT', n_endcolltypes);
    cl->AddCmd("s_querycolltype_ss",   'QRCT', n_querycolltype);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    begincollclasses

    @input
    v

    @output
    v

    @info
    Start defining collision classes. A collision class is a
    user defined abstract type name which is later used to
    define inter-class collision types (ignore, quick, or exact).
*/
static void n_begincollclasses(void *o, nCmd *)
{
    nCollideServer *self = (nCollideServer *) o;
    self->BeginCollClasses();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addcollclass

    @input
    s (CollClassName)

    @output
    v

    @info
    Add a new collision class name. Collision class names must
    be unique.
*/
static void n_addcollclass(void *o, nCmd *cmd)
{
    nCollideServer *self = (nCollideServer *) o;
    self->AddCollClass(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    endcollclasses

    @input
    v

    @output
    s (CollClassName)

    @info
    End adding collision class names. After collision class names
    are defined, the collision type table should be built
    by doing a 'begincolltypes/addcolltype/endcolltypes' sequence.
*/
static void n_endcollclasses(void *o, nCmd *)
{
    nCollideServer *self = (nCollideServer *) o;
    self->EndCollClasses();
}

//------------------------------------------------------------------------------
/**
    @cmd
    begincolltypes

    @input
    v

    @output
    v

    @info
    Begin definition of collision types. A collision type defines
    how 2 collision classes should check collision (ignore, quick
    or exact).
*/
static void n_begincolltypes(void *o, nCmd *)
{
    nCollideServer *self = (nCollideServer *) o;
    self->BeginCollTypes();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addcolltype

    @input
    s (CollClass1), s (CollClass2), s (CollType=ignore|quick|contact|exact)

    @output
    v

    @info
    Define a collision type, this is how 2 objects of a given
    collision class should perform their collision test:
    ignore  -> the 2 objects will ignore any collision
    quick   -> the 2 objects will do a quick spherical check
    contact -> the 2 objects report the first contact
    exact   -> the 2 objects should do a triangle exact check

    The default is 'exact'.
*/
static void n_addcolltype(void *o, nCmd *cmd)
{
    nCollideServer *self = (nCollideServer *) o;
    const char *s0 = cmd->In()->GetS();
    const char *s1 = cmd->In()->GetS();
    const char *s2 = cmd->In()->GetS();
    nCollType ct = COLLTYPE_EXACT;
    if (strcmp(s2, "ignore")       == 0) ct = COLLTYPE_IGNORE;
    else if (strcmp(s2, "quick")   == 0) ct = COLLTYPE_QUICK;
    else if (strcmp(s2, "exact")   == 0) ct = COLLTYPE_EXACT;
    else if (strcmp(s2, "contact") == 0) ct = COLLTYPE_CONTACT;
    else {
        n_printf("Unknown collision type '%s', valid is 'ignore|quick|contact|exact'!\n",s2);
    }
    self->AddCollType(s0,s1,ct);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endcolltypes

    @input
    v

    @output
    v

    @info
    End defining collision types.
*/
static void n_endcolltypes(void *o, nCmd *)
{
    nCollideServer *self = (nCollideServer *) o;
    self->EndCollTypes();
}

//------------------------------------------------------------------------------
/**
    @cmd
    querycolltype

    @input
    s (CollClass1), s (CollClass2)

    @output
    s (CollType)

    @info
    Query the collision type table for the collision type
    that has been defined for the 2 given collision classes.
*/
static void n_querycolltype(void *o, nCmd *cmd)
{
    nCollideServer *self = (nCollideServer *) o;
    const char *s1 = cmd->In()->GetS();
    const char *s2 = cmd->In()->GetS();
    nCollType ct = self->QueryCollType(s1,s2);
    switch (ct) {
        case COLLTYPE_IGNORE:   cmd->Out()->SetS("ignore"); break;
        case COLLTYPE_QUICK:    cmd->Out()->SetS("quick"); break;
        case COLLTYPE_CONTACT:  cmd->Out()->SetS("contact"); break;
        case COLLTYPE_EXACT:    cmd->Out()->SetS("exact"); break;
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------



