#ifndef N_CHNREADER_H
#define N_CHNREADER_H
//------------------------------------------------------------------------------
/**
    @class nChnReader
    @ingroup NebulaVisnodeModule

    @brief Read 1..4 dimensional animation channels and directly invoke
    commands with channel values on parent object.

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nChnReader
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nChannelConnect;
class nChnReader : public nVisNode
{
public:
    /// constructor.
    nChnReader();
    /// destructor.
    virtual ~nChnReader();

    /// one time init after linking to object hierarchie
    virtual void Initialize();
    /// update internal state
    virtual void Compute(nSceneGraph2*);
    /// object persistency
    virtual bool SaveCmds(nPersistServer* fileServer);
    /// update channel set
    virtual void AttachChannels(nChannelSet* chnSet);

    /// begin defining connections
    void BeginConnects(int num);
    /// define a connection
    void SetConnect(int index, const char* cmdName, const char* chnName);
    /// finish defining connection
    void EndConnects();
    /// get number of connections
    int GetNumConnects();
    /// get connect definition
    void GetConnect(int index, const char*& cmdName, const char*& chnName);

    static nClass* clazz;
    static nKernelServer* kernelServer;

protected:
    int numConnects;
    nChannelConnect* connects;
};
//------------------------------------------------------------------------------
#endif
