//---------------------------------------------------------------------------
// MDLEventListener.h
//---------------------------------------------------------------------------

/**
 * @file MDLEventListener.h
 * Contiene la declaraci�n de la clase abstracta que deben implementar las
 * clases que quieran ser notificadas de eventos en las secuencias de un
 * modelo.
 * @see HalfLifeMDL::MDLEventListener
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLEventListener
#define __HalfLifeMDL_MDLEventListener

#include "MDLEvent.h"

namespace HalfLifeMDL {

class MDLInstance;

/**
 * Clase abstracta que deben implementar las clases que quieran ser
 * notificadas de eventos en las secuencias de un modelo. Para a�adir
 * una clase a la lista de oyentes de una instancia se debe usar
 * el m�todo MDLInstance::addEventListener
 * @see HalfLifeMDL::MDLEvent
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
class MDL_PUBLIC MDLEventListener {
public:

	/**
	 * M�todo llamado para avisar de un evento en una secuencia al objeto
	 * interesado.
	 * @param instance Modelo que ha generado el evento.
	 * @param sequence �ndice de la secuencia que ha generado el evento.
	 * @param event Evento que se ha generado.
	 */
	virtual void eventRaised(const MDLInstance* instance,
							unsigned int sequence, const MDLEvent* event) = 0;

}; // class MDLEventListener

} // namespace HalfLifeMDL

#endif
