//---------------------------------------------------------------------------
// MDLEvent.h
//---------------------------------------------------------------------------

/**
 * @file MDLEvent.h
 * Contiene la declaración de la clase que almacena la información sobre
 * un evento de una secuencia del modelo de Half Life.
 * @see HalfLifeMDL::MDLEvent
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLEvent
#define __HalfLifeMDL_MDLEvent

#include <string>

#include "MDLBasic.h"

namespace HalfLifeMDL {

/**
 * Clase que almacena la información sobre un evento en una secuencia del
 * modelo. <p>
 * Una secuencia puede generar eventos en cualquier fotograma para avisar
 * al motor del juego de que una determinada acción debe realizarse
 * (por ejemplo reproducir un sonido, avisar de que el personaje ha
 * muerto, o de que debe generarse una nueva bala para disparar).
 * <p>
 * El gestor de los modelos avisará a quién le interese de los eventos que
 * se generan.
 * Ésta es una clase "de alto nivel" a la que el usuario del módulo tiene
 * acceso. Está directamente relacionada con la estructura
 * HalfLifeMDL::event_t utilizada en el fichero MDL.
 *
 * @see HalfLifeMDL::event_t
 * @see HalfLifeMDL::MDLSequence
 * @see HalfLifeMDL::MDLInstance
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 * @todo Enlace a la página general de la "arquitectura".
 */
class MDLEvent {
public:

	/**
	 * Constructor de los objetos de la clase.
	 * @param infoEvent Estructura con la información del evento tal
	 * y como se ha leído del fichero. Este puntero debe ser
	 * liberado por el usuario (se hace copia).
	 */
	MDLEvent(const event_t* infoEvent) : 
					_frame(infoEvent->frame),
					_eventCode(infoEvent->event),
					_options(infoEvent->options) {}

	/**
	 * Devuelve el número de fotograma donde se ha producido el evento.
	 * @return Número de fotograma del evento.
	 */
	unsigned int getNumFrame() const { return _frame; }

	/**
	 * Devuelve el código del evento. Debe existir una coordinación
	 * entre el creador del MDL (con el QC) y el programador que controla
	 * el modelo para ponerse de acuerdo en el significado de cada
	 * identificador de evento.
	 * <p>
	 * Algunos se pueden considerar "estándar" porque han sido utilizados
	 * en los modelos de Half Life originales. Por ejemplo un valor de 1000
	 * indica que el personaje muere. La lista está en el documento "Modeling
	 * for Half-Life.doc" proporcionado con las SDK de Half Life.
	 * @return Código del evento.
	 */
	int getEventCode() const { return _eventCode; }

	/**
	 * Devuelve los parámetros del evento, como por ejemplo el nombre del
	 * fichero wav a reproducir. El contenido depende del evento, y es el
	 * código asociado al modelo el que le da el sentido a la cadena.
	 */
	const std::string& getOptions() const { return _options; }

protected:

	/**
	 * Número de fotograma donde se genera el evento
	 */
	unsigned int _frame;

	/**
	 * Código del evento. En general, debe existir una coordinación
	 * entre el creador del MDL (con el QC) y el programador que controla
	 * el modelo para ponerse de acuerdo en el significado de cada
	 * identificador de evento.
	 * <p>
	 * Algunos se pueden considerar "estándar" porque han sido utilizados
	 * en los modelos de Half Life originales. Por ejemplo un valor de 1000
	 * indica que el personaje muere. La lista está en el documento "Modeling
	 * for Half-Life.doc" proporcionado con las SDK de Half Life.
	 */
	int _eventCode;

	// ¿¿event??

	/**
	 * Opciones del evento. Por ejemplo, el nombre del fichero wav a
	 * reproducir. El contenido depende del evento, y es el código
	 * asociado al modelo el que le da el sentido a la cadena.
	 */
	std::string _options;

}; // class MDLEvent

} // namespace HalfLifeMDL

#endif
