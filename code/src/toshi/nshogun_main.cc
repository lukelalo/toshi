#define N_IMPLEMENTS nShogun
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nshogun.h"
#include "toshi/ncampesino.h"
#include "toshi/njugador.h"

nNebulaScriptClass( nShogun, "npersonaje" );


// Constructor general
nShogun::nShogun( nJugador *jugador_shogun )
{
	jugador       = jugador_shogun;
	TipoPersonaje = PERSONAJE_SHOGUN;
}


// Constructor por defecto
nShogun::nShogun()
{
	jugador       = NULL;
	TipoPersonaje = PERSONAJE_SHOGUN;
}


// Destructor
nShogun::~nShogun()
{
}


nJugador *nShogun::GetJugador()
{
	return jugador;
}

void nShogun::ContratarCampesino( nCampesino* &campesino )
{
	campesino->Contratar( (nEntity *) this );
}


void nShogun::LiberarCampesino( nCampesino* &campesino )
{
	campesino->Liberar();
}


void nShogun::ResucitarCampesino( nCampesino* &campesino )
{
	//n_printf( "EL SHOGUN NO PUEDE RESUCITAR CAMPESINOS\n" );
}


void nShogun::DestruirEdificio( nConstruccion* &construccion )
{
	//n_printf( "EL SHOGUN NO PUEDE DESTRUIR CONSTRUCCIONES\n" );
}


int nShogun::Robar( nEntity* &personaje_a_robar )
{
	//n_printf( "EL SHOGUN NO PUEDE ROBAR\n"  );
	return 0;
}


void nShogun::Matar( nEntity* &personaje_a_matar )
{
	//n_printf( "EL SHOGUN NO PUEDE MATAR\n"  );
}