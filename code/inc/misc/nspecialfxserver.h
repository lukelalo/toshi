#ifndef N_SPECIALFXSERVER_H
#define N_SPECIALFXSERVER_H
//-------------------------------------------------------------------
/**
    @class nSpecialFxServer

    @brief central special fx generator

    nSpecialFxServer manages diverse global special fx that don't
    fit in anywhere else or are to specific to be included into
    other servers.
*/
//-------------------------------------------------------------------
#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nSpecialFxServer
#include "kernel/ndefdllclass.h"

//-------------------------------------------------------------------
class nVisNode;
class n3DNode;
class nSceneGraph2;
class N_DLLCLASS nSpecialFxServer : public nRoot {
    nAutoRef<nVisNode> ref_root;                // root node of special fx nodes

    // priority alpha overlay parameters
    bool ovr_activated;
    float ovr_red, ovr_green, ovr_blue, ovr_alpha;

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nSpecialFxServer();
    virtual ~nSpecialFxServer();
    virtual void Begin(void);
    virtual void End(nSceneGraph2 *sg);

    // the alpha overlay plane
    virtual void AddOverlay(float r, float g, float b, float a);
    virtual bool GetOverlay(float& r, float& g, float& b, float& a);
};
//-------------------------------------------------------------------
#endif
