#ifndef N_HYPERMIXER2_H
#define N_HYPERMIXER2_H
//------------------------------------------------------------------------------
/**
    @class nHyperMixer2
    @ingroup NebulaVisnodeModule

    Reimplementation of nHyperMixer, making use of the new channel
    system. The original hyper mixer was done by Jeremy, this just gives it
    an interface more conformant with the other Nebula mixer classes.

    Mix attributes of several source objects into a parent object.
    For each source object, the name of the source object (must
    be a child of nHyperMixer2 object), and a channel name defining
    the mix weight must be provided. Then connections are defined
    by giving a source command and a target command. The source
    command provides 1..4 float values in its out args and the
    target command takes 1..4 float values as its in arg (must be
    compatible with source command).

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_MIXERSOURCE_H
#include "node/nmixersource.h"
#endif

#ifndef N_HYPERMIXERCONNECT_H
#include "node/nhypermixerconnect.h"
#endif

#undef N_DEFINES
#define N_DEFINES nHyperMixer2
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nHyperMixer2 : public nVisNode
{
public:
    /// constructor
    nHyperMixer2();
    /// destructor
    virtual ~nHyperMixer2();
    /// object persistency
    virtual bool SaveCmds(nPersistServer* fileServer);
    /// post namespace linkage init
    virtual void Initialize();
    /// attach animation channels
    virtual void AttachChannels(nChannelSet* chnSet);    
    /// update internal state
    virtual void Compute(nSceneGraph2* sceneGraph);

    /// begin defining mixer sources (hierarchies of nVisNode)
    void BeginSources(int num);
    /// define a mixer source
    void SetSource(int index, const char* srcRoot, const char* srcChannel);
    /// finish defining mixer sources
    void EndSources();
    /// get number of sources
    int GetNumSources();
    /// get source definition
    void GetSource(int index, const char*& srcRoot, const char*& srcChannel);

    /// begin defining connections
    void BeginConnects(int num);
    /// define a connection
    void SetConnect(int index, const char* tarObjPath, const char* tarObjCmd, const char* srcObjPath, const char* srcObjCmd);
    /// finish defining connections
    void EndConnects();
    /// get number of connections
    int GetNumConnects();
    /// get connection definition
    void GetConnect(int index, const char*& tarObjPath, const char*& tarObjCmd, const char*& srcObjPath, const char*& srcObjCmd);

    static nClass *clazz;
    static nKernelServer *ks;

protected:
    /// validate object pointers
    void LookupObjPointers();

    enum
    {
        MAXSOURCES  = 16,
        MAXCONNECTS = 64,
        MAXARGS     = 4,
    };

    int numSources;
    nMixerSource sources[MAXSOURCES];

    int numConnects;
    nHyperMixerConnect connects[MAXCONNECTS];

    nVisNode* tarObjPtrs[MAXCONNECTS];
    nVisNode* srcObjPtrs[MAXCONNECTS][MAXSOURCES];

    bool objPtrsDirty;
    bool connectsInitialized;
};

//------------------------------------------------------------------------------
#endif

