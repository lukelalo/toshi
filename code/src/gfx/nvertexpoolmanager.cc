#define N_IMPLEMENTS nGfxServer
//-------------------------------------------------------------------
//  nvertexpoolmanager.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nvertexpoolmanager.h"
#include "gfx/nvertexpool.h"
#include "gfx/nvertexbubble.h"
#include "gfx/ngfxserver.h"

//-------------------------------------------------------------------
//  ~nVertexPoolManager()
//  31-Aug-00   floh    created
//-------------------------------------------------------------------
nVertexPoolManager::~nVertexPoolManager()
{
    // release all owned vertex pools
    int i;
    for (i=0; i<N_LISTARRAYSIZE; i++) {
        nVertexPool *vpool;
        while ((vpool = (nVertexPool *) this->coord[i].RemHead()))              n_delete vpool;
        while ((vpool = (nVertexPool *) this->coord_norm[i].RemHead()))         n_delete vpool;
        while ((vpool = (nVertexPool *) this->coord_rgba[i].RemHead()))         n_delete vpool;
        while ((vpool = (nVertexPool *) this->coord_norm_rgba[i].RemHead()))    n_delete vpool;
        while ((vpool = (nVertexPool *) this->coord_jw[i].RemHead()))           n_delete vpool;
        while ((vpool = (nVertexPool *) this->coord_norm_jw[i].RemHead()))      n_delete vpool;
        while ((vpool = (nVertexPool *) this->coord_rgba_jw[i].RemHead()))      n_delete vpool;
        while ((vpool = (nVertexPool *) this->coord_norm_rgba_jw[i].RemHead())) n_delete vpool;
    }
}

//-------------------------------------------------------------------
//  map_vtype_2_list()
//  31-Aug-00   floh    created
//-------------------------------------------------------------------
nList *nVertexPoolManager::map_vtype_2_list(int vtype)
{
    if (vtype & N_VT_COORD) {
        if (vtype & N_VT_NORM) {
            if (vtype & N_VT_RGBA) {
                if (N_VT_JW & vtype) {
                    // coords + normals + color + jointweights + any number of tex coords
                    if (N_VT_UV3 & vtype)      return &(coord_norm_rgba_jw[4]);
                    else if (N_VT_UV2 & vtype) return &(coord_norm_rgba_jw[3]);
                    else if (N_VT_UV1 & vtype) return &(coord_norm_rgba_jw[2]);
                    else if (N_VT_UV0 & vtype) return &(coord_norm_rgba_jw[1]);
                    else                       return &(coord_norm_rgba_jw[0]);
                } else {
                    // coords + normals + color + any number of tex coords
                    if (N_VT_UV3 & vtype)      return &(coord_norm_rgba[4]);
                    else if (N_VT_UV2 & vtype) return &(coord_norm_rgba[3]);
                    else if (N_VT_UV1 & vtype) return &(coord_norm_rgba[2]);
                    else if (N_VT_UV0 & vtype) return &(coord_norm_rgba[1]);
                    else                       return &(coord_norm_rgba[0]);
                }
            } else {
                if (N_VT_JW & vtype) {
                    // coords + normals + jointweights + any number of tex coords
                    if (N_VT_UV3 & vtype)      return &(coord_norm_jw[4]);
                    else if (N_VT_UV2 & vtype) return &(coord_norm_jw[3]);
                    else if (N_VT_UV1 & vtype) return &(coord_norm_jw[2]);
                    else if (N_VT_UV0 & vtype) return &(coord_norm_jw[1]);
                    else                       return &(coord_norm_jw[0]);
                } else {
                    // coords + normals + any number of tex coords
                    if (N_VT_UV3 & vtype)      return &(coord_norm[4]);
                    else if (N_VT_UV2 & vtype) return &(coord_norm[3]);
                    else if (N_VT_UV1 & vtype) return &(coord_norm[2]);
                    else if (N_VT_UV0 & vtype) return &(coord_norm[1]);
                    else                       return &(coord_norm[0]);
                }
            }
        } else if (vtype & N_VT_RGBA) {
            if (N_VT_JW & vtype) {
                // coords + colors + jointweights + any number of tex coords
                if (vtype & N_VT_UV3)      return &(coord_rgba_jw[4]);
                else if (vtype & N_VT_UV2) return &(coord_rgba_jw[3]);
                else if (vtype & N_VT_UV1) return &(coord_rgba_jw[2]);
                else if (vtype & N_VT_UV0) return &(coord_rgba_jw[1]);
                else                       return &(coord_rgba_jw[0]);
            } else {
                // coords + colors + any number of tex coords
                if (vtype & N_VT_UV3)      return &(coord_rgba[4]);
                else if (vtype & N_VT_UV2) return &(coord_rgba[3]);
                else if (vtype & N_VT_UV1) return &(coord_rgba[2]);
                else if (vtype & N_VT_UV0) return &(coord_rgba[1]);
                else                       return &(coord_rgba[0]);
            }
        } else {
            if (N_VT_JW & vtype) {
                // coords + any number of tex coords
                if (vtype & N_VT_UV3)      return &(coord_jw[4]);
                else if (vtype & N_VT_UV2) return &(coord_jw[3]);
                else if (vtype & N_VT_UV1) return &(coord_jw[2]);
                else if (vtype & N_VT_UV0) return &(coord_jw[1]);
                else                       return &(coord_jw[0]);
            } else {
                // coords + any number of tex coords
                if (vtype & N_VT_UV3)      return &(coord[4]);
                else if (vtype & N_VT_UV2) return &(coord[3]);
                else if (vtype & N_VT_UV1) return &(coord[2]);
                else if (vtype & N_VT_UV0) return &(coord[1]);
                else                       return &(coord[0]);
            }
        }
    }
    return NULL;
}

//-------------------------------------------------------------------
//  NewVertexBubble()
//  31-Aug-00   floh    created
//-------------------------------------------------------------------
nVertexBubble *nVertexPoolManager::NewVertexBubble(int vtype, int size)
{
    n_assert(this->gs);
    n_assert(this->prefered_pool_size > 0);

    // map the vertex type to one of the vertex pool lists
    nList *ls = this->map_vtype_2_list(vtype);
    n_assert(ls);

    // check if one of the vertex pools in the list can create
    // a bubble of the requested size
    nVertexBubble *vbubble = NULL;
    nVertexPool *vp = NULL;
    for (vp = (nVertexPool *) ls->GetHead();
         vp;
         vp = (nVertexPool *) vp->GetSucc())
    {
        vbubble = vp->NewBubble(size);
        if (vbubble) return vbubble;
    }

    // fallthrough: couldn't create a bubble in the existing
    // vertex pools, create a new pool...
    if (size >= this->prefered_pool_size) {
        // the requested size is bigger then the prefered
        // pool size, create a "custom" tool specifically
        // for this bubble
        vp = this->gs->NewVertexPool(this,this->vbuf_type,vtype,size);
    } else {
        // the requested size is less then the prefered pool
        // size, create a pool of the prefered size, so that
        // this pool can possibly be shared with future bubbles
        vp = this->gs->NewVertexPool(this,this->vbuf_type,vtype,this->prefered_pool_size);
    }
    n_assert(vp);
    ls->AddTail(vp);
    vbubble = vp->NewBubble(size);
    n_assert(vbubble);
    return vbubble;            
}

//-------------------------------------------------------------------
//  ReleaseVertexBubble()
//  31-Aug-00   floh    created
//-------------------------------------------------------------------
void nVertexPoolManager::ReleaseVertexBubble(nVertexBubble *vbubble)
{
    n_assert(vbubble);
    nVertexPool *vp = vbubble->GetVertexPool();
    n_assert(vp);
    
    // release the bubble
    vp->ReleaseBubble(vbubble);

    // check if the vertex pool is empty, if yes, release it
    if (vp->IsEmpty()) {
        vp->Remove();
        n_delete vp;
        vp = NULL;
    };
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
