//---------------------------------------------------------------------------
// MDLBoneAnimation.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLBoneAnimation.cpp
 * Contiene la definición (implementación) de la clase que almacena la
 * animación de un hueso.
 * @see HalfLifeMDL::MDLBoneAnimation
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#include <memory.h>

#include "MDLBoneAnimation.h"

namespace HalfLifeMDL {

/**
 * Constructor de los objetos de la clase.
 * @param anims Información de las animaciones del hueso a partir
 * de los que construir el objeto. Está en el formato de los
 * ficheros de Half Life. Los valores apuntados por el array del
 * campo deben estar disponibles. Es decir, el puntero proporcionado
 * debe ser la zona de memoria donde se ha leído la estructura de
 * un fichero <code>MDL</code>, y debe estar todo el fichero alojado
 * en memoria de forma consecutiva.
 * @param numFrames Número de fotogramas de la animación.
 * @param bone Hueso al que pertenece la animación. Se usa para
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
	} // for que recorría los 6 grados de libertad.

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
 * Calcula la posición del hueso según la animación para un determinado
 * fotograma.<p>
 * @param frame Fotograma para el que se quiere obtener la posición. Debe
 * ser un fotograma válido, pero no se comprobará que lo es. Si es un
 * número de fotograma incorrecto, el resultado será indefinido.
 * @param fraction Parte del fotograma actual que ha transcurrido. Se usa
 * para interpolar el fotograma actual con el siguiente. Si el fotograma
 * actual se guarda como un flotante, este parámetro recibe únicamente la
 * parte decimal de ese flotante.
 * @param position Posición del hueso en el fotograma. Es un parámetro
 * de entrada / salida. En este método, se calcula una nueva posición en
 * función de la animación y el fotograma solicitado en los dos parámetros
 * anteriores. La posición resultado se mezcla con la pasada en este
 * parámetro utilizando el peso especificado en el parámetro weight, y el
 * resultado de la mezcla se vuelve a depositar en este parámetro, de ahí
 * que sea de entrada/salida.
 * @param xController Valor del controlador X para el hueso. Su valor
 * por defecto es 0.
 * @param yController Valor del controlador Y para el hueso. Su valor
 * por defecto es 0.
 * @param zController Valor del controlador Z para el hueso. Su valor
 * por defecto es 0.
 * @param weight Peso de la posición obtenidao de esta animación que hay
 * que utilizar para mezclarlo con la posición pasada en el parámetro
 * position. Si se pasa el valor 1.0 en este parámetro, en position
 * se despositará la posición calculada, sin hacer uso del valor que
 * tuviera originalmente. Los valores admitidos son los reales entre
 * 0.0 y 1.0. No se comprueba que el valor pasado esté en ese rango. Si
 * no se hace así, el resultado será indeterminado.
 * @todo Cuando llegue el momento, contar un poco más lo de los
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
			// Hay animación en este grado de libertad. Hallamos la
			// modificación que hay que realizar al valor por defecto
			// en función del fotograma.
			// Obtenemos los valores de la animación para el fotograma
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
	} // for que recorría todas las coordenadas

	// Modificamos la posición a partir de los controladores.
	result[0] += xController;
	result[1] += yController;
	result[2] += zController;

	if (weight == 1.0)
		// No hay mezcla. Nos ahorramos los cálculos de la interpolación.
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
 * Calcula el cuaternión del hueso según la animación para un determinado
 * fotograma.<p>
 * @param frame Fotograma para el que se quiere obtener la posición. Debe
 * ser un fotograma válido, pero no se comprobará que lo es. Si es un
 * número de fotograma incorrecto, el resultado será indefinido.
 * @param fraction Parte del fotograma actual que ha transcurrido. Se usa
 * para interpolar el fotograma actual con el siguiente. Si el fotograma
 * actual se guarda como un flotante, este parámetro recibe únicamente la
 * parte decimal de ese flotante.
 * @param quaternion Cuaterión del hueso en el fotograma. Es un parámetro
 * de entrada / salida. En este método, se calcula un nuevo cuaternión en
 * función de la animación y del fotograma solicitado en los dos
 * parámetros anteriores. El cuaternión resultado se mezcla con el
 * cuaternión pasado en este parámetro utilizando el peso especificado
 * en el parámetro weight, y la mezcla se vuelve a depositar en este
 * parámetro, de ahí que sea de entrada/salida.
 * @param rxController Valor del controlador de rotación X para el
 * hueso. Su valor por defecto es 0.
 * @param ryController Valor del controlador de rotación X para el
 * hueso. Su valor por defecto es 0.
 * @param rzController Valor del controlador de rotación X para el
 * hueso. Su valor por defecto es 0.
 * @param weight Peso del cuaternión obtenido de esta animación que hay
 * que utilizar para mezclarlo con el cuaternión pasado en el parámetro
 * quaternion. Si se pasa el valor 1.0 en este parámetro, en quaternion
 * se despositará el cuaternión calculado, sin hacer uso del valor que
 * tuviera originalmente. Los valores admitidos son los reales entre
 * 0.0 y 1.0. No se comprueba que el valor pasado esté en ese rango. Si
 * no se hace así, el resultado será indeterminado.
 * @todo Cuando llegue el momento, contar un poco más lo de los
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
			// No hay animación para este grado de libertad.
			// Ponemos los valores por defecto.
			// El estado de equals no cambia
			angle1[coord] = angle2[coord] =
							_bone->getDefaultValue((DoF_t) (coord + 3));
		else {
			// Hay animación, tenemos que mirar los valores para
			// el fotograma actual y el siguiente, e interpolar
			if (_anims[coord + 3]->getValue(frame, valueFrame, valueNextFrame)
				|| (valueFrame == valueNextFrame)) {
				// El valor del fotograma actual y del siguiente son el
				// mismo. En la condición aprovechamos el orden de
				// ejecución. Si getValue devuelve cierto, entonces es que
				// seguro que valueGrame es igual que valueNextFrame, y
				// la segunda comprobación no se ejecutará.
				// Si no, tal vez getValue se haya equivocado. En este caso
				// nos compensa hacer la comprobación a nosotros para evitar
				// cálculos en algunos casos, de ahí que también se compruebe.
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
			} // if-else que miraba si el ángulo cambiaba entre el
			  // fotograma actual y el siguiente
		} // if-else que miraba si había animación en la coordenada coord.

	} // for que recorría todas las coordenadas

	// En angle1 tenemos los ángulos sobre los ejes x, y y z para el
	// fotograma actual, y en angle2 tenemos lo mismo para el fotograma
	// siguiente. Los modificamos añadiendo la información de los
	// controladores.
	angle1[0] += rxController;		angle2[0] += rxController;
	angle1[1] += rxController;		angle2[1] += rxController;
	angle1[2] += rxController;		angle2[2] += rxController;

	// En la variable booleana equals tenemos si angle1 y angle2 son
	// iguales. Si no lo son, tendremos que interpolar; si lo son,
	// el cuaternión lo podemos sacar directamente de cualquiera de ellos.
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
		// No hay mezcla. Nos evitamos los cálculos de la interpolación
		// esférica.
		quaternion = result;
	else
		// Tenemos que mezclar. weight es el peso para la mezcla
		// viejo-nuevo. Es decir si vale 1, habrá que dejar el valor
		// calculado. Si vale 0 hay que dejar el antiguo. Eso es
		// justo lo que hace slerp, siendo el "viejo" el this y
		// el nuevo el del parámetro.
		quaternion = quaternion.slerp(result, weight);

} // CalcBoneQuaterion

} // namespace HalfLifeMDL

