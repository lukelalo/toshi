//------------------------------------------------------------------------------
//  nshadowcaster_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shadow/nshadowcaster.h"

#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

nNebulaClass(nShadowCaster, "nroot");

//------------------------------------------------------------------------------
/**
*/
nShadowCaster::nShadowCaster() :
    isLocked(false),
    numCoords(0),
    numEdges(0),
    coords(0),
    edges(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nShadowCaster::~nShadowCaster()
{
    if (this->coords)
    {
        delete[] this->coords;
        this->coords = 0;
    }
    if (this->numEdges)
    {
        delete[] this->edges;
        this->edges = 0;
    }
}

//------------------------------------------------------------------------------


