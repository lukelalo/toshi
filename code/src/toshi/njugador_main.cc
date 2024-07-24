#define N_IMPLEMENTS nJugador
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/npartida.h"
#include "toshi/njugador.h"
#include "text/nlabel.h"
#include "toshi/ncampesino.h"
#include "toshi/nayuntamiento.h"
#include "toshi/nconstruccion.h"
#include "toshi/ncasaplanos.h"
#include "toshi/ncasamoneda.h"
#include "toshi/npersonaje.h"
#include "toshi/nkunoichi.h"
#include "toshi/nladrona.h"
#include "toshi/nshugenja.h"
#include "toshi/nshogun.h"
#include "toshi/nmaestro.h"
#include "toshi/nmercader.h"
#include "toshi/ningeniero.h"
#include "toshi/nsamurai.h"
#include "toshi/ncamera.h"
#include "toshi/ngame.h"
#include "toshi/nworld.h"

#include <list>
using namespace std;

nNebulaScriptClass( nJugador, "nroot");

// Constructor general
nJugador::nJugador(): refScriptServer( kernelServer, this ),
					  refChannelServer( kernelServer, this ),
					  planos(),
					  ModoConstruccion( false ),
					  TienePersonaje( false ),
					  plano_a_construir( -1 ),
					  EdificioAConstruir( "" )
{
	refScriptServer       = "/sys/servers/script";
	refChannelServer      = "/sys/servers/channel";
	Nombre				  = "";
	PersonajeActivo       = nPersonaje::PERSONAJE_CAMPESINO;
	PersonajeAnterior     = nPersonaje::PERSONAJE_CAMPESINO;
	Tiempo				  = 0.0f;
	TiempoEfectivo        = 0.0f;
	TiempoSeleccion		  = 0.0f;
	TiempoAnterior		  = 0.0f;
	TiempoInformacion	  = 0.0f;
	CanalTiempoTurno	  = refChannelServer->GenChannel( "tiempoTurno" );
	CanalTiempoPago 	  = refChannelServer->GenChannel( "tiempoTurnoPago" );
	timeChannel			  = refChannelServer->GenChannel( "time" );
	Oro      			  = 0;
	CampesinosContratados = 0;
	CampesinosLiberados   = 0;
	CampesinosResucitados = 0;
	PersonajesRobados     = 0;
	PersonajesAsesinados  = 0;
	EdificiosDerribados   = 0;
	AccionActual.accion.numAccion  = 0;
	IdJugador			  =	-1;
	JugadorActivo		  = false;
	JugadorPrincipal	  = false;
	JugadorBloqueado      = false;
	JugadorMuerto		  = false;
	BarraBloqueada		  = true;
	personaje             = NULL;
	personajeOculto		  = NULL;
	distrito              = NULL;
	AccionesPorTurno.Asesinatos           = 0;
	AccionesPorTurno.Construcciones       = 0;
	AccionesPorTurno.Resurrecciones       = 0;
	AccionesPorTurno.Robos                = 0;
	AccionesPorTurno.VisitasAAyuntamiento = 0;
	AccionesPorTurno.VisitasACasaMoneda   = 0;
	AccionesPorTurno.VisitasACasaPlanos   = 0;
	AccionesPorTurno.Intercambios         = 0;
	AccionesPorTurno.Recaudaciones        = 0;
	AccionesPorTurno.Liberaciones         = 0;
	planos.SetFixedSize( N_MAXPLANOS );
	for ( int i = 0; i < N_MAXPLANOS; i++ )
		planos.At( i ) =  NULL;
/*	aytos_recaudados.SetFixedSize( N_MAXPLANOS - 1 );
	for ( int i = 0; i < N_MAXPLANOS - 1 ; i++ )
	*/
	//aytos_recaudados.Set( i , -1 );
	aytos_recaudados.Clear();
	for ( int i = 0; i < N_MAXTIPODEPUNTOS; i++ )
	{
		switch( i )
		{
			case 0 : Puntos.At( i ).categoria = CONTRATOS;
					 Puntos.At( i ).puntos = 0;
					 break;
			case 1 : Puntos.At( i ).categoria = DESPIDOS;
					 Puntos.At( i ).puntos = 0;
					 break;
			case 2 : Puntos.At( i ).categoria = RESURRECCIONES;
					 Puntos.At( i ).puntos = 0;
					 break;
			case 3 : Puntos.At( i ).categoria = ROBOS;
					 Puntos.At( i ).puntos = 0;
					 break;
			case 4 : Puntos.At( i ).categoria = ASESINATOS;
					 Puntos.At( i ).puntos = 0;
					 break;
			case 5 : Puntos.At( i ).categoria = CONSTRUCCIONES;
					 Puntos.At( i ).puntos = 0;
					 break;
			case 6 : Puntos.At( i ).categoria = DESTRUCCIONES;
					 Puntos.At( i ).puntos = 0;
					 break;
			case 7 : Puntos.At( i ).categoria = DISTINTIVOS;
					 Puntos.At( i ).puntos = 0;
					 break;
			case 8 : Puntos.At( i ).categoria = DINERO;
					 Puntos.At( i ).puntos = 0;
					 break;
			case 9 : Puntos.At( i ).categoria = TIEMPO;
					 Puntos.At( i ).puntos = 0;
					 break;
			case 10: Puntos.At( i ).categoria = TOTAL;
					 Puntos.At( i ).puntos = 0;
					 break;
		}
	}
}

// Destructor
nJugador::~nJugador()
{
	for (int i=0;i<this->planos.Size();i++)
	{
		n_free( this->planos.At(i) );
	}
	this->planos.Clear();
	this->Puntos.Clear();
	VaciarPilaAcciones();
	n_free(this->distrito);
	n_free(personaje);
	n_free(personajeOculto);
	n_free(siguientePersonaje);
	n_free(this->partida);
}

void nJugador::SetTiempo( float tiempo )
{
	Tiempo = tiempo;
}

void nJugador::SetNombre( const char* nombre_jugador )
{
	Nombre = nombre_jugador;
}

void nJugador::SetCampesinosContratados( int campesinos )
{
	CampesinosContratados = campesinos;
}

void nJugador::SetCampesinosLiberados( int campesinos )
{
	CampesinosLiberados = campesinos;
}

void nJugador::SetCampesinosResucitados( int campesinos )
{
	CampesinosResucitados = campesinos;
}

bool nJugador::PuedeContratarCampesino( nCampesino* &campesino )
{
	if ( !campesino->EstaContratado() && !campesino->EstaMuerto() && GetOro() >= campesino->GetPrecio() )
		return true;
	return false;
}

void nJugador::ContratarCampesino( nCampesino* &campesino )
{
	if ( this->EstaBloqueado() )
		return;

	if ( PuedeContratarCampesino( campesino ) )
	{
		if ( PersonajeActivo == nPersonaje::PERSONAJE_KUNOICHI )
		{
			nKunoichi* kunoichi = (nKunoichi*) personaje;
			kunoichi->ContratarCampesino( campesino );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_LADRONA )
		{
			nLadrona* ladrona = (nLadrona*) personaje;
			ladrona->ContratarCampesino( campesino );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_SHUGENJA )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_COGER );
			nShugenja* shugenja = (nShugenja*) personaje;
		    shugenja->ContratarCampesino( campesino );
			SetOro( GetOro() - campesino->GetPrecio() );
			Puntuar( CONTRATOS, NULL, 0 );
			SetCampesinosContratados( GetCampesinosContratados() + 1 );
			personaje->ContratarCampesino( campesino->GetId() );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_SHOGUN )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_COGER );
			nShogun* shogun = (nShogun*) personaje;
			shogun->ContratarCampesino( campesino );
			SetOro( GetOro() - campesino->GetPrecio() );
			Puntuar( CONTRATOS, NULL, 0 );
			SetCampesinosContratados( GetCampesinosContratados() + 1 );
			personaje->ContratarCampesino( campesino->GetId() );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_MAESTRO )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_COGER );
			nMaestro* maestro = (nMaestro*) personaje;
			maestro->ContratarCampesino( campesino );
			SetOro( GetOro() - campesino->GetPrecio() );
			Puntuar( CONTRATOS, NULL, 0 );
			SetCampesinosContratados( GetCampesinosContratados() + 1 );
			personaje->ContratarCampesino( campesino->GetId() );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_MERCADER )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_COGER );
			nMercader* mercader = (nMercader*) personaje;
			mercader->ContratarCampesino( campesino );
			SetOro( GetOro() - campesino->GetPrecio() );
			Puntuar( CONTRATOS, NULL, 0 );
			SetCampesinosContratados( GetCampesinosContratados() + 1 );
			personaje->ContratarCampesino( campesino->GetId() );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_INGENIERO )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_COGER );
			nIngeniero* ingeniero = (nIngeniero*) personaje;
			ingeniero->ContratarCampesino( campesino );
			SetOro( GetOro() - campesino->GetPrecio() );
			Puntuar( CONTRATOS, NULL, 0 );
			SetCampesinosContratados( GetCampesinosContratados() + 1 );
			personaje->ContratarCampesino( campesino->GetId() );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_SAMURAI )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_COGER );
            nSamurai* samurai = (nSamurai*) personaje;
			samurai->ContratarCampesino( campesino );
			SetOro( GetOro() - campesino->GetPrecio() );
			Puntuar( CONTRATOS, NULL, 0 );
			SetCampesinosContratados( GetCampesinosContratados() + 1 );
			personaje->ContratarCampesino( campesino->GetId() );
		}
	}
}

void nJugador::LiberarCampesino( nCampesino* &campesino )
{
	if ( this->EstaBloqueado() )
		return;

	if ( campesino->EstaContratado() && personaje->CampesinoMio( campesino->GetId() ) )
	{
		if ( PersonajeActivo == nPersonaje::PERSONAJE_KUNOICHI )
		{
			nKunoichi* kunoichi = (nKunoichi*) personaje;
			kunoichi->LiberarCampesino( campesino );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_LADRONA )
		{
			nLadrona* ladrona = (nLadrona*) personaje;
			ladrona->LiberarCampesino( campesino );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_SHUGENJA )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_COGER );
			nShugenja* shugenja = (nShugenja*) personaje;
		    shugenja->LiberarCampesino( campesino );
			personaje->LiberarCampesino( campesino->GetId() );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_SHOGUN )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_COGER );
			nShogun* shogun = (nShogun*) personaje;
			shogun->LiberarCampesino( campesino );
			personaje->LiberarCampesino( campesino->GetId() );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_MAESTRO )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_COGER );
			nMaestro* maestro = (nMaestro*) personaje;
			maestro->LiberarCampesino( campesino );
			personaje->LiberarCampesino( campesino->GetId() );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_MERCADER )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_COGER );
			nMercader* mercader = (nMercader*) personaje;
			mercader->LiberarCampesino( campesino );
			personaje->LiberarCampesino( campesino->GetId() );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_INGENIERO )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_COGER );
			nIngeniero* ingeniero = (nIngeniero*) personaje;
			ingeniero->LiberarCampesino( campesino );
			personaje->LiberarCampesino( campesino->GetId() );
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_SAMURAI )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_COGER );
            nSamurai* samurai = (nSamurai*) personaje;
			samurai->LiberarCampesino( campesino );
			personaje->LiberarCampesino( campesino->GetId() );
		}
	}
}

void nJugador::CogerPlano( nPlano* &plano )
{
	TAccion* accionActual = pila_acciones.Top();
	vector2 posicion;

	for ( int i = 0; i < N_MAXPLANOS; i++ )
	{
		if ( planos.At( i ) == NULL )
		{
			plano->SetIdHUD( i );
			plano->SetPlanoHUD();
			posicion = plano->GetPosition();
			posicion.x = i * 0.0875 + 0.28;
			plano->SetPosition( posicion );
			planos.Set( i, plano );
			if ( GetJugadorPrincipal() )
				plano->Pintar();

			// Sonido de coger plano
			nGame *juego=(nGame *) kernelServer->Lookup("/game");
			juego->Play3DSound(0,"sounds:posicional/cogerplano.wav",accionActual->destino->GetPosition());

			break;
		}
	}
}

void nJugador::LiberarPlano( int id_plano )
{
	if ( this->EstaBloqueado() )
		return;

	if ( distrito->ExisteEdificio( planos.At( id_plano )->GetNombre() ) && !LoTieneConstruido( "ESCUELA IMPERIAL" ) )
	{
		//n_printf( "YA SE HA CONSTRUIDO EL EDIFICIO %s CON ANTERIORIDAD\n", planos.At( id_plano )->GetNombre().c_str() );
		MostrarInformacion("Ya tienes ese edificio construido");
	}
	else
	{
		plano_a_construir = id_plano;
		EdificioAConstruir = planos.At( id_plano )->Construir();
		nConstruccion* edificio = (nConstruccion*) kernelServer->Lookup( EdificioAConstruir.Get() );
		edificio->SetNombre( planos.At( id_plano )->GetNombre().c_str() );
		SetModoConstruccion( true );
		edificio->SetVisible( true );
		edificio->CambiarShader( "valido.n" );
	}
}

void nJugador::SetOro( int oro )
{
	Oro = oro;
	if ( GetJugadorPrincipal() )
	{
		nTexArrayNode* texto_num;
		char           buf[100];
		sprintf( buf, "textures:numero%d.png", oro % 10 );
		texto_num = (nTexArrayNode*) kernelServer->Lookup( "/game/scene/hud/numero2/tex" );
		texto_num->SetTexture( 0, buf, NULL );
		texto_num = (nTexArrayNode*) kernelServer->Lookup( "/game/scene/hud/numero1/tex" );
		if ( oro > 9 )
		{
			sprintf( buf, "textures:numero%d.png", (int) (oro/10) % 10 );
			texto_num->SetTexture( 0, buf, NULL );
		}
		else
			texto_num->SetTexture( 0, "textures:trans.png", NULL );
		texto_num = (nTexArrayNode*) kernelServer->Lookup( "/game/scene/hud/numero0/tex" );
		if ( oro > 99 )
		{
			sprintf( buf, "textures:numero%d.png", (int) (oro/100) % 10 );
			texto_num->SetTexture( 0, buf, NULL );
		}
		else
			texto_num->SetTexture( 0, "textures:trans.png", NULL );
	}
}

void nJugador::SetPuntos( int tipo, int cuantos )
{
	for ( int i = 0; i < N_MAXTIPODEPUNTOS; i++ )
	{
		if ( Puntos.At( i ).categoria == (nJugador::Puntuacion) tipo )
		{
			Puntos.At( i ).puntos = GetPuntos().At( i ).puntos + cuantos;
			Puntos.At(N_MAXTIPODEPUNTOS-1).puntos = Puntos.At(N_MAXTIPODEPUNTOS-1).puntos + cuantos;
			break;
		}
	}
}

void nJugador::SetPersonajesRobados( int personajes_robados )
{
	PersonajesRobados = personajes_robados;
}

void nJugador::SetPersonajesAsesinados( int personajes_asesinados )
{
	PersonajesAsesinados = personajes_asesinados;
}

void nJugador::SetEdificiosDerribados( int edificios_derribados )
{
	EdificiosDerribados = edificios_derribados;
}

void nJugador::LiberarPersonaje( bool finTurno )
{
	if ( !partida->PuedeLiberarPersonaje( GetId() ) )
		return;

	nCampesino*     campesino = NULL;
	int             id_campesino;
	char            path[N_MAXPATH];
	nTexArrayNode*  nombrepersonaje;
	nArray<int>     lista_campesinos;

	TienePersonaje = false;
	lista_campesinos = personaje->GetListaCampesinosContratados();

	// Se liberan todos los campesinos contratados
	while ( lista_campesinos.Size() > 0 )
	{
		id_campesino = lista_campesinos.At( 0 );
		lista_campesinos.Erase( 0 );
		sprintf( path, "/game/world/campesino%d", id_campesino );
		campesino = (nCampesino*) kernelServer->Lookup( path );
		LiberarCampesino( campesino );
	}

	AccionesPorTurno.Asesinatos           = 0;
	AccionesPorTurno.Construcciones       = 0;
	AccionesPorTurno.Resurrecciones       = 0;
	AccionesPorTurno.Robos                = 0;
	AccionesPorTurno.VisitasAAyuntamiento = 0;
	AccionesPorTurno.VisitasACasaMoneda   = 0;
	AccionesPorTurno.VisitasACasaPlanos   = 0;
	AccionesPorTurno.Intercambios         = 0;
	AccionesPorTurno.Recaudaciones        = 0;
	AccionesPorTurno.Liberaciones         = 0;

	// Se para el modelo en caso de que estuviera en movimiento
	//personaje->Parar();

	if ( finTurno )
	{
		PersonajeAnterior = PersonajeActivo;
		PersonajeActivo = nPersonaje::PERSONAJE_CAMPESINO;
		if ( GetJugadorPrincipal() )
		{
			// Se borra el nombre del personaje
			sprintf( path, "textures:trans.png" );
			nombrepersonaje = (nTexArrayNode*) kernelServer->Lookup( "/game/scene/hud/nombrepersonaje/tex" );
			nombrepersonaje->SetTexture( 0, path, NULL );
			// Se borra la cara del personaje
			sprintf( path, "textures:trans.png" );
			nombrepersonaje = (nTexArrayNode*) kernelServer->Lookup( "/game/scene/hud/cara/tex" );
			nombrepersonaje->SetTexture( 0, path, NULL );
		}
	}

	if ( GetJugadorPrincipal() )
	{
		refChannelServer->SetChannel1f( CanalTiempoTurno, 1.0f );
		refChannelServer->SetChannel1f( CanalTiempoPago, 0.0f );
	}

	this->TienePersonaje = false;

	// Paramos al personaje si está andando
	this->personaje->Parar();

	// Bloqueamos al personaje para que no jorobe la animación del cambio
	//n_printf("Bloqueamos al jugador %d por liberacion de personaje\n",GetId());
	this->Bloquear();

	//personaje->SetAnimacion( nPersonaje::ANIM_CAMBIAR_PERSONAJE );
	this->SacarPersonaje();

	// FMOD: Sonido al liberar al personaje
	SonidoPersonaje(LIBERAR_PERSONAJE);
}

void nJugador::MeterPersonaje()
{
	if ( !this->TienePersonaje )
	{
		partida->LiberarPersonaje( GetId() );
	}

	// Revivo a mi anterior personaje
	this->personaje->Revivir();

	// Cambio el personaje
	this->SetPersonaje(siguientePersonaje);

	// Revivo a mi personaje actual
	this->personaje->Revivir();

	// Muestro el nuevo personaje
	personaje->SetAnimacion( nPersonaje::ANIM_CAMBIAR_PERSONAJE );

	// FMOD: Sonido al cambiar de personaje
	SonidoPersonaje(CAMBIAR_PERSONAJE);

	// Mostramos la sombra del personaje
	personaje->MostrarSombra();

	//n_printf("Metiendo el nuevo personaje del jugador %d\n", GetId());
}

void nJugador::SacarPersonaje()
{
	// Saco el personaje antiguo de la escena
	personaje->SetAnimacion( nPersonaje::ANIM_LIBERAR_PERSONAJE );

	// Ocultamos la sombra del personaje
	personaje->OcultarSombra();

	//n_printf("Sacando el personaje del jugador %d\n", GetId());
}

void nJugador::CambiarPersonaje( nPersonaje::TPersonaje n_personaje )
{
	char            path[N_MAXPATH];
	nTexArrayNode*  nombrepersonaje;

	if ( PersonajeActivo != n_personaje )
	{
		PersonajeAnterior = PersonajeActivo;

		//LiberarPersonaje( true );
		TienePersonaje = false;

		// Paramos al personaje si está andando
		this->personaje->Parar();

		// Bloqueamos al personaje para que no jorobe la animación del cambio
		//n_printf("Bloqueamos al jugador %d por cambio de personaje\n",GetId());
		this->Bloquear();

		// Se selecciona el personaje activo
		PersonajeActivo = n_personaje;

		partida->BloqueaPersonaje( n_personaje, GetId() );
		TienePersonaje = true;

		if ( GetJugadorPrincipal() )
		{
			// Se cambia el nombre del personaje en la pantalla
			sprintf( path, "textures:name%d.png", n_personaje );
			nombrepersonaje = (nTexArrayNode*) kernelServer->Lookup( "/game/scene/hud/nombrepersonaje/tex" );
			nombrepersonaje->SetTexture( 0, path, NULL );
			// Se cambia la cara del personaje en la pantalla
			sprintf( path, "textures:p_%d.png", n_personaje );
			nombrepersonaje = (nTexArrayNode*) kernelServer->Lookup( "/game/scene/hud/cara/tex" );
			nombrepersonaje->SetTexture( 0, path, NULL );
		}

		// Se inicializa el tiempo de uso del personaje
		siguientePersonaje->SetTiempo( 0.0f );
		siguientePersonaje->SetTiempoPago( 0.0f );

		// Vaciamos el array de ayuntamientos recaudados
		aytos_recaudados.Clear();

		if ( GetJugadorPrincipal() )
		{
			refChannelServer->SetChannel1f( CanalTiempoTurno, 0.0f );
			refChannelServer->SetChannel1f( CanalTiempoPago, 0.0f );
		}
		//n_printf( "Tiempo Total Partida = %f   Tiempo Efectivo Jugador %d = %f\n", GetTiempo(), GetId(), GetTiempoEfectivo() );
		//n_printf( "Puntos Total Jugador %d = %d\n", GetId(), GetPuntos().At( N_MAXTIPODEPUNTOS-1 ).puntos );
		//personaje->SetAnimacion( nPersonaje::ANIM_CAMBIAR_PERSONAJE );
		this->SacarPersonaje();
	}
}

bool nJugador::SeleccionarPersonaje( float x, float y )
{
	if ( x > 0.0 && x < 0.22 && y > 0.73 && y < 1.0 )
	{
		if ( PersonajeActivo != nPersonaje::PERSONAJE_CAMPESINO )
		{
			AccionLiberarPersonaje();
			return true;
		}
	}
    else if ( x > 0.01 && x < 0.08 && y > 0.01 && y < 0.08 )
	{
		if ( PersonajeActivo != nPersonaje::PERSONAJE_CAMPESINO ||
			 PersonajeAnterior == nPersonaje::PERSONAJE_KUNOICHI )
		{
			return true;
		}
		if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_KUNOICHI ) )
		{
			AccionCambiarPersonaje( nPersonaje::PERSONAJE_KUNOICHI );
		}
		return true;
    }
    else if ( x > 0.01 && x < 0.08 && y > 0.085 && y < 0.165 )
	{
		if ( PersonajeActivo != nPersonaje::PERSONAJE_CAMPESINO ||
			 PersonajeAnterior == nPersonaje::PERSONAJE_LADRONA )
		{
			return true;
		}
		if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_LADRONA ) )
		{
			AccionCambiarPersonaje( nPersonaje::PERSONAJE_LADRONA );
		}
		return true;
    }
	else if ( x > 0.01 && x < 0.08 && y > 0.17 && y < 0.25 )
	{
		if ( PersonajeActivo != nPersonaje::PERSONAJE_CAMPESINO ||
			 PersonajeAnterior == nPersonaje::PERSONAJE_SHUGENJA )
		{
			return true;
		}
		if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHUGENJA ) )
		{
			AccionCambiarPersonaje( nPersonaje::PERSONAJE_SHUGENJA );
		}
		return true;
	}
	else if ( x > 0.01 && x < 0.08 && y > 0.255 && y < 0.335 )
	{
		if ( PersonajeActivo != nPersonaje::PERSONAJE_CAMPESINO ||
			 PersonajeAnterior == nPersonaje::PERSONAJE_SHOGUN )
		{
			return true;
		}
		if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHOGUN ) )
		{
			AccionCambiarPersonaje( nPersonaje::PERSONAJE_SHOGUN );
		}
		return true;
	}
	else if ( x > 0.01 && x < 0.08 && y > 0.34 && y < 0.42 )
	{
		if ( PersonajeActivo != nPersonaje::PERSONAJE_CAMPESINO ||
			 PersonajeAnterior == nPersonaje::PERSONAJE_MAESTRO )
		{
			return true;
		}
		if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MAESTRO ) )
		{
			AccionCambiarPersonaje( nPersonaje::PERSONAJE_MAESTRO );
		}
		return true;
    }
	else if ( x > 0.01 && x < 0.08 && y > 0.425 && y < 0.505 )
	{
		if ( PersonajeActivo != nPersonaje::PERSONAJE_CAMPESINO ||
			 PersonajeAnterior == nPersonaje::PERSONAJE_MERCADER )
		{
			return true;
		}
		if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MERCADER ) )
		{
			AccionCambiarPersonaje( nPersonaje::PERSONAJE_MERCADER );
		}
		return true;
    }
	else if ( x > 0.01 && x < 0.08 && y > 0.51 && y < 0.59 )
	{
		if ( PersonajeActivo != nPersonaje::PERSONAJE_CAMPESINO ||
			 PersonajeAnterior == nPersonaje::PERSONAJE_INGENIERO )
		{
			return true;
		}
		if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_INGENIERO ) )
		{
			AccionCambiarPersonaje( nPersonaje::PERSONAJE_INGENIERO );
		}
		return true;
    }
	else if ( x > 0.01 && x < 0.08 && y > 0.595 && y < 0.675 )
	{
		if ( PersonajeActivo != nPersonaje::PERSONAJE_CAMPESINO ||
			 PersonajeAnterior == nPersonaje::PERSONAJE_SAMURAI )
		{
			return true;
		}
		if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SAMURAI ) )
		{
			AccionCambiarPersonaje( nPersonaje::PERSONAJE_SAMURAI );
			return true;
		}
		return true;
    }
	return false;
}

bool nJugador::ManejadorRaton( float x, float y )
{
	if ( !this->personaje->EstaMuerto() )
	{
		char path[N_MAXPATH];

		if ( SeleccionarPersonaje( x, y ) )
			return true;
		else
		{
			for ( int i = 0; i < N_MAXPLANOS; i++ )
			{
				if ( planos.At( i ) != NULL )
				{
					if ( planos.At( i )->Click( x, y ) )
					{
						sprintf( path, "jugador%d", GetId() );
						if ( PersonajeActivo == nPersonaje::PERSONAJE_CAMPESINO )
						{
							//n_printf( "SIENDO CAMPESINO NO SE PUEDE CONSTRUIR\n" );
							MostrarInformacion("Siendo campesino no se puede construir");
						}
						else
						{
							// Si el jugador no ha realizado tres construcciones en el turno
							if ( AccionesPorTurno.Construcciones < 3 )
							{
								if ( PersonajeActivo == nPersonaje::PERSONAJE_INGENIERO )
								{
									int campesinos_contratados = personaje->GetCampesinosContratados();
									int reduccion_coste = (campesinos_contratados*planos.At( i )->GetCoste())/10;
									int coste = planos.At( i )->GetCoste() - reduccion_coste;
									// Si se tiene dinero para construir
									if ( Oro >= coste )
									{
										LiberarPlano( i );
										return true;
									}
									else
									{
										//n_printf( "NO TIENE SUFICIENTE DINERO PARA CONSTRUIR\n" );
										MostrarInformacion("No tienes suficiente dinero para construir");
									}
								}
								else
								{
									// Si el jugador no ha realizado tres construcciones en el turno
									if ( AccionesPorTurno.Construcciones < 2 )
									{
										// Si se tiene dinero para construir
										if ( Oro >= planos.At( i )->GetCoste() )
										{
											LiberarPlano( i );
											return true;
										}
										else
										{
											//n_printf( "NO TIENE SUFICIENTE DINERO PARA CONSTRUIR\n" );
											MostrarInformacion("No tienes suficiente dinero para construir");
										}
									}
									else
									{
										//n_printf( "YA SE HAN REALIZADO DOS CONSTRUCCIONES\n" );
										MostrarInformacion("No puedes construir mas en este turno");
									}
								}
							}
							else
							{
								//n_printf( "YA SE HAN REALIZADO TRES CONSTRUCCIONES\n" );
								MostrarInformacion("No puedes construir mas en este turno");
							}
						}
						return true;
					}
				}
			}
		}
	}
	return false;
}

int nJugador::PunteroRaton( float x, float y, bool p_ayuda )
{
	char*   params = NULL;
	char    buf[N_MAXPATH];
	nString i_titulo    = "";
	nString i_accion1   = "";
	nString i_accion2   = "";
	nString i_especial1 = "";
	nString i_especial2 = "";
	nString i_foto      = "";
	int     i_cursor    = -1;

    if ( x > 0.01 && x < 0.08 && y > 0.01 && y < 0.08 )
	{
		if ( personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_CAMPESINO &&
			 partida->PersonajeLibre( nPersonaje::PERSONAJE_KUNOICHI ) &&
			 this->GetPersonajeAnterior() != nPersonaje::PERSONAJE_KUNOICHI )
		{
			if ( p_ayuda )
			{
				i_titulo    = "Kunoichi";
				i_accion1   = "Btn Izdo: Accion normal";
				i_accion2   = "Btn Dcho: Asesina";
				i_especial1 = "";
				i_especial2 = "";
				i_foto      = "textures:f_kunoichi.png";
			}
		}
		i_cursor = 0;
    }
    else if ( x > 0.01 && x < 0.08 && y > 0.085 && y < 0.165 )
	{
		if ( personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_CAMPESINO &&
			 partida->PersonajeLibre( nPersonaje::PERSONAJE_LADRONA ) &&
			 this->GetPersonajeAnterior() != nPersonaje::PERSONAJE_LADRONA )
		{
			if ( p_ayuda )
			{
				i_titulo    = "Ladrona";
				i_accion1   = "Btn Izdo: Accion normal";
				i_accion2   = "Btn Dcho: Roba";
				i_especial1 = "";
				i_especial2 = "";
				i_foto      = "textures:f_ladrona.png";
			}
		}
		i_cursor = 0;
    }
	else if ( x > 0.01 && x < 0.08 && y > 0.17 && y < 0.25 )
	{
		if ( personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_CAMPESINO &&
			 partida->PersonajeLibre( nPersonaje::PERSONAJE_SHUGENJA ) &&
			 this->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHUGENJA )
		{
			if ( p_ayuda )
			{
				i_titulo    = "Shugenja";
				i_accion1   = "Btn Izdo: Accion normal";
				i_accion2   = "Btn Dcho: Cambia planos/resucita";
				i_especial1 = "";
				i_especial2 = "";
				i_foto      = "textures:f_shugenja.png";
			}
		}
		i_cursor = 0;
	}
	else if ( x > 0.01 && x < 0.08 && y > 0.255 && y < 0.335 )
	{
		if ( personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_CAMPESINO &&
			 partida->PersonajeLibre( nPersonaje::PERSONAJE_SHOGUN ) &&
			 this->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHOGUN )
		{
			if ( p_ayuda )
			{
				i_titulo    = "Shogun";
				i_accion1   = "Btn Izdo: Accion normal";
				i_accion2   = "Btn Dcho: Nada";
				i_especial1 = "Especial: Puede elegir el siguiente ";
				i_especial2 = "personaje sin restricciones";
				i_foto      = "textures:f_shogun.png";
			}
		}
		i_cursor = 0;
	}
	else if ( x > 0.01 && x < 0.08 && y > 0.34 && y < 0.42 )
	{
		if ( personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_CAMPESINO &&
			 partida->PersonajeLibre( nPersonaje::PERSONAJE_MAESTRO ) &&
			 this->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MAESTRO )
		{
			if ( p_ayuda )
			{
				i_titulo    = "Maestro";
				i_accion1   = "Btn Izdo: Accion normal";
				i_accion2   = "Btn Dcho: Nada";
				i_especial1 = "Especial: Al maestro no se le";
				i_especial2 = "pueden destruir los edificios";
				i_foto      = "textures:f_maestro.png";
			}
		}
		i_cursor = 0;
    }
	else if ( x > 0.01 && x < 0.08 && y > 0.425 && y < 0.505 )
	{
		if ( personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_CAMPESINO &&
			 partida->PersonajeLibre( nPersonaje::PERSONAJE_MERCADER ) &&
			 this->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MERCADER )
		{
			if ( p_ayuda )
			{
				i_titulo    = "Mercader";
				i_accion1   = "Btn Izdo: Accion normal";
				i_accion2   = "Btn Dcho: Nada";
				i_especial1 = "Especial: Cobra el doble de";
				i_especial2 = "dinero";
				i_foto      = "textures:f_mercader.png";
			}
		}
		i_cursor = 0;
    }
	else if ( x > 0.01 && x < 0.08 && y > 0.51 && y < 0.59 )
	{
		if ( personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_CAMPESINO &&
			 partida->PersonajeLibre( nPersonaje::PERSONAJE_INGENIERO ) &&
			 this->GetPersonajeAnterior() != nPersonaje::PERSONAJE_INGENIERO )
		{
			if ( p_ayuda )
			{
				i_titulo    = "Ingeniero";
				i_accion1   = "Btn Izdo: Accion normal";
				i_accion2   = "Btn Dcho: Nada";
				i_especial1 = "Especial: Consigue el doble de ";
				i_especial2 = "planos y puede construir 3 veces";
				i_foto      = "textures:f_ingeniero.png";
			}
		}
		i_cursor = 0;
    }
	else if ( x > 0.01 && x < 0.08 && y > 0.595 && y < 0.675 )
	{
		if ( personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_CAMPESINO &&
			 partida->PersonajeLibre( nPersonaje::PERSONAJE_SAMURAI ) &&
			 this->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SAMURAI )
		{
			if ( p_ayuda )
			{
				i_titulo    = "Samurai";
				i_accion1   = "Btn Izdo: Accion normal";
				i_accion2   = "Btn Dcho: Destruir edificio";
				i_especial1 = "";
				i_especial2 = "";
				i_foto      = "textures:f_samurai.png";
			}
		}
		i_cursor = 0;
    }

	if ( !( PersonajeActivo == nPersonaje::PERSONAJE_CAMPESINO ) &&
				x > 0.0 && x < 0.22 && y > 0.73 && y < 1.0 )
	{
		i_cursor = 0;
		i_titulo = "";
		if ( p_ayuda )
		{
			sprintf( buf, "MostrarInformacionContextual \"Liberar al personaje\" %f %f",x,y);
			refScriptServer->Run( buf, params );
		}
	}
	else
	{
		sprintf( buf, "OcultarInformacionContextual");
		refScriptServer->Run( buf, params );
	}

	for ( int i = 0; i < N_MAXPLANOS; i++ )
	{
		if ( planos.At( i ) != NULL )
		{
			if ( planos.At( i )->Click( x, y ) )
			{
				i_titulo = planos.At( i )->GetNombre().c_str();
				sprintf( buf, "Valor: %d", planos.At( i )->GetValor() );
				i_accion1 = buf;
				sprintf( buf, "Coste: %d", planos.At( i )->GetCoste() );
				i_accion2 = buf;
				if ( !planos.At( i )->GetDescripcion().empty() )
				{
					sprintf( buf, "Especial: %s", planos.At( i )->GetDescripcion().c_str() );
					i_especial1 = buf;
				}
				else
					i_especial1 = "";

				if ( !planos.At( i )->GetDescripcion2().empty() )
				{
					sprintf( buf, "%s", planos.At( i )->GetDescripcion2().c_str() );
					i_especial2 = buf;
				}
				else
					i_especial2 = "";

				sprintf( buf, "textures:pergamino%d%d.png", (int) planos.At( i )->GetDistintivo(), (int) (planos.At( i )->GetCoste()/10) );
				i_foto = buf;
				i_cursor = 3;
			}
		}
	}

	// Mostramos la información del plano que se encuentra seleccionado :)
	if ( !( i_titulo == "" ) )
	{
		sprintf( buf, "SetTituloInfo \"%s\"", i_titulo.Get() );
		refScriptServer->Run( buf, params );
		sprintf( buf, "SetAccion1Info \"%s\"", i_accion1.Get() );
		refScriptServer->Run( buf, params );
		sprintf( buf, "SetAccion2Info \"%s\"", i_accion2.Get() );
		refScriptServer->Run( buf, params );
		sprintf( buf, "SetEspecial1Info \"%s\"", i_especial1.Get() );
		refScriptServer->Run( buf, params );
		sprintf( buf, "SetEspecial2Info \"%s\"", i_especial2.Get() );
		refScriptServer->Run( buf, params );
		sprintf( buf, "SetFotoInfo \"%s\"", i_foto.Get() );
		refScriptServer->Run( buf, params );
		sprintf( buf, "MostrarInformacion" );
		refScriptServer->Run( buf, params );
	}
	else if ( i_cursor != -1 )
	{
		sprintf( buf, "OcultarInformacion" );
		refScriptServer->Run( buf, params );
	}

	return i_cursor;
}

void nJugador::AccionPrincipal( nEntity* entidadSeleccionada )
{
	nString clase;
	clase.Set( entidadSeleccionada->GetClass()->GetName() );

	if ( strcmp( clase.Get(), "nayuntamiento" ) == 0 )
	{
		nAyuntamiento* ayto;
		ayto = (nAyuntamiento*) entidadSeleccionada;
		ayto->Seleccionar();

		AccionCogerDinero( entidadSeleccionada );
	}
	else if ( strcmp( clase.Get(), "ncampesino" ) == 0 )
	{
		AccionContratar(entidadSeleccionada);
	}
	else if ( strcmp( clase.Get(), "nconstruccion" ) == 0 )
	{
		// No se hace nada con el botón principal sobre un edificio
		return;
	}
	else if ( strcmp( clase.Get(), "ncasaplanos" ) == 0 )
	{
		nCasaPlanos* casaplanos;
		casaplanos = (nCasaPlanos*) entidadSeleccionada;
		casaplanos->Seleccionar();

		AccionCogerPlano( entidadSeleccionada );
	}
	else if ( strcmp( clase.Get(), "ncasamoneda" ) == 0 )
	{
		nCasaMoneda* casamoneda;
		casamoneda = (nCasaMoneda*) entidadSeleccionada;
		casamoneda->Seleccionar();

		AccionCogerDinero( entidadSeleccionada );
	}
	else if ( strcmp( clase.Get(), "npersonaje" ) == 0  ||
		 	  strcmp( clase.Get(), "nkunoichi"  ) == 0  ||
			  strcmp( clase.Get(), "nladrona"   ) == 0  ||
			  strcmp( clase.Get(), "nmaestro"   ) == 0  ||
			  strcmp( clase.Get(), "nshugenja"  ) == 0  ||
			  strcmp( clase.Get(), "nshogun"    ) == 0  ||
			  strcmp( clase.Get(), "nsamurai"   ) == 0  ||
			  strcmp( clase.Get(), "nmercader"  ) == 0  ||
			  strcmp( clase.Get(), "ningeniero" ) == 0  )
	{

		// No hacemos nada con el botón principal sobre un personaje
		return;
	}
	else
	{
		// Se ha seleccionado un punto del mapa, no se hace nada
	}
}

void nJugador::AccionSecundaria( nEntity* entidadSeleccionada )
{
	nString clase;
	clase.Set( entidadSeleccionada->GetClass()->GetName() );

	if ( strcmp( clase.Get(), "nayuntamiento" ) == 0 )
	{
		nAyuntamiento* ayto;
		ayto = (nAyuntamiento*) entidadSeleccionada;
		ayto->Seleccionar();
		if ( this->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_SHUGENJA &&
			 ayto->GetId() != this->GetId() )
		{
			AccionRecaudar( entidadSeleccionada );
		}
		else if ( this->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_SHOGUN )
		{
			AccionRecaudar( entidadSeleccionada );
		}
	}
	else if ( strcmp( clase.Get(), "ncampesino" ) == 0 )
	{
		AccionEspecial( entidadSeleccionada );
	}
	else if ( strcmp( clase.Get(), "nconstruccion" ) == 0 )
	{
		AccionDestruirEdificio( entidadSeleccionada );
	}
	else if ( strcmp( clase.Get(), "ncasaplanos" ) == 0 )
	{
		nCasaPlanos *casaplanos;
		casaplanos = (nCasaPlanos*) entidadSeleccionada;
		casaplanos->Seleccionar();

		AccionIntercambiarPlanos( entidadSeleccionada );
	}
	else if ( strcmp( clase.Get(), "ncasamoneda" ) == 0 )
	{
		// No hacemos nada con el botón derecho sobre la casa
		// de la moneda
		return;
	}
	else if ( strcmp( clase.Get(), "npersonaje" ) == 0  ||
			  strcmp( clase.Get(), "nkunoichi"  ) == 0  ||
			  strcmp( clase.Get(), "nladrona"   ) == 0  ||
			  strcmp( clase.Get(), "nmaestro"   ) == 0  ||
			  strcmp( clase.Get(), "nshugenja"  ) == 0  ||
			  strcmp( clase.Get(), "nshogun"    ) == 0  ||
			  strcmp( clase.Get(), "nsamurai"   ) == 0  ||
			  strcmp( clase.Get(), "nmercader"  ) == 0  ||
			  strcmp( clase.Get(), "ningeniero" ) == 0  )
	{
		nPersonaje *v_personaje = (nPersonaje *) entidadSeleccionada;
		// Esto lo ponemos para evitar hacernos acciones a nosotros mismos
		if (v_personaje->GetJugador()->GetId() != this->GetId())
			AccionEspecial( entidadSeleccionada );
	}
	else
	{
		// Se ha seleccionado un punto del mapa, no se hace nada
	}
}

TAccion *nJugador::AccionVacia()
{
	TAccion *accion;
	accion = new TAccion();
	accion->accion = NINGUNA;
	accion->destino = NULL;
	accion->jugador = NULL;
	accion->tiempo	= 0.0f;
	accion->nombre_personaje = nPersonaje::PERSONAJE_CAMPESINO;
	accion->posicion = vector3(0.0f,0.0f,0.0f);
	return accion;
}

void nJugador::GuardarAccionActual(TAccion *accion)
{
	AccionActual.accion.accion=(short)accion->accion;
	if (accion->destino!=NULL)
	{
		if (strcmp(accion->destino->GetClass()->GetName(),"ncasaplanos")==0)
		{
			AccionActual.accion.clase_destino=C_CASAPLANOS;
			AccionActual.accion.id_destino=0;
		}
		if (strcmp(accion->destino->GetClass()->GetName(),"ncasamoneda")==0)
		{
			AccionActual.accion.clase_destino=C_CASAMONEDA;
			AccionActual.accion.id_destino=0;
		}
		if (strcmp(accion->destino->GetClass()->GetName(),"nayuntamiento")==0)
		{
			nAyuntamiento *v_ayto = (nAyuntamiento *) accion->destino;
			AccionActual.accion.clase_destino=C_AYUNTAMIENTO;
			AccionActual.accion.id_destino=v_ayto->GetId();
		}
		if (strcmp(accion->destino->GetClass()->GetName(),"nconstruccion")==0)
		{
			nConstruccion *v_cons = (nConstruccion *) accion->destino;
			AccionActual.accion.clase_destino=C_CONSTRUCCION;
			AccionActual.accion.id_destino=v_cons->GetId();
		}
		if (strcmp(accion->destino->GetClass()->GetName(),"ncampesino")==0)
		{
			nCampesino *v_campesino = (nCampesino *) accion->destino;
			AccionActual.accion.clase_destino=C_CAMPESINO;
			AccionActual.accion.id_destino=v_campesino->GetId();
		}
		if (strcmp(accion->destino->GetClass()->GetName(),"npersonaje")==0)
		{
			nPersonaje *v_personaje = (nPersonaje *) accion->destino;
			AccionActual.accion.clase_destino=C_PERSONAJE;
			AccionActual.accion.id_destino=v_personaje->GetJugador()->GetId();
		}
		if (strcmp(accion->destino->GetClass()->GetName(),"nkunoichi")==0)
		{
			nPersonaje *v_personaje = (nPersonaje *) accion->destino;
			AccionActual.accion.clase_destino=C_KUNOICHI;
			AccionActual.accion.id_destino=v_personaje->GetJugador()->GetId();
		}
		if (strcmp(accion->destino->GetClass()->GetName(),"nladrona")==0)
		{
			nPersonaje *v_personaje = (nPersonaje *) accion->destino;
			AccionActual.accion.clase_destino=C_LADRONA;
			AccionActual.accion.id_destino=v_personaje->GetJugador()->GetId();
		}
		if (strcmp(accion->destino->GetClass()->GetName(),"nshuguenja")==0)
		{
			nPersonaje *v_personaje = (nPersonaje *) accion->destino;
			AccionActual.accion.clase_destino=C_SHUGENJA;
			AccionActual.accion.id_destino=v_personaje->GetJugador()->GetId();
		}
		if (strcmp(accion->destino->GetClass()->GetName(),"nshogun")==0)
		{
			nPersonaje *v_personaje = (nPersonaje *) accion->destino;
			AccionActual.accion.clase_destino=C_SHOGUN;
			AccionActual.accion.id_destino=v_personaje->GetJugador()->GetId();
		}
		if (strcmp(accion->destino->GetClass()->GetName(),"nmaestro")==0)
		{
			nPersonaje *v_personaje = (nPersonaje *) accion->destino;
			AccionActual.accion.clase_destino=C_MAESTRO;
			AccionActual.accion.id_destino=v_personaje->GetJugador()->GetId();
		}
		if (strcmp(accion->destino->GetClass()->GetName(),"nmercader")==0)
		{
			nPersonaje *v_personaje = (nPersonaje *) accion->destino;
			AccionActual.accion.clase_destino=C_MERCADER;
			AccionActual.accion.id_destino=v_personaje->GetJugador()->GetId();
		}
		if (strcmp(accion->destino->GetClass()->GetName(),"ningeniero")==0)
		{
			nPersonaje *v_personaje = (nPersonaje *) accion->destino;
			AccionActual.accion.clase_destino=C_INGENIERO;
			AccionActual.accion.id_destino=v_personaje->GetJugador()->GetId();
		}
		if (strcmp(accion->destino->GetClass()->GetName(),"nsamurai")==0)
		{
			nPersonaje *v_personaje = (nPersonaje *) accion->destino;
			AccionActual.accion.clase_destino=C_SAMURAI;
			AccionActual.accion.id_destino=v_personaje->GetJugador()->GetId();
		}
	}
	else
	{
		AccionActual.accion.clase_destino=C_NINGUNA;
		AccionActual.accion.id_destino=0;
	}
	AccionActual.accion.numAccion++;
	if (accion->jugador!=NULL)
	{
		AccionActual.accion.jugador=(short)accion->jugador->GetId();
	}
	else
		AccionActual.accion.jugador=(short)(-1);
	AccionActual.accion.personaje=(short)accion->nombre_personaje;
	AccionActual.accion.tiempo=accion->tiempo;
	AccionActual.accion.posicion_x=accion->posicion.x;
	AccionActual.accion.posicion_y=accion->posicion.y;
	AccionActual.accion.posicion_z=accion->posicion.z;
	AccionActual.plano=(short) this->plano_a_construir;
	AccionActual.num_jugador=(short) this->GetId();
}

void nJugador::AgregarAccion( TAccion* accion )
{
	if ( this->EstaBloqueado() && accion->accion != NACER )
		return;

	if ( accion->accion == LIBERAR_PERSONAJE && !partida->PuedeLiberarPersonaje( this->GetId() ) )
		return;

	// Marcamos al jugador como activo (realizando una acción)
	JugadorActivo = true;

	GuardarAccionActual(accion);

	TAccion* siguienteAccion;
	siguienteAccion = AccionVacia();

	TAccion* siguienteAccion2;
	siguienteAccion2 = AccionVacia();

	TAccion* ultimaAccion;
	ultimaAccion = AccionVacia();

	VaciarPilaAcciones();

	// Insertamos la acción ninguna para que nos
	// desbloquee el personaje una vez haya concluido
	// su acción
	TAccion* ningunaAccion;
	ningunaAccion = AccionVacia();
	pila_acciones.Push( ningunaAccion );

	switch ( accion->accion )
	{
		case NINGUNA:
			pila_acciones.Push( accion );
			break;
		case CAMBIAR_PERSONAJE:
			//n_printf("El jugador %d agrega una accion de CAMBIAR PERSONAJE\n",GetId());
			siguienteAccion2->accion = ESPERAR;
			siguienteAccion2->tiempo = 1.0f;
			pila_acciones.Push( siguienteAccion2 );
			siguienteAccion->accion = METER_PERSONAJE;
			pila_acciones.Push( siguienteAccion );
			ultimaAccion->accion = ESPERAR;
			ultimaAccion->tiempo = 1.0f;
			pila_acciones.Push( ultimaAccion );
			pila_acciones.Push( accion );
			partida->BloqueaPersonaje( accion->nombre_personaje, this->GetId() );
			break;
		case LIBERAR_PERSONAJE:
			//n_printf("El jugador %d agrega una accion de LIBERAR PERSONAJE\n",GetId());
			siguienteAccion2->accion = ESPERAR;
			siguienteAccion2->tiempo = 1.0f;
			pila_acciones.Push( siguienteAccion2 );
			siguienteAccion->accion = METER_PERSONAJE;
			pila_acciones.Push( siguienteAccion );
			ultimaAccion->accion = ESPERAR;
			ultimaAccion->tiempo = 1.0f;
			pila_acciones.Push( ultimaAccion );
			pila_acciones.Push( accion );
			break;
		case PRIMERA:
			pila_acciones.Push( accion );
			break;
		case IR_A:
			pila_acciones.Push( accion );
			break;
		case IR_A_ENTIDAD:
			pila_acciones.Push( accion );
			break;
		case IR_A_AYUNTAMIENTO:
			//n_printf("El jugador %d agrega una accion de IR A AYUNTAMIENTO\n",GetId());
			ultimaAccion->accion = ESPERAR;
			ultimaAccion->tiempo = 1.0f;
			pila_acciones.Push( ultimaAccion );
			accion->accion = IR_A_ENTIDAD;
			pila_acciones.Push( accion );
			break;
		case COGER_DINERO:
			//n_printf("El jugador %d agrega una accion de COGER DINERO\n",GetId());
			ultimaAccion->accion = ESPERAR;
			ultimaAccion->tiempo = 1.0f;
			pila_acciones.Push( ultimaAccion );
			pila_acciones.Push( accion );
			siguienteAccion->accion = IR_A_ENTIDAD;
			siguienteAccion->destino = accion->destino;
			siguienteAccion->posicion = accion->destino->GetPosition();
			pila_acciones.Push( siguienteAccion );
			break;
		case RECAUDAR:
			//n_printf("El jugador %d agrega una accion de RECAUDAR\n",GetId());
			ultimaAccion->accion = ESPERAR;
			ultimaAccion->tiempo = 1.0f;
			pila_acciones.Push( ultimaAccion );
			pila_acciones.Push( accion );
			siguienteAccion->accion = IR_A_ENTIDAD;
			siguienteAccion->destino = accion->destino;
			siguienteAccion->posicion = accion->destino->GetPosition();
			pila_acciones.Push( siguienteAccion );
			break;
		case COGER_PLANO:
			//n_printf("El jugador %d agrega una accion de COGER PLANO\n",GetId());
			ultimaAccion->accion = ESPERAR;
			ultimaAccion->tiempo = 1.0f;
			pila_acciones.Push( ultimaAccion );
			pila_acciones.Push( accion );
			siguienteAccion->accion = IR_A_ENTIDAD;
			siguienteAccion->destino = accion->destino;
			siguienteAccion->posicion = accion->destino->GetPosition();
			pila_acciones.Push( siguienteAccion );
			break;
		case INTERCAMBIAR_PLANOS:
			//n_printf("El jugador %d agrega una accion de INTERCAMBIAR PLANOS\n",GetId());
			ultimaAccion->accion = ESPERAR;
			ultimaAccion->tiempo = 1.0f;
			pila_acciones.Push( ultimaAccion );
			pila_acciones.Push( accion );
			siguienteAccion->accion = IR_A_ENTIDAD;
			siguienteAccion->destino = accion->destino;
			siguienteAccion->posicion = accion->destino->GetPosition();
			pila_acciones.Push( siguienteAccion );
			break;
		case SEGUIR_A:
			pila_acciones.Push( accion );
			break;
		case SEGUIR_A_JUGADOR:
			pila_acciones.Push( accion );
			break;
		case SEGUNDA:
			//n_printf("El jugador %d agrega una accion ESPECIAL\n",GetId());
			ultimaAccion->accion = ESPERAR;
			ultimaAccion->tiempo = 1.0f;
			pila_acciones.Push( ultimaAccion );
			if ( accion->destino!=NULL )
			{
				siguienteAccion->accion = SEGUIR_A;
				siguienteAccion->destino = accion->destino;
				siguienteAccion->posicion = accion->destino->GetPosition();
			}
			else
			{
				siguienteAccion->accion = SEGUIR_A_JUGADOR;
				siguienteAccion->destino = accion->destino;
				nPersonaje* pers_sel = (nPersonaje*) accion->destino;
				if ( pers_sel->GetJugador() != NULL )
				{
					accion->jugador = pers_sel->GetJugador();
					siguienteAccion->jugador = pers_sel->GetJugador();
				}
				else
				{
					accion->jugador = NULL;
					siguienteAccion->jugador = NULL;
				}
				siguienteAccion->posicion = accion->destino->GetPosition();
			}
			pila_acciones.Push( accion );
			pila_acciones.Push( siguienteAccion );
			break;
		case DESTRUIR_EDIFICIO:
			//n_printf("El jugador %d agrega una accion de DESTRUIR EDIFICIO\n",GetId());
			ultimaAccion->accion = ESPERAR;
			ultimaAccion->tiempo = 3.0f;
			pila_acciones.Push( ultimaAccion );
			pila_acciones.Push( accion );
			siguienteAccion->accion = IR_A_ENTIDAD;
			siguienteAccion->destino = accion->destino;
			pila_acciones.Push( siguienteAccion );
			break;
		case CONSTRUIR_EDIFICIO:
			//n_printf("El jugador %d agrega una accion de CONSTRUIR EDIFICIO\n",GetId());
			ultimaAccion->accion = ESPERAR;
			ultimaAccion->tiempo = 3.0f;
			pila_acciones.Push( ultimaAccion );
			pila_acciones.Push( accion );
			siguienteAccion->accion = IR_A_ENTIDAD;
			siguienteAccion->destino = accion->destino;
			siguienteAccion->posicion = accion->destino->GetPosition();
			pila_acciones.Push( siguienteAccion );
			//n_printf("Bloqueamos al jugador %d por edificacion\n",GetId());
			this->Bloquear();
			break;
		case CONTRATAR:
			//n_printf("El jugador %d agrega una accion de CONTRATAR\n",GetId());
			ultimaAccion->accion = ESPERAR;
			ultimaAccion->tiempo = 1.0f;
			pila_acciones.Push( ultimaAccion );
			pila_acciones.Push( accion );
			siguienteAccion->accion = SEGUIR_A;
			siguienteAccion->destino = accion->destino;
			siguienteAccion->posicion = accion->destino->GetPosition();
			pila_acciones.Push( siguienteAccion );
			break;
		case ESPERAR:
			pila_acciones.Push( accion );
			break;
		case MORIR:
			//n_printf("El jugador %d agrega una accion de NACER\n",GetId());
			if (this->personaje->GetTipoPersonaje()!= nPersonaje::PERSONAJE_CAMPESINO )
			{
				// Esperamos hasta que el campesino llega al suelo
				siguienteAccion = AccionVacia();
				siguienteAccion->accion = ESPERAR;
				siguienteAccion->tiempo = 3.0f;
				pila_acciones.Push( siguienteAccion );

				// Metemos el campesino
				siguienteAccion = AccionVacia();
				siguienteAccion->accion = METER_PERSONAJE;
				pila_acciones.Push( siguienteAccion );

				// Esperamos hasta que el personaje se haya ido
				siguienteAccion = AccionVacia();
				siguienteAccion->accion = ESPERAR;
				siguienteAccion->tiempo = 3.0f;
				pila_acciones.Push( siguienteAccion );

				// Liberamos el personaje
				siguienteAccion = AccionVacia();
				siguienteAccion->accion=LIBERAR_PERSONAJE;
				pila_acciones.Push( siguienteAccion );
			}

			// Esperamos un tiempo mientras nos levantamos
			siguienteAccion = AccionVacia();
			siguienteAccion->accion=ESPERAR;
			siguienteAccion->tiempo=2.0f;
			pila_acciones.Push( siguienteAccion );

			// Nos levantamos del suelo
			siguienteAccion = AccionVacia();
			siguienteAccion->accion = NACER;
			pila_acciones.Push( siguienteAccion );

			// Esperamos un tiempo mientras estamos muertos
			siguienteAccion = AccionVacia();
			siguienteAccion->accion = ESPERAR;
			float tiempo;
			if (this->personaje->GetTipoPersonaje()==nPersonaje::PERSONAJE_CAMPESINO)
				tiempo=20.0f;
			else
				tiempo=MAX_TIEMPO_TURNO - this->personaje->GetTiempo();
			siguienteAccion->tiempo = tiempo;
			pila_acciones.Push( siguienteAccion );

			// Caemos al suelo
			siguienteAccion = AccionVacia();
			siguienteAccion->accion = MORIR;
			pila_acciones.Push( siguienteAccion );

			// Esperamos hasta que nos den el golpe
			siguienteAccion = AccionVacia();
			siguienteAccion->accion = ESPERAR;
			siguienteAccion->tiempo = 0.7f;
			pila_acciones.Push( siguienteAccion );

			//n_printf("Bloqueamos al jugador %d por muerte\n",GetId());
			this->Bloquear();
			break;
	}

	TAccion* accionFinal;
	accionFinal = AccionVacia();
	accionFinal->accion = PRIMERA;

	pila_acciones.Push( accionFinal );
}

bool nJugador::AccionTerminada(float dt)
{
	TAccion* accionActual = pila_acciones.Top();
	switch ( accionActual->accion )
	{
		case NINGUNA:
			return personaje->AnimacionTerminada();
			break;
		case PRIMERA:
			return true;
			break;
		case METER_PERSONAJE:
			return true;
			break;
		case CAMBIAR_PERSONAJE:
			return true;
			break;
		case LIBERAR_PERSONAJE:
			return true;
			break;
		case IR_A:
			return personaje->GetComportamiento() == nPersonaje::QUIETO;
			break;
		case IR_A_ENTIDAD:
			if ( accionActual->destino != NULL )
			{
				if ( accionActual->destino->EstaEnRango( personaje->GetPosition() ) )
					return true;
			}
			return false;
			break;
		case COGER_DINERO:
			return true;
			break;
		case RECAUDAR:
			return true;
			break;
		case COGER_PLANO:
			return true;
			break;
		case INTERCAMBIAR_PLANOS:
			return true;
			break;
		case SEGUIR_A:
			if ( accionActual->destino != NULL )
			{
				if ( accionActual->destino->EstaEnRangoDeAccion( personaje->GetPosition() ) )
				{
					this->personaje->Parar();
					return true;
				}
			}
			return false;
			break;
		case SEGUIR_A_JUGADOR:
			if ( accionActual->jugador != NULL )
			{
				if ( accionActual->jugador->GetPersonaje() != NULL )
				{
					if ( accionActual->jugador->GetPersonaje()->EstaEnRangoDeAccion( personaje->GetPosition() ) )
					{
						this->personaje->Parar();
						return true;
					}
				}
				else
				{
					if ( accionActual->destino != NULL )
					{
						if ( accionActual->destino->EstaEnRangoDeAccion( personaje->GetPosition() ) )
						{
							this->personaje->Parar();
							return true;
						}
					}
				}
			}
			else
			{
				if ( accionActual->destino != NULL )
				{
					if ( accionActual->destino->EstaEnRangoDeAccion( personaje->GetPosition() ) )
					{
						this->personaje->Parar();
						return true;
					}
				}
			}
			return false;
			break;
		case SEGUNDA:
			return true;
			break;
		case DESTRUIR_EDIFICIO:
			return true;
			break;
		case CONSTRUIR_EDIFICIO:
			nConstruccion* construccion;
			construccion = (nConstruccion*) accionActual->destino;
			if ( GetJugadorPrincipal() )
				return ( construccion->GetEstado() == nConstruccion::CONSTRUIDO );
			else
				return true;
			break;
		case CONTRATAR:
			return true;
			break;
		case ESPERAR:
			accionActual->tiempo -= dt;
			if ( accionActual->tiempo < 0.0f )
			{
				//n_printf( "Desactivando jugador %d\n", GetId() );
				return true;
			}
			break;
		case NACER:
			return true;
			break;
		case MORIR:
			return true;
			break;
	}
	return false;
}

void nJugador::EjecutarSiguienteAccion()
{
	char* params = NULL;
	list<nPlano*>::iterator iter;

	TAccion* accionActual = pila_acciones.Pop();
	n_free(accionActual);

	if ( pila_acciones.IsEmpty() )
		return;
	
	accionActual = pila_acciones.Top();

	switch ( accionActual->accion )
	{
		case NINGUNA:
			//n_printf("Desbloqueamos por fin de accion al jugador %d\n", this->GetId());
			this->Desbloquear();
			break;
		case PRIMERA:
			break;
		case METER_PERSONAJE:
			MeterPersonaje();
			break;
		case CAMBIAR_PERSONAJE:
			CambiarPersonaje( accionActual->nombre_personaje );
			break;
		case LIBERAR_PERSONAJE:
			//n_printf("Libera personaje por accion el jugador %d\n",GetId());
			LiberarPersonaje( true );
			break;
		case IR_A:
			personaje->IrA( true, accionActual->posicion.x, accionActual->posicion.z );

			// FMOD: Sonido de personajes al moverse
			SonidoPersonaje(IR_A);

			break;
		case IR_A_ENTIDAD:
			if ( accionActual->destino != NULL )
				personaje->IrAEdificio( true, accionActual->destino );
			break;
		case COGER_DINERO:
			CogerDinero();
			break;
		case RECAUDAR:
			Recaudar();
			break;
		case COGER_PLANO:
			if ( PersonajeActivo == nPersonaje::PERSONAJE_CAMPESINO )
			{
				//n_printf( "SIENDO CAMPESINO NO SE PUEDE COGER PLANOS\n" );
				MostrarInformacion("Siendo campesino no se pueden coger planos");
			}
			else
			{
				nCasaPlanos* casaplanos;
				casaplanos = (nCasaPlanos*) kernelServer->Lookup( "/game/world/casaplanos" );
				if ( PuedeCogerPlanos() )
				{
					nPlano* plano;
					if ( casaplanos->GetPlano( plano ) )
					{
						CogerPlano( plano );
						AccionesPorTurno.VisitasACasaPlanos++;
					}
				}
			}
			break;
		case INTERCAMBIAR_PLANOS:
			if ( accionActual->destino != NULL )
				IntercambiarPlanos();
			break;
		case SEGUIR_A:
			if ( accionActual->destino != NULL )
				personaje->SeguirA( true, accionActual->destino );
			break;
		case SEGUIR_A_JUGADOR:
			if ( accionActual->jugador != NULL )
				personaje->SeguirAJugador( true, accionActual->jugador );
			else
			{
				if ( accionActual->destino != NULL )
					personaje->SeguirA( true, accionActual->destino );
			}
			break;
		case SEGUNDA:
			if ( accionActual->destino != NULL )
			{
				nEntity* entidad;
				entidad = (nEntity*) accionActual->destino;
				nCampesino* campesino;
				campesino = (nCampesino*) accionActual->destino;

				if ( PersonajeActivo == nPersonaje::PERSONAJE_KUNOICHI )
				{
					// Si no se ha realizado todavía un asesinato en el turno
					if ( AccionesPorTurno.Asesinatos == 0 ||
						( AccionesPorTurno.Asesinatos == 1 && LoTieneConstruido( "ESCUELA DE ASESINOS" ) ) )
					{
						nKunoichi* kunoichi = (nKunoichi*) personaje;
						kunoichi->Matar( entidad );
						nPersonaje* personaje_a_matar = (nPersonaje*) entidad;
						nJugador* propietario = personaje_a_matar->GetJugador();

						if ( propietario != NULL ) // El personaje es un jugador
						{
							if ( !propietario->EstaBloqueado() )
							{
								personaje->Parar();
								personaje->SetAnimacion( nPersonaje::ANIM_ACCION );
								propietario->AccionMorir();

								Puntuar( ASESINATOS, NULL, 0 );
								SetPersonajesAsesinados( GetPersonajesAsesinados() + 1 );
								AccionesPorTurno.Asesinatos++;
							}
							/*else
							{
								n_printf("No se asesina porque el jugador %d estaba ocupado\n", propietario->GetId());
							}*/
						}
						else // Asesinamos a un campesino
						{
							personaje->SetAnimacion( nPersonaje::ANIM_ACCION );
							Puntuar( ASESINATOS, NULL, 0 );
							SetPersonajesAsesinados( GetPersonajesAsesinados() + 1 );
							AccionesPorTurno.Asesinatos++;
						}
					}
					else
					{
						//n_printf( "YA SE HA ASESINADO\n" );
						MostrarInformacion("No puedes matar mas en este turno");
					}
				}
				else if ( PersonajeActivo == nPersonaje::PERSONAJE_LADRONA )
				{
					// Si no se ha realizado todavía un robo en el turno
					if ( AccionesPorTurno.Robos == 0 ||
						( AccionesPorTurno.Robos == 1 && LoTieneConstruido( "ESCUELA DE LADRONES" ) ) )
					{
						nLadrona* ladrona = (nLadrona*) personaje;
						int dinero = ladrona->Robar( entidad );
						if ( dinero > 0 )
						{
							char buf[N_MAXPATH];
							personaje->SetAnimacion( nPersonaje::ANIM_ACCION );
							SetOro( GetOro() + dinero );
							Puntuar( ROBOS, NULL, 0 );
							SetPersonajesRobados( GetPersonajesRobados() + 1 );
							sprintf( buf, "Has robado %d monedas de oro", dinero );
							MostrarInformacion( buf );
							AccionesPorTurno.Robos++;

							// Sonido de coger moneda
							nGame *juego=(nGame *) kernelServer->Lookup("/game");
							juego->Play3DSound(0,"sounds:posicional/cogermoneda.wav",accionActual->destino->GetPosition());
						}
					}
					else
					{
						//n_printf( "YA SE HA ROBADO\n" );
						MostrarInformacion("No puedes robar mas en este turno");
					}
				}
				else if ( PersonajeActivo == nPersonaje::PERSONAJE_SHUGENJA )
				{
					if ( strcmp( entidad->GetClass()->GetName(), "ncampesino" ) == 0 )
					{
						// Si no se ha realizado todavía una resurrección en el turno
						if ( AccionesPorTurno.Resurrecciones == 0 )
						{
							nShugenja* shugenja = (nShugenja*) personaje;
							shugenja->ResucitarCampesino( campesino );
							SetCampesinosResucitados( GetCampesinosResucitados() + 1 );
							Puntuar( RESURRECCIONES, NULL, 0 );
							AccionesPorTurno.Resurrecciones++;
						}
						else
						{
							//n_printf( "YA SE HA RESUCITADO\n" );
							MostrarInformacion("No puedes resucitar mas en este turno");
						}
					}
					else
					{
						//n_printf( "SOLO SE PUEDE RESUCITAR A CAMPESINOS\n" );
						MostrarInformacion("Solo se puede resucitar a campesinos");
					}
				}
				else if ( PersonajeActivo == nPersonaje::PERSONAJE_SHOGUN )
				{
					if ( PuedeContratarCampesino( campesino ) )
					{
						personaje->SetAnimacion( nPersonaje::ANIM_COGER );
						nShogun* shogun = (nShogun*) personaje;
						shogun->ContratarCampesino( campesino );
						SetOro( GetOro() - campesino->GetPrecio() );
						Puntuar( CONTRATOS, NULL, 0 );
						SetCampesinosContratados( GetCampesinosContratados() + 1 );
						personaje->ContratarCampesino( campesino->GetId() );
					}
				}
				else if ( PersonajeActivo == nPersonaje::PERSONAJE_MAESTRO )
				{
					if ( PuedeContratarCampesino( campesino ) )
					{
						personaje->SetAnimacion( nPersonaje::ANIM_COGER );
						nMaestro* maestro = (nMaestro*) personaje;
						maestro->ContratarCampesino( campesino );
						SetOro( GetOro() - campesino->GetPrecio() );
						Puntuar( CONTRATOS, NULL, 0 );
						SetCampesinosContratados( GetCampesinosContratados() + 1 );
						personaje->ContratarCampesino( campesino->GetId() );
					}
				}
				else if ( PersonajeActivo == nPersonaje::PERSONAJE_MERCADER )
				{
					if ( PuedeContratarCampesino( campesino ) )
					{
						personaje->SetAnimacion( nPersonaje::ANIM_COGER );
						nMercader* mercader = (nMercader*) personaje;
						mercader->ContratarCampesino( campesino );
						SetOro( GetOro() - campesino->GetPrecio() );
						Puntuar( CONTRATOS, NULL, 0 );
						SetCampesinosContratados( GetCampesinosContratados() + 1 );
						personaje->ContratarCampesino( campesino->GetId() );
					}
				}
				else if ( PersonajeActivo == nPersonaje::PERSONAJE_INGENIERO )
				{
					if ( PuedeContratarCampesino( campesino ) )
					{
						personaje->SetAnimacion( nPersonaje::ANIM_COGER );
						nIngeniero* ingeniero = (nIngeniero*) personaje;
						ingeniero->ContratarCampesino( campesino );
						SetOro( GetOro() - campesino->GetPrecio() );
						Puntuar( CONTRATOS, NULL, 0 );
						SetCampesinosContratados( GetCampesinosContratados() + 1 );
						personaje->ContratarCampesino( campesino->GetId() );
					}
				}
				else if ( PersonajeActivo == nPersonaje::PERSONAJE_SAMURAI )
				{
					if ( PuedeContratarCampesino( campesino ) )
					{
						personaje->SetAnimacion( nPersonaje::ANIM_COGER );
						nSamurai* samurai = (nSamurai*) personaje;
						samurai->ContratarCampesino( campesino );
						SetOro( GetOro() - campesino->GetPrecio() );
						Puntuar( CONTRATOS, NULL, 0 );
						SetCampesinosContratados( GetCampesinosContratados() + 1 );
						personaje->ContratarCampesino( campesino->GetId() );
					}
				}
			}
			break;
		case DESTRUIR_EDIFICIO:
			if ( accionActual->destino != NULL )
			{
				if ( PersonajeActivo == nPersonaje::PERSONAJE_SAMURAI )
				{
					nConstruccion* construccion;
					construccion = (nConstruccion*) accionActual->destino;

					nJugador* propietario = construccion->GetManager();
					// Si el edificio es jugador está junto a su ayuntamiento
					if ( propietario->GetId() == IdJugador )
					{
						//n_printf( "NO SE PUEDE DESTRUIR EDIFICIOS PROPIOS\n" );
						MostrarInformacion("No puedes destruir tus edificios");
					}
					else
					{
						if ( propietario->GetPersonaje()->GetTipoPersonaje() == nPersonaje::PERSONAJE_MAESTRO && !LoTieneConstruido( "ESCUELA DE ASEDIO" ) )
						{
							//n_printf( "NO SE LE PUEDEN DERRIBAR EDIFICIOS AL MAESTRO\n" );
							MostrarInformacion("No se pueden derribar edificios al maestro");
						}
						else
						{
							nDistrito* distrito = propietario->GetDistrito();
							nPlano *v_plano = distrito->GetPlano( construccion->GetId() );
							int campesinos_contratados = personaje->GetCampesinosContratados();
							// Se ojean todas sus construcciones
							if ( v_plano != NULL)
							{
								int reduccion = (campesinos_contratados*v_plano->GetCoste())/10;
								int coste_destruccion = v_plano->GetCoste() - reduccion;
								if ( coste_destruccion > GetOro() )
								{
									//n_printf( "NO TIENE SUFICIENTE DINERO PARA DESTRUIR\n" );
									MostrarInformacion("No tienes suficiente dinero para destruir");
								}
								else
								{
									char path[N_MAXPATH];
									sprintf( path, "/game/world/casaplanos" );
									nCasaPlanos* casaplanos = (nCasaPlanos*) kernelServer->Lookup( path );
									n_printf( "PLANOS ANTES DE DESTRUCCION %d\n", casaplanos->NumeroDePlanos() );
									personaje->SetAnimacion( nPersonaje::ANIM_DESTRUIR );
									nSamurai* samurai = (nSamurai*) personaje;
									samurai->DestruirEdificio( construccion );
									distrito->EliminarConstruccion( construccion->GetId() );
									SetOro( GetOro() - coste_destruccion );
									Puntuar( DESTRUCCIONES, propietario, v_plano->GetValor() );
									SetEdificiosDerribados( GetEdificiosDerribados() + 1 );
									casaplanos->InsertaPlano( v_plano );
									n_printf( "PLANOS DESPUES DE DESTRUCCION %d\n", casaplanos->NumeroDePlanos() );
								}
							}
						}
					}
				}
			}
			break;
		case CONSTRUIR_EDIFICIO:
			if ( PersonajeActivo == nPersonaje::PERSONAJE_CAMPESINO )
			{
				//n_printf( "SIENDO CAMPESINO NO SE PUEDE CONSTRUIR EDIFICIOS\n" );
				MostrarInformacion("Siendo campesino no se puede construir");
			}
			else
			{
				personaje->SetAnimacion( nPersonaje::ANIM_CONSTRUIR );
				nConstruccion* construccion2;
				construccion2 = (nConstruccion*) accionActual->destino;
				construccion2->Construir( this );
				construccion2->SetNombre( planos.At( plano_a_construir )->GetNombre().c_str() );
				distrito->AgregarConstruccion( planos.At( plano_a_construir ) );
				AccionesPorTurno.Construcciones++;
				int coste = planos.At( plano_a_construir )->GetCoste();
				if ( PersonajeActivo == nPersonaje::PERSONAJE_INGENIERO )
				{
					int campesinos_contratados = personaje->GetCampesinosContratados();
					int reduccion_coste = (campesinos_contratados*coste)/10;
					coste -= reduccion_coste;
				}
				SetOro( GetOro() - coste );
				Puntuar( CONSTRUCCIONES, NULL, 0 );
				planos.Set( plano_a_construir, NULL );
				EdificioAConstruir = "";
			}
			break;
		case CONTRATAR:
			if ( accionActual->destino != NULL )
			{
				if ( PersonajeActivo == nPersonaje::PERSONAJE_CAMPESINO )
				{
					//n_printf( "SIENDO CAMPESINO NO SE PUEDE CONTRATAR CAMPESINOS\n" );
					MostrarInformacion("Siendo campesino no se puede contratar");
				}
				else if ( strcmp( accionActual->destino->GetClass()->GetName(), "ncampesino" ) != 0 )
				{
					//n_printf( "NO SE PUEDE CONTRATAR A PERSONAJES QUE NO SEAN CAMPESINOS\n" );
					MostrarInformacion("No se puede contratar a jugadores");
				}
				else
				{
					nCampesino* campesino2;
					campesino2 = (nCampesino*) accionActual->destino;

					if ( campesino2->EstaMuerto() )
					{
						//n_printf( "EL CAMPESINO %d ESTA MUERTO\n", campesino2->GetId() );
						MostrarInformacion("No puedes contratar a un campesino muerto");
					}
					else
					{
						if ( campesino2->EstaContratado() )
						{
							if ( personaje->CampesinoMio( campesino2->GetId() ) )
							{
								LiberarCampesino( campesino2 );
								SetCampesinosLiberados( GetCampesinosLiberados() + 1 );
								Puntuar( DESPIDOS, NULL, 0 );
							}
							else
							{
								int campesinos_contratados = personaje->GetCampesinosContratados();
								if ( PersonajeActivo == nPersonaje::PERSONAJE_MAESTRO && campesinos_contratados > 0 )
								{
									if ( campesinos_contratados > AccionesPorTurno.Liberaciones )
									{
										LiberarCampesino( campesino2 );
										AccionesPorTurno.Liberaciones++;
									}
									else
									{
										//n_printf( "LE FALTA CONTRATAR UN CAMPESINO\n" );
										MostrarInformacion("Necesitas contratar mas campesinos");
									}
								}
								else
								{
									//n_printf( "EL CAMPESINO %d NO ESTA CONTRATADO POR MI\n", campesino2->GetId() );
									MostrarInformacion("No has contratado al campesino");
								}
							}
						}
						else
							ContratarCampesino( campesino2 );
					}
				}
			}
			break;
		case ESPERAR:
			break;
		case NACER:
			personaje->Nacer();
			break;
		case MORIR:
			personaje->Morir();

			// FMOD: Sonido de personajes
			SonidoPersonaje(MORIR);
			break;
	}
	return;
}

void nJugador::SeleccionarPosicion( vector3 posicion )
{
	nShaderNode* shader = (nShaderNode*) kernelServer->Lookup( "/game/scene/seleccion/shader" );
	char* params = NULL;

    nMapa *v_mapa = (nMapa *) kernelServer->Lookup( "/data/mapa" );

	if ( v_mapa->Movimiento_Valido(0,0,floor(posicion.x),floor(posicion.z)) )
	{
	kernelServer->SetCwd( shader );
	refScriptServer->Run( "runshader destino.n", params );
	MostrarSeleccion( posicion );
	TiempoSeleccion = TIEMPO_PERMANENCIA_SELECCION;

	AccionIrA( posicion );
	}
}

void nJugador::Tick( float t )
{
	if ( personajeOculto != NULL && GetJugadorPrincipal() )
	{
		vector3 posPersonaje = personaje->GetPosition();
		personajeOculto->Txyz( posPersonaje.x, posPersonaje.y, posPersonaje.z );
	}

	if ( !pila_acciones.IsEmpty() )
	{
		if ( AccionTerminada( t - TiempoAnterior ) )
			EjecutarSiguienteAccion();
	}

	// Mostramos ambiente de mar según nos acercamos al borde
	if (this->GetJugadorPrincipal())
	{
		TratarSonidoAmbiente();
	}
	TratarTiempo( t );
}

void nJugador::TratarSonidoAmbiente()
{
	nGame		*juego					= (nGame *)kernelServer->Lookup("/game");
	vector3     vec_distancia_centro    = personaje->GetPosition()-vector3(40.0f,0.0f,40.0f);
	float       distancia_centro		= vec_distancia_centro.len();
	float		v_volumen				= 0.0f;

	if (distancia_centro<26.0f)
		v_volumen = 0.0f;
	else
		v_volumen=(distancia_centro-26.0f)/14.0f;

	if (v_volumen > 1.0f)
		v_volumen = 1.0f;

	// Asignamos al canal 3 (Ambiente Mar) el volumen
	// en función de la posición del jugador
	juego->SetMusicVolume(3,v_volumen);
}

void nJugador::TratarTiempo( float t )
{
	float TiempoActual = t;
	float dt = TiempoActual - TiempoAnterior;
	Tiempo += dt;

	if ( TienePersonaje )
	{
		personaje->SetTiempoPago( personaje->GetTiempoPago() + dt );
		personaje->SetTiempo( personaje->GetTiempo() + dt );
		TiempoEfectivo = GetTiempoEfectivo() + dt;
	}
	TratarMarcaSeleccionPosicion( dt );
	TratarMuestraInformacion( dt );
	TratarMarcadorTiempoTurno();
	TiempoAnterior = TiempoActual;
}

void nJugador::TratarMarcadorTiempoTurno()
{
	char        path[N_MAXPATH];
	int         id_campesino = 0;
	int         campesinos_contratados;
	nArray<int> lista_campesinos;

	if ( TienePersonaje )
	{
		float tiempoTurno = 1.0f - personaje->GetTiempo()/MAX_TIEMPO_TURNO;
		float tiempoTurnoPago = 1.0f - personaje->GetTiempoPago()/MAX_TIEMPO_PAGO;

		if ( tiempoTurnoPago > 0.0f )
		{
			if ( GetJugadorPrincipal() )
				refChannelServer->SetChannel1f( CanalTiempoPago, tiempoTurnoPago );
		}
		else
		{
			campesinos_contratados = personaje->GetCampesinosContratados();
			if ( campesinos_contratados > 0 )
			{
				lista_campesinos = personaje->GetListaCampesinosContratados();
				int pago = 0;
				for ( int i = 0; i < campesinos_contratados; i++ )
				{
					if ( PersonajeActivo != nPersonaje::PERSONAJE_MAESTRO || !LoTieneConstruido( "ESCUELA DE TEOLOGIA" ) )
					{
						id_campesino = lista_campesinos.At( i );
						sprintf( path, "/game/world/campesino%d", id_campesino );
						nCampesino* campesino = (nCampesino*) kernelServer->Lookup( path );
						pago += PagarACampesino( campesino );
					}
				}
				if ( pago > 0 )
				{
					char buf[N_MAXPATH];
					sprintf( buf, "Has pagado %d moneda/s de oro", pago );
					MostrarInformacion( buf );
				}
				personaje->SetTiempoPago( 0.0f );
				if ( GetJugadorPrincipal() )
					refChannelServer->SetChannel1f( CanalTiempoPago, 0.0f );
			}
		}

		// Tratamiento del color de la barra
		if (this->GetJugadorPrincipal())
		{
			nTexArrayNode *textura_barra=(nTexArrayNode *) kernelServer->Lookup("/game/scene/hud/tiempoTurno/tex");

			if ( tiempoTurno < 0.75f && BarraBloqueada && !this->personaje->EstaMuerto() )
			{
				BarraBloqueada=false;
				textura_barra->SetTexture(0,"textures:barra_azul.png",NULL);
			}

			if ( tiempoTurno > 0.75f && !BarraBloqueada && !this->personaje->EstaMuerto() &&
				 this->personaje->GetTipoPersonaje() != nPersonaje::PERSONAJE_SHOGUN )
			{
				BarraBloqueada=true;
				textura_barra->SetTexture(0,"textures:barra_roja.png",NULL);
			}

			if ( tiempoTurno > 0.75f && BarraBloqueada && !this->personaje->EstaMuerto() &&
				 this->personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_SHOGUN )
			{
				BarraBloqueada=false;
				textura_barra->SetTexture(0,"textures:barra_azul.png",NULL);
			}

			if ( this->personaje->EstaMuerto() && !BarraBloqueada )
			{
				BarraBloqueada=true;
				textura_barra->SetTexture(0,"textures:barra_roja.png",NULL);
			}
		}

		if ( tiempoTurno > 0.0f )
		{
			if ( GetJugadorPrincipal() )
			{
				refChannelServer->SetChannel1f( CanalTiempoTurno, tiempoTurno );
				if ( personaje->EstaMuerto() )
					SetModoConstruccion( false );
			}
		}
		else if ( tiempoTurno < 0.0f )
		{
			if ( GetModoConstruccion() )
				SetModoConstruccion( false );

			if ( !this->EstaBloqueado() )
			{
				AccionLiberarPersonaje();
				tiempoTurno = 0.0f;
			}
		}
	}
}

void nJugador::TratarMarcaSeleccionPosicion( float dt )
{
	if ( TiempoSeleccion < 0.0f )
	{
		OcultarSeleccion();
		TiempoSeleccion=0.0f;
	}
	else if ( TiempoSeleccion > 0.0f )
		TiempoSeleccion -= dt;
}

/**
* Agregado el 11/6
* Muestra la información acerca de las acciones realizadas del jugador
*/

void nJugador::TratarMuestraInformacion( float dt )
{
	if ( TiempoInformacion < 0.0f )
	{
		OcultarInformacion();
	}
	else if ( TiempoInformacion > 0.0f )
	{
		TiempoInformacion -= dt;
	}
}

void nJugador::MostrarInformacion(const char *p_informacion)
{
	if (this->GetJugadorPrincipal())
	{
		char *params;
		char buf[N_MAXPATH];
		sprintf(buf,"MostrarLineaInformacion \"%s\"",p_informacion);
		refScriptServer->Run(buf,params);
		TiempoInformacion=TIEMPO_MUESTRA_INFORMACION;
	}
}

void nJugador::OcultarInformacion()
{
	if (this->GetJugadorPrincipal())
	{
		char *params;
		char buf[N_MAXPATH];
		sprintf(buf,"OcultarLineaInformacion");
		refScriptServer->Run(buf,params);
		TiempoInformacion=0.0f;
	}
}

/**
* Hasta aqui lo nuevo
*/

void nJugador::OcultarSeleccion()
{
	n3DNode* seleccion = (n3DNode*) kernelServer->Lookup( "/game/scene/seleccion" );
	seleccion->SetActive( false );
}

void nJugador::MostrarSeleccion(vector3 p_posicion)
{
	n3DNode* seleccion = (n3DNode*) kernelServer->Lookup( "/game/scene/seleccion" );
	nWorld *v_world = (nWorld *) kernelServer->Lookup( "/game/world" );

	vector3 w_normal;
	v_world->GetNormal(vector3(p_posicion.x,p_posicion.y,p_posicion.z),w_normal);
	seleccion->Qxyzw(w_normal.x,w_normal.y,w_normal.z,0.0f);
	seleccion->Txyz( p_posicion.x, p_posicion.y+0.05, p_posicion.z );

	seleccion->SetActive( true );
}

void nJugador::SetModoConstruccion( bool activo )
{
	ModoConstruccion = activo;
	if ( ModoConstruccion )
	{
		CambiarVisibilidadHUD( false );
		distrito->MostrarSeleccion();
	}
	else
	{
		CambiarVisibilidadHUD( true );
		distrito->OcultarSeleccion();
		MostrarConstruccionNoValida( vector3(0,0,0) );
	}
}

void nJugador::CambiarVisibilidadHUD( bool visibilidad )
{
	n3DNode* nodoHUD = (n3DNode*) kernelServer->Lookup( "/game/scene/hud" );
	nodoHUD->SetActive( visibilidad );
}

bool nJugador::PosicionDentroDeDistrito( vector3 posicion )
{
	return distrito->DentroDelDistrito( posicion.x, posicion.z );
}

void nJugador::ConstruirEdificio( vector3 posicion )
{
	char* params = NULL;
	char  path[N_MAXPATH];

	VaciarPilaAcciones();

	if ( GetJugadorPrincipal() )
	{
		sprintf( path, "delete /game/scene/hud/plano%d", planos.At( plano_a_construir )->GetIdHUD() );
		refScriptServer->Run( path, params );
	}

	nEntity* edificio = (nEntity*) kernelServer->Lookup( EdificioAConstruir.Get() );

	SetModoConstruccion( false );
	edificio->SetPosition( floor( posicion.x ) + 0.5f, floor( posicion.z ) + 0.5f );
	edificio->SetEntityHeight( 0.0f );

	AccionConstruirEdificio(edificio);
}

void nJugador::MostrarConstruccionValida( vector3 posicion )
{
	nConstruccion* edificio = (nConstruccion*) kernelServer->Lookup( EdificioAConstruir.Get() );
	edificio->SetPosition( floor( posicion.x ) + 0.5f, floor( posicion.z ) + 0.5f );
	edificio->SetValorConstruccion( 5.0f );
}

void nJugador::MostrarConstruccionNoValida( vector3 posicion )
{
	nConstruccion* edificio = (nConstruccion*) kernelServer->Lookup( EdificioAConstruir.Get() );
	if ( edificio != NULL )
	{
		edificio->SetValorConstruccion( 0.0f );
	}
}

const char* nJugador::GetNombre() const
{
	return Nombre.Get();
}

float nJugador::GetTiempo()
{
	return Tiempo;
}

float nJugador::GetTiempoEfectivo()
{
	return TiempoEfectivo;
}

int nJugador::GetCampesinosContratados()
{
	return CampesinosContratados;
}

int nJugador::GetCampesinosLiberados()
{
	return CampesinosLiberados;
}

int nJugador::GetCampesinosResucitados()
{
	return CampesinosResucitados;
}

int nJugador::GetPersonajesRobados()
{
	return PersonajesRobados;
}

int nJugador::GetPersonajesAsesinados()
{
	return PersonajesAsesinados;
}

int nJugador::GetEdificiosDerribados()
{
	return EdificiosDerribados;
}

int nJugador::GetOro()
{
	return Oro;
}

nArray<nJugador::TPuntos> nJugador::GetPuntos()
{
	return Puntos;
}

nPersonaje* nJugador::GetPersonaje()
{
	return personaje;
}

nPersonaje* nJugador::GetSiguientePersonaje()
{
	return siguientePersonaje;
}


void nJugador::SetPersonaje( nPersonaje* p_personaje )
{
	if ( GetJugadorPrincipal() )
	{
		if ( personaje->GetTipoPersonaje() == nPersonaje::PERSONAJE_CAMPESINO )
		{
			//n_printf("**** GUARDA CAMPESINO ****\n");
			this->personajeOculto = this->personaje;
		}
		else
		{
			//n_printf("**** BORRA PERSONAJE OCULTO ****\n");
			this->personajeOculto = NULL;
		}
	}
	if ( this->personaje != NULL )
	{
		// Almacenamos la posición del personaje y lo escondemos
		vector3 v_posicion = this->personaje->GetPosition();
		this->personaje->SetVisible( false );

		//n_printf("**** OCULTA PERSONAJE %s ****\n",this->personaje->GetName());

		// Mostramos el nuevo personaje y lo reposicionamos
		this->personaje = p_personaje;
		this->personaje->SetJugador( this );
		this->personaje->SetVisible( true );
		//n_printf("**** MUESTRA 1 PERSONAJE %s ****\n",personaje->GetName());
		this->personaje->SetPosition( v_posicion.x, v_posicion.z );
		//this->personaje->SetEntityHeight( v_posicion.y );
	}
	else
	{
		this->personaje = p_personaje;
		this->personaje->SetJugador( this );
		this->personaje->SetVisible( true );
		//n_printf("**** MUESTRA 2 PERSONAJE %s ****\n",personaje->GetName());
	}
	SetSiguientePersonaje(p_personaje);
	this->personaje->MostrarSombra();
}

void nJugador::SetSiguientePersonaje( nPersonaje* p_personaje )
{
	this->siguientePersonaje = p_personaje;
}

bool nJugador::GetModoConstruccion()
{
	return ModoConstruccion;
}

bool nJugador::PuedeCogerPlanos()
{
	// Si no ha se ha cogido planos en el turno
	if ( AccionesPorTurno.VisitasACasaPlanos == 0 )
	{
		// Si no se ha cogido dinero en el turno
		if ( AccionesPorTurno.VisitasACasaMoneda == 0 )
		{
			for ( int i = 0; i < N_MAXPLANOS; i++ )
			{
				if ( planos.At( i ) == NULL )
				{
					personaje->SetAnimacion( nPersonaje::ANIM_COGER );
					return true;
				}
			}
			return false;
		}
		else
		{
			if ( PersonajeActivo == nPersonaje::PERSONAJE_INGENIERO && LoTieneConstruido( "ESCUELA DE INGENIERIA" ) )
			{
				for ( int i = 0; i < N_MAXPLANOS; i++ )
				{
					if ( planos.At( i ) == NULL )
					{
						personaje->SetAnimacion( nPersonaje::ANIM_COGER );
						return true;
					}
				}
				return false;
			}
			else
			{
				//n_printf( "YA SE HA COGIDO DINERO\n" );
				MostrarInformacion("No puedes coger planos en este turno");
				return false;
			}
		}
	}
	else
	{
		if ( AccionesPorTurno.VisitasACasaPlanos == 1 )
		{
			if ( PersonajeActivo == nPersonaje::PERSONAJE_INGENIERO )
			{
				for ( int i = 0; i < N_MAXPLANOS; i++ )
				{
					if ( planos.At( i ) == NULL )
					{
						personaje->SetAnimacion( nPersonaje::ANIM_COGER );
						return true;
					}
				}
				return false;
			}
			else
			{
				//n_printf( "YA SE HA COGIDO UN PLANO\n" );
				MostrarInformacion("No puedes coger mas planos en este turno");
				return false;
			}
		}
		else
		{
			//n_printf( "YA SE HAN COGIDO DOS PLANOS\n" );
			MostrarInformacion("No puedes coger mas planos en este turno");
			return false;
		}
	}
}

void nJugador::SetId( int id )
{
	IdJugador = id;
	distrito = new nDistrito;
	distrito->AgregarAyuntamiento( id );
}

int nJugador::GetId()
{
	return IdJugador;
}

void nJugador::CogerDinero()
{
	nAyuntamiento* ayto = NULL;
	list<nPlano*>::iterator iter;
	bool encontrado = false;

	TAccion* accionActual = pila_acciones.Top();

	// En la casa de la moneda se cobra sin atender al personaje
	if ( strcmp( accionActual->destino->GetClass()->GetName(), "ncasamoneda" ) == 0 )
	{
		// Si no ha realizado la acción de coger dinero todavía en el turno
		if ( AccionesPorTurno.VisitasACasaMoneda == 0 )
		{
			// Si no se ha realizado la acción de coger plano todavía en el turno
			if ( AccionesPorTurno.VisitasACasaPlanos == 0 )
			{
				if ( PersonajeActivo == nPersonaje::PERSONAJE_CAMPESINO )
				{
					//n_printf( "SIENDO CAMPESINO NO SE PUEDE COGER DINERO\n" );
					MostrarInformacion("Siendo campesino no puedes coger dinero");
				}
				else
				{
					// Todos los personajes cogen 10 monedas de oro
					SetOro( GetOro() + 10 );

					// Si además se tiene el edificio especial, otras 10 monedas
					if ( LoTieneConstruido( "ESCUELA DE COMERCIO" ) )
						SetOro( GetOro() + 10 );

					// Y si además es el mercader, otras 10 monedas
					if ( PersonajeActivo == nPersonaje::PERSONAJE_MERCADER )
					{
						personaje->SetAnimacion( nPersonaje::ANIM_ACCION );
						SetOro( GetOro() + 10 );
					}
					else
						personaje->SetAnimacion( nPersonaje::ANIM_COGER );
					AccionesPorTurno.VisitasACasaMoneda++;

					// Sonido de coger moneda
					nGame *juego=(nGame *) kernelServer->Lookup("/game");
					juego->Play3DSound(0,"sounds:posicional/cogermoneda.wav",accionActual->destino->GetPosition());
				}
			}
			else
			{
				if ( PersonajeActivo == nPersonaje::PERSONAJE_INGENIERO && LoTieneConstruido( "ESCUELA DE INGENIERIA" ) )
				{
					personaje->SetAnimacion( nPersonaje::ANIM_COGER );
					SetOro( GetOro() + 10 );
					// Si además se tiene el edificio especial, otras 10 monedas
					if ( LoTieneConstruido( "ESCUELA DE COMERCIO" ) )
						SetOro( GetOro() + 10 );
					AccionesPorTurno.VisitasACasaMoneda++;

					// Sonido de coger moneda
					nGame *juego=(nGame *) kernelServer->Lookup("/game");
					juego->Play3DSound(0,"sounds:posicional/cogermoneda.wav",accionActual->destino->GetPosition());
				}
				else
				{
					//n_printf( "YA SE HA COGIDO UN PLANO\n" );
					MostrarInformacion("No puedes coger dinero en este turno");
				}
			}
		}
		else
		{
			//n_printf( "YA SE HA COGIDO DINERO DE LA CASA DE PLANOS\n" );
			MostrarInformacion("No puedes coger mas dinero en este turno");
		}
	}
	else  // En un ayuntamiento
	{
		// Según el personaje se puede cobrar por construcción
		if ( PersonajeActivo == nPersonaje::PERSONAJE_SHOGUN   ||
			 PersonajeActivo == nPersonaje::PERSONAJE_MAESTRO  ||
			 PersonajeActivo == nPersonaje::PERSONAJE_MERCADER ||
			 PersonajeActivo == nPersonaje::PERSONAJE_SAMURAI )
		{
			ayto = (nAyuntamiento*) accionActual->destino;
			// Si el jugador está junto a su ayuntamiento
			if ( ayto->GetId() == IdJugador )
			{
				// Si no se ha realizado la recaudación todavía en el turno
				if ( AccionesPorTurno.VisitasAAyuntamiento == 0 )
				{
					int recaudo = 0;
					// Se ojean todas sus construcciones
					for ( int i = 0; i < distrito->GetListaPlanos().Size(); i++ )
					{
						if ( ( distrito->GetEdificio(i)->GetDistintivo() == nPlano::TPlano::COMERCIAL &&
							   PersonajeActivo == nPersonaje::PERSONAJE_MERCADER ) ||
							 ( distrito->GetEdificio(i)->GetDistintivo() == nPlano::TPlano::IMPERIAL &&
							   PersonajeActivo == nPersonaje::PERSONAJE_SHOGUN ) ||
							 ( distrito->GetEdificio(i)->GetDistintivo() == nPlano::TPlano::MILITAR &&
						  	   PersonajeActivo == nPersonaje::PERSONAJE_SAMURAI ) ||
							 ( distrito->GetEdificio(i)->GetDistintivo() == nPlano::TPlano::RELIGIOSO &&
							   PersonajeActivo == nPersonaje::PERSONAJE_MAESTRO ) ||
							   distrito->GetEdificio(i)->GetNombre() == "ESCUELA DE MAGIA" )
						{
							recaudo += 10;
							encontrado = true;
						}
					}

					if ( encontrado )
					{
						int campesinos_contratados = personaje->GetCampesinosContratados();
						int incremento = 0;
						if ( PersonajeActivo == nPersonaje::PERSONAJE_MERCADER && campesinos_contratados > 0 )
							incremento = (campesinos_contratados*recaudo)/10;
						SetOro( GetOro() + recaudo + incremento );
						personaje->SetAnimacion( nPersonaje::ANIM_COGER );
						AccionesPorTurno.VisitasAAyuntamiento++;

						// Sonido de coger moneda
						nGame *juego=(nGame *) kernelServer->Lookup("/game");
						juego->Play3DSound(0,"sounds:posicional/cogermoneda.wav",accionActual->destino->GetPosition());
					}
				}
				else
				{
					//n_printf( "YA SE HA COBRADO DEL AYUNTAMIENTO\n" );
					MostrarInformacion("No puedes cobrar mas dinero en este turno");
				}
			}
		}
	}
}

void nJugador::IntercambiarPlanos()
{
	char  path[N_MAXPATH];
	char* params = NULL;

	TAccion* accionActual = pila_acciones.Top();

	if ( PersonajeActivo == nPersonaje::PERSONAJE_SHUGENJA )
	{
		// Si no se ha realizado ningún intercambio de planos todavía en el turno
		if ( AccionesPorTurno.Intercambios == 0 )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_CAMBIAR_PLANOS );
			int planos_a_cambiar = 0;
			nCasaPlanos* casaplanos = (nCasaPlanos*) kernelServer->Lookup( "/game/world/casaplanos" );
			int planos_quedan = casaplanos->NumeroDePlanos();
			//n_printf( "Quedan %d planos en la Casa de Planos\n", planos_quedan );
			for ( int i = 0; i < N_MAXPLANOS; i++ )
			{
				if ( planos.At( i ) != NULL )
				{
					if ( !planos.At( i )->EstaConstruido() )
					{
						//n_printf( "Se va a intercambiar el plano %d %s con IdHUD = %d\n", planos.At( i )->GetIdEdificio(), planos.At( i )->GetNombre().c_str(), planos.At( i )->GetIdHUD() );
						planos_a_cambiar++;
						if ( GetJugadorPrincipal() )
						{
							sprintf( path, "delete /game/scene/hud/plano%d", planos.At( i )->GetIdHUD() );
							refScriptServer->Run( path, params );
						}
						planos.At( i )->SetIdHUD( 0 );
						casaplanos->InsertaPlano( planos.At( i ) );
						planos.At( i ) = NULL;
					}
				}
			}

			// Se decide si coger el mismo número de planos que intercambia, o,
			// coger los planos que quedan en la casa de planos, por ser un número menor
			int cambio;
			if ( planos_a_cambiar > planos_quedan )
				cambio = planos_quedan;
			else
				cambio = planos_a_cambiar;

			for ( int i = 0; i < cambio; i++ )
			{
				nPlano* plano;
				if ( casaplanos->GetPlano( plano ) )
					CogerPlano( plano );
				else
					break;
			}
			AccionesPorTurno.Intercambios++;
		}
		else
		{
			//n_printf( "YA SE HA REALIZADO UN INTERCAMBIO DE PLANOS\n" );
			MostrarInformacion("No se pueden cambiar mas planos en este turno");
		}
	}
}

void nJugador::IntercambiarPlanosConJugador( nJugador* &propietario )
{
	char            path[N_MAXPATH];
	char*           params = NULL;
	nArray<nPlano*> lista_planos_aux;
	int             j = 0;

	TAccion* accionActual = pila_acciones.Top();

	if ( PersonajeActivo == nPersonaje::PERSONAJE_SHUGENJA )
	{
		// Si no se ha realizado ningún intercambio de planos todavía en el turno
		if ( AccionesPorTurno.Intercambios == 0 )
		{
			personaje->SetAnimacion( nPersonaje::ANIM_CAMBIAR_PLANOS );
			int planos_a_cambiar = 0;
			lista_planos_aux.SetFixedSize( N_MAXPLANOS );
			for ( int i = 0; i < N_MAXPLANOS; i++ )
				lista_planos_aux.Set( i, NULL );

			// Se dejan los planos del jugador principal en un array auxiliar
			for ( int i = 0; i < N_MAXPLANOS; i++ )
			{
				if ( planos.At( i ) != NULL )
				{
					if ( !planos.At( i )->EstaConstruido() )
					{
						//n_printf( "El jugador %d deja en el array auxiliar el plano %d %s con IdHUD = %d\n", GetId(), planos.At( i )->GetIdEdificio(), planos.At( i )->GetNombre().c_str(), planos.At( i )->GetIdHUD() );
						if ( GetJugadorPrincipal() )
						{
							sprintf( path, "delete /game/scene/hud/plano%d", planos.At( i )->GetIdHUD() );
							refScriptServer->Run( path, params );
						}
						planos.At( i )->SetIdHUD( planos_a_cambiar );
						lista_planos_aux.Set( planos_a_cambiar, planos.At( i ) );
						planos_a_cambiar++;
						planos.Set( i, NULL );
					}
				}
			}

			int planos_quedan = propietario->GetNumPlanos();
			//n_printf( "Le quedan %d planos al jugador %d\n", planos_quedan, propietario->GetId() );

			// Si el otro jugador tiene planos, se los pasa al jugador principal
			if ( planos_quedan > 0 )
			{
				for ( int i = 0; i < N_MAXPLANOS; i++ )
				{
					if ( propietario->planos.At( i ) != NULL )
					{
						if ( !propietario->planos.At( i )->EstaConstruido() )
						{
							//n_printf( "El jugador %d coge el plano %d %s con IdHUD = %d\n", GetId(), propietario->GetPlanos().At( i )->GetIdEdificio(), propietario->GetPlanos().At( i )->GetNombre().c_str(), propietario->GetPlanos().At( i )->GetIdHUD() );
							if ( propietario->GetJugadorPrincipal() )
							{
								sprintf( path, "delete /game/scene/hud/plano%d", propietario->GetPlanos().At( i )->GetIdHUD() );
								refScriptServer->Run( path, params );
							}
							propietario->planos.At( i )->SetIdHUD( j );
							CogerPlano( propietario->planos.At( i ) );
							j++;
							propietario->planos.Set( i, NULL );
						}
					}
				}
			}

			// El otro jugador coge los planos del array auxiliar
			for ( int i = 0; i < N_MAXPLANOS; i++ )
			{
				if ( lista_planos_aux.At( i ) != NULL )
				{
					//n_printf( "El jugador %d coge el plano %d %s con IdHUD = %d\n", propietario->GetId(), lista_planos_aux.At( i )->GetIdEdificio(), lista_planos_aux.At( i )->GetNombre().c_str(), lista_planos_aux.At( i )->GetIdHUD() );
					propietario->planos.Set( i, lista_planos_aux.At( i ) );
					propietario->planos.At( i )->SetIdHUD( i );
					propietario->planos.At( i )->SetPlanoHUD();
					vector2 posicion = lista_planos_aux.At( i )->GetPosition();
					posicion.x = i * 0.0875 + 0.28;
					propietario->planos.At( i )->SetPosition( posicion );
					if ( propietario->GetJugadorPrincipal() )
						propietario->planos.At( i )->Pintar();
					lista_planos_aux.Set( i, NULL );
				}
			}
			AccionesPorTurno.Intercambios++;
		}
		else
		{
			//n_printf( "YA SE HA REALIZADO UN INTERCAMBIO DE PLANOS\n" );
			MostrarInformacion("No se pueden cambiar mas planos en este turno");
		}
	}
}

nDistrito* nJugador::GetDistrito()
{
	return distrito;
}

void nJugador::SetPartida( nPartida* refpartida )
{
	partida = refpartida;
}

bool nJugador::JugadorInactivo()
{
	return !JugadorActivo;
}

int nJugador::PagarACampesino( nCampesino* &campesino )
{
	if ( GetOro() < campesino->GetSueldo() )
	{
		LiberarCampesino( campesino );
		return 0;
	}
	else
	{
		campesino->SetOro( campesino->GetOro() + campesino->GetSueldo() );
		SetOro( GetOro() - campesino->GetSueldo() );
		return campesino->GetSueldo();
	}
}

void nJugador::SetJugadorPrincipal( bool es_principal )
{
	JugadorPrincipal = es_principal;
}

bool nJugador::GetJugadorPrincipal()
{
	return JugadorPrincipal;
}

bool nJugador::LoTieneConstruido( const char* nombre_edificio )
{
	string v_nombre=nombre_edificio;
	return distrito->ExisteEdificio( v_nombre );
}

// Roba al personaje que tenga mas cercano (incluyendo campesinos);
void nJugador::IA_RobarPersonajeMasCercano()
{
	// Buscar al personaje más cercano
	nEntity* v_destino = partida->PersonajeMasCercano( personaje->GetPosition().x, personaje->GetPosition().z, GetId() );
	if ( v_destino != NULL )
	{
		nPersonaje* personaje_a_robar = (nPersonaje*) v_destino;

		// Agregamos la acción
		AccionEspecial( v_destino );
		//n_printf("IA %d VA A ROBAR AL PERSONAJE %s\n",GetId(),personaje_a_robar->GetName());
	}
}

// Roba al personaje que tenga más edificios construidos
void nJugador::IA_RobarPersonajeMejor()
{
	nJugador *v_jugador = NULL;
	// Buscar al personaje con más edificios
	int pct_accion = rand() % 100;

	if ( pct_accion < 70 && !partida->GetJugadorPrincipal()->EstaBloqueado() )	
	{
		v_jugador = partida->GetJugadorPrincipal();
	}
	else
	{
		v_jugador = partida->JugadorMejor( GetId() );
	}

	if ( v_jugador != NULL )
	{
		if ( v_jugador->GetPersonaje() != NULL )
		{
			nEntity *v_destino = (nEntity *) v_jugador->GetPersonaje();
			nPersonaje *personaje_a_robar = (nPersonaje *) v_destino;

			// Agregamos la acción
			AccionEspecial( v_destino );
			//n_printf("IA %d VA A ROBAR AL PERSONAJE %s\n",GetId(),personaje_a_robar->GetName());
		}
	}
}

// Mata al jugador que tenga más edificios construidos
void nJugador::IA_MatarJugadorMejor()
{
	nJugador *v_jugador = NULL;

	// Selecciona el jugador con más edificios construidos
	int pct_accion = rand() % 100;

	if ( pct_accion < 70 && !partida->GetJugadorPrincipal()->EstaBloqueado() )	
	{
		v_jugador = partida->GetJugadorPrincipal();
	}
	else
	{
		v_jugador = partida->JugadorMejor( GetId() );
	}

	if ( v_jugador != NULL )
	{
		if ( v_jugador->GetPersonaje() != NULL )
		{
			if ( !v_jugador->GetPersonaje()->EstaMuerto() )
			{
				nEntity *v_destino = (nEntity *) v_jugador->GetPersonaje();
				nPersonaje *personaje_a_matar = (nPersonaje *) v_destino;

				// Agregamos la acción
				AccionEspecial( v_destino );
				//n_printf("IA %d VA A MATAR AL PERSONAJE %s\n",GetId(),personaje_a_matar->GetName());
			}
		}
	}
}

// Resucita al campesino que esté más cerca
void nJugador::IA_Resucitar()
{
	// Buscar al campesino más cercano
	nEntity *v_destino = partida->CampesinoMuertoMasCercano( personaje->GetPosition().x, personaje->GetPosition().z, GetId() );

	if ( v_destino != NULL )
	{
		nCampesino *campesino = (nCampesino *) v_destino;
		// Agregamos la acción
		AccionEspecial( v_destino );
		//n_printf("IA %d VA A RESUCITAR AL CAMPESINO %d\n",GetId(),campesino->GetId());
	}
}

// Cobra impuestos en un ayuntamiento diferente al suyo
void nJugador::IA_RecaudarEnAyuntamientoMasCercano()
{
	// Selecciona el ayuntamiento más cercano al jugador
	nEntity *v_destino = partida->AyuntamientoMasCercano( personaje->GetPosition().x, personaje->GetPosition().z, GetId() );
	if ( v_destino != NULL )
	{
		nAyuntamiento *ayuntamiento = (nAyuntamiento*) v_destino;
		// Agregamos la acción
		AccionRecaudar( v_destino );
		//n_printf("IA %d VA A RECAUDAR IMPUESTOS EN EL AYUNTAMIENTO DEL JUGADOR %d\n",GetId(),ayuntamiento->GetId());
	}
}

// Contrata al campesino que esté más cerca
void nJugador::IA_Contratar()
{
	// Buscar al campesino más cercano
	nEntity* v_destino = partida->CampesinoMasCercano( personaje->GetPosition().x, personaje->GetPosition().z, GetId() );

	if ( v_destino != NULL )
	{
		bool puede_contratar = true;
		nCampesino* campesino = (nCampesino*) v_destino;
		if ( GetOro() < campesino->GetPrecio() )
			puede_contratar = false;

		if ( !puede_contratar )
			return;

		// Agregamos la acción
		AccionContratar( v_destino );
		//n_printf("IA %d VA A CONTRATAR AL CAMPESINO %d\n",GetId(),campesino->GetId());
	}
}

// Va a la casa de planos a coger un plano
void nJugador::IA_CogerPlano()
{
	// Va a la casa de planos a hacer la acción
	nEntity* casaplanos = (nEntity*) kernelServer->Lookup( "/game/world/casaplanos" );

	// Agregamos la acción
	AccionCogerPlano( casaplanos );
	//n_printf("IA %d VA A COGER UN PLANO\n",GetId());
}

// Va a la casa de planos a intercambiar los planos
void nJugador::IA_IntercambiarPlanos()
{
	bool puede_construir = false;

	for ( int i = 0; i < planos.Size(); i++ )
	{
		if ( planos.At( i ) != NULL )
		{
			if ( !planos.At( i )->EstaConstruido() )
			{
				if ( planos.At( i )->GetCoste() <= GetOro() )
				{
					puede_construir = true;
					break;
				}
			}
		}
	}

	if ( puede_construir )
		return;

	// Va a la casa de planos a hacer la acción
	nEntity* casaplanos = (nEntity*) kernelServer->Lookup( "/game/world/casaplanos" );

	// Agregamos la acción
	AccionIntercambiarPlanos( casaplanos );
	//n_printf("IA %d VA A INTERCAMBIAR %d PLANOS CON LA CASA DE PLANOS\n",GetId(),GetNumPlanos());
}

// Va a la casa de la moneda a coger dinero
void nJugador::IA_CogerDinero()
{
	// Va a la casa de la moneda a hacer la acción
	nEntity* casamoneda = (nEntity*) kernelServer->Lookup( "/game/world/casamoneda" );

	// Agregamos la acción
	AccionCogerDinero( casamoneda );
	//n_printf("IA %d VA A COGER DINERO\n",GetId());
}

// Mata al jugador que tenga mas cercano
void nJugador::IA_MatarJugadorMasCercano()
{
	// Selecciona el jugador más cercano
	nEntity* v_destino = partida->JugadorMasCercano( personaje->GetPosition().x, personaje->GetPosition().z, GetId() );
	if ( v_destino != NULL )
	{
		nPersonaje* personaje_a_matar = (nPersonaje*) v_destino;

		// Agregamos la acción
		AccionEspecial( v_destino );
		//n_printf("IA %d VA A MATAR AL PERSONAJE %s\n",GetId(),personaje_a_matar->GetName());
	}
}

// Construye un plano en su distrito
void nJugador::IA_ConstruirPlano()
{
	int    coste_max   = 0;
	int    coste_corte = 70;
	nPlano *plano_sel  = NULL;
	bool   encontrado  = false;

	// Selecciona un plano que construir
	while ( !encontrado )
	{
		for ( int i = 0; i < planos.Size(); i++ )
		{
			if ( planos.At( i ) != NULL )
			{
				if ( !planos.At( i )->EstaConstruido() )
				{
					if ( planos.At( i )->GetCoste() > coste_max && planos.At( i )->GetCoste() < coste_corte )
					{
						coste_max = planos.At( i )->GetCoste();
						plano_sel = planos.At( i );
					}
				}
			}
		}

	//n_printf("COSTE_MAX=%d\n",coste_max);
	//if ( plano_sel != NULL )
	//n_printf("PLANO_SEL=%s\n",plano_sel->GetNombre().c_str());
		if ( coste_max > 0 )
		{
			if ( coste_max <= GetOro() )
			{
				encontrado        = true;
				plano_a_construir = plano_sel->GetIdHUD();
//n_printf("PUEDE CONSTRUIRLO\n");
			}
			else
			{
				coste_corte = coste_max;
				coste_max   = 0;
//n_printf("NO PUEDE CONSTRUIRLO. SIGUE BUSCANDO\n");
			}
		}
		else
		{
			encontrado = true;
//n_printf("NO PUEDE CONSTRUIR NINGUN EDIFICIO\n");
		}
	}

	if ( coste_max > 0 )
	{
		// Selecciona un lugar donde construir un plano
		vector2 posicion = distrito->BuscarPosicionConstruccion();
		char    path[N_MAXPATH];
		sprintf( path, "/game/world/cons%d", planos.At( plano_a_construir )->GetIdEdificio() );
		nEntity *construccion = (nEntity *) kernelServer->Lookup( path );
		construccion->SetPosition( floor( posicion.x ) + 0.5f, floor( posicion.y ) + 0.5f );
		construccion->SetEntityHeight( 0.0f );

		// Agregamos la acción
		AccionConstruirEdificio( construccion );
		//n_printf("IA %d VA A CONSTRUIR EL EDIFICIO %d %s\n",GetId(),planos.At(plano_a_construir)->GetIdEdificio(),planos.At(plano_a_construir)->GetNombre().c_str());
	}
}

// Intercambia planos con el jugador que tenga más planos (trampa jijiji)
void nJugador::IA_IntercambiarPlanosConJugador()
{
	// Selecciona el jugador que tenga más planos
	nEntity* v_destino = partida->JugadorConMasPlanos( GetId() );
	if ( v_destino != NULL )
	{
		nAyuntamiento* ayuntamiento = (nAyuntamiento*) v_destino;
		if ( ayuntamiento->GetId() == GetId() )
			return;

		// Agregamos la acción
		AccionRecaudar( v_destino );
		//n_printf("IA %d VA A INTERCAMBIAR %d PLANOS CON JUGADOR %d\n",GetId(),GetNumPlanos(),ayuntamiento->GetId());
	}
}

// Cobra en el ayuntamiento los impuestos
void nJugador::IA_Recaudar()
{
	char path[N_MAXPATH];
	// Selecciona el propio ayuntamiento del jugador
	sprintf( path, "/game/world/ayto%d", GetId() );
	nEntity* v_destino = (nEntity*) kernelServer->Lookup( path );
	if ( GetDistrito()->NumeroConstrucciones() == 0 )
		return;

	// Agregamos la acción
	AccionCogerDinero( v_destino );
	//n_printf("IA %d VA A RECAUDAR IMPUESTOS\n",GetId());
}

// Cambia de personaje
void nJugador::IA_CambiarPersonaje( nPersonaje::TPersonaje n_personaje )
{
	// Agregamos la acción
	AccionCambiarPersonaje( n_personaje );
	//n_printf("IA %d VA A CAMBIAR DE PERSONAJE\n",GetId());
}

// Libera al personaje
bool nJugador::IA_LiberarPersonaje()
{
	if ( partida->PuedeLiberarPersonaje( GetId() ) )
	{
		// Agregamos la acción
		AccionLiberarPersonaje();
		//n_printf("IA %d VA A LIBERAR EL PERSONAJE\n",GetId());
		return true;
	}
	else
		return false;
}

// Destruye un edificio del jugador que tenga más edificios construidos (mas trampa jijiji)
void nJugador::IA_DestruirEdificio()
{
	nPlano *plano = NULL;
	// Selecciona un edificio del jugador que tenga más edificios
	nEntity *v_destino = partida->EdificioMejorJugador( GetId() );
	if ( v_destino != NULL )
	{
		if ( v_destino->GetVisible() )
		{
			nConstruccion *construccion = (nConstruccion *) v_destino;
			nJugador *propietario = construccion->GetManager();
			for ( int i = 0; i < propietario->GetDistrito()->GetListaPlanos().Size(); i++ )
			{
				if ( propietario->GetDistrito()->GetListaPlanos().At( i )->GetIdEdificio() == construccion->GetId() )
				{
					plano = propietario->GetDistrito()->GetListaPlanos().At( i );
					break;
				}
			}
			if ( plano != NULL )
			{
				if ( GetOro() < plano->GetCoste() )
					return;

				// Agregamos la accion
				AccionDestruirEdificio( v_destino );
				//n_printf("IA %d VA A DESTRUIR EL EDIFICIO %s\n",GetId(),v_destino->GetName());
			}
		}
	}
}

// Va al ayuntamiento (para mantenerle ocupado jejeje)
void nJugador::IA_Ir_A_Ayuntamiento()
{
 	// Agregamos la accion
	AccionIrAAyuntamiento();
	//n_printf("IA %d VA A IR AL AYUNTAMIENTO\n",GetId());
}

void nJugador::Esperar( float tiempo )
{
	// Agregamos la accion
	AccionEsperar ( tiempo );
}

void nJugador::VaciarPilaAcciones()
{
	TAccion* accionActual;
	while ( !pila_acciones.IsEmpty() )
	{
		accionActual = pila_acciones.Pop();
		n_free( accionActual );
	}
}

int nJugador::GetNumPlanos()
{
	int num_planos = 0;
	for ( int i = 0; i < N_MAXPLANOS; i++ )
	{
		if ( planos.At( i ) != NULL )
			num_planos++;
	}
	return num_planos;
}

void nJugador::Recaudar()
{
	list<nPlano*>::iterator iter;
	bool encontrado = false;

	TAccion* accionActual = pila_acciones.Top();

	nAyuntamiento* ayuntamiento = (nAyuntamiento*) accionActual->destino;
	int id_ayuntamiento = ayuntamiento->GetId();
	if ( id_ayuntamiento == GetId() )
	{
		//n_printf( "ESTE AYUNTAMIENTO ES EL TUYO\n" );
		MostrarInformacion("No puedes recaudar dinero en tu ayuntamiento");
	}
	else
	{
		nPartida* partida = GetPartida();
		nJugador* propietario = NULL;
		for ( int i = 0; i < partida->GetJugadores()->Size(); i++ )
		{
			if ( partida->GetJugadores()->At( i )->GetId() == id_ayuntamiento )
			{
				propietario = partida->GetJugadores()->At( i );
				break;
			}
		}
		if ( PersonajeActivo == nPersonaje::PERSONAJE_SHUGENJA )
		{
			if ( !propietario->EstaBloqueado() || propietario->GetPersonaje()->EstaMuerto() )
				IntercambiarPlanosConJugador( propietario );
			/*else
				n_printf( "NO ES POSIBLE INTERCAMBIAR PLANOS CON JUGADOR %d POR ESTAR BLOQUEADO\n", propietario->GetId() );*/
		}
		else if ( PersonajeActivo == nPersonaje::PERSONAJE_SHOGUN )
		{
			int campesinos_contratados = personaje->GetCampesinosContratados();
			if ( campesinos_contratados > 0 )
			{
				if ( campesinos_contratados > AccionesPorTurno.Recaudaciones )
				{
					bool ayto_valido = true;
					/*for ( int i = 0; i <  N_MAXPLANOS - 1 ; i++ )
					{
						if ( aytos_recaudados.At( i ) == id_ayuntamiento )
							break;
						else if ( aytos_recaudados.At( i ) == -1 )
						{
							aytos_recaudados.Set( i, id_ayuntamiento );
							ayto_valido = true;
							break;
						}
					}*/

					for ( int i = 0; i <  aytos_recaudados.Size() ; i++ )
					{
						if ( aytos_recaudados.At( i ) == id_ayuntamiento )
						{
							ayto_valido = false;
						}
					}
					if ( ayto_valido )
					{
						aytos_recaudados.PushBack(id_ayuntamiento);
						// Se ojean todas sus construcciones
						for ( int i = 0; i < propietario->GetDistrito()->GetListaPlanos().Size(); i++ )
						{
							if ( propietario->GetDistrito()->GetEdificio(i)->GetDistintivo() == nPlano::TPlano::IMPERIAL )
							{
								SetOro( GetOro() + 10 );
								encontrado = true;
							}
						}
						if ( encontrado )
						{
							AccionesPorTurno.Recaudaciones++;
							personaje->SetAnimacion( nPersonaje::ANIM_COGER );
						}
					}
					else
					{
						//n_printf( "AYUNTAMIENTO YA RECAUDADO\n" );
						MostrarInformacion("En este ayuntamiento ya has recaudado");
					}
				}
				else
				{
					//n_printf( "LE FALTA CONTRATAR UN CAMPESINO\n" );
					MostrarInformacion("Necesitas contratar mas campesinos");
				}
			}
			else
			{
				//n_printf( "NO TIENE CONTRATADO NINGUN CAMPESINO\n" );
				MostrarInformacion("No tienes campesinos contratados");
			}
		}
	}
}

nArray<nPlano*> nJugador::GetPlanos()
{
	return planos;
}

nPartida* nJugador::GetPartida()
{
	return partida;
}

void nJugador::Puntuar( int categoria, nJugador* propietario, int valor_destruccion )
{
	Puntuacion categoria2 = (Puntuacion) categoria;

	switch ( categoria2 )
	{
		case DESPIDOS       : SetPuntos( DESPIDOS, -1 );
							  break;
		case CONTRATOS      : SetPuntos( CONTRATOS, 1 );
							  break;
		case RESURRECCIONES : SetPuntos( RESURRECCIONES, 1 );
							  break;
		case ROBOS          : SetPuntos( ROBOS, 1 );
							  break;
		case ASESINATOS     : SetPuntos( ASESINATOS, 1 );
							  break;
		case DESTRUCCIONES  : SetPuntos( DESTRUCCIONES, 1 );
							  propietario->SetPuntos( CONSTRUCCIONES, -(valor_destruccion) );
							  break;
		case DINERO         : SetPuntos( DINERO, 30 );
							  break;
		case TIEMPO         : SetPuntos( TIEMPO, 30 );
							  break;
		case DISTINTIVOS    : SetPuntos( DISTINTIVOS, 30 );
							  break;
		case CONSTRUCCIONES : SetPuntos( CONSTRUCCIONES, planos.At( plano_a_construir )->GetValor() );
						  	  break;
	}
}

void nJugador::Bloquear()
{
	this->JugadorBloqueado = true;
	if ( GetModoConstruccion() )
		SetModoConstruccion( false );

	//n_printf("Bloqueamos al jugador %d\n", GetId());
}

bool nJugador::EstaBloqueado()
{
	return this->JugadorBloqueado;
}

void nJugador::Desbloquear()
{
	if (this->personaje->EstaMuerto())
	{
		//n_printf("No se puede desbloquear al jugador %d porque está muerto\n",GetId());
		return;
	}

	this->JugadorBloqueado	= false;
	this->JugadorActivo		= false;

	//n_printf("Desbloqueamos al jugador %d\n", GetId());
}

int nJugador::PuntosPorAccion()
{
	int marcador = 0;

	for ( int i = 0; i < N_MAXTIPODEPUNTOS-1; i++ )
	{
		if ( Puntos.At( i ).categoria == ASESINATOS     ||
		     Puntos.At( i ).categoria == ROBOS          ||
		     Puntos.At( i ).categoria == CONTRATOS      ||
			 Puntos.At( i ).categoria == DESTRUCCIONES  ||
			 Puntos.At( i ).categoria == RESURRECCIONES ||
			 Puntos.At( i ).categoria == DESPIDOS )
			marcador += Puntos.At( i ).puntos;
	}
	return marcador;
}

int nJugador::PuntosPorConstruccion()
{
	int marcador = 0;

	for ( int i = 0; i < N_MAXTIPODEPUNTOS-1; i++ )
	{
		if ( Puntos.At( i ).categoria == CONSTRUCCIONES )
		{
			marcador = Puntos.At( i ).puntos;
			break;
		}
	}
	return marcador;
}

int nJugador::PuntosPorDistintivo()
{
	int marcador = 0;

	for ( int i = 0; i < N_MAXTIPODEPUNTOS-1; i++ )
	{
		if ( Puntos.At( i ).categoria == DISTINTIVOS )
		{
			marcador = Puntos.At( i ).puntos;
			break;
		}
	}
	return marcador;
}

int nJugador::PuntosPorDinero()
{
	int marcador = 0;

	for ( int i = 0; i < N_MAXTIPODEPUNTOS-1; i++ )
	{
		if ( Puntos.At( i ).categoria == DINERO )
		{
			marcador = Puntos.At( i ).puntos;
			break;
		}
	}
	return marcador;
}

int nJugador::PuntosPorTiempo()
{
	int marcador = 0;

	for ( int i = 0; i < N_MAXTIPODEPUNTOS-1; i++ )
	{
		if ( Puntos.At( i ).categoria == TIEMPO )
		{
			marcador = Puntos.At( i ).puntos;
			break;
		}
	}
	return marcador;
}

nPersonaje::TPersonaje nJugador::GetPersonajeAnterior()
{
	return this->PersonajeAnterior;
}

PlayerStateNet nJugador::getPlayerStateNet()
{
	PlayerStateNet state;
	state=AccionActual;
	return state;
}

void nJugador::setPlayerStateNet(PlayerStateNet state)
{
	if (state.accion.numAccion!=AccionActual.accion.numAccion)
	{
		TAccion* accion;
		accion = AccionVacia();
		// Generamos la acción
		accion->accion=(Acciones)state.accion.accion;
		if (state.accion.clase_destino==C_NINGUNA)
			accion->destino=NULL;
		else
		{
			char v_destino[N_MAXPATH];
			switch (state.accion.clase_destino) 
			{
			case C_CASAPLANOS:
				sprintf(v_destino,"/game/world/casaplanos");
				break;
			case C_CASAMONEDA:
				sprintf(v_destino,"/game/world/casamoneda");
				break;
			case C_AYUNTAMIENTO:
				sprintf(v_destino,"/game/world/ayto%d",state.accion.id_destino);
				break;
			case C_CONSTRUCCION:
				sprintf(v_destino,"/game/world/cons%d",state.accion.id_destino);
				break;
			case C_CAMPESINO:
				sprintf(v_destino,"/game/world/campesino%d",state.accion.id_destino);
				break;
			case C_PERSONAJE:
				sprintf(v_destino,"/game/world/jugador%d",state.accion.id_destino);
				break;
			case C_KUNOICHI:
				sprintf(v_destino,"/game/world/kunoichi");
				break;
			case C_LADRONA:
				sprintf(v_destino,"/game/world/ladrona");
				break;
			case C_SHUGENJA:
				sprintf(v_destino,"/game/world/shugenja");
				break;
			case C_SHOGUN:
				sprintf(v_destino,"/game/world/shogun");
				break;
			case C_MAESTRO:
				sprintf(v_destino,"/game/world/maestro");
				break;
			case C_MERCADER:
				sprintf(v_destino,"/game/world/mercader");
				break;
			case C_INGENIERO:
				sprintf(v_destino,"/game/world/ingeniero");
				break;
			case C_SAMURAI:
				sprintf(v_destino,"/game/world/kunoichi");
				break;
			}
			accion->destino=(nEntity *)kernelServer->Lookup(v_destino);
		}
		if (state.accion.jugador=-1)
			accion->jugador=NULL;
		else
			accion->jugador=partida->GetJugadores()->At((int)state.accion.jugador);
		accion->nombre_personaje=(nPersonaje::TPersonaje)state.accion.personaje;
		accion->tiempo=state.accion.tiempo;
		accion->posicion=vector3(state.accion.posicion_x,state.accion.posicion_y,state.accion.posicion_z);

		// Particularidades de algunas acciones
		if (accion->accion==CONSTRUIR_EDIFICIO)
		{
			nConstruccion *construccion=(nConstruccion *) accion->destino;
			construccion->SetPosition( floor( accion->posicion.x ) + 0.5f, floor( accion->posicion.z ) + 0.5f );
			this->plano_a_construir=(int)state.plano;
		}

		AgregarAccion(accion);
	}
}

// SISTEMA DE ACCIONES

void nJugador::AccionIrA(vector3 p_posicion)
{
	TAccion *accion;
	accion = AccionVacia();
	accion->accion = IR_A;
	accion->posicion = p_posicion;

	AgregarAccion( accion );
}

void nJugador::AccionCambiarPersonaje(nPersonaje::TPersonaje p_personaje)
{
	TAccion* accion;
	accion = AccionVacia();
	accion->accion = CAMBIAR_PERSONAJE;
	accion->nombre_personaje=p_personaje;

	AgregarAccion( accion );
}

void nJugador::AccionLiberarPersonaje()
{
	TAccion* accion;
	accion = AccionVacia();
	accion->accion = LIBERAR_PERSONAJE;

	AgregarAccion( accion );
}

void nJugador::AccionCogerDinero( nEntity *p_destino )
{
	TAccion* accion;
	accion = AccionVacia();
	accion->accion = COGER_DINERO;
	accion->destino = p_destino;

	AgregarAccion( accion );

	if (!this->personaje->EstaMuerto())
	{
		// FMOD: Sonido de personajes al ir a coger dinero
		SonidoPersonaje(COGER_DINERO);
	}
}

void nJugador::AccionCogerPlano( nEntity *p_destino )
{
	TAccion* accion;
	accion = AccionVacia();
	accion->accion = COGER_PLANO;
	accion->destino = p_destino;

	AgregarAccion( accion );

	if (!this->personaje->EstaMuerto())
	{
		// FMOD: Sonido de personajes al ir a coger un plano
		SonidoPersonaje(COGER_PLANO);
	}
}

void nJugador::AccionIrAAyuntamiento()
{
	char path[N_MAXPATH];
	sprintf( path, "/game/world/ayto%d", GetId() );
	TAccion* accion;
	accion = AccionVacia();
	accion->accion = IR_A_AYUNTAMIENTO;
	// Selecciona el ayuntamiento del jugador
	accion->destino = (nEntity*) kernelServer->Lookup( path );

	AgregarAccion( accion );
}

void nJugador::AccionEspecial(nEntity *p_destino)
{
	TAccion* accion;
	accion = AccionVacia();
	accion->accion = SEGUNDA;
	accion->destino = p_destino;

	AgregarAccion( accion );
}

void nJugador::AccionConstruirEdificio(nEntity *p_edificio)
{
	TAccion* accion;
	accion = AccionVacia();
	accion->accion = CONSTRUIR_EDIFICIO;
	accion->destino = p_edificio;
	accion->posicion = p_edificio->GetPosition();

	AgregarAccion( accion );

	if (!this->personaje->EstaMuerto())
	{
		// FMOD: Sonido de personajes al ir a construir edificios
		SonidoPersonaje(CONSTRUIR_EDIFICIO);
	}
}

void nJugador::AccionDestruirEdificio(nEntity *p_edificio)
{
	TAccion* accion;
	accion = AccionVacia();
	accion->accion = DESTRUIR_EDIFICIO;
	accion->destino = p_edificio;

	AgregarAccion( accion );

	if (!this->personaje->EstaMuerto())
	{
		// FMOD: Sonido de personajes al destruir edificios
		SonidoPersonaje(DESTRUIR_EDIFICIO);
	}
}

void nJugador::AccionIntercambiarPlanos(nEntity *p_destino)
{
	TAccion* accion;
	accion = AccionVacia();
	accion->accion = INTERCAMBIAR_PLANOS;
	accion->destino = p_destino;

	AgregarAccion( accion );

	if (!this->personaje->EstaMuerto())
	{
		// FMOD: Sonido de personajes al intercambiar planos
		SonidoPersonaje(INTERCAMBIAR_PLANOS);
	}
}

void nJugador::AccionRecaudar(nEntity *p_destino)
{
	TAccion* accion;
	accion = AccionVacia();
	accion->accion = RECAUDAR;
	accion->destino = p_destino;

	AgregarAccion( accion );

	if (!this->personaje->EstaMuerto())
	{
		// FMOD: Sonido de personajes al ir a recaudar
		SonidoPersonaje(RECAUDAR);
	}
}

void nJugador::AccionContratar(nEntity *p_destino)
{
	TAccion* accion;
	accion = AccionVacia();
	accion->accion = CONTRATAR;
	accion->destino = p_destino;

	AgregarAccion( accion );
}

void nJugador::AccionEsperar(float p_tiempo)
{
	TAccion* accion;
	accion = AccionVacia();
	accion->tiempo = p_tiempo;
	accion->accion = ESPERAR;

	AgregarAccion( accion );
}

void nJugador::AccionMorir()
{
	TAccion* accion;
	accion = AccionVacia();
	accion->accion = MORIR;
	accion->jugador = this;

	AgregarAccion( accion );

	personaje->Parar();
}

int nJugador::TotalCostePlanos()
{
	int cuantos = 0;
	for ( int i = 0; i < planos.Size(); i++ )
	{
		if ( planos.At( i ) != NULL && !planos.At( i )->EstaConstruido() )
			cuantos = cuantos + planos.At( i )->GetCoste();
	}
	return cuantos;
}

int nJugador::TotalValorPlanos()
{
	int cuantos = 0;
	for ( int i = 0; i < planos.Size(); i++ )
	{
		if ( planos.At( i ) != NULL && !planos.At( i )->EstaConstruido() )
			cuantos = cuantos + planos.At( i )->GetValor();
	}
	return cuantos;
}

int nJugador::TotalPlanosEspeciales()
{
	int cuantos = 0;
	for ( int i = 0; i < planos.Size(); i++ )
	{
		if ( planos.At( i ) != NULL && !planos.At( i )->EstaConstruido() )
		{
			if ( planos.At( i )->GetDistintivo() == nPlano::TPlano::ESPECIAL )
				cuantos++;
		}
	}
	return cuantos;
}

int nJugador::TotalPlanosDistintos()
{
	int cuantos   = 0;
	bool rojo     = false;
	bool verde    = false;
	bool azul     = false;
	bool amarillo = false;
	bool morado   = false;

	for ( int i = 0; i < planos.Size(); i++ )
	{
		if ( planos.At( i ) != NULL && !planos.At( i )->EstaConstruido() )
		{
			if ( !morado && planos.At( i )->GetDistintivo() == nPlano::TPlano::ESPECIAL )
			{
				morado = true;
				cuantos++;
			}
			else if ( !verde && planos.At( i )->GetDistintivo() == nPlano::TPlano::COMERCIAL )
			{
				verde = true;
				cuantos++;
			}
			else if ( !azul && planos.At( i )->GetDistintivo() == nPlano::TPlano::RELIGIOSO )
			{
				azul = true;
				cuantos++;
			}
			else if ( !rojo && planos.At( i )->GetDistintivo() == nPlano::TPlano::MILITAR )
			{
				rojo = true;
				cuantos++;
			}
			else if ( !amarillo && planos.At( i )->GetDistintivo() == nPlano::TPlano::IMPERIAL )
			{
				amarillo = true;
				cuantos++;
			}
		}
	}
	return cuantos;
}

void nJugador::SonidoPersonaje(Acciones p_accion)
{
	char buf[N_MAXPATH];
	nString v_accion_sel="";
	switch (p_accion)
	{
	case IR_A:
	case INTERCAMBIAR_PLANOS:
	case SEGUNDA:
		v_accion_sel="mover";
		break;
	case COGER_DINERO:
	case RECAUDAR:
	case COGER_PLANO:
		v_accion_sel="coger";
		break;
	case DESTRUIR_EDIFICIO:
		v_accion_sel="destruir";
		break;
	case CONSTRUIR_EDIFICIO:
		v_accion_sel="construir";
		break;
	case LIBERAR_PERSONAJE:
		v_accion_sel="liberar";
		break;
	case CAMBIAR_PERSONAJE:
		v_accion_sel="escoger";
		break;
	case MORIR:
		v_accion_sel="morir";
		break;
	}

	if (!(v_accion_sel == ""))
	{
		if ( this->GetJugadorPrincipal() || p_accion == MORIR )
		{
			nGame *juego=(nGame *) kernelServer->Lookup("/game");
			switch (this->personaje->GetTipoPersonaje())
			{
			case nPersonaje::PERSONAJE_KUNOICHI:
				sprintf(buf,"sounds:voces/asesina%s.ogg",v_accion_sel.Get());
				break;
			case nPersonaje::PERSONAJE_LADRONA:
				sprintf(buf,"sounds:voces/ladrona%s.ogg",v_accion_sel.Get());
				break;
			case nPersonaje::PERSONAJE_SHUGENJA:
				sprintf(buf,"sounds:voces/shugenja%s.ogg",v_accion_sel.Get());
				break;
			case nPersonaje::PERSONAJE_SHOGUN:
				sprintf(buf,"sounds:voces/shogun%s.ogg",v_accion_sel.Get());
				break;
			case nPersonaje::PERSONAJE_MAESTRO:
				sprintf(buf,"sounds:voces/maestro%s.ogg",v_accion_sel.Get());
				break;
			case nPersonaje::PERSONAJE_MERCADER:
				sprintf(buf,"sounds:voces/mercader%s.ogg",v_accion_sel.Get());
				break;
			case nPersonaje::PERSONAJE_INGENIERO:
				sprintf(buf,"sounds:voces/ingeniero%s.ogg",v_accion_sel.Get());
				break;
			case nPersonaje::PERSONAJE_SAMURAI:
				sprintf(buf,"sounds:voces/samurai%s.ogg",v_accion_sel.Get());
				break;
			}
			if ( p_accion == MORIR )
				juego->Play3DSound(1,buf,this->personaje->GetPosition());
			else
                juego->PlaySound(1,buf);
		}
	}
}