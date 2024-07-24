#define N_IMPLEMENTS nCampesino
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/ncampesino.h"
#include "toshi/maquinaestados.h"
#include "toshi/nmapa.h"

nNebulaScriptClass( nCampesino, "npersonaje" );

// Identificador único de campesino
static int        NUM_CAMPESINOS = 0;

// Cantidad de monedas de oro que cobrará un campesino cada vez que sea contratado
static int const  PRECIO         = 2;

// Cantidad de monedas de oro que cobrará un campesino cada periodo de pago durante su contratación
static int const  SUELDO         = 1;


// Constructor general
nCampesino::nCampesino()
{
	srand( (unsigned int) time( NULL ) );
    Id             = NUM_CAMPESINOS;
    // Comenzarán con una cantidad de dinero variable
	Oro            = ( rand() % 3 ) + 1;
    Contratado     = false;
    Muerto         = false;
	manager        = NULL;
	reposo         = 0.0f;
	reposoActual   = 0.0f;
	Precio         = PRECIO;
	Sueldo         = SUELDO;
	num_colisiones = 0;
	maqEstados.SetEstado( maqEstados.PARAR );
	NUM_CAMPESINOS++;
	TipoPersonaje  = PERSONAJE_CAMPESINO;
}


// Destructor
nCampesino::~nCampesino()
{
}


/********************************************************************************/
/* Método      : SetId														    */
/* Descripción : Asigna un identificador al campesino.							*/
/* Parámetros  : int id_campesino (ENT). Valor del identificador.				*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::SetId( int id_campesino )
{
    Id = id_campesino;
}


/********************************************************************************/
/* Método      : SetOro														    */
/* Descripción : Asigna una cantidad de monedas de oro al campesino.			*/
/* Parámetros  : int oro_campesino (ENT). Cantidad de monedas de oro.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::SetOro( int oro_campesino )
{
    Oro = oro_campesino;
}


/********************************************************************************/
/* Método      : EstaContratado												    */
/* Descripción : Comprueba si el campesino está contratado.						*/
/* Parámetros  : 																*/
/* Devuelve    : TRUE si el campesino está contratado.							*/
/* 				 FALSE si no está contratado.									*/
/********************************************************************************/
bool nCampesino::EstaContratado()
{
    return Contratado;
}


/********************************************************************************/
/* Método      : Contratar													    */
/* Descripción : Modifica los atributos involucrados en la contratación.		*/
/* Parámetros  : nEntity *personaje	(ENT). Personaje que contrató al campesino. */
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::Contratar( nEntity *personaje )
{
    Contratado  = true;
	manager	    = personaje;
	Oro        += Precio;
}


/********************************************************************************/
/* Método      : Liberar													    */
/* Descripción : Modifica los atributos involucrados en la liberación.			*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::Liberar()
{
	Contratado = false;
	manager	   = NULL;
}


/********************************************************************************/
/* Método      : EstaMuerto													    */
/* Descripción : Comprueba si el campesino está muerto.							*/
/* Parámetros  : 																*/
/* Devuelve    : TRUE si el campesino está muerto.								*/
/*				 FALSE si no lo está.											*/
/********************************************************************************/
bool nCampesino::EstaMuerto()
{
	return Muerto;
}


/********************************************************************************/
/* Método      : Robar													    	*/
/* Descripción : Modifica los atributos involucrados en el robo.				*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::Robar()
{
    Oro = 0;
}


/********************************************************************************/
/* Método      : Matar													    	*/
/* Descripción : Modifica los atributos involucrados en la muerte. Se asigna la	*/
/*				 animación de morir a la entidad.								*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::Matar()
{
	Muerto = true;
	SetAnimacion( ANIM_MORIR );
}


/********************************************************************************/
/* Método      : Resucitar													    */
/* Descripción : Modifica los atributos involucrados en la resurrección. Se		*/
/*				 asigna la animación de resucitar a la entidad.					*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::Resucitar()
{
	Muerto = false;
	SetAnimacion( ANIM_RESUCITAR );
}


/********************************************************************************/
/* Método      : FinDeTrayecto												    */
/* Descripción : Nos servirá como comprobación de cambio de estado. Cuando el 	*/
/*				 campesino se para, pasará a otro estado.						*/
/* Parámetros  : 																*/
/* Devuelve    : TRUE si el campesino está parado.								*/
/*               FALSE si no lo está.											*/
/********************************************************************************/
bool nCampesino::FinDeTrayecto()
{
	return ( GetAnimacion() == ANIM_QUIETO );
}


/********************************************************************************/
/* Método      : FinDeEspera												    */
/* Descripción : Nos servirá como comprobación de cambio de estado. Cuando el 	*/
/*				 campesino deja de esperar, pasará a otro estado.				*/
/* Parámetros  : 																*/
/* Devuelve    : TRUE si el campesino ha estado parado como mínimo el tiempo    */
/*				 que tenía asignado.											*/
/*               FALSE si todavía no ha llegado al mínimo de reposo.			*/
/********************************************************************************/
bool nCampesino::FinDeEspera()
{
	return ( reposoActual >= reposo );
}


/********************************************************************************/
/* Método      : actualizarMaquinaEstados									    */
/* Descripción : Actualizará el estado del campesino según la función de 		*/
/*				 transición se haya realizado o no.  							*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::actualizarMaquinaEstados()
{
	vector3 posicion;
	int     posx, posz;

	// Si al campesino lo contratan, seguirá a su manager
	if ( EstaContratado() && maqEstados.siguienteEstado( maqEstados.CONTRATADO ) )
	{
		maqEstados.SetEstado( maqEstados.SEGUIR );
		SeguirDeLejos( true, manager );
		return;
	}

	// Si al campesino lo matan
	if ( EstaMuerto() && maqEstados.siguienteEstado( maqEstados.MUERTO ) )
    {
		maqEstados.SetEstado( nMaquinaEstados::MORIR );
		num_colisiones = 0;
		return;
	}

	// Si al campesino lo liberan, se le dará una posición válida dentro del mapa a donde ir
	if ( !EstaContratado() && maqEstados.siguienteEstado( maqEstados.LIBERADO ) )
	{
		maqEstados.SetEstado( maqEstados.DEAMBULAR );
		num_colisiones = 0;
		nMapa *mapa = (nMapa *) kernelServer->Lookup( "/data/mapa" );
		vector3 pos = GetPosition();
		do
		{
			posx = ( rand() % (ANCHO_MAPA / 2) ) + (int) ( (ANCHO_MAPA) * 2 / 8 );
			posz = ( rand() % (ALTO_MAPA / 2) ) + (int) ( (ALTO_MAPA) * 2 / 8 ) ;
		}
		while ( !mapa->Movimiento_Valido( 0, 0, (int) posx , (int) posz ) );
		IrARapido( true, posx, posz );
		return;
	}

	// Si el campesino ha dejado de reposar, se le dará una posición válida dentro del mapa a donde ir
	if ( FinDeEspera() && maqEstados.siguienteEstado( maqEstados.FIN_ESPERA ) )
	{
		maqEstados.SetEstado( maqEstados.DEAMBULAR );
		num_colisiones = 0;
		nMapa *mapa = (nMapa *) kernelServer->Lookup( "/data/mapa" );
		vector3 pos = GetPosition();
		do
		{
			posx = ( rand() % (ANCHO_MAPA / 2) ) + (int) ( (ANCHO_MAPA) * 2 / 8 );
			posz = ( rand() % (ALTO_MAPA / 2) ) + (int) ( (ALTO_MAPA) * 2 / 8 ) ;
		}
		while ( !mapa->Movimiento_Valido( 0, 0, (int) posx , (int) posz ) ) ;
		IrARapido( true, posx, posz );
		return;
	}

	// Si al campesino lo resucitan, se le deja quieto un tiempo aleatorio
	if ( !EstaMuerto() && maqEstados.siguienteEstado( maqEstados.RESUCITADO ) )
	{
		maqEstados.SetEstado( maqEstados.PARAR );
		num_colisiones = 0;
		reposo = ( rand() % 10 ) + 1;
		reposoActual = 0.0f;
		return;
	}

	// Si el campesino ha llegado al destino dado, se le daja quieto un tiempo aleatorio
	if ( FinDeTrayecto() && maqEstados.siguienteEstado( maqEstados.FIN_TRAYECTO ) )
	{
		maqEstados.SetEstado( maqEstados.PARAR );
		num_colisiones = 0;
		reposo = ( rand() % 10 ) + 1;
		reposoActual = 0.0f;
		return;
	}
}


/********************************************************************************/
/* Método      : ejecutarComportamiento									    	*/
/* Descripción : Ejecutará la acción correspondiente al estado en el que se		*/
/*				 encuentre el campesino.  										*/
/* Parámetros  : float dt (ENT). Tiempo transcurrido desde el último tick.		*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::ejecutarComportamiento( float dt )
{
	switch ( maqEstados.GetEstado() )
	{
		// Aumenta el tiempo de reposo
		case maqEstados.PARAR	  : reposoActual += dt;
									break;
		// No hace nada
		case maqEstados.MORIR	  : break;
		// Sigue al manager
		case maqEstados.SEGUIR    : SeguirDeLejos( false, manager );
									break;
		// Se dirige al punto del mapa prefijado como destino
		case maqEstados.DEAMBULAR : IrARapido( false, destinoVector.x, destinoVector.z );
									break;
	}
}


/********************************************************************************/
/* Método      : UpdateEstado											    	*/
/* Descripción : Engloba acciones de la actualización de estado del campesino.  */
/* Parámetros  : float dt (ENT). Tiempo transcurrido desde el último tick.		*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::UpdateEstado( float dt )
{
	actualizarMaquinaEstados();
	ejecutarComportamiento( dt );
}


/********************************************************************************/
/* Método      : Tick									    					*/
/* Descripción : Actualiza la lógica del campesino.					 			*/
/* Parámetros  : float dt (ENT). Tiempo transcurrido desde el último tick.		*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::Tick( float dt )
{
	Trigger( dt );
	Integrate( dt );
	UpdateEstado( dt );
	ActualizarAnimaciones();
    Update();
}


/********************************************************************************/
/* Método      : GetId													    	*/
/* Descripción : Obtiene el identificador del campesino.						*/
/* Parámetros  : 																*/
/* Devuelve    : El identificador del campesino.								*/
/********************************************************************************/
const int nCampesino::GetId() const
{
	return Id;
}


/********************************************************************************/
/* Método      : GetOro													    	*/
/* Descripción : Obtiene la cantidad de monedas de oro que tiene el campesino.	*/
/* Parámetros  : 																*/
/* Devuelve    : La cantidad de monedas de oro del campesino.					*/
/********************************************************************************/
int nCampesino::GetOro()
{
	return Oro;
}


/********************************************************************************/
/* Método      : GetManager												    	*/
/* Descripción : Obtiene la entidad que ha contratado al campesino.				*/
/* Parámetros  : 																*/
/* Devuelve    : La entidad que ha contratado al campesino.						*/
/********************************************************************************/
nEntity *nCampesino::GetManager()
{
	return manager;
}


/********************************************************************************/
/* Método      : GetPrecio												    	*/
/* Descripción : Obtiene la cantidad de monedas de oro que cobrará el campesino	*/
/*				 por ser contratado.											*/
/* Parámetros  : 																*/
/* Devuelve    : El precio de la contratación del campesino.					*/
/********************************************************************************/
int nCampesino::GetPrecio()
{
	return Precio;
}


/********************************************************************************/
/* Método      : GetSueldo												    	*/
/* Descripción : Obtiene la cantidad de monedas de oro que cobrará el campesino	*/
/*				 cada momento de pago durante su contratación.					*/
/* Parámetros  : 																*/
/* Devuelve    : El sueldo del campesino cada turno de pago.					*/
/********************************************************************************/
int nCampesino::GetSueldo()
{
	return Sueldo;
}


/********************************************************************************/
/* Método      : Colisionar												    	*/
/* Descripción : Contabiliza el número de colisiones del campesino con otras	*/
/*				 entidades campesino o personaje. Hicimos esto porque, a veces, */
/*				 dos campesinos se encontraban y colisionaban desviando sus     */
/* 				 destinos hasta salirse del mapa, ya que en el estado deambular */
/*				 sólo se le dice a donde ir pero sin colisiones.				*/
/*				 Como solución, pusimos un tope de colisiones a partir del cuál */
/*				 se le paraba a la entidad que hubiera sobrepasado ese límite.  */
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::Colisionar()
{
	num_colisiones++;
}


/********************************************************************************/
/* Método      : GetNumColisiones										    	*/
/* Descripción : Obtiene el número de colisiones acumuladas del campesino.		*/
/* Parámetros  : 																*/
/* Devuelve    : El número de colisiones del campesino.							*/
/********************************************************************************/
int nCampesino::GetNumColisiones()
{
	return num_colisiones;
}