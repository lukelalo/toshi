#ifndef N_CHNSPLITTER_H
#define N_CHNSPLITTER_H
//-------------------------------------------------------------------
/**
    @class nChnSplitter
    @ingroup NebulaVisnodeModule
    
    @brief Simple channel splitter.

    Split an input channel into multiple new channels.  Every keyframe
    corresponds to a channel reaching his maximum of 1.0 at the keyframe
    position and his minimum of 0.0 at the adjacent keyframes.
*/
//-------------------------------------------------------------------
#ifndef N_ANIMNODE_H
#include "node/nanimnode.h"
#endif
//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nChnSplitter
#include "kernel/ndefdllclass.h"
//-------------------------------------------------------------------
class nChnSplitterKey {
public:
    float t;
    char chn_name[N_MAXNAMELEN];
    int chn_num;
    float chn_val;

    void Set(float _t, const char *_chn_name) {
        t = _t;
        n_strncpy2(chn_name,_chn_name,sizeof(chn_name));
        chn_num = -1;
        chn_val = 0.0f;
    };
};
//-------------------------------------------------------------------
class N_DLLCLASS nChnSplitter : public nAnimNode {
protected:
    int num_keys;
    int keyarray_size;
    nChnSplitterKey *keyarray;

public:
    static nClass *local_cl;
    static nKernelServer *ks;
    nChnSplitter();
    virtual ~nChnSplitter();

    virtual bool SaveCmds(nPersistServer *);
    virtual bool Attach(nSceneGraph2*);
    virtual void Compute(nSceneGraph2*);
    virtual void AttachChannels(nChannelSet*);

    virtual void AddKey(float, const char *);
    virtual void BeginKeys(int);
    virtual void SetKey(int, float, const char *);
    virtual void EndKeys(void);
    virtual void GetKeyInfo(int&);
    virtual void GetKey(int, float&, const char *&);
private:
    nChnSplitterKey *getKey(void);
};
//-------------------------------------------------------------------
#endif
