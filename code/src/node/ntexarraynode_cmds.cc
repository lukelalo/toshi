#define N_IMPLEMENTS nTexArrayNode
//-------------------------------------------------------------------
//  ntexarraynode_cmds.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "node/ntexarraynode.h"
#include "kernel/npersistserver.h"

static void n_settexture(void *, nCmd *);
static void n_gettexture(void *, nCmd *);
static void n_setgenmipmaps(void *, nCmd *);
static void n_getgenmipmaps(void *, nCmd *);
static void n_sethighquality(void*, nCmd*);
static void n_gethighquality(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ntexarraynode

    @superclass
    nvisnode

    @classinfo
    Define textures for nshadernode. Holds all textures for
    the multitexture stage. MUST be used if working with nshadernode,
    instead of ntexnode.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_settexture_iss",      'STXT', n_settexture);
    cl->AddCmd("ss_gettexture_i",       'GTXT', n_gettexture);
    cl->AddCmd("v_setgenmipmaps_ib",    'SGMM', n_setgenmipmaps);
    cl->AddCmd("b_getgenmipmaps_i",     'GGMM', n_getgenmipmaps);
    cl->AddCmd("v_sethighquality_ib",   'SHQL', n_sethighquality);
    cl->AddCmd("b_gethighquality_i",    'GHQL', n_gethighquality);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    settexture

    @input
    i(TextureStage), s(PixelFileName), s(AlphaFileName)

    @output
    v

    @info
    Define filenames for textures of a stage. A 'none' filename
    is allowed for both the pixel and alpha channel files.
    The files must currently be 8 or 24 bpp uncompressed BMP files,
    or any image format supported by the version and build of DevIL
    that your application is using.
    
    Also note that if you supply an alpha channel file both the
    pixel and alpha file have to be BMPs.
*/
static void n_settexture(void *o, nCmd *cmd)
{
    nTexArrayNode *self = (nTexArrayNode *) o;
    int i0 = cmd->In()->GetI();
    const char *s0 = cmd->In()->GetS();
    const char *s1 = cmd->In()->GetS();
    if (strcmp(s0,"none")==0) s0=NULL;
    if (strcmp(s1,"none")==0) s1=NULL;
    self->SetTexture(i0,s0,s1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettexture

    @input
    i(TextureStage)

    @output
    s(PixelFileName), s(AlphaFileName)

    @info
    Return filenames for given texture stage, a 'none' filename
    can be returned for both strings.
*/
static void n_gettexture(void *o, nCmd *cmd)
{
    nTexArrayNode *self = (nTexArrayNode *) o;
    int i0 = cmd->In()->GetI();
    const char *s0,*s1;
    self->GetTexture(i0,s0,s1);
    if (!s0) s0="none";
    if (!s1) s1="none";
    cmd->Out()->SetS(s0);
    cmd->Out()->SetS(s1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setgenmipmaps

    @input
    i(TextureStage), b(GenMipMaps)

    @output
    v

    @info
    Turn automatic mipmap generation for a stage on/off. Default is
    'true', mipmap generation on.
*/
static void n_setgenmipmaps(void *o, nCmd *cmd)
{
    nTexArrayNode *self = (nTexArrayNode *) o;
    int i0  = cmd->In()->GetI();
    bool b0 = cmd->In()->GetB();
    self->SetGenMipMaps(i0,b0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getgenmipmaps

    @input
    i(TextureStage)

    @output
    b(GenMipMaps)

    @info
    Return automatic mipmaps generation flag of given texture stage.
*/
static void n_getgenmipmaps(void *o, nCmd *cmd)
{
    nTexArrayNode *self = (nTexArrayNode *) o;
    int i0 = cmd->In()->GetI();
    cmd->Out()->SetB(self->GetGenMipMaps(i0));
}

//------------------------------------------------------------------------------
/**
    @cmd
    sethighquality

    @input
    i(TextureStage), b(HighQuality)

    @output
    v

    @info
    Set the high quality flag for this texture stage. This may especially
    be useful for textures with alpha channel, where full 32 bit
    resolution is needed.
*/
static void n_sethighquality(void *o, nCmd *cmd)
{
    nTexArrayNode *self = (nTexArrayNode *) o;
    int i0  = cmd->In()->GetI();
    bool b0 = cmd->In()->GetB();
    self->SetHighQuality(i0,b0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gethighquality

    @input
    i(TextureStage)

    @output
    b(HighQuality)

    @info
    Get high quality flag for given texture stage.
*/
static void n_gethighquality(void *o, nCmd *cmd)
{
    nTexArrayNode *self = (nTexArrayNode *) o;
    int i0 = cmd->In()->GetI();
    cmd->Out()->SetB(self->GetHighQuality(i0));
}

//-------------------------------------------------------------------
//  SaveCmds()
//  16-Nov-00   floh    created
//-------------------------------------------------------------------
bool nTexArrayNode::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;
        int i;

        //--- settexture ---
        for (i=0; i<N_MAXNUM_TEXSTAGES; i++) {
            const char *s0,*s1;
            this->GetTexture(i,s0,s1);
            if (s0) {
                if (!s1) s1="none";
                cmd = fs->GetCmd(this,'STXT');
                cmd->In()->SetI(i);
                cmd->In()->SetS(s0);
                cmd->In()->SetS(s1);
                fs->PutCmd(cmd);
            }
        }

        //--- setgenmipmaps ---
        for (i=0; i<N_MAXNUM_TEXSTAGES; i++) {
            if (!this->GetGenMipMaps(i))
            {
                cmd = fs->GetCmd(this,'SGMM');
                cmd->In()->SetI(i);
                cmd->In()->SetB(this->GetGenMipMaps(i));
                fs->PutCmd(cmd);
            }
        }

        //--- sethighquality ---
        for (i = 0; i < N_MAXNUM_TEXSTAGES; i++)
        {
            if (this->GetHighQuality(i))
            {
                cmd = fs->GetCmd(this, 'SHQL');
                cmd->In()->SetI(i);
                cmd->In()->SetB(this->GetHighQuality(i));
                fs->PutCmd(cmd);
            }
        }

        retval = true;
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
