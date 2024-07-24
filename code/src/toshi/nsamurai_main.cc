#define N_IMPLEMENTS nSamurai
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nsamurai.h"
#include "toshi/ncampesino.h"
#include "toshi/njugador.h"
#include "toshi/nconstruccion.h"

nNebulaScriptClass( nSamurai, "npersonaje" );


// Constructor general
nSamurai::nSamurai( nJugador *jugador_samurai )
{
	jugador       = jugador_samurai;
	TipoPersonaje = PERSONAJE_SAMURAI;
}


// Constructor por defecto
nSamurai::nSamurai()
{
	jugador       = NULL;
	TipoPersonaje = PERSONAJE_SAMURAI;
}


// Destructor
nSamurai::~nSamurai()
{
}


void nSamurai::ContratarCampesino( nCampesino* &campesino )
{
	campesino->Contratar( (nEntity *) this );
}


void nSamurai::LiberarCampesino( nCampesino* &campesino )
{
	campesino->Liberar();
}


void nSamurai::ResucitarCampesino( nCampesino* &campesino )
{
	//n_printf( "EL SAMURAI NO PUEDE RESUCITAR CAMPESINOS\n" );
}


void nSamurai::DestruirEdificio( nConstruccion* &construccion )
{
	construccion->Destruir();
	//n_printf( "%s DESTRUIDO\n", construccion->GetName() );
}


int nSamurai::Robar( nEntity* &personaje_a_robar )
{
	//n_printf( "EL SAMURAI NO PUEDE ROBAR\n"  );
	return 0;
}


void nSamurai::Matar( nEntity* &personaje_a_matar )
{
	//n_printf( "EL SAMURAI NO PUEDE MATAR\n"  );
}