#define N_IMPLEMENTS nMapNode
//-----------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo.
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#include "map/nmap.h"
#include "map/mapquadtree.h"
#include "map/mapblock.h"

/**
    Automatically infer map position and obtain centre and all that.
*/
MapBlock::MapBlock(nMapNode* m) :
    map(m),
    startX(),
    startZ(),
    blockSize(map->GetBlockSize()),
    minD2(n_new float[map->GetNumMipMapLevels()]),
    boundingBox(),
    isVisible(false),
    currentLevel(0),
    currentStep(1),
    quadElement(NULL),
    refVertexBuffer(map),
    curIndexBuffer(NULL),
    curVertex(0)
{
    memset(neighborBlock, 0, 4 * sizeof(MapBlock*));
    sprintf(mapvbuf_name, "%08xm", (unsigned int)this);
}

MapBlock::~MapBlock()
{
    // Assume someone else has removed it for us
    if (NULL != quadElement)
        n_delete quadElement;

    if (NULL != minD2)
        delete[] minD2;
}

/**
    @brief Initialise vertex buffer
*/
void MapBlock::Init(nGfxServer* gfx_server, int num, int x, int z)
{
    startX = blockSize * x - x;
    startZ = blockSize * z - z;

    nMap* map_data = map->GetMap();

    // Set up bounding box
    boundingBox.set(map_data->GetPoint(startX, startZ).coord,
                    map_data->GetPoint(startX, startZ).coord);
    for (int loopZ = 0; loopZ < map->GetBlockSize(); ++loopZ)
    {
        for (int loopX = 0; loopX < map->GetBlockSize(); ++loopX)
        {
            boundingBox.grow(map_data->GetPoint(startX + loopX, startZ + loopZ).coord);
        }
    }

    char vbuf_name[80];
    sprintf(vbuf_name, "%s%d", mapvbuf_name, num);

    nVertexBuffer* vbuf = gfx_server->FindVertexBuffer(vbuf_name);
    if (NULL != vbuf)
        vbuf->Release();
    
    int num_indices = map->GetBlockSize() * map->GetBlockSize();
    int vertexType = N_VT_COORD | N_VT_NORM | N_VT_UV0 | N_VT_UV1;
    nVBufType vbufType = N_VBTYPE_STATIC;
    vbuf = gfx_server->NewVertexBuffer(vbuf_name, vbufType, vertexType, num_indices);
    n_assert(NULL != vbuf);

    // Initialise vertex buffer
    vbuf->LockVertices();

    float dim = float(map_data->GetDimension()-1);

    // Create a bounding box for the vertex buffer
    vector2 uv;
    for (int j = 0; j < map->GetBlockSize(); ++j)
    {
        int z = startZ + j;
        for (int i = 0; i < map->GetBlockSize(); ++i)
        {
            int x = startX + i;
            const MapPoint& pt = map_data->GetPoint(x, z);

            int index = i + j * map->GetBlockSize();

            vbuf->Coord(index, pt.coord);
            vbuf->Norm(index, pt.normal);

            float u = x / dim;
            float v = 1.0f - z / dim;

            uv.set(u, v);
            vbuf->Uv(index, 0, uv);

            uv *= map->GetDetailScale();
            vbuf->Uv(index, 1, uv);
        }
    }
    vbuf->UnlockVertices();
    vbuf->SetBBox(boundingBox);

    refVertexBuffer = vbuf;
}

/**
    Calculates the minimum distance squared (dMin^2) for each mip map
    level in this terrain block.

    For each mip map level, it takes the greatest delta from that level
    to the missing vertices.
*/
void MapBlock::CalculateMinD2Levels(float c2)
{
    nMap* map_data = map->GetMap();
    // d for level 0 is always 0
    minD2[0] = 0;
    
    float max_delta = 0;

    float heights[4];
    
    // Loop thru mip map levels
    for (int level = 1; level < map->GetNumMipMapLevels(); ++level)
    {
        int step = 1 << level;

        // Loop thru quads
        for (int j = startZ; j < startZ + map->GetBlockSize()-1; j += step)
        {
            for (int i = startX; i < startX + map->GetBlockSize()-1; i += step)
            {
                heights[0] = map_data->GetPoint(i,        j).coord.y;
                heights[1] = map_data->GetPoint(i,        j + step).coord.y;
                heights[2] = map_data->GetPoint(i + step, j).coord.y;
                heights[3] = map_data->GetPoint(i + step, j + step).coord.y;

                // For each quad, work out maximum delta
                for (int z = 1; z < step; ++z)
                {
                    for (int x = 1; x < step; ++x)
                    {
                        float interp_height = BilinearInterpolate(heights, float(x)/step, float(x)/step);
                        float height = map_data->GetPoint(i + x, j + z).coord.y;
                        float delta = fabsf(height - interp_height);
                        max_delta = n_max(delta, max_delta);
                    }
                }
            }
        }

        // Keep max delta between mip map levels to ensure it increases
        minD2[level] = max_delta*max_delta * c2;
    }
}

/**
    Obtain the interpolated height given four heights, where
    h[0] = top left
    h[1] = top right
    h[2] = bottom left
    h[3] = bottom right
*/
float MapBlock::BilinearInterpolate(float* h, float x, float z) const
{
    float top = h[0]*(1.0f - x) + x*h[1]; 
    float bottom = h[2]*(1.0f - x) + x*h[3];

    return top*(1.0f - z) + z*bottom;
}

/**
    Render block
*/
void MapBlock::Render(nSceneGraph2* scene_graph, nIndexBuffer* strip_ibuf, nIndexBuffer* ibuf)
{
    currentStep = 1 << currentLevel;

    // Check which neighbours require aligning,
    // could probably optimise this chunk to be obscure
    bool align_north = NULL != neighborBlock[NORTH] && neighborBlock[NORTH]->isVisible && neighborBlock[NORTH]->currentLevel > currentLevel;
    bool align_south = NULL != neighborBlock[SOUTH] && neighborBlock[SOUTH]->isVisible && neighborBlock[SOUTH]->currentLevel > currentLevel;
    bool align_west = NULL != neighborBlock[WEST] && neighborBlock[WEST]->isVisible && neighborBlock[WEST]->currentLevel > currentLevel;
    bool align_east = NULL != neighborBlock[EAST] && neighborBlock[EAST]->isVisible && neighborBlock[EAST]->currentLevel > currentLevel;

    int start_x = 0;
    int end_x =  blockSize-1;
    int start_z = 0;
    int end_z = blockSize-1;

    if (true == align_north)
        start_z += currentStep;
    if (true == align_south)
        end_z -= currentStep;
    if (true == align_west)
        start_x += currentStep;
    if (true == align_east)
        end_x -= currentStep;

    // Lock strip index buffer
    BeginRender(strip_ibuf);

    // Strip diagonals towards se, i.e. trailing diagonals
    bool downwards = true;
    for (int x = start_x; x < end_x; x += currentStep)
    {
        // Walk down
        if (true == downwards)
        {
            for (int z = start_z; z <= end_z; z += currentStep)
            {
                strip_ibuf->Index(curVertex, MapVertexToIndex(x+currentStep, z));
                strip_ibuf->Index(curVertex+1, MapVertexToIndex(x, z));
                curVertex += 2;
            }
        }
        // Walk up, add two degenerate triangles at start and each of strip
        // to generate correctly ordered vertices
        else
        {
            strip_ibuf->Index(curVertex, MapVertexToIndex(x, end_z));
            strip_ibuf->Index(curVertex+1, MapVertexToIndex(x, end_z));
            curVertex += 2;
            for (int z = end_z; z >= start_z; z -= currentStep)
            {
                strip_ibuf->Index(curVertex, MapVertexToIndex(x, z));
                strip_ibuf->Index(curVertex+1, MapVertexToIndex(x+currentStep, z));
                curVertex += 2;
            }
            strip_ibuf->Index(curVertex, MapVertexToIndex(x+currentStep, start_z));
            strip_ibuf->Index(curVertex+1, MapVertexToIndex(x+currentStep, start_z));
            curVertex += 2;
        }

        downwards = !downwards;
    }

    EndRender(scene_graph);

    BeginRender(ibuf);
    if (true == align_west) RenderWestEdge(align_north, align_south);
    if (true == align_east) RenderEastEdge(align_north, align_south);
    if (true == align_north) RenderNorthEdge(align_west, align_east);
    if (true == align_south) RenderSouthEdge(align_west, align_east);
    EndRender(scene_graph);
}

/**
    Render triangle fans starting from northwest corners
*/
void MapBlock::RenderNorthEdge(bool align_west, bool align_east)
{
    int vertices[5];

    int dbl_step = currentStep << 1;

    int z = 0;
    int end_x =  map->GetBlockSize()-1;
    int last_x = end_x - dbl_step;

    for (int x = 0; x < end_x; x += dbl_step)
    {
        vertices[0] = MapVertexToIndex(x, z);
        vertices[1] = MapVertexToIndex(x, z + currentStep);
        vertices[2] = MapVertexToIndex(x + currentStep, z + currentStep);
        vertices[3] = MapVertexToIndex(x + dbl_step, z + currentStep);
        vertices[4] = MapVertexToIndex(x + dbl_step, z);

        // Render weird blob
        if (true == align_east && last_x == x)
        {
            RenderTriangle(vertices[0], vertices[2], vertices[4]);
        }
        // Or normal
        else
        {
            RenderTriangle(vertices[0], vertices[3], vertices[4]);
            RenderTriangle(vertices[0], vertices[2], vertices[3]);
        }

        // Omit last triangle
        if (true == align_west && 0 == x)
            continue;

        RenderTriangle(vertices[0], vertices[1], vertices[2]);
    }
}

/**
    Render triangle fans starting from southwest corners
*/
void MapBlock::RenderSouthEdge(bool align_west, bool align_east)
{
    int vertices[5];

    int dbl_step = currentStep << 1;

    int z = map->GetBlockSize()-1 - currentStep;
    int end_x =  map->GetBlockSize()-1;
    int last_x = end_x - dbl_step;

    for (int x = 0; x < end_x; x += dbl_step)
    {
        vertices[0] = MapVertexToIndex(x + dbl_step, z + currentStep);
        vertices[1] = MapVertexToIndex(x + dbl_step, z);
        vertices[2] = MapVertexToIndex(x + currentStep, z);
        vertices[3] = MapVertexToIndex(x, z);
        vertices[4] = MapVertexToIndex(x, z + currentStep);

        // Render weird blob
        if (true == align_west && 0 == x)
        {
            RenderTriangle(vertices[0], vertices[2], vertices[4]);
        }
        // Or normal
        else
        {
            RenderTriangle(vertices[0], vertices[3], vertices[4]);
            RenderTriangle(vertices[0], vertices[2], vertices[3]);
        }

        // Omit last triangle
        if (true == align_east && last_x == x)
            continue;

        RenderTriangle(vertices[0], vertices[1], vertices[2]);
    }
}

/**
    Render triangle fans starting from northwest corners
*/
void MapBlock::RenderWestEdge(bool align_north, bool align_south)
{
    int vertices[5];

    int west_step = currentStep << 1;

    int x = 0;
    int end_z = map->GetBlockSize()-1;
    int last_z = end_z - west_step;

    for (int z = 0; z < end_z; z += west_step)
    {
        vertices[0] = MapVertexToIndex(x, z);
        vertices[1] = MapVertexToIndex(x, z + west_step);
        vertices[2] = MapVertexToIndex(x + currentStep, z + west_step);
        vertices[3] = MapVertexToIndex(x + currentStep, z + currentStep);
        vertices[4] = MapVertexToIndex(x + currentStep, z);

        // Render weird blob
        if (true == align_south && last_z == z)
        {
            RenderTriangle(vertices[0], vertices[1], vertices[3]);
        }
        // Or normal
        else
        {
            RenderTriangle(vertices[0], vertices[1], vertices[2]);
            RenderTriangle(vertices[0], vertices[2], vertices[3]);
        }

        // Omit last triangle
        if (true == align_north && 0 == z)
            continue;

        RenderTriangle(vertices[0], vertices[3], vertices[4]);
    }
}

/**
    Render triangle fans starting from southeast corners
*/
void MapBlock::RenderEastEdge(bool align_north, bool align_south)
{
    int vertices[5];

    int east_step = currentStep << 1;

    int x =  map->GetBlockSize()-1 - currentStep;
    int end_z = map->GetBlockSize()-1;
    int last_z = end_z - east_step;

    for (int z = 0; z < end_z; z += east_step)
    {
        vertices[0] = MapVertexToIndex(x + currentStep, z + east_step);
        vertices[1] = MapVertexToIndex(x + currentStep, z);
        vertices[2] = MapVertexToIndex(x, z);
        vertices[3] = MapVertexToIndex(x, z + currentStep);
        vertices[4] = MapVertexToIndex(x, z + east_step);

        // Render weird blob
        if (true == align_north && 0 == z)
        {
            RenderTriangle(vertices[0], vertices[1], vertices[3]);
        }
        // Or normal
        else
        {
            RenderTriangle(vertices[0], vertices[1], vertices[2]);
            RenderTriangle(vertices[0], vertices[2], vertices[3]);
        }

        // Omit last triangle
        if (true == align_south && last_z == z)
            continue;

        RenderTriangle(vertices[0], vertices[3], vertices[4]);
    }
}

MapQuadElement* MapBlock::GetQuadElement()
{
    if (NULL == quadElement)
        quadElement = n_new MapQuadElement(this);

    return quadElement;
}
