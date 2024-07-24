//---------------------------------------------------------------------------
// MDLDoFAnimaction.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLDoFAnimation.cpp
 * Contiene la definici�n (implementaci�n) de la clase que almacena la
 * animaci�n de un hueso en un s�lo grado de libertad.
 * @see HalfLifeMDL::MDLDoFAnimation
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 */

#include <memory.h>

#include "MDLDoFAnimation.h"

namespace HalfLifeMDL {

/**
 * Constructor de los objetos de la clase.
 * @param anim Informaci�n de la animaci�n para el grado de
 * libertad. Es un array de animvalue_t, comprimido del mismo modo
 * que el usado en el formato usado en el fichero MDL.
 * @param numFrames N�mero de fotogramas de la animaci�n.
 */
MDLDoFAnimation::MDLDoFAnimation(animvalue_t* anim, unsigned int numFrames) {

	assert(anim);

	// Lo primero que tenemos que hacer es calcular el tama�o del array anim
	// para pedir memoria suficiente donde copiarlo.
	// Para eso lo recorremos una vez conocidos el n�mero de frames.
	animvalue_t* ptr = anim;
	unsigned int cont = numFrames;

	// ptr apunta a un elemento que act�a como "cabecera" de la lista
	// enlazada con la compresi�n.
	while(ptr->num.total < cont) {
		// Todav�a no hemos llegado. Avanzamos
		cont -= ptr->num.total;
		ptr += ptr->num.valid + 1;
	}
	// Estamos en el �ltimo lapso. Usamos resta de punteros para saber
	// cuanto hemos avanzado.
	cont = ptr - anim;
	// Sumamos el hueco del �ltimo lapso.
	cont += ptr->num.valid + 1;

	// En cont tenemos el n�mero de elementos del array.
	_anim = new animvalue_t[cont];

	// Copiamos el original.
	memcpy(_anim, anim, cont*sizeof(animvalue_t));

} // Constructor

//---------------------------------------------------------------------------

/**
 * Destructor. Libera la memoria.
 */
MDLDoFAnimation::~MDLDoFAnimation() {

	if (_anim != NULL)
		delete[] _anim;

} // Destructor

//---------------------------------------------------------------------------

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
bool MDLDoFAnimation::getValue(unsigned int frame,
							   int& value, int &valueNextFrame) const {

	animvalue_t* span;

	unsigned int count;

	count = frame;
	span = _anim;

	// Desplazamos span hasta que apunte a la cabecera del bloque
	// donde est� el fotograma buscado.
	while (span->num.total <= count) {
		count -= span->num.total;
		span += span->num.valid + 1;
	}

	// Tenemos span apuntando a la cabecera del lapso donde est�
	// la informaci�n del fotograma, y en count el n�mero de fotograma
	// relativo al lapso de span.
	// Hay que devolver el valro para count y para count+1. Tenemos
	// cuatro casos:
	if (span->num.valid > count) {
		// La informaci�n del fotograma solicitado est� en la zona
		// inicial, es decir no est� "comprimido".
		value = span[count + 1].value;
		if (span->num.valid > count + 1) {
			// El fotograma siguiente tampoco est� en la parte
			// "comprimida".
			valueNextFrame = span[count + 2].value;
			return false;
		}
		else {
			// El fotograma siguiente s� est� en la parte comprimida.
			valueNextFrame = span[count + 1].value;
			return true;
		}
	}
	else {
		// El fotograma solicitado est� en la parte "comprimida".
		value = span[span->num.valid].value;
		if (span->num.total <= count + 1) {
			// El fotograma siguiente est� en el lapso siguiente, no en
			// el actual.
			valueNextFrame = span[span->num.valid + 2].value;
			return false;
		}
		else {
			valueNextFrame = span[span->num.valid].value;
			return true;
		}

	}

} // getValue

} // namespace HalfLifeMDL

