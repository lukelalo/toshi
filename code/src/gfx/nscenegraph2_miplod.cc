#define N_IMPLEMENTS nSceneGraph2
//------------------------------------------------------------------------------
//  nscenegraph2_miplod.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/nscenegraph2.h"

//------------------------------------------------------------------------------
/**
    @brief Enable/disable mipmap LOD handling.
    
    When disabled, always the complete set of mipmaps will be used
    for rendering.
*/
void
nSceneGraph2::SetEnableMipLod(bool b)
{
    this->mipLodEnabled = b;
}

//------------------------------------------------------------------------------
/**
    @brief Get the current enable/disable state of mipmap LOD handling.
*/
bool 
nSceneGraph2::GetEnableMipLod()
{
    return this->mipLodEnabled;
}

//------------------------------------------------------------------------------
/**
    @brief Begin building the mipmap lod table.
    
    This is simply a table of distances.

    @param    index         number of levels to store in the table
*/
void
nSceneGraph2::BeginMipLod(int numLevels)
{
    n_assert(numLevels > 0);
    if (this->mipLodTable)
    {
        delete[] this->mipLodTable;
        this->mipLodTable = 0;
    }
    this->numMipLods = numLevels;
    this->mipLodTable = new float[numLevels];
}

//------------------------------------------------------------------------------
/**
    @brief Set an entry in the mipmap LOD table.

    @param   index     entry in the table to set
    @param   dist      distance to set the entry to
*/
void
nSceneGraph2::SetMipLod(int index, float dist)
{
    n_assert((index >= 0) && (index < this->numMipLods));
    this->mipLodTable[index] = dist;
}

//------------------------------------------------------------------------------
/**
    @brief Finish defining the mip lod table.
*/
void
nSceneGraph2::EndMipLod()
{
    // empty
}

//------------------------------------------------------------------------------
