//---------------------------------------------------------------------------
// MDLDoFAnimation.h
//---------------------------------------------------------------------------

/**
 * @file MDLDoFAnimation.h
 * Contiene la declaraci�n de la clase que almacena la informaci�n de una
 * animaci�n de un hueso en un grado de libertad.
 * @see HalfLifeMLD::MDLDoFAnimation
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLDoFAnimation
#define __HalfLifeMDL_MDLDoFAnimation

#include <string>
#include <assert.h>

#include "MDLBasic.h"

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
 * @todo Mejora en la implementaci�n: usar una lista con punto de inter�s.
 */
class MDLDoFAnimation {
public:

	/**
	 * Constructor de los objetos de la clase.
	 * @param anim Informaci�n de la animaci�n para el grado de
	 * libertad. Es un array de animvalue_t, comprimido del mismo modo
	 * que el usado en el formato usado en el fichero MDL.
	 * @param numFrames N�mero de fotogramas de la animaci�n.
	 */
	MDLDoFAnimation(animvalue_t* anim, unsigned int numFrames);

	/**
	 * Destructor. Libera la memoria.
	 */
	~MDLDoFAnimation();

	/**
	 * Devuelve el valor del grado de libertad de la animaci�n almacenada
	 * en la clase para el fotograma solicitado y para el siguiente.
	 * @param frame N�mero de fotograma cuyo valor para el grado de libertad
	 * guardado por el objeto se desea obtener. No se comprueba que este
	 * valor est� dentro del n�mero de fotogramas de la animaci�n, por lo
	 * que si el valor es incorrecto el resultado ser� indeterminado.
	 * @param value Valor de la animaci�n para el fotograma solicitado
	 * (par�metro de salida).
	 * @param valueNextFrame Valor de la animaci�n para el fotograma
	 * siguiente al solicitado (par�metro de salida).
	 * @return Devuelve cierto si los dos valores devueltos son iguales.
	 * En ese caso ambos valores se habr�n inicializado. El resultado puede
	 * usarse para evitar la interpolaci�n entre ambos valores. En realidad
	 * puede darse el caso de que se devuelva falso y ambos sean iguales. Lo
	 * que nunca ocurrir� ser� que se devuelva cierto cuando no lo sean.
	 */
	bool getValue(unsigned int frame, int& value, int &valueNextFrame) const;

protected:

	/**
	 * Array con la informaci�n de la animaci�n del grado de libertad del
	 * hueso que contiene el objeto. Est� comprimido del mismo modo que
	 * en el fichero MDL de Half Life.
	 */
	animvalue_t* _anim;

private:

	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLDoFAnimation(const MDLDoFAnimation& source) {
		assert(!"Constructor copia MDLDoFAnimation no disponible"); }

	/**
	 * Operador de asignaci�n privado para evitar problemas en los
	 * delete. Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLDoFAnimation& operator=(const MDLDoFAnimation &source) {
		assert(!"Operador asignaci�n en MDLDoFAnimation no disponible"); 
		return *this;}

}; // class MDLDoFAnimation

} // namespace HalfLifeMDL

#endif
