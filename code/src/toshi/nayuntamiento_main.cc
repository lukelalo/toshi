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
/* M�todo      : SetId														    */
/* Descripci�n : Asigna un identificador al ayuntamiento.				    	*/
/* Par�metros  : int id (ENT). Valor del identificador del ayuntamiento.		*/
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
/* M�todo      : SetPos														    */
/* Descripci�n : Asigna una posici�n al ayuntamiento.				    		*/
/* Par�metros  : float x (ENT). Valor de la coordenada x.			    		*/
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
/* M�todo      : Pintar														    */
/* Descripci�n : Asigna un nodo visual, una malla de colisi�n, un tipo y clase  */
/*				 de colisi�n a la entidad creada.				    			*/
/* Par�metros  : 																*/
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
/* M�todo      : Seleccionar												    */
/* Descripci�n : Creamos un efecto de selecci�n cuando se selecciona la entidad	*/
/* Par�metros  : 																*/
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
/* M�todo      : EstaEnRango												    */
/* Descripci�n : Comprobamos si la entidad est� dentro de un rango.				*/
/* Par�metros  : vector3 posicion (ENT). Posici�n espacial a comprobar.			*/
/* Devuelve    : TRUE si la posici�n pasada est� a menor distancia del margen.	*/
/*				 FALSE si la posici�n pasada queda fuera del margen.			*/
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
/* M�todo      : Tick														    */
/* Descripci�n : Ejecuta la l�gica de la entidad durante el tiempo pasado por	*/
/*				 par�metro.														*/
/* Par�metros  : float dt (ENT). Tiempo transcurrido desde el �ltimo tick.		*/
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
/* M�todo      : GetId														    */
/* Descripci�n : Obtiene el identificador del ayuntamiento.						*/
/* Par�metros  : 																*/
/* Devuelve    : El valor del identificador del ayuntamiento.					*/
/********************************************************************************/
int nAyuntamiento::GetId()
{
	return id_ayuntamiento;
}