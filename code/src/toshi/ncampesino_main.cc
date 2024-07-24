#define N_IMPLEMENTS nCampesino
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/ncampesino.h"
#include "toshi/maquinaestados.h"
#include "toshi/nmapa.h"

nNebulaScriptClass( nCampesino, "npersonaje" );

// Identificador �nico de campesino
static int        NUM_CAMPESINOS = 0;

// Cantidad de monedas de oro que cobrar� un campesino cada vez que sea contratado
static int const  PRECIO         = 2;

// Cantidad de monedas de oro que cobrar� un campesino cada periodo de pago durante su contrataci�n
static int const  SUELDO         = 1;


// Constructor general
nCampesino::nCampesino()
{
	srand( (unsigned int) time( NULL ) );
    Id             = NUM_CAMPESINOS;
    // Comenzar�n con una cantidad de dinero variable
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
/* M�todo      : SetId														    */
/* Descripci�n : Asigna un identificador al campesino.							*/
/* Par�metros  : int id_campesino (ENT). Valor del identificador.				*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::SetId( int id_campesino )
{
    Id = id_campesino;
}


/********************************************************************************/
/* M�todo      : SetOro														    */
/* Descripci�n : Asigna una cantidad de monedas de oro al campesino.			*/
/* Par�metros  : int oro_campesino (ENT). Cantidad de monedas de oro.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::SetOro( int oro_campesino )
{
    Oro = oro_campesino;
}


/********************************************************************************/
/* M�todo      : EstaContratado												    */
/* Descripci�n : Comprueba si el campesino est� contratado.						*/
/* Par�metros  : 																*/
/* Devuelve    : TRUE si el campesino est� contratado.							*/
/* 				 FALSE si no est� contratado.									*/
/********************************************************************************/
bool nCampesino::EstaContratado()
{
    return Contratado;
}


/********************************************************************************/
/* M�todo      : Contratar													    */
/* Descripci�n : Modifica los atributos involucrados en la contrataci�n.		*/
/* Par�metros  : nEntity *personaje	(ENT). Personaje que contrat� al campesino. */
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::Contratar( nEntity *personaje )
{
    Contratado  = true;
	manager	    = personaje;
	Oro        += Precio;
}


/********************************************************************************/
/* M�todo      : Liberar													    */
/* Descripci�n : Modifica los atributos involucrados en la liberaci�n.			*/
/* Par�metros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::Liberar()
{
	Contratado = false;
	manager	   = NULL;
}


/********************************************************************************/
/* M�todo      : EstaMuerto													    */
/* Descripci�n : Comprueba si el campesino est� muerto.							*/
/* Par�metros  : 																*/
/* Devuelve    : TRUE si el campesino est� muerto.								*/
/*				 FALSE si no lo est�.											*/
/********************************************************************************/
bool nCampesino::EstaMuerto()
{
	return Muerto;
}


/********************************************************************************/
/* M�todo      : Robar													    	*/
/* Descripci�n : Modifica los atributos involucrados en el robo.				*/
/* Par�metros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::Robar()
{
    Oro = 0;
}


/********************************************************************************/
/* M�todo      : Matar													    	*/
/* Descripci�n : Modifica los atributos involucrados en la muerte. Se asigna la	*/
/*				 animaci�n de morir a la entidad.								*/
/* Par�metros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::Matar()
{
	Muerto = true;
	SetAnimacion( ANIM_MORIR );
}


/********************************************************************************/
/* M�todo      : Resucitar													    */
/* Descripci�n : Modifica los atributos involucrados en la resurrecci�n. Se		*/
/*				 asigna la animaci�n de resucitar a la entidad.					*/
/* Par�metros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::Resucitar()
{
	Muerto = false;
	SetAnimacion( ANIM_RESUCITAR );
}


/********************************************************************************/
/* M�todo      : FinDeTrayecto												    */
/* Descripci�n : Nos servir� como comprobaci�n de cambio de estado. Cuando el 	*/
/*				 campesino se para, pasar� a otro estado.						*/
/* Par�metros  : 																*/
/* Devuelve    : TRUE si el campesino est� parado.								*/
/*               FALSE si no lo est�.											*/
/********************************************************************************/
bool nCampesino::FinDeTrayecto()
{
	return ( GetAnimacion() == ANIM_QUIETO );
}


/********************************************************************************/
/* M�todo      : FinDeEspera												    */
/* Descripci�n : Nos servir� como comprobaci�n de cambio de estado. Cuando el 	*/
/*				 campesino deja de esperar, pasar� a otro estado.				*/
/* Par�metros  : 																*/
/* Devuelve    : TRUE si el campesino ha estado parado como m�nimo el tiempo    */
/*				 que ten�a asignado.											*/
/*               FALSE si todav�a no ha llegado al m�nimo de reposo.			*/
/********************************************************************************/
bool nCampesino::FinDeEspera()
{
	return ( reposoActual >= reposo );
}


/********************************************************************************/
/* M�todo      : actualizarMaquinaEstados									    */
/* Descripci�n : Actualizar� el estado del campesino seg�n la funci�n de 		*/
/*				 transici�n se haya realizado o no.  							*/
/* Par�metros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::actualizarMaquinaEstados()
{
	vector3 posicion;
	int     posx, posz;

	// Si al campesino lo contratan, seguir� a su manager
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

	// Si al campesino lo liberan, se le dar� una posici�n v�lida dentro del mapa a donde ir
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

	// Si el campesino ha dejado de reposar, se le dar� una posici�n v�lida dentro del mapa a donde ir
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
/* M�todo      : ejecutarComportamiento									    	*/
/* Descripci�n : Ejecutar� la acci�n correspondiente al estado en el que se		*/
/*				 encuentre el campesino.  										*/
/* Par�metros  : float dt (ENT). Tiempo transcurrido desde el �ltimo tick.		*/
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
/* M�todo      : UpdateEstado											    	*/
/* Descripci�n : Engloba acciones de la actualizaci�n de estado del campesino.  */
/* Par�metros  : float dt (ENT). Tiempo transcurrido desde el �ltimo tick.		*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::UpdateEstado( float dt )
{
	actualizarMaquinaEstados();
	ejecutarComportamiento( dt );
}


/********************************************************************************/
/* M�todo      : Tick									    					*/
/* Descripci�n : Actualiza la l�gica del campesino.					 			*/
/* Par�metros  : float dt (ENT). Tiempo transcurrido desde el �ltimo tick.		*/
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
/* M�todo      : GetId													    	*/
/* Descripci�n : Obtiene el identificador del campesino.						*/
/* Par�metros  : 																*/
/* Devuelve    : El identificador del campesino.								*/
/********************************************************************************/
const int nCampesino::GetId() const
{
	return Id;
}


/********************************************************************************/
/* M�todo      : GetOro													    	*/
/* Descripci�n : Obtiene la cantidad de monedas de oro que tiene el campesino.	*/
/* Par�metros  : 																*/
/* Devuelve    : La cantidad de monedas de oro del campesino.					*/
/********************************************************************************/
int nCampesino::GetOro()
{
	return Oro;
}


/********************************************************************************/
/* M�todo      : GetManager												    	*/
/* Descripci�n : Obtiene la entidad que ha contratado al campesino.				*/
/* Par�metros  : 																*/
/* Devuelve    : La entidad que ha contratado al campesino.						*/
/********************************************************************************/
nEntity *nCampesino::GetManager()
{
	return manager;
}


/********************************************************************************/
/* M�todo      : GetPrecio												    	*/
/* Descripci�n : Obtiene la cantidad de monedas de oro que cobrar� el campesino	*/
/*				 por ser contratado.											*/
/* Par�metros  : 																*/
/* Devuelve    : El precio de la contrataci�n del campesino.					*/
/********************************************************************************/
int nCampesino::GetPrecio()
{
	return Precio;
}


/********************************************************************************/
/* M�todo      : GetSueldo												    	*/
/* Descripci�n : Obtiene la cantidad de monedas de oro que cobrar� el campesino	*/
/*				 cada momento de pago durante su contrataci�n.					*/
/* Par�metros  : 																*/
/* Devuelve    : El sueldo del campesino cada turno de pago.					*/
/********************************************************************************/
int nCampesino::GetSueldo()
{
	return Sueldo;
}


/********************************************************************************/
/* M�todo      : Colisionar												    	*/
/* Descripci�n : Contabiliza el n�mero de colisiones del campesino con otras	*/
/*				 entidades campesino o personaje. Hicimos esto porque, a veces, */
/*				 dos campesinos se encontraban y colisionaban desviando sus     */
/* 				 destinos hasta salirse del mapa, ya que en el estado deambular */
/*				 s�lo se le dice a donde ir pero sin colisiones.				*/
/*				 Como soluci�n, pusimos un tope de colisiones a partir del cu�l */
/*				 se le paraba a la entidad que hubiera sobrepasado ese l�mite.  */
/* Par�metros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCampesino::Colisionar()
{
	num_colisiones++;
}


/********************************************************************************/
/* M�todo      : GetNumColisiones										    	*/
/* Descripci�n : Obtiene el n�mero de colisiones acumuladas del campesino.		*/
/* Par�metros  : 																*/
/* Devuelve    : El n�mero de colisiones del campesino.							*/
/********************************************************************************/
int nCampesino::GetNumColisiones()
{
	return num_colisiones;
}