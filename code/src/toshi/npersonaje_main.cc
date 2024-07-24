#define N_IMPLEMENTS nPersonaje
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/npersonaje.h"
#include "toshi/nworld.h"
#include "collide/ncollideserver.h"
#include "collide/ncollideobject.h"
#include "toshi/njugador.h"
#include "toshi/ngame.h"
#include "toshi/ncampesino.h"

nNebulaScriptClass( nPersonaje, "nentity" );


// Constructor por defecto
nPersonaje::nPersonaje()
{
	Nombre				  = NULL;
	Tiempo				  = 0.0f;
	TiempoPago            = 0.0f;
	PosicionX			  = 0.0f;
	PosicionY			  = 0.0f;
	CampesinosContratados = 0;
	Animacion			  = ANIM_QUIETO;
	nodoActual            = 0;
	o_modelo              = NULL;
	NumSecuencias		  = 0;
    estado                = QUIETO;
	entidadDestino        = NULL;
	TipoPersonaje		  = PERSONAJE_CAMPESINO;
	manager               = NULL;
	Muerto                = false;
	CanalSonido			  = -1;
	srand( (unsigned int) time( NULL ) );
}


// Destructor
nPersonaje::~nPersonaje()
{
}


/********************************************************************************/
/* Método      : SetPosicion												    */
/* Descripción : Asigna una posición al personaje.						    	*/
/* Parámetros  : float pos_x (ENT). Valor de la coordenada x.					*/
/*				 float pos_y (ENT). Valor de la coordenada y.				    */
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::SetPosicion( float pos_x, float pos_y )
{
	PosicionX = pos_x;
	PosicionY = pos_y;
}


/********************************************************************************/
/* Método      : SetTiempo													    */
/* Descripción : Asigna un tiempo de turno inicial al personaje.				*/
/* Parámetros  : float tiempo (ENT). Valor del atributo Tiempo.					*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::SetTiempo( float tiempo )
{
	Tiempo = tiempo;
}


/********************************************************************************/
/* Método      : SetTiempoPago												    */
/* Descripción : Asigna un momento de pago durante el turno dl personaje cuando	*/
/*				 tenga campesinos contratados.									*/
/* Parámetros  : float tiempo_pago (ENT). Valor del atributo TiempoPago.		*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::SetTiempoPago( float tiempo_pago )
{
	TiempoPago = tiempo_pago;
}


/********************************************************************************/
/* Método      : SetNombre													    */
/* Descripción : Asigna un nombre al personaje.									*/
/* Parámetros  : const char nombre_personaje (ENT). Valor del atributo Nombre.	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::SetNombre( const char *nombre_personaje )
{
	Nombre = nombre_personaje;
}


/********************************************************************************/
/* Método      : SetCampesinosContratados									    */
/* Descripción : Asigna un número de campesinos contratados al personaje.		*/
/* Parámetros  : int campesinos (ENT). Valor del atributo CampesinosContratados	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::SetCampesinosContratados( int campesinos )
{
	CampesinosContratados = campesinos;
}


/********************************************************************************/
/* Método      : ContratarCampesino											    */
/* Descripción : Añade el identificador del campesino contratado a la lista de	*/
/*				 campesinos contratados y aumenta el contador de campesinos     */
/*				 contratados.													*/
/* Parámetros  : int id_campesino (ENT). Identificador del campesino.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::ContratarCampesino( int id_campesino )
{
	lista_campesinos.PushBack( id_campesino );
	//n_printf( "CAMPESINO %d CONTRATADO\n", id_campesino );
	CampesinosContratados++;
}


/********************************************************************************/
/* Método      : LiberarCampesino											    */
/* Descripción : Elimina el identificador del campesino a liberar de la lista 	*/
/*				 de campesinos contratados y decrementa el contador de 			*/
/*				 campesinos contratados.										*/
/* Parámetros  : int id_campesino (ENT). Identificador del campesino.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::LiberarCampesino( int id_campesino )
{
	for ( int i = 0; i < (int) lista_campesinos.Size(); i++ )
	{
		if ( lista_campesinos.At( i ) == id_campesino )
		{
			lista_campesinos.Erase( i );
			//n_printf( "CAMPESINO %d LIBERADO\n", id_campesino );
			CampesinosContratados--;
			return;
		}
	}
}


/********************************************************************************/
/* Método      : OrientarSombra												    */
/* Descripción : Orienta la sombra del personaje para que el mapa de alturas	*/
/*				 no la oculta.													*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::OrientarSombra()
{
	nWorld *v_world = (nWorld *) kernelServer->Lookup( "/game/world" );
	char buf[N_MAXPATH];
	sprintf( buf, "/game/scene/sombras/%s", GetName() );
	n3DNode *v_sombra = (n3DNode *) kernelServer->Lookup( buf );

	vector3 v_posicion = GetPosition();
	vector3 w_normal;

	v_world->GetNormal( v_posicion, w_normal );

	quaternion aux( w_normal.x, w_normal.y, w_normal.z, 0.0f );
	w_normal = aux.rotate( w_normal );

	if ( v_sombra != NULL )
	{
		v_sombra->Qxyzw( w_normal.x, w_normal.y, w_normal.z, 0.0f );
		v_sombra->Txyz( v_posicion.x, v_world->GetHeight( v_posicion ), v_posicion.z );
	}
}


/********************************************************************************/
/* Método      : OcultarSombra												    */
/* Descripción : Desactiva la sombra del personaje.								*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::OcultarSombra()
{
	nWorld *v_world = (nWorld *) kernelServer->Lookup( "/game/world" );
	char buf[N_MAXPATH];
	sprintf( buf, "/game/scene/sombras/%s", GetName() );
	n3DNode *v_sombra = (n3DNode *) kernelServer->Lookup( buf );

	if ( v_sombra != NULL )
	{
		v_sombra->SetActive( false );
		OrientarSombra();
	}
}


/********************************************************************************/
/* Método      : MostrarSombra												    */
/* Descripción : Activa la sombra del personaje.								*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::MostrarSombra()
{
	nWorld *v_world = (nWorld *) kernelServer->Lookup( "/game/world" );
	char buf[N_MAXPATH];
	sprintf( buf, "/game/scene/sombras/%s", GetName() );
	n3DNode *v_sombra = (n3DNode *) kernelServer->Lookup( buf );

	if ( v_sombra != NULL )
	{
		v_sombra->SetActive( true );
		OrientarSombra();
	}
}


/********************************************************************************/
/* Método      : SetAnimacion												    */
/* Descripción : Asigna una secuencia de animaciones al personaje según indique	*/
/*			 	 el parámetro de entrada. A casi todas las animaciones les 		*/
/*				 acompañará un sonido FX.										*/
/* Parámetros  : TAnim animacionNueva (ENT). Tipo de animación del personaje.	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::SetAnimacion( TAnim animacionNueva )
{
	// Posición idle aleatorio entre las dos que tenemos
	int anim_quieto = rand() % 2;
	Animacion = animacionNueva;

	lista_animaciones.Clear();
	NumSecuencias = 0;

	// Si va a poner la animación de quieto, la ponemos después de que termine la acción que tiene pendiente
	if ( o_modelo->HasAnimationFinished() && animacionNueva == ANIM_QUIETO )
	{
		lista_animaciones.PushBack( anim_quieto );
		NumSecuencias++;
		return;
	}

	nGame *juego = (nGame *) kernelServer->Lookup( "/game" );
	switch ( Animacion )
	{
		case ANIM_ANDAR:
			o_modelo->SetAnimation( 5 );
			CanalSonido = juego->Play3DSoundEx( 2, "sounds:posicional/pasolento.ogg", GetPosition(), 0.1f, 40.0f, true, 10 );
			break;
		case ANIM_QUIETO:
			o_modelo->SetAnimation( anim_quieto );
			juego->StopSound( CanalSonido );
			break;
		case ANIM_ACCION:
			lista_animaciones.PushBack( anim_quieto );
			NumSecuencias++;
			o_modelo->SetAnimation( 22 );
			break;
		case ANIM_CORRER:
			o_modelo->SetAnimation( 6 );
			if ( GetJugador() != NULL )
			{
				if ( GetJugador()->GetJugadorPrincipal() )
					CanalSonido = juego->Play3DSoundEx( 2, "sounds:posicional/pasorapidob.ogg", GetPosition(), 0.001f, 40.0f, true, 50 );
				else
					CanalSonido = juego->Play3DSoundEx( 2, "sounds:posicional/pasorapido.ogg", GetPosition(), 0.1f, 40.0f, true, 50 );
			}
			else
				CanalSonido = juego->Play3DSoundEx( 2, "sounds:posicional/pasorapido.ogg", GetPosition(), 0.1f, 40.0f, true, 50 );
			break;
		case ANIM_COGER:
			lista_animaciones.PushBack( anim_quieto );
			NumSecuencias++;
			o_modelo->SetAnimation( 12 );
			break;
		case ANIM_LIBERAR_PERSONAJE:
			o_modelo->SetAnimation( 16 );
			break;
		case ANIM_CAMBIAR_PERSONAJE:
			lista_animaciones.PushBack( anim_quieto );
			NumSecuencias++;
			o_modelo->SetAnimation( 17 );
			break;
		case ANIM_MORIR:
			lista_animaciones.PushBack( 9 );
			lista_animaciones.PushBack( 8 );
			NumSecuencias += 2;
			o_modelo->SetAnimation( 7 );
			juego->Play3DSound( 0, "sounds:posicional/asesina.wav", GetPosition() );
			break;
		case ANIM_RESUCITAR:
			lista_animaciones.PushBack( anim_quieto );
			lista_animaciones.PushBack( 11 );
			NumSecuencias += 2;
			o_modelo->SetAnimation( 10 );
			break;
		case ANIM_RESUCITAR_A:
			lista_animaciones.PushBack( anim_quieto );
			lista_animaciones.PushBack( 23 );
			NumSecuencias += 2;
			o_modelo->SetAnimation( 22 );
			juego->Play3DSound( 0, "sounds:posicional/shugenja2.wav", GetPosition() );
			break;
		case ANIM_CAMBIAR_PLANOS:
			lista_animaciones.PushBack( anim_quieto );
			NumSecuencias++;
			o_modelo->SetAnimation( 23 );
			juego->Play3DSound( 0, "sounds:posicional/shugenja1.wav", GetPosition() );
			break;
		case ANIM_CONSTRUIR:
			lista_animaciones.PushBack( anim_quieto );
			lista_animaciones.PushBack( 15 );
			lista_animaciones.PushBack( 14 );
			lista_animaciones.PushBack( 14 );
			NumSecuencias += 4;
			o_modelo->SetAnimation( 13 );
			break;
		case ANIM_DESTRUIR:
			lista_animaciones.PushBack( anim_quieto );
			lista_animaciones.PushBack( 24 );
			lista_animaciones.PushBack( 23 );
			NumSecuencias += 3;
			o_modelo->SetAnimation( 22 );
			juego->Play3DSound( 0, "sounds:posicional/golpe.wav", GetPosition() );
			juego->Play3DSound( 0, "sounds:posicional/destruir1.wav", GetPosition() );
			break;
	}
}


/********************************************************************************/
/* Método      : Correr														    */
/* Descripción : Recalcula la orientación del movimiento del personaje según 	*/
/*				 sean los vectores de posición y velocidad y los puntos inicio  */
/*				 y destino.														*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::Correr()
{
	Collide();

	quaternion aux1;
	vector3    destino;
	vector3    distancia2ACero = vector3( destinoVector.x, 0.0f, destinoVector.z ) - vector3( positionVector.x, 0.0f, positionVector.z );
	float      angulo          = 0.0f;
	float      anguloDestino   = 0.0f;
	float      anguloi         = 0.0f;
	float      angulod         = 0.0f;
	float      incAngulo       = 0.0f;

	// Animación del modelo
	if ( GetAnimacion() != ANIM_CORRER )
		SetAnimacion( ANIM_CORRER );

	nGame *juego = (nGame *) kernelServer->Lookup( "/game" );
	juego->SetPosSound( CanalSonido, GetPosition() );

	// Cálculo de velocidad y pitch
	if ( velocityVector.x == 0.0f && velocityVector.z == 0.0f )
		velocityVector.set( siguienteVector.x - positionVector.x, 0.0f, siguienteVector.z - positionVector.z );

	destino.set( siguienteVector.x - positionVector.x, 0.0f, siguienteVector.z - positionVector.z );
	angulo        = CalcularAngulo( velocityVector );
	anguloDestino = CalcularAngulo( destino );
	anguloi       = n_rad2deg( angulo );
	angulod       = n_rad2deg( anguloDestino );
	aux1.set( velocityVector.x, 0.0f, velocityVector.z, 0.0f );

	if ( anguloi < angulod )
		anguloi += 360;

	incAngulo = 1.5f;
	if ( distancia2ACero.len_squared() < 1.5f )
		incAngulo = 5*1/distancia2ACero.len_squared()*distancia2ACero.len_squared();

	if ( anguloi - angulod < 2.0 )
		aux1.set_rotate_y( anguloDestino - angulo );
	else if ( anguloi - angulod < 180 )
		aux1.set_rotate_y( n_deg2rad( -incAngulo ) );
	else
		aux1.set_rotate_y( n_deg2rad( incAngulo ) );

	velocityVector = aux1.rotate( velocityVector );
	rotateVector.y = CalcularAngulo( -velocityVector );
	velocityVector.norm();
	velocityVector.set( velocityVector.x*velocidadCorrer, 0, velocityVector.z*velocidadCorrer );
	OrientarSombra();
}


/********************************************************************************/
/* Método      : Mover														    */
/* Descripción : Recalcula la orientación del movimiento del personaje según 	*/
/*				 sean los vectores de posición y velocidad y los puntos inicio  */
/*				 y destino.														*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::Mover()
{
	this->Collide();

	quaternion aux1;
	vector3    destino;
	vector3    distancia2ACero = vector3( destinoVector.x, 0.0f, destinoVector.z ) - vector3( positionVector.x, 0.0f, positionVector.z );
	float      angulo          = 0.0f;
	float      anguloDestino   = 0.0f;
	float      anguloi         = 0.0f;
	float      angulod         = 0.0f;
	float      incAngulo       = 0.0f;

	// Animación del modelo
	if ( GetAnimacion() != ANIM_ANDAR )
		SetAnimacion( ANIM_ANDAR );

	nGame *juego = (nGame *) kernelServer->Lookup( "/game" );
	juego->SetPosSound( CanalSonido, GetPosition() );

	// Cálculo de velocidad y pitch
	if ( velocityVector.x == 0.0f && velocityVector.z == 0.0f )
		velocityVector.set( siguienteVector.x - positionVector.x, 0.0f, siguienteVector.z - positionVector.z );

	destino.set( siguienteVector.x - positionVector.x, 0.0f, siguienteVector.z - positionVector.z );
	angulo        = CalcularAngulo( velocityVector );
	anguloDestino = CalcularAngulo( destino );
	anguloi       = n_rad2deg( angulo );
	angulod       = n_rad2deg( anguloDestino );
	aux1.set( velocityVector.x, 0.0f, velocityVector.z, 0.0f );

	if ( anguloi < angulod )
		anguloi += 360;

	incAngulo = 1.5f;
	if ( distancia2ACero.len_squared() < 1.5f )
		incAngulo = 5*1/distancia2ACero.len_squared()*distancia2ACero.len_squared();

	if ( anguloi - angulod < 2.0 )
		aux1.set_rotate_y( anguloDestino - angulo );
	else if ( anguloi - angulod < 180 )
		aux1.set_rotate_y( n_deg2rad( -incAngulo ) );
	else
		aux1.set_rotate_y( n_deg2rad( incAngulo ) );

	velocityVector = aux1.rotate( velocityVector );
	rotateVector.y = CalcularAngulo( -velocityVector );
	velocityVector.norm();
	velocityVector.set( velocityVector.x*velocidadAndar, 0, velocityVector.z*velocidadAndar );
	OrientarSombra();
}


/********************************************************************************/
/* Método      : Parar														    */
/* Descripción : Se le asigna al personaje la animación correspondiente a parar	*/
/*				 y se le deja el vector de velocidad a cero.					*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::Parar()
{
	// Animación del modelo
	if ( GetAnimacion() != ANIM_QUIETO )
		SetAnimacion( ANIM_QUIETO );

	// Cálculo de velocidad y pitch
	velocityVector.set( 0, 0, 0 );
	destinoVector = positionVector;
	estado = QUIETO;
}


/********************************************************************************/
/* Método      : CalcularRutaAEstrella										    */
/* Descripción : Evalúa el algoritmo A*											*/
/* Parámetros  : float x (ENT). Valor de la coordenada x.						*/
/*				 float z (ENT). Valor de la coordenada z.						*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::CalcularRutaAEstrella( float x, float z )
{
	nodoActual = NULL;
	mapa->BuscarRuta( positionVector.x, positionVector.z, x, z );
	_asNode *nodo = mapa->path->GetBestNode();
	nodoActual = 0;
	if ( nodo != NULL )
	{
		camino[nodoActual].posx = nodo->x;
		camino[nodoActual].posy = nodo->y;
		while ( nodo->parent != NULL )
		{
			nodo = mapa->path->GetParent( nodo );
			nodoActual++;
			camino[nodoActual].posx = nodo->x;
			camino[nodoActual].posy = nodo->y;
		}
		SiguienteNodo( x, z );
	}
	destinoVector.set( x, 0, z );
}


/********************************************************************************/
/* Método      : SiguienteNodo												    */
/* Descripción : Evalúa el algoritmo A*											*/
/* Parámetros  : float x (ENT). Valor de la coordenada x.						*/
/*				 float z (ENT). Valor de la coordenada z.						*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::SiguienteNodo( float x, float z )
{
	if ( nodoActual != 0 )
	{
		siguienteVector.set( (float) (camino[nodoActual].posx/NUM_DIVISIONES), 0.0f, (float) (camino[nodoActual].posy/NUM_DIVISIONES) );
		nodoActual--;
	}
	else
		siguienteVector.set( x, 0, z );
}


/********************************************************************************/
/* Método      : IrA														    */
/* Descripción : Acción de mover al personaje a un destino con algoritmo A*		*/
/* Parámetros  : bool actualiza (ENT). Es necesario modificar los vectores.		*/
/*				 float x (ENT). Valor de la coordenada x.						*/
/*				 float z (ENT). Valor de la coordenada z.						*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::IrA( bool actualiza, float x, float z )
{
	// FASE DE INICIALIZACIÓN
	// Inicializamos los datos del camino

	if ( actualiza )
	{
		if ( mapa == NULL )
			mapa = (nMapa *) kernelServer->Lookup( "/data/mapa" );

		if ( !mapa->Movimiento_Valido( (int) positionVector.x, (int) positionVector.z, floor(x), floor(z) ) )
		{
			destinoVector = positionVector;
			return;
		}

		if ( (int) x == (int) positionVector.x && (int) z == (int) positionVector.z )
		{
			destinoVector.set( x, 0, z );
			siguienteVector = destinoVector;
		}
		else
			CalcularRutaAEstrella( x, z );
	}

	vector3 distancia1ACero = vector3( siguienteVector.x, 0.0f, siguienteVector.z ) - vector3( positionVector.x, 0.0f, positionVector.z );
	vector3 distancia2ACero = vector3( destinoVector.x, 0.0f, destinoVector.z ) - vector3(positionVector.x, 0.0f, positionVector.z );

	if ( distancia1ACero.len_squared() < 3.0f && distancia2ACero.len_squared() > 0.005f )
		SiguienteNodo( x, z );

	// FASE DE COMPROBACIÓN
	// Aqui comprobamos si el personaje tiene algo delante dentro de un radio

	if ( distancia2ACero.len_squared() < 0.005f )
		estado = QUIETO;
	else
		estado = BUSQUEDA;

	// FASE DE ACCIÓN
	// Aquí es donde movemos al personaje según el estado que tenga
	switch ( estado )
	{
		case BUSQUEDA:
			Correr();
			break;
		case QUIETO:
			Parar();
			break;
		case MUERTO:
			velocityVector.set( 0, 0, 0 );
			destinoVector = positionVector;
			break;
	}
}


/********************************************************************************/
/* Método      : IrARapido													    */
/* Descripción : Acción de mover al personaje a un destino sin algoritmo A*		*/
/* Parámetros  : bool actualiza (ENT). Es necesario modificar los vectores.		*/
/*				 float x (ENT). Valor de la coordenada x.						*/
/*				 float z (ENT). Valor de la coordenada z.						*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::IrARapido( bool actualiza, float x, float z )
{
	// FASE DE INICIALIZACIÓN
	// Inicializamos los datos del camino

	if ( actualiza )
	{
		if ( mapa == NULL )
			mapa = (nMapa *) kernelServer->Lookup( "/data/mapa" );
		if ( !mapa->Movimiento_Valido( (int) positionVector.x, (int) positionVector.z, (int) x, (int) z ) )
		{
			destinoVector = positionVector;
			return;
		}
		destinoVector.set( x, 0, z );
		siguienteVector = destinoVector;
	}

	vector3 distancia1ACero = vector3( siguienteVector.x, 0.0f, siguienteVector.z ) - vector3( positionVector.x, 0.0f, positionVector.z );
	vector3 distancia2ACero = vector3( destinoVector.x, 0.0f, destinoVector.z ) - vector3( positionVector.x, 0.0f, positionVector.z );

	// FASE DE COMPROBACIÓN
	// Aqui comprobamos si el personaje tiene algo delante dentro de un radio

	if ( distancia2ACero.len_squared() < 1.0f )
		estado = QUIETO;
	else
		estado = BUSQUEDA;

	// FASE DE ACCIÓN
	// Aquí es donde movemos al personaje según el estado que tenga
	switch ( estado )
	{
		case BUSQUEDA:
			Mover();
			break;
		case QUIETO:
			Parar();
			break;
		case MUERTO:
			velocityVector.set( 0, 0, 0 );
			destinoVector = positionVector;
			break;
	}
}


/********************************************************************************/
/* Método      : IrAEdificio												    */
/* Descripción : Acción de mover al personaje a un edificio con algoritmo A*	*/
/* Parámetros  : bool actualiza (ENT). Es necesario modificar los vectores.		*/
/*				 nEntity edificio (ENT). Edificio a donde ir.					*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::IrAEdificio( bool actualiza, nEntity *edificio )
{
	float x = edificio->GetPosition().x;
	float z = edificio->GetPosition().z;

	// FASE DE INICIALIZACIÓN
	// Inicializamos los datos del camino

	if ( actualiza )
	{
		entidadDestino = edificio;
		if ( mapa == NULL )
			mapa = (nMapa *) kernelServer->Lookup( "/data/mapa" );

		if ( (int) x == (int) positionVector.x && (int) z == (int) positionVector.z )
		{
			destinoVector.set( x, 0, z );
			siguienteVector = destinoVector;
		}
		else
			CalcularRutaAEstrella( x, z );
	}

	vector3 distancia1ACero = vector3( siguienteVector.x, 0.0f, siguienteVector.z ) - vector3( positionVector.x, 0.0f, positionVector.z );
	vector3 distancia2ACero = vector3( destinoVector.x, 0.0f, destinoVector.z ) - vector3( positionVector.x, 0.0f, positionVector.z );

	if ( distancia1ACero.len_squared() < 3.0f && distancia2ACero.len_squared() > 0.3f )
		SiguienteNodo( x, z );

	// FASE DE COMPROBACIÓN
	// Aqui comprobamos si el personaje tiene algo delante dentro de un radio
	if ( edificio->EstaEnRango( positionVector ) )
	{
		rotateVector.y = CalcularAngulo( -distancia2ACero );
		estado = QUIETO;
	}
	else
		estado = BUSQUEDA_EDIFICIO;

	// FASE DE ACCIÓN
	// Aquí es donde movemos al personaje según el estado que tenga
	switch ( estado )
	{
		case BUSQUEDA_EDIFICIO:
			Correr();
			break;
		case QUIETO:
			Parar();
			break;
		case MUERTO:
			velocityVector.set( 0, 0, 0 );
			destinoVector = positionVector;
			break;
	}
}


/********************************************************************************/
/* Método      : SeguirA													    */
/* Descripción : Acción de seguir a una entidad sin algoritmo A*				*/
/* Parámetros  : bool actualiza (ENT). Es necesario modificar los vectores.		*/
/*				 nEntity entidadACazar (ENT). Entidad a la que seguir.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::SeguirA( bool actualiza, nEntity *entidadACazar )
{
	vector3 velocidadDestino = entidadACazar->GetVelocity();
	vector3 posicionDestino  = entidadACazar->GetPosition();

	// Hacemos un cálculo predictivo de la posición, a menos que estemos cercad del destino
	if ( vector3( positionVector - posicionDestino ).len() > 5.0f )
		posicionDestino += velocidadDestino;
	else
		posicionDestino += velocidadDestino*0.1f;

	float x = posicionDestino.x;
	float z = posicionDestino.z;

	// FASE DE INICIALIZACIÓN
	// Inicializamos los datos del camino

	destinoVector.set( x, 0, z );
	siguienteVector = destinoVector;

	if ( actualiza )
	{
		entidadDestino = entidadACazar;
		if ( mapa == NULL )
			mapa = (nMapa *) kernelServer->Lookup( "/data/mapa" );
	}

	vector3 distancia3 = entidadACazar->GetPosition() - positionVector;

	// FASE DE COMPROBACIÓN
	// Aqui comprobamos si el personaje tiene algo delante dentro de un radio

	if ( entidadACazar->EstaEnRango( positionVector ) )
	{
		rotateVector.y = CalcularAngulo( -distancia3 );
		estado = QUIETO;
	}
	else
		estado = SEGUIMIENTO;

	// FASE DE ACCIÓN
	// Aquí es donde movemos al personaje según el estado que tenga

	switch ( estado )
	{
		case SEGUIMIENTO:
			Correr();
			break;
		case QUIETO:
			Parar();
			break;
	}
}


/********************************************************************************/
/* Método      : SeguirAJugador												    */
/* Descripción : Acción de seguir a un jugador sin algoritmo A*					*/
/* Parámetros  : bool actualiza (ENT). Es necesario modificar los vectores.		*/
/*				 nJugador jugadorACazar (ENT). Jugador al que seguir.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::SeguirAJugador( bool actualiza, nJugador *jugadorACazar )
{
	nEntity *entidadACazar   = jugadorACazar->GetPersonaje();
	vector3 velocidadDestino = entidadACazar->GetVelocity();
	vector3 posicionDestino  = entidadACazar->GetPosition();

	// Hacemos un cálculo predictivo de la posición, a menos que estemos cercad del destino
	if ( vector3( positionVector - posicionDestino ).len() > 5.0f )
		posicionDestino += velocidadDestino;
	else
		posicionDestino += velocidadDestino*0.1f;

	float x = posicionDestino.x;
	float z = posicionDestino.z;

	// FASE DE INICIALIZACIÓN
	// Inicializamos los datos del camino

	destinoVector.set( x, 0, z );
	siguienteVector = destinoVector;

	if ( actualiza )
	{
		jugadorDestino = jugadorACazar;
		if ( mapa == NULL )
			mapa = (nMapa *) kernelServer->Lookup( "/data/mapa" );
	}

	vector3 distancia3 = entidadACazar->GetPosition() - positionVector;

	// FASE DE COMPROBACIÓN
	// Aqui comprobamos si el personaje tiene algo delante dentro de un radio

	if ( entidadACazar->EstaEnRango( positionVector ) )
	{
		rotateVector.y = CalcularAngulo( -distancia3 );
		estado = QUIETO;
	}
	else
		estado = SEGUIMIENTO_JUGADOR;

	// FASE DE ACCIÓN
	// Aquí es donde movemos al personaje según el estado que tenga

	switch ( estado )
	{
		case SEGUIMIENTO_JUGADOR:
			Correr();
			break;
		case QUIETO:
			Parar();
			break;
		case MUERTO:
			velocityVector.set( 0, 0, 0 );
			destinoVector = positionVector;
			break;
	}
}


/********************************************************************************/
/* Método      : SeguirDeLejos												    */
/* Descripción : Acción de seguir de lejos a una entidad sin algoritmo A*		*/
/* Parámetros  : bool actualiza (ENT). Es necesario modificar los vectores.		*/
/*				 nEntity entidadACazar (ENT). Entidad a la que seguir.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::SeguirDeLejos( bool actualiza, nEntity *entidadACazar )
{
	float x = entidadACazar->GetPosition().x;
	float z = entidadACazar->GetPosition().z;

	// FASE DE INICIALIZACIÓN
	// Inicializamos los datos del camino

	destinoVector.set( x, 0, z );
	siguienteVector = destinoVector;

	if ( actualiza )
	{
		entidadDestino = entidadACazar;
		if ( mapa == NULL )
			mapa = (nMapa *) kernelServer->Lookup( "/data/mapa" );

		if ( !mapa->Movimiento_Valido( (int) positionVector.x, (int) positionVector.z, (int) x, (int) z ) )
		{
			destinoVector = positionVector;
			return;
		}
	}

	// FASE DE COMPROBACIÓN
	// Aqui comprobamos si el personaje tiene algo delante dentro de un radio

	nPersonaje *personajeCazado = (nPersonaje *) entidadACazar;
	if ( personajeCazado->EstaEnRangoDeSeguimientoLejano( positionVector ) )
		estado = QUIETO;
	else if ( personajeCazado->EstaEnRangoDePersecucion( positionVector ) )
		estado = SEGUIMIENTO;

	// FASE DE ACCIÓN
	// Aquí es donde movemos al personaje según el estado que tenga

	switch ( estado )
	{
		case SEGUIMIENTO:
			Correr();
			break;
		case QUIETO:
			Parar();
			break;
		case MUERTO:
			velocityVector.set( 0, 0, 0 );
			destinoVector = positionVector;
			break;
	}
}


/********************************************************************************/
/* Método      : Tick														    */
/* Descripción : Actualiza la lógica del personaje.								*/
/* Parámetros  : float dt (ENT). Tiempo transcurrido desde el último tick.		*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::Tick( float dt )
{
	Trigger( dt );
    ComputeForces();
	Integrate( dt );

	switch ( estado )
	{
		case BUSQUEDA:
			IrA( false, destinoVector.x, destinoVector.z );
			break;
		case BUSQUEDA_EDIFICIO:
			IrAEdificio( false, entidadDestino );
			break;
		case SEGUIMIENTO:
			SeguirA( false, entidadDestino );
			break;
		case SEGUIMIENTO_JUGADOR:
			SeguirAJugador( false, jugadorDestino );
			break;
		case MUERTO:
			velocityVector.set( 0, 0, 0 );
			destinoVector = positionVector;
			break;
	}

	ActualizarAnimaciones();
    Update();
}


/********************************************************************************/
/* Método      : AnimacionTerminada											    */
/* Descripción : Comprueba la animación ha concluido.							*/
/* Parámetros  : 																*/
/* Devuelve    : TRUE si la animación ha terminado.								*/
/*				 FALSE si no ha terminado.										*/
/********************************************************************************/
bool nPersonaje::AnimacionTerminada()
{
	bool terminada = o_modelo->HasAnimationFinished();
	terminada |= GetAnimacion() == TAnim::ANIM_ANDAR;
	terminada |= GetAnimacion() == TAnim::ANIM_CORRER;
	terminada |= GetAnimacion() == TAnim::ANIM_QUIETO;

	return terminada;
}


/********************************************************************************/
/* Método      : ActualizarAnimaciones										    */
/* Descripción : Actualiza la lista de animaciones.								*/
/* Parámetros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::ActualizarAnimaciones()
{
	if ( NumSecuencias > 0 )
	{
		if ( o_modelo->HasAnimationFinished() )
		{
			NumSecuencias--;
			o_modelo->SetAnimation( lista_animaciones.At( NumSecuencias ) );
			nGame *juego = (nGame *) kernelServer->Lookup( "/game" );

			// Sonidos de las animaciones
			if ( manager != NULL )
			{
				switch ( lista_animaciones.At( NumSecuencias ) )
				{
					case 14:
						juego->Play3DSound( 0, "sounds:posicional/construir.wav", GetPosition() );
						break;
					case 23:
						juego->Play3DSound( 0, "sounds:posicional/golpe.wav", GetPosition() );
						break;
					case 24:
						juego->Play3DSound( 0, "sounds:posicional/golpe.wav", GetPosition() );
						break;
				}
			}

			lista_animaciones.EraseQuick( NumSecuencias );

			if ( !Muerto && lista_animaciones.Size() == 0 && manager != NULL )
			{
				if ( manager->EstaBloqueado() )
				{
					//n_printf("Desbloqueamos por fin de animación al jugador %d\n", manager->GetId());
					manager->Desbloquear();
				}
			}
		}
	}
}


/********************************************************************************/
/* Método      : SetVisNode													    */
/* Descripción : Asigna un nodo visual al personaje.							*/
/* Parámetros  : const char visnode_path (ENT).	Ruta del nodo visual.			*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPersonaje::SetVisNode( const char *visnode_path )
{
	char modelo_path[N_MAXPATH];
    n_assert( visnode_path );
	visNode = visnode_path;
	refVisNode = visNode.Get();
	sprintf( modelo_path, "%s/mdl", GetVisNode() );
	o_modelo = (nMLoader *) kernelServer->Lookup( modelo_path );
}


/********************************************************************************/
/* Método      : EstaEnRangoDeAccion										    */
/* Descripción : Comprueba si el destino pasado por parámetro está en el rango	*/
/* 				 de acción del personaje.										*/
/* Parámetros  : vector3 p_posicion (ENT). posición destino.					*/
/* Devuelve    : TRUE si está en el rango de acción.							*/
/*				 FALSE si no lo está.											*/
/********************************************************************************/
bool nPersonaje::EstaEnRangoDeAccion( vector3 p_posicion )
{
    vector3 v_distancia;
	p_posicion.y  = 0.0f;
	v_distancia   = GetPosition();
	v_distancia.y = 0.0f;
	v_distancia   = p_posicion - v_distancia;
	return v_distancia.len() < DISTANCIA_DE_ACCION;
}


/********************************************************************************/
/* Método      : EstaEnRango												    */
/* Descripción : Comprueba si el destino pasado por parámetro está en el rango	*/
/* 				 de alcance del personaje.										*/
/* Parámetros  : vector3 p_posicion (ENT). posición destino.					*/
/* Devuelve    : TRUE si está en el rango de alcance.							*/
/*				 FALSE si no lo está.											*/
/********************************************************************************/
bool nPersonaje::EstaEnRango( vector3 p_posicion )
{
    vector3 v_distancia;
	p_posicion.y  = 0.0f;
	v_distancia   = GetPosition();
	v_distancia.y = 0.0f;
	v_distancia   = p_posicion - v_distancia;
	return v_distancia.len() < DISTANCIA_DE_ALCANCE;
}


/********************************************************************************/
/* Método      : EstaEnRangoDeSeguimientoLejano								    */
/* Descripción : Comprueba si el destino pasado por parámetro está en el rango	*/
/* 				 de seguimiento del personaje.									*/
/* Parámetros  : vector3 p_posicion (ENT). posición destino.					*/
/* Devuelve    : TRUE si está en el rango de seguimiento.						*/
/*				 FALSE si no lo está.											*/
/********************************************************************************/
bool nPersonaje::EstaEnRangoDeSeguimientoLejano( vector3 p_posicion )
{
    vector3 v_distancia;
	p_posicion.y  = 0.0f;
	v_distancia   = GetPosition();
	v_distancia.y = 0.0f;
	v_distancia   = p_posicion - v_distancia;
	return v_distancia.len() < DISTANCIA_DE_SEGUIMIENTO;
}


/********************************************************************************/
/* Método      : EstaEnRangoDePersecucion									    */
/* Descripción : Comprueba si el destino pasado por parámetro está en el rango	*/
/* 				 de persecucion del personaje.									*/
/* Parámetros  : vector3 p_posicion (ENT). posición destino.					*/
/* Devuelve    : TRUE si está fuera del rango de seguimiento más un margen.		*/
/*				 FALSE si no lo está.											*/
/********************************************************************************/
bool nPersonaje::EstaEnRangoDePersecucion( vector3 p_posicion )
{
    vector3 v_distancia;
	p_posicion.y  = 0.0f;
	v_distancia   = GetPosition();
	v_distancia.y = 0.0f;
	v_distancia   = p_posicion - v_distancia;
	return v_distancia.len() > DISTANCIA_DE_SEGUIMIENTO + 0.5;
}


int nPersonaje::Robar( nEntity* &personaje_a_robar )
{
	//n_printf( "EL %s NO PUEDE ROBAR\n", GetName() );
	return 0;
}


void nPersonaje::Matar( nEntity* &personaje_a_matar )
{
}


void nPersonaje::ResucitarCampesino( int id_campesino )
{
}


void nPersonaje::DestruirEdificio( nConstruccion* &construccion )
{
}


nArray<int> nPersonaje::GetListaCampesinosContratados()
{
	return lista_campesinos;
}


bool nPersonaje::CampesinoMio( int id_campesino )
{
	for ( int i = 0; i < (int) lista_campesinos.Size(); i++ )
	{
		if ( lista_campesinos.At( i ) == id_campesino )
			return true;
	}
	return false;
}


void nPersonaje::SetJugador( nJugador* jugador )
{
	manager = jugador;
}


nJugador* nPersonaje::GetJugador()
{
	return manager;
}


void nPersonaje::Morir()
{
	Muerto = true;
	estado = MUERTO;
	SetAnimacion( ANIM_MORIR );
}


void nPersonaje::Nacer()
{
	nCampesino* campesino = NULL;
	char        path[N_MAXPATH];

	Muerto = false;
	estado = QUIETO;
	SetAnimacion( ANIM_RESUCITAR );

	if ( GetCampesinosContratados() > 0 )
	{
		for ( int i = 0; i < GetCampesinosContratados(); i++ )
		{
			int id_campesino = GetListaCampesinosContratados().At( i );
			sprintf( path, "/game/world/campesino%d", id_campesino );
			campesino = (nCampesino *) kernelServer->Lookup( path );
			LiberarCampesino( id_campesino );
			campesino->Liberar();
		}
	}
}


void nPersonaje::Revivir()
{
	Muerto = false;
}


bool nPersonaje::EstaMuerto()
{
	return Muerto;
}


const char *nPersonaje::GetNombre() const
{
	return Nombre.Get();
}


float nPersonaje::GetTiempo()
{
	return Tiempo;
}


float nPersonaje::GetTiempoPago()
{
	return TiempoPago;
}


int nPersonaje::GetCampesinosContratados()
{
	return CampesinosContratados;
}


nPersonaje::TAnim nPersonaje::GetAnimacion()
{
	return Animacion;
}


nPersonaje::TPersonaje nPersonaje::GetTipoPersonaje()
{
	return TipoPersonaje;
}


nPersonaje::Comportamiento nPersonaje::GetComportamiento()
{
	return estado;
}