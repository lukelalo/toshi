#define N_IMPLEMENTS nChnReader
//------------------------------------------------------------------------------
//  nchnreader_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelcontext.h"
#include "node/nchnreader.h"
#include "node/nchannelconnect.h"

//------------------------------------------------------------------------------
/**
*/
nChnReader::nChnReader() :
    numConnects(0),
    connects(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nChnReader::~nChnReader()
{
    if (this->connects)
    {
        delete[] this->connects;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nChnReader::Initialize()
{
    nVisNode *p = (nVisNode *) this->GetParent();
    if (p && p->IsA(kernelServer->FindClass("nvisnode"))) 
    {
        p->DependsOn(this);
    }
    nVisNode::Initialize();
}

//------------------------------------------------------------------------------
/**
*/
void
nChnReader::AttachChannels(nChannelSet* chnSet)
{
    int i;
    for (i = 0; i < this->numConnects; i++)
    {
        this->connects[i].InitChannelIndex(this->refChannel.get(), chnSet);
    }
    nVisNode::AttachChannels(chnSet);
}

//------------------------------------------------------------------------------
/**
*/
void
nChnReader::BeginConnects(int num)
{
    n_assert(num > 0);
    n_assert(0 == this->connects);
    this->numConnects = num;
    this->connects = new nChannelConnect[num];
}

//------------------------------------------------------------------------------
/**
*/
void
nChnReader::SetConnect(int index, const char* cmdName, const char* chnName)
{
    n_assert((index >= 0) && (index < this->numConnects));
    n_assert(cmdName && chnName);
    this->connects[index].SetNames(cmdName, chnName);
}

//------------------------------------------------------------------------------
/**
*/
void
nChnReader::EndConnects()
{
    // resolve cmd protos
    nVisNode* parent = (nVisNode*) this->GetParent();
    n_assert(parent && parent->IsA(kernelServer->FindClass("nvisnode")));
    
    int i;
    for (i = 0; i < this->numConnects; i++)
    {
        this->connects[i].InitCmdProto(parent);
    }
}

//------------------------------------------------------------------------------
/**
*/
int
nChnReader::GetNumConnects()
{
    return this->numConnects;
}

//------------------------------------------------------------------------------
/**
*/
void
nChnReader::GetConnect(int index, const char*& cmdName, const char*& chnName)
{
    n_assert((index >= 0) && (index < this->numConnects));
    cmdName = this->connects[index].GetCmdName();
    chnName = this->connects[index].GetChannelName();
}

//------------------------------------------------------------------------------
/**
*/
void
nChnReader::Compute(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);
    nVisNode::Compute(sceneGraph);

    // get channel context
    nChannelContext* chnContext = sceneGraph->GetChannelContext();
    n_assert(chnContext);

    // handle each connection
    int i;
    nVisNode* parent = (nVisNode*) this->GetParent();
    for (i = 0; i < this->numConnects; i++)
    {
        nChannelConnect *curConnect = &(this->connects[i]);
        int chnIndex = curConnect->GetChannelIndex();
        nCmdProto* cmdProto = curConnect->GetCmdProto();
        int numArgs = curConnect->GetNumArgs();

        // read current channel value
        float val[4];
        chnContext->GetChannel4f(chnIndex, val[0], val[1], val[2], val[3]);

        // build cmd and invoke on parent object
        nCmd* cmd = cmdProto->NewCmd();
        cmd->Rewind();
        int j;
        for (j = 0; j < numArgs; j++)
        {
            cmd->In()->SetF(val[j]);
        }
        parent->Dispatch(cmd);
        cmdProto->RelCmd(cmd);
    }
}
//------------------------------------------------------------------------------
