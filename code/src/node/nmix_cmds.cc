#define N_IMPLEMENTS nMixer
//-------------------------------------------------------------------
//  node/nmix_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/nmixer.h"

static void n_beginsources(void*, nCmd*);
static void n_setsource(void*, nCmd*);
static void n_endsources(void*, nCmd*);
static void n_getnumsources(void*, nCmd*);
static void n_getsource(void*, nCmd*);
static void n_beginconnects(void*, nCmd*);
static void n_setconnect(void*, nCmd*);
static void n_endconnects(void*, nCmd*);
static void n_getnumconnects(void*, nCmd*);
static void n_getconnect(void*, nCmd*);
static void n_setnormalize(void *, nCmd *);
static void n_getnormalize(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmixer

    @superclass
    nvisnode

    @classinfo
    Mix attributes of several source objects into parent
    object. For each source object, name of the source object
    (must be child of nmixer object), and a channel name defining
    the mix weight must be provided. Then connections are defined
    by giving a source command and a target command. The source
    command provides 1..4 float values in its out args and the
    target command takes 1..4 float values as its in arg (must be
    compatible with source command).
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_beginsources_i",      'BSRC', n_beginsources);
    cl->AddCmd("v_setsource_iss",       'SSRC', n_setsource);
    cl->AddCmd("v_endsources_v",        'ESRC', n_endsources);
    cl->AddCmd("i_getnumsources_v",     'GNSC', n_getnumsources);
    cl->AddCmd("v_getsource_iss",       'GSRC', n_getsource);
    cl->AddCmd("v_beginconnects_i",     'BCNC', n_beginconnects);
    cl->AddCmd("v_setconnect_iss",      'SCNC', n_setconnect);
    cl->AddCmd("v_endconnects_v",       'ECNC', n_endconnects);
    cl->AddCmd("i_getnumconnects_v",    'GNCC', n_getnumconnects);
    cl->AddCmd("ss_getconnect_i",       'GCNC', n_getconnect);
    cl->AddCmd("v_setnormalize_b",      'SNRM', n_setnormalize);
    cl->AddCmd("b_getnormalize_v",      'GNRM', n_getnormalize);
    cl->EndCmds();
}

//-------------------------------------------------------------------
/**
    @cmd
    beginsources
   
    @input
    i(NumSources)
   
    @output
    v
   
    @info
    Begin defining mixer sources, expects number of sources.
*/
static void n_beginsources(void* slf, nCmd* cmd)
{
    nMixer* self = (nMixer*) slf;
    self->BeginSources(cmd->In()->GetI());
}

//-------------------------------------------------------------------
/**
    @cmd
    setsource
  
    @input
    i(SourceIndex), s(ObjectName), s(ChannelName)
  
    @output
    v
  
    @info
    Describe a mixer source by name of object (must be a direct
    child object of the nmixer object) and a channel name
    which provides the weight.
*/
static void n_setsource(void* slf, nCmd* cmd)
{
    nMixer* self = (nMixer*) slf;
    int i0         = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SetSource(i0, s0, s1);
}

//-------------------------------------------------------------------
/**
    @cmd
    endsources
  
    @input
    v
  
    @output
    v
  
    @info
    Finish defining sources.
*/
static void n_endsources(void* slf, nCmd* )
{
    nMixer* self = (nMixer*) slf;
    self->EndSources();
}

//-------------------------------------------------------------------
/**
    @cmd
    getnumsources
   
    @input
    v
   
    @output
    i(NumSources)
   
    @info
    Returns the number of mixer sources.
*/
static void n_getnumsources(void* slf, nCmd* cmd)
{
    nMixer* self = (nMixer*) slf;
    cmd->Out()->SetI(self->GetNumSources());
}

//-------------------------------------------------------------------
/**
    @cmd
    getsource
   
    @input
    i (SourceIndex)
   
    @output
    s(ObjectName), s(ChannelName)
   
    @info
    Get definition of source defined by its index.
*/
static void n_getsource(void* slf, nCmd* cmd)
{
    nMixer* self = (nMixer*) slf;
    const char* s0;
    const char* s1;
    self->GetSource(cmd->In()->GetI(), s0, s1);
    cmd->Out()->SetS(s0);
    cmd->Out()->SetS(s1);
}

//-------------------------------------------------------------------
/**
    @cmd
    beginconnects
   
    @input
    i(NumConnects)
   
    @output
    v
   
    @info
    Begin defining mixer connections, expects number of connections.
*/
static void n_beginconnects(void* slf, nCmd* cmd)
{
    nMixer* self = (nMixer*) slf;
    self->BeginConnects(cmd->In()->GetI());
}

//-------------------------------------------------------------------
/**
    @cmd
    setconnect
   
    @input
    i(ConnectIndex), s(TargetCommand), s(SourceCommand)
   
    @output
    v
   
    @info
    Define a mixer connection, which is simply the script command
    to invoke on the target object (always the parent of the nmixer
    object) and the command to invoke on the source object (always 
    a child of the nmixer object).
*/
static void n_setconnect(void* slf, nCmd* cmd)
{
    nMixer* self = (nMixer*) slf;
    int i0         = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SetConnect(i0, s0, s1);
}

//-------------------------------------------------------------------
/**
    @cmd
    endconnects
   
    @input
    v
   
    @output
    v
   
    @info
    Finish defining connections.
*/
static void n_endconnects(void* slf, nCmd* )
{
    nMixer* self = (nMixer*) slf;
    self->EndConnects();
}

//-------------------------------------------------------------------
/**
    @cmd
    getnumconnects
   
    @input
    v
   
    @output
    i(NumSources)
   
    @info
    Returns the number of mixer connections.
*/
static void n_getnumconnects(void* slf, nCmd* cmd)
{
    nMixer* self = (nMixer*) slf;
    cmd->Out()->SetI(self->GetNumConnects());
}

//-------------------------------------------------------------------
/**
    @cmd
    getconnect
   
    @input
    i (ConnectIndex)
   
    @output
    s(TargetCommand), s(SourceCommand)
   
    @info
    Get definition of a connection defined by its index.
*/
static void n_getconnect(void* slf, nCmd* cmd)
{
    nMixer* self = (nMixer*) slf;
    const char* s0;
    const char* s1;
    self->GetConnect(cmd->In()->GetI(), s0, s1);
    cmd->Out()->SetS(s0);
    cmd->Out()->SetS(s1);
}

//-------------------------------------------------------------------
/**
    @cmd
    setnormalize
   
    @input
    b(NormalizeFlag)
   
    @output
    v
   
    @info
    Turn on/off weight normalization.
*/
static void n_setnormalize(void *o, nCmd *cmd)
{
    nMixer *self = (nMixer *) o;
    self->SetNormalize(cmd->In()->GetB());
}

//-------------------------------------------------------------------
/**
    @cmd
    getnormalize
   
    @input
    v
   
    @output
    b(NormalizeFlag)
   
    @info
    Get current weight normalization flag.
*/
static void n_getnormalize(void *o, nCmd *cmd)
{
    nMixer *self = (nMixer *) o;
    cmd->Out()->SetB(self->GetNormalize());
}

//-------------------------------------------------------------------
//  SaveCmds()
//  02-Jan-99   floh    machine generated
//-------------------------------------------------------------------
bool nMixer::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;
        int i;

        //--- beginsources, setsource, endsources ---
        if (this->numSources > 0)
        {
            cmd = fs->GetCmd(this, 'BSRC');
            cmd->In()->SetI(this->numSources);
            fs->PutCmd(cmd);

            for (i = 0; i < this->numSources; i++)
            {
                const char* srcObj;
                const char* srcChn;
                this->GetSource(i, srcObj, srcChn);

                cmd = fs->GetCmd(this, 'SSRC');
                cmd->In()->SetI(i);
                cmd->In()->SetS(srcObj);
                cmd->In()->SetS(srcChn);
                fs->PutCmd(cmd);
            }

            cmd = fs->GetCmd(this, 'ESRC');
            fs->PutCmd(cmd);
        }

        //--- beginconnects, setconnect, endconnect ---
        if (this->numConnects > 0)
        {
            cmd = fs->GetCmd(this, 'BCNC');
            cmd->In()->SetI(this->numConnects);
            fs->PutCmd(cmd);

            for (i = 0; i < this->numConnects; i++)
            {
                const char* tarCmd;
                const char* srcCmd;
                this->GetConnect(i, tarCmd, srcCmd);

                cmd = fs->GetCmd(this, 'SCNC');
                cmd->In()->SetI(i);
                cmd->In()->SetS(tarCmd);
                cmd->In()->SetS(srcCmd);
                fs->PutCmd(cmd);
            }

            cmd = fs->GetCmd(this, 'ECNC');
            fs->PutCmd(cmd);
        }

        //--- setnormalize ---
        cmd = fs->GetCmd(this, 'SNRM');
        cmd->In()->SetB(this->GetNormalize());
        fs->PutCmd(cmd);

        retval = true;
    }
    return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
