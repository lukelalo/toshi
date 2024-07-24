#define N_IMPLEMENTS nDistrito
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/ndistrito.h"
#include "toshi/nayuntamiento.h"
#include "toshi/nplano.h"
#include "toshi/nmapa.h"
#include "toshi/nworld.h"

nNebulaScriptClass( nDistrito, "nroot" );


// Constructor por defecto
nDistrito::nDistrito()
{
	lista_planos.Clear();
	ayuntamiento = "";
	srand( (unsigned int) time( NULL ) );
}


// Destructor
nDistrito::~nDistrito()
{
	for ( int i = 0; i < lista_planos.Size(); i++ )
		n_free( lista_planos.At( i ) );

	lista_planos.Clear();
}


/********************************************************************************/
/* M�todo      : AgregarAyuntamiento										    */
/* Descripci�n : Asigna un ayuntamiento al distrito.							*/
/* Par�metros  : int id_ayuntamiento (ENT). Valor del identificador del ayto.	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nDistrito::AgregarAyuntamiento( int id_ayuntamiento )
{
	char path[N_MAXPATH];

	sprintf( path, "/game/world/ayto%d", id_ayuntamiento );
	ayuntamiento = (nString) path;
}


/********************************************************************************/
/* M�todo      : AgregarConstruccion										    */
/* Descripci�n : A�ade un plano construido al distrito.							*/
/* Par�metros  : nPlano p_plano (ENT/SAL). Plano que se va a insertar.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nDistrito::AgregarConstruccion( nPlano* &p_plano )
{
	lista_planos.PushBack( p_plano );
}


/********************************************************************************/
/* M�todo      : ExisteEdificio												    */
/* Descripci�n : Comprueba si el edificio pasado por par�metro existe como		*/
/*				 construcci�n en el distrito.									*/
/* Par�metros  : string p_nombre (ENT). Nombre del edificio a buscar.			*/
/* Devuelve    : TRUE si el edificio est� construido.							*/
/*				 FALSE si no est� construido.									*/
/********************************************************************************/
bool nDistrito::ExisteEdificio( string p_nombre )
{
	for ( int i = 0; i < lista_planos.Size(); i++ )
	{
		if ( lista_planos.At( i ) != NULL )
		{
			if ( lista_planos.At( i )->GetNombre() == p_nombre )
				return true;
		}
	}
	return false;
}


/********************************************************************************/
/* M�todo      : GetPlano													    */
/* Descripci�n : Obtiene el plano del distrito cuyo identificador es igual al	*/
/*				 pasado por par�metro.											*/
/* Par�metros  : int p_id_plano (ENT). Identificador del plano a buscar.		*/
/* Devuelve    : El plano buscado o NULL si no lo encuentra.					*/
/********************************************************************************/
nPlano *nDistrito::GetPlano( int p_id_plano )
{
	for ( int i = 0; i < lista_planos.Size(); i++ )
	{
		if ( lista_planos.At( i ) != NULL )
		{
			if ( lista_planos.At( i )->GetIdEdificio() == p_id_plano )
				return lista_planos.At( i );
		}
	}
	return NULL;
}


/********************************************************************************/
/* M�todo      : EliminarConstruccion										    */
/* Descripci�n : Se elimina de la lista de construcciones del distrito la que 	*/
/*				 se pasa por par�metro.											*/
/* Par�metros  : int construccion (ENT). Identificador de la construcci�n a 	*/
/*				 eliminar.														*/
/* Devuelve    : 																*/
/********************************************************************************/
void nDistrito::EliminarConstruccion( int construccion )
{
	for ( int i = 0; i < lista_planos.Size(); i++ )
	{
		if ( lista_planos.At( i ) != NULL )
		{
			if ( lista_planos.At( i )->GetIdEdificio() == construccion )
				lista_planos.Erase( i );
		}
	}
}


/********************************************************************************/
/* M�todo      : NumeroConstrucciones										    */
/* Descripci�n : Cuenta cu�ntos edificios tiene construidos el distrito.	  	*/
/* Par�metros  : 															 	*/
/* Devuelve    : El n�mero de construcciones del distrito.						*/
/********************************************************************************/
int nDistrito::NumeroConstrucciones()
{
	return lista_planos.Size();
}


/********************************************************************************/
/* M�todo      : DentroDelDistrito											    */
/* Descripci�n : Comprueba si la posici�n pasada por par�metro se encuentra  	*/
/*				 dentro de una circunferencia de radio = RADIO_DISTRITO.		*/
/* Par�metros  : float x (ENT). Valor de la coordenada x.					 	*/
/*				 float y (ENT). Valor de la coordenada y.						*/
/* Devuelve    : TRUE si la posici�n est� dentro del radio.						*/
/*				 FALSE si no est� dentro.										*/
/********************************************************************************/
bool nDistrito::DentroDelDistrito( float x, float y )
{
	vector2 posicionAyuntamiento, posicionDestino;

	nAyuntamiento *refAyuntamiento = (nAyuntamiento *) kernelServer->Lookup( ayuntamiento.Get() );
	posicionAyuntamiento.set( refAyuntamiento->GetPosition().x, refAyuntamiento->GetPosition().z );
	posicionDestino.set( x, y );
	posicionDestino -= posicionAyuntamiento;
	float distancia = posicionDestino.len();
	return distancia < RADIO_DISTRITO;
}


/********************************************************************************/
/* M�todo      : MostrarSeleccion											    */
/* Descripci�n : Se muestra una zona destacada en el mapa correspondiente con  	*/
/*				 el distrito. Esta zona ser� la zona v�lida para construir.		*/
/* Par�metros  : 															 	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nDistrito::MostrarSeleccion()
{
	nWorld        *v_world         = (nWorld *) kernelServer->Lookup( "/game/world" );
	n3DNode       *seleccion       = (n3DNode *) kernelServer->Lookup( "/game/scene/selcons" );
	nAyuntamiento *refAyuntamiento = (nAyuntamiento *) kernelServer->Lookup( ayuntamiento.Get() );

	vector3 v_posicion = refAyuntamiento->GetPosition();
	seleccion->Txyz( v_posicion.x, v_world->GetHeight( v_posicion ) + -1.0f, v_posicion.z );
	seleccion->SetActive( true );
}


/********************************************************************************/
/* M�todo      : OcultarSeleccion											    */
/* Descripci�n : Se desactiva la zona destacada del mapa que correspond�a con  	*/
/*				 el distrito donde se pod�a construir. 							*/
/* Par�metros  : 															 	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nDistrito::OcultarSeleccion()
{
	n3DNode *seleccion = (n3DNode *) kernelServer->Lookup( "/game/scene/selcons" );
	seleccion->SetActive( false );
}


/********************************************************************************/
/* M�todo      : GetListaPlanos												    */
/* Descripci�n : Obtiene la lista de planos construidos en el distrito.		  	*/
/* Par�metros  : 															 	*/
/* Devuelve    : La lista de planos construidos en el distrito.					*/
/********************************************************************************/
nArray<nPlano *> nDistrito::GetListaPlanos()
{
	return lista_planos;
}


/********************************************************************************/
/* M�todo      : GetEdificio												    */
/* Descripci�n : Obtiene el plano de la lista de planos construidos cuya 	 	*/
/*				 posici�n en la lista es la pasada por par�metro.				*/
/* Par�metros  : int posicion (ENT). Posici�n en la lista de planos construidos */
/* Devuelve    : El plano construido.											*/
/********************************************************************************/
nPlano *nDistrito::GetEdificio( int posicion )
{
	return lista_planos.At( posicion );
}


/********************************************************************************/
/* M�todo      : BuscarPosicionConstruccion									    */
/* Descripci�n : Obtiene una posici�n al azar dentro de la circunferencia que	*/
/*				 delimita el distrito, donde se va a construir. Se comprueba 	*/
/*				 la posici�n sea v�lida.										*/
/* Par�metros  : 															    */
/* Devuelve    : La posici�n donde se va a construir.							*/
/********************************************************************************/
vector2 nDistrito::BuscarPosicionConstruccion()
{
	vector2 posicionAyuntamiento, posicion;
	int posx, posy;

	nAyuntamiento *refAyuntamiento = (nAyuntamiento *) kernelServer->Lookup( ayuntamiento.Get() );
	posicionAyuntamiento.set( refAyuntamiento->GetPosition().x, refAyuntamiento->GetPosition().z );

	nMapa *mapa = (nMapa *) kernelServer->Lookup( "/data/mapa" );
	do
	{
		posx = (int) ( (rand() % 8) - 4 + posicionAyuntamiento.x );
		posy = (int) ( (rand() % 8) - 4 + posicionAyuntamiento.y );

	} while ( !mapa->PuedeConstruirEdificio( posx, posy ) );
	posicion.set( (int) posx + 0.5f, (int) posy + 0.5f);
	return posicion;
}