#define N_IMPLEMENTS nCamera
//------------------------------------------------------------------------------
//  (C) 2002	ling
//------------------------------------------------------------------------------
#include "node/n3dnode.h"
#include "toshi/nentity.h"
#include "toshi/nworld.h"
#include "toshi/ncamera.h"

nNebulaScriptClass(nCamera, "nroot");

// Temporary hack
const vector3 upVector(0.0f, 1.0f, 0.0f);

//------------------------------------------------------------------------------
/**
*/
nCamera::nCamera() :
    transformMatrix(),
    world(kernelServer, this),
    cameraNode(kernelServer, this),
    targetEntity(kernelServer, this),
	cameraStyle(nCamera::TOTAL_STATIONARY),
    targetPosition(),
	camaraActual(NULL)
{
    targetPosition.set(0, 5, 0);

    transformMatrix.ident();
    transformMatrix.translate(targetPosition);
    transformMatrix.lookat(vector3(1, 0, 1), upVector);

	giroDerecha = 0;
	distanciaCamara=5.0f;
	alturaCamara=3.0f;

	posicionEstatica.set(-3, 3, -1);
	destinoEstatico.set(0, 0, -1);
	

}

//------------------------------------------------------------------------------

/**
*/
nCamera::~nCamera()
{
}

//------------------------------------------------------------------------------

/**
    Update the camera, move it around and stuff.
*/
void nCamera::Update(double dt)
{
    if (false == targetEntity.isvalid() || false == world.isvalid())
        return;

	if (camaraActual == NULL)
		return;

	vector3 nuevaPosicion;
	vector3 nuevoDestino;

	vector3 movimientoDeseado;
	CAbstractCamera* nuevaCamara;
	movimientoDeseado.set(giroDerecha, 0, 0);

	nuevaPosicion = camaraActual->getNuevaPos(posActual,
									destinoActual, movimientoDeseado, distanciaCamara, alturaCamara);
	// TODO: movimiento de la cámara... aquí pasamos el mismo movimiento
	// pedido supuestamente para el origen en el cálculo del destino...
	nuevoDestino = camaraActual->getNuevoDestino(posActual,
									destinoActual, movimientoDeseado);
	posActual = nuevaPosicion;
	destinoActual = nuevoDestino;
	nuevaCamara = camaraActual->getNuevaCamara();
	if (nuevaCamara != NULL) {
		n_free(camaraActual);
		camaraActual = nuevaCamara;
	}

	// Colocamos la cámara.
	transformMatrix.M41 = posActual.x;
	transformMatrix.M42 = posActual.y;
	transformMatrix.M43 = posActual.z;

	// ¿¿TODO: upVector de la cámara?? Además, ahora mismo upVector es
	// una variable global :-/
	transformMatrix.lookat(destinoActual, upVector);

	// Cambiamos el nodo de la cámara para el "listener" del sonido.
	if (true == cameraNode.isvalid())
		cameraNode->M(transformMatrix);

	return;
}

/**
    @brief Obtain current camera transform.
*/
const matrix44& nCamera::GetTransform() const
{
    return transformMatrix;
}

/**
    @brief Set position of camera.
*/
void nCamera::SetPosition(float x, float y, float z)
{
    transformMatrix.M41 = x;
    transformMatrix.M42 = y;
    transformMatrix.M43 = z;

	posActual.x = x;
	posActual.y = y;
	posActual.z = z;

}

/**
*/
void nCamera::SetTargetPosition(float x, float y, float z)
{
    targetPosition.set(x, y, z);
}

/**
    @brief Set the world.
*/
void nCamera::SetWorld(const char* world_path)
{
    n_assert(NULL != world_path);
    world = world_path;
}

/**
    @brief Sets the camera node.
*/
void nCamera::SetNode(const char* node_path)
{
    n_assert(NULL != node_path);
    cameraNode = node_path;
}

/**
    @brief Set the target entity to track.
*/
void nCamera::SetTarget(const char* entity_path)
{
    n_assert(NULL != entity_path);
    targetEntity = entity_path;

	if (camaraActual != NULL)
		n_free(camaraActual);
	camaraActual = new CTrackingCamera(targetEntity.get());

	// Colocamos la posición a huevo de la cámara.
	destinoActual = targetEntity->GetPosition();
	posActual = destinoActual;
	posActual.x -= 10;

}

/**
    @brief Get the tracked entity.
*/
nEntity* nCamera::GetTarget()
{
    return targetEntity.get();
}

void
nCamera::SetStyle(nCamera::Style style)
{
	switch (style) {
	case TOTAL_STATIONARY: {
		CAbstractCamera* camaraDestino;
		camaraDestino = new CStaticCamera(posicionEstatica, destinoEstatico);
		if (camaraActual != NULL)
			n_free(camaraActual);
		camaraActual = new CTransitionCamera(camaraDestino,
							posicionEstatica, destinoEstatico);
		break;
	}
	case CHASE:
	default: {	// TODO: resto de cámaras??
		CAbstractCamera* camaraDestino;
		vector3 inicioCamara, destinoCamara, diferencia;
		float distancia;
		camaraDestino = new CTrackingCamera(targetEntity.get());
		// Calculamos la posición inicial (simulada) de la cámara
		// que sigue a la entidad.
		// La colocamos en la posición más cercana a la cámara actual
		// que esté lo suficiente lejos de la entidad.
		diferencia = targetEntity->GetPosition() - posActual;
		distancia = diferencia.len();
		diferencia.norm();
		inicioCamara = posActual + diferencia * (distancia-5.0f);
		//inicioCamara.z+=distanciaActual;
		//n_printf("Distancia actual: %f\n",distanciaActual);

		destinoCamara = targetEntity->GetPosition();

		if (camaraActual != NULL)
			n_free(camaraActual);
		camaraActual = new CTransitionCamera(camaraDestino, inicioCamara, destinoCamara);
		break;
	} // case CHASE
	}

	cameraStyle = style;

}


//------------------------------------------------------------------------------
//	                             CTrackingCamera
//------------------------------------------------------------------------------

CTrackingCamera::CTrackingCamera(nEntity* entidad) {

	this->targetEntity = entidad;

} // constructor

//------------------------------------------------------------------------------

vector3 CTrackingCamera::getNuevaPos(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento,
							float &distanciaCamara,
							float &alturaCamara) {

	// FUNCIONAMIENTO:
	// La intención es estar siempre a una distancia máxima de la entidad
	// destino, sin preocuparnos (demasiado) de si le miramos desde
	// delante, desde detrás, o desde donde sea.
	// De todas formas, queremos permitir cierta "versatilidad" en la
	// distancia al personaje, es decir, queremos dejarle que se aleje
	// levemente, o no vamos a asustarnos si nos acercamos un poco.
	// Total, el funcionamiento básico es calcular el vector entre la
	// posición actual de la cámara y la entidad, calcular su longitud,
	// y si es demasiado, avanzar hasta colocarnos a la distancia deseada.
	// En realidad no recorremos todo el espacio, sólo un porcentaje. De
	// ese modo damos un poco de margen para que la entidad se nos aleje,
	// que además dependerá de la velocidad a la que se mueva.

	// Con esa misma técnica también nos separamos de la entidad si ésta
	// se nos acerca.

	// En realidad, antes de hacer lo anterior, miramos si hay que
	// girar. El usuario puede solicitar un giro hacia la izquierda o derecha
	// para ver otro lado del personaje. Esa información la recibimos en el
	// último parámetro. Si eso ocurre, "desplazamos la cámara" antes de
	// nada, sin preocuparnos de si nos quedamos lejos o no. La técnica de
	// acercamiento/alejamiento que se ejecuta después nos colocará en la
	// posición correcta.

	vector3 origenInicial;
	float distanciaDeseada = distanciaCamara;	 // TODO: pasarlo a atributo?
	float distanciaActual;
	float avanceNecesario;	// Para llegar a la posición "correcta"
	vector3 diferencia;
	vector3 posicionEntidad;
	vector3 resultado;

	origenInicial = origenActual;

	vector3 iCamara, jCamara, kCamara;	// Vectores directores de
										// los ejes de la cámara.

	kCamara = destinoActual - origenActual;
	kCamara.y = 0;
	kCamara.norm();
	jCamara = upVector;	// TODO: esto es una constante global...
	iCamara = jCamara * kCamara;
	jCamara = kCamara * iCamara;

	origenInicial.x -= iCamara.x * desplazamiento.x +
						jCamara.x * desplazamiento.y +
						kCamara.x * desplazamiento.z;
	origenInicial.y -= iCamara.y * desplazamiento.x +
						jCamara.y * desplazamiento.y +
						kCamara.y * desplazamiento.z;
	origenInicial.z -= iCamara.z * desplazamiento.x +
						jCamara.z * desplazamiento.y +
						kCamara.z * desplazamiento.z;

	resultado = origenInicial;	// Mientras no se demuestre lo contrario, no
								// nos movemos.

	// TODO: colisiones!

	posicionEntidad = targetEntity->GetPosition();
	diferencia = posicionEntidad - origenInicial;
	distanciaActual = diferencia.len();
	avanceNecesario = distanciaActual - distanciaDeseada;
	// ¡avanceNecesario podría ser negativo!

	diferencia.norm();	// Lo pasamos a unitario.

	// Calculamos cuanto hay que avanzar realmente.
	diferencia *= avanceNecesario * 1.0f;//0.5f;//1.0f;
	resultado.x += diferencia.x;
	resultado.z += diferencia.z;

	// TODO: la altura se hace "a mano"
	resultado.y += ((posicionEntidad.y+alturaCamara) - resultado.y) * 0.1f;

	return resultado;

} // CTrackingCamera::getNuevaPos

//------------------------------------------------------------------------------

vector3 CTrackingCamera::getNuevoDestino(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento) {

	// Siempre miramos al mismo sitio...
	return targetEntity->GetPosition();

} // CTrackingCamera::getNuevoDestino

//------------------------------------------------------------------------------
//	                             CStaticCamera
//------------------------------------------------------------------------------

CStaticCamera::CStaticCamera(vector3 origen, vector3 destino) : 
								_origen(origen), _destino(destino) {
} // CStaticCamera::constructor

//------------------------------------------------------------------------------

vector3 CStaticCamera::getNuevaPos(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento,
							float &distanciaCamara,
							float &alturaCamara) {

	return _origen;

} // CStaticCamera::getNuevaPos

//------------------------------------------------------------------------------

vector3 CStaticCamera::getNuevoDestino(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento) {

	return _destino;

} // CStaticCamera::getNuevoDestino

//------------------------------------------------------------------------------
//	                             CTransitionCamera
//------------------------------------------------------------------------------

CTransitionCamera::CTransitionCamera(CAbstractCamera *camaraPerseguida,
									 vector3 origen, vector3 destino) :
			_camaraPerseguida(camaraPerseguida),
			_origen(origen), _destino(destino), _camaraAlcanzada(false) {

} // CTransitionCamera::constructor

//------------------------------------------------------------------------------

vector3 CTransitionCamera::getNuevaPos(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento,
							float &distanciaCamara,
							float &alturaCamara) {

	vector3 nuevoOrigenPerseguido, nuevoDestinoPerseguido;

	nuevoOrigenPerseguido = _camaraPerseguida->getNuevaPos(_origen,
								_destino, desplazamiento, distanciaCamara, alturaCamara);
	nuevoDestinoPerseguido = _camaraPerseguida->getNuevoDestino(_origen,
								_destino, desplazamiento);

	_origen = nuevoOrigenPerseguido;
	_destino = nuevoDestinoPerseguido;

	// Ya tenemos las posiciones hacia las que vamos. Calculamos la
	// distancia desde donde estamos ahora mismo hasta las posiciones
	// destino. Haremos las cuentas con la más larga (la de la fuente
	// o la del destino de la cámara). Calculamos el número de pasos que
	// necesitamos

	vector3 diferenciaOrigen;
	vector3 diferenciaDestino;
	vector3 resultado;
	float distanciaOrigen;
	float distanciaDestino;
	float pasos;	// Número de pasos hasta llegar al destino a la
					// máxima velocidad.
	float velMax;	// velocidad máxima TODO: atributo de la clase.
	velMax = 5.0;

	diferenciaOrigen = _origen - origenActual;
	diferenciaDestino = _destino - destinoActual;

	distanciaOrigen = diferenciaOrigen.len();
	distanciaDestino = diferenciaDestino.len();

	// TODO: pasos se mide en segundos (suponiendo que la velocidad también
	// ;-))... por tanto el desplazamiento real debería depender del tiempo
	// transcurrido...
	// TODO: parece que da un salto en la transición a la otra cámara :-m
	// Comprobar que informa del cambio cuando debe.
	if (distanciaOrigen > distanciaDestino) {
		pasos = distanciaOrigen / velMax;
		diferenciaOrigen.norm();
		resultado = origenActual + diferenciaOrigen * velMax;
		_nuevoDestino = destinoActual + diferenciaDestino / pasos;
	}
	else {
		pasos = distanciaDestino / velMax;
		diferenciaDestino.norm();
		resultado = origenActual + diferenciaOrigen / pasos;
		_nuevoDestino = destinoActual + diferenciaDestino * velMax;
	}

	_camaraAlcanzada = (pasos < 1);

	return resultado;

} // CTransitionCamera::getNuevaPos

//------------------------------------------------------------------------------

vector3 CTransitionCamera::getNuevoDestino(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento) {

	return _nuevoDestino;

} // CTransitionCamera::getNuevoDestino

//------------------------------------------------------------------------------

CAbstractCamera* CTransitionCamera::getNuevaCamara() {

	if (_camaraAlcanzada)
		return _camaraPerseguida;
	else
		return NULL;

} // CTransitionCamera::getNuevaCamara
