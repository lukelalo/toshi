#define N_IMPLEMENTS nChnReader
//------------------------------------------------------------------------------
//  nchnreader_cmds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "node/nchnreader.h"
#include "kernel/npersistserver.h"

static void n_beginconnects(void*, nCmd*);
static void n_setconnect(void*, nCmd*);
static void n_endconnects(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nchnreader

    @superclass
    nvisnode

    @classinfo
    Read channel values and invoke command with channel values as args
    on parent object.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_beginconnects_i", 'BCNC', n_beginconnects);
    cl->AddCmd("v_setconnect_iss",  'SCNC', n_setconnect);
    cl->AddCmd("v_endconnects_v",    'ECNC', n_endconnects);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/** 
    @cmd
    beginconnects

    @input
    i (NumConnects)

    @output
    v

    @info
    Begin defining connection, expects the number of connections as
    argument.
*/
static void
n_beginconnects(void* slf, nCmd* cmd)
{
    nChnReader* self = (nChnReader*) slf;
    self->BeginConnects(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setconnect

    @input
    i (Index), s (CommandName), s (ChannelName)

    @output
    v

    @info
    Define a single connection, expects the index of the connection 
    (0..(NumConnects-1)), a command name and a channel name. 
*/
static void
n_setconnect(void* slf, nCmd* cmd)
{
    nChnReader* self = (nChnReader*) slf;
    int i0 = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SetConnect(i0, s0, s1);
}

//------------------------------------------------------------------------------
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
static void
n_endconnects(void* slf, nCmd* )
{
    nChnReader* self = (nChnReader*) slf;
    self->EndConnects();
}

//------------------------------------------------------------------------------
/**
*/
bool
nChnReader::SaveCmds(nPersistServer* fs)
{
    bool retval = false;
    
    if (nVisNode::SaveCmds(fs))
    {
        nCmd* cmd;

        //--- beginconnects/setconnect/endconnects ---
        if (this->numConnects > 0)
        {
            //--- beginconnects ---
            cmd = fs->GetCmd(this, 'BCNC');
            cmd->In()->SetI(this->numConnects);
            fs->PutCmd(cmd);
        
            //--- setconnect ---
            int i;
            for (i = 0; i < this->numConnects; i++)
            {
                const char* cmdName;
                const char* chnName;
                this->GetConnect(i, cmdName, chnName);
                
                cmd = fs->GetCmd(this, 'SCNC');
                cmd->In()->SetI(i);
                cmd->In()->SetS(cmdName);
                cmd->In()->SetS(chnName);
                fs->PutCmd(cmd);
            }

            //--- endconnects ---
            cmd = fs->GetCmd(this, 'ECNC');
            fs->PutCmd(cmd);
        }
    }
    return retval;
}
//------------------------------------------------------------------------------
