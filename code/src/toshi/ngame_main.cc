#define N_IMPLEMENTS nGame
#define redondea(a)  a-double(long(a))>0.5?ceil(a):floor(a)

//------------------------------------------------------------------------------
//  Implementación de la clase nGame
//
//  27/6/05 Revisado y comentado por Pablo
//
//------------------------------------------------------------------------------
#include "kernel/ntimeserver.h"
#include "kernel/nscriptserver.h"
#include "gfx/ngfxserver.h"
#include "input/ninputserver.h"
#include "input/ninputevent.h"
#include "misc/nconserver.h"
#include "gfx/nscenegraph2.h"
#include "misc/nspecialfxserver.h"
#include "node/n3dnode.h"
#include "toshi/ngame.h"
#include "map/nmapnode.h"
#include "map/nmap.h"
#include "audio/naudioserver2.h"
#include "gfx/nchannelserver.h"
#include "toshi/nia.h"
#include "toshi/nnet.h"
#include "toshi/nworld.h"
#include "toshi/nentity.h"
#include "toshi/nmenu.h"
#include "toshi/nvideo.h"
#include "toshi/nayuntamiento.h"
#include "toshi/nconstruccion.h"
#include "toshi/ncasaplanos.h"
#include "toshi/ncasamoneda.h"
#include "toshi/nmapa.h"
#include "toshi/njugador.h"
#include "toshi/ncamera.h"
#include "toshi/npartida.h"
#include "toshi/ncampesino.h"
#include "toshi/fmod.h"
#include "collide/ncollideobject.h"
#include "collide/ncollideserver.h"
#include <string>

nNebulaScriptClass(nGame, "nroot");

#define MAX_TIEMPO_PASADA 0.01
#define MAX_VUELTAS 10
#define TIEMPO_IA 2.0


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
	channelServer(kernelServer, this),
	ias(kernelServer,this),
    nets(kernelServer, this),
	menus(kernelServer, this),
	videos(kernelServer, this),
	world(kernelServer,this),
	renderCamera(kernelServer,this),
	stop(false),
	pausa(false),
	iniciado(false),
	JuegoEnRed(false),
	EsServidor(false),
	AyudaActiva(true),
	MostrandoInformacion(false),
	mostrarRender(true)
	
{
    scriptServer = "/sys/servers/script";
    gfxServer = "/sys/servers/gfx";
    inputServer = "/sys/servers/input";
    consoleServer = "/sys/servers/console";
    sceneGraph = "/sys/servers/sgraph2";
    specialfxServer = "/sys/servers/specialfx";
	channelServer = "/sys/servers/channel";
	renderScene = "/game/scene";
	iaPath = "";
	netPath = "";
	menuPath = "";
	videoPath = "";
	for (int i=0;i<8;i++)
	{
		canalMusica[i]=-1;
		canalSonido[i]=-1;
		p_cancion[i]=NULL;
		p_sonido[i]=NULL;
		volumenMusica[i]=255;
		volumenSonido[i]=255;
	}
	nEnv *hwnd=(nEnv *) kernelServer->Lookup("/sys/env/hwnd");
	FSOUND_SetHWND(hwnd->GetPtr());

	FSOUND_Init(44100,16,0x0000);
	jugador=NULL;
	TiempoIA=0.0f;
	menuSeleccionado = NULL;
	this->partida_actual= NULL;

    this->ref_camera = "/game/camera";
    this->ref_lookat = "/game/lookat";

    this->mouse_old_x = 0;
    this->mouse_old_y = 0;
    this->mouse_cur_x = 0;
    this->mouse_cur_y = 0;
    this->mouse_rel_x = 0;
    this->mouse_rel_y = 0;
	this->g_rueda= 0.0f;
	this->altura_camara=4.0f;
	
	
}

//------------------------------------------------------------------------------
/**
*/
nGame::~nGame()
{
	// Liberamos el puntero de la partida
	n_free(partida_actual);

	// Cerramos FMOD
	FSOUND_Close();
}


/**
    @brief Carga de Script Inicial

    Carga el script que se ejecutará inicialmente
*/
void nGame::Init(const char *script_path) {

    const char* result = NULL;
    scriptServer->RunScript(script_path, result);
}

/**
    @brief Main game loop.

    Main loop where everything happens.
*/
void nGame::Run() {

	// Inicializamos el servidor de tiempo    
	kernelServer->ts->EnableFrameTime();
	kernelServer->ts->ResetTime();
	kernelServer->ts->Trigger();

	//as2 = (nAudioServer2*) kernelServer->Lookup("/sys/servers/audio");

	timeChannel       = channelServer->GenChannel("time");
	globalTimeChannel = channelServer->GenChannel("gtime");
	int numeroVueltas=0;
	float t=0.0f;

	// BUCLE MODIFICADO

	while (true == Trigger())
	{
		kernelServer->Trigger();

		// Find the time elapsed since last frame
		double last_time = kernelServer->ts->GetFrameTime();
		kernelServer->ts->Trigger();
		float dt = kernelServer->ts->GetFrameTime() - last_time;

		// Trigger input server, also triggers the console server
		inputServer->Trigger(kernelServer->ts->GetFrameTime());
		float tiempo=channelServer->GetChannel1f(timeChannel);
		channelServer->SetChannel1f(timeChannel, tiempo+dt);
		channelServer->SetChannel1f(globalTimeChannel, (float)kernelServer->ts->GetFrameTime());
		numeroVueltas=0;

		// MANEJAMOS LA ENTRADA
		handle_input();
	
		this->menuSeleccionado=NULL;
		// EJECUTAMOS LOS MENUS
		if (menus.isvalid()) {
			nMenu *menu = NULL;
			for (menu = (nMenu*)menus->GetHead(); menu != NULL; menu = (nMenu*)menu->GetSucc()) 
			{
				t=(float)kernelServer->ts->GetFrameTime();
				menu->Tick(t);
				if (menu->GetActivo()) 
				{	
					this->menuSeleccionado=menu;
					this->CambiarCursor();
				}
			}
		}

		// EJECUTAMOS LOS VIDEOS
		if (videos.isvalid()) {
			nVideo *video = NULL;
			for (video = (nVideo*)videos->GetHead(); video != NULL; video = (nVideo*)video->GetSucc()) {
					t=(float)kernelServer->ts->GetFrameTime();
					video->Tick(t);
			}
		}

		// EJECUTAMOS LA RED
		if (nets.isvalid()){
			// Asumimos que todos los descendientes de net
			// son nNets. Si no, la aplicación fallará...
			nNet *net = NULL;
			for (net = (nNet*)nets->GetHead(); net != NULL; net = (nNet*)net->GetSucc()) {
				nNet* no = (nNet*)net;
				no->Trigger();
			}
		}

		while (dt > MAX_TIEMPO_PASADA && numeroVueltas < MAX_VUELTAS) {

			if (!this->JuegoEnRed && this->iniciado)
			{
				if (kernelServer->ts->GetFrameTime()>TiempoIA)
				{
					TiempoIA=kernelServer->ts->GetFrameTime()+TIEMPO_IA;
					// EJECUTAMOS LA IA
					if (ias.isvalid()) 
					{
						// Asumimos que todos los descendientes de ias
						// son nIAs. Si no, la aplicación fallará...
						nIA *ia = NULL;
						for (ia = (nIA*)ias->GetHead(); ia != NULL; ia = (nIA*)ia->GetSucc()) 
						{
								//n_assert(ia->IsInstanceOf(kernelServer->FindClass("nia")));
								ia->Trigger();
						}
					}
				}
			}

			// EJECUTAMOS LA LÓGICA DEL MUNDO
			if (partida_actual!=NULL)
			{
				if (world.isvalid()) {
					t=(float)kernelServer->ts->GetFrameTime();
					if (this->iniciado)
						partida_actual->Tick(t);
					this->CambiarCursor();

					if (renderCamera.isvalid()) 
					{
						t=(float)kernelServer->ts->GetFrameTime();
						renderCamera->Update(t);
						vector3 v_posicion,v_pitch;
						if (renderCamera->GetTarget()!=NULL)
						{
							v_posicion=renderCamera->GetTarget()->GetPosition();
							v_pitch=renderCamera->GetTarget()->GetVelocityVector();
							v_pitch.norm();
						}
						else
						{
							v_posicion.set(0,0,0);
							v_pitch.set(0,1,0);
						}
						float listenerPos[3]={v_posicion.x,v_posicion.y,v_posicion.z};
						FSOUND_3D_Listener_SetAttributes(&listenerPos[0],NULL,v_pitch.x,v_pitch.y,v_pitch.z,1,0,0);
					}
					else
					{
						float listenerPos[3]={0,0,0};
						FSOUND_3D_Listener_SetAttributes(&listenerPos[0],NULL,0,1,0,1,0,0);
					}

					t=(float)kernelServer->ts->GetFrameTime();
					world->Run(t);
				}
			}
			numeroVueltas++;
		}

		inputServer->FlushEvents();	
		Update();
	}

	gfxServer->Release();
	for (int i=0;i<8;i++)
	{
		StopSong(i);
	}
}

/********************************************************************************/
/* Método      : SetIniciado												    */
/* Descripción : Pone el flag iniciado (que marca el comienzo del movimiento	*/
/*				 del jugador y la IA a true. Si es false libera la memoria		*/
/*				 de la partida													*/
/* Parámetros  : bool p_iniciado. Valor para el flag					 		*/
/* Devuelve    : 																*/
/********************************************************************************/
void nGame::SetIniciado(bool p_iniciado)
{
	this->iniciado=p_iniciado;
	if (!this->iniciado)
	{
		n_free ( partida_actual );
		partida_actual=NULL;
	}
}

/********************************************************************************/
/* Método      : Pause														    */
/* Descripción : Pausa la partida												*/
/* Parámetros  : bool p_pausa. Valor para el flag					 			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nGame::Pause(bool p_pausa)
{
	this->pausa=p_pausa;
	if (p_pausa)
	{
		kernelServer->ts->StopTime();
	}
	else
	{
		inputServer->FlushEvents();
		kernelServer->ts->StartTime();
	}
}

/********************************************************************************/
/* Método      : Mute														    */
/* Descripción : Realiza un mute en el sonido									*/
/* Parámetros  : bool p_apagar. Valor para el flag					 			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nGame::Mute(bool p_apagar)
{
	if (as2) 
	{
		if (p_apagar)
				as2->SetMasterVolume(0.0f);
		else
				as2->SetMasterVolume(1.0f);
	}
}

/********************************************************************************/
/* Método      : Update														    */
/* Descripción : Muestra los gráficos y el sonido								*/
/* Parámetros  :													 			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nGame::Update() 
{
	double t=kernelServer->ts->GetFrameTime();
	// Actualizamos los gráficos y el sonido
	Render();
	FSOUND_Update();
	// Sleep to save the OS from certain tax death
	// n_sleep(0.01f);
}

/********************************************************************************/
/* Método      : Stop														    */
/* Descripción : Para el juego													*/
/* Parámetros  :													 			*/
/* Devuelve    : 																*/
/********************************************************************************/
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

/********************************************************************************/
/* Método      : Render														    */
/* Descripción : Muestra los gráficos											*/
/* Parámetros  :													 			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nGame::Render()
{
	if (this->mostrarRender) {
		if (gfxServer->BeginScene()) {
			if ( renderCamera.isvalid() )
			{
				gfxServer->SetMatrix(N_MXM_VIEWER, const_cast<matrix44&>(renderCamera->GetTransform()));

				matrix44 inverted_matrix = renderCamera->GetTransform();
				inverted_matrix.invert_simple();

				sceneGraph->BeginScene(inverted_matrix);
			}
			else if ( ref_camera.isvalid() )
			{
				matrix44 ivwr = this->ref_camera->GetM();
				gfxServer->SetMatrix(N_MXM_VIEWER, ivwr);
				ivwr.invert_simple();

				sceneGraph->BeginScene(ivwr);
			}
			else 
			{
				if (gfxServer->BeginScene()) 
				{
					consoleServer->Render();
					gfxServer->EndScene();
				}
				return;
			}

			// Dibujamos la escena
			if (renderScene.isvalid())
				sceneGraph->Attach(renderScene.get(), 0);
			// Dibujamos todas las entidades.
			if (this->world.isvalid()) {
				//this->world->GetCollideContext()->Visualize(this->gfxServer.get());
				if (this->world->GetTerrainVisNode())
					this->sceneGraph->Attach(this->world->GetTerrainVisNode(),0);

				// The render context needs to be different for certain subsystem
				// visnodes which get attached to the scene multiple times.  One
				// of these is the particle system.
				int render_context = 0;
				nEntity* entity = (nEntity*)this->world->GetHead();
				while (entity != NULL)
				{
					if	( ( !pausa || 
						 strcmp(entity->GetClass()->GetName(),"ncasaplanos") == 0	||
						 strcmp(entity->GetClass()->GetName(),"ncasamoneda") == 0	||
						 strcmp(entity->GetClass()->GetName(),"nayuntamiento") == 0 ||
						 strcmp(entity->GetClass()->GetName(),"nconstruccion") == 0 )
						) 
					{
						n3DNode* node = entity->Attach();
						if (node != NULL) {
							sceneGraph->Attach(node, render_context);
							++render_context;
						}
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
	float rueda=0.0f;
    nInputEvent *ie;
    if ((ie = is->FirstEvent())) do {
		if (ie->GetType() == N_INPUT_MOUSE_WHEEL) {
            rueda = ie->GetAxisValue();
        }
        if (ie->GetType() == N_INPUT_MOUSE_MOVE) {
            mouse_cur_x = ie->GetAbsXPos();
            mouse_cur_y = ie->GetAbsYPos();
			g_rueda = 0.0f;
        }

    } while ((ie = is->NextEvent(ie)));
    if (mouse_old_x != 0) mouse_rel_x = mouse_cur_x - mouse_old_x;
    if (mouse_old_y != 0) mouse_rel_y = mouse_cur_y - mouse_old_y;
    mouse_old_x = mouse_cur_x;
    mouse_old_y = mouse_cur_y;
	g_rueda=rueda;
}

/********************************************************************************/
/* Método      : GetPosMouseWorld											    */
/* Descripción : Devuelve la posición del ratón en el terreno (en valores 0..1)	*/
/* Parámetros  :													 			*/
/* Devuelve    : vector3																*/
/********************************************************************************/
vector3 nGame::GetPosMouseWorld() {
	matrix44 cam;
	matrix44 look;
	int x0, y0, x1, y1;
	float ratio,fov,fnear,ffar;
	//cam=this->ref_camera->GetM();
	//look=this->ref_lookat->GetM();
	cam=this->renderCamera->GetTransform();
	//look=this->renderCamera->GetTransform();
	gfxServer->GetDisplayDesc(x0, y0, x1, y1);
	float rayLength = 100.0f;
	float mouseX=mouse_cur_x/(float)x1;
	float mouseY=mouse_cur_y/(float)y1;
	
    // These are based upon the angle of the field of view and
    // set up here for a 90 degree field of view.
	
	gfxServer->GetPerspective(fov,ratio,fnear,ffar);
    float frustumScaleX = 1/ratio*1.32;
    float frustumScaleY = 1/ratio*0.98;

    vector3 to;
	
    to.x = (rayLength * frustumScaleX * (2.0 * mouseX - 1.0));
    to.y = (rayLength * frustumScaleY * (1.0 - 2.0 * mouseY));
    to.z = -rayLength;

    // Create pick ray
	to = cam.transform_coord(to);
    vector3 from(cam.pos_component());
	vector3 v;
	line3 line(from, to);
	world->GetMapNode()->GetMap()->GetIntersect(line,v);
	return v;
}

/********************************************************************************/
/* Método      : GetEntMouseWorld											    */
/* Descripción : Devuelve la primera entidad seleccionada por el ratón y NULL	*/
/*				 si no existe ninguna											*/
/* Parámetros  :													 			*/
/* Devuelve    : nEntity *														*/
/********************************************************************************/
nEntity *nGame::GetEntMouseWorld() {
	matrix44 cam;
	matrix44 look;
	int x0, y0, x1, y1;
	float ratio,fov,fnear,ffar;
	//cam=this->ref_camera->GetM();
	//look=this->ref_lookat->GetM();
	cam=this->renderCamera->GetTransform();
	//look=this->renderCamera->GetTransform();
	gfxServer->GetDisplayDesc(x0, y0, x1, y1);
	float rayLength = 100.0f;
	float mouseX=mouse_cur_x/(float)x1;
	float mouseY=mouse_cur_y/(float)y1;
	
    // These are based upon the angle of the field of view and
    // set up here for a 90 degree field of view.
	
	gfxServer->GetPerspective(fov,ratio,fnear,ffar);
    float frustumScaleX = 1/ratio*1.32;
    float frustumScaleY = 1/ratio*0.98;

    vector3 to;
	
    to.x = (rayLength * frustumScaleX * (2.0 * mouseX - 1.0));
    to.y = (rayLength * frustumScaleY * (1.0 - 2.0 * mouseY));
    to.z = -rayLength;

    // Create pick ray
	to = cam.transform_coord(to);
    vector3 from(cam.pos_component());
    line3 line(from, to);

	nCollideReport** pick_report = NULL;
	nCollideContext *collideContext=world->GetCollideContext();
	nEntity *entidad = NULL;
	int num_picks = collideContext->LineCheck(line, COLLTYPE_CONTACT,
                                              COLLCLASS_ALWAYS_EXACT,
                                              pick_report);

    if (num_picks > 0)
    {
        entidad=(nEntity *)pick_report[0]->co1->GetClientData();
		if (entidad->GetVisible())
			return entidad;
		else
			return NULL;
    }
	else
		return NULL;
}

/********************************************************************************/
/* Método      : GetPosMouse												    */
/* Descripción : Devuelve la posición del ratón en la pantalla en rango (0..1)	*/
/* Parámetros  :													 			*/
/* Devuelve    : vector3														*/
/********************************************************************************/
vector3 nGame::GetPosMouse() {
	int x0, y0, x1, y1;
	gfxServer->GetDisplayDesc(x0, y0, x1, y1);
	float mouseX=mouse_cur_x/(float)x1;
	float mouseY=mouse_cur_y/(float)y1;
	vector3 v(mouseX,mouseY,0);
	return v;
}

/********************************************************************************/
/* Método      : CambiarCursor												    */
/* Descripción : Modifica el cursor del ratón según en qué parte de la			*/
/*				 pantalla se encuentre											*/
/* Parámetros  :													 			*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::CambiarCursor()
{
	nString clase;
	vector3 posicion=this->GetPosMouse();
	nEntity *entidad;
	char *params;
	char buf[N_MAXPATH];
	// En caso de que lo que se muestra sea un menu...
	if (this->menuSeleccionado!=NULL)
		this->menuSeleccionado->MoverRaton(posicion.x,posicion.y);
	else 
	{
		// Primero vemos si hemos seleccionado algún elemento del HUD
		int cursor=0;
		if (!this->jugador->GetModoConstruccion())
		{
			cursor=this->jugador->PunteroRaton(posicion.x,posicion.y, this->AyudaActiva);
			if (cursor==-1)
			{
				// No hay seleccionado ningún elemento del HUD, así que si la pantalla de información 
				// está activa, pues la ocultamos, a modo de onMouseOut
				if (MostrandoInformacion)
				{
					char * params=NULL;
					scriptServer->Run("OcultarInformacion",params);
				}

				// NO se está sobre el HUD, así que buscamos la posición de la entidad sobre 
				// la que se encuentra (en caso de que esté sobre una entidad)
				entidad = GetEntMouseWorld();
				if ( entidad != NULL )
				{
					clase.Set( entidad->GetClass()->GetName() );
					if ( strcmp( clase.Get(), "nayuntamiento" ) == 0 )
					{
						nAyuntamiento* v_ayuntamiento = (nAyuntamiento*) entidad;
						int id_jugador = v_ayuntamiento->GetId();
						nJugador* propietario = partida_actual->GetJugadores()->At( id_jugador );
						nJugador* jugador = partida_actual->GetJugadorPrincipal();
						int campesinos_contratados = jugador->GetPersonaje()->GetCampesinosContratados();
						switch ( partida_actual->GetJugadorPrincipal()->GetPersonaje()->GetTipoPersonaje() )
						{
							case nPersonaje::PERSONAJE_SHUGENJA:
								char path[N_MAXPATH];
								switch( campesinos_contratados )
								{
									case 1:
										sprintf( path, "Coste total de planos: %d", propietario->TotalCostePlanos());
										sprintf( buf, "MostrarInformacionContextual \"%s\" %f %f", path, posicion.x, posicion.y );
										scriptServer->Run( buf, params );
										break;
									case 2:
										sprintf( path, "Valor total de planos: %d", propietario->TotalValorPlanos());
										sprintf( buf, "MostrarInformacionContextual \"%s\" %f %f", path, posicion.x, posicion.y );
										scriptServer->Run( buf, params );
										break;
									case 3:
										sprintf( path, "Numero de planos especiales: %d", propietario->TotalPlanosEspeciales());
										sprintf( buf, "MostrarInformacionContextual \"%s\" %f %f", path, posicion.x, posicion.y );
										scriptServer->Run( buf, params );
										break;
									case 4:
										sprintf( path, "Numero de planos distintos: %d", propietario->TotalPlanosDistintos());
										sprintf( buf, "MostrarInformacionContextual \"%s\" %f %f", path, posicion.x, posicion.y );
										scriptServer->Run( buf, params );
										break;
									default: break;
								}
								if ( id_jugador == partida_actual->GetJugadorPrincipal()->GetId() )
									cursor = 0;
								else
									cursor = 5;
								break;
							case nPersonaje::PERSONAJE_SHOGUN:
								cursor=2;
								break;
							case nPersonaje::PERSONAJE_MERCADER:
							case nPersonaje::PERSONAJE_MAESTRO:
							case nPersonaje::PERSONAJE_SAMURAI:
								if ( id_jugador == partida_actual->GetJugadorPrincipal()->GetId() )
									cursor=2;
								else
									cursor=0;
								break;
							default:
								cursor = 0;
								break;
						}
					}
					else if (strcmp(clase.Get(),"nconstruccion")==0) 
					{
						nConstruccion *v_construccion = (nConstruccion *) entidad;
						sprintf( buf, "MostrarInformacionContextual \"%s\" %f %f",v_construccion->GetNombre(),posicion.x,posicion.y);
						scriptServer->Run( buf, params );
						if ( partida_actual->GetJugadorPrincipal()->GetPersonaje()->GetTipoPersonaje() == nPersonaje::PERSONAJE_SAMURAI )
							cursor=6;
						else
							cursor=0;
					}
					else if (strcmp(clase.Get(),"ncasaplanos")==0) 
					{
						if (AyudaActiva)
						{
							sprintf( buf, "MostrarInformacionContextual \"Casa de Planos\" %f %f",posicion.x,posicion.y);
							scriptServer->Run( buf, params );
						}

						if ( partida_actual->GetJugadorPrincipal()->GetPersonaje()->GetTipoPersonaje() != nPersonaje::PERSONAJE_CAMPESINO )
							cursor=1;
						else
							cursor=0;
					}
					else if (strcmp(clase.Get(),"ncasamoneda")==0) 
					{
						if (AyudaActiva)
						{
							sprintf( buf, "MostrarInformacionContextual \"Casa de la Moneda\" %f %f",posicion.x,posicion.y);
							scriptServer->Run( buf, params );
						}

						if ( partida_actual->GetJugadorPrincipal()->GetPersonaje()->GetTipoPersonaje() != nPersonaje::PERSONAJE_CAMPESINO )
							cursor=2;
						else
							cursor=0;
					}
					else if (strcmp(clase.Get(),"ncampesino")==0)
					{
						switch ( partida_actual->GetJugadorPrincipal()->GetPersonaje()->GetTipoPersonaje() )
						{
							case nPersonaje::PERSONAJE_SHUGENJA:
								if ( partida_actual->GetJugadorPrincipal()->GetPersonaje()->GetCampesinosContratados() > 2 )
								{
									nCampesino* campesino = (nCampesino*) entidad;
									char path[N_MAXPATH];
									sprintf( path, "Tiene %d monedas", campesino->GetOro() );
									sprintf( buf, "MostrarInformacionContextual \"%s\" %f %f",path,posicion.x,posicion.y);
									scriptServer->Run( buf, params );
									
								}
								cursor=4;
								break;
							case nPersonaje::PERSONAJE_KUNOICHI:
								cursor=6;
								break;
							case nPersonaje::PERSONAJE_LADRONA:
								cursor=2;
								break;
							case nPersonaje::PERSONAJE_CAMPESINO:
								cursor=0;
								break;
							default:
								cursor=4;
								break;
						}
					}
					else if (strcmp(clase.Get(),"npersonaje") == 0  || 
							 strcmp(clase.Get(),"nkunoichi")  == 0  ||
							 strcmp(clase.Get(),"nladrona")	  == 0  ||
							 strcmp(clase.Get(),"nmaestro")	  == 0  ||
							 strcmp(clase.Get(),"nshogun")	  == 0  ||
							 strcmp(clase.Get(),"nshugenja")  == 0  ||
							 strcmp(clase.Get(),"nmercader")  == 0  ||
							 strcmp(clase.Get(),"ningeniero") == 0  ||
							 strcmp(clase.Get(),"nsamurai")	  == 0 ) 
					{
						nPersonaje *v_personaje=(nPersonaje *)entidad;
						if ( partida_actual->GetJugadorPrincipal()->GetId() != v_personaje->GetJugador()->GetId() )
							switch ( partida_actual->GetJugadorPrincipal()->GetPersonaje()->GetTipoPersonaje() )
							{
							case nPersonaje::PERSONAJE_KUNOICHI:
								cursor=6;
								break;
							case nPersonaje::PERSONAJE_LADRONA:
								cursor=2;
								break;
							default:
								cursor=0;
							}
						else
							cursor=0;
					}
					else 
						cursor=0;
				}
				else 
					cursor=0;
			}
			else 
			{
				// Estamos sobre elementos del HUD, así que ponemos el marcador de mostrando información a true (por si acaso);
				MostrandoInformacion=true;
			}
		}
		else 
		{
			cursor=3;
			nMapa *mapa = (nMapa *)kernelServer->Lookup("/data/mapa");
			posicion=this->GetPosMouseWorld();
			if (mapa->PuedeConstruirEdificio(floor(posicion.x),floor(posicion.z)) &&
				this->jugador->PosicionDentroDeDistrito(vector3(posicion.x,posicion.y,posicion.z)))
				this->jugador->MostrarConstruccionValida(vector3(posicion.x,posicion.y,posicion.z));
			else 
				this->jugador->MostrarConstruccionNoValida(vector3(posicion.x,posicion.y,posicion.z));
		}
		if (cursor!=this->gfxServer->GetCurrentCursor())
			this->gfxServer->SetCurrentCursor(cursor,true);
	}
}

/********************************************************************************/
/* Método      : ManejadorBotonPrincipal									    */
/* Descripción : Gestiona cuando se pincha con el botón izquierdo				*/
/*				 sobre la pantalla												*/
/* Parámetros  :													 			*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::ManejadorBotonPrincipal() {
	vector3 posicion=this->GetPosMouse();
	// En caso de que lo que se muestra sea un menu...
	if (this->menuSeleccionado!=NULL)
	{
		this->menuSeleccionado->Click(posicion.x,posicion.y);
	}
	else 
	{
		// Primero vemos si hemos seleccionado algún elemento del HUD
		nEntity *entidad;
		if (!this->jugador->GetModoConstruccion()) {
			if (this->jugador->ManejadorRaton(posicion.x,posicion.y)) {
				return;
			}
			else {
				entidad=GetEntMouseWorld();
				if (entidad!=NULL) {
					this->jugador->AccionPrincipal(entidad);
				}
				else {
					posicion=this->GetPosMouseWorld();
					this->jugador->SeleccionarPosicion(posicion);
				}
			}
		}
		else {
			nMapa *mapa = (nMapa *)kernelServer->Lookup("/data/mapa");
			posicion=this->GetPosMouseWorld();
			if (mapa->PuedeConstruirEdificio(floor(posicion.x),floor(posicion.z)) &&
				this->jugador->PosicionDentroDeDistrito(vector3(posicion.x,posicion.y,posicion.z)))
				this->jugador->ConstruirEdificio(vector3(posicion.x,posicion.y,posicion.z));
			else 
			{
				//n_printf( "ME RAJO\n" );
				this->jugador->SetModoConstruccion( false );
			}
		}
	}
}

/********************************************************************************/
/* Método      : ManejadorBotonSecundario									    */
/* Descripción : Gestiona cuando se pincha con el botón derecho					*/
/*				 sobre la pantalla												*/
/* Parámetros  :													 			*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::ManejadorBotonSecundario() {
	// Primero vemos si hemos seleccionado algún elemento del HUD
	if (!this->jugador->GetModoConstruccion())
	{
		nString path;
		vector3 posicion=this->GetPosMouse();
		nEntity *entidad;
		entidad=GetEntMouseWorld();
		if (entidad!=NULL)
			this->jugador->AccionSecundaria(entidad);
		else 
		{
			//n_printf( "ME RAJO\n" );
			this->jugador->SetModoConstruccion( false );
		}
	}
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
/*void nGame::place_camera(matrix44& cam, matrix44& look, float dist)
{
    cam = look;
    cam.M41 = look.M41 + (look.M31 * dist);
    cam.M42 = look.M42 + (look.M32 * dist);
    cam.M43 = look.M43 + (look.M33 * dist);
}*/

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
    bool orbit  = is->GetButton("orbit");
    bool pan    = is->GetButton("pan");
	this->get_mouse_input(is);
	if ( renderCamera.isvalid() )
	{
		if (orbit) 
		{
			this->renderCamera->setGiroDerecha(-((float)this->mouse_rel_x) * 0.005f);
		}
		else 
		{
			this->renderCamera->setGiroDerecha(0.0f);
		}
		if (pan) 
		{
			this->renderCamera->setDistancia(-((float)this->mouse_rel_y) * 0.005f);
		}
		else 
		{
			this->renderCamera->setDistancia(0.0f);
		}
		if (this->g_rueda!=0.0f) 
		{
			altura_camara  =((float)this->g_rueda-33320.0f) * 0.00005f;
			this->renderCamera->setAltura(altura_camara/10);
		}
	}
}

void nGame::SetIA(const char*ia_path){
	iaPath=ia_path;
	ias=iaPath.Get();
}

void nGame::SetNet(const char *net_path){
	netPath = net_path;
	nets = netPath.Get();
}

void nGame::SetMenu(const char *menu_path){
	menuPath = menu_path;
	menus = menuPath.Get();
}

void nGame::SetVideo(const char *video_path){
	videoPath = video_path;
	videos = videoPath.Get();
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

void nGame::MostrarRender(bool mostrar) {
	this->mostrarRender=mostrar;
}

/********************************************************************************/
/* Método      : ComenzarPartidaLocal										    */
/* Descripción : Función para comenzar una partida local						*/
/* Parámetros  : int num_jugadores	(numero de jugadores en la partida)			*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::ComenzarPartidaLocal(int num_jugadores) 
{
	partida_actual=new nPartida;
	partida_actual->ComenzarPartidaLocal(num_jugadores);
	partida_actual->SetJuego(this);
	this->jugador=partida_actual->GetJugadorPrincipal();
	this->JuegoEnRed=false;
	this->EsServidor=false;
}

/********************************************************************************/
/* Método      : ComenzarPartidaEnRed										    */
/* Descripción : Función para comenzar una partida en red						*/
/* Parámetros  : bool servidor: Flag que indica si el jugador hace de servidor	*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::ComenzarPartidaRed(bool servidor) 
{
	partida_actual=new nPartida;
	partida_actual->ComenzarPartidaRed(servidor);
	partida_actual->SetJuego(this);
	this->jugador=partida_actual->GetJugadorPrincipal();
	this->JuegoEnRed=true;
	this->EsServidor=servidor;
}

/********************************************************************************/
/* Método      : GetPartida													    */
/* Descripción : Devuelve el puntero a la partida actual						*/
/* Parámetros  :																*/
/* Devuelve    : nPartida *														*/
/********************************************************************************/
nPartida *nGame::GetPartida() 
{
	return partida_actual;
}

/********************************************************************************/
/* Método      : PlaySong													    */
/* Descripción : Toca una canción en streaming									*/
/* Parámetros  : int canal: Canal donde se toca la canción						*/
/*				 const char *song: Ruta relativa de la canción					*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::PlaySong(int canal, const char *song)
{
	// Convertimos la ruta en una ruta real
	char buf[N_MAXPATH];
	kernelServer->GetFileServer2()->ManglePath(song,buf,N_MAXPATH);

	// Paramos la canción anterior
	StopSong(canal);

	// Iniciamos la canción seleccionada con streaming
	p_cancion[canal]=FSOUND_Stream_Open(buf,FSOUND_NORMAL | FSOUND_LOOP_NORMAL,0,0);
	canalMusica[canal]=FSOUND_Stream_Play(FSOUND_FREE,p_cancion[canal]);
	FSOUND_SetPriority(canalMusica[canal],250);

	// Ajustamos el volumen de la música
	FSOUND_SetVolumeAbsolute(canalMusica[canal],volumenMusica[canal]);
}

/********************************************************************************/
/* Método      : SetMusicVolume												    */
/* Descripción : Cambia el volumen de una canción								*/
/* Parámetros  : int canal: Canal donde se toca la canción						*/
/*				 float vol: Volumen de 0 a 1									*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::SetMusicVolume(int canal, float vol) 
{
	// El volumen que nos entra es un valor de 0 a 1, así que lo convertimos en un valor de 0 a 255
	vol*=255;

	// Asignamos el volumen al canal
	FSOUND_SetVolumeAbsolute(canalMusica[canal],(int)vol);

	// Almacenamos el volumen de la música
	volumenMusica[canal]=vol;
}

/********************************************************************************/
/* Método      : SetSoundVolume												    */
/* Descripción : Cambia el volumen de un sonido									*/
/* Parámetros  : int canal: Canal donde se toca el sonido						*/
/*				 float vol: Volumen de 0 a 1									*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::SetSoundVolume(int canal, float vol) 
{
	// El volumen que nos entra es un valor de 0 a 1, así que lo convertimos en un valor de 0 a 255
	vol*=255;

	FSOUND_SetSFXMasterVolume((int) vol);

	// Asignamos el volumen al canal
	//FSOUND_SetVolume(canalSonido[canal],(int)vol);

	// Almacenamos el volumen del sonido
	volumenSonido[canal]=vol;
}

/********************************************************************************/
/* Método      : PlaySound													    */
/* Descripción : Toca un sonido													*/
/* Parámetros  : int canal: Canal donde se toca el sonido						*/
/*				 const char *sound: Ruta relativa del sonido					*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::PlaySound(int canal, const char *sound)
{
	// Convertimos la ruta en una ruta real
	char buf[N_MAXPATH];
	kernelServer->GetFileServer2()->ManglePath(sound,buf,N_MAXPATH);

	// Iniciamos el sonido
	p_sonido[canal] = FSOUND_Sample_Load(FSOUND_FREE,buf,FSOUND_NORMAL | FSOUND_HW2D,0,0);
	canalSonido[canal] = FSOUND_PlaySound(FSOUND_FREE, p_sonido[canal]);
	FSOUND_SetPriority(canalSonido[canal],240);

	// Almacenamos el puntero para luego liberarlo
	pt_samples.PushBack(p_sonido[canal]);

	// Ajustamos el volumen del sonido
	FSOUND_SetVolume(canalSonido[canal],volumenSonido[canal]);
	FSOUND_SetPaused(canalSonido[canal],FALSE);
}

/********************************************************************************/
/* Método      : Play3DSound												    */
/* Descripción : Toca un sonido posicional										*/
/* Parámetros  : int canal: Canal donde se toca el sonido						*/
/*				 const char *sound: Ruta relativa del sonido					*/
/*				 vector3 p_pos: Posición del sonido								*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::Play3DSound(int canal, const char *sound, vector3 p_pos)
{
	// Convertimos la ruta en una ruta real
	char buf[N_MAXPATH];
	kernelServer->GetFileServer2()->ManglePath(sound,buf,N_MAXPATH);

	// Iniciamos el sonido
	p_sonido[canal] = FSOUND_Sample_Load(FSOUND_FREE,buf,FSOUND_HW3D | FSOUND_MPEGACCURATE,0,0);
	canalSonido[canal] = FSOUND_PlaySound(FSOUND_FREE, p_sonido[canal]);
	
	// Almacenamos el puntero para luego liberarlo
	pt_samples.PushBack(p_sonido[canal]);

	float objectPos[3]={p_pos.x,p_pos.y,p_pos.z};
	FSOUND_3D_SetAttributes(canalSonido[canal],objectPos,NULL);
	FSOUND_3D_SetMinMaxDistance(canalSonido[canal],0.6f,40.0f);
	FSOUND_SetPriority(canalSonido[canal],200);

	// Ajustamos el volumen del sonido
	// FSOUND_SetVolume(canalSonido[canal],volumenSonido[canal]);
}

/********************************************************************************/
/* Método      : Play3DSound												    */
/* Descripción : Toca un sonido posicional con posibilidad de loop y devuelve	*/
/*				 el canal														*/
/* Parámetros  : int canal: Canal donde se toca el sonido						*/
/*				 const char *sound: Ruta relativa del sonido					*/
/*				 vector3 p_pos: Posición del sonido								*/
/*				 float p_mind: Distancia mínima									*/
/*				 float p_maxd: Distancia máxima									*/
/*				 bool p_loop: Hace loop?										*/
/*				 int p_prioridad: Prioridad del sonido							*/
/* Devuelve    : int canal														*/
/********************************************************************************/
int nGame::Play3DSoundEx(int canal, const char *sound, vector3 p_pos, float p_mind, float p_maxd, bool p_loop, int p_prioridad)
{
	// Convertimos la ruta en una ruta real
	char buf[N_MAXPATH];
	kernelServer->GetFileServer2()->ManglePath(sound,buf,N_MAXPATH);

	// Iniciamos el sonido
	if ( p_loop )
	{
		p_sonido[canal] = FSOUND_Sample_Load(FSOUND_FREE,buf,FSOUND_HW3D | FSOUND_MPEGACCURATE |  FSOUND_LOOP_NORMAL ,0,0);
	}
	else
	{
		p_sonido[canal] = FSOUND_Sample_Load(FSOUND_FREE,buf,FSOUND_HW3D | FSOUND_MPEGACCURATE,0,0);
	}
	canalSonido[canal] = FSOUND_PlaySound(FSOUND_FREE, p_sonido[canal]);

	// Almacenamos el puntero para luego liberarlo
	pt_samples.PushBack(p_sonido[canal]);

	float objectPos[3]={p_pos.x,p_pos.y,p_pos.z};
	FSOUND_3D_SetAttributes(canalSonido[canal],objectPos,NULL);
	FSOUND_3D_SetMinMaxDistance(canalSonido[canal],p_mind,p_maxd);
	FSOUND_SetPriority(canalSonido[canal],p_prioridad);

	return canalSonido[canal];
}

/********************************************************************************/
/* Método      : SetPosSound												    */
/* Descripción : Actualiza la posición de un sonido								*/
/* Parámetros  : int canal: Canal donde se toca el sonido						*/
/*				 vector3 p_pos: Posición del sonido								*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::SetPosSound(int canal, vector3 p_pos)
{
	float objectPos[3]={p_pos.x,p_pos.y,p_pos.z};
	FSOUND_3D_SetAttributes(canal,objectPos,NULL);
}

/********************************************************************************/
/* Método      : StopSong													    */
/* Descripción : Para una canción												*/
/* Parámetros  : int canal: Canal donde se toca la canción						*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::StopSong(int canal)
{
	FSOUND_Stream_Stop(p_cancion[canal]);
	//FSOUND_Stream_Close(p_cancion[canal]);
}

/********************************************************************************/
/* Método      : StopSound													    */
/* Descripción : Para un sonido													*/
/* Parámetros  : int canal: Canal donde se toca el sonido						*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::StopSound(int canal)
{
	FSOUND_StopSound(canal);
}

/********************************************************************************/
/* Método      : StopAllSounds												    */
/* Descripción : Para todos los sonidos activos									*/
/* Parámetros  :																*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::StopAllSounds()
{
	FSOUND_StopSound(FSOUND_ALL);
	pt_samples.Clear();
}

/********************************************************************************/
/* Método      : PauseAllSounds												    */
/* Descripción : Pausa todos los sonidos activos								*/
/* Parámetros  : bool p_pausa: Los pausa?										*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::PauseAllSounds(bool p_pausa)
{
	if (p_pausa)
		FSOUND_SetPaused(FSOUND_ALL, TRUE);
	else
		FSOUND_SetPaused(FSOUND_ALL, FALSE);
}

/********************************************************************************/
/* Método      : PauseSong													    */
/* Descripción : Pausa una canción												*/
/* Parámetros  : bool p_pausa: La pausa?										*/
/*				 int canal: Canal de la canción									*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::PauseSong(int canal, bool p_pausa)
{
	if (p_pausa)
		FSOUND_SetPaused(canalMusica[canal], TRUE);
	else
		FSOUND_SetPaused(canalMusica[canal], FALSE);
}

/********************************************************************************/
/* Método      : EsJugadorPrincipal											    */
/* Descripción : Devuelve cierto si el parametro id jugador corresponde 		*/
/*				 al jugador principal											*/
/* Parámetros  : int jugador: id del jugador									*/
/* Devuelve    : bool 'jugador' es el jugador principal							*/
/********************************************************************************/
bool nGame::EsJugadorPrincipal(int jugador)
{
	return partida_actual->EsJugadorPrincipal(jugador);
}

/********************************************************************************/
/* Método      : GetPuntosPorAccion											    */
/* Descripción : Devuelve los puntos por acción del jugador				 		*/
/* Parámetros  : int jugador: id del jugador									*/
/* Devuelve    : int: Puntos por acción del jugador								*/
/********************************************************************************/
int nGame::GetPuntosPorAccion(int jugador)
{
	return partida_actual->GetPuntosPorAccion(jugador);
}

/********************************************************************************/
/* Método      : GetPuntosPorConstruccion									    */
/* Descripción : Devuelve los puntos por construcción del jugador		 		*/
/* Parámetros  : int jugador: id del jugador									*/
/* Devuelve    : int: Puntos por construcción del jugador						*/
/********************************************************************************/
int nGame::GetPuntosPorConstruccion(int jugador)
{
	return partida_actual->GetPuntosPorConstruccion(jugador);
}

/********************************************************************************/
/* Método      : GetNumConstrucciones										    */
/* Descripción : Devuelve el número de construcciones del jugador		 		*/
/* Parámetros  : int jugador: id del jugador									*/
/* Devuelve    : int: Número de construcciones del jugador						*/
/********************************************************************************/
int nGame::GetNumConstrucciones(int jugador)
{
	return partida_actual->GetNumConstrucciones(jugador);
}

/********************************************************************************/
/* Método      : GetPuntosPorDistintivo										    */
/* Descripción : Devuelve los puntos por distintivo del jugador			 		*/
/* Parámetros  : int jugador: id del jugador									*/
/* Devuelve    : int: Puntos por distintivo del jugador							*/
/********************************************************************************/
int nGame::GetPuntosPorDistintivo(int jugador)
{
	return partida_actual->GetPuntosPorDistintivo(jugador);
}

/********************************************************************************/
/* Método      : GetPuntosPorOro											    */
/* Descripción : Devuelve los puntos por oro del jugador		 				*/
/* Parámetros  : int jugador: id del jugador									*/
/* Devuelve    : int: Puntos por oro del jugador								*/
/********************************************************************************/
int nGame::GetPuntosPorOro(int jugador)
{
	return partida_actual->GetPuntosPorOro(jugador);
}

/********************************************************************************/
/* Método      : GetPuntosPorTiempo											    */
/* Descripción : Devuelve los puntos por tiempo del jugador				 		*/
/* Parámetros  : int jugador: id del jugador									*/
/* Devuelve    : int: Puntos por tiempo del jugador								*/
/********************************************************************************/
int nGame::GetPuntosPorTiempo(int jugador)
{
	return partida_actual->GetPuntosPorTiempo(jugador);
}

/********************************************************************************/
/* Método      : GetPuntosTotales												*/
/* Descripción : Devuelve los puntos totales del jugador		 				*/
/* Parámetros  : int jugador: id del jugador									*/
/* Devuelve    : int: Puntos totales del jugador								*/
/********************************************************************************/
int nGame::GetPuntosTotales(int jugador)
{
	return partida_actual->GetPuntosTotales(jugador);
}

/********************************************************************************/
/* Método      : MostrarAyuda													*/
/* Descripción : Indica si la ayuda se muestra o no						 		*/
/* Parámetros  : bool p_mostrar: flag de muestra								*/
/* Devuelve    :																*/
/********************************************************************************/
void nGame::MostrarAyuda(bool p_mostrar)
{
	this->AyudaActiva=p_mostrar;
}
