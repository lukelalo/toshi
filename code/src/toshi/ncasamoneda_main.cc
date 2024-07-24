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
/* M�todo      : SetPos														    */
/* Descripci�n : Asigna una posici�n a la casa de la moneda.					*/
/* Par�metros  : float x (ENT). Valor de la coordenada x.						*/
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
/* M�todo      : Pintar														    */
/* Descripci�n : Asigna un nodo visual, una malla de colisi�n, un tipo y clase  */
/*				 de colisi�n a la entidad creada.				    			*/
/* Par�metros  : 																*/
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
/* M�todo      : Seleccionar												    */
/* Descripci�n : Creamos un efecto de selecci�n cuando se selecciona la entidad	*/
/* Par�metros  : 																*/
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
/* M�todo      : EstaEnRango												    */
/* Descripci�n : Comprobamos si la entidad est� dentro de un rango.				*/
/* Par�metros  : vector3 posicion (ENT). Posici�n espacial a comprobar.			*/
/* Devuelve    : TRUE si la posici�n pasada est� a menor distancia del margen.	*/
/*				 FALSE si la posici�n pasada queda fuera del margen.			*/
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
/* M�todo      : Tick														    */
/* Descripci�n : Ejecuta la l�gica de la entidad durante el tiempo pasado por	*/
/*				 par�metro.														*/
/* Par�metros  : float dt (ENT). Tiempo transcurrido desde el �ltimo tick.		*/
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