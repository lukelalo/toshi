#ifndef N_VERTEXPOOLMANAGER_H
#define N_VERTEXPOOLMANAGER_H
//-------------------------------------------------------------------
/**
    @class nVertexPoolManager

    @brief Factory object for vertex bubbles of different vertex formats.

    Manages internal lists of vertex pools where the
    vertex bubbles live in. Creates vertex pools through a
    gfx server, since vertex pools are 3d API specific.

    Vertex pools are available in several commonly used
    vertex component combinations.
    Currently there are 20 possible vertex formats supported:

    - coords with 0..4 uv coordinate sets
    - coords+normals with 0..4 uv coordinate sets
    - coords+colors with 0..4 uv coordinate sets
    - coords+normals+colors with 0..4 uv coordinate sets

    Different vertex buffer types (static, read only, write only)
    should be handled by using 3 different vertex pool manager
    objects, each of them dedicated to a single type.
*/
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif

//-------------------------------------------------------------------
//  Code belongs to ngfxserver and needs to export interface
//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nGfxServer
#include "kernel/ndefdllclass.h"

//-------------------------------------------------------------------
class nGfxServer;
class nVertexBubble;
class N_PUBLIC nVertexPoolManager {

    enum {
        N_LISTARRAYSIZE = 5,
    };

    // vertex pool lists for each supported vertex format
    nList coord[N_LISTARRAYSIZE];               // coords only with 0..4 uv coordinates
    nList coord_norm[N_LISTARRAYSIZE];          // coords and norms with 0..4 uv coordinates
    nList coord_rgba[N_LISTARRAYSIZE];          // coords and rgbas with 0..4 uv coordinates
    nList coord_norm_rgba[N_LISTARRAYSIZE];     // coords, norms and rgbas with 0..4 uv coordinates
    nList coord_jw[N_LISTARRAYSIZE];            // as above, but with joint weights
    nList coord_norm_jw[N_LISTARRAYSIZE];       
    nList coord_rgba_jw[N_LISTARRAYSIZE];       
    nList coord_norm_rgba_jw[N_LISTARRAYSIZE];  

    nGfxServer *gs;    // the gfx server who owns this vertex pool (nVertexPool factory)
    nVBufType vbuf_type;        // the vertex buffer type for the pools
    int prefered_pool_size;     // the prefered vertex pool size

public:
    nVertexPoolManager(nGfxServer *_gs, nVBufType vbt, int pref_size)
        : gs(_gs),
          vbuf_type(vbt),
          prefered_pool_size(pref_size)
    {
        const char *vbt_str = "UNKNOWN";
        switch (vbt) {
            case N_VBTYPE_STATIC:       vbt_str = "N_VBTYPE_STATIC"; break;
            case N_VBTYPE_READONLY:     vbt_str = "N_VBTYPE_READONLY"; break;
            case N_VBTYPE_WRITEONLY:    vbt_str = "N_VBTYPE_WRITEONLY"; break;
            default: break;
        }
        n_printf("new nVertexPoolManager(type=%s,size=%d)\n",vbt_str,pref_size);
    } 
    ~nVertexPoolManager();

    nVertexBubble *NewVertexBubble(int vtype, int size);
    void ReleaseVertexBubble(nVertexBubble *);
    
    int GetPreferedPoolSize(void) {
        return this->prefered_pool_size;
    };

private:
    nList *map_vtype_2_list(int);
};

//-------------------------------------------------------------------
#endif
