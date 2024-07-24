#define N_IMPLEMENTS nMeshMixer
//-------------------------------------------------------------------
//  nmeshmixer_cmds.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/nmeshmixer.h"

static void n_setreadonly(void *, nCmd *);
static void n_getreadonly(void *, nCmd *);
static void n_beginsources(void *, nCmd *);
static void n_setsource(void *, nCmd *);
static void n_endsources(void *, nCmd *);
static void n_getnumsources(void *, nCmd *);
static void n_getsource(void *, nCmd *);
static void n_setnormalize(void *, nCmd *);
static void n_getnormalize(void *, nCmd *);
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
    nmeshmixer

    @superclass
    nvisnode

    @classinfo
    Render a mesh from a number of weighted source meshes.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setreadonly_b",               'SRDO', n_setreadonly);
    cl->AddCmd("b_getreadonly_v",               'GRDO', n_getreadonly);
    cl->AddCmd("v_beginsources_i",              'BSRC', n_beginsources);
    cl->AddCmd("v_setsource_iss",               'SSRC', n_setsource);
    cl->AddCmd("v_endsources_v",                'ESRC', n_endsources);
    cl->AddCmd("i_getnumsources_v",             'GNSR', n_getnumsources);
    cl->AddCmd("ss_getsource_i",                'GSRC', n_getsource);
    cl->AddCmd("v_setnormalize_b",              'SNRM', n_setnormalize);
    cl->AddCmd("b_getnormalize_v",              'GNRM', n_getnormalize);
    cl->AddCmd("v_setupdatecoord_b",            'SUCD', n_setupdatecoord);
    cl->AddCmd("b_getupdatecoord_v",            'GUCD', n_getupdatecoord);
    cl->AddCmd("v_setupdatenorm_b",             'SUNM', n_setupdatenorm);
    cl->AddCmd("b_getupdatenorm_v",             'GUNM', n_getupdatenorm);
    cl->AddCmd("v_setupdatecolor_b",            'SUCL', n_setupdatecolor);
    cl->AddCmd("b_getupdatecolor_v",            'GUCL', n_getupdatecolor);
    cl->AddCmd("v_setupdateuv0_b",              'SUV0', n_setupdateuv0);
    cl->AddCmd("b_getupdateuv0_v",              'GUV0', n_getupdateuv0);
    cl->AddCmd("v_setupdateuv1_b",              'SUV1', n_setupdateuv1);
    cl->AddCmd("b_getupdateuv1_v",              'GUV1', n_getupdateuv1);
    cl->AddCmd("v_setupdateuv2_b",              'SUV2', n_setupdateuv2);
    cl->AddCmd("b_getupdateuv2_v",              'GUV2', n_getupdateuv2);
    cl->AddCmd("v_setupdateuv3_b",              'SUV3', n_setupdateuv3);
    cl->AddCmd("b_getupdateuv3_v",              'GUV3', n_getupdateuv3);
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
    nMeshMixer *self = (nMeshMixer *) o;
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
    nMeshMixer *self = (nMeshMixer *) o;
    cmd->Out()->SetB(self->GetReadOnly());
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginsources

    @input
    i (NumSources)

    @output
    v

    @info
    Begin defining source objects, expects number of sources.
    Source objects must be of class nmeshnode.
*/
void n_beginsources(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    self->BeginSources(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setsource

    @input
    i (SourceIndex), s (SourceObjectPath), s (ChannelName)

    @output
    v

    @info
    Define one of the source objects. The mixing weight value is
    directly taken from the channel.
*/
void n_setsource(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    int i = cmd->In()->GetI();
    const char *s0 = cmd->In()->GetS();
    const char *s1 = cmd->In()->GetS();
    self->SetSource(i,s0,s1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endsources

    @input
    v

    @output
    v

    @info
    Finish defining source objects.
*/
void n_endsources(void *o, nCmd *)
{
    nMeshMixer *self = (nMeshMixer *) o;
    self->EndSources();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumsources

    @input
    v

    @output
    i (NumSources)

    @info
    Return number of sources defined by 'beginsources'.
*/
void n_getnumsources(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    cmd->Out()->SetI(self->GetNumSources());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getsource

    @input
    i (SourceIndex)

    @output
    s (SourceObjectPath), s (ChannelName)

    @info
    Return the source object and the optional channel name
    defined at a given index.
*/
void n_getsource(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    const char *s0;
    const char *s1;
    self->GetSource(cmd->In()->GetI(),s0,s1);
    cmd->Out()->SetS(s0);
    cmd->Out()->SetS(s1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setnormalize

    @input
    b (Normalize)

    @output
    v

    @info
    Define whether weights should be normalized if they their
    sum is greater one.
*/
void n_setnormalize(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    self->SetNormalize(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnormalize

    @input
    v

    @output
    b (Normalize)

    @info
    Get the 'setnormalize' state.
*/
void n_getnormalize(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    cmd->Out()->SetB(self->GetNormalize());
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
    Explicitely enable/disable weighting vertex components.
    By default, all vertex components are disabled. Disabling
    components that don't need to be updated is more efficient.
*/
static void n_setupdatecoord(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    self->SetUpdateCoord(cmd->In()->GetB());
}
static void n_setupdatenorm(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    self->SetUpdateNorm(cmd->In()->GetB());
}
static void n_setupdatecolor(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    self->SetUpdateColor(cmd->In()->GetB());
}
static void n_setupdateuv0(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    self->SetUpdateUv0(cmd->In()->GetB());
}
static void n_setupdateuv1(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    self->SetUpdateUv1(cmd->In()->GetB());
}
static void n_setupdateuv2(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    self->SetUpdateUv2(cmd->In()->GetB());
}
static void n_setupdateuv3(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
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
    nMeshMixer *self = (nMeshMixer *) o;
    cmd->Out()->SetB(self->GetUpdateCoord());
}
static void n_getupdatenorm(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    cmd->Out()->SetB(self->GetUpdateNorm());
}
static void n_getupdatecolor(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    cmd->Out()->SetB(self->GetUpdateColor());
}
static void n_getupdateuv0(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    cmd->Out()->SetB(self->GetUpdateUv0());
}
static void n_getupdateuv1(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    cmd->Out()->SetB(self->GetUpdateUv1());
}
static void n_getupdateuv2(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    cmd->Out()->SetB(self->GetUpdateUv2());
}
static void n_getupdateuv3(void *o, nCmd *cmd)
{
    nMeshMixer *self = (nMeshMixer *) o;
    cmd->Out()->SetB(self->GetUpdateUv3());
}

//-------------------------------------------------------------------
//  SaveCmds()
//  27-Sep-00   floh    created
//  28-Sep-00   floh    setnormalize
//  18-Oct-00   floh    + setreadonly
//-------------------------------------------------------------------
bool nMeshMixer::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;

        //--- setreadonly ---
        cmd = fs->GetCmd(this,'SRDO');
        cmd->In()->SetB(this->GetReadOnly());
        fs->PutCmd(cmd);

        //--- beginsources,setsource,endsource ---
        int num = this->GetNumSources();
        if (num > 0) {
            cmd = fs->GetCmd(this,'BSRC');
            cmd->In()->SetI(num);
            fs->PutCmd(cmd);

            int i;
            for (i=0; i<num; i++) {
                const char *s0, *s1;
                this->GetSource(i, s0, s1);
                n_assert(s0);
				n_assert(s1);

                cmd = fs->GetCmd(this, 'SSRC');
                cmd->In()->SetI(i);
                cmd->In()->SetS(s0);
                cmd->In()->SetS(s1);
                fs->PutCmd(cmd);
            }

            cmd = fs->GetCmd(this,'ESRC');
            fs->PutCmd(cmd);
        }

        //--- setnormalize ---
        cmd = fs->GetCmd(this,'SNRM');
        cmd->In()->SetB(this->GetNormalize());
        fs->PutCmd(cmd);

        //--- setupdatecoord ---
        cmd = fs->GetCmd(this,'SUCD');
        cmd->In()->SetB(this->GetUpdateCoord());
        fs->PutCmd(cmd);

        //--- setupdatenorm ---
        cmd = fs->GetCmd(this,'SUNM');
        cmd->In()->SetB(this->GetUpdateNorm());
        fs->PutCmd(cmd);

        //--- setupdatecolor ---
        cmd = fs->GetCmd(this,'SUCL');
        cmd->In()->SetB(this->GetUpdateColor());
        fs->PutCmd(cmd);

        //--- setupdateuv0 ---
        cmd = fs->GetCmd(this,'SUV0');
        cmd->In()->SetB(this->GetUpdateUv0());
        fs->PutCmd(cmd);

        //--- setupdateuv1 ---
        cmd = fs->GetCmd(this,'SUV1');
        cmd->In()->SetB(this->GetUpdateUv1());
        fs->PutCmd(cmd);

        //--- setupdateuv2 ---
        cmd = fs->GetCmd(this,'SUV2');
        cmd->In()->SetB(this->GetUpdateUv2());
        fs->PutCmd(cmd);

        //--- setupdateuv3 ---
        cmd = fs->GetCmd(this,'SUV3');
        cmd->In()->SetB(this->GetUpdateUv3());
        fs->PutCmd(cmd);
        retval = true;
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
