//---------------------------------------------------------------------------
// MDLBoneAnimation.h
//---------------------------------------------------------------------------

/**
 * @file MDLBoneAnimation.h
 * Contiene la declaración de la clase que almacena la información de una
 * animación de un hueso.
 * @see HalfLifeMLD::MDLBoneAnimation
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLBoneAnimation
#define __HalfLifeMDL_MDLBoneAnimation

#include <string>
#include <assert.h>

#include "MDLBasic.h"
#include "MDLPoint.h"
#include "MDLDoFAnimation.h"
#include "MDLBone.h"
#include "MDLQuaternion.h"

namespace HalfLifeMDL {

/**
 * Clase que almacena la información sobre la animación de un hueso en un
 * sólo grado de libertad.
 * <p>
 * Ésta es una clase "de alto nivel" usada para almacenar las animaciones
 * de un modelo. En realidad el usuario de la librería nunca la utilizará,
 * porque sus detalles son abstraidos por MDLInstance. En cualquier caso,
 * esta clase está directamente relacionada con la estructura
 * HalfLifeMDL::animvalue_t.
 *
 * @see HalfLifeMDL::animvalue_t
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 * @todo Enlace a la página general de la "arquitectura".
 */
class MDLBoneAnimation {
public:

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
	MDLBoneAnimation(anim_t* anims, unsigned int numFrames,
										const MDLBone* bone);

	/**
	 * Destructor. Libera la memoria.
	 */
	~MDLBoneAnimation();

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
	void CalcBonePosition(unsigned int frame, float fraction,
							MDLPoint& position,
							int xController = 0, int yController = 0,
							int zController = 0, float weight = 1.0) const;

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
	void CalcBoneQuaternion(unsigned int frame, float fraction,
							MDLQuaternion& quaternion,
							int rxController = 0, int ryController = 0,
							int rzController = 0, float weight = 1.0) const;

protected:

	/**
	 * Animaciones del hueso en cada grado de libertad. En lugar de índices
	 * numéricos para acceder a las posiciones, se usan los valores del
	 * enumerado HalfLifeMDL::DoF_t
	 */
	MDLDoFAnimation* _anims[6];

	/**
	 * Hueso al que pertenece esta animación.
	 */
	const MDLBone* _bone;

private:

	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementación es vacía y no debe usarse.
	 */
	MDLBoneAnimation(const MDLBoneAnimation& source) {
		assert(!"Constructor copia MDLBoneAnimation no disponible"); }

	/**
	 * Operador de asignación privado para evitar problemas en los
	 * delete. Su implementación es vacía y no debe usarse.
	 */
	MDLBoneAnimation& operator=(const MDLBoneAnimation &source) {
		assert(!"Operador asignación en MDLBoneAnimation no disponible"); 
		return *this;}

}; // class MDLBoneAnimation

} // namespace HalfLifeMDL

#endif
