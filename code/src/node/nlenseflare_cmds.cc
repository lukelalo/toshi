#define N_IMPLEMENTS nLenseFlare

//------------------------------------------------------------------------------
//  node/nlenseflare_cmds.cc
//  author: Jeremy Bishop
//  (C) 2000 Radon Labs GmbH
//------------------------------------------------------------------------------

#include "node/nlenseflare.h"
#include "kernel/npersistserver.h"

static void n_beginflares(void *, nCmd *);
static void n_setbasecolor(void *, nCmd *);
static void n_setblindcolor(void *, nCmd *);
static void n_setflaresizeat(void *, nCmd *);
static void n_setflareposat(void *, nCmd *);
static void n_setflarecolorat(void *, nCmd *);
static void n_endflares(void *, nCmd *);

static void n_getnumberofflares(void *, nCmd *);
static void n_getbasecolor(void *, nCmd *);
static void n_getblindcolor(void *, nCmd *);
static void n_getflaresizeat(void *, nCmd *);
static void n_getflareposat(void *, nCmd *);
static void n_getflarecolorat(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nlenseflare

    @superclass
    nvisnode

    @classinfo
    provides nebula with a lenseflare effect that can be positioned under an
    n3dnode with adjacent nshadernode and ntexarraynodes; setbasecolor enables
    the lense flare to be used with a mixer, useing the base color to alter
    the individual flare colors via a multiplication (see
    nLenseFlare::ColorFlares(...))
*/
void
n_initcmds(nClass *cl)
{
    cl->BeginCmds();

    cl->AddCmd("v_beginflares_i",         'BFLR', n_beginflares);
    cl->AddCmd("v_setbasecolor_ffff",     'SBCL', n_setbasecolor);
    cl->AddCmd("v_setblindcolor_ffff",    'SBLC', n_setblindcolor);
    cl->AddCmd("v_setflaresizeat_if",     'SFS_', n_setflaresizeat);  // no SFSA
    cl->AddCmd("v_setflareposat_if",      'SFPA', n_setflareposat);
    cl->AddCmd("v_setflarecolorat_iffff", 'SFCA', n_setflarecolorat);
    cl->AddCmd("v_endflares_v",           'EFLR', n_endflares);

    cl->AddCmd("i_getnumberofflares_v",   'GNOF', n_getnumberofflares);
    cl->AddCmd("ffff_getbasecolor_v",     'GBCL', n_getbasecolor);
    cl->AddCmd("ffff_getblindcolor_v",    'GBLC', n_getblindcolor);
    cl->AddCmd("f_getflaresizeat_i",      'GFS_', n_getflaresizeat);  // no GFSA
    cl->AddCmd("f_getflareposat_i",       'GFPA', n_getflareposat);
    cl->AddCmd("ffff_getflarecolorat_i",  'GFCA', n_getflarecolorat);

    cl->EndCmds();
}
//------------------------------------------------------------------------------
/**
    @cmd
    beginflares

    @input
    i (numberOfFlares)

    @output
    v

    @info
    allocates the proper amount of memory to accomodate the desired
    number of flares and enables the initialization routines to be
    called
*/
static
void
n_beginflares(void *o, nCmd *cmd)
{
    nLenseFlare *self = static_cast<nLenseFlare*>(o);
    self->BeginFlares(cmd->In()->GetI());
}
//------------------------------------------------------------------------------
/**
    @cmd
    setbasecolor

    @input
    f (baseRed), f (baseGrn), f (baseBlu), f (baseAlpha)

    @output
    v

    @info
    sets the base color (used to multiply against the individual flare colors to
    allow a single function to alter multiple flare colors)
*/
static
void
n_setbasecolor(void *o, nCmd *cmd)
{
    float fTmp1 = cmd->In()->GetF();    // 1.
    float fTmp2 = cmd->In()->GetF();    // 1.
    float fTmp3 = cmd->In()->GetF();    // 1.
    float fTmp4 = cmd->In()->GetF();    // 1.

    nLenseFlare *self = static_cast<nLenseFlare*>(o);
    self->SetBaseColor(fTmp1, fTmp2, fTmp3, fTmp4);
}
//------------------------------------------------------------------------------
/**
    @cmd
    setblindcolor

    @input
    f (red), f (grn), f (blu), f (alpha)

    @output
    v

    @info
    sets the color used when blind effect active (looking directly into light
    source); the alpha value is a modifier multiplied by the calculated alpha
    for the overlay plane (i.e., if you have a "moon" flare, you may want to
    set the alpha value of this color to something like 0.2f to reduce the
    effect of the blind
*/
static
void
n_setblindcolor(void *o, nCmd *cmd)
{
    float fTmp1 = 0.0f, fTmp2 = 0.0f, fTmp3 = 0.0f, fTmp4 = 0.0f;

    fTmp1 = cmd->In()->GetF();  //  1.
    fTmp2 = cmd->In()->GetF();  //  1.
    fTmp3 = cmd->In()->GetF();  //  1.
    fTmp4 = cmd->In()->GetF();  //  1.

    vector4 vBClr(fTmp1, fTmp2, fTmp3, fTmp4);

    nLenseFlare *self = static_cast<nLenseFlare*>(o);
    self->SetBlindColor(vBClr);
}
//------------------------------------------------------------------------------
/**
    @cmd
    setflaresizeat

    @input
    i (index), f (size)

    @output
    v

    @info
    sets the length of a side of the quad representing the flare at this index
*/
static
void
n_setflaresizeat(void *o, nCmd *cmd)
{
    int iTmp = cmd->In()->GetI();   // 1.
    float fTmp = cmd->In()->GetF(); // 1.

    nLenseFlare *self = static_cast<nLenseFlare*>(o);
    self->SetFlareSizeAt(iTmp, fTmp);
}
//------------------------------------------------------------------------------
/**
    @cmd
    setflareposat

    @input
    i (index), f (size)

    @output
    v

    @info
    sets the position along the "flare line"; the flare line is the imaginary
    line that passes through the lightsource and the center of the screen; a
    value of 0.0f for the position places the flare over the light source, and
    a value of 1.0f places the flare over the center of the screen;  values less
    than 0.0f will be placed behind the light, and values of greater than 1.0f
    will be placed beyond the center of the screen (e.g. a value of 2.0f would
    place the flare directly opposite the light source, reflected about the line
    perpendicular to the flare line that passes through the center of the 
    screen)
*/
static
void
n_setflareposat(void *o, nCmd *cmd)
{
    int iTmp = cmd->In()->GetI();   // 1.
    float fTmp = cmd->In()->GetF(); // 1.

    nLenseFlare *self = static_cast<nLenseFlare*>(o);
    self->SetFlarePosAt(iTmp, fTmp);
}
//------------------------------------------------------------------------------
/**
    @cmd
    setflarecolorat

    @input
    i (index), f (red), f (grn), f (blu), f (alpha)

    @output
    v

    @info
    sets the RGBA values of the quad's vertices representing the flare at index
*/
static
void
n_setflarecolorat(void *o, nCmd *cmd)
{
    int iTmp = cmd->In()->GetI();       // 1.
    float fTmp1 = cmd->In()->GetF();    // 1.
    float fTmp2 = cmd->In()->GetF();    // 1.
    float fTmp3 = cmd->In()->GetF();    // 1.
    float fTmp4 = cmd->In()->GetF();    // 1.

    nLenseFlare *self = static_cast<nLenseFlare*>(o);
    self->SetFlareColorAt(iTmp, fTmp1, fTmp2, fTmp3, fTmp4);
}
//------------------------------------------------------------------------------
/**
    @cmd
    endflares

    @input
    v

    @output
    v

    @info
    signifies the end of the initialization phase, enabling the system to begin
    to process the lenseflare (i.e Attach(...) and Compute(...) can now be
    called)
*/
static
void
n_endflares(void *o, nCmd *)
{
    nLenseFlare *self = static_cast<nLenseFlare*>(o);
    self->EndFlares();
}
//------------------------------------------------------------------------------
/**
    @cmd
    getbasecolor

    @input
    v

    @output
    f (baseRed), f (baseGrn), f (baseBlu), f (baseAlpha)

    @info
    returns the base color's RGBA values as out parameters
*/
static
void
n_getbasecolor(void *o, nCmd *cmd)
{
    float fTmp1 = 0.0f, fTmp2 = 0.0f, fTmp3 = 0.0f, fTmp4 = 0.0f;

    nLenseFlare *self = static_cast<nLenseFlare*>(o);
    self->GetBaseColor(fTmp1, fTmp2, fTmp3, fTmp4);

    cmd->Out()->SetF(fTmp1);
    cmd->Out()->SetF(fTmp2);
    cmd->Out()->SetF(fTmp3);
    cmd->Out()->SetF(fTmp4);
}
//------------------------------------------------------------------------------
/**
    @cmd
    getblindcolor

    @input
    v

    @output
    f (red), f (grn), f (blu), f (alpha)

    @info
    returns the blind effect's reference color
*/
static
void
n_getblindcolor(void *o, nCmd *cmd)
{
    vector4 vTmp(0.0f, 0.0f, 0.0f, 0.0f);

    nLenseFlare *self = static_cast<nLenseFlare*>(o);
    vTmp = self->GetBlindColor();

    cmd->Out()->SetF(vTmp.x);
    cmd->Out()->SetF(vTmp.y);
    cmd->Out()->SetF(vTmp.z);
    cmd->Out()->SetF(vTmp.w);;
}
//------------------------------------------------------------------------------
/**
    @cmd
    getnumberofflares

    @input
    v

    @output
    i (numberOfFlares)

    @info
    returns the number of flares in the current lenseflare
*/
static
void
n_getnumberofflares(void *o, nCmd *cmd)
{
    nLenseFlare *self = static_cast<nLenseFlare*>(o);
    cmd->Out()->SetI(self->GetNumberOfFlares());
}
//------------------------------------------------------------------------------
/**
    @cmd
    getflaresizeat

    @input
    i (index)

    @output
    f (size)

    @info
    returns the size of the flare at index
*/
static
void
n_getflaresizeat(void *o, nCmd *cmd)
{
    int iTmp = cmd->In()->GetI();
    nLenseFlare *self = static_cast<nLenseFlare*>(o);
    cmd->Out()->SetF(self->GetFlareSizeAt(iTmp));
}
//------------------------------------------------------------------------------
/**
    @cmd
    getflareposat

    @input
    i (index)

    @output
    f (position)

    @info
    returns the position of the flare at index (see setflareposat for a
    description of how to interpret the value of position)
*/
static
void
n_getflareposat(void *o, nCmd *cmd)
{
    int iTmp = cmd->In()->GetI();
    nLenseFlare *self = static_cast<nLenseFlare*>(o);
    cmd->Out()->SetF(self->GetFlarePosAt(iTmp));
}
//------------------------------------------------------------------------------
/**
    @cmd
    getflarecolorat

    @input
    i (index)

    @output
    f (red), f (grn), f (blu), f (alpha)

    @info
    returns the RGBA values of the flare at index as out parameters
*/
void
n_getflarecolorat(void *o, nCmd *cmd)
{
    float fTmp1 = 0.0f, fTmp2 = 0.0f, fTmp3 = 0.0f, fTmp4 = 0.0f;

    int iTmp = cmd->In()->GetI();
    nLenseFlare *self = static_cast<nLenseFlare*>(o);
    self->GetFlareColorAt(iTmp, fTmp1, fTmp2, fTmp3, fTmp4);

    cmd->Out()->SetF(fTmp1);
    cmd->Out()->SetF(fTmp2);
    cmd->Out()->SetF(fTmp3);
    cmd->Out()->SetF(fTmp4);
}
//------------------------------------------------------------------------------
//  2000.04.30  floh    fixed warnings under Linux/gcc
//------------------------------------------------------------------------------
bool
nLenseFlare::SaveCmds(nPersistServer *fs)
{
    if (nVisNode::SaveCmds(fs))
    {
        nCmd *cmd = 0;

        if ((cmd = fs->GetCmd(this, 'BFLR')))
        {
            cmd->In()->SetI(iNumberOfFlares);
            fs->PutCmd(cmd);
        }

        if ((cmd = fs->GetCmd(this, 'SBLC')))
        {
            vector4 vTmp = GetBlindColor();
            
            cmd->In()->SetF(vTmp.x);
            cmd->In()->SetF(vTmp.y);
            cmd->In()->SetF(vTmp.z);
            cmd->In()->SetF(vTmp.w);

            fs->PutCmd(cmd);
        }

        int i;
        for (i = 0; i < iNumberOfFlares; i++)
        {
            if ((cmd = fs->GetCmd(this, 'SFS_')))
            {
                cmd->In()->SetI(i);
                cmd->In()->SetF(aFlares[i].fSize);
                fs->PutCmd(cmd);
            }
            if ((cmd = fs->GetCmd(this, 'SFPA')))
            {
                cmd->In()->SetI(i);
                cmd->In()->SetF(aFlares[i].fPosition);
                fs->PutCmd(cmd);
            }
            if ((cmd = fs->GetCmd(this, 'SFCA')))
            {
                cmd->In()->SetI(i);
                cmd->In()->SetF(aFlares[i].fRed);
                cmd->In()->SetF(aFlares[i].fGreen);
                cmd->In()->SetF(aFlares[i].fBlue);
                fs->PutCmd(cmd);
            }
        }
        if ((cmd = fs->GetCmd(this, 'EFLR')))
            fs->PutCmd(cmd);

        return true;
    }
    else
        return false;
}
//------------------------------------------------------------------------------
