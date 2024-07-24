#define N_IMPLEMENTS nPartida

#include "kernel/ntimeserver.h"
#include "toshi/npartida.h"
#include "toshi/njugador.h"
#include "toshi/npersonaje.h"
#include "toshi/ncampesino.h"
#include "toshi/nayuntamiento.h"
#include "toshi/ngame.h"
#include "toshi/ncamera.h"
#include "toshi/ncasaplanos.h"

#define NUM_CAMPESINOS_POR_JUGADOR	2
#define NUM_MAX_CONSTRUCCIONES      8

/******************************************************************************************

CLASE nPartida

******************************************************************************************/

nNebulaScriptClass(nPartida, "nroot");

nPartida::nPartida(): refScriptServer( kernelServer, this ) 
{
	refScriptServer       = "/sys/servers/script";
	this->jugadores=new nArray<nJugador *>;
	this->campesinos=new nArray<nCampesino *>;
	this->personajes=new nArray<T_Personaje *>;
	this->Reset();
}

nPartida::~nPartida()
{
	for (int i=0;i<this->jugadores->Size();i++)
	{
		this->jugadores->At(i)->Release();
	}
	for (int i=0;i<this->personajes->Size();i++)
	{
		this->personajes->At(i)->personaje->Release();
	}
	for (int i=0;i<this->campesinos->Size();i++)
	{
		this->campesinos->At(i)->Release();
	}
	this->jugadores->Clear();
	this->personajes->Clear();
	this->campesinos->Clear();
	n_free(this->jugadores);
	n_free(this->personajes);
	n_free(this->campesinos);
}

void nPartida::AsignarNodosVisuales()
{
	for (int i=0;i<personajes->Size();i++)
	{
		switch (i)
		{
		case 0:
			personajes->At(i)->personaje=(nPersonaje *) kernelServer->New("nkunoichi","/game/world/kunoichi");
			personajes->At(i)->personaje->SetVisNode("/game/models/kunoichi");
			break;
		case 1:
			personajes->At(i)->personaje=(nPersonaje *) kernelServer->New("nladrona","/game/world/ladrona");
			personajes->At(i)->personaje->SetVisNode("/game/models/ladrona");
			break;
		case 2:
			personajes->At(i)->personaje=(nPersonaje *) kernelServer->New("nshugenja","/game/world/shugenja");
			personajes->At(i)->personaje->SetVisNode("/game/models/shugenja");
			break;
		case 3:
			personajes->At(i)->personaje=(nPersonaje *) kernelServer->New("nmercader","/game/world/mercader");
			personajes->At(i)->personaje->SetVisNode("/game/models/mercader");
			break;
		case 4:
			personajes->At(i)->personaje=(nPersonaje *) kernelServer->New("ningeniero","/game/world/ingeniero");
			personajes->At(i)->personaje->SetVisNode("/game/models/ingeniero");
			break;
		case 5:
			personajes->At(i)->personaje=(nPersonaje *) kernelServer->New("nshogun","/game/world/shogun");
			personajes->At(i)->personaje->SetVisNode("/game/models/shogun");
			break;
		case 6:
			personajes->At(i)->personaje=(nPersonaje *) kernelServer->New("nsamurai","/game/world/samurai");
			personajes->At(i)->personaje->SetVisNode("/game/models/samurai");
			break;
		case 7:
			personajes->At(i)->personaje=(nPersonaje *) kernelServer->New("nmaestro","/game/world/maestro");
			personajes->At(i)->personaje->SetVisNode("/game/models/maestro");
			break;
		}
		personajes->At(i)->personaje->SetEntityHeight(0.0f);
		personajes->At(i)->personaje->SetCollideClass("humano");
		personajes->At(i)->personaje->SetCollideShape("meshes:hbbox2a.n3d");
		personajes->At(i)->personaje->SetTouchMethod("colisionHumano");
		personajes->At(i)->personaje->Rxyz(-90.0f,180.0f,0.0f);
		personajes->At(i)->personaje->Txyz(i*5+10,-10,-50);
		personajes->At(i)->personaje->SetVisible(false);
	}
}

void nPartida::Reset() 
{
	this->jugadores->Clear();
	this->personajes->Clear();
	this->campesinos->Clear();
	this->activa=false;
	this->TiempoTurno=0.0f;
	this->TiempoAnterior=0.0f;
	this->TiempoTurnoPago=0.0f;
	this->TiempoParaTerminar=10.0f;
	this->TiempoTurnoSiguiente=0.0f;
	this->TodosOcultos=false;
	this->siguienteJugador=0;
	this->FinPartida=false;

	T_Personaje *desc_personaje=NULL;

	desc_personaje=new T_Personaje();
	desc_personaje->libreAnterior=true;
	desc_personaje->libre=true;
	desc_personaje->jugador=-1;
	desc_personaje->personaje=NULL;
	personajes->PushBack(desc_personaje);

	desc_personaje=new T_Personaje();
	desc_personaje->libreAnterior=true;
	desc_personaje->libre=true;
	desc_personaje->jugador=-1;
	desc_personaje->personaje=NULL;
	personajes->PushBack(desc_personaje);

	desc_personaje=new T_Personaje();
	desc_personaje->libreAnterior=true;
	desc_personaje->libre=true;
	desc_personaje->jugador=-1;
	desc_personaje->personaje=NULL;
	personajes->PushBack(desc_personaje);

	desc_personaje=new T_Personaje();
	desc_personaje->libreAnterior=true;
	desc_personaje->libre=true;
	desc_personaje->jugador=-1;
	desc_personaje->personaje=NULL;
	personajes->PushBack(desc_personaje);

	desc_personaje=new T_Personaje();
	desc_personaje->libreAnterior=true;
	desc_personaje->libre=true;
	desc_personaje->jugador=-1;
	desc_personaje->personaje=NULL;
	personajes->PushBack(desc_personaje);

	desc_personaje=new T_Personaje();
	desc_personaje->libreAnterior=true;
	desc_personaje->libre=true;
	desc_personaje->jugador=-1;
	desc_personaje->personaje=NULL;
	personajes->PushBack(desc_personaje);

	desc_personaje=new T_Personaje();
	desc_personaje->libreAnterior=true;
	desc_personaje->libre=true;
	desc_personaje->jugador=-1;
	desc_personaje->personaje=NULL;
	personajes->PushBack(desc_personaje);

	desc_personaje=new T_Personaje();
	desc_personaje->libreAnterior=true;
	desc_personaje->libre=true;
	desc_personaje->jugador=-1;
	desc_personaje->personaje=NULL;
	personajes->PushBack(desc_personaje);

	srand( (unsigned int) time( NULL ) );
}

void nPartida::ComenzarPartidaLocal(int num_jugadores) 
{
	// Vaciamos la partida
	this->Reset();

	nJugador *jugador;
	nPersonaje *personaje;
	nCampesino *campesino;
	nCamera *camara;
	nAyuntamiento *ayuntamiento;
	vector3 posAyuntamiento3D;
	vector2 posAyuntamiento;
	vector2 dirAyuntamiento;
	vector2 posDestino;
	nMapa *mapa=(nMapa *)kernelServer->Lookup("/data/mapa");
	float posx,posy;
	char path[N_MAXPATH];
	char *params = NULL;
	this->numero_jugadores=num_jugadores;
	this->activa=true;
	AsignarNodosVisuales();

	for (int i=0;i<num_jugadores;i++) 
	{
		// Generamos los jugadores 
		jugador=new nJugador();
		jugador->SetId(i);
		jugador->SetPartida(this);
		this->jugadores->PushBack(jugador);
		sprintf(path,"/game/world/jugador%d",i);

		// Generamos los personajes de los jugadores
		personaje=(nPersonaje *)kernelServer->New("npersonaje",path);
		personaje->SetEntityHeight(0.0f);
		personaje->SetCollideClass("humano");
		personaje->SetCollideShape("meshes:hbbox2a.n3d");
		personaje->SetTouchMethod("colisionHumano");
		personaje->Rxyz(-90.0f,180.0f,0.0f);

		// Asignamos el nodo visual al personaje
		sprintf(path,"/game/models/jugador%d",i);
		personaje->SetVisNode(path);

		// Calculamos la posición del personaje (al lado del ayuntamiento en el que sale)
		sprintf(path,"/game/world/ayto%d",i);
		ayuntamiento=(nAyuntamiento *)kernelServer->Lookup(path);

		posAyuntamiento3D=ayuntamiento->GetPosition();
		vector3 v_pos_central(40,0,40);

		float v_angulo = CalcularAngulo(posAyuntamiento3D-v_pos_central);
		ayuntamiento->Rxyz(0.0f,n_rad2deg(v_angulo)-90.0f,0.0f);
		
		posAyuntamiento.set(posAyuntamiento3D.x,posAyuntamiento3D.z);
		dirAyuntamiento=posAyuntamiento;
		dirAyuntamiento.norm();

		posDestino=posAyuntamiento-dirAyuntamiento*2;

		personaje->SetPosition(posDestino.x,posDestino.y);
		jugador->SetPersonaje(personaje);
	}

	// Generamos los campesinos del juego
	for (int i=0;i<num_jugadores*NUM_CAMPESINOS_POR_JUGADOR;i++) 
	{
		sprintf(path,"/game/world/campesino%d",i);
		campesino=(nCampesino *) kernelServer->Lookup(path);
		do
		{
			//posx=(int)((rand()%40)-20+ANCHO_MAPA/2);
			//posy=(int)((rand()%40)-20+ALTO_MAPA/2);
			posx = ( rand() % (ANCHO_MAPA / 2) ) + (int) ( (ANCHO_MAPA) * 2 / 8 );
			posy = ( rand() % (ALTO_MAPA / 2) ) + (int) ( (ALTO_MAPA) * 2 / 8 ) ; 
		} while (!mapa->PuedeConstruirEdificio(posx,posy));
		campesino->SetPosition(posx,posy);
		this->campesinos->PushBack(campesino);
		this->campesinos->At(i)->SetId(i);
	}

	// Asignamos una posicion al azar al jugador principal
	int pos_jugador=(int)(rand()%num_jugadores);
	jugadores->At(pos_jugador)->SetJugadorPrincipal(true);
	camara=(nCamera *)kernelServer->Lookup("/game/camaraPartida");
	sprintf(path,"/game/world/jugador%d",pos_jugador);
	camara->SetTarget(path);

	// Generamos la marca del ayuntamiento que indique al jugador su ayuntamiento 
	// llamando al procedimiento Generar_Marca_Ayto (x,y,z)
	sprintf(path,"/game/world/ayto%d",pos_jugador);
	ayuntamiento=(nAyuntamiento *)kernelServer->Lookup(path);

	vector3 v_pos_marca = ayuntamiento->GetPosition();
	vector3 v_pos_central(40,0,40);

	float v_angulo = CalcularAngulo(v_pos_marca-v_pos_central);

	//v_pos_marca.y+=0.5f;

	sprintf(path,"Generar_Marca_Ayto %f %f %f %f",v_pos_marca.x,v_pos_marca.y,v_pos_marca.z,n_rad2deg(v_angulo));
	refScriptServer->Run(path,params);

	// Reseteamos el tiempo de juego
	// kernelServer->ts->ResetTime();
}

void nPartida::ComenzarPartidaRed(bool servidor) 
{
	// Vaciamos la partida
	this->Reset();

	// TODO: Coger el número de jugadores del módulo de red
	int num_jugadores=2;

	nJugador *jugador;
	nPersonaje *personaje;
	nCamera *camara;
	nAyuntamiento *ayuntamiento;
	vector3 posAyuntamiento3D;
	vector2 posAyuntamiento;
	vector2 dirAyuntamiento;
	vector2 posDestino;
	nMapa *mapa=(nMapa *)kernelServer->Lookup("/data/mapa");
	char path[N_MAXPATH];
	char *params = NULL;
	this->numero_jugadores=num_jugadores;
	this->activa=true;
	this->es_servidor=servidor;
	AsignarNodosVisuales();

	for (int i=0;i<num_jugadores;i++) 
	{
		// Generamos los jugadores 
		sprintf(path,"personaje%d",i);
		jugador=new nJugador();
		jugador->SetId(i);
		jugador->SetPartida(this);
		this->jugadores->PushBack(jugador);

		// Generamos los personajes de los jugadores
		sprintf(path,"/game/world/jugador%d",i);
		personaje=(nPersonaje *)kernelServer->New("npersonaje",path);
		personaje->SetEntityHeight(0.0f);
		personaje->SetCollideClass("humano");
		personaje->SetCollideShape("meshes:hbbox2a.n3d");
		personaje->SetTouchMethod("colisionHumano");
		personaje->Rxyz(-90.0f,180.0f,0.0f);

		// Asignamos el nodo visual al personaje
		sprintf(path,"/game/models/jugador%d",i);
		personaje->SetVisNode(path);

		// Calculamos la posición del personaje
		sprintf(path,"/game/world/ayto%d",i);
		ayuntamiento=(nAyuntamiento *)kernelServer->Lookup(path);

		posAyuntamiento3D=ayuntamiento->GetPosition();
		posAyuntamiento.set(posAyuntamiento3D.x,posAyuntamiento3D.z);
		dirAyuntamiento=posAyuntamiento;
		dirAyuntamiento.norm();
		posDestino=posAyuntamiento-dirAyuntamiento*2;

		personaje->SetPosition(posDestino.x,posDestino.y);
		jugador->SetPersonaje(personaje);
	}
	
	camara=(nCamera *)kernelServer->Lookup("/game/camaraPartida");
	// Si es servidor, le damos el jugador 0, si no le damos otro jugador
	// TODO: Esto habría que cogerlo del módulo de red, es decir, 
	// que el cliente nos diga que número de jugador es
	if (this->es_servidor)
	{
		jugadores->At(0)->SetJugadorPrincipal(true);
		camara->SetTarget("/game/world/jugador0");

		// Generamos la marca del ayuntamiento que indique al jugador su ayuntamiento 
		// llamando al procedimiento Generar_Marca_Ayto (x,y,z)
		ayuntamiento=(nAyuntamiento *)kernelServer->Lookup("/game/world/ayto0");

		vector3 v_pos_marca = ayuntamiento->GetPosition();
		vector3 v_pos_central(40,0,40);

		float v_angulo = CalcularAngulo(v_pos_marca-v_pos_central);

		//v_pos_marca.y+=0.5f;

		sprintf(path,"Generar_Marca_Ayto %f %f %f %f",v_pos_marca.x,v_pos_marca.y,v_pos_marca.z,n_rad2deg(v_angulo));
		refScriptServer->Run(path,params);
	}
	else
	{
		jugadores->At(1)->SetJugadorPrincipal(true);
		camara->SetTarget("/game/world/jugador1");

		// Generamos la marca del ayuntamiento que indique al jugador su ayuntamiento 
		// llamando al procedimiento Generar_Marca_Ayto (x,y,z)
		ayuntamiento=(nAyuntamiento *)kernelServer->Lookup("/game/world/ayto1");

		vector3 v_pos_marca = ayuntamiento->GetPosition();
		vector3 v_pos_central(40,0,40);

		float v_angulo = CalcularAngulo(v_pos_marca-v_pos_central);

		//v_pos_marca.y+=0.5f;

		sprintf(path,"Generar_Marca_Ayto %f %f %f %f",v_pos_marca.x,v_pos_marca.y,v_pos_marca.z,n_rad2deg(v_angulo));
		refScriptServer->Run(path,params);
	}

	// Reseteamos el tiempo de juego
	// kernelServer->ts->ResetTime();
}

float nPartida::CalcularAngulo(vector3 p_dir) {
	float v_angulo=0.0f;
	p_dir.norm();
	if (p_dir.x<0) {
		if (p_dir.z<0) {
			v_angulo=-asin(p_dir.x)+PI/2;
		}
		else {
			v_angulo=asin(p_dir.x)-PI/2;
		}
	}
	else {
		if (p_dir.z<0) {
			v_angulo=-asin(p_dir.x)+PI/2;
		}
		else {
			v_angulo=asin(p_dir.x)-PI/2;
		}
	}
	return v_angulo;
}

void nPartida::LiberarPersonaje(int p_jugador)
{
	char path[N_MAXPATH];
	for (int i=0;i<personajes->Size();i++) 
	{
		if (personajes->At(i)->jugador == p_jugador) 
		{
			personajes->At(i)->jugador=-1;
			personajes->At(i)->libre=true;
		}
	}

	sprintf(path,"/game/world/jugador%d",p_jugador);
	jugadores->At( p_jugador )->SetSiguientePersonaje((nPersonaje *)kernelServer->Lookup(path));

	if (jugadores->At( p_jugador )->GetJugadorPrincipal())
	{
		TodosOcultos=false;
		RedibujaSeleccionPersonajes(true);
	}
}

bool nPartida::PuedeLiberarPersonaje(int num_jugador)
{
	for (int i=0;i<personajes->Size();i++) 
	{
		if ( personajes->At( i )->jugador == num_jugador && !jugadores->At( num_jugador )->GetPersonaje()->EstaMuerto() )
		{
			if (personajes->At( i )->tiempo_jugado > MIN_TIEMPO_TURNO )
			{
				return true;
			}
			else
			{
				if ( jugadores->At(num_jugador)->GetPersonaje()->GetTipoPersonaje() == nPersonaje::PERSONAJE_SHOGUN ) 
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	return false;
}

bool nPartida::PersonajeLibre(nPersonaje::TPersonaje tipo_personaje)
{
	for (int i=0;i<personajes->Size();i++) 
	{
		if (personajes->At(i)->personaje->GetTipoPersonaje() == tipo_personaje) 
		{
			return personajes->At(i)->libre;
		}
	}	
	return false;
}

bool nPartida::TodosLosPersonajesOcupados()
{
	int num_personajes_libres=0;
	for (int i=0;i<personajes->Size();i++) 
	{
		if (personajes->At(i)->libre)
		{
			num_personajes_libres++;
		}
	}
	return num_personajes_libres<2;
}

void nPartida::BloqueaPersonaje(nPersonaje::TPersonaje tipo_personaje,int num_jugador)
{
	for (int i=0;i<personajes->Size();i++) 
	{
		if (personajes->At(i)->personaje->GetTipoPersonaje() == tipo_personaje && personajes->At(i)->libre)
		{
			personajes->At(i)->jugador=num_jugador;
			personajes->At(i)->libre=false;
			personajes->At(i)->tiempo_jugado=0.0f;
			nPersonaje *personajeSel=personajes->At(i)->personaje;
			jugadores->At(num_jugador)->SetSiguientePersonaje(personajeSel);
			//n_printf("#*** EL JUGADOR %d BLOQUEA AL PERSONAJE '%s' ***#\n",num_jugador,personajeSel->GetName());
		}
	}
}

void nPartida::SetJuego(nGame *p_juego) 
{
	this->juego=p_juego;	
}

void nPartida::Tick(float t) 
{
	char path[N_MAXPATH];
	TiempoTurno=t;
	float dt=TiempoTurno-TiempoAnterior;
	if (this->activa)
	{
		if ( !FinPartida || TiempoParaTerminar > 0.0f )
		{
			sprintf( path, "/game/world/casaplanos" );
			nCasaPlanos* casaplanos = (nCasaPlanos*) kernelServer->Lookup( path );

			// Si la casa de planos se ha quedado sin planos se acaba la partida
			if ( casaplanos->GetListaPlanos().size() == 0 )
			{
				if ( FinPartida )
					TiempoParaTerminar -= dt;
				else
	  	    		FinPartida = true;
			}

			for (int i=0;i<jugadores->Size();i++) 
			{
				jugadores->At(i)->Tick(t);

				// Si algún jugador ha construido ocho edificios se acaba la partida
				if ( jugadores->At(i)->GetDistrito()->NumeroConstrucciones() >= NUM_MAX_CONSTRUCCIONES )
				{
					if ( FinPartida )
						TiempoParaTerminar -= dt;
					else
	  	    			FinPartida = true;
				}
			}
			for (int i=0;i<personajes->Size();i++) 
			{
				if (!personajes->At(i)->libre)
					personajes->At(i)->tiempo_jugado+=dt;
			}
			RedibujaSeleccionPersonajes(false);
		}
		else
		{
			PuntuarPorTiempo();
			PuntuarPorDinero();
			PuntuarPorDistintivo();
			if ( GetJugadorPrincipal()->GetModoConstruccion() )
				GetJugadorPrincipal()->SetModoConstruccion( false );

			nGame *juego = (nGame *) kernelServer->Lookup("/game");
			char *params=NULL;
			this->activa=false;
			refScriptServer->Run( "TratarMenuFinalizacion", params );

			nCasaPlanos* casaplanos = (nCasaPlanos*) kernelServer->Lookup( "/game/world/casaplanos" );
			casaplanos->LeePlanos();
		}
	}
	TiempoAnterior=TiempoTurno;
}

nJugador *nPartida::GetJugadorPrincipal() 
{
	for (int i=0;i<this->jugadores->Size();i++)
	{
		if (jugadores->At(i)->GetJugadorPrincipal())
			return jugadores->At(i);
	}
	return NULL;
}

void nPartida::OcultaTodosLosPersonajes() 
{
	nTexArrayNode *tex;
	for (int i=0;i<personajes->Size();i++) 
	{
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_KUNOICHI )
		{
			tex=(nTexArrayNode*) kernelServer->Lookup("/game/scene/hud/kunoichi/tex");
			tex->SetTexture(0,"textures:SelKunoichi2.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_LADRONA )
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/ladrona/tex");
			tex->SetTexture(0,"textures:SelLadrona2.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_SHUGENJA )
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/shugenja/tex");
			tex->SetTexture(0,"textures:SelShugenja2.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_SHOGUN )
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/shogun/tex");
			tex->SetTexture(0,"textures:SelShogun2.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_MERCADER )
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/mercader/tex");
			tex->SetTexture(0,"textures:SelMercader2.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_MAESTRO )
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/maestro/tex");
			tex->SetTexture(0,"textures:SelMaestro2.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_INGENIERO )
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/ingeniero/tex");
			tex->SetTexture(0,"textures:SelIngeniero2.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_SAMURAI )
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/samurai/tex");
			tex->SetTexture(0,"textures:SelSamurai2.png",NULL);
		}
	}
	TodosOcultos=true;
}

void nPartida::MostrarPersonajesActivos() 
{
	nTexArrayNode *tex;
	for (int i=0;i<personajes->Size();i++) 
	{
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_KUNOICHI && personajes->At(i)->libre)
		{
			tex=(nTexArrayNode*) kernelServer->Lookup("/game/scene/hud/kunoichi/tex");
			tex->SetTexture(0,"textures:SelKunoichi.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_LADRONA  && personajes->At(i)->libre)
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/ladrona/tex");
			tex->SetTexture(0,"textures:SelLadrona.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_SHUGENJA  && personajes->At(i)->libre)
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/shugenja/tex");
			tex->SetTexture(0,"textures:SelShugenja.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_SHOGUN  && personajes->At(i)->libre)
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/shogun/tex");
			tex->SetTexture(0,"textures:SelShogun.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_MERCADER  && personajes->At(i)->libre)
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/mercader/tex");
			tex->SetTexture(0,"textures:SelMercader.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_MAESTRO  && personajes->At(i)->libre)
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/maestro/tex");
			tex->SetTexture(0,"textures:SelMaestro.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_INGENIERO  && personajes->At(i)->libre)
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/ingeniero/tex");
			tex->SetTexture(0,"textures:SelIngeniero.png",NULL);
		}
		if (personajes->At(i)->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_SAMURAI  && personajes->At(i)->libre)
		{
			tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/samurai/tex");
			tex->SetTexture(0,"textures:SelSamurai.png",NULL);
		}
	}
	TodosOcultos=false;
}

nJugador *nPartida::AsignarJugadorIA() 
{
	nJugador *asignado;
	if (jugadores->At(siguienteJugador)->GetJugadorPrincipal())
	{
		siguienteJugador++;
		asignado=jugadores->At(siguienteJugador);
	}
	else
	{
		asignado=jugadores->At(siguienteJugador);
	}
	siguienteJugador++;
	return asignado;
}

void nPartida::RedibujaSeleccionPersonajes(bool redibujarTodos) 
{
	nTexArrayNode *tex;
	char ncampesino[N_MAXPATH];
	sprintf(ncampesino,"jugador%d",jugadores->At(0)->GetId());
	if (GetJugadorPrincipal()->GetSiguientePersonaje()->GetTipoPersonaje()!=nPersonaje::PERSONAJE_CAMPESINO)
	{
		if (!TodosOcultos)
			OcultaTodosLosPersonajes();
	}
	else 
	{
		for (int i=0;i<personajes->Size();i++) 
		{
			if (personajes->At(i)->libre!=personajes->At(i)->libreAnterior || redibujarTodos) 
			{
				personajes->At(i)->libreAnterior=personajes->At(i)->libre;
				switch (personajes->At(i)->personaje->GetTipoPersonaje())
				{
				case nPersonaje::PERSONAJE_KUNOICHI:
					tex=(nTexArrayNode*) kernelServer->Lookup("/game/scene/hud/kunoichi/tex");
					if (personajes->At(i)->libre && 
						GetJugadorPrincipal()->GetPersonajeAnterior() != personajes->At(i)->personaje->GetTipoPersonaje() )
					{
						tex->SetTexture(0,"textures:SelKunoichi.png",NULL);
					}
					else
					{
						tex->SetTexture(0,"textures:SelKunoichi2.png",NULL);
					}
					break;
				case nPersonaje::PERSONAJE_LADRONA:
					tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/ladrona/tex");
					if (personajes->At(i)->libre && 
						GetJugadorPrincipal()->GetPersonajeAnterior() != personajes->At(i)->personaje->GetTipoPersonaje() )
					{
						tex->SetTexture(0,"textures:SelLadrona.png",NULL);
					}
					else
					{
						tex->SetTexture(0,"textures:SelLadrona2.png",NULL);
					}
					break;
				case nPersonaje::PERSONAJE_SHUGENJA:
					tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/shugenja/tex");
					if (personajes->At(i)->libre && 
						GetJugadorPrincipal()->GetPersonajeAnterior() != personajes->At(i)->personaje->GetTipoPersonaje() )
					{
						tex->SetTexture(0,"textures:SelShugenja.png",NULL);
					}
					else
					{
						tex->SetTexture(0,"textures:SelShugenja2.png",NULL);
					}
					break;
				case nPersonaje::PERSONAJE_SHOGUN:
					tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/shogun/tex");
					if (personajes->At(i)->libre && 
						GetJugadorPrincipal()->GetPersonajeAnterior() != personajes->At(i)->personaje->GetTipoPersonaje() )
					{
						tex->SetTexture(0,"textures:SelShogun.png",NULL);
					}
					else
					{
						tex->SetTexture(0,"textures:SelShogun2.png",NULL);
					}
					break;
				case nPersonaje::PERSONAJE_MERCADER:
					tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/mercader/tex");
					if (personajes->At(i)->libre && 
						GetJugadorPrincipal()->GetPersonajeAnterior() != personajes->At(i)->personaje->GetTipoPersonaje() )
					{
						tex->SetTexture(0,"textures:SelMercader.png",NULL);
					}
					else
					{
						tex->SetTexture(0,"textures:SelMercader2.png",NULL);
					}
					break;
				case nPersonaje::PERSONAJE_MAESTRO:
					tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/maestro/tex");
					if (personajes->At(i)->libre && 
						GetJugadorPrincipal()->GetPersonajeAnterior() != personajes->At(i)->personaje->GetTipoPersonaje() )
					{
						tex->SetTexture(0,"textures:Selmaestro.png",NULL);
					}
					else
					{
						tex->SetTexture(0,"textures:SelMaestro2.png",NULL);
					}
					break;
				case nPersonaje::PERSONAJE_INGENIERO:
					tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/ingeniero/tex");
					if (personajes->At(i)->libre && 
						GetJugadorPrincipal()->GetPersonajeAnterior() != personajes->At(i)->personaje->GetTipoPersonaje() )
					{
						tex->SetTexture(0,"textures:SelIngeniero.png",NULL);
					}
					else
					{
						tex->SetTexture(0,"textures:SelIngeniero2.png",NULL);
					}
					break;
				case nPersonaje::PERSONAJE_SAMURAI:
					tex=(nTexArrayNode*)kernelServer->Lookup("/game/scene/hud/samurai/tex");
					if (personajes->At(i)->libre && 
						GetJugadorPrincipal()->GetPersonajeAnterior() != personajes->At(i)->personaje->GetTipoPersonaje() )
					{
						tex->SetTexture(0,"textures:SelSamurai.png",NULL);
					}
					else
					{
						tex->SetTexture(0,"textures:SelSamurai2.png",NULL);
					}
					break;
				}
			}
		}
	}
}

// Devuelve el personaje del jugador mas cercano a la posición de entrada
nEntity *nPartida::JugadorMasCercano(float posx, float posy, int id)
{
	nPersonaje *personaje, *personaje_sel=NULL;
	float dist_min=1000.0f;
	vector2 pos_actual,pos_personaje,distancia;
	pos_actual.set(posx,posy);
	for (int i=0;i<jugadores->Size();i++) 
	{
		if ( !jugadores->At( i )->EstaBloqueado() )
		{
			personaje=jugadores->At(i)->GetPersonaje();
			pos_personaje.set(personaje->GetPosition().x,personaje->GetPosition().z);
			distancia=pos_personaje-pos_actual;
			if (distancia.len()<dist_min && jugadores->At(i)->GetId()!=id)
			{
				dist_min=distancia.len();
				personaje_sel=personaje;
			}
		}
	}
	return (nEntity *)personaje_sel;
}

// Devuelve el personaje del jugador o el campesino mas cercano a la posición de entrada
nEntity *nPartida::PersonajeMasCercano(float posx, float posy, int id)
{
	nPersonaje *personaje, *personaje_sel = NULL;
	float dist_min=1000.0f;
	vector2 pos_actual,pos_personaje,distancia;
	pos_actual.set(posx,posy);
	for (int i=0;i<jugadores->Size();i++) 
	{
		if ( !jugadores->At( i )->EstaBloqueado() || 
			 jugadores->At( i )->GetPersonaje()->EstaMuerto() )
		{
			personaje=jugadores->At(i)->GetPersonaje();
			if (!personaje->EstaMuerto() )
			pos_personaje.set(personaje->GetPosition().x,personaje->GetPosition().z);
			distancia=pos_personaje-pos_actual;
			if (distancia.len()<dist_min && jugadores->At(i)->GetId()!=id)
			{
				dist_min=distancia.len();
				personaje_sel=personaje;
			}
		}
	}
	for (int i=0;i<campesinos->Size();i++) 
	{
		personaje=(nPersonaje *)campesinos->At(i);
		pos_personaje.set(personaje->GetPosition().x,personaje->GetPosition().z);
		distancia=pos_personaje-pos_actual;
		if (distancia.len()<dist_min)
		{
			dist_min=distancia.len();
			personaje_sel=personaje;
		}
	}
	return (nEntity *)personaje_sel;
}

// Devuelve el campesino más cercano a la posición de entrada
nEntity *nPartida::CampesinoMasCercano(float posx, float posy, int id)
{
	nPersonaje *personaje, *personaje_sel = NULL;
	float dist_min = 1000.0f;
	vector2 pos_actual, pos_personaje, distancia;
	pos_actual.set( posx, posy );
	for ( int i = 0; i < campesinos->Size(); i++ ) 
	{
		if ( !campesinos->At( i )->EstaContratado() && !campesinos->At( i )->EstaMuerto() )
		{
			personaje = (nPersonaje*) campesinos->At( i );
			pos_personaje.set( personaje->GetPosition().x, personaje->GetPosition().z );
			distancia = pos_personaje - pos_actual;
			if ( distancia.len() < dist_min )
			{
				dist_min      = distancia.len();
				personaje_sel = personaje;
			}
		}
	}
	return (nEntity*) personaje_sel;
}

// Devuelve el campesino muerto más cercano a la posición de entrada
nEntity *nPartida::CampesinoMuertoMasCercano( float posx, float posy, int id )
{
	nPersonaje *personaje     = NULL;
	nPersonaje *personaje_sel = NULL;
	float      dist_min       = 1000.0f;
	vector2    pos_actual, pos_personaje, distancia;

	pos_actual.set( posx, posy );
	for ( int i = 0; i < campesinos->Size(); i++ )
	{
		if ( campesinos->At( i )->EstaMuerto() )
		{
			personaje = (nPersonaje *) campesinos->At( i );
			pos_personaje.set( personaje->GetPosition().x, personaje->GetPosition().z );
			distancia = pos_personaje - pos_actual;
			if ( distancia.len() < dist_min )
			{
				dist_min      = distancia.len();
				personaje_sel = personaje;
			}
		}
	}

	return (nEntity *) personaje_sel;
}

// Devuelve el jugador con más edificios construidos
nJugador *nPartida::JugadorMejor( int id )
{
	nJugador *jugador_sel  = NULL;
	int      num_edificios = 0;

	for ( int i = 0; i < jugadores->Size(); i++ )
	{
		if ( !jugadores->At( i )->EstaBloqueado() || jugadores->At( i )->GetPersonaje()->EstaMuerto() )
		{
			if ( jugadores->At( i )->GetDistrito()->NumeroConstrucciones() > num_edificios && jugadores->At( i )->GetId() != id )
			{
				num_edificios = jugadores->At( i )->GetDistrito()->NumeroConstrucciones();
				jugador_sel   = jugadores->At( i );
			}
		}
	}
	return jugador_sel;
}

nArray<nCampesino *> *nPartida::GetCampesinos()
{
	return campesinos;
}

// Devuelve el ayuntamiento más cercano a la posición de entrada
nEntity *nPartida::AyuntamientoMasCercano( float posx, float posy, int id )
{
	nEntity  *ayuntamiento     = NULL;
	nEntity  *ayuntamiento_sel = NULL;
	float    dist_min          = 1000.0f;
	vector2  pos_actual, pos_ayto, distancia;
	char     path[N_MAXPATH];

	pos_actual.set( posx, posy );
	for ( int i = 0; i < jugadores->Size(); i++ )
	{
		if ( !jugadores->At( i )->EstaBloqueado() && jugadores->At( i )->GetId() != id )
		{
			bool encontrado = false;
			for ( int j = 0; j < jugadores->At( i )->GetDistrito()->GetListaPlanos().Size(); j++ )
			{
				if ( jugadores->At( i )->GetDistrito()->GetListaPlanos().At( j )->GetDistintivo() == nPlano::TPlano::IMPERIAL )
				{
					encontrado = true;
					break;
				}
			}
			if ( encontrado )
			{
				sprintf( path, "/game/world/ayto%d", jugadores->At( i )->GetId() );
				nEntity *ayuntamiento = (nEntity *) kernelServer->Lookup( path );
				pos_ayto.set( ayuntamiento->GetPosition().x, ayuntamiento->GetPosition().z );
				distancia = pos_ayto - pos_actual;
				if ( distancia.len() < dist_min )
				{
					dist_min         = distancia.len();
					ayuntamiento_sel = ayuntamiento;
				}
			}
		}
	}

	return (nEntity *) ayuntamiento_sel;
}

// Devuelve un edificio del jugador con mas edificios
nEntity *nPartida::EdificioMejorJugador(int id)
{
	nEntity *edificio_sel = NULL;
	nDistrito *distrito;
	int num_edificios = 0;
	int edificio_elegido;
	int id_edificio = -1;
	int probabilidad = (rand() % 100) + 1;
	nJugador *jugador_principal = GetJugadorPrincipal();
	distrito = jugador_principal->GetDistrito();

	if ( probabilidad <= 80 && distrito->NumeroConstrucciones() > 0 )
	{
		num_edificios = distrito->NumeroConstrucciones();
		edificio_elegido = rand() % num_edificios;
		nPlano *plano = distrito->GetEdificio( edificio_elegido );
		if ( plano != NULL )
			id_edificio = plano->GetIdEdificio();
	}
	else
	{
		for ( int i = 0; i < jugadores->Size(); i++ ) 
		{
			distrito = jugadores->At( i )->GetDistrito();
			if ( distrito->NumeroConstrucciones() > num_edificios && jugadores->At( i )->GetId() != id )
			{
				num_edificios = distrito->NumeroConstrucciones();
				edificio_elegido = rand() % num_edificios;
				nPlano *plano = distrito->GetEdificio( edificio_elegido );
				if ( plano != NULL )
					id_edificio = plano->GetIdEdificio();
			}
		}
	}
	char path[N_MAXPATH];
	if ( id_edificio > -1 )
	{
		sprintf( path, "/game/world/cons%d", id_edificio );
		edificio_sel = (nEntity *) kernelServer->Lookup( path );
		return edificio_sel;
	}
	else
		return NULL;
}

// Devuelve el ayuntamiento del jugador con mas planos
nEntity *nPartida::JugadorConMasPlanos(int id)
{
	int jugador_sel=id;
	int num_planos=0;
	for (int i=0;i<jugadores->Size();i++) 
	{
		if (jugadores->At(i)->GetNumPlanos()>num_planos && jugadores->At(i)->GetId()!=id)
		{
			num_planos=jugadores->At(i)->GetNumPlanos();
			jugador_sel=jugadores->At(i)->GetId();
		}
	}    
	char path[N_MAXPATH];
	sprintf(path,"/game/world/ayto%d",jugador_sel);
	nEntity *ayuntamiento=(nEntity *)kernelServer->Lookup(path);
	return ayuntamiento;
}

nArray<nJugador*>* nPartida::GetJugadores()
{
	return jugadores;
}

nArray<nPartida::T_Personaje*>* nPartida::GetPersonajes()
{
	return personajes;
}

nJugador* nPartida::GetJugadorConPersonaje( nPersonaje::TPersonaje t_personaje )
{
	for ( int i = 0; i< jugadores->Size(); i++ )
	{
		if (jugadores->At(i)->GetPersonaje()->GetTipoPersonaje() == t_personaje || 
			jugadores->At(i)->GetSiguientePersonaje()->GetTipoPersonaje() == t_personaje || 
			(jugadores->At(i)->GetPersonaje()->GetTipoPersonaje() == nPersonaje::PERSONAJE_CAMPESINO && 
			jugadores->At(i)->GetPersonajeAnterior() == t_personaje ) )
		{
			return jugadores->At(i);
		}
	}
	return NULL;
}

void nPartida::PuntuarPorDinero()
{
	int  dinero_maximo = 0;

	// Se evalúa cuál es la cantidad máxima de dinero en la partida
	for ( int i = 0; i < (int) jugadores->Size(); i++ )
	{
		//n_printf("JUGADOR %d CON %d MONEDAS\n",jugadores->At( i )->GetId(),jugadores->At( i )->GetOro());
		if ( jugadores->At( i )->GetOro() > dinero_maximo )
			dinero_maximo = jugadores->At( i )->GetOro();
	}

	// Se le da los puntos a los jugadores con ese dinero
	for ( int i = 0; i < (int) jugadores->Size(); i++ )
	{
		if ( jugadores->At( i )->GetOro() == dinero_maximo )
			jugadores->At( i )->Puntuar( (int) DINERO, NULL, 0 );
	}
}

void nPartida::PuntuarPorTiempo()
{
	float  tiempo_minimo = 999999.0f;

	// Se evalúa cuál es el tiempo mínimo de personaje en la partida
	for ( int i = 0; i < (int) jugadores->Size(); i++ )
	{
		//n_printf("JUGADOR %d CON %f SEGUNDOS EMPLEADOS EN PERSONAJES\n",jugadores->At( i )->GetId(),jugadores->At( i )->GetTiempoEfectivo());
		if ( jugadores->At( i )->GetTiempoEfectivo() < tiempo_minimo )
			tiempo_minimo = jugadores->At( i )->GetTiempoEfectivo();
	}

	// Se le da los puntos a los jugadores con ese tiempo
	for ( int i = 0; i < (int) jugadores->Size(); i++ )
	{
		if ( jugadores->At( i )->GetTiempoEfectivo() == tiempo_minimo )
			jugadores->At( i )->Puntuar( (int) TIEMPO, NULL, 0 );
	}
}

void nPartida::PuntuarPorDistintivo()
{
	list<nPlano*>::iterator iter;
	bool amarillo = false;
	bool verde    = false;
	bool rojo     = false;
	bool azul     = false;
	bool morado   = false;

	for ( int i = 0; i < (int) jugadores->Size(); i++ )
	{
		nDistrito* distrito = jugadores->At( i )->GetDistrito();
		for ( int j = 0; j < distrito->GetListaPlanos().Size(); j++ )
		{
			if ( distrito->GetEdificio( j ) != NULL )
			{
				switch ( distrito->GetEdificio( j )->GetDistintivo() )
				{
				case nPlano::IMPERIAL:
					amarillo = true;
					break;
				case nPlano::COMERCIAL:
					verde = true;
					break;
				case nPlano::MILITAR:
					rojo = true;
					break;
				case nPlano::RELIGIOSO:
					azul = true;
					break;
				case nPlano::ESPECIAL:
					morado = true;
					break;
				}
			}
		}
		if ( amarillo && verde && rojo && azul && morado )
			jugadores->At( i )->Puntuar( (int) DISTINTIVOS, NULL, 0 );

		amarillo = false;
		verde    = false;
		rojo     = false;
		azul     = false;
		morado   = false;
	}
}

bool nPartida::EsJugadorPrincipal(int jugador)
{
	return jugadores->At(jugador)->GetJugadorPrincipal();
}

int nPartida::GetPuntosPorAccion(int jugador)
{
	return jugadores->At(jugador)->PuntosPorAccion();
}

int nPartida::GetPuntosPorConstruccion(int jugador)
{
	return jugadores->At(jugador)->PuntosPorConstruccion();
}

int nPartida::GetNumConstrucciones(int jugador)
{
	return jugadores->At(jugador)->GetDistrito()->NumeroConstrucciones();
}

int nPartida::GetPuntosPorDistintivo(int jugador)
{
	return jugadores->At(jugador)->PuntosPorDistintivo();
}

int nPartida::GetPuntosPorOro(int jugador)
{
	return jugadores->At(jugador)->PuntosPorDinero();
}

int nPartida::GetPuntosPorTiempo(int jugador)
{
	return jugadores->At(jugador)->PuntosPorTiempo();
}

int nPartida::GetPuntosTotales(int jugador)
{
	return jugadores->At(jugador)->GetPuntos().At(N_MAXTIPODEPUNTOS-1).puntos;
}