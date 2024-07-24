//---------------------------------------------------------------------------
// MDLDoFAnimaction.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLDoFAnimation.cpp
 * Contiene la definición (implementación) de la clase que almacena la
 * animación de un hueso en un sólo grado de libertad.
 * @see HalfLifeMDL::MDLDoFAnimation
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#include <memory.h>

#include "MDLDoFAnimation.h"

namespace HalfLifeMDL {

/**
 * Constructor de los objetos de la clase.
 * @param anim Información de la animación para el grado de
 * libertad. Es un array de animvalue_t, comprimido del mismo modo
 * que el usado en el formato usado en el fichero MDL.
 * @param numFrames Número de fotogramas de la animación.
 */
MDLDoFAnimation::MDLDoFAnimation(animvalue_t* anim, unsigned int numFrames) {

	assert(anim);

	// Lo primero que tenemos que hacer es calcular el tamaño del array anim
	// para pedir memoria suficiente donde copiarlo.
	// Para eso lo recorremos una vez conocidos el número de frames.
	animvalue_t* ptr = anim;
	unsigned int cont = numFrames;

	// ptr apunta a un elemento que actúa como "cabecera" de la lista
	// enlazada con la compresión.
	while(ptr->num.total < cont) {
		// Todavía no hemos llegado. Avanzamos
		cont -= ptr->num.total;
		ptr += ptr->num.valid + 1;
	}
	// Estamos en el último lapso. Usamos resta de punteros para saber
	// cuanto hemos avanzado.
	cont = ptr - anim;
	// Sumamos el hueco del último lapso.
	cont += ptr->num.valid + 1;

	// En cont tenemos el número de elementos del array.
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
bool MDLDoFAnimation::getValue(unsigned int frame,
							   int& value, int &valueNextFrame) const {

	animvalue_t* span;

	unsigned int count;

	count = frame;
	span = _anim;

	// Desplazamos span hasta que apunte a la cabecera del bloque
	// donde está el fotograma buscado.
	while (span->num.total <= count) {
		count -= span->num.total;
		span += span->num.valid + 1;
	}

	// Tenemos span apuntando a la cabecera del lapso donde está
	// la información del fotograma, y en count el número de fotograma
	// relativo al lapso de span.
	// Hay que devolver el valro para count y para count+1. Tenemos
	// cuatro casos:
	if (span->num.valid > count) {
		// La información del fotograma solicitado está en la zona
		// inicial, es decir no está "comprimido".
		value = span[count + 1].value;
		if (span->num.valid > count + 1) {
			// El fotograma siguiente tampoco está en la parte
			// "comprimida".
			valueNextFrame = span[count + 2].value;
			return false;
		}
		else {
			// El fotograma siguiente sí está en la parte comprimida.
			valueNextFrame = span[count + 1].value;
			return true;
		}
	}
	else {
		// El fotograma solicitado está en la parte "comprimida".
		value = span[span->num.valid].value;
		if (span->num.total <= count + 1) {
			// El fotograma siguiente está en el lapso siguiente, no en
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

