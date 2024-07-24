#define N_IMPLEMENTS nGame

//------------------------------------------------------------------------------
//  Implementación de la clase nGame
//------------------------------------------------------------------------------
#include "kernel/ntimeserver.h"
#include "kernel/nscriptserver.h"
#include "gfx/ngfxserver.h"
#include "input/ninputserver.h"
#include "misc/nconserver.h"
#include "gfx/nscenegraph2.h"
#include "misc/nspecialfxserver.h"
#include "node/n3dnode.h"
#include "laberintosultan/ngame.h"
#include "audio/naudioserver2.h"
#include "gfx/nchannelserver.h"
#include "laberintosultan/nia.h"
#include "laberintosultan/nworld.h"
#include "laberintosultan/nentity.h"
#include "laberintosultan/ncamera.h"
#include "collide/ncollideobject.h"
#include "collide/ncollideserver.h"

nNebulaScriptClass(nGame, "nroot");

//------------------------------------------------------------------------------
nGame::nGame() :
    scriptServer(kernelServer, this),
    gfxServer(kernelServer, this),
    inputServer(kernelServer, this),
    consoleServer(kernelServer, this),
    sceneGraph(kernelServer, this),
    specialfxServer(kernelServer, this),
    renderScene(kernelServer, this),
	ref_camera(kernelServer, this),
	ref_lookat(kernelServer, this),
	ref_prim(kernelServer, this),
    channelServer(kernelServer, this),
	ias(kernelServer,this),
	world(kernelServer,this),
	renderCamera(kernelServer,this),
	stop(false) 
	
{
    scriptServer = "/sys/servers/script";
    gfxServer = "/sys/servers/gfx";
    inputServer = "/sys/servers/input";
    consoleServer = "/sys/servers/console";
    sceneGraph = "/sys/servers/sgraph2";
    specialfxServer = "/sys/servers/specialfx";
	channelServer = "/sys/servers/channel";
	renderScene = "/usr/scene";
	iaPath = "";
	

    this->ref_camera = "/usr/camera";
    this->ref_lookat = "/usr/lookat";
    this->ref_prim   = "/sys/servers/primitive";

    this->mouse_old_x = 0;
    this->mouse_old_y = 0;
    this->mouse_cur_x = 0;
    this->mouse_cur_y = 0;
    this->mouse_rel_x = 0;
    this->mouse_rel_y = 0;
}

//------------------------------------------------------------------------------
/**
*/
nGame::~nGame()
{
}


void nGame::Init(const char *script_path) {

    const char* result = NULL;
    scriptServer->RunScript(script_path, result);
}

/**
    @brief Main game loop.

    Main loop where everything happens.
*/
void nGame::Run() {
    kernelServer->ts->EnableFrameTime();
    kernelServer->ts->ResetTime();
    kernelServer->ts->Trigger();
	as2 = (nAudioServer2*) kernelServer->Lookup("/sys/servers/audio");

    timeChannel       = channelServer->GenChannel("time");
    globalTimeChannel = channelServer->GenChannel("gtime");

    // The game loop
    while (true == Trigger())
    {
        kernelServer->Trigger();

        // Find the time elapsed since last frame
        double last_time = kernelServer->ts->GetFrameTime();
        kernelServer->ts->Trigger();
        double dt = kernelServer->ts->GetFrameTime() - last_time;

        // Trigger input server, also triggers the console server
        inputServer->Trigger(kernelServer->ts->GetFrameTime());

		channelServer->SetChannel1f(timeChannel, (float)kernelServer->ts->GetFrameTime());
		channelServer->SetChannel1f(globalTimeChannel, (float)kernelServer->ts->GetFrameTime());

#       ifdef USA_CAMARA_CON_RATON
		//handle_input();
#       endif

        // Trigger AI.
		if (ias.isvalid()) {
			// Asumimos que todos los descendientes de ias
			// son nIAs. Si no, la aplicación fallará...
			nIA *ia = NULL;
			for (ia = (nIA*)ias->GetHead(); ia != NULL; ia = (nIA*)ia->GetSucc()) {
				n_assert(ia->IsInstanceOf(kernelServer->FindClass("nia")));
				ia->Trigger();
			}
		}

		// Simular mundo
		float t;
		if (world.isvalid()) {
			t=(float)kernelServer->ts->GetFrameTime();
			world->Run(t);
			renderCamera->Update(t);
		}

        // Render frame ( grafico + audio)
		// render audio
		if (as2)
			as2->BeginScene(last_time);
        Render();
		if(as2)
			as2->EndScene();

		inputServer->FlushEvents();
        // Sleep to save the OS from certain tax death
        n_sleep(0.02f);
    }
}

void nGame::Stop()
{
    stop = true;
}

/**
    Trigger bunch of servers, should really do loop inside here
*/
bool nGame::Trigger()
{
    if (true == stop)
        return false;

    // Run window message pump
    if (false == gfxServer->Trigger())
        return false;

    // Run script stuff?
    if (false == scriptServer->Trigger())
        return false;

    return true;
}

/**
    @brief Render current scene.

    :TODO: Put this into a render context?
    How about occlusion?
*/

void nGame::Render()
{

    if (false == renderCamera.isvalid()) {
		if (gfxServer->BeginScene()) {
		    consoleServer->Render();
		    gfxServer->EndScene();
		}
        return;
	}

    if (gfxServer->BeginScene()) {
	    gfxServer->SetMatrix(N_MXM_VIEWER, const_cast<matrix44&>(renderCamera->GetTransform()));
		/*
		
		matrix44 ivwr;

		ivwr = this->ref_camera->GetM();
		gfxServer->SetMatrix(N_MXM_VIEWER, ivwr);
		ivwr.invert_simple();

		
		// render_node(renderScene.get(),ivwr);
		// render_grid(gfxServer.get(),ivwr);

		sceneGraph->BeginScene(ivwr);
		*/

		matrix44 inverted_matrix = renderCamera->GetTransform();
		inverted_matrix.invert_simple();
		sceneGraph->BeginScene(inverted_matrix);

		// Dibujamos la escena
		if (renderScene.isvalid())
			sceneGraph->Attach(renderScene.get(), 0);

		// Dibujamos todas las entidades.
		if (this->world.isvalid()) {

			// The render context needs to be different for certain subsystem
			// visnodes which get attached to the scene multiple times.  One
			// of these is the particle system.

			int render_context = 0;
			nEntity* entity = (nEntity*)this->world->GetHead();
			this->sceneGraph->Attach(this->world->GetTerrainVisNode(),0);
			while (entity != NULL)
			{
				n3DNode* node = entity->Attach();
				if (node != NULL) {
					sceneGraph->Attach(node, render_context);
					++render_context;
				}
				entity = (nEntity*)entity->GetSucc();
			}
		}
		
		sceneGraph->EndScene(true);
	
		if (consoleServer.isvalid())
			consoleServer->Render();
		gfxServer->EndScene();
	}
}

/*
void nGame::Render()
{

    if (false == renderCamera.isvalid()) {
		if (gfxServer->BeginScene()) {
		    consoleServer->Render();
		    gfxServer->EndScene();
		}
        return;
	}
    if (gfxServer->BeginScene()) {
	    gfxServer->SetMatrix(N_MXM_VIEWER, const_cast<matrix44&>(renderCamera->GetTransform()));

		matrix44 inverted_matrix = renderCamera->GetTransform();
		inverted_matrix.invert_simple();
		sceneGraph->BeginScene(inverted_matrix);

		// Dibujamos la escena
		if (renderScene.isvalid())
			sceneGraph->Attach(renderScene.get(), 0);

		// Dibujamos todas las entidades.
		if (this->world.isvalid()) {

			this->sceneGraph->Attach(this->world->GetTerrainVisNode(), 0);

			// The render context needs to be different for certain subsystem
			// visnodes which get attached to the scene multiple times.  One
			// of these is the particle system.
			int render_context = 0;
			nEntity* entity = (nEntity*)this->world->GetHead();
			while (entity != NULL)
			{
				n3DNode* node = entity->Attach();
				if (node != NULL) {
					sceneGraph->Attach(node, render_context);
					++render_context;
				}
				entity = (nEntity*)entity->GetSucc();
			}
		}

		sceneGraph->EndScene(true);
		if (consoleServer.isvalid())
			consoleServer->Render();
		gfxServer->EndScene();

	}
}
*/
void nGame::render_grid(nGfxServer *gs, matrix44& ivwr)
{
    if (this->ref_prim.isvalid())
    {
        nPrimitiveServer *prim = this->ref_prim.get();
        gs->SetMatrix(N_MXM_MODELVIEW, ivwr);
        prim->SetColor(0.5f,1.0f,0.5f,1.0f);
        prim->WirePlane(20,1);
    }
}

void nGame::render_node(n3DNode *n, matrix44& m)
{

    nSceneGraph2* sg = sceneGraph.get();
    if (sg->BeginScene(m))
    {
        sg->Attach(n, 0);
        sg->EndScene(true);
    }
}

//-------------------------------------------------------------------
/**
    @brief Update the values for mouse_act_x, mouse_act_y, mouse_rel_x,
    mouse_rel_y.

    - 26-Jul-99   floh    created
*/
//-------------------------------------------------------------------
void nGame::get_mouse_input(nInputServer *is)
{
    nInputEvent *ie;
    if ((ie = is->FirstEvent())) do {
        if (ie->GetType() == N_INPUT_MOUSE_MOVE) {
            mouse_cur_x = ie->GetAbsXPos();
            mouse_cur_y = ie->GetAbsYPos();
        }
    } while ((ie = is->NextEvent(ie)));
    if (mouse_old_x != 0) mouse_rel_x = mouse_cur_x - mouse_old_x;
    if (mouse_old_y != 0) mouse_rel_y = mouse_cur_y - mouse_old_y;
    mouse_old_x = mouse_cur_x;
    mouse_old_y = mouse_cur_y;
}

//-------------------------------------------------------------------
/**
    @brief Place and orient the camera relative to the lookat node,
    @c look.

    The rotation matrix is copied, and the position is placed along
    the Z-axis at distance @c dist.

    - 26-Jul-99   floh    created
*/
//-------------------------------------------------------------------
void nGame::place_camera(matrix44& cam, matrix44& look, float dist)
{
    cam = look;
    cam.M41 = look.M41 + (look.M31 * dist);
    cam.M42 = look.M42 + (look.M32 * dist);
    cam.M43 = look.M43 + (look.M33 * dist);
}

//-------------------------------------------------------------------
/**
    - 21-Jul-99   floh    created
    - 30-Jul-99   floh    + Kanaele umbenannt: move -> pan, zoom -> dolly
                          + diverse Richtungen invertiert, damit identisch
                            mit Maya
*/
//-------------------------------------------------------------------
void nGame::handle_input(void)
{
    nInputServer *is = this->inputServer.get();
    matrix44 cam,look,m;
    vector3 v;

    // Determine the camera movement
    bool orbit  = is->GetButton("orbit");
    bool pan    = is->GetButton("pan");
    bool dolly  = is->GetButton("dolly");

    this->get_mouse_input(is);    
    cam  = this->ref_camera->GetM();
    look = this->ref_lookat->GetM();

    // Determine the distance between camera and lookat.
    vector3 c(cam.pos_component());
    vector3 l(look.pos_component());
    vector3 d = c - l;
    float dist  = d.len();

    // Translation shifts only lookat, because the camera is
    // placed again anyway.
    if (pan) {
        v.set(0.0f,0.0f,0.0f);
        v.x = -((float)this->mouse_rel_x) * 0.025f;
        v.y = +((float)this->mouse_rel_y) * 0.025f;
        m.ident();
        m.translate(v);
        m.mult_simple(look);
        look = m;
    }

    // Orbiting, dazu wird ebenfalls nur Lookat rotiert,
    // und die Kamera erst spaeter ausgerichtet
    if (orbit) {
        v.set(0.0f,0.0f,0.0f);
        v.x = -((float)this->mouse_rel_y) * 0.005f;
        v.y = -((float)this->mouse_rel_x) * 0.005f;
        if (v.y != 0.0f) {
            m.ident();
            m.rotate_y(v.y);
            vector3 bu(look.M41,look.M42,look.M43);
            look.M41=0.0f; look.M42=0.0f; look.M43=0.0f;
            look.mult_simple(m);
            look.M41=bu.x; look.M42=bu.y; look.M43=bu.z;
        }
        if (v.x != 0.0f) {
            m.ident();
            m.rotate_x(v.x);
            m.mult_simple(look);
            look = m;
        }
    }

    // Zoom shot, vertical mouse zoomed.
    if (dolly) {
        v.set(0.0f,0.0f,0.0f);
        v.z = -((float)this->mouse_rel_y) * 0.025f;
        dist += v.z;
        if (dist < 0.01f) dist=0.01f;
    }

    // Place the camera...
    this->place_camera(cam,look,dist);

    // Write back the resulting matrices.
    this->ref_camera->M(cam);
    this->ref_lookat->M(look);
}

void nGame::SetIA(const char*ia_path){
	iaPath=ia_path;
	ias=iaPath.Get();
}
void nGame::SetScene(const char*scene_path) {
	n_assert(scene_path);
	this->renderScene=scene_path;
}

void nGame::SetWorld(const char*world_path) {
	n_assert(world_path);
	this->world=world_path;
}

void nGame::SetCamera(const char*camera_path) {
	n_assert(camera_path);
	this->renderCamera=camera_path;
}