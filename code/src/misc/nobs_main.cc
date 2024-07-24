#define N_IMPLEMENTS nObserver
//-------------------------------------------------------------------
//  nobs_main.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "gfx/ngfxserver.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelserver.h"
#include "misc/nconserver.h"
#include "input/ninputserver.h"
#include "kernel/nscriptserver.h"
#include "node/n3dnode.h"
#include "misc/nobserver.h"
#include "misc/nparticleserver.h"
#include "misc/nspecialfxserver.h"
#include "audio/naudioserver2.h"
#include "kernel/nremoteserver.h"

//-------------------------------------------------------------------
/**
    - 20-Jul-99   floh    created
*/
//-------------------------------------------------------------------
nObserver::nObserver()
         :  ref_gs(ks, this),
            ref_is(ks, this),
            ref_ss(ks, this),
            ref_sg(ks, this),
            ref_chn(ks, this),
            ref_fx(ks, this),
            ref_con(ks, this),
            ref_ps(ks, this),
            ref_as(ks, this),
            ref_prim(ks, this),
            ref_scene(ks, this),
            ref_camera(ks, this),
            ref_lookat(ks, this),
            ref_remoteServer(ks, this)
{
    this->ref_gs     = "/sys/servers/gfx";
    this->ref_is     = "/sys/servers/input";
    this->ref_ss     = "/sys/servers/script";
    this->ref_sg     = "/sys/servers/sgraph2";
    this->ref_chn    = "/sys/servers/channel";
    this->ref_fx     = "/sys/servers/specialfx";
    this->ref_con    = "/sys/servers/console";
    this->ref_ps     = "/sys/servers/particle";
    this->ref_as     = "/sys/servers/audio";
    this->ref_fx     = "/sys/servers/specialfx";
    this->ref_prim   = "/sys/servers/primitive";
    this->ref_scene  = "/usr/scene";
    this->ref_camera = "/usr/camera";
    this->ref_lookat = "/usr/lookat";
    this->ref_remoteServer = "/sys/servers/remote";

    this->sleep = 0.02f;
    this->grid  = true;
    this->stop_requested = false;

    this->mouse_old_x = 0;
    this->mouse_old_y = 0;
    this->mouse_cur_x = 0;
    this->mouse_cur_y = 0;
    this->mouse_rel_x = 0;
    this->mouse_rel_y = 0;

    this->timeChannel       = 0;
    this->globalTimeChannel = 0;

    this->ref_remoteServer->Open("nebula");
}

//-------------------------------------------------------------------
/**
    - 20-Jul-99   floh    created
*/
//-------------------------------------------------------------------
nObserver::~nObserver()
{
    if (this->ref_remoteServer.isvalid())
    {
        this->ref_remoteServer->Close();
    }
}

//-------------------------------------------------------------------
/**
    @brief Set the amount of time to sleep between rendering each frame.

    This is usually used to allow other applications time to execute on
    the system if your application doesn't require a high framerate.

    - 20-Jul-99   floh    created
*/
//-------------------------------------------------------------------
void nObserver::SetSleep(float s)
{
    this->sleep = s;
}

//-------------------------------------------------------------------
/**
    @return the amount of time that is spent sleeping between rendering
    each frame.

    - 20-Jul-99   floh    created
*/
//-------------------------------------------------------------------
float nObserver::GetSleep(void)
{
    return this->sleep;
}

//-------------------------------------------------------------------
/**
    @brief Enable or disable the floor grid.
    
    This is only rendered under OpenGL.

    - 20-Jul-99   floh    created
*/
//-------------------------------------------------------------------
void nObserver::SetGrid(bool b)
{
    this->grid = b;
}

//-------------------------------------------------------------------
/**
    @return whether or not the grid is enabled.

    - 20-Jul-99   floh    created
*/
//-------------------------------------------------------------------
bool nObserver::GetGrid(void)
{
    return this->grid;
}

//-------------------------------------------------------------------
/**
    @brief Start the rendering loop.

    This method returns when one of the following happens:
     - The window is closed.
     - The 'exit' script command is executed.
     - The 'Stop' method on nObserver is called.

    - 20-Jul-99   floh    created
*/
//-------------------------------------------------------------------
bool nObserver::Start(void)
{
    ks->ts->EnableFrameTime();
    ks->ts->ResetTime();

    this->timeChannel       = this->ref_chn->GenChannel("time");
    this->globalTimeChannel = this->ref_chn->GenChannel("gtime");

    // If we have an audio server, start it.
    if (this->ref_as.isvalid())
    {
        this->ref_as->OpenAudio();
    }

    this->ref_scene->RenderContextCreated(0);
    while (this->trigger());
    this->ref_scene->RenderContextDestroyed(0);

    if (this->ref_as.isvalid())
    {
        this->ref_as->CloseAudio();
    }

    this->stop_requested = false;
    return true;
}

//-------------------------------------------------------------------
/**
    @brief Halt the rendering loop and return from the previous
    invocation of nObserver::Start().

    - 20-Jul-99   floh    created
*/
//-------------------------------------------------------------------
void nObserver::Stop(void)
{
    this->stop_requested = true;
    ks->ts->DisableFrameTime();
}

//-------------------------------------------------------------------
/**
    @brief Enter into single step rendering mode.

    See the scripting documentation for a usage example.

    - 19-Dec-99   floh    created
*/
//-------------------------------------------------------------------
void nObserver::StartSingleStep(void)
{
    ks->ts->ResetTime();

    this->timeChannel       = this->ref_chn->GenChannel("time");
    this->globalTimeChannel = this->ref_chn->GenChannel("gtime");

    this->stop_requested = false;
    
    this->ref_scene->RenderContextCreated(0);
}

//-------------------------------------------------------------------
/**
    @brief Render a single frame while in single step mode.

    - 19-Dec-99   floh    created
*/
//-------------------------------------------------------------------
bool nObserver::RenderSingleStep(void)
{
    bool success = this->trigger();
    if ( !success )
      this->ref_scene->RenderContextDestroyed(0);
    return success;
}

//-------------------------------------------------------------------
/**
    @brief Render the grid.  This will only show up under
    the OpenGL server.

    - 21-Jul-99   floh    created
*/
//-------------------------------------------------------------------
void nObserver::render_grid(nGfxServer *gs, matrix44& ivwr)
{
    if (this->ref_prim.isvalid())
    {
        nPrimitiveServer *prim = this->ref_prim.get();
        gs->SetMatrix(N_MXM_MODELVIEW, ivwr);
        prim->SetColor(0.5f,1.0f,0.5f,1.0f);
        prim->WirePlane(20,1);
    }
}

//-------------------------------------------------------------------
/**
    @brief Render the given node in the scenegraph.

    - 21-Jul-99   floh    created
    - 24-Nov-99   floh    + Support fuer gtime Channel
    - 18-Jun-01   floh    + converted to new sgraph/channel handling
*/
//-------------------------------------------------------------------
void nObserver::render_node(n3DNode *n, matrix44& m, float time)
{
    nSpecialFxServer *fx = NULL;
    if (this->ref_fx.isvalid()) 
    {
        fx = this->ref_fx.get();
    }

    this->ref_chn->SetChannel1f(this->timeChannel, time);
    this->ref_chn->SetChannel1f(this->globalTimeChannel, time);

    nSceneGraph2* sg = this->ref_sg.get();
    if (sg->BeginScene(m))
    {
        // trigger specialfx server
        if (fx) fx->Begin();
        sg->Attach(n, 0);
        if (fx) fx->End(sg);
        sg->EndScene(true);
    }
}

//-------------------------------------------------------------------
/**
    @brief Render a single frame.

    - 01-May-00   floh    created
*/
//-------------------------------------------------------------------
void nObserver::render_frame(float time)
{
    nGfxServer     *gs =  this->ref_gs.get();
    nConServer     *con = this->ref_con.get();
    nAudioServer2* as2 = (nAudioServer2*) ks->Lookup("/sys/servers/audio");

    this->handle_input();

    if (this->ref_as.isvalid())
    {
        this->ref_as->BeginScene(time);
    }

    // Rendering
    if (gs->BeginScene()) 
    {
        matrix44 ivwr;

        if (as2) as2->BeginScene(time);

        // trigger particle server (needs timestamp)
        if (this->ref_ps.isvalid()) 
        {
            this->ref_ps->Trigger();
        }
        
        ivwr = this->ref_camera->GetM();
        gs->SetMatrix(N_MXM_VIEWER, ivwr);
        ivwr.invert_simple();
        this->render_node(this->ref_scene.get(), ivwr, time);

        if (this->grid) this->render_grid(gs, ivwr);
        con->Render();
        
        if (as2) as2->EndScene();
        gs->EndScene();
        
        if (this->ref_as.isvalid())
        {
            this->ref_as->EndScene();
        }
        
        this->ref_is->FlushEvents();
        if (this->sleep > 0.0) n_sleep(this->sleep);
    }
}

//-------------------------------------------------------------------
/**
    Trigger the following servers:
      - GFX Server
      - Script Server
      - Kernel Server
      - Time Server
      - Input Server

    And then render a frame.
      
    - 20-Jul-99   floh    created
    - 20-Dec-99   floh    Console-Server wird jetzt mit im Input-
                          Server getriggert (Input-Recording!)
    - 26-Jan-00   floh    GetFrameTime()
    - 28-Apr-00   floh    no longer depends on particle server
*/
//-------------------------------------------------------------------
bool nObserver::trigger(void)
{
    // Trigger the servers
    if (this->stop_requested)     return false;
    if (!this->ref_gs->Trigger()) return false;
    if (!this->ref_ss->Trigger()) return false;
    this->ref_remoteServer->Trigger();

    ks->Trigger();
    ks->ts->Trigger();
    double time = ks->ts->GetFrameTime();
    this->ref_is->Trigger(time);
    this->render_frame((float)time);
    return true;
}

//-------------------------------------------------------------------
/**
    @brief Render a single frame independently from render loop.
    
    Useful if display needs to be refreshed "at once" inside a
    script callback procedure.
  
    - 01-May-00   floh    created
*/
//-------------------------------------------------------------------
void nObserver::RenderFrame(float time)
{
    this->render_frame(time);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

