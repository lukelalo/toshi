#ifndef N_OBSERVER_H
#define N_OBSERVER_H
//-------------------------------------------------------------------
/**
    @class nObserver

    @brief The nObserver object permits the structure of a more flexible and
    more powerful gfxserv in a normal nsh script.

    The nObserver implements a system for handling input, controlling
    the camera and a basic rendering loop.
*/
//-------------------------------------------------------------------
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_PRIMITIVESERVER_H
#include "gfx/nprimitiveserver.h"
#endif

//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nObserver
#include "kernel/ndefdllclass.h"
//-------------------------------------------------------------------
class nGfxServer;
class nInputServer;
class nConServer;
class nScriptServer;
class nParticleServer;
class nAudioServer2;
class nSceneGraph2;
class nChannelServer;
class nInputEvent;
class n3DNode;
class nSpecialFxServer;
class N_DLLCLASS nObserver : public nRoot {
protected:
    nAutoRef<nGfxServer>        ref_gs;
    nAutoRef<nInputServer>      ref_is;
    nAutoRef<nConServer>        ref_con;
    nAutoRef<nScriptServer>     ref_ss;
    nAutoRef<nSceneGraph2>      ref_sg;
    nAutoRef<nChannelServer>    ref_chn;
    nAutoRef<nParticleServer>   ref_ps;
    nAutoRef<nAudioServer2>     ref_as;
    nAutoRef<nSpecialFxServer>  ref_fx;
    nAutoRef<nPrimitiveServer>  ref_prim;
    nAutoRef<nRemoteServer>     ref_remoteServer;

    nAutoRef<n3DNode> ref_scene;
    nAutoRef<n3DNode> ref_camera;
    nAutoRef<n3DNode> ref_lookat;

    float sleep;
    bool grid;
    bool stop_requested;    

    int mouse_old_x, mouse_old_y;
    int mouse_cur_x, mouse_cur_y;
    int mouse_rel_x, mouse_rel_y;

    int timeChannel;
    int globalTimeChannel;

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nObserver();
    virtual ~nObserver();

    virtual void  SetSleep(float);
    virtual float GetSleep(void);
    virtual void  SetGrid(bool);
    virtual bool  GetGrid(void);

    virtual bool Start(void);
    virtual void Stop(void);

    virtual void StartSingleStep(void);
    virtual bool RenderSingleStep(void);

    void RenderFrame(float time);

private:
    bool trigger(void);
    void get_mouse_input(nInputServer *);
    void handle_input(void);
    void render_grid(nGfxServer *, matrix44&);
    void render_node(n3DNode *, matrix44&, float);
    void place_camera(matrix44&, matrix44&, float);
    void render_frame(float time);
};
//-------------------------------------------------------------------
#endif
