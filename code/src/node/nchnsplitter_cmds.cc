#define N_IMPLEMENTS nChnSplitter
//-------------------------------------------------------------------
//  node/nchnsplitter_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/nchnsplitter.h"

static void n_addkey(void *, nCmd *);
static void n_beginkeys(void *, nCmd *);
static void n_setkey(void *, nCmd *);
static void n_endkeys(void *, nCmd *);
static void n_getkeyinfo(void *, nCmd *);
static void n_getkey(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nchnsplitter

    @superclass
    nanimnode

    @classinfo
    Generates new channels splitting several "Maximum-Keyframes" of a
    source channel. Every keyframe corresponds to a channel reaching
    his maximum of 1.0 at the keyframe position and his minimum of 0.0
    at the adjacent keyframes.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_addkey_fs",'AKEY',n_addkey);
    cl->AddCmd("v_beginkeys_i",'BKEY',n_beginkeys);
    cl->AddCmd("v_setkey_ifs",'SKEY',n_setkey);
    cl->AddCmd("v_endkeys_v",'EKEY',n_endkeys);
    cl->AddCmd("i_getkeyinfo_v",'GKYI',n_getkeyinfo);
    cl->AddCmd("fs_getkey_i",'GKEY',n_getkey);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addkey

    @input
    f (TimeStamp), s (ChannelName)

    @output
    v

    @info
    Attaches a new keyframe. You define the position on the
    source channel and the name of the channel to be generated.
*/
static void n_addkey(void *o, nCmd *cmd)
{
    nChnSplitter *self = (nChnSplitter *) o;
    {
        float f = cmd->In()->GetF();
        const char *s = cmd->In()->GetS();
        self->AddKey(f,s);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginkeys

    @input
    i (NumKeys)

    @output
    v

    @info
    Starts a random access key definition. You must state the
    number of keys you want to define by 'setkey'. 
    The definition must be ended with the 'endkeys' statement.
*/
static void n_beginkeys(void *o, nCmd *cmd)
{
    nChnSplitter *self = (nChnSplitter *) o;
    self->BeginKeys(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setkey

    @input
    i (KeyIndex), f (TimeStamp), s (ChannelName)

    @output
    v

    @info
    Defines a key in random access mode (in between a
    'beginkeys' / 'endkeys').
*/
static void n_setkey(void *o, nCmd *cmd)
{
    nChnSplitter *self = (nChnSplitter *) o;
    {
        int i   = cmd->In()->GetI();
        float f = cmd->In()->GetF();
        const char *s = cmd->In()->GetS();
        self->SetKey(i,f,s);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    endkeys

    @input
    v

    @output
    v

    @info
    Ends a with 'beginkeys' started key definition.
*/
static void n_endkeys(void *o, nCmd *)
{
    nChnSplitter *self = (nChnSplitter *) o;
    self->EndKeys();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkeyinfo

    @input
    v

    @output
    i (NumKeys)

    @info
    Returns the number of keyframes.
*/
static void n_getkeyinfo(void *o, nCmd *cmd)
{
    nChnSplitter *self = (nChnSplitter *) o;
    int num_keys;
    self->GetKeyInfo(num_keys);
    cmd->Out()->SetI(num_keys);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkey

    @input
    i (KeyIndex)

    @output
    f (TimeStamp), s (ChannelName)

    @info
    Returns timestamp and channel name for the given keyframe index.
    If the index value is to high, an assertion occurs.
*/
static void n_getkey(void *o, nCmd *cmd)
{
    nChnSplitter *self = (nChnSplitter *) o;
    {
        int i;
        float f;
        const char *s;
        i = cmd->In()->GetI();
        self->GetKey(i,f,s);
        cmd->Out()->SetF(f);
        cmd->Out()->SetS(s);
    }
}

//-------------------------------------------------------------------
//  SaveCmds()
//  02-Jan-99   floh    machine generated
//-------------------------------------------------------------------
bool nChnSplitter::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nAnimNode::SaveCmds(fs)) {
        nCmd *cmd;
        if (this->num_keys > 0) {
            //--- beginkeys ---
            if ((cmd = fs->GetCmd(this,nID('B','K','E','Y')))) {
                cmd->In()->SetI(this->num_keys);
                fs->PutCmd(cmd);
            }

            //--- setkey ---
            int i;
            for (i=0; i<this->num_keys; i++) {
                if ((cmd = fs->GetCmd(this,nID('S','K','E','Y')))) {
                    nChnSplitterKey *k = &(this->keyarray[i]);
                    cmd->In()->SetI(i);
                    cmd->In()->SetF(k->t);
                    cmd->In()->SetS(k->chn_name);
                    fs->PutCmd(cmd);
                }
            }

            //--- endkeys ---
            if ((cmd = fs->GetCmd(this,nID('E','K','E','Y')))) {
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
