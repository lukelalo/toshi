#define N_IMPLEMENTS nTerrainNode
//--------------------------------------------------------------------
//  nterrain_init.cc
//  (C) 2000 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "terrain/quadtree.h"
#include "terrain/nterrainnode.h"
#include "math/nmath.h"

nClass *nTerrainNode::local_cl  = NULL;
nKernelServer *nTerrainNode::ks = NULL;

extern char *nTerrainNode_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);
extern void n_initcmds(nClass *);

float *quadsquare::sin_lookup = NULL;
float *quadsquare::cos_lookup = NULL;

nProfiler *nTerrainNode::p_update = NULL;
nProfiler *nTerrainNode::p_render = NULL;

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nTerrainNode::local_cl = cl;
    nTerrainNode::ks       = ks;
    ks->AddClass("nvisnode" ,cl);
    n_initcmds(cl);
    
    // initialize the sincos lookup-table
    quadsquare::sin_lookup = new float[quadsquare::LOOKUP_ENTRIES+1];
    quadsquare::cos_lookup = new float[quadsquare::LOOKUP_ENTRIES+1];
    float a = 0.0f;
    float da = (2.0f*PI)/float(quadsquare::LOOKUP_ENTRIES);
    int i;
    for (i=0; i<=quadsquare::LOOKUP_ENTRIES; i++) {
        quadsquare::sin_lookup[i] = (float) sin(a);
        quadsquare::cos_lookup[i] = (float) cos(a);
        a += da;
    }

    // initialize profilers
    nTerrainNode::p_update = ks->ts->NewProfiler("terr_update");
    nTerrainNode::p_render = ks->ts->NewProfiler("terr_render");

    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    if (quadsquare::sin_lookup) {
        delete[] quadsquare::sin_lookup;
        quadsquare::sin_lookup = NULL;
    }
    if (quadsquare::cos_lookup) {
        delete[] quadsquare::cos_lookup;
        quadsquare::cos_lookup = NULL;
    }
    nTerrainNode::ks->RemClass(nTerrainNode::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nTerrainNode;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nTerrainNode_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
