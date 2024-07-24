#define N_IMPLEMENTS nMapNode
//-----------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo.
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#include "gfx/nvertexbuffer.h"
#include "gfx/nindexbuffer.h"
#include "gfx/nscenegraph2.h"
// #include "gfx/nvertexbuffer.h"
#include "node/n3dnode.h"
#include "map/mapquadtree.h"
#include "map/mapblock.h"
#include "map/nmapnode.h"
// #include "map/nmap.h"

/**
    Compute
*/
void nMapNode::Compute(nSceneGraph2* scene_graph)
{
    int i;
    n_assert(scene_graph);
    nVisNode::Compute(scene_graph);

    // Process map if dirty
    if (true == isDirty)
    {
        ProcessMap();
        InitBuffers();
    }

    nGfxServer* gfx_server = refGfxServer.get();

    // Set visible
    for (int j = 0; j < numBlocks; ++j)
        for (i = 0; i < numBlocks; ++i)
            blockArray[j][i]->SetVisible(false);

    // Frustum culling
    MapQuadElement** collect_array = NULL;
    int num_blocks = mapQuadtree->CollectByFrustum(gfx_server, collect_array);

    // Calculate mipmaps
    matrix44 view;
    gfx_server->GetMatrix(N_MXM_VIEWER, view);
    vector3 camera_pos = view.pos_component();
    for (i = 0; i < num_blocks; ++i)
    {
        MapBlock* block = (MapBlock*)collect_array[i]->GetPtr();
        block->SelectMipMapLevel(camera_pos);
    }

    // Level mipmapping with neighbours
    bool repeat = false;
    do
    {
        repeat = false;

        for (i = 0; i < num_blocks; ++i)
        {
            MapBlock* block = (MapBlock*)collect_array[i]->GetPtr();
            if (true == block->AlignMipMapLevel())
                repeat = true;
        }
    }
    while (true == repeat);

    // Render it all
	if (refIndexBuffer.isvalid()) 
	{
		nIndexBuffer* ibuf = refIndexBuffer.get();
		nIndexBuffer* strip_ibuf = refStripIndexBuffer.get();
		for (i = 0; i < num_blocks; ++i)
		{
			MapBlock* block = (MapBlock*)collect_array[i]->GetPtr();
				block->Render(scene_graph, strip_ibuf, ibuf);
		}
	}
}

/**
    @brief Initialise all the buffers necessary for rendering.
*/
void nMapNode::InitBuffers()
{
    int i;
    n_assert(false == isDirty);

    /* For when reducing the number of vertex buffers in use
    nMap* map = refMap.get();
    int map_size = map->GetDimension();

    // Create complete vertex buffer
    nVertexBuffer* vbuf = refGfxServer->FindVertexBuffer("nmapnode_vbuf");
    if (NULL != vbuf)
        vbuf->Release();

    nVBufType vbuf_type = N_VBTYPE_STATIC;
    int vertex_type = N_VT_COORD | N_VT_NORM | N_VT_UV0 | N_VT_UV1;
    int num_vertices = map_size * map_size;
    vbuf = refGfxServer->NewVertexBuffer("nmapnode_vbuf", vbuf_type, vertex_type, num_vertices);
    vbuf->LockVertices();

    vector2 uv;
    for (int z = 0; z < map_size; ++z)
    {
        for (int x = 0; x < map_size; ++x)
        {
            const MapPoint& pt = map->GetPoint(x, z);
            int index = x + z * map_size;

            vbuf->Coord(index, pt.coord);
            vbuf->Norm(index, pt.normal);

            float u = x / float(map_size);
            float v = 1.0f - z / float(map_size);
            // Silly wrapping for one texture per block
            // float u = x / float(map->GetBlockSize());
            // float v = 1.0f - z / float(map->GetBlockSize());

            uv.set(u, v);
            vbuf->Uv(index, 0, uv);

            uv *= this->detailScale;
            vbuf->Uv(index, 1, uv);
        }
    }
    vbuf->UnlockVertices();
    bbox3 bbox(map->GetPoint(0, 0).coord,
               map->GetPoint(map_size-1, map_size-1).coord);
    bbox.vmax.y = map->GetHeightRangeMax();
    bbox.vmin.y = map->GetHeightRangeMin();
    vbuf->SetBBox(bbox);

    refVertexBuffer = vbuf;
    */

    // Create triangle list index buffer
    int num_indices = 6 * blockSize * blockSize;

    nIndexBuffer* ibuf = refGfxServer->FindIndexBuffer(edgebuf_name);
    if (NULL != ibuf)
        ibuf->Release();
    ibuf = refGfxServer->NewIndexBuffer(edgebuf_name);
    n_assert(NULL != ibuf);

    ibuf->Begin(N_IBTYPE_WRITEONLY, N_PTYPE_TRIANGLE_LIST, num_indices);
    for (i = 0; i < num_indices; ++i)
        ibuf->Index(i, i);
    ibuf->End();
    refIndexBuffer = ibuf;

    // Create triangle strip index buffer
    ibuf = refGfxServer->FindIndexBuffer(stripbuf_name);
    if (NULL != ibuf)
        ibuf->Release();
    ibuf = refGfxServer->NewIndexBuffer(stripbuf_name);
    n_assert(NULL != ibuf);

    ibuf->Begin(N_IBTYPE_WRITEONLY, N_PTYPE_TRIANGLE_STRIP, num_indices);
    for (i = 0; i < num_indices; ++i)
        ibuf->Index(i, i);
    ibuf->End();
    refStripIndexBuffer = ibuf;
}

/**
    Visualises terrain occlusion details
*/
int nMapNode::Visualize()
{
    nGfxServer* gfx_server = refGfxServer.get();
    if (NULL != mapQuadtree)
    {
        mapQuadtree->Visualize(gfx_server);
        MapQuadElement** collect_array = NULL;
        return mapQuadtree->GetCollect(collect_array);
    }

    return 0;
}
