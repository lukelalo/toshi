#ifndef N_MESHMIXER_H
#define N_MESHMIXER_H
//--------------------------------------------------------------------
/**
    @class nMeshMixer
    @ingroup NebulaVisnodeModule
    
    @brief Mixes any number of weighted source meshes into a target mesh
*/
//--------------------------------------------------------------------
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#ifndef N_MIXERSOURCE_H
#include "node/nmixersource.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nMeshMixer
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nMeshNode;
class N_DLLCLASS nMeshMixer : public nVisNode {

    enum {
        N_UPDATE_COORD  = (1<<0),
        N_UPDATE_NORM   = (1<<1),
        N_UPDATE_RGBA   = (1<<2),
        N_UPDATE_UV0    = (1<<3),
        N_UPDATE_UV1    = (1<<4),
        N_UPDATE_UV2    = (1<<5),
        N_UPDATE_UV3    = (1<<6),
        N_NORMALIZE     = (1<<7),
        N_MAXSOURCES    = 16,
    };

    bool in_begin;
    int mix_flags;
    int num_sources;
    nMixerSource sources[N_MAXSOURCES];
    nDynVertexBuffer dyn_vb;

public:
    static nKernelServer *kernelServer;

    nMeshMixer()
        : mix_flags(0),
          num_sources(0),
          in_begin(false),
          dyn_vb(kernelServer,this)
    {
        int i;
        for (i=0; i<N_MAXSOURCES; i++) 
		{
            this->sources[i].Initialize(kernelServer,this);
        }
    };
    virtual ~nMeshMixer();
    virtual bool SaveCmds(nPersistServer*);
    virtual void AttachChannels(nChannelSet*);
	virtual bool Attach(nSceneGraph2*);
    virtual void Compute(nSceneGraph2*);

    //---------------------------------------------------------------
    void SetReadOnly(bool b) {

        this->dyn_vb.SetReadOnly(b);
    };
    bool GetReadOnly(void) {
        return this->dyn_vb.GetReadOnly();
    };

    //---------------------------------------------------------------
    void BeginSources(int num) {
        n_assert((num>0) && (num<N_MAXSOURCES));
        this->num_sources = num;
        this->in_begin = true;
    };
    void SetSource(int i, const char *s, const char* chn) {
        n_assert((i>=0) && (i<this->num_sources));
        n_assert(s);
		n_assert(chn);
        n_assert(this->in_begin);
        this->sources[i].Clear();
        this->sources[i].SetSourceName(s);
		this->sources[i].SetChannelName(chn);
    };
    void EndSources(void) {
        n_assert(this->in_begin);
        this->in_begin = false;
        this->NotifyChannelSetDirty();
    };
    int GetNumSources(void) {
        return this->num_sources;
    };
    void GetSource(int i, const char *& n, const char *& chn) {
        n_assert((i>=0) && (i<this->num_sources));
        n   = this->sources[i].GetSourceName();
        chn = this->sources[i].GetChannelName();
    };

    //---------------------------------------------------------------
    void SetNormalize(bool b) {
        if (b) this->mix_flags |= N_NORMALIZE;
        else   this->mix_flags &= ~N_NORMALIZE;
    };
    bool GetNormalize(void) {
        return (this->mix_flags & N_NORMALIZE) ? true : false;
    };
    
    void SetUpdateCoord(bool b) {
        if (b) this->mix_flags |= N_UPDATE_COORD;
        else   this->mix_flags &= ~N_UPDATE_COORD;
    };
    bool GetUpdateCoord(void) {
        return (this->mix_flags & N_UPDATE_COORD) ? true : false;
    };

    void SetUpdateNorm(bool b) {
        if (b) this->mix_flags |= N_UPDATE_NORM;
        else   this->mix_flags &= ~N_UPDATE_NORM;
    };
    bool GetUpdateNorm(void) {
        return (this->mix_flags & N_UPDATE_NORM) ? true : false;
    };

    void SetUpdateColor(bool b) {
        if (b) this->mix_flags |= N_UPDATE_RGBA;
        else   this->mix_flags &= ~N_UPDATE_RGBA;
    };
    bool GetUpdateColor(void) {
        return (this->mix_flags & N_UPDATE_RGBA) ? true : false;
    };

    void SetUpdateUv0(bool b) {
        if (b) this->mix_flags |= N_UPDATE_UV0;
        else   this->mix_flags &= ~N_UPDATE_UV0;
    };
    bool GetUpdateUv0(void) {
        return (this->mix_flags & N_UPDATE_UV0) ? true : false;
    };

    void SetUpdateUv1(bool b) {
        if (b) this->mix_flags |= N_UPDATE_UV1;
        else   this->mix_flags &= ~N_UPDATE_UV1;
    };
    bool GetUpdateUv1(void) {
        return (this->mix_flags & N_UPDATE_UV1) ? true : false;
    };

    void SetUpdateUv2(bool b) {
        if (b) this->mix_flags |= N_UPDATE_UV2;
        else   this->mix_flags &= ~N_UPDATE_UV2;
    };
    bool GetUpdateUv2(void) {
        return (this->mix_flags & N_UPDATE_UV2) ? true : false;
    };

    void SetUpdateUv3(bool b) {
        if (b) this->mix_flags |= N_UPDATE_UV3;
        else   this->mix_flags &= ~N_UPDATE_UV3;
    };
    bool GetUpdateUv3(void) {
        return (this->mix_flags & N_UPDATE_UV3) ? true : false;
    };
    //----------------------------------------------------------------
private:
    void mix(nVertexBuffer **, float *, int, nSceneGraph2*);
};
//--------------------------------------------------------------------
#endif
