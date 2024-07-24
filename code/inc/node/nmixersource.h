#ifndef N_MIXERSOURCE_H
#define N_MIXERSOURCE_H
//------------------------------------------------------------------------------
/**
    @class nMixerSource
    @ingroup NebulaVisnodeModule

    @brief Inline class to store mixer sources. Used by nMeshMixer
    and nMixer class.

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

//------------------------------------------------------------------------------
class nMixerSource
{
    nDynAutoRef<nVisNode> ref_source;
    char channel_name[N_MAXNAMELEN];
    int channel_index;

public:
    nMixerSource() 
    {
        memset(channel_name,0,sizeof(channel_name));
        channel_index = -1;
    }
    void Initialize(nKernelServer *ks, nRoot *o) 
    {
        this->ref_source.initialize(ks,o);
    }

    void Clear(void) 
    {
        ref_source = (const char*) NULL;
        memset(channel_name,0,sizeof(channel_name));
        channel_index = -1;
    }
    
    void SetSourceName(const char *n) 
    {
        n_assert(n);
        ref_source = n;
    }
    
    const char *GetSourceName(void) 
    {
        return ref_source.getname();
    }
    
    nVisNode *GetSource(void) 
    {
        return ref_source.get();
    }
    
    void SetChannelName(const char *n) 
    {
        n_assert(n);
        n_strncpy2(channel_name,n,sizeof(channel_name));
    }
    
    const char *GetChannelName(void) 
    {
        if (channel_name[0]) return channel_name;
        else                 return NULL;
    }

    void SetChannelIndex(int i) 
    {
        channel_index = i;
    }

    int GetChannelIndex(void) 
    {
        return channel_index;
    }
};  

//------------------------------------------------------------------------------
#endif 
