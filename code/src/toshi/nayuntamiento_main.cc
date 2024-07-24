#define N_IMPLEMENTS nAyuntamiento
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nayuntamiento.h"

nNebulaScriptClass( nAyuntamiento, "nedificio" );


// Constructor general
nAyuntamiento::nAyuntamiento() : ruta( "" )
{
	posicion_x = 0.0f;
	posicion_y = 0.0f;
	SetTipo( T_AYUNTAMIENTO );
}


// Destructor
nAyuntamiento::~nAyuntamiento()
{
}


/********************************************************************************/
/* Método      : SetId														    */
/* Descripción : Asigna un identificador al ayuntamiento.				    	*/
/* Parámetros  : int id (ENT). Valor del identificador del ayuntamiento.		*/
/* Devuelve    : 																*/
/********************************************************************************/
void nAyuntamiento::SetId( int id )
{
	char buf[N_MAXPATH];

	id_ayuntamiento = id;
	sprintf( buf, "sel_ay%d", id_ayuntamiento );
	CanalSeleccion = refChannelServer->GenChannel( buf );
	valorSeleccion = TIEMPO_SELECCION;
	refChannelServer->SetChannel1f( CanalSeleccion, valorSeleccion );
}


/********************************************************************************/
/* Método      : SetPos														    */
/* Descripción : Asigna una posición al ayuntamiento.				    		*/
/* Parámetros  : float x (ENT). Valor de la coordenada x.			    		*/
/*				 float y (ENT). Valor de la coordenada y.						*/
/* Devuelve    : 																*/
/********************************************************************************/
void nAyuntamiento::SetPos( float x, float y )
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
void nAyuntamiento::Pintar()
{
	char buf[N_MAXPATH];

	sprintf( buf, "/game/models/ayuntamiento%d", id_ayuntamiento );
	SetVisNode( buf );
	SetCollideShape( "meshes:ayuntamiento.n3d" );
	SetTouchMethod( "ignorarColision" );
	SetCollideClass( "edificio" );
}


/********************************************************************************/
/* Método      : Seleccionar												    */
/* Descripción : Creamos un efecto de selección cuando se selecciona la entidad	*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nAyuntamiento::Seleccionar()
{
	vector3     posicion;
	nShaderNode *shader;
	char        *params = NULL;
	char        buf[N_MAXPATH];

	posicion = GetPosition();
	sprintf( buf, "/game/models/ayuntamiento%d/shader", id_ayuntamiento );
	shader = (nShaderNode *) kernelServer->Lookup( buf );
	valorSeleccion = 0.0f;
}


/********************************************************************************/
/* Método      : EstaEnRango												    */
/* Descripción : Comprobamos si la entidad está dentro de un rango.				*/
/* Parámetros  : vector3 posicion (ENT). Posición espacial a comprobar.			*/
/* Devuelve    : TRUE si la posición pasada está a menor distancia del margen.	*/
/*				 FALSE si la posición pasada queda fuera del margen.			*/
/********************************************************************************/
bool nAyuntamiento::EstaEnRango( vector3 posicion )
{
    vector3 distancia, miPosicion;

	miPosicion	 = GetPosition();
	miPosicion.y = 0.0f;
	posicion.y	 = 0.0f;
	distancia	 = posicion - miPosicion;
	return distancia.len() < DISTANCIA_AL_AYUNTAMIENTO;
}


/********************************************************************************/
/* Método      : Tick														    */
/* Descripción : Ejecuta la lógica de la entidad durante el tiempo pasado por	*/
/*				 parámetro.														*/
/* Parámetros  : float dt (ENT). Tiempo transcurrido desde el último tick.		*/
/* Devuelve    : 																*/
/********************************************************************************/
void nAyuntamiento::Tick( float dt )
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


/********************************************************************************/
/* Método      : GetId														    */
/* Descripción : Obtiene el identificador del ayuntamiento.						*/
/* Parámetros  : 																*/
/* Devuelve    : El valor del identificador del ayuntamiento.					*/
/********************************************************************************/
int nAyuntamiento::GetId()
{
	return id_ayuntamiento;
}