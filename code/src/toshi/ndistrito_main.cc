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
/* Método      : AgregarAyuntamiento										    */
/* Descripción : Asigna un ayuntamiento al distrito.							*/
/* Parámetros  : int id_ayuntamiento (ENT). Valor del identificador del ayto.	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nDistrito::AgregarAyuntamiento( int id_ayuntamiento )
{
	char path[N_MAXPATH];

	sprintf( path, "/game/world/ayto%d", id_ayuntamiento );
	ayuntamiento = (nString) path;
}


/********************************************************************************/
/* Método      : AgregarConstruccion										    */
/* Descripción : Añade un plano construido al distrito.							*/
/* Parámetros  : nPlano p_plano (ENT/SAL). Plano que se va a insertar.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nDistrito::AgregarConstruccion( nPlano* &p_plano )
{
	lista_planos.PushBack( p_plano );
}


/********************************************************************************/
/* Método      : ExisteEdificio												    */
/* Descripción : Comprueba si el edificio pasado por parámetro existe como		*/
/*				 construcción en el distrito.									*/
/* Parámetros  : string p_nombre (ENT). Nombre del edificio a buscar.			*/
/* Devuelve    : TRUE si el edificio está construido.							*/
/*				 FALSE si no está construido.									*/
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
/* Método      : GetPlano													    */
/* Descripción : Obtiene el plano del distrito cuyo identificador es igual al	*/
/*				 pasado por parámetro.											*/
/* Parámetros  : int p_id_plano (ENT). Identificador del plano a buscar.		*/
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
/* Método      : EliminarConstruccion										    */
/* Descripción : Se elimina de la lista de construcciones del distrito la que 	*/
/*				 se pasa por parámetro.											*/
/* Parámetros  : int construccion (ENT). Identificador de la construcción a 	*/
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
/* Método      : NumeroConstrucciones										    */
/* Descripción : Cuenta cuántos edificios tiene construidos el distrito.	  	*/
/* Parámetros  : 															 	*/
/* Devuelve    : El número de construcciones del distrito.						*/
/********************************************************************************/
int nDistrito::NumeroConstrucciones()
{
	return lista_planos.Size();
}


/********************************************************************************/
/* Método      : DentroDelDistrito											    */
/* Descripción : Comprueba si la posición pasada por parámetro se encuentra  	*/
/*				 dentro de una circunferencia de radio = RADIO_DISTRITO.		*/
/* Parámetros  : float x (ENT). Valor de la coordenada x.					 	*/
/*				 float y (ENT). Valor de la coordenada y.						*/
/* Devuelve    : TRUE si la posición está dentro del radio.						*/
/*				 FALSE si no está dentro.										*/
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
/* Método      : MostrarSeleccion											    */
/* Descripción : Se muestra una zona destacada en el mapa correspondiente con  	*/
/*				 el distrito. Esta zona será la zona válida para construir.		*/
/* Parámetros  : 															 	*/
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
/* Método      : OcultarSeleccion											    */
/* Descripción : Se desactiva la zona destacada del mapa que correspondía con  	*/
/*				 el distrito donde se podía construir. 							*/
/* Parámetros  : 															 	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nDistrito::OcultarSeleccion()
{
	n3DNode *seleccion = (n3DNode *) kernelServer->Lookup( "/game/scene/selcons" );
	seleccion->SetActive( false );
}


/********************************************************************************/
/* Método      : GetListaPlanos												    */
/* Descripción : Obtiene la lista de planos construidos en el distrito.		  	*/
/* Parámetros  : 															 	*/
/* Devuelve    : La lista de planos construidos en el distrito.					*/
/********************************************************************************/
nArray<nPlano *> nDistrito::GetListaPlanos()
{
	return lista_planos;
}


/********************************************************************************/
/* Método      : GetEdificio												    */
/* Descripción : Obtiene el plano de la lista de planos construidos cuya 	 	*/
/*				 posición en la lista es la pasada por parámetro.				*/
/* Parámetros  : int posicion (ENT). Posición en la lista de planos construidos */
/* Devuelve    : El plano construido.											*/
/********************************************************************************/
nPlano *nDistrito::GetEdificio( int posicion )
{
	return lista_planos.At( posicion );
}


/********************************************************************************/
/* Método      : BuscarPosicionConstruccion									    */
/* Descripción : Obtiene una posición al azar dentro de la circunferencia que	*/
/*				 delimita el distrito, donde se va a construir. Se comprueba 	*/
/*				 la posición sea válida.										*/
/* Parámetros  : 															    */
/* Devuelve    : La posición donde se va a construir.							*/
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