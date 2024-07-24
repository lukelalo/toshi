#define N_IMPLEMENTS nMaestro
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nmaestro.h"
#include "toshi/ncampesino.h"
#include "toshi/njugador.h"

nNebulaScriptClass( nMaestro, "npersonaje" );

// Constructor general
nMaestro::nMaestro( nJugador *jugador_maestro )
{
	jugador       = jugador_maestro;
	TipoPersonaje = PERSONAJE_MAESTRO;
}


// Constructor por defecto
nMaestro::nMaestro()
{
	jugador       = NULL;
	TipoPersonaje = PERSONAJE_MAESTRO;
}


// Destructor
nMaestro::~nMaestro()
{
}


void nMaestro::ContratarCampesino( nCampesino* &campesino )
{
	campesino->Contratar( (nEntity *) this );
}


void nMaestro::LiberarCampesino( nCampesino* &campesino )
{
	campesino->Liberar();
}


void nMaestro::ResucitarCampesino( nCampesino* &campesino )
{
	//n_printf( "EL MAESTRO NO PUEDE RESUCITAR CAMPESINOS\n" );
}


void nMaestro::DestruirEdificio( nConstruccion* &construccion )
{
	//n_printf( "EL MAESTRO NO PUEDE DESTRUIR CONSTRUCCIONES\n" );
}


int nMaestro::Robar( nEntity* &personaje_a_robar )
{
	//n_printf( "EL MAESTRO NO PUEDE ROBAR\n"  );
	return 0;
}


void nMaestro::Matar( nEntity* &personaje_a_matar )
{
	//n_printf( "EL MAESTRO NO PUEDE MATAR\n"  );
}