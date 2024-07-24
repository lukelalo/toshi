//---------------------------------------------------------------------------
// MDLSequence.h
//---------------------------------------------------------------------------

/**
 * @file MDLSequence.h
 * Contiene la declaraci�n de la clase que almacena la informaci�n sobre
 * una secuencia del modelo de Half Life.
 * @see HalfLifeMDL::MDLSequence
 * @author Pedro Pablo G�mez Mart�n
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
 * Clase que almacena la informaci�n sobre una secuencia del modelo.
 * Por secuencia se entiende la informaci�n sobre el n�mero de fotogramas
 * por segundo de reproducci�n de la animaci�n, el avance (si existe) del
 * modelo por el mundo, informaci�n sobre eventos, etc�tera.
 * <p>
 * �sta es una clase "de alto nivel" a la que el usuario del m�dulo tiene
 * acceso. Est� directamente relacionada con la estructura
 * HalfLifeMDL::seqdesc_t utilizada en el fichero MDL.
 * @see HalfLifeMDL::seqdesc_t
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 * @todo Enlace a la p�gina general de la "arquitectura".
 */
class MDLSequence {
public:

	/**
	 * Tipo de datos que especifica los tipos de movimientos sobre el
	 * mundo que el personaje puede tener cuando se ejecuta esta secuencia.
	 * <br>
	 * Tiene relaci�n con HalfLifeMDL::motionType_t, aunque no es
	 * completamente directa.
	 */
	enum MotionType {
		/**
		 * No hay movimiento absoluto del modelo.
		 */
		MT_NONE = 0,
		/**
		 * Hay desplazamiento constante y lineal a lo largo de toda la
		 * reproducci�n de la secuencia. El desplazamiento puede obtenerse
		 * con MDLSequence::getLinearMovement.
		 */
		MT_LINEAR
	};

	/**
	 * Constructor de los objetos de la clase.
	 * @param infoSeq Estructura con la informaci�n de la secuencia tal
	 * y como se ha le�do del fichero. Este puntero debe ser
	 * liberado por el usuario (se hace copia).
	 * @param events Array con los eventos de la secuencia. Sustituye
	 * al campo  seqdesc_t.eventindex del primer par�metro, que es un puntero
	 * relativo al origen del fichero, no a memoria. Es responsabilidad
	 * del usuario eliminar este puntero. El objeto hace copia de la
	 * informaci�n apuntada por �l.
	 * @param numBones n�mero de huesos del modelo al que pertenece la
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
	 * @return Nombre de la secuencia tal y como se especific� en
	 * el QC al crear el MDL.
	 */
	const std::string& getName() const { return _name; }

	/**
	 * Devuelve el n�mero de fotogramas por segundo necesarios para
	 * la reproducci�n de la secuencia.
	 * @return Velocidad de refresco de la secuencia.
	 */
	float getFPS() const { return _fps; }

	/**
	 * Devuelve el n�mero de fotogramas de la secuencia.
	 * @return N�mero de fotogramas.
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
	 * Devuelve la constante representando al tipo est�ndar de la secuencia.
	 * @return Tipo de la secuencia.
	 */
	actionTags_t getType() const { return _type; }

	/**
	 * Devuelve el peso de la secuencia.<p>
	 * Un mismo modelo puede tener varias secuencias para la misma actividad,
	 * y la parte del motor gr�fico que se encarga de controlar al modelo
	 * puede establecer aleatoriamente una cualquiera cuando desde fuera
	 * se le pide que establezca una secuencia proprcionando el identificador
	 * de secuencia est�ndar. Esa elecci�n aleatoria se puede ajustar dando
	 * un peso a cada versi�n de la secuencia est�ndar, para dar m�s
	 * probabilidad a unas que a otras. Ese peso es almacenado en este campo,
	 * que no tendr� sentido si la secuencia no es est�ndar. Si s�lo hay
	 * una secuencia para esta actividad, su valor tampoco tendr� mucho
	 * sentido, pero ser� diferente de 0.
	 * @return Peso de la secuencia.
	 */
	int getWeight() const { return _weight; }

	/**
	 * Devuelve el n�mero de huesos del modelo al que pertenece esta
	 * secuencia. Este m�todo se proporciona por comodidad en el uso de
	 * MDLSequence::getAnimations(); el valor devuelto ser� el mismo que el
	 * obtenido por MDLCore::getNumBones con el modelo al que pertenece
	 * la secuencia.
	 * @return N�mero de huesos del modelo al que pertenece la secuencia.
	 */
	unsigned int getNumBones() const { return _numBones; }

	/**
	 * Devuelve el n�mero de animaciones que se mezclan en esta secuencia
	 * para crear la posici�n del modelo. La parte de la librer�a que se
	 * encarga de mezclar las animaciones bajo demanda, no soporta mezclar
	 * m�s de dos, por lo que este m�todo s�lo devolver� 1 � 2. En cualquier
	 * caso, parece que el formato <code>QC</code> usado como entrada al
	 * empaquetador de <code>MDLs</code> s�lo admite tambi�n como mucho dos
	 * animaciones por secuencia.
	 * @return N�mero de animaciones mezcladas en esta secuencia.
	 */
	unsigned int getNumAnimations() const { return _numAnimations; }

	/**
	 * Devuelve la informaci�n de las animaciones de los huesos de la
	 * secuencia. Devuelve un array, cuyo tama�o ser�
	 * getNumBones() * getNumAnimations(). El primer elemento ser� la
	 * animaci�n del hueso 0 para la animaci�n 0, el siguiente el
	 * hueso 1 para la animaci�n 0, etc. Luego comenzar� de nuevo con
	 * el hueso 0 para la animaci�n 2, si existe.
	 * @return Array de animaciones de los huesos para la secuencia. Este
	 * array NO debe liberarse. La gesti�n de la memoria la realiza el
	 * objeto.
	 */
	const MDLBoneAnimation** getAnimations() const { 
					return (const MDLBoneAnimation**) _animations; }

	/**
	 * Devuelve la informaci�n de la animaci�n de un hueso.
	 * @param bone Hueso cuya animaci�n se desea. Empieza a contar desde 0.
	 * @param anim N�mero de la animaci�n que se desea. Para crear la posici�n
	 * del hueso de la secuencia puede usarse m�s de una animaci�n, que se
	 * mezclan utilizando un peso. El n�mero de animaciones usadas en la
	 * secuencia actual se obtiene con getNumAnimations. Este campo
	 * especifica el n�mero de animaci�n (empezando a contar en 0).
	 * @return Animaci�n del hueso solicitado.
	 * @note No se comprueba que los par�metros son correctos (salvo en modo
	 * depuraci�n con assert), por lo que valores fuera de rango generar�n
	 * un resultado indefinido.
	 */
	const MDLBoneAnimation* getAnimation(unsigned int bone, 
											unsigned int anim) const {
					assert(bone < _numBones);
					assert(anim < _numAnimations);
					return(_animations[anim*_numAnimations + bone]);	}

	/**
	 * Devuelve el n�mero de eventos de la secuencia. Cada evento especifica
	 * en qu� fotograma debe ser generado.
	 * @return N�mero de eventos de la secuencia.
	 */
	unsigned int getNumEvents() const { return _numEvents; }

	/**
	 * Devuelve el evento i�simo de la secuencia.
	 * @param event �ndice del evento que se desea. Debe tener un �ndice
	 * v�lido. Si no, el resultado ser� indeterminado (en modo depuraci�n
	 * se comprueba con assert).
	 * @return Evento i�simo de la secuencia. El puntero no debe liberarse,
	 * la memoria ocupada es controlada por el objeto.
	 */
	const MDLEvent* getEvent(unsigned int event) const {
					assert (event < _numEvents);
					return _events[event]; }

	/**
	 * Devuelve el Bounding Box alineado con los ejes de la secuencia. Este
	 * bounding box es constante en toda la secuencia, aglutinando a todos
	 * los bounding box de cada frame. <br>
	 * Para m�s informaci�n, vease HalfLifeMDL::seqdesc_t::bbmin.
	 * @param bbmin Par�metro de salida con la esquina inferior del bounding
	 * box alineado con los ejes.
	 * @param bbmax Par�metro de salida con la esquina superior del bounding
	 * box alineado con los ejes.
	 * @see HalfLifeMDL::seqdesc_t::bbmin
	 */
	void getBoundingBox(MDLPoint& bbmin, MDLPoint& bbmax) const {
					bbmin = _bbmin; bbmax = _bbmax; }


	/**
	 * Devuelve el tipo de movimiento absoluto (respecto al mundo) del
	 * modelo cuando se reproduce esta secuencia. En funci�n del valor
	 * devuelto, tendr� o no sentido llamar, por ejemplo, a
	 * getLinearMovement.
	 * @return Tipo de movimiento absoluto del modelo en el mundo.
	 */
	MotionType getMotionType() const { return _motionType; }

	/**
	 * Devuelve el desplazamiento lineal en un segundo del modelo. S�lo
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
	 * Fotogramas por segundo necesarios para la reproducci�n de la
	 * secuencia.
	 */
	float _fps;

	/**
	 * N�mero de fotogramas de la secuencia.
	 */
	unsigned int _numFrames;

	/**
	 * A cierto si la secuencia es un ciclo.
	 */
	bool _loop;

	/**
	 * Tipo de la secuencia, si la secuencia es est�ndar.
	 */
	actionTags_t _type;

	/**
	 * "Peso" de esta secuencia para el tipo de secuencia _type.<p>
	 * Un mismo modelo puede tener varias secuencias para la misma actividad,
	 * y la parte del motor gr�fico que se encarga de controlar al modelo
	 * puede establecer aleatoriamente una cualquiera cuando desde fuera
	 * se le pide que establezca una secuencia proprcionando el identificador
	 * de secuencia est�ndar. Esa elecci�n aleatoria se puede ajustar dando
	 * un peso a cada versi�n de la secuencia est�ndar, para dar m�s
	 * probabilidad a unas que a otras. Ese peso es almacenado en este campo,
	 * que no tendr� sentido si la secuencia no es est�ndar. Si s�lo hay
	 * una secuencia para esta actividad, su valor tampoco tendr� mucho
	 * sentido, pero ser� diferente de 0.
	 */
	int _weight;

	/**
	 * Eventos de la secuencia.
	 */
	MDLEvent** _events;

	/**
	 * N�mero de eventos asociados a la secuencia.
	 */
	unsigned int _numEvents;

	/**
	 * N�mero de huesos del modelo al que pertenece esta secuencia.
	 */
	unsigned int _numBones;

	/**
	 * N�mero de animaciones que se mezclan para crear la posici�n
	 * del modelo en esta secuencia. La parte de la librer�a que se
	 * encarga de mezclar las animaciones bajo demanda, no soporta mezclar
	 * m�s de dos, por lo que este campo s�lo podr� valer 1 o 2. En cualquier
	 * caso, parece que el formato <code>QC</code> usado como entrada al
	 * empaquetador de <code>MDLs</code> s�lo admite como mucho dos
	 * animaciones por secuencia.
	 */
	unsigned int _numAnimations;

	/**
	 * Array con las animaciones de cada hueso. Es un array din�mico, cuyo
	 * n�mero de elementos se calcula multiplicando el n�mero de huesos
	 * del modelo por el n�mero de animaciones de la secuencia
	 * (MDLSequence::_numAnimations). Primero van las animaciones de todos
	 * los huesos en la primera animaci�n, y luego los de la segunda.
	 */
	MDLBoneAnimation** _animations;

	/*
		FALTA POR METER:
		motiontype
		motionbone??
		linearmovement
		bbmin
		bbmax
		animaciones: informaci�n sobre su mezcla.
	 */

	/**
	 * Esquina de menor coordenadas del boundig box alineado con los ejes
	 * de la secuencia. Este bounding box es constante en toda la secuencia,
	 * aglutinando a todos los bounding boxes de cada frame. <br> La
	 * esquina superior se guarda en MDLSequence::_bbmax.
	 * <p>
	 * Este es el campo del "alto nivel" que en el fichero <code>MDL</code>
	 * se mantiene en HalfLifeMDL::seqdesc_t::bbmin. Para m�s informaci�n
	 * puede consultarse la documentaci�n de ese campo.
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
	 * se mantiene en HalfLifeMDL::seqdesc_t::bbmax. Para m�s informaci�n
	 * puede consultarse la documentaci�n de ese campo.
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
	 * Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLSequence(const MDLSequence& source) {
		assert(!"Constructor copia MDLSequence no disponible"); }

	/**
	 * Operador de asignaci�n privado para evitar problemas en los
	 * delete. Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLSequence& operator=(const MDLSequence &source) {
		assert(!"Operador asignaci�n en MDLSequence no disponible");
		return *this;}


}; // class MDLSequence

} // namespace HalfLifeMDL

#endif
