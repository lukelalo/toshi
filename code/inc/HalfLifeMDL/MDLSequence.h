//---------------------------------------------------------------------------
// MDLSequence.h
//---------------------------------------------------------------------------

/**
 * @file MDLSequence.h
 * Contiene la declaración de la clase que almacena la información sobre
 * una secuencia del modelo de Half Life.
 * @see HalfLifeMDL::MDLSequence
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */

#ifndef __HalfLifeMDL_MDLSequence
#define __HalfLifeMDL_MDLSequence

#include <string>
#include <assert.h>

#include "MDLBasic.h"
#include "MDLEvent.h"
#include "MDLBoneAnimation.h"

namespace HalfLifeMDL {

/**
 * Clase que almacena la información sobre una secuencia del modelo.
 * Por secuencia se entiende la información sobre el número de fotogramas
 * por segundo de reproducción de la animación, el avance (si existe) del
 * modelo por el mundo, información sobre eventos, etcétera.
 * <p>
 * Ésta es una clase "de alto nivel" a la que el usuario del módulo tiene
 * acceso. Está directamente relacionada con la estructura
 * HalfLifeMDL::seqdesc_t utilizada en el fichero MDL.
 * @see HalfLifeMDL::seqdesc_t
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 * @todo Enlace a la página general de la "arquitectura".
 */
class MDLSequence {
public:

	/**
	 * Tipo de datos que especifica los tipos de movimientos sobre el
	 * mundo que el personaje puede tener cuando se ejecuta esta secuencia.
	 * <br>
	 * Tiene relación con HalfLifeMDL::motionType_t, aunque no es
	 * completamente directa.
	 */
	enum MotionType {
		/**
		 * No hay movimiento absoluto del modelo.
		 */
		MT_NONE = 0,
		/**
		 * Hay desplazamiento constante y lineal a lo largo de toda la
		 * reproducción de la secuencia. El desplazamiento puede obtenerse
		 * con MDLSequence::getLinearMovement.
		 */
		MT_LINEAR
	};

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
	MDLSequence(const seqdesc_t* infoSeq, const event_t* events,
				unsigned int numBones, anim_t* animations, 
				const MDLBone** bones);

	/**
	 * Destructor del objeto. Libera la memoria de los eventos.
	 */
	~MDLSequence();

	/**
	 * Devuelve el nombre de la secuencia.
	 * @return Nombre de la secuencia tal y como se especificó en
	 * el QC al crear el MDL.
	 */
	const std::string& getName() const { return _name; }

	/**
	 * Devuelve el número de fotogramas por segundo necesarios para
	 * la reproducción de la secuencia.
	 * @return Velocidad de refresco de la secuencia.
	 */
	float getFPS() const { return _fps; }

	/**
	 * Devuelve el número de fotogramas de la secuencia.
	 * @return Número de fotogramas.
	 */
	unsigned int getNumFrames() const { return _numFrames; }

	/**
	 * Informa sobre si la secuencia es o no un ciclo.
	 * @return
	 *			- Cierto si la secuencia es un ciclo.
	 *			- Falso si no lo es.
	 */
	bool isCycle() const { return _loop; }

	/**
	 * Devuelve la constante representando al tipo estándar de la secuencia.
	 * @return Tipo de la secuencia.
	 */
	actionTags_t getType() const { return _type; }

	/**
	 * Devuelve el peso de la secuencia.<p>
	 * Un mismo modelo puede tener varias secuencias para la misma actividad,
	 * y la parte del motor gráfico que se encarga de controlar al modelo
	 * puede establecer aleatoriamente una cualquiera cuando desde fuera
	 * se le pide que establezca una secuencia proprcionando el identificador
	 * de secuencia estándar. Esa elección aleatoria se puede ajustar dando
	 * un peso a cada versión de la secuencia estándar, para dar más
	 * probabilidad a unas que a otras. Ese peso es almacenado en este campo,
	 * que no tendrá sentido si la secuencia no es estándar. Si sólo hay
	 * una secuencia para esta actividad, su valor tampoco tendrá mucho
	 * sentido, pero será diferente de 0.
	 * @return Peso de la secuencia.
	 */
	int getWeight() const { return _weight; }

	/**
	 * Devuelve el número de huesos del modelo al que pertenece esta
	 * secuencia. Este método se proporciona por comodidad en el uso de
	 * MDLSequence::getAnimations(); el valor devuelto será el mismo que el
	 * obtenido por MDLCore::getNumBones con el modelo al que pertenece
	 * la secuencia.
	 * @return Número de huesos del modelo al que pertenece la secuencia.
	 */
	unsigned int getNumBones() const { return _numBones; }

	/**
	 * Devuelve el número de animaciones que se mezclan en esta secuencia
	 * para crear la posición del modelo. La parte de la librería que se
	 * encarga de mezclar las animaciones bajo demanda, no soporta mezclar
	 * más de dos, por lo que este método sólo devolverá 1 ó 2. En cualquier
	 * caso, parece que el formato <code>QC</code> usado como entrada al
	 * empaquetador de <code>MDLs</code> sólo admite también como mucho dos
	 * animaciones por secuencia.
	 * @return Número de animaciones mezcladas en esta secuencia.
	 */
	unsigned int getNumAnimations() const { return _numAnimations; }

	/**
	 * Devuelve la información de las animaciones de los huesos de la
	 * secuencia. Devuelve un array, cuyo tamaño será
	 * getNumBones() * getNumAnimations(). El primer elemento será la
	 * animación del hueso 0 para la animación 0, el siguiente el
	 * hueso 1 para la animación 0, etc. Luego comenzará de nuevo con
	 * el hueso 0 para la animación 2, si existe.
	 * @return Array de animaciones de los huesos para la secuencia. Este
	 * array NO debe liberarse. La gestión de la memoria la realiza el
	 * objeto.
	 */
	const MDLBoneAnimation** getAnimations() const { 
					return (const MDLBoneAnimation**) _animations; }

	/**
	 * Devuelve la información de la animación de un hueso.
	 * @param bone Hueso cuya animación se desea. Empieza a contar desde 0.
	 * @param anim Número de la animación que se desea. Para crear la posición
	 * del hueso de la secuencia puede usarse más de una animación, que se
	 * mezclan utilizando un peso. El número de animaciones usadas en la
	 * secuencia actual se obtiene con getNumAnimations. Este campo
	 * especifica el número de animación (empezando a contar en 0).
	 * @return Animación del hueso solicitado.
	 * @note No se comprueba que los parámetros son correctos (salvo en modo
	 * depuración con assert), por lo que valores fuera de rango generarán
	 * un resultado indefinido.
	 */
	const MDLBoneAnimation* getAnimation(unsigned int bone, 
											unsigned int anim) const {
					assert(bone < _numBones);
					assert(anim < _numAnimations);
					return(_animations[anim*_numAnimations + bone]);	}

	/**
	 * Devuelve el número de eventos de la secuencia. Cada evento especifica
	 * en qué fotograma debe ser generado.
	 * @return Número de eventos de la secuencia.
	 */
	unsigned int getNumEvents() const { return _numEvents; }

	/**
	 * Devuelve el evento iésimo de la secuencia.
	 * @param event Índice del evento que se desea. Debe tener un índice
	 * válido. Si no, el resultado será indeterminado (en modo depuración
	 * se comprueba con assert).
	 * @return Evento iésimo de la secuencia. El puntero no debe liberarse,
	 * la memoria ocupada es controlada por el objeto.
	 */
	const MDLEvent* getEvent(unsigned int event) const {
					assert (event < _numEvents);
					return _events[event]; }

	/**
	 * Devuelve el Bounding Box alineado con los ejes de la secuencia. Este
	 * bounding box es constante en toda la secuencia, aglutinando a todos
	 * los bounding box de cada frame. <br>
	 * Para más información, vease HalfLifeMDL::seqdesc_t::bbmin.
	 * @param bbmin Parámetro de salida con la esquina inferior del bounding
	 * box alineado con los ejes.
	 * @param bbmax Parámetro de salida con la esquina superior del bounding
	 * box alineado con los ejes.
	 * @see HalfLifeMDL::seqdesc_t::bbmin
	 */
	void getBoundingBox(MDLPoint& bbmin, MDLPoint& bbmax) const {
					bbmin = _bbmin; bbmax = _bbmax; }


	/**
	 * Devuelve el tipo de movimiento absoluto (respecto al mundo) del
	 * modelo cuando se reproduce esta secuencia. En función del valor
	 * devuelto, tendrá o no sentido llamar, por ejemplo, a
	 * getLinearMovement.
	 * @return Tipo de movimiento absoluto del modelo en el mundo.
	 */
	MotionType getMotionType() const { return _motionType; }

	/**
	 * Devuelve el desplazamiento lineal en un segundo del modelo. Sólo
	 * tiene sentido si el desplazamiento es lineal (MT_LINEAR).
	 * @return Desplazamiento lineal por segundo del modelo.
	 */
	MDLPoint getLinearMovement() const {
					assert(_motionType == MT_LINEAR);
					return _linearMovement; }

protected:

	/**
	 * Nombre de la secuencia. Se establece en el fichero QC.
	 */
	std::string _name;

	/**
	 * Fotogramas por segundo necesarios para la reproducción de la
	 * secuencia.
	 */
	float _fps;

	/**
	 * Número de fotogramas de la secuencia.
	 */
	unsigned int _numFrames;

	/**
	 * A cierto si la secuencia es un ciclo.
	 */
	bool _loop;

	/**
	 * Tipo de la secuencia, si la secuencia es estándar.
	 */
	actionTags_t _type;

	/**
	 * "Peso" de esta secuencia para el tipo de secuencia _type.<p>
	 * Un mismo modelo puede tener varias secuencias para la misma actividad,
	 * y la parte del motor gráfico que se encarga de controlar al modelo
	 * puede establecer aleatoriamente una cualquiera cuando desde fuera
	 * se le pide que establezca una secuencia proprcionando el identificador
	 * de secuencia estándar. Esa elección aleatoria se puede ajustar dando
	 * un peso a cada versión de la secuencia estándar, para dar más
	 * probabilidad a unas que a otras. Ese peso es almacenado en este campo,
	 * que no tendrá sentido si la secuencia no es estándar. Si sólo hay
	 * una secuencia para esta actividad, su valor tampoco tendrá mucho
	 * sentido, pero será diferente de 0.
	 */
	int _weight;

	/**
	 * Eventos de la secuencia.
	 */
	MDLEvent** _events;

	/**
	 * Número de eventos asociados a la secuencia.
	 */
	unsigned int _numEvents;

	/**
	 * Número de huesos del modelo al que pertenece esta secuencia.
	 */
	unsigned int _numBones;

	/**
	 * Número de animaciones que se mezclan para crear la posición
	 * del modelo en esta secuencia. La parte de la librería que se
	 * encarga de mezclar las animaciones bajo demanda, no soporta mezclar
	 * más de dos, por lo que este campo sólo podrá valer 1 o 2. En cualquier
	 * caso, parece que el formato <code>QC</code> usado como entrada al
	 * empaquetador de <code>MDLs</code> sólo admite como mucho dos
	 * animaciones por secuencia.
	 */
	unsigned int _numAnimations;

	/**
	 * Array con las animaciones de cada hueso. Es un array dinámico, cuyo
	 * número de elementos se calcula multiplicando el número de huesos
	 * del modelo por el número de animaciones de la secuencia
	 * (MDLSequence::_numAnimations). Primero van las animaciones de todos
	 * los huesos en la primera animación, y luego los de la segunda.
	 */
	MDLBoneAnimation** _animations;

	/*
		FALTA POR METER:
		motiontype
		motionbone??
		linearmovement
		bbmin
		bbmax
		animaciones: información sobre su mezcla.
	 */

	/**
	 * Esquina de menor coordenadas del boundig box alineado con los ejes
	 * de la secuencia. Este bounding box es constante en toda la secuencia,
	 * aglutinando a todos los bounding boxes de cada frame. <br> La
	 * esquina superior se guarda en MDLSequence::_bbmax.
	 * <p>
	 * Este es el campo del "alto nivel" que en el fichero <code>MDL</code>
	 * se mantiene en HalfLifeMDL::seqdesc_t::bbmin. Para más información
	 * puede consultarse la documentación de ese campo.
	 * @see HalfLifeMDL::seqdesc_t::bbmin
	 * @see HalfLifeMDL::_bbmax;
	 */
	MDLPoint _bbmin;

	/**
	 * Esquina de mayor coordenadas del boundig box alineado con los ejes
	 * de la secuencia. Este bounding box es constante en toda la secuencia,
	 * aglutinando a todos los bounding boxes de cada frame. <br> La
	 * esquina inferior se guarda en MDLSequence::_bbmin.
	 * <p>
	 * Este es el campo del "alto nivel" que en el fichero <code>MDL</code>
	 * se mantiene en HalfLifeMDL::seqdesc_t::bbmax. Para más información
	 * puede consultarse la documentación de ese campo.
	 * @see HalfLifeMDL::seqdesc_t::bbmax
	 * @see HalfLifeMDL::seqdesc_t::bbmin
	 * @see HalfLifeMDL::_bbmin;
	 */
	MDLPoint _bbmax;

	/**
	 * Tipo de movimiento absoluto del modelo en la secuencia.
	 */
	MotionType _motionType;

	/**
	 * Si _motionType es MT_LINEAR, este campo contiene el desplazamiento
	 * lineal del modelo en un segundo.
	 */
	MDLPoint _linearMovement;

private:
	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementación es vacía y no debe usarse.
	 */
	MDLSequence(const MDLSequence& source) {
		assert(!"Constructor copia MDLSequence no disponible"); }

	/**
	 * Operador de asignación privado para evitar problemas en los
	 * delete. Su implementación es vacía y no debe usarse.
	 */
	MDLSequence& operator=(const MDLSequence &source) {
		assert(!"Operador asignación en MDLSequence no disponible");
		return *this;}


}; // class MDLSequence

} // namespace HalfLifeMDL

#endif
