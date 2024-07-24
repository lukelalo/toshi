#ifndef N_OVERLAYPLANE_H
#define N_OVERLAYPLANE_H
//--------------------------------------------------------------------
/**
    @class nOverlayPlane
    @ingroup NebulaVisnodeModule
    
    @brief Render an alpha overlay plane onto the screen.

    Renders a colored alpha overlay plane on screen. Depends
    on material and probably texture providing surface 
    attributes.

    Usually used exclusively by the special fx server.
*/
//--------------------------------------------------------------------
#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nOverlayPlane
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nGfxServer;
class nSpecialFxServer;
class N_DLLCLASS nOverlayPlane : public nVisNode {
    nAutoRef<nGfxServer>       ref_gs;
    nAutoRef<nSpecialFxServer> ref_sfx;
    nRef<nIndexBuffer>         ref_ibuf;
    nDynVertexBuffer           dyn_vb;
    matrix44 identity;
    vector4 color;

public:
    static nKernelServer *kernelServer;

    nOverlayPlane()
    : ref_gs(kernelServer,this),
      ref_sfx(kernelServer,this),
      ref_ibuf(this),
      dyn_vb(kernelServer,this)
    {
        this->ref_gs  = "/sys/servers/gfx";
        this->ref_sfx = "/sys/servers/specialfx";
    }
    virtual ~nOverlayPlane();
    virtual bool Attach(nSceneGraph2*);
    virtual void Compute(nSceneGraph2*);

    void SetColor(float r, float g, float b, float a) {
        color.set(r,g,b,a);
    };
    void GetColor(float& r, float& g, float& b, float& a) {
        r = this->color.x;
        g = this->color.y;
        b = this->color.z;
        a = this->color.w;
    };
    
private:
    void init_vbuffer(void);
};
//--------------------------------------------------------------------
#endif
