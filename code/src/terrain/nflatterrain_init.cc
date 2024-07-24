#define N_IMPLEMENTS nFlatTerrainNode
//--------------------------------------------------------------------
//  nflatterrain_init.cc
//  (C) 2000 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "terrain/flat_quadtree.h"
#include "terrain/nflatterrainnode.h"
#include "math/nmath.h"

nClass *nFlatTerrainNode::local_cl  = NULL;
nKernelServer *nFlatTerrainNode::ks = NULL;

extern char *nFlatTerrainNode_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);
extern void n_initcmds(nClass *);

//float *flatquadsquare::sin_lookup = NULL;
//float *flatquadsquare::cos_lookup = NULL;

nProfiler *nFlatTerrainNode::p_update = NULL;
nProfiler *nFlatTerrainNode::p_render = NULL;

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nFlatTerrainNode::local_cl = cl;
    nFlatTerrainNode::ks       = ks;
    nClass *super = ks->OpenClass("nvisnode");
    if (super) super->AddSubClass(nFlatTerrainNode::local_cl);
    else n_error("Could not open superclass 'nvisnode'\n");
    n_initcmds(cl);    
    
    // initialize the sincos lookup-table
//    flatquadsquare::sin_lookup = new float[flatquadsquare::LOOKUP_ENTRIES+1];
//    flatquadsquare::cos_lookup = new float[flatquadsquare::LOOKUP_ENTRIES+1];
//    float a = 0.0f;
//    float da = (2.0f*PI)/float(flatquadsquare::LOOKUP_ENTRIES);
//    int i;
//    for (i=0; i<=flatquadsquare::LOOKUP_ENTRIES; i++) {
//        flatquadsquare::sin_lookup[i] = (float) sin(a);
//        flatquadsquare::cos_lookup[i] = (float) cos(a);
//        a += da;
//    }
    nFlatTerrainNode::p_update = ks->ts->NewProfiler("flatter_update");
    nFlatTerrainNode::p_render = ks->ts->NewProfiler("flatter_render");


    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
//    if (flatquadsquare::sin_lookup) {
//        delete[] flatquadsquare::sin_lookup;
//        flatquadsquare::sin_lookup = NULL;
//   }
//  if (flatquadsquare::cos_lookup) {
//        delete[] flatquadsquare::cos_lookup;
//        flatquadsquare::cos_lookup = NULL;
//    }
    nClass *super = nFlatTerrainNode::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nFlatTerrainNode::local_cl);
        nFlatTerrainNode::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_new()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nFlatTerrainNode;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nFlatTerrainNode_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
