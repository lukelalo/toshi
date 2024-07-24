#define N_IMPLEMENTS nKunoichi
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nkunoichi.h"
#include "toshi/ncampesino.h"
#include "toshi/njugador.h"
#include "toshi/npartida.h"

nNebulaScriptClass( nKunoichi, "npersonaje" );


// Constructor por defecto
nKunoichi::nKunoichi()
{
	jugador       = NULL;
	TipoPersonaje = PERSONAJE_KUNOICHI;
}


// Constructor general
nKunoichi::nKunoichi( nJugador* jugador_kunoichi )
{
	jugador       = jugador_kunoichi;
	TipoPersonaje = PERSONAJE_KUNOICHI;
}


// Destructor
nKunoichi::~nKunoichi()
{
}


void nKunoichi::ContratarCampesino( nCampesino* &campesino )
{
	//n_printf( "LA KUNOICHI NO PUEDE CONTRATAR CAMPESINOS\n" );
}


void nKunoichi::LiberarCampesino( nCampesino* &campesino )
{
	//n_printf( "LA KUNOICHI NO PUEDE LIBERAR CAMPESINOS\n" );
}


void nKunoichi::ResucitarCampesino( nCampesino* &campesino )
{
	//n_printf( "LA KUNOICHI NO PUEDE RESUCITAR CAMPESINOS\n" );
}


void nKunoichi::DestruirEdificio( nConstruccion* &construccion )
{
	//n_printf( "LA KUNOICHI NO PUEDE DESTRUIR CONSTRUCCIONES\n" );
}


int nKunoichi::Robar( nEntity* &personaje_a_robar )
{
	//n_printf( "LA KUNOICHI NO PUEDE ROBAR\n"  );
	return 0;
}


void nKunoichi::Matar( nEntity* &personaje_a_matar )
{
	nPersonaje *personaje =	(nPersonaje *) personaje_a_matar;
	personaje->GetVelocityVector().set( 0, 0, 0 );
	if ( strcmp( personaje_a_matar->GetClass()->GetName(), "ncampesino" ) == 0 )
	{
		nCampesino *campesino = (nCampesino *) personaje_a_matar;
		if ( !campesino->EstaMuerto() )
			campesino->Matar();
	}
	else
	{
		if ( personaje->GetTipoPersonaje() == nPersonaje::TPersonaje::PERSONAJE_CAMPESINO )
		{
			if ( personaje->GetJugador() != NULL )
			{
				nJugador *propietario = personaje->GetJugador();
				/*if ( !propietario->EstaBloqueado() )
					n_printf( "LA KUNOICHI MATA AL JUGADOR %d CONVERTIDO EN CAMPESINO\n", propietario->GetId() );*/
			}
		}
		else
		{
			if ( personaje->GetJugador() != NULL )
			{
				nJugador *jugador = GetJugador();
				nPartida *partida = jugador->GetPartida();
				nJugador *propietario = partida->GetJugadorConPersonaje( personaje->GetTipoPersonaje() );
				/*if ( jugador->GetId() != propietario->GetId() && !propietario->EstaBloqueado() )
					n_printf( "LA KUNOICHI MATA AL PERSONAJE %s PROPIEDAD DEL JUGADOR %d\n", personaje->GetName(), propietario->GetId() );
				else
					n_printf( "LA KUNOICHI NO PUEDE SUICIDARSE NI SE PUEDE MATAR A UN PERSONAJE BLOQUEADO\n" );*/
			}
		}
	}
}