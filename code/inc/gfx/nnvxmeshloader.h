#ifndef N_NVXMESHLOADER_H
#define N_NVXMESHLOADER_H
//------------------------------------------------------------------------------
/**
    @class nNvxMeshLoader

    @brief Mesh loader class for the nvx Nebula binary mesh file format.

    The NVX file format is optimized for fast loading time. There's a converter
    tool for converting n3d to nvx files called nn3d2nvx.

    Here is the file format spec for nvx:

    <pre>
    // the header:

    int32 'NVX1'            // magic number for NVX1 file format
    int32 numVertices       // number of vertices
    int32 numIndices        // number of indices (can be 0)
    int32 numWingedEdges    // number of winged edges (can be 0)
    int32 vertexFormat      // OR'ed Nebula vertex components (N_VT_???)
    int32 dataStart         // start of data block
    int32 dataSize          // overall byte size of data block

    // data block
    // ALL data sub-blocks are optional!
    repeat numVertices:
        if (vertexFormat & N_VT_COORD)  
            float32[3] coord                // 3d coord
        if (vertexFormat & N_VT_NORM)
            float32[3] norm                 // normal
        if (vertexFormat & N_VT_RGBA)
            uint32     color                // vertex colors (in N_COLOR_BGRA format)!
        if (vertexFormat & N_VT_UV0)
            float32[2] uv0                  // uv coords layer 0
        if (vertexFormat & N_VT_UV1)
            float32[2] uv1                  // uv coords layer 1
        if (vertexFormat & N_VT_UV2)
            float32[2] uv2                  // uv coords layer 2
        if (vertexFormat & N_VT_UV3)
            float32[2] uv3                  // uv coords layer 3
        if (vertexFormat & N_VT_JW)
            int16 jointIndex0               // joint indices for skinning (-1 if not valid!)
            int16 jointIndex1
            int16 jointIndex2
            int16 jointIndex3
            float32 weight0                 // vertex weights for skinning
            float32 weight1
            float32 weight2
            float32 weight3
    endrepeat

    repeat numWingedEdges
        uint16[4] we                    // winged edge definition

    repeat numIndices
        uint16 index                    // primitive index
    </pre>
    

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_MESHLOADER_H
#include "gfx/nmeshloader.h"
#endif


//------------------------------------------------------------------------------
class nFile;
class nNvxMeshLoader : public nMeshLoader
{
public:
    /// constructor
    nNvxMeshLoader(nKernelServer* ks);
    /// load a ntx mesh file
    bool Load(const char* fileName);
private:
    /// read a 32 bit integer from file
    int readInt(nFile* file);
};

//------------------------------------------------------------------------------
/**
*/
inline
nNvxMeshLoader::nNvxMeshLoader(nKernelServer* ks) :
    nMeshLoader(ks)
{
    // empty
}

//------------------------------------------------------------------------------
#endif



