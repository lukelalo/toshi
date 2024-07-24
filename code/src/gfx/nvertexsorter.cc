#define N_IMPLEMENTS nVertexSorter
//------------------------------------------------------------------------------
//  nvertexsorter.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/nvertexsorter.h"

// static member initialization
float* nVertexSorter::tmpArrayPointer = 0;

//------------------------------------------------------------------------------
/**
*/
nVertexSorter::nVertexSorter() :
    numVertices(0),
    sortedIndexArray(0),
    zArray(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nVertexSorter::~nVertexSorter()
{
    this->FreeArrays();
}

//------------------------------------------------------------------------------
/**
*/
void
nVertexSorter::FreeArrays()
{
    if (this->sortedIndexArray)
    {
        n_free(this->sortedIndexArray);
        this->sortedIndexArray = 0;
    }
    if (this->zArray)
    {
        n_free(this->zArray);
        this->zArray = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nVertexSorter::AllocArrays(int size)
{
    n_assert(0 == this->sortedIndexArray);
    n_assert(0 == this->zArray);

    this->sortedIndexArray = (ushort*) n_malloc(sizeof(ushort) * size);
    this->zArray = (float*) n_malloc(sizeof(float) * size);
    
    n_assert(this->sortedIndexArray);
    n_assert(this->zArray);
}

//------------------------------------------------------------------------------
/**
    Prepare vertex sorter for a new vertex array. This step can take a 
    long time and should only be called once for the vertex array

    @param  vertices        pointer to vertex array (first 3 components must be x,y,z)
    @param  numVerts        number of vertices in the array
    @param  stride4         stride of the vertex array in sizeof(float)
*/
void
nVertexSorter::Prepare(float* vertices, int numVerts, int stride4)
{
    n_assert(vertices);
    n_assert(numVerts > 0);
    n_assert(stride4 >= 3);

    // reallocate arrays
    this->numVertices = numVerts;
    this->FreeArrays();
    this->AllocArrays(this->numVertices);

    // initialize the sorted index array
    int i;
    for (i = 0; i < this->numVertices; i++)
    {
        this->sortedIndexArray[i] = i;
    }
}

//------------------------------------------------------------------------------
/**
    The qsort() cmp hook.
*/
int
nVertexSorter::QSortHook(const ushort* elm1, const ushort* elm2)
{
    float* ptr1 = nVertexSorter::tmpArrayPointer + (*elm1);
    float* ptr2 = nVertexSorter::tmpArrayPointer + (*elm2);
    float z1 = *ptr1;
    float z2 = *ptr2;
    if (z1 < z2)        return -1;
    else if (z1 > z2)   return +1;
    else                return 0;
}

//------------------------------------------------------------------------------
/**
    Creates a depth sorted index array from the provided transformed vertex
    array using the helper data created in the Prepare() method. The 
    provided vertex array must contain the same vertices (in the same
    order!) which have been handed to the Prepare() method

    The current implementation is lazy and slow and just does a qsort() on 
    the transformed vertices.

    @param  vwr                 viewer matrix
    @param  vertices            pointer to the vertex array
    @param  numVerts            number of vertices in the array
    @param  stride4             stride of the vertex array in sizeof(float)
    @return                     pointer to array of sorted indices
*/
const ushort*
nVertexSorter::SortIndices(const matrix44& vwr, const float* vertices, int numVerts, int stride4)
{
    n_assert(this->sortedIndexArray);
    n_assert(this->numVertices = numVerts);

    // transform z coordinates into view space
    const float* ptr = vertices;
    int i = 0;
    for (; i < this->numVertices; i++, ptr += stride4)
    {
        this->zArray[i] = vwr.M13 * ptr[0] + vwr.M23 * ptr[1] + vwr.M33 * ptr[2] + vwr.M43;
    }

    // sort the index array
    nVertexSorter::tmpArrayPointer = this->zArray;
    qsort(this->sortedIndexArray, this->numVertices, sizeof(ushort), 
        (int(*)(const void *, const void *)) nVertexSorter::QSortHook);
    return this->sortedIndexArray;
}

//------------------------------------------------------------------------------





