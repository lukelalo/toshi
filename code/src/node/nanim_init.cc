#define N_IMPLEMENTS nAnimNode
//-------------------------------------------------------------------
//  nanimnode.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "gfx/nchannelset.h"
#include "node/nanimnode.h"

nClass *nAnimNode::local_cl = NULL;
nKernelServer *nAnimNode::ks = NULL;

extern void n_initcmds(nClass *);
extern char *nAnimNode_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nAnimNode::local_cl = cl;
    nAnimNode::ks       = ks;
    ks->AddClass("nvisnode", cl);
    n_initcmds(cl);
    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nAnimNode::ks->RemClass(nAnimNode::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nAnimNode;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nAnimNode_version;
}

//--------------------------------------------------------------------
//  nAnimNode()
//  25-Jan-99   floh    created
//--------------------------------------------------------------------
nAnimNode::nAnimNode() : 
    repType(N_REPTYPE_LOOP),
    localChannelIndex(0),
    scale(1.0f)
{
    strcpy(this->channelName, "time");
}

//--------------------------------------------------------------------
//  ~nAnimNode()
//  25-Jan-99   floh    created
//--------------------------------------------------------------------
nAnimNode::~nAnimNode()
{ }

//--------------------------------------------------------------------
//  AttachChannels()
//  05-Jun-01   floh    created
//--------------------------------------------------------------------
void nAnimNode::AttachChannels(nChannelSet* chnSet)
{
    n_assert(chnSet);
    this->localChannelIndex = chnSet->AddChannel(this->refChannel.get(), this->channelName);
    nVisNode::AttachChannels(chnSet);
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

    
