//---------------------------------------------------------------------------
// MDLSequence.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLSequence.cpp
 * Contiene la definición (implementación) de la clase que almacena la
 * información sobre una secuencia del modelo de Half Life.
 * @see HalfLifeMDL::MDLSequence
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */

#include "MDLSequence.h"

#include <search.h>	// Para el qsort

namespace HalfLifeMDL {

/**
 * Función de comparación entre dos punteros a MDLEvent.<p>
 * Está hecha para poder ordenar vectores de MDLEvent* usando
 * el QuickSort (librería de C).<p>
 * La comparación la hace utilizando el número de frame
 * de cada evento.
 * @param elem1 Puntero al primer elemento (es decir, al
 * primer MDLEvent*).
 * @param elem2 Puntero al segundo elemento (es decir al
 * segundo MDLEvent*).
 * @return 
 *	- < 0 si elem1 es menor que elem2
 *	- = 0 si elem1 es igual a eleme2
 *	- > 0 si elem1 es mayor que elem2
*/
static int cmpPtrEvents(const void *elem1, const void *elem2) {
	const MDLEvent	**event1 = (const MDLEvent**)elem1;
	const MDLEvent	**event2 = (const MDLEvent**)elem2;

	// Comprobación de NULLs
	if (!*event1)
		if (!*event2)
			return 0;	// iguales.
		else
			return 1;	// elem2 es mayor.
	if (!*event2)
		return -1;		// elem1 es menor.

	// Comprobación de frames.
	int	f1, f2;
	f1 = (*event1)->getNumFrame();
	f2 = (*event2)->getNumFrame();
	if (f1 < f2)
		return -1;
	if (f1 == f2)
		return 0;
	return 1;
}

/**
 * Constructor de los objetos de la clase.
 * @param infoSeq Estructura con la información de la secuencia tal
 * y como se ha leído del fichero. Este puntero debe ser
 * liberado por el usuario (se hace copia).
 * @param events Array con los eventos de la secuencia. Sustituye
 * al campo  seqdesc_t.eventindex del primer parámetro, que es un puntero
 * relativo al origen del fichero, no a memoria. Es responsabilidad
 * del usuario eliminar este puntero. El objeto hace copia de la
 * información apuntada por él.
 * @param numBones número de huesos del modelo al que pertenece la
 * secuencia.
 * @param animations Animaciones de la secuencia. Van ordenadas del
 * mismo modo que en el fichero <code>MDL</code>. Este puntero debe
 * ser liberado por el usuario (se hace copia).
 * @param bones Array con los huesos (ya creados) del modelo. No se
 * realiza copia, ni se libera en el destructor. Los punteros pasados
 * no deben liberarse antes de destruir este objeto (se utiliza memoria
 * compartida).
 */
MDLSequence::MDLSequence(const seqdesc_t* infoSeq, const event_t* events,
				unsigned int numBones, anim_t* animations, 
				const MDLBone** bones) :
				_name(infoSeq->label), _fps(infoSeq->fps),
				_numFrames(infoSeq->numframes),
				_loop(infoSeq->flags & 0x01),
				_type((actionTags_t)infoSeq->activity), _weight(infoSeq->actweight),
				_numEvents(infoSeq->numevents), _numBones(numBones),
				_numAnimations(infoSeq->numblends) {

	MDLBoneAnimation** objAnimPtr;
	anim_t* structAnimPtr;

	assert(animations);

	// Inicializamos los bounding boxes
	for (int coord = 0; coord < 3; coord++) {
		_bbmin[coord] = infoSeq->bbmin[coord];
		_bbmax[coord] = infoSeq->bbmax[coord];
	}

	// Inicializamos los eventos.
	if (_numEvents != 0) {
		// Hay eventos.
		_events = new MDLEvent*[_numEvents];
		for (unsigned int cont = 0; cont < _numEvents; cont++)
			_events[cont] = new MDLEvent(&events[cont]);
		qsort(_events, _numEvents, sizeof(MDLEvent*), cmpPtrEvents);
	} // if (_numEvents != 0)

	_animations = new MDLBoneAnimation*[_numAnimations*_numBones];
	objAnimPtr = _animations;
	structAnimPtr = animations;
	for (unsigned int anim = 0; anim < _numAnimations; anim++) {
		for (unsigned int bone = 0; bone < _numBones; bone++) {
			*objAnimPtr = new MDLBoneAnimation(structAnimPtr,
											_numFrames, bones[bone]);
			++objAnimPtr;
			++structAnimPtr;
		}
	}

	// Miramos el tipo de desplazamiento de la secuencia.
	motionType_t mt = (motionType_t) infoSeq->motiontype;

	if ((mt & MT_LX) == MT_LX) {
		// El campo infoSeq->linearmovement tiene el
		// desplazamiento total, que debe aplicarse a lo largo
		// de toda la secuencia. Para saber el desplazamiento
		// en un segundo hay que calcularlo.
		_linearMovement.x() = (float) _fps / this->_numFrames *
									infoSeq->linearmovement[0];
		_motionType = MT_LINEAR;
	}
	if ((mt & MT_LY) == MT_LY) {
		_linearMovement.y() = (float) _fps / this->_numFrames *
									infoSeq->linearmovement[1];
		_motionType = MT_LINEAR;
	}
	if ((mt & MT_LZ) == MT_LZ) {
		_linearMovement.z() = (float) _fps / this->_numFrames *
									infoSeq->linearmovement[2];
		_motionType = MT_LINEAR;
	}
	
} // Constructor

//---------------------------------------------------------------------------

/**
 * Destructor del objeto. Libera la memoria de los eventos.
 */
MDLSequence::~MDLSequence() {

	if (_numEvents != NULL)
		delete[] _events;
	if (_animations != NULL) {
	}

} // Destructor

} // namespace HalfLifeMDL

