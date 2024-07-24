#define N_IMPLEMENTS nMapNode
//-----------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo.
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#include "gfx/nindexbuffer.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nvertexbuffer.h"
#include "map/nmapnode.h"
#include "map/nmap.h"

nNebulaScriptClass(nMapNode, "nvisnode");

//------------------------------------------------------------------------------
/**
*/
nMapNode::nMapNode() :
    refGfxServer(kernelServer, this),
    // refVertexBuffer(this),
    refIndexBuffer(this),
    refStripIndexBuffer(this),
    // Map variables
    refMap(kernelServer, this),
    detailSize(1.0f),
    detailScale(0.0f),
    isDirty(false),
    numMipMapLevels(0),
    numBlocks(0),
    blockArray(),
    mapQuadtree(NULL),
    renderDebug(false)
{
    refGfxServer = "/sys/servers/gfx";
    sprintf(edgebuf_name, "%08xe", (unsigned int)this);
    sprintf(stripbuf_name, "%08xs", (unsigned int)this);
}

//------------------------------------------------------------------------------
/**
*/
nMapNode::~nMapNode()
{
    // release index buffer
    if (refIndexBuffer.isvalid())
    {
        refIndexBuffer->Release();
        refIndexBuffer.invalidate();
    }
}

/**
*/
void nMapNode::Preload()
{
    nVisNode::Preload();
    if (true == this->refMap.isvalid())
        refMap->LoadMap();
}

//---------------------------------------------------------------------------
/**
    Attach
*/
bool nMapNode::Attach(nSceneGraph2* sceneGraph)
{
    if (true == nVisNode::Attach(sceneGraph)) 
    {
        sceneGraph->AttachVisualNode(this);
        return true;
    }
    return false;
}

/**
    If this replaces an existing heightmap,
    it forces a reload immediately to ensure sim runs without a hitch.
*/
void nMapNode::SetMapPath(const char* name)
{
    refMap = name;
    isDirty = true;
}

/**
    @return The filename for the heightmap data.
*/
const char* nMapNode::GetMapPath()
{
    return refMap.getname();
}

/**
    @brief Set the terrain block size in vertices.
    This is used to divide into the heightmap where:
    
        dimensions = block_size * num_blocks - (num_blocks - 1)

    Or:

        num_blocks = (dimensions - 1) / (block_size - 1)

    It is far easier to keep blockSize as 2^n+1.
        
    @param size A value of 2^n+1
*/
void nMapNode::SetBlockSize(int size)
{
    n_assert(0 < size);
    n_assert(size%2);
    blockSize = size;
    isDirty = true;
}

void nMapNode::SetError(int error)
{
    pixelError = error;
    isDirty = true;
}

int nMapNode::GetError() const
{
    return pixelError;
}

/**
    @param size Size of the texture detail in metres.
*/
void nMapNode::SetDetailSize(float size)
{
    n_assert(0.0f < size);
    detailSize = size;
    isDirty = true;
}

/**
    @return Size of a single detail texture.
*/
float nMapNode::GetDetailSize()
{
    return detailSize;
}
