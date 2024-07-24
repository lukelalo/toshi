//---------------------------------------------------------------------------
// MDLDoFAnimation.h
//---------------------------------------------------------------------------

/**
 * @file MDLDoFAnimation.h
 * Contiene la declaración de la clase que almacena la información de una
 * animación de un hueso en un grado de libertad.
 * @see HalfLifeMLD::MDLDoFAnimation
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLDoFAnimation
#define __HalfLifeMDL_MDLDoFAnimation

#include <string>
#include <assert.h>

#include "MDLBasic.h"

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
 * @todo Mejora en la implementación: usar una lista con punto de interés.
 */
class MDLDoFAnimation {
public:

	/**
	 * Constructor de los objetos de la clase.
	 * @param anim Información de la animación para el grado de
	 * libertad. Es un array de animvalue_t, comprimido del mismo modo
	 * que el usado en el formato usado en el fichero MDL.
	 * @param numFrames Número de fotogramas de la animación.
	 */
	MDLDoFAnimation(animvalue_t* anim, unsigned int numFrames);

	/**
	 * Destructor. Libera la memoria.
	 */
	~MDLDoFAnimation();

	/**
	 * Devuelve el valor del grado de libertad de la animación almacenada
	 * en la clase para el fotograma solicitado y para el siguiente.
	 * @param frame Número de fotograma cuyo valor para el grado de libertad
	 * guardado por el objeto se desea obtener. No se comprueba que este
	 * valor está dentro del número de fotogramas de la animación, por lo
	 * que si el valor es incorrecto el resultado será indeterminado.
	 * @param value Valor de la animación para el fotograma solicitado
	 * (parámetro de salida).
	 * @param valueNextFrame Valor de la animación para el fotograma
	 * siguiente al solicitado (parámetro de salida).
	 * @return Devuelve cierto si los dos valores devueltos son iguales.
	 * En ese caso ambos valores se habrán inicializado. El resultado puede
	 * usarse para evitar la interpolación entre ambos valores. En realidad
	 * puede darse el caso de que se devuelva falso y ambos sean iguales. Lo
	 * que nunca ocurrirá será que se devuelva cierto cuando no lo sean.
	 */
	bool getValue(unsigned int frame, int& value, int &valueNextFrame) const;

protected:

	/**
	 * Array con la información de la animación del grado de libertad del
	 * hueso que contiene el objeto. Está comprimido del mismo modo que
	 * en el fichero MDL de Half Life.
	 */
	animvalue_t* _anim;

private:

	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementación es vacía y no debe usarse.
	 */
	MDLDoFAnimation(const MDLDoFAnimation& source) {
		assert(!"Constructor copia MDLDoFAnimation no disponible"); }

	/**
	 * Operador de asignación privado para evitar problemas en los
	 * delete. Su implementación es vacía y no debe usarse.
	 */
	MDLDoFAnimation& operator=(const MDLDoFAnimation &source) {
		assert(!"Operador asignación en MDLDoFAnimation no disponible"); 
		return *this;}

}; // class MDLDoFAnimation

} // namespace HalfLifeMDL

#endif
