//---------------------------------------------------------------------------
// MDLBoneAnimation.h
//---------------------------------------------------------------------------

/**
 * @file MDLBoneAnimation.h
 * Contiene la declaraci�n de la clase que almacena la informaci�n de una
 * animaci�n de un hueso.
 * @see HalfLifeMLD::MDLBoneAnimation
 * @author Pedro Pablo G�mez Mart�n
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
 * Clase que almacena la informaci�n sobre la animaci�n de un hueso en un
 * s�lo grado de libertad.
 * <p>
 * �sta es una clase "de alto nivel" usada para almacenar las animaciones
 * de un modelo. En realidad el usuario de la librer�a nunca la utilizar�,
 * porque sus detalles son abstraidos por MDLInstance. En cualquier caso,
 * esta clase est� directamente relacionada con la estructura
 * HalfLifeMDL::animvalue_t.
 *
 * @see HalfLifeMDL::animvalue_t
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 * @todo Enlace a la p�gina general de la "arquitectura".
 */
class MDLBoneAnimation {
public:

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
	MDLBoneAnimation(anim_t* anims, unsigned int numFrames,
										const MDLBone* bone);

	/**
	 * Destructor. Libera la memoria.
	 */
	~MDLBoneAnimation();

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
	void CalcBonePosition(unsigned int frame, float fraction,
							MDLPoint& position,
							int xController = 0, int yController = 0,
							int zController = 0, float weight = 1.0) const;

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
	void CalcBoneQuaternion(unsigned int frame, float fraction,
							MDLQuaternion& quaternion,
							int rxController = 0, int ryController = 0,
							int rzController = 0, float weight = 1.0) const;

protected:

	/**
	 * Animaciones del hueso en cada grado de libertad. En lugar de �ndices
	 * num�ricos para acceder a las posiciones, se usan los valores del
	 * enumerado HalfLifeMDL::DoF_t
	 */
	MDLDoFAnimation* _anims[6];

	/**
	 * Hueso al que pertenece esta animaci�n.
	 */
	const MDLBone* _bone;

private:

	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLBoneAnimation(const MDLBoneAnimation& source) {
		assert(!"Constructor copia MDLBoneAnimation no disponible"); }

	/**
	 * Operador de asignaci�n privado para evitar problemas en los
	 * delete. Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLBoneAnimation& operator=(const MDLBoneAnimation &source) {
		assert(!"Operador asignaci�n en MDLBoneAnimation no disponible"); 
		return *this;}

}; // class MDLBoneAnimation

} // namespace HalfLifeMDL

#endif
