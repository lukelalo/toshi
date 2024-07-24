//---------------------------------------------------------------------------
// MDLAttachment.h
//---------------------------------------------------------------------------

/**
 * @file MDLAttachment.h
 * Contiene la declaraci�n de la clase que almacena la informaci�n sobre
 * un punto de uni�n del modelo con otro objeto, como un arma.
 * @see HalfLifeMDL::MDLAttachment
 * @author Pedro Pablo G�mez Mart�n
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
 * Clase que almacena la informaci�n de un "punto de uni�n" del modelo con
 * un objeto del entorno, como por ejemplo un arma.
 * <p>
 * Cada modelo puede tener hasta cuatro puntos de uni�n. Cada uno se
 * crea relativo a un hueso del modelo, proporcionando el desplazamiento
 * del punto de uni�n respecto al hueso del que depende.
 * <p>
 * �sta es una clase "de alto nivel" a la que el usuario del m�dulo tiene 
 * acceso. Est� directamente relacionada con la estructura
 * HalfLifeMDL::attachment_t utilizada en el fichero MDL.
 * @see HalfLifeMDL::attachment_t
 * @todo Enlace a la p�gina general de la "arquitectura".
 */
class MDLAttachment {
public:

	/**
	 * Constructor de un objeto de la clase.
	 * @param attach Puntero a la informaci�n del punto de uni�n, tal
	 * y como se almacena en el fichero MDL. Se hace copia de lo
	 * necesario; el puntero debe ser liberado por el cliente.
	 * @param bones Huesos del modelo ya inicializados. No se hace
	 * copia, se usa memoria compartida. S�lo se utilizar� uno
	 * de ellos, y no se liberar� en el destructor. Los huesos no deben
	 * liberarse hasta que no se haya destruido el objeto creado aqu�.
	 * Si no, getBone devolver�a un puntero a memoria ya liberada.
	 */
	MDLAttachment(const attachment_t* attach, const MDLBone** bones) :
				_offset(attach->org[0], attach->org[1], attach->org[2]) {
				_indexBone = attach->bone;
				_bone = bones[_indexBone]; }

	/**
	 * Devuelve el �ndice del hueso dentro del modelo al que est�
	 * asociado este punto de uni�n.
	 * @return �ndice del hueso del que depende el punto de uni�n.
	 */
	unsigned int getIndexBone() const { return _indexBone; }

	/**
	 * Devuelve el hueso del modelo al que est� asociado este punto de
	 * uni�n.
	 * @return Hueso del que depende el punto de uni�n.
	 */
	const MDLBone* getBone() const { return _bone; }

	/**
	 * Devuelve el desplazamiento de este punto de uni�n respecto al
	 * hueso del modelo del que depende.
	 * @return Desplazamiento del punto de uni�n.
	 */
	MDLPoint getOffset() const { return _offset; }

protected:

	/**
	 * �ndice del hueso del modelo del que depende este punto de
	 * uni�n.
	 */
	unsigned int _indexBone;

	/**
	 * Puntero al hueso del que depende el punto de uni�n. No
	 * se libera en el destructor.
	 */
	const MDLBone* _bone;

	/**
	 * Desplazamiento del punto de uni�n respecto al hueso del que depende.
	 */
	MDLPoint _offset;

}; // class MDLAttachment

} // namespace HalfLifeMDL

#endif
