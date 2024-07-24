#ifndef N_CHANNELCONNECT_H
#define N_CHANNELCONNECT_H
//------------------------------------------------------------------------------
/**
    @class nChannelConnect
    @ingroup NebulaVisnodeModule

    @brief Helper class, associates a command name with a channel name.

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_CHANNELSET_H
#include "gfx/nchannelset.h"
#endif

//------------------------------------------------------------------------------
class nChannelServer;
class nChannelConnect
{
public:
    /// constructor
    nChannelConnect();
    /// destructor
    ~nChannelConnect();
    /// set names
    void SetNames(const char* cmd, const char* chn);
    /// get cmd name
    const char* GetCmdName();
    /// get channel name
    const char* GetChannelName();
    /// initialize command proto
    void InitCmdProto(nVisNode* obj);
    /// initialize the channel index
    void InitChannelIndex(nChannelServer* chnServer, nChannelSet* chnSet);
    /// get cmd proto
    nCmdProto* GetCmdProto();
    /// get channel index
    int GetChannelIndex();
    /// get number of args for command proto
    int GetNumArgs();

protected:
    char cmdName[N_MAXNAMELEN];
    char chnName[N_MAXNAMELEN];
    nCmdProto* cmdProto;
    int chnIndex;
    int numArgs;
};

//------------------------------------------------------------------------------
/**
*/
inline
nChannelConnect::nChannelConnect() :
    cmdProto(0),
    chnIndex(-1),
    numArgs(0)
{
    memset(this->cmdName, 0, sizeof(this->cmdName));
    memset(this->chnName, 0, sizeof(this->chnName));
}

//------------------------------------------------------------------------------
/**
*/
inline
nChannelConnect::~nChannelConnect()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param  cmdName     name of script command
    @param  chnName     name of channel
*/
inline
void
nChannelConnect::SetNames(const char* cmd, const char* chn)
{
    n_assert(cmd && chn);
    n_strncpy2(this->cmdName, cmd, sizeof(this->cmdName));
    n_strncpy2(this->chnName, chn, sizeof(this->chnName));
}

//------------------------------------------------------------------------------
/**
    @return     name of script command or 0
*/
inline
const char*
nChannelConnect::GetCmdName()
{
    n_assert(this->cmdName[0]);
    return this->cmdName;
}

//------------------------------------------------------------------------------
/**
    @return     name of channel or 0
*/
inline
const char*
nChannelConnect::GetChannelName()
{
    n_assert(this->chnName[0]);
    return this->chnName;
}

//------------------------------------------------------------------------------
/**
    Resolves the script command name into a nCmdProto object and stores
    pointer to it.

    @param  obj     pointer to object which understands the script command
*/
inline
void
nChannelConnect::InitCmdProto(nVisNode* obj)
{
    n_assert(obj);
    n_assert(cmdName[0] != 0);

    // get cmd proto
    this->cmdProto = obj->GetClass()->FindCmdByName(this->cmdName);
    if (!this->cmdProto)
    {
        char buf[N_MAXPATH];
        n_error("nChannelConnect: Object '%s' doesn't support command '%s'!\n",
            obj->GetFullName(buf, sizeof(buf)), this->cmdName);
    }

    // get number of args
    this->numArgs = this->cmdProto->GetNumInArgs();
}

//------------------------------------------------------------------------------
/**
    Initialize the channel index, normally called from inside 
    nVisNode::AttachChannels(). Must be called AFTER InitCmdProto().

    @param  chnSet      pointer to channel set object
*/
inline
void
nChannelConnect::InitChannelIndex(nChannelServer* chnServer, nChannelSet* chnSet)
{
    n_assert(chnServer && chnSet);
    n_assert(this->cmdProto);
    n_assert(this->numArgs > 0);
    this->chnIndex = chnSet->AddChannel(chnServer, this->chnName);
}

//------------------------------------------------------------------------------
/**
    Get pointer to cmd proto object.

    @return     pointer to nCmdProto object (0 if not yet initialized)
*/
inline
nCmdProto*
nChannelConnect::GetCmdProto()
{
    return this->cmdProto;
}

//------------------------------------------------------------------------------
/**
    Get channel index (-1 if not yet initialized).

    @return     the channel index
*/
inline
int
nChannelConnect::GetChannelIndex()
{
    return this->chnIndex;
}

//------------------------------------------------------------------------------
/**
    @return     number of args for cmd proto
*/
inline
int
nChannelConnect::GetNumArgs()
{
    return this->numArgs;
}

//------------------------------------------------------------------------------
#endif
