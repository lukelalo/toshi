#ifndef N_VERTEXSORTER_H
#define N_VERTEXSORTER_H
//------------------------------------------------------------------------------
/**
    @class nVertexSorter

    @brief Provides depth sorted access to an array of 3d coordinates from
    a dynamic view point.

    (C) 2001 A.Weissflog
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

//------------------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nVertexSorter
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nVertexSorter 
{
public:
    /// constructor
    nVertexSorter();
    /// destructor
    ~nVertexSorter();

    /// initialize the vertex sorter from an untransformed vertex array
    void Prepare(float* vertices, int numVerts, int stride4);
    /// get sorted index array from transformed vertices
    const ushort* SortIndices(const matrix44& vwr, const float* vertices, int numVerts, int stride4);

    /// data for qsort hook function
    static float* tmpArrayPointer;

private:
    /// delete arrays
    void FreeArrays();
    /// allocate arrays
    void AllocArrays(int size);
    /// a qsort compare hook function
    static int QSortHook(const ushort* elm1, const ushort* elm2);

    /// number of vertices
    int numVertices;
    /// resulting array of sorted indices
    ushort* sortedIndexArray;
    /// array of transformed z coordinated
    float* zArray;
};

//------------------------------------------------------------------------------
#endif
