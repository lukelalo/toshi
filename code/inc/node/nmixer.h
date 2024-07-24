#ifndef N_MIXER_H
#define N_MIXER_H
//-------------------------------------------------------------------
/**
    @class nMixer
    @ingroup NebulaVisnodeModule

    @brief flexibler Mix-Interpolator

    Ein nMixer Object wird wie ein nIpol Objekt unter ein
    zu manipulierendes Objekt geklinkt. Anstatt aber feste
    Keyframes zu definieren, holt sich der nMixer die
    zu interpolierenden Daten aus seinen Subobjekten.
    Wie beim nIpol-Objekt "connected" man das nMixer
    Objekt durch Angabe eines Scriptkommandos an sein
    Target-Objekt und an seine Source-Objekte.
    Bei einer Connection wird jeweils der "Uplink" und
    der "Downlink" definiert. Ueber einen Downlink holt
    sich der Mixer die Sourcedaten, vermixt sie und
    schickt sie den Uplink hoch.
    Es koennen beliebig viele Connections definiert werden.
    
    Mit welcher Wichtung die Subobjekte eingerechnet
    werden, ist durch deren Wichtung bestimmt
    (einzustellen direkt am Subobjekt per getweight).
*/
//-------------------------------------------------------------------
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_MIXERSOURCE_H
#include "node/nmixersource.h"
#endif

//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nMixer
#include "kernel/ndefdllclass.h"

//-------------------------------------------------------------------
class nMixerConnect
{
public:
    /// da constructaaa
    nMixerConnect() :
        tarCmdName(0),
        srcCmdName(0),
        tarCmdProto(0),
        srcCmdProto(0),
        numArgs(0)
    {
        // empty
    }

    /// da destructaaa
    ~nMixerConnect()
    {
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

    /// set cmd names
    void SetCmdNames(const char* tarCmd, const char* srcCmd)
    {
        n_assert(0 == this->tarCmdName);
        n_assert(0 == this->srcCmdName);
        this->tarCmdName = n_strdup(tarCmd);
        this->srcCmdName = n_strdup(srcCmd);
    }
    /// get target command name
    const char* GetTarCmdName()
    {
        return this->tarCmdName;
    }
    /// get source command name
    const char* GetSrcCmdName()
    {
        return this->srcCmdName;
    }

    /// initialize cmd protos from command names and objects
    void InitCmdProtos(nVisNode* tarObj, nVisNode* srcObj)
    {
        n_assert(tarObj);
        n_assert(srcObj);
        n_assert(this->tarCmdName);
        n_assert(this->srcCmdName);

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
    /// get target cmd proto
    nCmdProto* GetTarCmdProto()
    {
        return this->tarCmdProto;
    }
    /// get source cmd proto
    nCmdProto* GetSrcCmdProto()
    {
        return this->srcCmdProto;
    }
    /// get number of args in command proto
    int GetNumArgs()
    {
        return this->numArgs;
    }

protected:
    const char* tarCmdName;
    const char* srcCmdName;
    nCmdProto *tarCmdProto;
    nCmdProto *srcCmdProto;
    int numArgs;
};

//-------------------------------------------------------------------
class nMixer : public nVisNode 
{
public:
    /// constructor
    nMixer();
    /// destructor
    virtual ~nMixer();
    /// initialize after linkage into name hierarchie
    virtual void Initialize(void);
    /// object persistency
    virtual bool SaveCmds(nPersistServer *);
    /// attach animation channels
    virtual void AttachChannels(nChannelSet*);
    /// perform mixing
    virtual void Compute(nSceneGraph2*);

    /// begin defining source objects
    void BeginSources(int num);
    /// define a source object and weight channel
    void SetSource(int index, const char* srcObj, const char* srcChannel);
    /// finish defining source objects
    void EndSources();
    /// get number of sources
    int GetNumSources();
    /// get source definition
    void GetSource(int index, const char*& srcObj, const char*& srcChannel);

    /// begin defining connections
    void BeginConnects(int num);
    /// define a connection
    void SetConnect(int index, const char* tarCmd, const char* srcCmd);
    /// finish defining connections
    void EndConnects();
    /// get number of connections
    int GetNumConnects();
    /// get connection definition
    void GetConnect(int index, const char*& tarCmd, const char*& srcCmd);

    /// turn on/off weight normalization
    void SetNormalize(bool);
    /// get current weight normalization state
    bool GetNormalize(void);

    static nKernelServer *kernelServer;

protected:
    enum
    {
        MAXSOURCES  = 16,
        MAXCONNECTS = 8,
        MAXARGS     = 4,
    };

    int numSources;
    nMixerSource sources[MAXSOURCES];

    int numConnects;
    nMixerConnect connects[MAXCONNECTS];

    bool normalize;
    bool connectsInitialized;
};
//-------------------------------------------------------------------
#endif
