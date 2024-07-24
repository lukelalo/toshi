//---------------------------------------------------------------------------
// MDLAttachment.h
//---------------------------------------------------------------------------

/**
 * @file MDLAttachment.h
 * Contiene la declaración de la clase que almacena la información sobre
 * un punto de unión del modelo con otro objeto, como un arma.
 * @see HalfLifeMDL::MDLAttachment
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLAttachment
#define __HalfLifeMDL_MDLAttachment

#include <string>
#include "MDLBasic.h"
#include "MDLBone.h"
#include "MDLPoint.h"

namespace HalfLifeMDL {

/**
 * Clase que almacena la información de un "punto de unión" del modelo con
 * un objeto del entorno, como por ejemplo un arma.
 * <p>
 * Cada modelo puede tener hasta cuatro puntos de unión. Cada uno se
 * crea relativo a un hueso del modelo, proporcionando el desplazamiento
 * del punto de unión respecto al hueso del que depende.
 * <p>
 * Ésta es una clase "de alto nivel" a la que el usuario del módulo tiene 
 * acceso. Está directamente relacionada con la estructura
 * HalfLifeMDL::attachment_t utilizada en el fichero MDL.
 * @see HalfLifeMDL::attachment_t
 * @todo Enlace a la página general de la "arquitectura".
 */
class MDLAttachment {
public:

	/**
	 * Constructor de un objeto de la clase.
	 * @param attach Puntero a la información del punto de unión, tal
	 * y como se almacena en el fichero MDL. Se hace copia de lo
	 * necesario; el puntero debe ser liberado por el cliente.
	 * @param bones Huesos del modelo ya inicializados. No se hace
	 * copia, se usa memoria compartida. Sólo se utilizará uno
	 * de ellos, y no se liberará en el destructor. Los huesos no deben
	 * liberarse hasta que no se haya destruido el objeto creado aquí.
	 * Si no, getBone devolvería un puntero a memoria ya liberada.
	 */
	MDLAttachment(const attachment_t* attach, const MDLBone** bones) :
				_offset(attach->org[0], attach->org[1], attach->org[2]) {
				_indexBone = attach->bone;
				_bone = bones[_indexBone]; }

	/**
	 * Devuelve el índice del hueso dentro del modelo al que está
	 * asociado este punto de unión.
	 * @return Índice del hueso del que depende el punto de unión.
	 */
	unsigned int getIndexBone() const { return _indexBone; }

	/**
	 * Devuelve el hueso del modelo al que está asociado este punto de
	 * unión.
	 * @return Hueso del que depende el punto de unión.
	 */
	const MDLBone* getBone() const { return _bone; }

	/**
	 * Devuelve el desplazamiento de este punto de unión respecto al
	 * hueso del modelo del que depende.
	 * @return Desplazamiento del punto de unión.
	 */
	MDLPoint getOffset() const { return _offset; }

protected:

	/**
	 * Índice del hueso del modelo del que depende este punto de
	 * unión.
	 */
	unsigned int _indexBone;

	/**
	 * Puntero al hueso del que depende el punto de unión. No
	 * se libera en el destructor.
	 */
	const MDLBone* _bone;

	/**
	 * Desplazamiento del punto de unión respecto al hueso del que depende.
	 */
	MDLPoint _offset;

}; // class MDLAttachment

} // namespace HalfLifeMDL

#endif
