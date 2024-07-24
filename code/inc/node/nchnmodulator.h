#ifndef N_CHNMODULATOR_H
#define N_CHNMODULATOR_H
//--------------------------------------------------------------------
/**
    @class nChnModulator
    @ingroup NebulaVisnodeModule
    
    @brief Modulaltes channels based on values from other channels.

    One can define a set of channels, which are modulated by other
    channels: 
    
      chn0 = chn0 * chn1
*/
//--------------------------------------------------------------------
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif
//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nChnModulator
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nChnModDef {
public:
    char chn0_name[N_MAXNAMELEN];
    char chn1_name[N_MAXNAMELEN];
    int chn0_num;
    int chn1_num;
    float val0;
    float val1;
    nChnModDef() {
        memset(chn0_name,0,sizeof(chn0_name));
        memset(chn1_name,0,sizeof(chn1_name));
        chn0_num = -1;
        chn1_num = -1;
        val0 = 0.0;
        val1 = 0.0;
    };
    void Set(const char *c0, const char *c1) {
        n_strncpy2(chn0_name,c0,sizeof(chn0_name));
        n_strncpy2(chn1_name,c1,sizeof(chn1_name));
    };
};

//--------------------------------------------------------------------
class N_DLLCLASS nChnModulator : public nVisNode {
    int num_defs;
    nChnModDef *defarray;

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nChnModulator();
    virtual ~nChnModulator();

    virtual bool SaveCmds(nPersistServer *);
    virtual bool Attach(nSceneGraph2*);
    virtual void Compute(nSceneGraph2 *);
    virtual void AttachChannels(nChannelSet*);

    virtual void Begin(int);
    virtual void Set(int, const char *, const char *);
    virtual void End(void);
    virtual int  GetNum(void);
    virtual void Get(int, const char *&, const char *&);
};
//--------------------------------------------------------------------
#endif
