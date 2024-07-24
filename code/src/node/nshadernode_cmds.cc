#define N_IMPLEMENTS nShaderNode
//-------------------------------------------------------------------
//  nshadernode_cmds.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "node/nshadernode.h"

static void n_setrenderpri(void *o, nCmd *);
static void n_getrenderpri(void *o, nCmd *);

static void n_setnumstages(void *o, nCmd *);
static void n_getnumstages(void *o, nCmd *);

static void n_setcolorop(void *, nCmd *);
static void n_getcolorop(void *, nCmd *);
static void n_setalphaop(void *, nCmd *);
static void n_getalphaop(void *, nCmd *);

static void n_begintunit(void *, nCmd *);
static void n_endtunit(void *, nCmd *);

static void n_setaddress(void *, nCmd *);
static void n_getaddress(void *, nCmd *);
static void n_setminmagfilter(void *, nCmd *);
static void n_getminmagfilter(void *, nCmd *);
static void n_settexcoordsrc(void *, nCmd *);
static void n_gettexcoordsrc(void *, nCmd *);
static void n_setmiplodbias(void*, nCmd* cmd);
static void n_getmiplodbias(void*, nCmd* cmd);

static void n_setconst(void *, nCmd *);
static void n_getconst(void *, nCmd *);
static void n_setenabletransform(void *, nCmd *);
static void n_getenabletransform(void *, nCmd *);
static void n_txyz(void *, nCmd *);
static void n_gett(void *, nCmd *);
static void n_rxyz(void *, nCmd *);
static void n_getr(void *, nCmd *);
static void n_sxyz(void *, nCmd *);
static void n_gets(void *, nCmd *);

static void n_setdiffuse(void *, nCmd *);
static void n_getdiffuse(void *, nCmd *);
static void n_setemissive(void *, nCmd *);
static void n_getemissive(void *, nCmd *);
static void n_setambient(void *, nCmd *);
static void n_getambient(void *, nCmd *);
static void n_setlightenable(void *, nCmd *);
static void n_getlightenable(void *, nCmd *);
static void n_setalphaenable(void *, nCmd *);
static void n_getalphaenable(void *, nCmd *);
static void n_setzwriteenable(void *, nCmd *);
static void n_getzwriteenable(void *, nCmd *);
static void n_setfogenable(void *, nCmd *);
static void n_getfogenable(void *, nCmd *);
static void n_setalphablend(void *, nCmd *);
static void n_getalphablend(void *, nCmd *);
static void n_setzfunc(void *, nCmd *);
static void n_getzfunc(void *, nCmd *);
static void n_setcullmode(void *, nCmd *);
static void n_getcullmode(void *, nCmd *);
static void n_setcolormaterial(void *, nCmd *);
static void n_getcolormaterial(void *, nCmd *);
static void n_setalphatestenable(void*, nCmd*);
static void n_getalphatestenable(void*, nCmd*);
static void n_setalpharef(void*, nCmd*);
static void n_getalpharef(void*, nCmd*);
static void n_setalphafunc(void*, nCmd*);
static void n_getalphafunc(void*, nCmd*);
static void n_setwireframe(void*, nCmd*);
static void n_getwireframe(void*, nCmd*);
static void n_setnormalizenormals(void*, nCmd*);
static void n_getnormalizenormals(void*, nCmd*);

static void n_setconst0(void *, nCmd *);
static void n_setconst1(void *, nCmd *);
static void n_setconst2(void *, nCmd *);
static void n_setconst3(void *, nCmd *);

static void n_txyz0(void *, nCmd *);
static void n_txyz1(void *, nCmd *);
static void n_txyz2(void *, nCmd *);
static void n_txyz3(void *, nCmd *);

static void n_rxyz0(void *, nCmd *);
static void n_rxyz1(void *, nCmd *);
static void n_rxyz2(void *, nCmd *);
static void n_rxyz3(void *, nCmd *);

static void n_sxyz0(void *, nCmd *);
static void n_sxyz1(void *, nCmd *);
static void n_sxyz2(void *, nCmd *);
static void n_sxyz3(void *, nCmd *);

static void n_setemissiver(void *, nCmd *);
static void n_setemissiveg(void *, nCmd *);
static void n_setemissiveb(void *, nCmd *);
static void n_setemissivea(void *, nCmd *);

struct str2param {
    const char *str;
    nRStateParam rp;
};

// render state parameter translation table
static struct str2param str2param_table[] =
{
    { "nearest",                N_TFILTER_NEAREST },
    { "linear",                 N_TFILTER_LINEAR },
    { "nearest_mipmap_nearest", N_TFILTER_NEAREST_MIPMAP_NEAREST },
    { "linear_mipmap_nearest",  N_TFILTER_LINEAR_MIPMAP_NEAREST },
    { "nearest_mipmap_linear",  N_TFILTER_NEAREST_MIPMAP_LINEAR },
    { "linear_mipmap_linear",   N_TFILTER_LINEAR_MIPMAP_LINEAR },
    { "zero",                   N_ABLEND_ZERO },
    { "one",                    N_ABLEND_ONE },
    { "srccolor",               N_ABLEND_SRCCOLOR },
    { "invsrccolor",            N_ABLEND_INVSRCCOLOR },
    { "srcalpha",               N_ABLEND_SRCALPHA },
    { "invsrcalpha",            N_ABLEND_INVSRCALPHA },
    { "destalpha",              N_ABLEND_DESTALPHA },
    { "invdestalpha",           N_ABLEND_INVDESTALPHA },
    { "destcolor",              N_ABLEND_DESTCOLOR },
    { "invdestcolor",           N_ABLEND_INVDESTCOLOR },
    { "wrap",                   N_TADDR_WRAP },
    { "clamp",                  N_TADDR_CLAMP },
    { "never",                  N_CMP_NEVER },
    { "less",                   N_CMP_LESS },
    { "equal",                  N_CMP_EQUAL },
    { "lessequal",              N_CMP_LESSEQUAL },
    { "greater",                N_CMP_GREATER },
    { "notequal",               N_CMP_NOTEQUAL },
    { "greaterequal",           N_CMP_GREATEREQUAL },
    { "always",                 N_CMP_ALWAYS },
    { "none",                   N_CULL_NONE },
    { "cw",                     N_CULL_CW },
    { "ccw",                    N_CULL_CCW },
    { "material",               N_CMAT_MATERIAL },
    { "diffuse",                N_CMAT_DIFFUSE },
    { "emissive",               N_CMAT_EMISSIVE },
    { "ambient",                N_CMAT_AMBIENT },
    { "uv0",                    N_TCOORDSRC_UV0 },
    { "uv1",                    N_TCOORDSRC_UV1 },
    { "uv2",                    N_TCOORDSRC_UV2 },
    { "uv3",                    N_TCOORDSRC_UV3 },
    { "objectspace",            N_TCOORDSRC_OBJECTSPACE },
    { "eyespace",               N_TCOORDSRC_EYESPACE },
    { "spheremap",              N_TCOORDSRC_SPHEREMAP },
    { NULL,                     N_FALSE },
};

//--------------------------------------------------------------------
//  _str2param()
//  22-Aug-00   floh    created
//--------------------------------------------------------------------
static nRStateParam str2param(const char *str)
{
    int i=0;
    struct str2param *p = NULL;
    while (p=&(str2param_table[i++]), p->str) {
        if (strcmp(p->str,str)==0) return p->rp;

    }
    n_printf("nShaderNode: string '%s' not accepted as render state!\n", str);
    // n_error("Aborting.");
    return N_FALSE;
}

//--------------------------------------------------------------------
//  param2str()
//  22-Aug-00   floh    created
//--------------------------------------------------------------------
static const char *param2str(nRStateParam rp)
{
    int i=0;
    struct str2param *p = NULL;
    while (p=&(str2param_table[i++]), p->rp!=N_FALSE) {
        if (p->rp == rp) return p->str;

    }
    n_printf("nShaderNode: unknown render state!\n");
    // n_error("Aborting.");
    return NULL;
}

//------------------------------------------------------------------------------
/**
    @scriptclass
    nshadernode

    @superclass
    nvisnode

    @classinfo
    Describe surface characteristics, inluding multitexture
    configuration, lighting parameters, alpha blending, and
    several lower level render state settings.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();

    cl->AddCmd("v_setrenderpri_i",          'SRPR', n_setrenderpri);
    cl->AddCmd("i_getrenderpri_v",          'GRPR', n_getrenderpri);

    cl->AddCmd("v_setnumstages_i",          'SNST', n_setnumstages);
    cl->AddCmd("i_getnumstages_v",          'GNST', n_getnumstages);

    cl->AddCmd("b_setcolorop_is",           'SPOP', n_setcolorop);
    cl->AddCmd("s_getcolorop_i",            'GPOP', n_getcolorop);
    cl->AddCmd("b_setalphaop_is",           'SAOP', n_setalphaop);
    cl->AddCmd("s_getalphaop_i",            'GAOP', n_getalphaop);
    cl->AddCmd("v_begintunit_i",            'BGTU', n_begintunit);
    cl->AddCmd("v_endtunit_v",              'EDTU', n_endtunit);
    cl->AddCmd("v_setaddress_ss",           'SADR', n_setaddress);
    cl->AddCmd("ss_getaddress_i",           'GADR', n_getaddress);
    cl->AddCmd("v_setminmagfilter_ss",      'SMMF', n_setminmagfilter);
    cl->AddCmd("ss_getminmagfilter_i",      'GMMF', n_getminmagfilter);
    cl->AddCmd("v_settexcoordsrc_s",        'STCS', n_settexcoordsrc);
    cl->AddCmd("s_gettexcoordsrc_i",        'GTCS', n_gettexcoordsrc);
    cl->AddCmd("v_setmiplodbias_i",         'SMLB', n_setmiplodbias);
    cl->AddCmd("i_getmiplodbias_v",         'GMLB', n_getmiplodbias);
    cl->AddCmd("v_setconst_iffff",          'SCOT', n_setconst);
    cl->AddCmd("ffff_getconst_i",           'GCOT', n_getconst);
    cl->AddCmd("v_setenabletransform_b",    'SENT', n_setenabletransform);
    cl->AddCmd("b_getenabletransform_i",    'GENT', n_getenabletransform);
    cl->AddCmd("v_txyz_fff",                'TXYZ', n_txyz);
    cl->AddCmd("fff_gett_i",                'GETT', n_gett);
    cl->AddCmd("v_rxyz_fff",                'RXYZ', n_rxyz);
    cl->AddCmd("fff_getr_i",                'GETR', n_getr);
    cl->AddCmd("v_sxyz_fff",                'SXYZ', n_sxyz);
    cl->AddCmd("fff_gets_i",                'GETS', n_gets);
    cl->AddCmd("v_setdiffuse_ffff",         'SDIF', n_setdiffuse);
    cl->AddCmd("ffff_getdiffuse_v",         'GDIF', n_getdiffuse);
    cl->AddCmd("v_setemissive_ffff",        'SEMV', n_setemissive);
    cl->AddCmd("ffff_getemissive_v",        'GEMV', n_getemissive);
    cl->AddCmd("v_setambient_ffff",         'SAMB', n_setambient);
    cl->AddCmd("ffff_getambient_v",         'GAMB', n_getambient);
    cl->AddCmd("v_setlightenable_b",        'SLEN', n_setlightenable);
    cl->AddCmd("b_getlightenable_v",        'GLEN', n_getlightenable);
    cl->AddCmd("v_setalphaenable_b",        'SAEN', n_setalphaenable);
    cl->AddCmd("b_getalphaenable_v",        'GAEN', n_getalphaenable);
    cl->AddCmd("v_setzwriteenable_b",       'SZEN', n_setzwriteenable);
    cl->AddCmd("b_getzwriteenable_v",       'GZEN', n_getzwriteenable);
    cl->AddCmd("v_setfogenable_b",          'SFEN', n_setfogenable);
    cl->AddCmd("b_getfogenable_v",          'GFEN', n_getfogenable);
    cl->AddCmd("v_setalphablend_ss",        'SABL', n_setalphablend);
    cl->AddCmd("ss_getalphablend_v",        'GABL', n_getalphablend);
    cl->AddCmd("v_setzfunc_s",              'SZFC', n_setzfunc);
    cl->AddCmd("s_getzfunc_v",              'GZFC', n_getzfunc);
    cl->AddCmd("v_setcullmode_s",           'SCMD', n_setcullmode);
    cl->AddCmd("s_getcullmode_v",           'GCMD', n_getcullmode);
    cl->AddCmd("v_setcolormaterial_s",      'SCLM', n_setcolormaterial);
    cl->AddCmd("s_getcolormaterial_v",      'GCLM', n_getcolormaterial);
    cl->AddCmd("v_setalphatestenable_b",    'SATE', n_setalphatestenable);
    cl->AddCmd("b_getalphatestenable_v",    'GATE', n_getalphatestenable);
    cl->AddCmd("v_setalpharef_f",           'SARF', n_setalpharef);
    cl->AddCmd("f_getalpharef_v",           'GARF', n_getalpharef);
    cl->AddCmd("v_setalphafunc_s",          'SAFC', n_setalphafunc);
    cl->AddCmd("s_getalphafunc_v",          'GAFC', n_getalphafunc);
	cl->AddCmd("v_setwireframe_b",			'SWFE', n_setwireframe);
    cl->AddCmd("b_getwireframe_v",		    'GWFE', n_getwireframe);	
	cl->AddCmd("v_setnormalizenormals_b",   'SNNL', n_setnormalizenormals);
	cl->AddCmd("b_getnormalizenormals_v",   'GNNL', n_getnormalizenormals);
    
    cl->AddCmd("v_setconst0_ffff",          'SCT0', n_setconst0);
    cl->AddCmd("v_setconst1_ffff",          'SCT1', n_setconst1);
    cl->AddCmd("v_setconst2_ffff",          'SCT2', n_setconst2);
    cl->AddCmd("v_setconst3_ffff",          'SCT3', n_setconst3);
    
    cl->AddCmd("v_txyz0_fff",               'TXZ0', n_txyz0);
    cl->AddCmd("v_txyz1_fff",               'TXZ1', n_txyz1);
    cl->AddCmd("v_txyz2_fff",               'TXZ2', n_txyz2);
    cl->AddCmd("v_txyz3_fff",               'TXZ3', n_txyz3);

    cl->AddCmd("v_rxyz0_fff",               'RXZ0', n_rxyz0);
    cl->AddCmd("v_rxyz1_fff",               'RXZ1', n_rxyz1);
    cl->AddCmd("v_rxyz2_fff",               'RXZ2', n_rxyz2);
    cl->AddCmd("v_rxyz3_fff",               'RXZ3', n_rxyz3);

    cl->AddCmd("v_sxyz0_fff",               'SXZ0', n_sxyz0);
    cl->AddCmd("v_sxyz1_fff",               'SXZ1', n_sxyz1);
    cl->AddCmd("v_sxyz2_fff",               'SXZ2', n_sxyz2);
    cl->AddCmd("v_sxyz3_fff",               'SXZ3', n_sxyz3);

    cl->AddCmd("v_setemissiver_f",          'SEMR', n_setemissiver);
    cl->AddCmd("v_setemissiveg_f",          'SEMG', n_setemissiveg);
    cl->AddCmd("v_setemissiveb_f",          'SEMB', n_setemissiveb);
    cl->AddCmd("v_setemissivea_f",          'SEMA', n_setemissivea);

    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setrenderpri

    @input
    i(RenderPri)

    @output
    v

    @info
    Define render priority for the shader node.
    Default priority is 0.
    Smaller values are rendered before higher values.
*/
static void n_setrenderpri(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    self->SetRenderPri(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrenderpri

    @input
    v

    @output
    i(RenderPri)

    @info
    Get the renderpri.
*/
static void n_getrenderpri(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    cmd->Out()->SetI(self->GetRenderPri());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setnumstages

    @input
    i(NumStages)

    @output
    v

    @info
    Define the number of stages this pixel shader requires.
*/
static void n_setnumstages(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    self->SetNumStages(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumstages

    @input
    v

    @output
    i(NumStages)

    @info
    Get the number of stages this pixel shader requires.
*/
static void n_getnumstages(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    cmd->Out()->SetI(self->GetNumStages());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcolorop

    @input
    i(Stage), s(ColorOp)

    @output
    b (Success)

    @info
    Specify a pixel shader operation which works with the
    rgb component of a color (for each stage of the shader,
    a separate pixel and alpha operation may be defined).
    
    The syntax for a valid instruction is:
        op[.1,.2,.4] [-]arg0[.a,.c] [[-]arg1[.a,.c]] [[-]arg1[.a,.c]]
    
    The following operations are defined:
        replace     -> replace color with arg0
        mul         -> multiply (modulate):     res = arg0 * arg1
        add         -> add unsigned:            res = arg0 + arg1
        adds        -> add signed:              res = (arg0 + arg1) - 0.5
        ipol        -> interpolate:             res = arg0*arg2 + arg1*(1-arg2)
        dot         -> dot3:                    res = 4* ((arg0_r - 0.5)*
                                                          (arg1_r - 0.5)
                                                        + (arg0_g - 0.5)*
                                                          (arg1_g - 0.5)
                                                        + (arg0_b - 0.5)*
                                                          (arg1_b - 0.5))

    The dot operation may not be available on all older hardware.  Where
    it isn't available, it is replaced by a multiply.

    The following optional postfixes may be appended to the opcodes to
    manipulate the final result:

        .2  -> multiply the result by 2
        .4  -> multiply the result by 4

    Please note that those postfixes are not supported by all host systems.

    Valid values for 'Arg' are:
        tex     -> the result of the texture read operation of this stage
        prev    -> the result of the previous pixel shader stage
        const   -> the color constant defined for this stage (see 'setconst')
        prim    -> the untextured "base pixel" from the lighting equation

    Argument strings can be prefixed by a '-' (minus sign) to invert the
    argument before it goes into the operation.

    Argument strings can be postfixed by a '.c' or '.a' to explicitly select
    the rgb component (.c) or alpha component (.a) of the pixel. The default
    is '.c' for .setcolorop and '.a' for .setalphaop.

*/
static void n_setcolorop(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    int i         = cmd->In()->GetI();
    const char *s = cmd->In()->GetS();
    cmd->Out()->SetB(self->SetColorOp(i,s));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcolorop

    @input
    i(Stage)

    @output
    s(ColorOp)

    @info
    Return the color operation defined for the given stage.
*/
static void n_getcolorop(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    char buf[32];
    int i = cmd->In()->GetI();
    self->GetColorOp(i, buf, sizeof(buf));
    cmd->Out()->SetS(buf);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setalphaop

    @input
    i(Stage), s(AlphaOp)

    @output
    b (Success)

    @info
    Specify the separate alpha operation for this stage. This
    works exactly like the 'setcolorop' command, but works with
    the alpha component instead of the rgb component.
*/
static void n_setalphaop(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    int i         = cmd->In()->GetI();
    const char *s = cmd->In()->GetS();
    cmd->Out()->SetB(self->SetAlphaOp(i,s));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getalphaop

    @input
    i(Stage)

    @output
    s(AlphaOp)

    @info
    Return the alpha operation defined for the given stage.
*/
static void n_getalphaop(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    char buf[32];
    int i = cmd->In()->GetI();
    self->GetAlphaOp(i, buf, sizeof(buf));
    cmd->Out()->SetS(buf);
}

//------------------------------------------------------------------------------
/**
    @cmd
    begintunit

    @input
    i(Stage)

    @output
    v

    @info
    Start configuring a texture unit.
*/
static void n_begintunit(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    self->BeginTUnit(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    endtunit

    @input
    v

    @output
    i(Stage)

    @info
    Finish configuring a texture unit.
*/
static void n_endtunit(void *o, nCmd *)
{
    nShaderNode *self = (nShaderNode *) o;
    self->EndTUnit();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setaddress

    @input
    s(TextureAddressU), s(TextureAddressV)

    @output
    v

    @info
    Set the texture addressing modes for the u and v dimension
    for the current texture unit.
    Valid values are:
        'clamp'     - clamp values outside 0.0 and 1.0
        'wrap'      - wrap around (repeat)
*/
static void n_setaddress(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    const char *s0 = cmd->In()->GetS();
    const char *s1 = cmd->In()->GetS();
    self->SetAddress(str2param(s0),str2param(s1));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getaddress

    @input
    i(Unit)

    @output
    s(TextureAddressU), s(TextureAddressV)

    @info
    Return the texture addressing modes for the u and v dimension
    of the given stage as defined by 'setaddress'.
*/
static void n_getaddress(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    int i = cmd->In()->GetI();
    nRStateParam p0,p1;
    self->GetAddress(i,p0,p1);
    cmd->Out()->SetS(param2str(p0));
    cmd->Out()->SetS(param2str(p1));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setminmagfilter

    @input
    s(TextureMinFilter), s(TextureMagFilter)

    @output
    v

    @info
    Set the texture minify and magnify filters for the current
    texture unit.
    
    Valid values for the minify filter are:
        nearest
        linear
        nearest_mipmap_nearest
        linear_mipmap_nearest
        nearest_mipmap_linear
        linear_mipmap_linear
    
    Valid values for the magnify filter are:
        nearest
        linear
*/
static void n_setminmagfilter(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    const char *s0 = cmd->In()->GetS();
    const char *s1 = cmd->In()->GetS();
    self->SetMinMagFilter(str2param(s0),str2param(s1));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getminmagfilter

    @input
    i(Unit)

    @output
    s(TextureMinFilter), s(TextureMagFilter)

    @info
    Get the current minify and magnify texture filters for
    the given texture unit.
*/
static void n_getminmagfilter(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    int i = cmd->In()->GetI();
    nRStateParam p0,p1;
    self->GetMinMagFilter(i,p0,p1);
    cmd->Out()->SetS(param2str(p0));
    cmd->Out()->SetS(param2str(p1));
}

//------------------------------------------------------------------------------
/**
    @cmd
    settexcoordsrc

    @input
    s(TexCoordSrc)

    @output
    v

    @info
    Set the source where this texture unit should get its
    uv coordinates from.
    
    Valid values are:
        uv0..uv3    - the vertex buffer uv coordinate streams 0..3
        objectspace - generate on the fly from object space coordinates
        eyespace    - generate on the fly from eye space coordinates
        spheremap   - generate on the fly for spherical env mapping
    
    *** IMPORTANT ***
    When using 'objectspace', 'eyespace', 'spheremap' modes,
    the '.setenabletransform' flag must be set to true, even if
    the texture matrix is not manipulated.
*/
static void n_settexcoordsrc(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    const char *s0 = cmd->In()->GetS();
    self->SetTexCoordSrc(str2param(s0));
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettexcoordsrc

    @input
    i(Unit)

    @output
    s(TexCoordSrc)

    @info
    Get the current texture coordinate source.
*/
static void n_gettexcoordsrc(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    int i = cmd->In()->GetI();
    nRStateParam p0;
    p0 = self->GetTexCoordSrc(i);
    cmd->Out()->SetS(param2str(p0));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setconst

    @input
    i(ConstNum), f(Red), f(Green), f(Blue), f(Alpha)

    @output
    v

    @info
    Set a color constant.
    The color constant can be accessed in the pixel shader
    operation through the 'const' argument. Please see also
    the 'setconst0..4' commands which can be used to write
    the constants from interpolator objects.
*/
static void n_setconst(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector4 v;
    int i = cmd->In()->GetI();
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    v.w = cmd->In()->GetF();
    self->SetConst(i,v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getconst

    @input
    i(Stage)

    @output
    f(Red), f(Green), f(Blue), f(Alpha)

    @info
    Get the color constant of the given pixel shader stage.
*/
static void n_getconst(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    int i = cmd->In()->GetI();
    vector4 v = self->GetConst(i);
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmiplodbias
    @input
    i(MipLodBias)
    @output
    v
    @info
    Set the mipmap lod bias for all textures rendered with this pixel shader.
*/
static
void
n_setmiplodbias(void* slf, nCmd* cmd)
{
    nShaderNode* self = (nShaderNode*) slf;
    self->SetMipLodBias(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmiplodbias
    @input
    v
    @output
    i(MipLodBias)
    @info
    Get the mipmap lod bias.
*/
static
void
n_getmiplodbias(void* slf, nCmd* cmd)
{
    nShaderNode* self = (nShaderNode*) slf;
    cmd->Out()->SetI(self->GetMipLodBias());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setenabletransform

    @input
    b(EnableTransform)

    @output
    v

    @info
    Explicitely turn on/off texture coordinate transformation
    for this texture unit. If turned off, the commands
    'txyz', 'rxyz', 'sxyz' will have no effect. The default
    value is 'false'.
*/
static void n_setenabletransform(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    self->SetEnableTransform(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getenabletransform

    @input
    i(Stage)

    @output
    b(EnableTransform)

    @info
    Get the current state of 'setenabletransform'.
*/
static void n_getenabletransform(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    int i = cmd->In()->GetI();
    cmd->Out()->SetB(self->GetEnableTransform(i));
}

//------------------------------------------------------------------------------
/**
    @cmd
    txyz

    @input
    f(X), f(Y), f(Z)

    @output
    v

    @info
    Translate the texture matrix of the current texture unit. Please
    see also the 'txyz0..4' commands, which can be used to
    attach interpolators.
*/
static void n_txyz(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->Txyz(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gett

    @input
    i(Unit)

    @output
    f(X), f(Y), f(Z)

    @info
    Get the current texture matrix translation of the given
    texture unit.
*/
static void n_gett(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    int i = cmd->In()->GetI();
    vector3 v = self->GetT(i);
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    rxyz

    @input
    f(X), f(Y), f(Z)

    @output
    v

    @info
    Rotate the texture matrix of the current texture unit in Euler 
    notation. Please see also the 'rxyz0..4' commands which can be 
    used to attach interpolators.
*/
static void n_rxyz(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->Rxyz(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getr

    @input
    i(Unit)

    @output
    f(X), f(Y), f(Z)

    @info
    Get the current texture matrix rotation of the given texture
    unit.
*/
static void n_getr(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    int i = cmd->In()->GetI();
    vector3 v = self->GetR(i);
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    sxyz

    @input
    f(X), f(Y), f(Z)

    @output
    v

    @info
    Scale the texture matrix of the current texture unit 
    Please see also the 'sxyz0..4' commands which can be 
    used to attach interpolators.  If you scale the texture
    matrix, you will need to enable transforms as well via
    setenabletransform.
*/
static void n_sxyz(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->Sxyz(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gets

    @input
    i(Unit)

    @output
    f(X), f(Y), f(Z)

    @info
    Get the current texture matrix scale of the given texture
    unit.
*/
static void n_gets(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    int i = cmd->In()->GetI();
    vector3 v = self->GetS(i);
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setdiffuse

    @input
    f(Red), f(Green), f(Blue), f(Alpha)

    @output
    v

    @info
    Set the diffuse reflection component for the lighting 
    equation.
*/
static void n_setdiffuse(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector4 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    v.w = cmd->In()->GetF();
    self->SetDiffuse(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getdiffuse

    @input
    v

    @output
    f(Red), f(Green), f(Blue), f(Alpha)

    @info
    Get the diffuse reflection component for the lighting 
    equation.
*/
static void n_getdiffuse(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector4 v = self->GetDiffuse();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setemissive
    setemissiver
    setemissiveg
    setemissiveb
    setemissivea

    @input
    f(Red), f(Green), f(Blue), f(Alpha)

    @output
    v

    @info
    Set the emissive component for the lighting 
    equation.
*/
static void n_setemissive(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector4 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    v.w = cmd->In()->GetF();
    self->SetEmissive(v);
}

static void n_setemissiver(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector4 c = self->GetEmissive();
    c.x = cmd->In()->GetF();
    self->SetEmissive(c);
}

static void n_setemissiveg(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector4 c = self->GetEmissive();
    c.y = cmd->In()->GetF();
    self->SetEmissive(c);
}

static void n_setemissiveb(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector4 c = self->GetEmissive();
    c.z = cmd->In()->GetF();
    self->SetEmissive(c);
}

static void n_setemissivea(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector4 c = self->GetEmissive();
    c.w = cmd->In()->GetF();
    self->SetEmissive(c);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getemissive

    @input
    v

    @output
    f(Red), f(Green), f(Blue), f(Alpha)

    @info
    Get the emissive component for the lighting 
    equation.
*/
static void n_getemissive(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector4 v = self->GetEmissive();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setambient

    @input
    f(Red), f(Green), f(Blue), f(Alpha)

    @output
    v

    @info
    Set the ambient reflection component for the lighting 
    equation.
*/
static void n_setambient(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector4 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    v.w = cmd->In()->GetF();
    self->SetAmbient(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getambient

    @input
    v

    @output
    f(Red), f(Green), f(Blue), f(Alpha)

    @info
    Get the ambient component for the lighting 
    equation.
*/
static void n_getambient(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector4 v = self->GetAmbient();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlightenable

    @input
    b (LightEnable)

    @output
    v

    @info
    Enable/disable lighting. If lighting is enabled, the per-vertex-
    color will be computed by the lighting equation and the rendered 
    vertex buffer should contain vertex
    normals. If lighting is disabled, the vertex colors are 
    routed to the renderer directly, thus the vertex buffer
    should provide vertex colors.
*/
static void n_setlightenable(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    self->SetLightEnable(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlightenable

    @input
    v

    @output
    b (LightEnable)

    @info
    Get the current light enabled state.
*/
static void n_getlightenable(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    cmd->Out()->SetB(self->GetLightEnable());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setalphaenable

    @input
    b (AlphaEnable)

    @output
    v

    @info
    Enable/disable alpha blending. Alpha blending defines how
    the result from the final pixel shader stage is written 
    to the frame buffer. If alpha blending is enabled,
    the 'setalphablend' command describes the actual blending 
    operation.
*/
static void n_setalphaenable(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    self->SetAlphaEnable(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getalphaenable

    @input
    v

    @output
    b (AlphaEnable)

    @info
    Get the current alpha blending enabled state.
*/
static void n_getalphaenable(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    cmd->Out()->SetB(self->GetAlphaEnable());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setzwriteenable

    @input
    b (ZWriteEnable)

    @output
    v

    @info
    Enable/disable writing to the zbuffer.
*/
static void n_setzwriteenable(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    self->SetZWriteEnable(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getzwriteenable

    @input
    v

    @output
    b (ZWriteEnable)

    @info
    Get the current zwrite enable state.
*/
static void n_getzwriteenable(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    cmd->Out()->SetB(self->GetZWriteEnable());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfogenable

    @input
    b (FogEnable)

    @output
    v

    @info
    Enable/disable fogging.
*/
static void n_setfogenable(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    self->SetFogEnable(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfogenable

    @input
    v

    @output
    b (FogEnable)

    @info
    Get the current fog enable state.
*/
static void n_getfogenable(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    cmd->Out()->SetB(self->GetFogEnable());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setalphablend

    @input
    s(SrcBlend), s(DestBlend)

    @output
    v

    @info
    Define the alpha blending operation, which describes how
    the result of the final pixel shader stage will be
    combined with the frame buffer.
	SrcBlend and DestBlend can be
	zero
	one 
	srccolor
	invsrccolor
	srcalpha
	destalpha
	invdestalpha
	destcolor
	invdestcolor
*/
static void n_setalphablend(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    const char *s0 = cmd->In()->GetS();
    const char *s1 = cmd->In()->GetS();
    self->SetAlphaBlend(str2param(s0),str2param(s1));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getalphablend

    @input
    v

    @output
    s(SrcBlend), s(DestBlend)

    @info
    Get the current alpha blending operation.
*/
static void n_getalphablend(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    nRStateParam p0,p1;
    self->GetAlphaBlend(p0,p1);
    cmd->Out()->SetS(param2str(p0));
    cmd->Out()->SetS(param2str(p1));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setzfunc

    @input
    s(ZFunc)

    @output
    v

    @info
    Define the function which is used for the z check.
    Valid values are:
        never
        less
        equal
        lessequal
        greater
        notequal
        greaterequal
        always
    The default value is lessequal.
*/
static void n_setzfunc(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    self->SetZFunc(str2param(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getzfunc

    @input
    v

    @output
    s(ZFunc)

    @info
    Get the function which is used for the z check.
*/
static void n_getzfunc(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    cmd->Out()->SetS(param2str(self->GetZFunc()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcullmode

    @input
    s(CullMode)

    @output
    v

    @info
    Set the mode used for backface culling.
    
    Valid values are:
        none
        cw
        ccw
*/
static void n_setcullmode(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    self->SetCullMode(str2param(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcullmode

    @input
    v

    @output
    s(CullMode)

    @info
    Get the mode used for backface culling.
*/
static void n_getcullmode(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    cmd->Out()->SetS(param2str(self->GetCullMode()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcolormaterial

    @input
    s(ColorMaterial)

    @output
    v

    @info
    Define whether and how the vertex color should be included
    into the lighting equation. With 'setcolormaterial', the per-vertex-
    color can be routed into one the 'diffuse', 'emissive' or
    'ambient' component of the lighting equation on a per vertex 
    basis.
    Valid values are:
        material    - don't use per-vertex-color in lighting
        diffuse     - route the vertex color to the diffuse component
        emissive    - route the vertex color to the emissive component
        ambient     - route the vertex color to the ambient component
*/
static void n_setcolormaterial(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    self->SetColorMaterial(str2param(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcolormaterial

    @input
    v

    @output
    s(ColorMaterial)

    @info
    Get the value defined by 'setcolormaterial'
*/
static void n_getcolormaterial(void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    cmd->Out()->SetS(param2str(self->GetColorMaterial()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setalphatestenable

    @input
    b (AlphaTestEnable)

    @output
    v

    @info
    Turn alpha test on/off.
*/
static void n_setalphatestenable(void* o, nCmd* cmd)
{
    nShaderNode* self = (nShaderNode*) o;
    self->SetAlphaTestEnable(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getalphatestenable

    @input
    v

    @output
    b (AlphaTestEnable)

    @info
    Get alpha test state.
*/
static void n_getalphatestenable(void* o, nCmd* cmd)
{
    nShaderNode* self = (nShaderNode*) o;
    cmd->Out()->SetB(self->GetAlphaTestEnable());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setalpharef

    @input
    f (AlphaRef)

    @output
    v

    @info
    Set the reference value for alpha testing (0..1).
*/
static void n_setalpharef(void* o, nCmd* cmd)
{
    nShaderNode* self = (nShaderNode*) o;
    self->SetAlphaRef(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getalpharef

    @input
    v

    @output
    f (AlphaRef)

    @info
    Get alpha ref value.
*/
static void n_getalpharef(void* o, nCmd* cmd)
{
    nShaderNode* self = (nShaderNode*) o;
    cmd->Out()->SetF(self->GetAlphaRef());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setalphafunc

    @input
    s (AlphaFunc)

    @output
    v

    @info
    Set the compare operator to use for the alpha test:
        never
        less
        equal
        lessequal
        greater
        notequal
        greaterequal
        always
*/
static void n_setalphafunc(void* o, nCmd* cmd)
{
    nShaderNode* self = (nShaderNode*) o;
    self->SetAlphaFunc(str2param(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getalphafunc

    @input
    v

    @output
    s (AlphaFunc)

    @info
    Get the compare operator to use for the alpha test:
*/
static void n_getalphafunc(void* o, nCmd* cmd)
{
    nShaderNode* self = (nShaderNode*) o;
    cmd->Out()->SetS(param2str(self->GetAlphaFunc()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setwireframe

    @input
    b (Wireframe Enable)

    @output
    v

    @info
    Turn wireframe rendering off/on.
*/
static void n_setwireframe(void* o, nCmd* cmd)
{
    nShaderNode* self = (nShaderNode*) o;
    self->SetWireframe(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getwireframe

    @input
    v

    @output
    b (Wireframe Enable)

    @info
    Get wireframe rendering state.
*/
static void n_getwireframe(void* o, nCmd* cmd)
{
    nShaderNode* self = (nShaderNode*) o;
    cmd->Out()->SetB(self->GetWireframe());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setconst# (#=0..3)

    @input
    f(Red), f(Green), f(Blue), f(Alpha)

    @output
    v

    @info
    Set the color constant for one of the 7 texture units
    directly. Useful for attaching interpolators.
*/
inline static void n_setconstn(int i, void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector4 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    v.w = cmd->In()->GetF();
    self->SetConst(i,v);
}

static void n_setconst0(void *o, nCmd *cmd)
{
    n_setconstn(0,o,cmd);
}
static void n_setconst1(void *o, nCmd *cmd)
{
    n_setconstn(1,o,cmd);
}
static void n_setconst2(void *o, nCmd *cmd)
{
    n_setconstn(2,o,cmd);
}
static void n_setconst3(void *o, nCmd *cmd)
{
    n_setconstn(3,o,cmd);
}

//------------------------------------------------------------------------------
/**
    @cmd
    txyz# (#=0..3)

    @input
    f(X), f(Y), f(Z)

    @output
    v

    @info
    Set the texture matrix translation for one of the texture
    units directly. Useful for attaching interpolators.
*/
inline static void n_txyzn(int i, void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->Txyz(i,v);
}

static void n_txyz0(void *o, nCmd *cmd)
{
    n_txyzn(0,o,cmd);
}
static void n_txyz1(void *o, nCmd *cmd)
{
    n_txyzn(1,o,cmd);
}
static void n_txyz2(void *o, nCmd *cmd)
{
    n_txyzn(2,o,cmd);
}
static void n_txyz3(void *o, nCmd *cmd)
{
    n_txyzn(3,o,cmd);
}

//------------------------------------------------------------------------------
/**
    @cmd
    rxyz# (#=0..3)

    @input
    f(X), f(Y), f(Z)

    @output
    v

    @info
    Set the texture matrix rotation for one of the texture
    units directly. Useful for attaching interpolators.
*/
inline static void n_rxyzn(int i, void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->Rxyz(i,v);
}

static void n_rxyz0(void *o, nCmd *cmd)
{
    n_rxyzn(0,o,cmd);
}
static void n_rxyz1(void *o, nCmd *cmd)
{
    n_rxyzn(1,o,cmd);
}
static void n_rxyz2(void *o, nCmd *cmd)
{
    n_rxyzn(2,o,cmd);
}
static void n_rxyz3(void *o, nCmd *cmd)
{
    n_rxyzn(3,o,cmd);
}

//------------------------------------------------------------------------------
/**
    @cmd
    sxyz# (#=0..3)

    @input
    f(X), f(Y), f(Z)

    @output
    v

    @info
    Set the texture matrix scaling for one of the texture
    units directly. Useful for attaching interpolators.
*/
inline static void n_sxyzn(int i, void *o, nCmd *cmd)
{
    nShaderNode *self = (nShaderNode *) o;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->Sxyz(i,v);
}

static void n_sxyz0(void *o, nCmd *cmd)
{
    n_sxyzn(0,o,cmd);
}
static void n_sxyz1(void *o, nCmd *cmd)
{
    n_sxyzn(1,o,cmd);
}
static void n_sxyz2(void *o, nCmd *cmd)
{
    n_sxyzn(2,o,cmd);
}
static void n_sxyz3(void *o, nCmd *cmd)
{
    n_sxyzn(3,o,cmd);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setnormalizenormals

    @input
    b (Enable normal normalization)

    @output
    v

    @info
    Set whether or not normalization of normals should be enabled. This
    is necessary to have enabled for correct lighting on an object that
    is being scaled.  However, it is also a performance hit and should
    only be enabled when absolutely necessary.
*/
static void n_setnormalizenormals(void* o, nCmd* cmd)
{
	nShaderNode *self = (nShaderNode *) o;   
    self->SetNormalizeNormals(cmd->In()->GetB());    
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnormalizenormals

    @input
    v

    @output
    b (Enable normal normalization)

    @info
    Get whether or not normalization of normals has been enabled. This
    is necessary to have enabled for correct lighting on an object that
    is being scaled.  However, it is also a performance hit and should
    only be enabled when absolutely necessary.
*/
static void n_getnormalizenormals(void* o, nCmd* cmd)
{
	nShaderNode *self = (nShaderNode *) o;   
    cmd->Out()->SetB(self->GetNormalizeNormals());    
}

//-------------------------------------------------------------------
//  SaveCmds()
//  22-Aug-00   floh    created
//-------------------------------------------------------------------
bool nShaderNode::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;
        int i;
        int stages = this->GetNumStages();
        vector3 v3;
        vector4 v4;

        //--- setrenderpri ---
        cmd = fs->GetCmd(this,'SRPR');
        cmd->In()->SetI(this->GetRenderPri());
        fs->PutCmd(cmd);

        //--- setnumstages ---
        cmd = fs->GetCmd(this,'SNST');
        cmd->In()->SetI(stages);
        fs->PutCmd(cmd);

        //--- pixel shader instruction sequence ---
        for (i=0; i<stages; i++) {
            char buf[32];

            this->GetColorOp(i, buf, sizeof(buf));
            cmd = fs->GetCmd(this,'SPOP');
            cmd->In()->SetI(i);
            cmd->In()->SetS(buf);
            fs->PutCmd(cmd);

            this->GetAlphaOp(i, buf, sizeof(buf));
            cmd = fs->GetCmd(this,'SAOP');
            cmd->In()->SetI(i);
            cmd->In()->SetS(buf);
            fs->PutCmd(cmd);
        }

        //--- constants ---
        for (i=0; i<stages; i++) {
            //--- setconst ---
            cmd = fs->GetCmd(this,'SCOT');
            vector4 v = this->GetConst(i);
            cmd->In()->SetI(i);
            cmd->In()->SetF(v.x);
            cmd->In()->SetF(v.y);
            cmd->In()->SetF(v.z);
            cmd->In()->SetF(v.w);
            fs->PutCmd(cmd);
        }

        //--- texture unit definitions ---
        for (i=0; i<stages; i++) {

            nRStateParam p0,p1;

            //--- begintunit ---
            cmd = fs->GetCmd(this,'BGTU');
            cmd->In()->SetI(i);
            fs->PutCmd(cmd);

            //--- setaddress ---
            cmd = fs->GetCmd(this,'SADR');
            this->GetAddress(i,p0,p1);
            cmd->In()->SetS(param2str(p0));
            cmd->In()->SetS(param2str(p1));
            fs->PutCmd(cmd);

            //--- setminmagfilter ---
            cmd = fs->GetCmd(this,'SMMF');
            this->GetMinMagFilter(i,p0,p1);
            cmd->In()->SetS(param2str(p0));
            cmd->In()->SetS(param2str(p1));
            fs->PutCmd(cmd);

            //--- settexcoordsrc ---
            cmd = fs->GetCmd(this,'STCS');
            p0 = this->GetTexCoordSrc(i);
            cmd->In()->SetS(param2str(p0));
            fs->PutCmd(cmd);

            //--- setenabletransform ---
            cmd = fs->GetCmd(this,'SENT');
            cmd->In()->SetB(this->GetEnableTransform(i));
            fs->PutCmd(cmd);

            //--- txyz ---
            cmd = fs->GetCmd(this,'TXYZ');
            v3 = this->GetT(i);
            cmd->In()->SetF(v3.x);
            cmd->In()->SetF(v3.y);
            cmd->In()->SetF(v3.z);
            fs->PutCmd(cmd);

            //--- rxyz ---
            cmd = fs->GetCmd(this,'RXYZ');
            v3 = this->GetR(i);
            cmd->In()->SetF(v3.x);
            cmd->In()->SetF(v3.y);
            cmd->In()->SetF(v3.z);
            fs->PutCmd(cmd);

            //--- sxyz ---
            cmd = fs->GetCmd(this,'SXYZ');
            v3 = this->GetS(i);
            cmd->In()->SetF(v3.x);
            cmd->In()->SetF(v3.y);
            cmd->In()->SetF(v3.z);
            fs->PutCmd(cmd);

            //--- endtunit ---
            cmd = fs->GetCmd(this,'EDTU');
            fs->PutCmd(cmd);
        }

        //--- setdiffuse ---
        cmd = fs->GetCmd(this,'SDIF');
        v4 = this->GetDiffuse();
        cmd->In()->SetF(v4.x);
        cmd->In()->SetF(v4.y);
        cmd->In()->SetF(v4.z);
        cmd->In()->SetF(v4.w);
        fs->PutCmd(cmd);

        //--- setemissive ---
        cmd = fs->GetCmd(this,'SEMV');
        v4 = this->GetEmissive();
        cmd->In()->SetF(v4.x);
        cmd->In()->SetF(v4.y);
        cmd->In()->SetF(v4.z);
        cmd->In()->SetF(v4.w);
        fs->PutCmd(cmd);

        //--- setambient ---
        cmd = fs->GetCmd(this,'SAMB');
        v4 = this->GetAmbient();
        cmd->In()->SetF(v4.x);
        cmd->In()->SetF(v4.y);
        cmd->In()->SetF(v4.z);
        cmd->In()->SetF(v4.w);
        fs->PutCmd(cmd);

        //--- setlightenable ---
        cmd = fs->GetCmd(this,'SLEN');
        cmd->In()->SetB(this->GetLightEnable());
        fs->PutCmd(cmd);

        //--- setalphaenable ---
        cmd = fs->GetCmd(this,'SAEN');
        cmd->In()->SetB(this->GetAlphaEnable());
        fs->PutCmd(cmd);

        //--- setzwriteenable ---
        cmd = fs->GetCmd(this,'SZEN');
        cmd->In()->SetB(this->GetZWriteEnable());
        fs->PutCmd(cmd);

        //--- setfogenable ---
        cmd = fs->GetCmd(this,'SFEN');
        cmd->In()->SetB(this->GetFogEnable());
        fs->PutCmd(cmd);

        //--- setalphablend ---
        cmd = fs->GetCmd(this,'SABL');
        nRStateParam p0,p1;
        this->GetAlphaBlend(p0,p1);
        cmd->In()->SetS(param2str(p0));
        cmd->In()->SetS(param2str(p1));
        fs->PutCmd(cmd);

        //--- setzfunc ---
        cmd = fs->GetCmd(this,'SZFC');
        p0 = this->GetZFunc();
        cmd->In()->SetS(param2str(p0));
        fs->PutCmd(cmd);

        //--- setcullmode ---
        cmd = fs->GetCmd(this,'SCMD');
        p0 = this->GetCullMode();
        cmd->In()->SetS(param2str(p0));
        fs->PutCmd(cmd);

        //--- setcolormaterial ---
        cmd = fs->GetCmd(this,'SCLM');
        p0 = this->GetColorMaterial();
        cmd->In()->SetS(param2str(p0));
        fs->PutCmd(cmd);

        //--- setalphatestenable ---
        cmd = fs->GetCmd(this, 'SATE');
        cmd->In()->SetB(this->GetAlphaTestEnable());
        fs->PutCmd(cmd);

        //--- setalpharef ---
        cmd = fs->GetCmd(this, 'SARF');
        cmd->In()->SetF(this->GetAlphaRef());
        fs->PutCmd(cmd);

        //--- setalphafunc ---
        cmd = fs->GetCmd(this, 'SAFC');
        cmd->In()->SetS(param2str(this->GetAlphaFunc()));
        fs->PutCmd(cmd);

        //--- setmiplodbias ---
        cmd = fs->GetCmd(this, 'SMLB');
        cmd->In()->SetI(this->GetMipLodBias());
        fs->PutCmd(cmd);

		//--- setwireframe ---
        cmd = fs->GetCmd(this, 'SWFE');
        cmd->In()->SetB(this->GetWireframe());
        fs->PutCmd(cmd);

        //--- setnormalizenormals ---
		cmd = fs->GetCmd(this, 'SNNL');
        cmd->In()->SetB(this->GetNormalizeNormals());
        fs->PutCmd(cmd);

		retval = true;
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

