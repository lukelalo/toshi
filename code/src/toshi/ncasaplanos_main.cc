#define N_IMPLEMENTS nCasaPlanos
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/ncasaplanos.h"
#include "toshi/njugador.h"

nNebulaScriptClass( nCasaPlanos, "nedificio" );

// Máximo número de caracteres por línea en el fichero
#define NUM_MAX_CARACTERES   350

#include <iostream>
#include <fstream>
#include <string>
#include "time.h"

using namespace std;

// Constructor general
nCasaPlanos::nCasaPlanos() : refFileServer( kernelServer, this ),
							 ruta_fichero( "" )
{
	srand( (unsigned int) time( NULL ) );
	refFileServer = "/sys/servers/file2";
	posicion_x    = 0.0f;
	posicion_y    = 0.0f;
	SetTipo( T_CASAPLANOS );
	SetRutaFichero( "text:Fabrica_De_Planos.txt" );
	CargaFicheroPlanos();
	CanalSeleccion = refChannelServer->GenChannel( "sel_cp" );
	valorSeleccion = TIEMPO_SELECCION;
	refChannelServer->SetChannel1f( CanalSeleccion, valorSeleccion );
}


// Destructor
nCasaPlanos::~nCasaPlanos()
{
	list<nPlano *>::iterator iter;

	iter = lista_planos.begin();
	for ( int j = 0; j < NumeroDePlanos(); j++ )
	{
		n_free( iter.operator *() );
		iter = iter.operator ++();
	}
	lista_planos.clear();
}


/********************************************************************************/
/* Método      : SetPos														    */
/* Descripción : Asigna una posición a la casa de planos.						*/
/* Parámetros  : float x (ENT). Valor de la coordenada x.						*/
/*				 float y (ENT). Valor de la coordenada y.						*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCasaPlanos::SetPos( float x, float y )
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
void nCasaPlanos::Pintar()
{
	SetVisNode( "/game/models/casaplanos" );
	SetCollideShape( "meshes:casaplanos.n3d" );
	SetTouchMethod( "ignorarColision" );
	SetCollideClass( "edificio" );
}


/********************************************************************************/
/* Método      : Seleccionar												    */
/* Descripción : Creamos un efecto de selección cuando se selecciona la entidad	*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCasaPlanos::Seleccionar()
{
	vector3     posicion;
	nShaderNode *shader;
	char        *params = NULL;

	shader = (nShaderNode *) kernelServer->Lookup( "/game/models/casaplanos/shader" );
	posicion = GetPosition();
	valorSeleccion = 0.0f;
}


/********************************************************************************/
/* Método      : SetRutaFichero												    */
/* Descripción : Asigna la ruta al atributo correspondiente.					*/
/* Parámetros  : const char * ruta (ENT). Nombre completo de la ruta.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCasaPlanos::SetRutaFichero( const char *ruta )
{
	char buf[N_MAXPATH];

	ruta_fichero = refFileServer->ManglePath( ruta, buf, N_MAXPATH );
}


/********************************************************************************/
/* Método      : CargaFicheroPlanos											    */
/* Descripción : Se encarga de leer del fichero de texto los planos e insertar- */
/*				 los desordenadamente en la estructura de datos que contiene la */
/* 				 casa de planos, que es una lista.								*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCasaPlanos::CargaFicheroPlanos()
{
	int						  id_plano     = 0;
	string					  nombre_plano = "";
	string					  tipo_plano   = "";
	nPlano::TPlano			  dist_plano   = nPlano::TPlano::DESCONOCIDO;
	string					  desc_plano   = "";
	string					  desc_plano2  = "";
	int						  coste_plano  = 0;
	int						  valor_plano  = 0;
	nPlano					  *plano       = NULL;
	fstream					  fichero;
	char					  linea[NUM_MAX_CARACTERES];
	int					 	  posicion     = 1;
	list<nPlano *>::iterator  nodo;

	// Se abre el fichero
	fichero.open( ruta_fichero.Get(), ios_base::in );

	if ( !fichero.fail() )
	{
		while ( !fichero.eof() )
		{
			// Identificador único del plano
			id_plano++;
			// Nombre del plano
			fichero.getline( linea, NUM_MAX_CARACTERES );
			nombre_plano = linea;
			// Distintivo del plano
			fichero.getline( linea, NUM_MAX_CARACTERES );
			tipo_plano = linea;
			dist_plano = TipoDePlano( tipo_plano );
			// Descripción del plano
			fichero.getline( linea, NUM_MAX_CARACTERES );
			desc_plano = linea;
			// Descripción del plano, segunda linea
			fichero.getline( linea, NUM_MAX_CARACTERES );
			desc_plano2 = linea;
			// Coste del plano
			fichero >> coste_plano;
			// Valor del plano
			fichero >> valor_plano;
			// Después del valor entero hay que saltar línea
			fichero.getline( linea, 1 );

			// Crea una instancia del objeto plano leído del fichero
			plano = new nPlano( id_plano, id_plano, nombre_plano, dist_plano, desc_plano, desc_plano2, coste_plano, valor_plano );

			// Inserta en la lista de planos el plano leído del fichero
			if ( lista_planos.empty() )
				lista_planos.push_back( plano );
			else
			{
				// Número aleatorio: valor posible desde 1 hasta el número de planos leídos
				posicion = ( rand() % id_plano ) + 1;

				// En la primera posición
				if ( posicion == 1 )
					lista_planos.push_back( plano );
				else
				{
					// En la última posición
					if ( posicion == id_plano )
						lista_planos.push_front( plano );
					else
					{
						// iterator
						nodo = lista_planos.begin();

						for ( int iter = 0; iter < posicion; iter++ )
							nodo = nodo.operator ++();

						lista_planos.insert( nodo , 1, plano );
					}
				}
			}
		}
	}

	// Cierra el fichero
	fichero.close();
}


/********************************************************************************/
/* Método      : GetPlano											    		*/
/* Descripción : Saca el último elemento de la lista de planos.					*/
/* Parámetros  : nPlano plano (SAL). Plano que saca de la lista de planos.		*/
/* Devuelve    : TRUE si la lista no está vacía.								*/
/*				 FALSE si la lista está vacía.									*/
/********************************************************************************/
bool nCasaPlanos::GetPlano( nPlano* &plano )
{
	// La casa de planos no tiene planos
	if ( lista_planos.empty() )
	{
		plano = NULL;
		return false;
	}
	else
	{
		plano = lista_planos.back();
		lista_planos.pop_back();
		return true;
	}
}


/********************************************************************************/
/* Método      : TipoDePlano										    		*/
/* Descripción : Convierte un tipo de plano de tipo cadena a tipo enumerado.	*/
/* Parámetros  : string tipo_plano (ENT). Tipo de plano a convertir.			*/
/* Devuelve    : El tipo enumerado asociado a la cadena.						*/
/********************************************************************************/
nPlano::TPlano nCasaPlanos::TipoDePlano( string tipo_plano )
{
	nPlano::TPlano t_plano = nPlano::TPlano::DESCONOCIDO;

	if ( tipo_plano == "IMPERIAL" )
		t_plano = nPlano::TPlano::IMPERIAL;
	else if ( tipo_plano == "RELIGIOSO" )
		t_plano = nPlano::TPlano::RELIGIOSO;
	else if ( tipo_plano == "COMERCIAL" )
		t_plano = nPlano::TPlano::COMERCIAL;
	else if ( tipo_plano == "MILITAR" )
		t_plano = nPlano::TPlano::MILITAR;
	else if ( tipo_plano == "ESPECIAL" )
		t_plano = nPlano::TPlano::ESPECIAL;
	else
		t_plano = nPlano::TPlano::DESCONOCIDO;

	return t_plano;
}


/********************************************************************************/
/* Método      : EstaEnRango												    */
/* Descripción : Comprobamos si la entidad está dentro de un rango.				*/
/* Parámetros  : vector3 posicion (ENT). Posición espacial a comprobar.			*/
/* Devuelve    : TRUE si la posición pasada está a menor distancia del margen.	*/
/*				 FALSE si la posición pasada queda fuera del margen.			*/
/********************************************************************************/
bool nCasaPlanos::EstaEnRango( vector3 posicion )
{
    vector3 distancia;
	vector3 miPosicion;

	miPosicion   = GetPosition();
	miPosicion.y = 0.0f;
	posicion.y	 = 0.0f;
	distancia    = posicion - miPosicion;
	return distancia.len() < DISTANCIA_A_CASAPLANOS;
}


/********************************************************************************/
/* Método      : Tick														    */
/* Descripción : Ejecuta la lógica de la entidad durante el tiempo pasado por	*/
/*				 parámetro.														*/
/* Parámetros  : float dt (ENT). Tiempo transcurrido desde el último tick.		*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCasaPlanos::Tick( float dt )
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
/* Método      : GetListaPlanos										    		*/
/* Descripción : Obtiene la lista de planos que contiene la casa de planos. 	*/
/* Parámetros  : 																*/
/* Devuelve    : La lista de planos.											*/
/********************************************************************************/
list<nPlano *> nCasaPlanos::GetListaPlanos()
{
	return lista_planos;
}


/********************************************************************************/
/* Método      : InsertaPlano										    		*/
/* Descripción : Inserta en la lista de planos el plano pasado por parámetro en	*/
/*				 la primera posición.											*/
/* Parámetros  : nPlano plano (ENT/SAL). Plano que se va a insertar.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCasaPlanos::InsertaPlano( nPlano* &plano )
{
	list<nPlano *>::iterator  iter;

	iter = lista_planos.begin();
	lista_planos.insert( iter , 1, plano );
}


/********************************************************************************/
/* Método      : LeePlanos											    		*/
/* Descripción : Recorre toda la lista de planos y muestra la información de 	*/
/*				 cada plano. Lo utilizamos para modo debug.						*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCasaPlanos::LeePlanos()
{
	list<nPlano *>::iterator iter;

	iter = lista_planos.begin();
	for ( int j = 0; j < NumeroDePlanos(); j++ )
	{
		n_printf( "PLANO %d - %s - CON IDHUD = %d COSTE = %d VALOR = %d\n", iter.operator *()->GetIdEdificio(),
																			iter.operator *()->GetNombre().c_str(),
																			iter.operator *()->GetIdHUD(),
																			iter.operator *()->GetCoste(),
																			iter.operator *()->GetValor() );
		iter = iter.operator ++();
	}
}