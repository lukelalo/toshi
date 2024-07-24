#define N_IMPLEMENTS nLadrona
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nladrona.h"
#include "toshi/ncampesino.h"
#include "toshi/njugador.h"
#include "toshi/npartida.h"

nNebulaScriptClass( nLadrona, "npersonaje" );


// Constructor general
nLadrona::nLadrona( nJugador *jugador_ladrona )
{
	jugador       = jugador_ladrona;
	TipoPersonaje = PERSONAJE_LADRONA;
}


// Constructor por defecto
nLadrona::nLadrona()
{
	jugador       = NULL;
	TipoPersonaje = PERSONAJE_LADRONA;
}


// Destructor
nLadrona::~nLadrona()
{
}


void nLadrona::ContratarCampesino( nCampesino* &campesino )
{
	//n_printf( "LA LADRONA NO PUEDE CONTRATAR CAMPESINOS\n" );
}


void nLadrona::LiberarCampesino( nCampesino* &campesino )
{
	//n_printf( "LA LADRONA NO PUEDE LIBERAR CAMPESINOS\n" );
}


void nLadrona::ResucitarCampesino( nCampesino* &campesino )
{
	//n_printf( "LA LADRONA NO PUEDE RESUCITAR CAMPESINOS\n" );
}


void nLadrona::DestruirEdificio( nConstruccion* &construccion )
{
	//n_printf( "LA LADRONA NO PUEDE DESTRUIR CONSTRUCCIONES\n" );
}


int nLadrona::Robar( nEntity* &personaje_a_robar )
{
	if ( strcmp( personaje_a_robar->GetClass()->GetName(), "ncampesino" ) == 0 )
	{
		nCampesino *campesino = (nCampesino *) personaje_a_robar;
		int dinero = campesino->GetOro();
		campesino->Robar();
		/*if ( dinero > 0 )
			n_printf( "AL CAMPESINO %d LE HAN ROBADO %d MONEDAS\n", campesino->GetId(), dinero );
		else
			n_printf( "¡MALA SUERTE! EL CAMPESINO %d ESTABA SIN BLANCA\n", campesino->GetId() );*/
		return dinero;
	}
	else
	{
		nPersonaje *personaje_robado = (nPersonaje *) personaje_a_robar;
		if ( personaje_robado->GetJugador() != NULL )
		{
			nJugador *jugador = GetJugador();
			if ( jugador->GetPartida() != NULL )
			{
				nJugador *propietario = NULL;
				if ( personaje_robado->GetTipoPersonaje() == nPersonaje::PERSONAJE_CAMPESINO )
					propietario = personaje_robado->GetJugador();
				else
				{
					nPartida *partida = NULL;
					partida = jugador->GetPartida();
					propietario = partida->GetJugadorConPersonaje( personaje_robado->GetTipoPersonaje() );
				}

				if ( jugador->GetId() != propietario->GetId() )
				{
					if ( !propietario->EstaBloqueado() || propietario->GetPersonaje()->EstaMuerto() )
					{
						int dinero = propietario->GetOro();
						propietario->SetOro( 0 );
						//n_printf( "AL PERSONAJE %s LE HAN ROBADO %d MONEDAS\n", personaje_robado->GetName(), dinero );
						return dinero;
					}
				}
				/*else
					n_printf( "LA LADRONA NO SE PUEDE ROBAR A SI MISMA\n" );*/
			}
		}
		return 0;
	}
}


void nLadrona::Matar( nEntity* &personaje_a_matar )
{
	//n_printf( "LA LADRONA NO PUEDE MATAR\n"  );
}