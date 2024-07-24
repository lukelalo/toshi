//---------------------------------------------------------------------------
// MDLBoneAnimation.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLBoneAnimation.cpp
 * Contiene la definici�n (implementaci�n) de la clase que almacena la
 * animaci�n de un hueso.
 * @see HalfLifeMDL::MDLBoneAnimation
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 */

#include <memory.h>

#include "MDLBoneAnimation.h"

namespace HalfLifeMDL {

/**
 * Constructor de los objetos de la clase.
 * @param anims Informaci�n de las animaciones del hueso a partir
 * de los que construir el objeto. Est� en el formato de los
 * ficheros de Half Life. Los valores apuntados por el array del
 * campo deben estar disponibles. Es decir, el puntero proporcionado
 * debe ser la zona de memoria donde se ha le�do la estructura de
 * un fichero <code>MDL</code>, y debe estar todo el fichero alojado
 * en memoria de forma consecutiva.
 * @param numFrames N�mero de fotogramas de la animaci�n.
 * @param bone Hueso al que pertenece la animaci�n. Se usa para
 * sus valores por defecto.
 */
MDLBoneAnimation::MDLBoneAnimation(anim_t* anims, unsigned int numFrames,
												   const MDLBone* bone) {
	for (unsigned int i = 0; i < 6; i++) {
		// Recorremos todos los grados de libertad.
		if (anims->offset[i] == 0)
			_anims[i] = NULL;
		else {
			animvalue_t* animdof;
			animdof = (animvalue_t*)((char*)anims + anims->offset[i]);
			_anims[i] = new MDLDoFAnimation(animdof, numFrames);
		}
	} // for que recorr�a los 6 grados de libertad.

	_bone = bone;

} // Constructor

//---------------------------------------------------------------------------

/**
 * Destructor. Libera la memoria.
 */
MDLBoneAnimation::~MDLBoneAnimation() {

	for (unsigned int i = 0; i < 6; i++)
		if (_anims[i] != NULL)
			delete _anims[i];

} // destructor

//---------------------------------------------------------------------------

/**
 * Calcula la posici�n del hueso seg�n la animaci�n para un determinado
 * fotograma.<p>
 * @param frame Fotograma para el que se quiere obtener la posici�n. Debe
 * ser un fotograma v�lido, pero no se comprobar� que lo es. Si es un
 * n�mero de fotograma incorrecto, el resultado ser� indefinido.
 * @param fraction Parte del fotograma actual que ha transcurrido. Se usa
 * para interpolar el fotograma actual con el siguiente. Si el fotograma
 * actual se guarda como un flotante, este par�metro recibe �nicamente la
 * parte decimal de ese flotante.
 * @param position Posici�n del hueso en el fotograma. Es un par�metro
 * de entrada / salida. En este m�todo, se calcula una nueva posici�n en
 * funci�n de la animaci�n y el fotograma solicitado en los dos par�metros
 * anteriores. La posici�n resultado se mezcla con la pasada en este
 * par�metro utilizando el peso especificado en el par�metro weight, y el
 * resultado de la mezcla se vuelve a depositar en este par�metro, de ah�
 * que sea de entrada/salida.
 * @param xController Valor del controlador X para el hueso. Su valor
 * por defecto es 0.
 * @param yController Valor del controlador Y para el hueso. Su valor
 * por defecto es 0.
 * @param zController Valor del controlador Z para el hueso. Su valor
 * por defecto es 0.
 * @param weight Peso de la posici�n obtenidao de esta animaci�n que hay
 * que utilizar para mezclarlo con la posici�n pasada en el par�metro
 * position. Si se pasa el valor 1.0 en este par�metro, en position
 * se despositar� la posici�n calculada, sin hacer uso del valor que
 * tuviera originalmente. Los valores admitidos son los reales entre
 * 0.0 y 1.0. No se comprueba que el valor pasado est� en ese rango. Si
 * no se hace as�, el resultado ser� indeterminado.
 * @todo Cuando llegue el momento, contar un poco m�s lo de los
 * controladores... o cambiarlo por completo :-m
 */
void MDLBoneAnimation::CalcBonePosition(unsigned int frame, float fraction,
						MDLPoint& position,
						int xController, int yController,
						int zController, float weight) const {

	MDLPoint result;
	int valueFrame, valueNextFrame;

	assert ((fraction >= 0) && (fraction < 1));

	for (unsigned int coord = 0; coord < 3; coord++) {
		// Recorremos todas las coordenadas.
		// Establecemos como valor inicial el usado por defecto para el hueso
		// en el grado de libertad actual.
		result[coord] = _bone->getDefaultValue((DoF_t)coord);
		if (_anims[coord] != NULL) {
			// Hay animaci�n en este grado de libertad. Hallamos la
			// modificaci�n que hay que realizar al valor por defecto
			// en funci�n del fotograma.
			// Obtenemos los valores de la animaci�n para el fotograma
			// actual.
			if (_anims[coord]->getValue(frame, valueFrame, valueNextFrame))
				// El valor del fotograma actual y del siguiente son el
				// mismo. No hay que interpolar.
				result[coord] += valueFrame * _bone->getScale((DoF_t)coord);
			else {
				// Ambos valores son diferentes. Tenemos que interpolar.
				result[coord] += (valueFrame * (1.0 - fraction) + 
								  valueNextFrame * fraction) * 
											_bone->getScale((DoF_t)coord);
			}
		}
	} // for que recorr�a todas las coordenadas

	// Modificamos la posici�n a partir de los controladores.
	result[0] += xController;
	result[1] += yController;
	result[2] += zController;

	if (weight == 1.0)
		// No hay mezcla. Nos ahorramos los c�lculos de la interpolaci�n.
		position = result;
	else {
		// Hay que hacer algunas cuentas.
		float w1;
		w1 = 1.0 - weight;
		position[0] = w1 * position[0] + weight * result[0];
		position[1] = w1 * position[1] + weight * result[1];
		position[2] = w1 * position[2] + weight * result[2];
	}

} // CalcBonePosition

//---------------------------------------------------------------------------

/**
 * Calcula el cuaterni�n del hueso seg�n la animaci�n para un determinado
 * fotograma.<p>
 * @param frame Fotograma para el que se quiere obtener la posici�n. Debe
 * ser un fotograma v�lido, pero no se comprobar� que lo es. Si es un
 * n�mero de fotograma incorrecto, el resultado ser� indefinido.
 * @param fraction Parte del fotograma actual que ha transcurrido. Se usa
 * para interpolar el fotograma actual con el siguiente. Si el fotograma
 * actual se guarda como un flotante, este par�metro recibe �nicamente la
 * parte decimal de ese flotante.
 * @param quaternion Cuateri�n del hueso en el fotograma. Es un par�metro
 * de entrada / salida. En este m�todo, se calcula un nuevo cuaterni�n en
 * funci�n de la animaci�n y del fotograma solicitado en los dos
 * par�metros anteriores. El cuaterni�n resultado se mezcla con el
 * cuaterni�n pasado en este par�metro utilizando el peso especificado
 * en el par�metro weight, y la mezcla se vuelve a depositar en este
 * par�metro, de ah� que sea de entrada/salida.
 * @param rxController Valor del controlador de rotaci�n X para el
 * hueso. Su valor por defecto es 0.
 * @param ryController Valor del controlador de rotaci�n X para el
 * hueso. Su valor por defecto es 0.
 * @param rzController Valor del controlador de rotaci�n X para el
 * hueso. Su valor por defecto es 0.
 * @param weight Peso del cuaterni�n obtenido de esta animaci�n que hay
 * que utilizar para mezclarlo con el cuaterni�n pasado en el par�metro
 * quaternion. Si se pasa el valor 1.0 en este par�metro, en quaternion
 * se despositar� el cuaterni�n calculado, sin hacer uso del valor que
 * tuviera originalmente. Los valores admitidos son los reales entre
 * 0.0 y 1.0. No se comprueba que el valor pasado est� en ese rango. Si
 * no se hace as�, el resultado ser� indeterminado.
 * @todo Cuando llegue el momento, contar un poco m�s lo de los
 * controladores... o cambiarlo por completo :-m
 */
void MDLBoneAnimation::CalcBoneQuaternion(unsigned int frame, float fraction,
						MDLQuaternion& quaternion,
						int rxController, int ryController,
						int rzController, float weight) const {
	MDLQuaternion result;
	MDLBasicType angle1[3], angle2[3];

	int valueFrame, valueNextFrame;
	bool equals = true;

	assert ((fraction >= 0) && (fraction < 1));

	for (unsigned int coord = 0; coord < 3; coord++) {
		if (_anims[coord + 3] == NULL)
			// No hay animaci�n para este grado de libertad.
			// Ponemos los valores por defecto.
			// El estado de equals no cambia
			angle1[coord] = angle2[coord] =
							_bone->getDefaultValue((DoF_t) (coord + 3));
		else {
			// Hay animaci�n, tenemos que mirar los valores para
			// el fotograma actual y el siguiente, e interpolar
			if (_anims[coord + 3]->getValue(frame, valueFrame, valueNextFrame)
				|| (valueFrame == valueNextFrame)) {
				// El valor del fotograma actual y del siguiente son el
				// mismo. En la condici�n aprovechamos el orden de
				// ejecuci�n. Si getValue devuelve cierto, entonces es que
				// seguro que valueGrame es igual que valueNextFrame, y
				// la segunda comprobaci�n no se ejecutar�.
				// Si no, tal vez getValue se haya equivocado. En este caso
				// nos compensa hacer la comprobaci�n a nosotros para evitar
				// c�lculos en algunos casos, de ah� que tambi�n se compruebe.
				angle1[coord] = _bone->getDefaultValue((DoF_t) (coord + 3)) +
								(float) valueFrame * 
									_bone->getScale((DoF_t) (coord + 3));
				angle2[coord] = angle1[coord];

			}
			else {
				// Los dos valores son diferentes. Nos va a tocar interpolar.
				angle1[coord] = _bone->getDefaultValue((DoF_t) (coord + 3)) +
								(float) valueFrame * 
									_bone->getScale((DoF_t) (coord + 3));
				angle2[coord] = _bone->getDefaultValue((DoF_t) (coord + 3)) +
								(float) valueNextFrame * 
									_bone->getScale((DoF_t) (coord + 3));
				equals = false;
			} // if-else que miraba si el �ngulo cambiaba entre el
			  // fotograma actual y el siguiente
		} // if-else que miraba si hab�a animaci�n en la coordenada coord.

	} // for que recorr�a todas las coordenadas

	// En angle1 tenemos los �ngulos sobre los ejes x, y y z para el
	// fotograma actual, y en angle2 tenemos lo mismo para el fotograma
	// siguiente. Los modificamos a�adiendo la informaci�n de los
	// controladores.
	angle1[0] += rxController;		angle2[0] += rxController;
	angle1[1] += rxController;		angle2[1] += rxController;
	angle1[2] += rxController;		angle2[2] += rxController;

	// En la variable booleana equals tenemos si angle1 y angle2 son
	// iguales. Si no lo son, tendremos que interpolar; si lo son,
	// el cuaterni�n lo podemos sacar directamente de cualquiera de ellos.
	if (equals)
		// Son iguales.
		result.setAngles(angle1[0], angle1[1], angle1[2]);
	else {
		// Son diferentes. A currar.
		MDLQuaternion q1, q2;
		q1.setAngles(angle1[0], angle1[1], angle1[2]);
		q2.setAngles(angle2[0], angle2[1], angle2[2]);
		// Los interpolamos.
		q1.slerp(q2, fraction, result);
	}

	if (weight == 1.0)
		// No hay mezcla. Nos evitamos los c�lculos de la interpolaci�n
		// esf�rica.
		quaternion = result;
	else
		// Tenemos que mezclar. weight es el peso para la mezcla
		// viejo-nuevo. Es decir si vale 1, habr� que dejar el valor
		// calculado. Si vale 0 hay que dejar el antiguo. Eso es
		// justo lo que hace slerp, siendo el "viejo" el this y
		// el nuevo el del par�metro.
		quaternion = quaternion.slerp(result, weight);

} // CalcBoneQuaterion

} // namespace HalfLifeMDL

