#define N_IMPLEMENTS nMercader
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nmercader.h"
#include "toshi/ncampesino.h"
#include "toshi/njugador.h"

nNebulaScriptClass( nMercader, "npersonaje" );


// Constructor general
nMercader::nMercader( nJugador *jugador_mercader )
{
	jugador       = jugador_mercader;
	TipoPersonaje = PERSONAJE_MERCADER;
}


// Constructor por defecto
nMercader::nMercader()
{
	jugador       = NULL;
	TipoPersonaje = PERSONAJE_MERCADER;
}


// Destructor
nMercader::~nMercader()
{
}


void nMercader::ContratarCampesino( nCampesino* &campesino )
{
	campesino->Contratar( (nEntity *) this );
}


void nMercader::LiberarCampesino( nCampesino* &campesino )
{
	campesino->Liberar();
}


void nMercader::ResucitarCampesino( nCampesino* &campesino )
{
	//n_printf( "EL MERCADER NO PUEDE RESUCITAR CAMPESINOS\n" );
}


void nMercader::DestruirEdificio( nConstruccion* &construccion )
{
	//n_printf( "EL MERCADER NO PUEDE DESTRUIR CONSTRUCCIONES\n" );
}


int nMercader::Robar( nEntity* &personaje_a_robar )
{
	//n_printf( "EL MERCADER NO PUEDE ROBAR\n"  );
	return 0;
}


void nMercader::Matar( nEntity* &personaje_a_matar )
{
	//n_printf( "EL MERCADER NO PUEDE MATAR\n"  );
}