#define N_IMPLEMENTS nCasaPlanos
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/ncasaplanos.h"
#include "toshi/njugador.h"

nNebulaScriptClass( nCasaPlanos, "nedificio" );

// M�ximo n�mero de caracteres por l�nea en el fichero
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
/* M�todo      : SetPos														    */
/* Descripci�n : Asigna una posici�n a la casa de planos.						*/
/* Par�metros  : float x (ENT). Valor de la coordenada x.						*/
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
/* M�todo      : Pintar														    */
/* Descripci�n : Asigna un nodo visual, una malla de colisi�n, un tipo y clase  */
/*				 de colisi�n a la entidad creada.				    			*/
/* Par�metros  : 																*/
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
/* M�todo      : Seleccionar												    */
/* Descripci�n : Creamos un efecto de selecci�n cuando se selecciona la entidad	*/
/* Par�metros  : 																*/
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
/* M�todo      : SetRutaFichero												    */
/* Descripci�n : Asigna la ruta al atributo correspondiente.					*/
/* Par�metros  : const char * ruta (ENT). Nombre completo de la ruta.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCasaPlanos::SetRutaFichero( const char *ruta )
{
	char buf[N_MAXPATH];

	ruta_fichero = refFileServer->ManglePath( ruta, buf, N_MAXPATH );
}


/********************************************************************************/
/* M�todo      : CargaFicheroPlanos											    */
/* Descripci�n : Se encarga de leer del fichero de texto los planos e insertar- */
/*				 los desordenadamente en la estructura de datos que contiene la */
/* 				 casa de planos, que es una lista.								*/
/* Par�metros  : 																*/
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
			// Identificador �nico del plano
			id_plano++;
			// Nombre del plano
			fichero.getline( linea, NUM_MAX_CARACTERES );
			nombre_plano = linea;
			// Distintivo del plano
			fichero.getline( linea, NUM_MAX_CARACTERES );
			tipo_plano = linea;
			dist_plano = TipoDePlano( tipo_plano );
			// Descripci�n del plano
			fichero.getline( linea, NUM_MAX_CARACTERES );
			desc_plano = linea;
			// Descripci�n del plano, segunda linea
			fichero.getline( linea, NUM_MAX_CARACTERES );
			desc_plano2 = linea;
			// Coste del plano
			fichero >> coste_plano;
			// Valor del plano
			fichero >> valor_plano;
			// Despu�s del valor entero hay que saltar l�nea
			fichero.getline( linea, 1 );

			// Crea una instancia del objeto plano le�do del fichero
			plano = new nPlano( id_plano, id_plano, nombre_plano, dist_plano, desc_plano, desc_plano2, coste_plano, valor_plano );

			// Inserta en la lista de planos el plano le�do del fichero
			if ( lista_planos.empty() )
				lista_planos.push_back( plano );
			else
			{
				// N�mero aleatorio: valor posible desde 1 hasta el n�mero de planos le�dos
				posicion = ( rand() % id_plano ) + 1;

				// En la primera posici�n
				if ( posicion == 1 )
					lista_planos.push_back( plano );
				else
				{
					// En la �ltima posici�n
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
/* M�todo      : GetPlano											    		*/
/* Descripci�n : Saca el �ltimo elemento de la lista de planos.					*/
/* Par�metros  : nPlano plano (SAL). Plano que saca de la lista de planos.		*/
/* Devuelve    : TRUE si la lista no est� vac�a.								*/
/*				 FALSE si la lista est� vac�a.									*/
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
/* M�todo      : TipoDePlano										    		*/
/* Descripci�n : Convierte un tipo de plano de tipo cadena a tipo enumerado.	*/
/* Par�metros  : string tipo_plano (ENT). Tipo de plano a convertir.			*/
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
/* M�todo      : EstaEnRango												    */
/* Descripci�n : Comprobamos si la entidad est� dentro de un rango.				*/
/* Par�metros  : vector3 posicion (ENT). Posici�n espacial a comprobar.			*/
/* Devuelve    : TRUE si la posici�n pasada est� a menor distancia del margen.	*/
/*				 FALSE si la posici�n pasada queda fuera del margen.			*/
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
/* M�todo      : Tick														    */
/* Descripci�n : Ejecuta la l�gica de la entidad durante el tiempo pasado por	*/
/*				 par�metro.														*/
/* Par�metros  : float dt (ENT). Tiempo transcurrido desde el �ltimo tick.		*/
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
/* M�todo      : GetListaPlanos										    		*/
/* Descripci�n : Obtiene la lista de planos que contiene la casa de planos. 	*/
/* Par�metros  : 																*/
/* Devuelve    : La lista de planos.											*/
/********************************************************************************/
list<nPlano *> nCasaPlanos::GetListaPlanos()
{
	return lista_planos;
}


/********************************************************************************/
/* M�todo      : InsertaPlano										    		*/
/* Descripci�n : Inserta en la lista de planos el plano pasado por par�metro en	*/
/*				 la primera posici�n.											*/
/* Par�metros  : nPlano plano (ENT/SAL). Plano que se va a insertar.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nCasaPlanos::InsertaPlano( nPlano* &plano )
{
	list<nPlano *>::iterator  iter;

	iter = lista_planos.begin();
	lista_planos.insert( iter , 1, plano );
}


/********************************************************************************/
/* M�todo      : LeePlanos											    		*/
/* Descripci�n : Recorre toda la lista de planos y muestra la informaci�n de 	*/
/*				 cada plano. Lo utilizamos para modo debug.						*/
/* Par�metros  : 																*/
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