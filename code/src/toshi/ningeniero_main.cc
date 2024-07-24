#define N_IMPLEMENTS nIngeniero
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/ningeniero.h"
#include "toshi/ncampesino.h"
#include "toshi/njugador.h"

nNebulaScriptClass( nIngeniero, "npersonaje" );


// Constructor general
nIngeniero::nIngeniero( nJugador *jugador_ingeniero )
{
	jugador       = jugador_ingeniero;
	TipoPersonaje = PERSONAJE_INGENIERO;
}


// Constructor por defecto
nIngeniero::nIngeniero()
{
	jugador       = NULL;
	TipoPersonaje = PERSONAJE_INGENIERO;
}


// Destructor
nIngeniero::~nIngeniero()
{
}


void nIngeniero::ContratarCampesino( nCampesino* &campesino )
{
	campesino->Contratar( (nEntity *) this );
}


void nIngeniero::LiberarCampesino( nCampesino* &campesino )
{
	campesino->Liberar();
}


void nIngeniero::ResucitarCampesino( nCampesino* &campesino )
{
	//n_printf( "EL INGENIERO NO PUEDE RESUCITAR CAMPESINOS\n" );
}


void nIngeniero::DestruirEdificio( nConstruccion* &construccion )
{
	//n_printf( "EL INGENIERO NO PUEDE DESTRUIR CONSTRUCCIONES\n" );
}


int nIngeniero::Robar( nEntity* &personaje_a_robar )
{
	//n_printf( "EL INGENIERO NO PUEDE ROBAR\n"  );
	return 0;
}


void nIngeniero::Matar( nEntity* &personaje_a_matar )
{
	//n_printf( "EL INGENIERO NO PUEDE MATAR\n"  );
}