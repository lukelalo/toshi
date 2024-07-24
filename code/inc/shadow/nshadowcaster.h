#ifndef N_SHADOWCASTER_H
#define N_SHADOWCASTER_H
//------------------------------------------------------------------------------
/**
    @class nShadowCaster

    @brief A "vertex buffer" optimized for shadow generation.

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_WINGEDEDGE_H
#include "shadow/nwingededge.h"
#endif

#undef N_DEFINES
#define N_DEFINES nShadowCaster
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nShadowCaster : public nRoot
{
public:
    /// the constructor
    nShadowCaster();
    /// the destructor
    virtual ~nShadowCaster();

    /// initialize object
    void Initialize(int numCrds, int numEdgs);
    /// lock data for reading or writing
    void Lock();
    /// unlock data
    void Unlock();

    /// write a 3d coordinate (only when locked)
    void Coord(int i, const vector3& v);
    /// write a winged edge element (only when locked)
    void Edge(int i, const nWingedEdge& we);

    /// get number of coords
    int GetNumCoords();
    /// get number of edges
    int GetNumEdges();
    /// get pointer to coords (only when locked)
    vector3* GetCoordPtr();
    /// get pointer to edges (only when locked)
    nWingedEdge* GetEdgePtr();
    /// copy coordinates
    void CopyCoords(nShadowCaster* src, int first, int num);
    /// copy edges
    void CopyEdges(nShadowCaster* src, int first, int num);

    /// nKernelServer dependency.
    static nKernelServer* kernelServer;

public:
    bool isLocked;
    int numCoords;
    int numEdges;
    vector3* coords;
    nWingedEdge* edges;
};

//------------------------------------------------------------------------------
/**
    @param  numCrds     number of coordinates in shadowcaster
    @param  numEdgs     number of edges in shadowcaster
*/
inline
void
nShadowCaster::Initialize(int numCrds, int numEdgs)
{
    n_assert(numCrds > 0);
    n_assert(numEdgs > 0);
    if (this->coords)
    {
        delete[] this->coords;
        this->coords = 0;
        this->numCoords = 0;
    }
    if (this->edges)
    {
        delete[] this->edges;
        this->edges = 0;
        this->numEdges = 0;
    }
    this->numCoords = numCrds;
    this->coords    = new vector3[this->numCoords];
    this->numEdges  = numEdgs;
    this->edges     = new nWingedEdge[this->numEdges];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowCaster::Lock()
{
    n_assert(!this->isLocked);
    n_assert(this->coords && this->edges);
    this->isLocked = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowCaster::Unlock()
{
    n_assert(this->isLocked);
    this->isLocked = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowCaster::Coord(int i, const vector3& v)
{
    n_assert(this->isLocked);
    n_assert(this->coords);
    n_assert((i >= 0) && (i < this->numCoords));
    this->coords[i] = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowCaster::Edge(int i, const nWingedEdge& we)
{
    n_assert(this->isLocked);
    n_assert(this->edges);
    n_assert((i >= 0) && (i < this->numEdges));
    this->edges[i] = we;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nShadowCaster::GetNumCoords()
{
    return this->numCoords;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nShadowCaster::GetNumEdges()
{
    return this->numEdges;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3*
nShadowCaster::GetCoordPtr()
{
    n_assert(this->isLocked);
    n_assert(this->coords);
    return this->coords;
}

//------------------------------------------------------------------------------
/**
*/
inline
nWingedEdge*
nShadowCaster::GetEdgePtr()
{
    n_assert(this->isLocked);
    n_assert(this->edges);
    return this->edges;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowCaster::CopyCoords(nShadowCaster* src, int first, int num)
{
    n_assert(src);
    n_assert(first >= 0);
    n_assert((first + num) <= src->GetNumCoords());
    n_assert((first + num) <= this->numCoords);
    this->Lock();
    src->Lock();
    memcpy(&(this->coords[first]), &(src->coords[first]), num * sizeof(vector3));
    src->Unlock();
    this->Unlock();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowCaster::CopyEdges(nShadowCaster* src, int first, int num)
{
    n_assert(src);
    n_assert(first >= 0);
    n_assert((first + num) <= src->GetNumEdges());
    n_assert((first + num) <= this->numEdges);
    this->Lock();
    src->Lock();
    memcpy(&(this->edges[first]), &(src->edges[first]), num * sizeof(nWingedEdge));
    src->Unlock();
    this->Unlock();
}

//------------------------------------------------------------------------------
#endif
