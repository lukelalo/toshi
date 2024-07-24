//---------------------------------------------------------------------------
// MDLEventListener.h
//---------------------------------------------------------------------------

/**
 * @file MDLEventListener.h
 * Contiene la declaración de la clase abstracta que deben implementar las
 * clases que quieran ser notificadas de eventos en las secuencias de un
 * modelo.
 * @see HalfLifeMDL::MDLEventListener
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLEventListener
#define __HalfLifeMDL_MDLEventListener

#include "MDLEvent.h"

namespace HalfLifeMDL {

class MDLInstance;

/**
 * Clase abstracta que deben implementar las clases que quieran ser
 * notificadas de eventos en las secuencias de un modelo. Para añadir
 * una clase a la lista de oyentes de una instancia se debe usar
 * el método MDLInstance::addEventListener
 * @see HalfLifeMDL::MDLEvent
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
class MDL_PUBLIC MDLEventListener {
public:

	/**
	 * Método llamado para avisar de un evento en una secuencia al objeto
	 * interesado.
	 * @param instance Modelo que ha generado el evento.
	 * @param sequence Índice de la secuencia que ha generado el evento.
	 * @param event Evento que se ha generado.
	 */
	virtual void eventRaised(const MDLInstance* instance,
							unsigned int sequence, const MDLEvent* event) = 0;

}; // class MDLEventListener

} // namespace HalfLifeMDL

#endif
