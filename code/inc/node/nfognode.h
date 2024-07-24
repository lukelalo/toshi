#ifndef N_FOGNODE_H
#define N_FOGNODE_H
//--------------------------------------------------------------------
/** 
    @class nFogNode
    @ingroup NebulaVisnodeModule
    
    @brief Define fog parameters for scene

    Works similar to an ambient lightsource. Describes fog 
    attributes for the current scene. If more then one fog node
    is rendered within the same frame, one will overwrite the other.
*/
//--------------------------------------------------------------------
#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif

#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nFogNode
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nGfxServer;
class nFogNode : public nVisNode {
    nAutoRef<nGfxServer> ref_gs; 
    nFogMode fog_mode;
    float fog_start;
    float fog_end;
    float fog_density;
    float fog_color[4];
    
public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nFogNode();
    virtual ~nFogNode();
    virtual bool SaveCmds(nPersistServer *);
    virtual bool Attach(nSceneGraph2 *);
    virtual void Compute(nSceneGraph2 *); 
    
    virtual void SetMode(nFogMode);
    virtual void SetRange(float, float);
    virtual void SetDensity(float);
    virtual void SetColor(float, float, float, float);

    virtual nFogMode GetMode(void);
    virtual void GetRange(float&, float&);
    virtual float GetDensity(void);
    virtual void GetColor(float&, float&, float&, float&);
};
//--------------------------------------------------------------------
#endif
