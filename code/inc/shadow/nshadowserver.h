#ifndef N_SHADOWSERVER_H
#define N_SHADOWSERVER_H
//------------------------------------------------------------------------------
/**
    @class nShadowServer

    @brief Super class for specific shadow casting servers.

    A specific shadow server lives under "/sys/servers/shadow" and
    implements a specific shadow casting technique. When rendering a frame
    the shadow caster needs to be informed about light sources which can
    initiate shadows, and vertex buffers (static or dynamic) which can cast 
    shadows. Everything else should be hidden inside the shadow server.

    (C) 2001 RadonLabs GmbH
*/

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#undef N_DEFINES
#define N_DEFINES nShadowServer
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nLight;
class nShadowCaster;
class nShadowServer : public nRoot
{
public:
    /// el constructor
    nShadowServer();
    /// destructor
    virtual ~nShadowServer();

    /// turn shadow casting on/off
    void SetCastShadows(bool b);
    /// get shadow casting state
    bool GetCastShadows();
    /// set the shadow color
    void SetShadowColor(float r, float g, float b, float a);
    /// get the shadow color
    void GetShadowColor(float& r, float& g, float& b, float& a);

    /// begin a shadow casting scene
    virtual void BeginScene();
    /// attach a light source
    virtual void AttachLight(const matrix44& modelview, nLight* light);
    /// attach a shadow caster
    virtual void AttachCaster(const matrix44& modelview, nShadowCaster* caster);
    /// finish a shadow casting scene
    virtual void EndScene();

    /// create a shadow caster object
    virtual nShadowCaster* NewCaster(const char* name);
    /// find an existing shadow caster object
    virtual nShadowCaster* FindCaster(const char* name);

    static nKernelServer *kernelServer;

protected:
    /// get a unique resource id
    char *getResourceId(const char *name, char *buf, int bufSize);

    nRef<nRoot> refShadowCasters;       // (shared) shadow casters live here
    bool castShadows;
    int uniqueId;
    vector4 shadowColor;
};

//------------------------------------------------------------------------------
#endif
