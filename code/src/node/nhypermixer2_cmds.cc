#define N_IMPLEMENTS nHyperMixer2
//-------------------------------------------------------------------
//  node/nmix_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/nhypermixer2.h"

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

//------------------------------------------------------------------------------
/**
    @scriptclass
    nhypermixer2

    @superclass
    nvisnode

    @classinfo
    Mix attributes of several source objects into a parent object.
    For each source object, the name of the source object (must
    be a child of nHyperMixer2 object), and a channel name defining
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
    cl->AddCmd("v_setconnect_issss",    'SCNC', n_setconnect);
    cl->AddCmd("v_endconnects_v",       'ECNC', n_endconnects);
    cl->AddCmd("i_getnumconnects_v",    'GNCC', n_getnumconnects);
    cl->AddCmd("ssss_getconnect_i",     'GCNC', n_getconnect);
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
    nHyperMixer2* self = (nHyperMixer2*) slf;
    self->BeginSources(cmd->In()->GetI());
}

//-------------------------------------------------------------------
/**
    @cmd
    setsource

    @input
    i(SourceIndex), s(RootObjectName), s(ChannelName)

    @output
    v

    @info
    Describe a mixer source, expects index of source, 
    relative path to a root object of a hierarchy, and
    a channel name, which will deliver the mixing weight.
*/
static void n_setsource(void* slf, nCmd* cmd)
{
    nHyperMixer2* self = (nHyperMixer2*) slf;
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
    nHyperMixer2* self = (nHyperMixer2*) slf;
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
    nHyperMixer2* self = (nHyperMixer2*) slf;
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
    nHyperMixer2* self = (nHyperMixer2*) slf;
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
    nHyperMixer2* self = (nHyperMixer2*) slf;
    self->BeginConnects(cmd->In()->GetI());
}

//-------------------------------------------------------------------
/**
    @cmd
    setconnect
   
    @input
    i(ConnectIndex), s (TargetObject), s(TargetCommand), s(SourceObject), s(SourceCommand)
   
    @output
    v
   
    @info
    Define a mixer connection. Each connection consists of a relative
    path to the target object, the command to invoke on the target
    object, the path of a source object relative to its "source root
    object", and the command to invoke on the source object.
*/
static void n_setconnect(void* slf, nCmd* cmd)
{
    nHyperMixer2* self = (nHyperMixer2*) slf;
    int i0         = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    const char* s2 = cmd->In()->GetS();
    const char* s3 = cmd->In()->GetS();
    self->SetConnect(i0, s0, s1, s2, s3);
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
static void n_endconnects(void* slf, nCmd*)
{
    nHyperMixer2* self = (nHyperMixer2*) slf;
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
    nHyperMixer2* self = (nHyperMixer2*) slf;
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
    Get the definition of a connection defined by its index.
*/
static void n_getconnect(void* slf, nCmd* cmd)
{
    nHyperMixer2* self = (nHyperMixer2*) slf;
    const char* s0;
    const char* s1;
    const char* s2;
    const char* s3;
    self->GetConnect(cmd->In()->GetI(), s0, s1, s2, s3);
    cmd->Out()->SetS(s0);
    cmd->Out()->SetS(s1);
    cmd->Out()->SetS(s2);
    cmd->Out()->SetS(s3);
}

//-------------------------------------------------------------------
//  SaveCmds()
//  02-Jan-99   floh    machine generated
//-------------------------------------------------------------------
bool nHyperMixer2::SaveCmds(nPersistServer *fs)
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
                const char* tarObj;
                const char* tarCmd;
                const char* srcObj;
                const char* srcCmd;
                this->GetConnect(i, tarObj, tarCmd, srcObj, srcCmd);

                cmd = fs->GetCmd(this, 'SCNC');
                cmd->In()->SetI(i);
                cmd->In()->SetS(tarObj);
                cmd->In()->SetS(tarCmd);
                cmd->In()->SetS(srcObj);
                cmd->In()->SetS(srcCmd);
                fs->PutCmd(cmd);
            }

            cmd = fs->GetCmd(this, 'ECNC');
            fs->PutCmd(cmd);
        }

        retval = true;
    }
    return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
