//---------------------------------------------------------------------------
// MDLInstance.h
//---------------------------------------------------------------------------

/**
 * @file MDLInstance.h
 * Contiene la declaración de la clase que almacena la información
 * dinámica de un modelo de Half Life (HalfLifeMDL::MDLInstance).
 * Esta clase tiene métodos para establecer el modelo, la animación
 * actual, etc., y para obtener la información necesaria para el
 * renderizado del modelo.<p>
 * @todo Constructor copia y operador de asignación??
 * @see HalfLifeMDL::MDLCore
 * @see HalfLifeMDL::MDLInstance
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */

#ifndef __HalfLifeMDL_MDLInstance
#define __HalfLifeMDL_MDLInstance

#include <vector>
#include <algorithm>

#include "MDLCore.h"
#include "MDLMatrix.h"
#include "MDLQuaternion.h"
#include "MDLEventListener.h"

#include "MDLDefinesDLLClass.h"

namespace HalfLifeMDL {


/*
	Modo de uso pretendido.
	Se supone que el modelo está cargado, y todo establecido (fotograma,
	piel, etc.).
	En cada fotograma hay que llamar al update(...).

	Suponemos que el modelo está en una variable model.

	MDLTexture* texturaAntigua = NULL;
	MDLTexture* currentTexture;
	for (int c = 0; c < model.getNumMeshes(); c++) {
		model.selectMesh(c);
		currentTexture = getTexture();
		if (currentTexture != texturaAntigua)
			establecer(currentTexture);

		// Array de flotantes, cada tres hacen un vértice.
		float* vertices = model.getVertices();
		int nV = model.getNumVertices(); // Tamaño de getVertices / 3
		int nT = model.getNumTriangles();  // Tamaño de getVertexIndices / 3... bueno, más o menos :-m
		getMeshType() :-m
		unsigned int* adyacencia = model.getVertexIndices()
	}

	Contar la diferencia entre getVertices y getVertexIndices. El número
	de cada uno es diferente, y quizá en getVertexIndices no se enlazan
	todos los vértices de getVertices.
	Los métodos de establecimiento y obtención de información no
	comprueban que el modelo esté inicializado correctamente ni
	avisan de problemas, por lo que si hay algo mal el resultado es
	indeterminado. Sólo se comprueba con assert en modo depuración.
	Así se supone que será más rápido, aunque menos robusto.

	Contar que esta implementación no escatima en memoria, tratando
	de acelerar el proceso más que ahorrar espacio en la mayoría de
	las ocasiones. Como ejemplo está el puntero a los cuaterniones
	usados por updateMatrices.

	Ejemplo de los attach?
 */

/**
 * Clase que almacena la información dinámica de un modelo de Half Life.
 * La información estática del modelo (huesos, mallas, animaciones,
 * etcétera) se almacena en objetos de la clase HalfLifeMDL::MDLCore.
 * Los objetos de esta clase mantienen, por ejemplo, la secuencia actual,
 * el fotograma, piel seleccionada, etc.<p>
 * Manteniendo esta división se puede ahorrar memoria si una aplicación
 * utiliza simultáneamente dos personajes con el mismo modelo, que se
 * compartiría entre los dos objetos.<p>
 * @todo Explicar más su uso, y poner algún ejemplo. Es una "máquina de
 * estados", etc. Decir que no se ha metido un beginRendering() o algo así
 * para no complicar la vida al usuario de la clase a lo tonto, pero que
 * el orden debería ser el del ejemplo. Es decir no se debería llamar a
 * métodos de actualización de la parte dinámica del modelo (update(...),
 * seleccionar piel, seleccionar modelo de una parte del cuerpo, etc.)
 * entre una llamada a selectMesh() y cualquier método de acceso a los datos
 * de la malla, que selectMesh es más óptimo si se le llama con mallas
 * consecutivas.
 * O sea, hay tres ... nada :-/
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
class MDL_PUBLIC MDLInstance {

public:

	/**
	 * Constructor por defecto. Requiere una llamada posterior a
	 * attachMDLCore.
	 */
	MDLInstance();

	/**
	 * Constructor a partir de un MDLCore.
	 * @param model Modelo que se instancia en el objeto. Debe
	 * estar inicializado correctamente. El objeto no es liberado por
	 * el destructor de la clase, para permitir compartir el mismo
	 * modelo en varias instancias y que no se produzcan problemas en
	 * los destructores. Por la misma razón tampoco se realiza copia.
	 */
	MDLInstance(const MDLCore* model);

	/**
	 * Destructor. No libera el espacio del modelo que se instancia,
	 * aunque sí la memoria utilizada internamente.
	 */
	~MDLInstance();

	/**
	 * Añade un nuevo oyente a los eventos producidos por las secuencias
	 * que reproduzca el modelo.
	 * @param listener Nuevo oyente a añadir. No se comprueba que el oyente
	 * ya esté registrado. El puntero no debe ser destruido (no se hace
	 * copia).
	 */
	void addEventListener(MDLEventListener* listener) {
					_eventListeners.push_back(listener); }

	/**
	 * Elimina un oyente de los eventos producidos por las secuencias
	 * que reproduzca el modelo.
	 * @param listener Oyente a eliminar.
	 * @return Cierto si se ha eliminado el oyente.
	 */
	bool removeEventListener(MDLEventListener* listener) {
					std::vector<MDLEventListener*>::iterator it;
					it = std::find(_eventListeners.begin(),
								_eventListeners.end(), listener);
					if (it == _eventListeners.end())
						return false;

					_eventListeners.erase(it); 
					return true; }

	/**
	 * Devuelve cierto si la secuencia que se está reproduciendo 
	 * actualmente ha llegado al final. Nunca se devolverá cierto si
	 * dicha secuencia es cíclica. Si se ha cambiado de animación
	 * "en este ciclo" (sin llamar después a update), y se consulta
	 * si la animación ha terminado, devolverá siempre que no (aunque
	 * la animación nueva sólo tenga un fotograma).
	 *
	 * @return Cierto si la secuencia actual (no cíclica) ha terminado.
	 */
	bool hasSequenceFinished() const {
					const MDLSequence* seq;
					assert(_model);
					if (_currentState.sequence != _nextState.sequence)
						return false;
					seq = _model->getSequence(_currentState.sequence);
					return ((!seq->isCycle()) && 
							((seq->getNumFrames() - 1) == _frame)); }

	//---------------------------------------------------------------
	//   Métodos de actualización de la parte dinámica del modelo
	//---------------------------------------------------------------

	/**
	 * Establece el modelo que se instancia con el objeto actual.
	 * @param model Modelo ya inicializado. El objeto no es liberado por
	 * el destructor de la clase, para permitir compartir el mismo
	 * modelo en varias instancias y que no se produzcan problemas en
	 * los destructores. Por la misma razón tampoco se realiza copia.
	 */
	void attachMDLCore(const MDLCore* model);

	/**
	 * Devuelve el modelo que se está instanciando. Si no se ha
	 * establecido, devolverá NULL.
	 */
	const MDLCore* getMDLCore() const {return _model; }

	/**
	 * Establece el fotograma de la animación actual. Normalmente no se
	 * utilizará.
	 * @param frame Nuevo fotograma a establecer. Debe ser un fotograma
	 * válido.
	 */
	void setFrame(unsigned int frame);

	/**
	 * Devuelve el número de pieles (diferentes grupos de textura) de
	 * el modelo.
	 * @todo ¿Algún sitio donde se cuente qué son las pieles?
	 */
	unsigned int getNumSkins() const {
					assert(_model != NULL);
					return _model->getNumSkins(); }

	/**
	 * Establece la piel (grupo de textura) actual.
	 * @param skin Índice de la piel. Debe estar entre 0 y getNumSkins - 1.
	 * En caso contrario el resultado es indefinido.
	 */
	void setSkin(unsigned int skin) {
					assert(_model != NULL);
					_nextState.skin = skin;
					_stateChanged = true; }

	/**
	 * Devuelve el número de secuencias del modelo.
	 * @return Número de secuencias del modelo.
	 */
	unsigned int getNumSequences() const {
					assert(_model != NULL);
					return _model->getNumSequences(); }

	/**
	 * Establece la secuencia actual. Si se establece como actual
	 * la secuencia que se está reproduciendo en este momento, la llamada
	 * no tiene ningún efecto. <br>
	 * Si la secuencia tiene varias animaciones, se establece como peso para
	 * la primera de ellas 1.0, y 0.0 para todas las demás.
	 * @param index Índice de la secuencia actual.
	 */
	void setSequenceByIndex(unsigned int index);

	/**
	 * Establece la secuencia actual utilizando su nombre. Es el
	 * nombre establecido por el creador del .QC utilizado para
	 * empaquetar el modelo. No hay una forma sencilla de obtener esos
	 * nombres (habría que recorrerlas todas). Este método se implementa
	 * para que sea utilizado por código que está fuertemente relacionado
	 * con un modelo determinado y conoce los nombres de las animaciones.
	 * <p>
	 * Si se establece como actual la secuencia que se está reproduciendo
	 * en este momento, la llamada no tiene ningún efecto. <br>
	 * Si la secuencia tiene varias animaciones, se establece como peso para
	 * la primera de ellas 1.0, y 0.0 para todas las demás.
	 * @param name Nombre de la secuencia. Si no existe esta secuencia el
	 * resultado es indeterminado.
	 * @return 
	 *		- Cierto: se encontró la secuencia y se estableció.
	 *		- Falso: la secuencia no se encontró, ni se estableció. El
	 * estado del objeto no habrá cambiado.
	 * @warning Si el objeto está en un estado erroneo (no se ha establecido
	 * modelo, por ejemplo), el resultado de este método será indeterminado.
	 * Es decir, el valor devuelto a falso sólo se utiliza si la secuencia
	 * no se encontró, pero no como un valor de aviso de errores. La
	 * comprobación de problemas más básicos no se realiza, salvo en la
	 * compilación en modo depuración (donde se usa assert), para no penalizar
	 * en eficiencia. Por tanto, un uso de este método con un objeto con
	 * problemas tendrá un resultado indeterminado.
	 * @note Este método es menos eficiente que
	 * MDLInstance::setSequenceByIndex.
	 */
	bool setSequence(const std::string& name);

	/**
	 * Establece la secuencia actual utilizando un identificador de
	 * secuencia estandar. Si el modelo define más de una secuencia para
	 * ese identificador de secuencia, se selecciona una aleatoriamente en
	 * función de los pesos que estableciera el QC usado para empaquetar
	 * el modelo.
	 * <p>
	 * Si se establece como actual la secuencia que se está reproduciendo
	 * en este momento, la llamada no tiene ningún efecto. <br>
	 * Si la secuencia tiene varias animaciones, se establece como peso para
	 * la primera de ellas 1.0, y 0.0 para todas las demás.
	 * @param seq Identificador de secuencia estandar.
	 * @return
	 *		- Cierto:  se encontró la secuencia y se estableció.
	 *		- Falso: la secuencia no se encontró, ni se estableció. El
	 * estado del objeto no habrá cambiado.
	 * @warning Si el objeto está en un estado erroneo (no se ha establecido
	 * modelo, por ejemplo), el resultado de este método será indeterminado.
	 * Es decir, el valor devuelto a falso sólo se utiliza si la secuencia
	 * no se encontró, pero no como un valor de aviso de errores. La
	 * comprobación de problemas más básicos no se realiza, salvo en la
	 * compilación en modo depuración (donde se usa assert), para no penalizar
	 * en eficiencia. Por tanto, un uso de este método con un objeto con
	 * problemas tendrá un resultado indeterminado.
	 * @note Este método es menos eficiente que
	 * MDLInstance::setSequenceByIndex.
	 * @note La elección aleatoria de la secuencia se realiza usando la
	 * función rand() de la librería estándar de C. Es responsabilidad del
	 * usuario haber establecido la semilla aleatoria llamando previamente
	 * a srand.
	 * @warning Este método depende de una constante de compilación definida
	 * en el fichero .cpp de nombre MAX_SEQUENCES_SAME_STANDAR_MOVEMENT cuyo
	 * valor actual es 5. Si hay más de 5 secuencias del mismo tipo de
	 * movimiento estándar, sólo se considerarán las 5 primeras.
	 */
	bool setSequence(HalfLifeMDL::actionTags_t seq);

	/**
	 * Devuelve el índice de la secuencia del modelo con un nombre dado.
	 * @return Índice de la secuencia si existe. Si no existe, se devuelve
	 * el número de secuencias (pues es un índice incorrecto).
	 * Es decir:
	 * <table border="0" cellspacing="1" cellpadding="1" align="center">
	 * <tr><td>
	 * <tt><pre>
	 * unsigned int indiceSecuencia;
	 * indiceSecuencia = getSequence("walk");
	 * if (indiceSecuencia == getNumSequences()) {
	 *	// No existe una secuencia "walk"
	 *	...
	 * }
	 * </pre></tt>
	 * </td></tr></table>
	 * Este método puede usarse para obtener el índice de una secuencia
	 * y poder utilizar así setSequenceByIndex en lugar de setSequence
	 * que es más lento.
	 * @param name Nombre de la secuencia.
	 * @return Índice de la secuencia, y getNumSequences() si la secuencia
	 * no existe.
	 */
	unsigned int getSequence(const std::string& name);

	/**
	 * Devuelve cierto o falso indicando si existe una secuencia en el
	 * modelo con un identificador estándar de secuencia.
	 * 
	 * @param seq Identificador de secuencia estandar buscada.
	 * @return Cierto si existe al menos una secuencia con ese identificador.
	 */
	bool existSequence(HalfLifeMDL::actionTags_t seq);

	/**
	 * Devuelve el número de animaciones de la secuencia establecida
	 * actualmente. <p>
	 * Una secuencia puede crearse a partir de varias animaciones, que se
	 * mezclan al vuelo para determinar la posición del modelo. Esto permite
	 * modificar el comportamiento de una secuencia modificando los pesos
	 * asignados a cada una de las animaciones que la forman. <br>
	 * Este método proporciona el número de animaciones que se mezclan
	 * para construir la postura del modelo en la secuencia actual. Para
	 * dar un peso a cada animación se utiliza setAnimationWeight.
	 */
	unsigned int getNumAnimations() const {
					assert(_model != NULL);
					return _model->getSequence(_nextState.sequence)->
														getNumAnimations(); }

	/**
	 * Establece el peso de una animación de la secuencia establecida
	 * actualmente. <p>
	 * Una secuencia puede crearse a partir de varias animaciones, que se
	 * mezclan al vuelo para determinar la posición del modelo. Esto permite
	 * modificar el comportamiento de una secuencia modificando los pesos
	 * asignados a cada una de las animaciones que la forman. <br>
	 * Este método se utiliza para establecer el peso que se utilizará
	 * con cada una de las animaciones de la secuencia actual. La suma de
	 * pesos no necesariamente debe sumar uno. El objeto se encarga de
	 * calcular la suma de todos los pesos de las animaciones de la secuencia
	 * y utilizar la proporción de cada peso con el resto para mezclar todas
	 * las animaciones.
	 * @param animation Índice de la animación cuyo peso se proporciona. La
	 * primera animación tiene el índice 0. El número de animaciones de la
	 * secuencia se obtiene con getNumAnimations. Este campo no debe superar
	 * el número total de animaciones. En caso contrario el resultado es
	 * indeterminado (no se comprueba el rango, salvo en modo depuración 
	 * con assert).
	 * @param weight Peso asignado a la animación indicada en el parámetro
	 * anterior. La suma de todos los pesos de las animaciones de la secuencia
	 * actual no necesariamente deben sumar 1. Se utiliza la suma de todos
	 * ellos para mezclar las animaciones en función de los pesos
	 * individuales.<br>
	 * Aunque no se impone la obligación de que todos los pesos deban sumar
	 * uno, sí es necesario que alguna animación tenga un peso diferente de
	 * 0. De no ser así, el resultado queda indeterminado.
	 */
	void setAnimationWeight(unsigned int animation, MDLBasicType weight) {
					assert(_animationWeights);
					assert(animation < getNumAnimations());
					_animationWeights[animation] = weight; }

	/**
	 * Devuelve el peso asociado a una animación. Es la operación inversa a
	 * setAnimationWeight.
	 * @param animation Índice de la animación cuyo peso se desea conocer.
	 * @return Peso de la animación solicitada.
	 * @see setAnimationWeight
	 */
	MDLBasicType getAnimationWeight(unsigned int animation) const {
					assert(_animationWeights);
					assert(animation < getNumAnimations());
					return _animationWeights[animation]; }


	/**
	 * Devuelve la secuencia actual. Es la secuencia que se utilizará para
	 * dibujar la próxima vez que se llame a update.
	 * @return Secuencia actual.
	 */
	const MDLSequence* getSequence() const {
					assert(_model != NULL);
					return _model->getSequence(_nextState.sequence); }

	/**
	 * Devuelve el número de partes en las que está dividido el modelo.
	 * @return Número de partes del modelo.
	 */
	unsigned int getNumBodyParts() const {
					assert(_model != NULL);
					return _model->getNumBodyParts(); }

	/**
	 * Devuelve el número de submodelos (recubrimientos) de una parte del
	 * cuerpo.
	 * @param bodyPart Parte del cuerpo cuyo número de recubrimientos se
	 * desea conocer.
	 * @return Número de recubrimientos de la parte del cuerpo solicitada.
	 */
	unsigned int getNumSubmodels(unsigned int bodyPart) const {
					assert(_model != NULL);
					assert(bodyPart < getNumBodyParts());
					return (_model->getBodyPart(bodyPart)->getNumModels()); }

	/**
	 * Establece el recubrimiento utilizado para una determinada parte
	 * del cuerpo.
	 * @param bodyPart Identificador (índice) de la parte del cuerpo.
	 * @param submodel Índice del recubrimiento a establecer.
	 */
	void setSubmodel(unsigned int bodyPart, unsigned int submodel) {
					assert(submodel < getNumSubmodels(bodyPart));
					_nextState.selectedModels[bodyPart] = submodel;
					_stateChanged = true; _submodelChanged = true; }

	/**
	 * Devuelve el submodelo (recubrimiento) establecido actualmente
	 * en una parte del modelo. Devuelve el valor que se utilizará la
	 * próxima vez que se llame a update.
	 * @param bodyPart Parte del cuerpo cuyo recubrimiento se desea conocer.
	 * @return Índice del recubrimiento seleccionado para la parte del cuerpo
	 * pasada como parámetro.
	 */
	unsigned int getSubmodel(unsigned int bodyPart) const {
					assert(bodyPart < getNumBodyParts());
					return _nextState.selectedModels[bodyPart]; }

	/**
	 * Devuelve el tipo de movimiento absoluto (respecto al mundo) del
	 * modelo cuando se reproduce la secuencia actual. En función del
	 * valor devuelto, tendrá o no sentido llamar, por ejemplo, a
	 * getLinearMovement.
	 * @return Tipo de movimiento absoluto del modelo en el mundo.
	 */
	MDLSequence::MotionType getMotionType() const { 
					return getSequence()->getMotionType(); }

	/**
	 * Devuelve el desplazamiento lineal en un segundo del modelo. Sólo
	 * tiene sentido si el desplazamiento es lineal (MT_LINEAR).
	 * @return Desplazamiento lineal por segundo del modelo.
	 */
	MDLPoint getLinearMovement() const {
					return getSequence()->getLinearMovement(); }

	/**
	 * Devuelve el número de puntos de unión del modelo. Un punto de unión
	 * especifica un sistema de coordenadas respecto a la posición del
	 * modelo que depende de su postura (secuencia y fotograma actual).
	 * Se supone que ese sistema de coordenadas es coherente con el
	 * modelo y especificado por el diseñador, para que se puedan
	 * dibujar objetos usando ese nuevo sistema de coordenadas 
	 * (tradicionalmente armas) y el resultado quede coherente.
	 * @return Número de puntos de unión del modelo.
	 */
	unsigned int getNumAttachments() const {
					assert(_model != NULL);
					return _model->getNumAttachments(); }

	/**
	 * Devuelve el sistema de coordenadas creado por un punto de unión
	 * en la secuencia y fotograma actual del modelo. <p>
	 * @param attachment Número de punto de unión cuya información
	 * se desea obtener.
	 * @param x Parámetro de salida con la dirección del eje X del
	 * nuevo sistema de referencia. En realidad devuelve el vector
	 * libre <em>i</em> del sistema, que será unitario.
	 * @param y Parámetro de salida con la dirección del eje Y del
	 * nuevo sistema de referencia. En realidad devuelve el vector
	 * libre <em>j</em> del sistema, que será unitario.
	 * @param z Parámetro de salida con la dirección del eje Z del
	 * nuevo sistema de referencia. En realidad devuelve el vector
	 * libre <em>k</em> del sistema, que será unitario.
	 * @param trans Parámetro de salida con la traslación del nuevo
	 * sistema de referencia respecto al sistema del modelo completo.
	 */
	void getAttachment(unsigned int attachment, MDLPoint& x, MDLPoint& y,
												MDLPoint& z, MDLPoint& trans);

	// Controladores, boca (por comodidad), blendings...

	//---------------------------------------------------------------
	//              Métodos de acceso a la geometría
	//---------------------------------------------------------------

	/**
	 * Avanza el frame actual en tantos <em>segundos</em> como se solicite.
	 * Esta función es la que hace de puente entre las funciones de
	 * actualización del estado del modelo y las usadas para obtener
	 * la geometría.
	 * @param time Tiempo de la aplicación en <b>segundos</b>. El valor
	 * concreto recibido no es importante. Lo es la diferencia entre los
	 * valores en las llamadas consecutivas. Se usa para avanzar la animación
	 * actual en función del tiempo transcurrido desde la última llamada.
	 * @note Este método puede provocar la llamada a los capturadores de
	 * eventos del modelo actual en función de la secuencia que se esté
	 * reproduciendo y los eventos que pueda generar.
	 */
	void update(float time);

	/**
	 * Devuelve el Bounding Box alineado con los ejes de la secuencia activa
	 * actualmente. Este bounding box es constante en toda la secuencia,
	 * aglutinando a todos los bounding box de cada frame. <br>
	 * Para más información, vease HalfLifeMDL::seqdesc_t::bbmin.
	 * @param bbmin Parámetro de salida con la esquina inferior del bounding
	 * box alineado con los ejes.
	 * @param bbmax Parámetro de salida con la esquina superior del bounding
	 * box alineado con los ejes.
	 * @see HalfLifeMDL::seqdesc_t::bbmin
	 * @todo Si en algún momento se arregla
	 * HalfLifeMDL::MDLCore::getBoundingBox estaría bien que aquí se
	 * devolviera ese bounding box si no hay secuencia seleccionada... claro
	 * que habría que admitir que no hubiera secuencia seleccionada :-)
	 */
	void getBoundingBox(MDLPoint& bbmin, MDLPoint& bbmax) const {
					assert(_model);
					assert(_currentState.sequence < 
								_model->getNumSequences());
					_model->getSequence(_currentState.sequence)->
								getBoundingBox(bbmin, bbmax); }


	/**
	 * Devuelve el número de mallas del modelo.<p>
	 * Se calcula de forma sofisticada a partir del número de mallas
	 * de cada una de las partes del cuerpo del modelo original para
	 * abstraer todas las divisiones del modelo durante la fase de
	 * dibujado.
	 * @return Número de mallas del modelo.
	 * @warning El valor devuelto por este método puede variar a lo
	 * largo de la ejecución. En concreto, existe posibilidad de
	 * variación si se modifica el submodelo activo para cualquier
	 * parte del cuerpo.
	 * @todo Enlace a vease el método de cambio de parte del cuerpo.
	 */
	unsigned int getNumMeshes() const {
					assert(_model);
					return _numMeshes; }

	/**
	 * Establece como activa la malla de índice indicado. Las llamadas
	 * que se realicen a este método pueden ser costosas en tiempo. Está
	 * optimizada para que sea llamada con mallas consecutivas, es decir
	 * en un recorrido de las mallas desde la 0 hasta la última el
	 * establecimiento de la siguiente malla será más rápido que en el
	 * mismo recorrido en sentido inverso, o en cualquier otro orden.
	 * @param mesh Índice de la malla a establecer.
	 * @return El tipo de la malla establecida.
	 * @see MDLMesh::MeshType
	 */
	MDLMesh::MeshType selectMesh(unsigned int mesh);

	/**
	 * Devuelve la textura de la malla seleccionada actualmente (establecida
	 * con MDLInstance::selectMesh). Para acelerar la ejecución del método, no
	 * se comprueba que hay malla seleccionada (salvo en modo depuración con
	 * assert), por lo que no se devuelve NULL ante error y el resultado será
	 * indefinido si no hay malla seleccionada.
	 * @return Textura de la malla seleccionada.
	 */
	const MDLTexture* getTexture() const;

	/**
	 * Devuelve el índice de la textura de la malla seleccionada actualmente
	 * (establecida con MDLInstance::selectMesh). Para acelerar la ejecución
	 * del método, no se comprueba que hay malla seleccionada (salvo en modo
	 * depuración con assert), por lo que no se devuelve NULL ante error y el
	 * resultado será indefinido si no hay malla seleccionada.
	 * @return Índice de la teextura de la malla seleccionada.
	 */
	const unsigned int getTextureIndex() const;

	/**
	 * Devuelve el número de vértices de la malla seleccionada.
	 * Para acelerar la ejecución  del método, no se comprueba que hay
	 * malla seleccionada (salvo en modo depuración con assert), por lo
	 * que no se devuelve 0 ante error y el resultado será indefinido si
	 * no hay malla seleccionada.
	 * @return Número de vértices de la malla seleccionada.
	 */
	unsigned int getNumVertices() const;

	/**
	 * Devuelve el tipo de la malla actual.
	 * @return Tipo de la malla actual.
	 * @see MDLMesh::MeshType
	 */
	MDLMesh::MeshType getMeshType() const;

	/**
	 * Devuelve el puntero a un array con los vértices de la malla
	 * seleccionada. Estos vértices serán referenciados posteriormente por
	 * el array devuelto por MDLInstance::getVertexIndices.<p>
	 * Como el array devuelto es <em>lineal</em> (sólo tiene una
	 * dimensión), los vértices se organizan todos seguidos:<p>
	 * <center><code>[ x0, y0, z0, x1, y1, z1, x2, y2, z2, ... ]
	 * </code></center><p>
	 * El array NO debe liberarse, pues es gestionado por el objeto.
	 * Como norma general, su contenido se pierde entre llamadas
	 * a MDLInstance::selectMesh.<p>
	 * En realidad siempre se devuelve el mismo puntero, que apunta a un
	 * buffer interno del objeto, pero el contenido va variando con la
	 * malla seleccionada. No obstante, no se debería aprovechar esta
	 * característica, y los usuarios deberían considerar que el puntero
	 * devuelto puede cambiar en cualquier momento.
	 * @return Puntero a un array con los vértices de la malla.
	 */
	const MDLBasicType* getVertices() const {
					assert (_vertices); return _vertices; }

	/**
	 * Devuelve el puntero a un array con las normales de la malla
	 * seleccionada. Estas normales serán referenciadas posteriormente por
	 * el array devuelto por MDLInstance::getVertexIndices.<p>
	 * Como el array devuelto es <em>lineal</em> (sólo tiene una
	 * dimensión), las normales se organizan todas seguidas: <p>
	 * <center><code>[ x0, y0, z0, x1, y1, z1, x2, y2, z2, ... ]
	 * </code></center><p>
	 * El array NO debe liberarse, pues es gestionado por el objeto.
	 * Como norma general, su contenido se pierde entre llamadas
	 * a MDLInstance::selectMesh.<p>
	 * En realidad siempre se devuelve el mismo puntero, que apunta a un
	 * buffer interno del objeto, pero el contenido va variando con la
	 * malla seleccionada. No obstante, no se debería aprovechar esta
	 * característica, y los usuarios deberían considerar que el puntero
	 * devuelto puede cambiar en cualquier momento.
	 * @return Puntero a un array con las normales de la malla.
	 */
	const MDLBasicType* getNormals() const {
					assert (_normals); return _normals; }

	/**
	 * Devuelve el puntero a un array con las coordenadas de textura de
	 * la malla seleccionada. Estas coordenadas de textura serán
	 * posteriormente referenciadas por el array devuelto por
	 * MDLInstance::getVertexIndices. <p>
	 * Como el array devuelto es <em>lineal</em> (sólo tiene una dimensión),
	 * las coordenadas de textura se organizan todas seguidas: <p>
	 * <center><code>[ u0, v0, u1, v1, u2, v2, ... ]</code></center><p>
	 * El array NO debe liberarse, pues es gestionado por el objeto. Como
	 * norma general, su contenido se pierde entre llamadas a
	 * MDLInstance::selectMesh.<p> No obstante, si no se modifica la
	 * configuración del modelo en lo que se refiere a los recubrimientos
	 * usados para cada parte del cuerpo, siempre se devolverá el mismo
	 * puntero (y contenido) cuando se llame a selectMesh(i) con el mismo
	 * índice.
	 * @return Puntero a un array con las coordenadas de textura de la malla.
	 */
	const MDLBasicType* getTextureCoords();

	/**
	 * Devuelve un puntero a un array con la matriz de adyacencia de los
	 * vértices devueltos por MDLInstance::getVertices, las normales de
	 * MDLInstance::getNormals y las coordenadas de textura de
	 * MDLInstance::getTextureCoords para crear los triángulos de la malla.<p>
	 * Cada elemento del array es un índice que indica la posición en los
	 * arrays anteriores donde están las coordenadas, normal y coordenadas
	 * de textura del vértice al que se refiere el elemento. La construcción
	 * de los triángulos de la malla a partir de la matriz de adyacencia
	 * depende del valor devuelto por MDLInstance::getMeshType. Se puede
	 * encontrar información sobre esto en MDLMesh::MeshType.
	 * @return Puntero a un array con la información de adyacencia de los
	 * vértices para formar los triángulos. <p>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 * @note Es posible que una malla no referencie a todos los vértices
	 * contenidos en la matriz devuelta por MDLInstance::getVertices.
	 */
	const unsigned int* getVertexIndices();

	/**
	 * Devuelve el número de vértices contenidos en el array devuelto por
	 * MDLInstance::getVertexIndices. En función del tipo de la malla
	 * actual (información devuelta por MDLInstance::getMeshType), el
	 * número de triángulos de la malla será el número devuelto por
	 * esta función entre tres, o menos dos. Se puede encontrar más
	 * información sobre esto en MDLMesh::MeshType.
	 * @return Número de elementos válido en el puntero devuelto por
	 * MDLInstance::getVertexIndices.
	 * @note Aunque esta función no tiene cálculos intensos, sí posee
	 * varias indirecciones. Para una implementación óptima, es mejor
	 * asignar el valor devuelto a una variable que se utilizará como
	 * condición del bucle, en lugar de llamar a esta función contínuamente.
	 */
	unsigned int getNumVertexIndices();

	/**
	 * Devuelve la posición de un hueso en el fotograma actual. Este método
	 * sólo tiene utilidad en depuración, para poder dibujar los huesos
	 * además del modelo. <p>
	 * Como ejemplo, el código para mostrar los huesos de un modelo
	 * tendría el siguiente esqueleto:
	 * <table border="0" cellspacing="1" cellpadding="1" align="center">
	 * <tr><td>
	 * <tt><pre>
	 * const MDLCore* core = instancia.getMDLCore();
	 * for (unsigned bone = 0; bone < core->getNumBones(); bone++) {
	 *	// Se permiten varios huesos padre, no se puede asegurar que
	 *	// sólo lo sea el hueso 0. Hay que comprobar.
	 * 	MDLPoint origen, destino;
	 * 	int parent = core->getBone(bone)->getParentIndex();
	 *	if (parent != -1) {
	 * 		model->getBonePos(bone, origen);
	 * 		model->getBonePos(parent, destino);
	 * 		dibujarLinea(origen, destino);
	 *	}
	 * }
	 * </pre></tt>
	 * </td></tr></table>
	 * @param bone Hueso cuya posición se desea conocer.
	 * @param bonePos Parámetro de salida con la posición del hueso
	 * solicitado.
	 */
	void getBonePos(unsigned int bone, MDLPoint& bonePos);

protected:

	/**
	 * Libera toda la memoria auxiliar necesaria para controlar la
	 * dinámica del modelo. Controla que los punteros sean NULL.
	 */
	void free();

	/**
	 * Calcula en MDLInstance::_vertices la transformación de la malla
	 * seleccionada actual, indicada por MDLInstance::_currentBodyPart y 
	 * MDLInstance::_currentSubmodelMesh.<br>
	 * Es llamado por selectMesh, y utiliza las matrices de transformación
	 * de los huesos calculados en update.
	 */
	void updateVertices();

	/**
	 * Calcula en MDLInstance::_normals la transformación de las normales
	 * de la malla seleccionada actualmente, indicada por
	 * MDLInstance::_currentBodyPart y MDLInstance::_currentSubmodelMesh.<br>
	 * Es llamado por selectMesh, y utiliza las matrices de transformación
	 * de los huesos calculados en update.
	 */
	void updateNormals();

	/**
	 * Calcula las matrices de transformación de los vértices asociados
	 * a cada hueso en el array MDLInstance::_boneMatrices. Es llamado
	 * por MDLInstance::update, y utiliza la información del modelo y el
	 * fotograma actual.
	 */
	void updateMatrices();

	/**
	 * Comprueba si con el último avance del fotograma actual se ha
	 * atravesado algún evento de la secuencia que haya que generar.
	 * Es un método auxiliar usado en update(). Si hay algún evento
	 * nuevo, se avisa a los oyentes.
	 */
	void checkEvents();

	/**
	 * Información estática del modelo. De este objeto se obtiene la
	 * información sobre los huesos, pieles, etc. que no cambian a lo
	 * largo del tiempo.
	 */
	const MDLCore* _model;

	/**
	 * Fotograma actual de la secuencia. Se almacena como flotante para
	 * mantener la velocidad de reproducción de la animación lo más
	 * correcta posible utilizando interpolación entre fotogramas.
	 */
	float _frame;

	/**
	 * Array con el peso asociado a cada animación de la secuencia actual.
	 * @todo Enlace al método donde se cuente con más detalle qué es esto.
	 * Se pide memoria para este array al asociar el modelo estático.
	 * @todo Información sobre dónde se crea.
	 */
	MDLBasicType* _animationWeights;

	/**
	 * Instante del último momento en el que se llamó a update.
	 */
	float _lastTime;

	/**
	 * Bandera que indica si _lastTime posee o no un valor válido.
	 * Será falso antes de llamar a update por primera vez.
	 */
	bool _validLastTime;

	/**
	 * Clase que almacena todo el estado dinámico de un modelo, como
	 * por ejemplo la secuencia seleccionada actualmente, la piel, etc.
	 * La intención de tenerlo todo en una clase en lugar de como
	 * atributos de MDLInstance (al fin y al cabo es una estructura
	 * interna a una clase) es porque MDLInstance almacena dos atributos de
	 * esta estructura, uno con los datos utilizados por los métodos que
	 * proporcionan la geometría del fotograma actual, y otro con la
	 * información que se usará en el próximo fotograma. Esto hace que la
	 * clase pueda ser utilizada por dos hebras simultáneamente, la que se
	 * encarga del dibujado, y la que se encarga de configurar el personaje.
	 * <p>
	 * Como es una clase interna, los atributos de dejan como públicos para
	 * no complicar la implementación con métodos de acceso y modificación.
	 * No se ha implementado como una estructura porque tiene algunos métodos
	 * útiles (un constructor, por ejemplo), y, aunque sea posible, no
	 * parece apropiado definir una estructura con métodos.
	 */
	class ModelState {
	public:
		/**
		 * Submodelo seleccionado para cada parte del cuerpo. Es un puntero
		 * para no fijar un máximo número de partes del modelo ni desperdiciar
		 * memoria, aunque esto obliga a controlar su gestión. Antes de su
		 * uso debe llamarse a ModelState::setNumberModels. NO debe pedirse
		 * memoria para él ni liberarse a mano. Para eso existen los métodos
		 * correspondientes.
		 */
		unsigned int* selectedModels;

		/**
		 * Índice de la secuencia actual.
		 */
		unsigned int sequence;

		/**
		 * Piel (grupo de texturas) seleccionada actualmente.
		 */
		unsigned int skin;

		/**
		 * Valor de los controladores. La posición iésima del vector guarda
		 * la información del controlador de la posición i del array de
		 * controladores del modelo instanciado, no la del controlador
		 * de identificador i.<p>
		 * El valor se guarda restringido a 0..255, considerando a 0 el menor
		 * valor posible y a 255 el mayor, según el rango establecido para el
		 * controlador en el fichero qc usado para crear el modelo.
		 * @todo Plantearse si esto debería ser unsigned char... o cambiar
		 * el comentario.
		 */
		unsigned char controller[4];

		// byte	m_blending[2];		// animation blending
		// byte	m_mouth;			// Semejante a m_controller, pero
									// para la boca.

		/**
		 * Constructor. No se pide memoria. Debe llamarse a
		 * ModelState::setNumberModels antes de poder usar selectedModels.
		 */
		ModelState();

		/**
		 * Destructor. Libera la memoria que se haya asignado para el objeto.
		 */
		~ModelState() { free(); }

		/** 
		 * Pide memoria para almacenar el modelo (recubrimiento) asignado a
		 * tantas partes del modelo como se indique en el parámetro.
		 * @param numBodyParts Número de partes del cuerpo de las que se
		 * tendrá que almacenar el recubrimiento seleccionado.
		 */
		void setNumberModels(unsigned int numBodyParts);

		/**
		 * Libera la memoria usada por punteros del objeto.
		 */
		void free();

		/**
		 * Operador de asignación. Se utiliza copia profunda, con los punteros.
		 */
		ModelState& operator=(const ModelState& source);

	protected:

		/**
		 * Tamaño del array apuntado por selectedModels.
		 */
		unsigned int numModels;

	private:

		/**
		 * Constructor copia. Se implementa privado para prohibir la copia
		 * de objetos si no es con el operador de asignación. El paso de
		 * objetos de esta clase por valor como parámetro estará prohibido.
		 */
		ModelState(const ModelState& source) {}

	}; // class ModelState

	/**
	 * Estado del modelo actual, utilizado por los métodos que proporcionan
	 * información sobre la <b>geometría</b>. Se copia de _nextState si es
	 * necesario al llamar a los métodos de actualización de la geometría. <p>
	 * Se pide la memoria necesaria para los punteros de la estructura al
	 * asociar el modelo estático (MDLCore) al objeto.
	 */
	ModelState _currentState;

	/**
	 * Estado del modelo que se está "gestando" para el próximo fotograma.
	 * Se utiliza en todos los métodos de consulta y modificación del estado
	 * del objeto. Sus modificaciones no se llevarán a cabo en la geometría
	 * hasta que no se llame a alguno de los métodos de actualización, como
	 * MDLInstance::update.<p>
	 * Se pide la memoria necesaria para los punteros de la estructura al
	 * asociar el modelo estático (MDLCore) al objeto.
	 */
	ModelState _nextState;

	/**
	 * Bandera que se activa si el estado del siguiente fotograma ha cambiado
	 * respecto al actual. Se modifica en todos los métodos que establecen el
	 * estado del objeto, y se desactiva en los métodos de actualización,
	 * como MDLInstance::update.
	 */
	bool _stateChanged;

	/**
	 * Bandera que se activa si del estado ha cambiado la información sobre
	 * qué modelo (recubrimiento) se está utilizando en alguna parte del 
	 * cuerpo. Se modifica en MDLInstance::setSubmodel, y se desactiva en
	 * los métodos de actualización.
	 */
	bool _submodelChanged;

	/**
	 * Número de mallas del modelo. Es el valor devuelto por
	 * MDLInstance::getNumMeshes. Se calcula en los métodos de actualización
	 * de la geometría.
	 */
	unsigned int _numMeshes;

	/**
	 * Número de malla seleccionada actualmente. Es el número "creado" en esta
	 * clase; la malla a la que pertenece en el MDLCore correspondiente se
	 * calcula a partir del número de mallas del submodelo seleccionado de
	 * todas las partes del cuerpo.
	 */
	unsigned int _selectedMesh;

	/**
	 * Array con los vértices de la malla seleccionada actualmente ya
	 * transformados según la posición del modelo. Se pide memoria para
	 * él al asociar el modelo estático (MDLCore) al objeto, de tamaño
	 * suficiente para almacenar los vértices de la malla más grande del
	 * modelo. Como los vértices se almacenan en realidad en los
	 * recubrimientos (MDLSubmodel, que son los que tienen las mallas),
	 * el tamaño se calcula a partir de ellos, no de las mallas.
	 */
	MDLBasicType* _vertices;

	/**
	 * Array con las normales de la malla seleccionada actualmente ya
	 * transformados según la postura del modelo. Se pide memoria para
	 * él al asociar el modelo estático (MDLCore) al objeto, de tamaño
	 * suficiente para almacenar las normales de la malla más grande del
	 * modelo. Como las normales se almacenan en realidad en los
	 * recubrimientos (MDLSubmodel, que son los que tienen las mallas),
	 * el tamaño se calcula a partir de ellos, no de las mallas.
	 */
	MDLBasicType* _normals;

	/**
	 * Matrices de transformación de cada hueso para el fotograma actual.
	 * Se calculan cuando se llama a MDLInstance::update. Se pide memoria para
	 * este array al asociar el modelo estático (MDLCore) al objeto.<br>
	 * La posición i del array almacena la matriz de transformación que debe
	 * aplicarse a todos los vértices que estén asociados con el hueso i en
	 * el modelo.
	 * <p>
	 * Se actualiza en MDLInstance::updateMatrices llamado por
	 * MDLInstance::update.
	 */
	MDLMatrix* _boneMatrices;

	/**
	 * Array de cuateriones dinámico. En realidad debería ser una variable
	 * local de updateMatrices, pero mantenerlo fuera ahorra tiempo en
	 * ejecución.<p>
	 * MDLInstance::updateMatrices necesita un array de cuaterniones para
	 * calcular las matrices de transformación de cada hueso. El tamaño
	 * de ese array depende del número de huesos del modelo. Una
	 * alternativa sería crear una constante MAX_NUM_BONES o algo similar,
	 * y declarar el array estático como variable local en
	 * MDLInstance::updateMatrices. Pero es un poco restringida, por lo que
	 * una solución más razonable es declarar un puntero y pedir memoria
	 * dinámica para el array. El problema es que pedir y liberar memoria
	 * cada vez que se llama a MDLInstance::updateMatrices para los
	 * cuaterniones es un derroche en tiempo. La solución adoptada
	 * es tener el puntero como atributo, pedir memoria para él al establecer
	 * el modelo (momento en el que se conoce el número de huesos), y
	 * liberarlo de forma controlada cuando el modelo deje de utilizarse.
	 * MDLInstance::updateMatrices tendrá siempre disponible el espacio justo
	 * y no tendrá que preocuparse por él. La desventaja es que se utiliza
	 * continuamente este espacio de memoria, incluso aunque no se llame
	 * nunca a MDLInstance::updateMatrices.
	 * @see MDLInstance::updateMatrices
	 * @see MDLInstance::_boneMatrices
	 */
	MDLQuaternion* _boneQuaternions;

	/**
	 * Array de vectores dinámico. En realidad debería ser una variable
	 * local de MDLInstance::updateMatrices, pero mantenerlo fuera ahorra
	 * tiempo de ejecución.
	 * Vease MDLInstance::_boneQuaternions para información adicional.
	 *
	 * @see MDLInstance::_boneQuaternions
	 * @see MDLInstance::updateMatrices
	 * @see MDLInstance::_boneMatrices
	 */
	MDLPoint* _bonePositions;

	/**
	 * Bandera que indica si las matrices de transformación de los huesos han
	 * cambiado desde la última vez que se llamó a selectMesh. <p>
	 * Es una bandera que se activa en update() y se desactiva en selectMesh.
	 * Es necesaria porque, si es posible, selectMesh no recalcula la
	 * proyección de los vértices del modelo respecto a los huesos (esto
	 * ocurre si la nueva malla a establecer tiene los mismos vértices que
	 * la antigua). Este ahorro de transformación no debe aplicarse si las
	 * matrices de transformación han sido modificadas (en update), y esa
	 * información se almacena en esta bandera.
	 */
	bool _boneMatricesChanged;

	/**
	 * Tamaño del array _vertices y _normals. El número de vértices o
	 * normales que pueden contener será este valor entre tres. No es el
	 * número de vértices de la malla actual, sino el número de vértices
	 * que puede almacenar MDLInstance::_vertices o MDLInstance::_normals
	 */
	unsigned int _tamVerticesArray;

	/**
	 * Parte del cuerpo a la que pertenece la malla que está seleccionada
	 * actualmente.
	 */
	unsigned int _currentBodyPart;

	/**
	 * Índice de la malla del submodelo seleccionada actualmente.
	 * La malla seleccionada actualmente pertenece a un submodelo
	 * (HalfLifeMDL::MDLSubmodel), y éste a una parte del cuerpo
	 * (HalfLifeMDL::MDLBodyPart). En cada momento, sólo un submodelo
	 * de cada parte del cuerpo se utiliza en la representación del modelo,
	 * y esa información se guarda en MDLInstance::_currentSelectedSubmdl.<br>
	 * Cada submodelo tiene a su vez varias mallas. Este campo guarda el
	 * índice de la malla seleccionada.
	 */
	unsigned int _currentSubmodelMesh;

	/**
	 * Número del evento de la secuencia actual que será el próximo que se
	 * genere. Se utiliza para optimizar la búsqueda de la generación de
	 * eventos. Está asociado con _frame.
	 */
	unsigned int _nextEvent;

	/**
	 * Número de fotograma donde salta el siguiente evento de la secuencia
	 * que se generará. Si la secuencia no tiene eventos, tomará un valor
	 * por encima del número de fotogramas de la secuencia (UINT_MAX).
	 * Este atributo no es absolutamente necesario (puede obtenerse a
	 * partir de _nextEvent),  pero evita algunas comprobaciones que se
	 * producen una vez por cada refresco.
	 */
	unsigned int _frameNextEvent;

	/**
	 * Vector de oyentes de los eventos de las secuencias que reproduzca el
	 * modelo.
	 */
	std::vector<MDLEventListener*> _eventListeners;

}; // class MDLCore

} // namespace HalfLifeMDL

#endif
