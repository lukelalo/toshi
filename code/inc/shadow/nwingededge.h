#ifndef N_WINGEDEDGE_H
#define N_WINGEDEDGE_H
//------------------------------------------------------------------------------
/**
    @class nWingedEdge

    @brief Simple inline class to define winged edges (good for
    silhouette finding).

    The winged edge is defined by 4 indices into a vertex array. The
    indices v0 and v1 are the actual vertices forming the edge. The 2
    other indices (vp0 and vp1) are the 2 vertices which define the
    remaining points of 2 neighboring triangles.

@verbatim
            + vp0
          /   \
     v0 /       \ v1
      +===========+  <-- the edge
        \       /
          \   /
            + vp1
@endverbatim



    (C) 2001 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

//------------------------------------------------------------------------------
class nWingedEdge
{
public:
    /// constructor
    nWingedEdge();
    /// copy constructor
    nWingedEdge(ushort _v0, ushort _v1, ushort _vp0, ushort _vp1);
    /// set contents
    void Set(ushort _v0, ushort _v1, ushort _vp0, ushort _vp1);
    /// assignment operator
    nWingedEdge& operator=(const nWingedEdge& rhs);

    ushort v0;
    ushort v1;
    ushort vp0;
    ushort vp1;
};

//------------------------------------------------------------------------------
/**
*/
inline
nWingedEdge::nWingedEdge() :
    v0(0),
    v1(0),
    vp0(0),
    vp1(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nWingedEdge::nWingedEdge(ushort _v0, ushort _v1, ushort _vp0, ushort _vp1) :
    v0(_v0),
    v1(_v1),
    vp0(_vp0),
    vp1(_vp1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nWingedEdge& 
nWingedEdge::operator=(const nWingedEdge& rhs)
{
    this->v0 = rhs.v0;
    this->v1 = rhs.v1;
    this->vp0 = rhs.vp0;
    this->vp1 = rhs.vp1;
    return *this;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nWingedEdge::Set(ushort _v0, ushort _v1, ushort _vp0, ushort _vp1)
{
    this->v0  = _v0;
    this->v1  = _v1;
    this->vp0 = _vp0;
    this->vp1 = _vp1;
}

//------------------------------------------------------------------------------
#endif
