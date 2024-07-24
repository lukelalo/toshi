#ifndef N_SHADOWCONTROL_H
#define N_SHADOWCONTROL_H
//------------------------------------------------------------------------------
/**
    @class nShadowControl

    @brief Tune global shadow attributes from a nVisNode class.

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#undef N_DEFINES
#define N_DEFINES nShadowControl
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nShadowServer;
class nShadowControl : public nVisNode
{
public:
    /// constructor
    nShadowControl();
    /// destructor
    virtual ~nShadowControl();
    /// attach to parent as depend node
    virtual void Initialize();
    /// update state and render
    virtual void Compute(nSceneGraph2*);
    /// persistency
    virtual bool SaveCmds(nPersistServer* fileServer);

    /// set shadow color
    void SetColor(float r, float g, float b, float a);
    /// get shadow color
    void GetColor(float& r, float& g, float& b, float& a);

    static nKernelServer* kernelServer;

private:
    nAutoRef<nShadowServer> refShadowServer;
    vector4 color;
};
//------------------------------------------------------------------------------
#endif
