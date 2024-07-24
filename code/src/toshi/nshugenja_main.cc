#define N_IMPLEMENTS nShugenja
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nshugenja.h"
#include "toshi/ncampesino.h"
#include "toshi/njugador.h"

nNebulaScriptClass( nShugenja, "npersonaje" );


// Constructor general
nShugenja::nShugenja( nJugador *jugador_shugenja )
{
	jugador       = jugador_shugenja;
	TipoPersonaje = PERSONAJE_SHUGENJA;
}


// Constructor por defecto
nShugenja::nShugenja()
{
	jugador       = NULL;
	TipoPersonaje = PERSONAJE_SHUGENJA;
}


// Destructor
nShugenja::~nShugenja()
{
}


void nShugenja::ContratarCampesino( nCampesino* &campesino )
{
	campesino->Contratar( (nEntity *) this );
}


void nShugenja::LiberarCampesino( nCampesino* &campesino )
{
	campesino->Liberar();
}


void nShugenja::ResucitarCampesino( nCampesino* &campesino )
{
	if ( campesino->EstaMuerto() )
	{
		SetAnimacion( nPersonaje::ANIM_RESUCITAR_A );
		campesino->SetAnimacion( ANIM_QUIETO );
		campesino->Resucitar();
		//n_printf( "CAMPESINO %d RESUCITADO\n", campesino->GetId() );
	}
}


void nShugenja::DestruirEdificio( nConstruccion* &construccion )
{
	//n_printf( "EL SHUGENJA NO PUEDE DESTRUIR CONSTRUCCIONES\n" );
}


int nShugenja::Robar( nEntity* &personaje_a_robar )
{
	//n_printf( "EL SHUGENJA NO PUEDE ROBAR\n"  );
	return 0;
}


void nShugenja::Matar( nEntity* &personaje_a_matar )
{
	//n_printf( "EL SHUGENJA NO PUEDE MATAR\n"  );
}