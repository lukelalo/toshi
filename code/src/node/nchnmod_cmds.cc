#define N_IMPLEMENTS nChnModulator
//-------------------------------------------------------------------
//  node/nchnmod_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/nchnmodulator.h"

static void n_begin(void *, nCmd *);
static void n_set(void *, nCmd *);
static void n_end(void *, nCmd *);
static void n_getnum(void *, nCmd *);
static void n_get(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nchnmodulator

    @superclass
    nvisnode

    @classinfo
    Modulates a set of channels with the help of source channels.
    Technicaly seen the first channel gets multiplied with the
    second channel, the result is written back to the first channel.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_begin_i",'BGIN',n_begin);
    cl->AddCmd("v_set_iss",'SET_',n_set);
    cl->AddCmd("v_end_v",'END_',n_end);
    cl->AddCmd("i_getnum_v",'GNUM',n_getnum);
    cl->AddCmd("ss_get_i",'GET_',n_get);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    begin

    @input
    i (NumDefs)

    @output
    v

    @info
    Starts a set of channel modulator definitions. You
    pass the number of definitions (reads number of following
    'set' statements)
*/
static void n_begin(void *o, nCmd *cmd)
{
    nChnModulator *self = (nChnModulator *) o;
    self->Begin(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    set

    @input
    i (Index), s (SrcChannelName), s (ModChannelName)  

    @output
    v

    @info
    Describes a channel modulator operation. The first string
    argument describes the channel to be modulated, the second
    argument the channel providing the multiplication factor.
*/
static void n_set(void *o, nCmd *cmd)
{
    nChnModulator *self = (nChnModulator *) o;
    {
        int i = cmd->In()->GetI();
        const char *s0 = cmd->In()->GetS();
        const char *s1 = cmd->In()->GetS();
        self->Set(i,s0,s1);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    end

    @input
    v

    @output
    v

    @info
    Ends a 'begin'-'set' definition.
*/
static void n_end(void *o, nCmd *)
{
    nChnModulator *self = (nChnModulator *) o;
    self->End();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnum

    @input
    v

    @output
    i (NumIndices)

    @info
    Returns the number of modulation definitions in an object.
*/
static void n_getnum(void *o, nCmd *cmd)
{
    nChnModulator *self = (nChnModulator *) o;
    cmd->Out()->SetI(self->GetNum());
}

//------------------------------------------------------------------------------
/**
    @cmd
    get

    @input
    i (Index)

    @output
    s (SrcChannelName), s (ModChannelName)

    @info
    Returns the settings of definition number 'Index'.
*/
static void n_get(void *o, nCmd *cmd)
{
    nChnModulator *self = (nChnModulator *) o;
    {
        int i = cmd->In()->GetI();
        const char *s0;
        const char *s1;
        self->Get(i,s0,s1);
        cmd->Out()->SetS(s0);
        cmd->Out()->SetS(s1);
    }
}

//-------------------------------------------------------------------
//  SaveCmds()
//  02-Jan-99   floh    machine generated
//-------------------------------------------------------------------
bool nChnModulator::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;
        if (this->num_defs > 0) {
            //--- begin ---
            if ((cmd = fs->GetCmd(this,nID('B','G','I','N')))) {
                cmd->In()->SetI(this->num_defs);
                fs->PutCmd(cmd);
            }
            //--- set ---
            int i;
            for (i=0; i<num_defs; i++) {
                if ((cmd = fs->GetCmd(this,nID('S','E','T','_')))) {
                    nChnModDef *d = &(this->defarray[i]);
                    cmd->In()->SetI(i);
                    cmd->In()->SetS(d->chn0_name);
                    cmd->In()->SetS(d->chn1_name);
                    fs->PutCmd(cmd);
                }
            }
            //--- end ---
            if ((cmd = fs->GetCmd(this,nID('E','N','D','_')))) {
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
