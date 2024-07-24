//------------------------------------------------------------------------------
//  nn3d2nvx.cc
//  
//  Convert n3d ascii files to Nebula nvx binary mesh files.
//
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/wfobject.h"
#include "tools/wftools.h"
#include "gfx/ngfxtypes.h"

//------------------------------------------------------------------------------
static
int 
triangle_sort_hook(const void *elm0, const void *elm1)
{
    ushort *i0 = (ushort *) elm0;
    ushort *i1 = (ushort *) elm1;
    return int(*i0) - int(*i1);
}

//------------------------------------------------------------------------------
void
sortTriangles(ushort* indices, int numIndices)
{
    // sort the triangles to improve cache coherence
    qsort(indices, numIndices/3, 3 * sizeof(ushort), triangle_sort_hook);
}

//------------------------------------------------------------------------------
void
writeInt(FILE* fp, unsigned int val)
{
    fwrite(&val, sizeof(val), 1, fp);
}

//------------------------------------------------------------------------------
void
writeNvx(FILE* fp, wfObject& src)
{
    // compute header data
    int magic          = 'NVX1';
    int numVertices    = src.v_array.Size();
    int numIndices     = src.f_array.Size() * 3;
    int numWingedEdges = src.we_array.Size();

    // get vertex format and size of data block
    int vertexType = 0;
    int dataStart = 7 * sizeof(int);
    int dataSize  = (sizeof(ushort) * numIndices) + (4 * sizeof(ushort) * numWingedEdges);

    if (!src.v_array.Empty())
    {
        vertexType |= N_VT_COORD;
        dataSize += numVertices * 3 * sizeof(float);

        // vertex weights?
        if (src.v_array.At(0).ji[0] != -1)
        {
            vertexType |= N_VT_JW;
            dataSize += numVertices * ((4 * sizeof(ushort)) + (4 * sizeof(float)));
        }
    }
    if (!src.vn_array.Empty())
    {
        vertexType |= N_VT_NORM;
        dataSize += numVertices * 3 * sizeof(float);
    }
    if (!src.c_array.Empty())
    {
        vertexType |= N_VT_RGBA;
        dataSize += numVertices * sizeof(int);
    }
    if (!src.vt_array.Empty())
    {
        vertexType |= N_VT_UV0;
        dataSize += numVertices * 2 * sizeof(float);
    }
    if (!src.vt1_array.Empty())
    {
        vertexType |= N_VT_UV1;
        dataSize += numVertices * 2 * sizeof(float);
    }
    if (!src.vt2_array.Empty())
    {
        vertexType |= N_VT_UV2;
        dataSize += numVertices * 2 * sizeof(float);
    }
    if (!src.vt3_array.Empty())
    {
        vertexType |= N_VT_UV3;
        dataSize += numVertices * 2 * sizeof(float);
    }

    // write file header
    writeInt(fp, magic);            // magic number
    writeInt(fp, numVertices);
    writeInt(fp, numIndices);
    writeInt(fp, numWingedEdges);
    writeInt(fp, (unsigned int) vertexType);
    writeInt(fp, dataStart);
    writeInt(fp, dataSize);

    // allocate and fill data block
#define write_elm(type, elm)    *(type*)ptr = elm; ptr += sizeof(type);   

    char* data = (char*) n_malloc(dataSize);
    char* ptr = (char*) data;
    int i;
    for (i = 0; i < numVertices; i++)
    {
        if (vertexType & N_VT_COORD)
        {
            const vector3& v = src.v_array.At(i).v;
            write_elm(float, v.x);
            write_elm(float, v.y);
            write_elm(float, v.z);
        }
        if (vertexType & N_VT_NORM)
        {
            const vector3& v = src.vn_array.At(i);
            write_elm(float, v.x);
            write_elm(float, v.y);
            write_elm(float, v.z);
        }
        if (vertexType & N_VT_RGBA)
        {
            const vector4& v = src.c_array.At(i);
            unsigned int c = n_f2bgra(v.x, v.y, v.z, v.w);
            write_elm(unsigned int, c);
        }
        if (vertexType & N_VT_UV0)
        {
            const vector2& v = src.vt_array.At(i);
            write_elm(float, v.x);
            write_elm(float, v.y);
        }
        if (vertexType & N_VT_UV1)
        {
            const vector2& v = src.vt1_array.At(i);
            write_elm(float, v.x);
            write_elm(float, v.y);
        }
        if (vertexType & N_VT_UV2)
        {
            const vector2& v = src.vt2_array.At(i);
            write_elm(float, v.x);
            write_elm(float, v.y);
        }
        if (vertexType & N_VT_UV3)
        {
            const vector2& v = src.vt3_array.At(i);
            write_elm(float, v.x);
            write_elm(float, v.y);
        }
        if (vertexType & N_VT_JW)
        {
            const wfCoord& coord = src.v_array.At(i);
            write_elm(short, coord.ji[0]);
            write_elm(short, coord.ji[1]);
            write_elm(short, coord.ji[2]);
            write_elm(short, coord.ji[3]);
            write_elm(float, coord.w[0]);
            write_elm(float, coord.w[1]);
            write_elm(float, coord.w[2]);
            write_elm(float, coord.w[3]);
        }
    }

    // write winged edges
    for (i = 0; i < numWingedEdges; i++)
    {
        const wfWingedEdge& we = src.we_array.At(i);
        write_elm(ushort, we.v0);
        write_elm(ushort, we.v1);
        write_elm(ushort, we.vp0);
        write_elm(ushort, we.vp1);
    }

    // write triangle indices
    int numFaces = src.f_array.Size();
    ushort* indexStart = (ushort*) ptr;
    for (i = 0; i < numFaces; i++)
    {
        wfFace& face = src.f_array.At(i);
        const wfPoint& p0 = face.points.At(0);
        const wfPoint& p1 = face.points.At(1);
        const wfPoint& p2 = face.points.At(2);
        write_elm(ushort, p0.v_index);
        write_elm(ushort, p1.v_index);
        write_elm(ushort, p2.v_index);
    }

    // sort triangles indices
    sortTriangles(indexStart, numIndices);

    // make sure the data block has been written correctly
    assert((data + dataSize) == (char*)ptr);

    // write data block
    fwrite(data, dataSize, 1, fp);

    // cleanup
    if (data)
    {
        n_free(data);
        data = 0;
    }
}

//------------------------------------------------------------------------------
int
main(int argc, char* argv[])
{
    bool help;
    const char* n3dName;
    const char* nvxName;
    int retval = 0;
    FILE* in  = 0;
    FILE* out = 0;

    // get args
    help    = wf_getboolarg(argc, argv, "-help");
    n3dName = wf_getstrarg(argc, argv, "-in", "mesh.n3d");
    nvxName = wf_getstrarg(argc, argv, "-out", "mesh.nvx");
    if (help)
    {
        printf("nn3d2nvx [-help] -in n3dfile -out nvxfile\n"
               "(C) 2001 RadonLabs GmbH\n"
               "Convert n3d to Nebula nvx file format.\n"
               "-help   -- show help\n"
               "-in     -- filename of input n3d file (def mesh.n3d)\n"
               "-out    -- filename of output nvx file (def mesh.nvx)\n");
        return 0;
    }

    wfObject src;

    // open files
    in = fopen(n3dName, "r");
    if (!in)
    {
        printf("Could not open input file '%s'\n", n3dName);
        printf("Try running with -help for usage information.\n");
        retval = 5;
        goto ende;
    }
    out = fopen(nvxName, "wb");
    if (!out)
    {
        printf("Could not open output file '%s'\n", nvxName);
        retval = 5;
        goto ende;
    }

    // load n3d file into wfObject
    if (!src.load(in))
    {
        printf("Failed to load '%s' as n3d!\n", n3dName);
        retval = 5;
        goto ende;
    }

    // write wfObject as nvx file
    writeNvx(out, src);

    // clean up
ende:
    if (in)
    {
        fclose(in);
        in = 0;
    }
    if (out)
    {
        fclose(out);
        out = 0;
    }
    return retval;
}
//------------------------------------------------------------------------------
