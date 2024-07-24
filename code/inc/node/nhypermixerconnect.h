#ifndef N_HYPERMIXERCONNECT_H
#define N_HYPERMIXERCONNECT_H
//------------------------------------------------------------------------------
/**
    @class nHyperMixerConnect
    @ingroup NebulaVisnodeModule

    @brief A hyper mixer connection as used by the nHyperMixer2 class.

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

//------------------------------------------------------------------------------
class nHyperMixerConnect
{
public:
    /// constructor
    nHyperMixerConnect();
    /// destructor
    ~nHyperMixerConnect();
    /// define the connection
    void Set(const char* tarObjPath, const char* tarCmd, const char* srcObjPath, const char* srcCmd);
    /// get path to target object
    const char* GetTarObjPath();
    /// get path to source object
    const char* GetSrcObjPath();
    /// get target command name
    const char* GetTarCmd();
    /// get source command name
    const char* GetSrcCmd();
    /// get cmd proto of target command
    nCmdProto* GetTarCmdProto();
    /// get cmd proto of source command
    nCmdProto* GetSrcCmdProto();
    /// get number of args (input args of target command == output args of source command)
    int GetNumArgs();
    /// initialize the command prototype objects
    void InitCmdProtos(nVisNode* tarObj, nVisNode* srcObj);

protected:
    const char* srcObjPath;
    const char* tarObjPath;
    const char* tarCmdName;
    const char* srcCmdName;
    nCmdProto *tarCmdProto;
    nCmdProto *srcCmdProto;
    int numArgs;
};

//------------------------------------------------------------------------------
/**
*/
inline
nHyperMixerConnect::nHyperMixerConnect() :
    tarObjPath(0),
    srcObjPath(0),
    tarCmdName(0),
    srcCmdName(0),
    tarCmdProto(0),
    srcCmdProto(0),
    numArgs(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nHyperMixerConnect::~nHyperMixerConnect()
{
    if (this->tarObjPath)
    {
        n_free((void*) this->tarObjPath);
        this->tarObjPath = 0;
    }
    if (this->srcObjPath)
    {
        n_free((void*) this->srcObjPath);
        this->srcObjPath = 0;
    }
    if (this->tarCmdName)
    {
        n_free((void*) this->tarCmdName);
        this->tarCmdName = 0;
    }
    if (this->srcCmdName)
    {
        n_free((void*) this->srcCmdName);
        this->srcCmdName = 0;
    }
}

//------------------------------------------------------------------------------
/**
    @param  tarObjPath      path to target object
    @param  tarCmd          name of target command
    @param  srcObjPath      path to source object
    @param  srcCmd          name of source command
*/
inline
void
nHyperMixerConnect::Set(
    const char* tarObj, 
    const char* tarCmd, 
    const char* srcObj, 
    const char* srcCmd)
{
    n_assert(tarObj);
    n_assert(tarCmd);
    n_assert(srcObj);
    n_assert(srcCmd);
    if (this->tarObjPath)
    {
        n_free((void*) this->tarObjPath);
        this->tarObjPath = 0;
    }
    if (this->srcObjPath)
    {
        n_free((void*) this->srcObjPath);
        this->srcObjPath = 0;
    }
    if (this->tarCmdName)
    {
        n_free((void*) this->tarCmdName);
        this->tarCmdName = 0;
    }
    if (this->srcCmdName)
    {
        n_free((void*) this->srcCmdName);
        this->srcCmdName = 0;
    }
    this->tarObjPath = n_strdup(tarObj);
    this->srcObjPath = n_strdup(srcObj);
    this->tarCmdName = n_strdup(tarCmd);
    this->srcCmdName = n_strdup(srcCmd);
}

//------------------------------------------------------------------------------
/**
    @return     path to target object
*/
inline
const char*
nHyperMixerConnect::GetTarObjPath()
{
    return this->tarObjPath;
}

//------------------------------------------------------------------------------
/**
    @return     path to source object
*/
inline
const char*
nHyperMixerConnect::GetSrcObjPath()
{
    return this->srcObjPath;
}

//------------------------------------------------------------------------------
/**
    @return     target command string
*/
inline
const char*
nHyperMixerConnect::GetTarCmd()
{
    return this->tarCmdName;
}

//------------------------------------------------------------------------------
/**
    @return     source command string
*/
inline
const char*
nHyperMixerConnect::GetSrcCmd()
{
    return this->srcCmdName;
}

//------------------------------------------------------------------------------
/**
    @return     pointer to nCmdProto object of target command
*/
inline
nCmdProto*
nHyperMixerConnect::GetTarCmdProto()
{
    n_assert(this->tarCmdProto);
    return this->tarCmdProto;
}

//------------------------------------------------------------------------------
/**
    @return     pointer to nCmdProto object of source command
*/
inline
nCmdProto*
nHyperMixerConnect::GetSrcCmdProto()
{
    n_assert(this->srcCmdProto);
    return this->srcCmdProto;
}

//------------------------------------------------------------------------------
/**
    @return     number of arguments of target and source commands
*/
inline
int
nHyperMixerConnect::GetNumArgs()
{
    return this->numArgs;
}

//------------------------------------------------------------------------------
/**
    Initialize tarCmdProto, srcCmdProto and numArgs. Call once after
    Set() when the referenced objects should exist.
*/
inline
void
nHyperMixerConnect::InitCmdProtos(nVisNode* tarObj, nVisNode* srcObj)
{
    n_assert(tarObj);
    n_assert(srcObj);

    // get command protos 
    this->tarCmdProto = tarObj->GetClass()->FindCmdByName(this->tarCmdName);
    if (!this->tarCmdProto)
    {
        char buf[N_MAXPATH];
        n_error("nMixer: Target object '%s' doesn't support command '%s'!\n",
            tarObj->GetFullName(buf, sizeof(buf)), this->tarCmdName);
    }
    this->srcCmdProto = srcObj->GetClass()->FindCmdByName(this->srcCmdName);
    if (!this->srcCmdProto)
    {
        char buf[N_MAXPATH];
        n_error("nMixer: Source object '%s' doesn't support command '%s'!\n",
            srcObj->GetFullName(buf, sizeof(buf)),this->srcCmdName);
    }

    // fill num args and make sure the cmd formats match
    this->numArgs = this->tarCmdProto->GetNumInArgs();
    n_assert(this->srcCmdProto->GetNumOutArgs() == this->numArgs);
}

//------------------------------------------------------------------------------
#endif
