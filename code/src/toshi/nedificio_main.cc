#define N_IMPLEMENTS nEdificio
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nedificio.h"
#include "toshi/nayuntamiento.h"
#include "toshi/ncasamoneda.h"
#include "toshi/ncasaplanos.h"
#include "toshi/nconstruccion.h"
#include "toshi/nmapa.h"

nNebulaScriptClass(nEdificio, "nentity");

//------------------------------------------------------------------------------
/**
*/
nEdificio::nEdificio()
{
	edificio_x=0.0f;
	edificio_y=0.0f;
	tipo_edificio=T_CONSTRUCCION;
	id_edificio=-1;
}

//------------------------------------------------------------------------------
/**
*/
nEdificio::~nEdificio()
{
}

void nEdificio::SetId(int id) 
{
	id_edificio=id;
}

void nEdificio::SetTipo(TEdificio tipo)
{
	tipo_edificio=tipo;
}

void nEdificio::SetPos(float x,float y)
{
	edificio_x=x;
	edificio_y=y;
}

void nEdificio::Pintar() 
{
}

void nEdificio::Tick(float dt)
{
	Update();
}