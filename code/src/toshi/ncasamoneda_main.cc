#define N_IMPLEMENTS nCasaMoneda
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/ncasamoneda.h"
#include "toshi/njugador.h"

nNebulaScriptClass( nCasaMoneda, "nedificio" );

// Constructor general
nCasaMoneda::nCasaMoneda() : ruta( "" )
{
	posicion_x = 0.0f;
	posicion_y = 0.0f;
	SetTipo( T_CASAMONEDA );
	CanalSeleccion = refChannelServer->GenChannel( "sel_cm" );
	valorSeleccion = TIEMPO_SELECCION;
	refChannelServer->SetChannel1f( CanalSeleccion, valorSeleccion );
}


// Destructor
nCasaMoneda::~nCasaMoneda()
{
}


/********************************************************************************/
/* Método      : SetPos														    */
/* Descripción : Asigna una posición a la casa de la moneda.					*/
/* Parámetros  : float x (ENT). Valor de la coordenada x.						*/
/*				 float y (ENT). Valor de la coordenada y.						*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCasaMoneda::SetPos( float x, float y )
{
	posicion_x = x;
	posicion_y = y;
	SetPosition( x, y );
}


/********************************************************************************/
/* Método      : Pintar														    */
/* Descripción : Asigna un nodo visual, una malla de colisión, un tipo y clase  */
/*				 de colisión a la entidad creada.				    			*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCasaMoneda::Pintar()
{
	SetVisNode( "/game/models/casamoneda" );
	SetCollideShape( "meshes:casamoneda.n3d" );
	SetTouchMethod( "ignorarColision" );
	SetCollideClass( "edificio" );
}


/********************************************************************************/
/* Método      : Seleccionar												    */
/* Descripción : Creamos un efecto de selección cuando se selecciona la entidad	*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCasaMoneda::Seleccionar()
{
	vector3     posicion;
	nShaderNode *shader;
	char        *params = NULL;

	posicion = GetPosition();
	shader = (nShaderNode *) kernelServer->Lookup( "/game/models/casamoneda/shader" );
	valorSeleccion = 0.0f;
}


/********************************************************************************/
/* Método      : EstaEnRango												    */
/* Descripción : Comprobamos si la entidad está dentro de un rango.				*/
/* Parámetros  : vector3 posicion (ENT). Posición espacial a comprobar.			*/
/* Devuelve    : TRUE si la posición pasada está a menor distancia del margen.	*/
/*				 FALSE si la posición pasada queda fuera del margen.			*/
/********************************************************************************/
bool nCasaMoneda::EstaEnRango( vector3 p_posicion )
{
    vector3 v_distancia, v_miPosicion;

	v_miPosicion   = GetPosition();
	v_miPosicion.y = 0.0f;
	p_posicion.y   = 0.0f;
	v_distancia	   = p_posicion - v_miPosicion;
	return v_distancia.len() < DISTANCIA_A_CASAMONEDA;
}


/********************************************************************************/
/* Método      : Tick														    */
/* Descripción : Ejecuta la lógica de la entidad durante el tiempo pasado por	*/
/*				 parámetro.														*/
/* Parámetros  : float dt (ENT). Tiempo transcurrido desde el último tick.		*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCasaMoneda::Tick( float dt )
{
	Trigger( dt );
	if ( valorSeleccion < TIEMPO_SELECCION )
	{
		valorSeleccion += dt;
		refChannelServer->SetChannel1f( CanalSeleccion, valorSeleccion );
	}
	else if ( valorSeleccion > TIEMPO_SELECCION )
	{
		valorSeleccion = TIEMPO_SELECCION;
		refChannelServer->SetChannel1f( CanalSeleccion, valorSeleccion );
	}
	Update();
}