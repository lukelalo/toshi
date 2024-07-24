#ifndef N_VERTEXBUBBLE_H
#define N_VERTEXBUBBLE_H
//-------------------------------------------------------------------
/**
    @class nVertexBubble

    @brief defines continous chunk of vertices in a nVertexPool
*/
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_NODE_H
#include "util/nnode.h"
#endif

//--------------------------------------------------------------------
class nVertexPool;
class nVertexBubble : public nNode {
    int start;           // index of first vertex
    int size;            // number of vertices
    int trunc;           // optional truncated size

public:
    nVertexBubble(nVertexPool *pool, int _start, int _size) {
        n_assert(pool);
        n_assert(_start>=0);
        n_assert(_size>0);
        this->SetPtr(pool);
        start = _start;
        size  = _size;
        trunc = _size;
    };

    void Set(int _start, int _size) {
        n_assert(_start>=0);
        n_assert(_size>0);
        start = _start;
        size  = _size;
        trunc = _size;
    };

    nVertexPool *GetVertexPool(void) {
        return (nVertexPool *) this->GetPtr();
    };

    int GetStart(void) {
        return start;
    };
    int GetBubbleSize(void) {
        return size;
    };
    int GetTruncSize(void) {
        return trunc;
    };

    void Truncate(int t) {
        n_assert(t <= size);
        trunc = t;
    };
    void Untruncate(void) {
        trunc = size;
    };
};
//--------------------------------------------------------------------
#endif
