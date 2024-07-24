//---------------------------------------------------------------------------
// MDLInstance.h
//---------------------------------------------------------------------------

/**
 * @file MDLInstance.h
 * Contiene la declaraci�n de la clase que almacena la informaci�n
 * din�mica de un modelo de Half Life (HalfLifeMDL::MDLInstance).
 * Esta clase tiene m�todos para establecer el modelo, la animaci�n
 * actual, etc., y para obtener la informaci�n necesaria para el
 * renderizado del modelo.<p>
 * @todo Constructor copia y operador de asignaci�n??
 * @see HalfLifeMDL::MDLCore
 * @see HalfLifeMDL::MDLInstance
 * @author Pedro Pablo G�mez Mart�n
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
	Se supone que el modelo est� cargado, y todo establecido (fotograma,
	piel, etc.).
	En cada fotograma hay que llamar al update(...).

	Suponemos que el modelo est� en una variable model.

	MDLTexture* texturaAntigua = NULL;
	MDLTexture* currentTexture;
	for (int c = 0; c < model.getNumMeshes(); c++) {
		model.selectMesh(c);
		currentTexture = getTexture();
		if (currentTexture != texturaAntigua)
			establecer(currentTexture);

		// Array de flotantes, cada tres hacen un v�rtice.
		float* vertices = model.getVertices();
		int nV = model.getNumVertices(); // Tama�o de getVertices / 3
		int nT = model.getNumTriangles();  // Tama�o de getVertexIndices / 3... bueno, m�s o menos :-m
		getMeshType() :-m
		unsigned int* adyacencia = model.getVertexIndices()
	}

	Contar la diferencia entre getVertices y getVertexIndices. El n�mero
	de cada uno es diferente, y quiz� en getVertexIndices no se enlazan
	todos los v�rtices de getVertices.
	Los m�todos de establecimiento y obtenci�n de informaci�n no
	comprueban que el modelo est� inicializado correctamente ni
	avisan de problemas, por lo que si hay algo mal el resultado es
	indeterminado. S�lo se comprueba con assert en modo depuraci�n.
	As� se supone que ser� m�s r�pido, aunque menos robusto.

	Contar que esta implementaci�n no escatima en memoria, tratando
	de acelerar el proceso m�s que ahorrar espacio en la mayor�a de
	las ocasiones. Como ejemplo est� el puntero a los cuaterniones
	usados por updateMatrices.

	Ejemplo de los attach?
 */

/**
 * Clase que almacena la informaci�n din�mica de un modelo de Half Life.
 * La informaci�n est�tica del modelo (huesos, mallas, animaciones,
 * etc�tera) se almacena en objetos de la clase HalfLifeMDL::MDLCore.
 * Los objetos de esta clase mantienen, por ejemplo, la secuencia actual,
 * el fotograma, piel seleccionada, etc.<p>
 * Manteniendo esta divisi�n se puede ahorrar memoria si una aplicaci�n
 * utiliza simult�neamente dos personajes con el mismo modelo, que se
 * compartir�a entre los dos objetos.<p>
 * @todo Explicar m�s su uso, y poner alg�n ejemplo. Es una "m�quina de
 * estados", etc. Decir que no se ha metido un beginRendering() o algo as�
 * para no complicar la vida al usuario de la clase a lo tonto, pero que
 * el orden deber�a ser el del ejemplo. Es decir no se deber�a llamar a
 * m�todos de actualizaci�n de la parte din�mica del modelo (update(...),
 * seleccionar piel, seleccionar modelo de una parte del cuerpo, etc.)
 * entre una llamada a selectMesh() y cualquier m�todo de acceso a los datos
 * de la malla, que selectMesh es m�s �ptimo si se le llama con mallas
 * consecutivas.
 * O sea, hay tres ... nada :-/
 * @author Pedro Pablo G�mez Mart�n
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
	 * los destructores. Por la misma raz�n tampoco se realiza copia.
	 */
	MDLInstance(const MDLCore* model);

	/**
	 * Destructor. No libera el espacio del modelo que se instancia,
	 * aunque s� la memoria utilizada internamente.
	 */
	~MDLInstance();

	/**
	 * A�ade un nuevo oyente a los eventos producidos por las secuencias
	 * que reproduzca el modelo.
	 * @param listener Nuevo oyente a a�adir. No se comprueba que el oyente
	 * ya est� registrado. El puntero no debe ser destruido (no se hace
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
	 * Devuelve cierto si la secuencia que se est� reproduciendo 
	 * actualmente ha llegado al final. Nunca se devolver� cierto si
	 * dicha secuencia es c�clica. Si se ha cambiado de animaci�n
	 * "en este ciclo" (sin llamar despu�s a update), y se consulta
	 * si la animaci�n ha terminado, devolver� siempre que no (aunque
	 * la animaci�n nueva s�lo tenga un fotograma).
	 *
	 * @return Cierto si la secuencia actual (no c�clica) ha terminado.
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
	//   M�todos de actualizaci�n de la parte din�mica del modelo
	//---------------------------------------------------------------

	/**
	 * Establece el modelo que se instancia con el objeto actual.
	 * @param model Modelo ya inicializado. El objeto no es liberado por
	 * el destructor de la clase, para permitir compartir el mismo
	 * modelo en varias instancias y que no se produzcan problemas en
	 * los destructores. Por la misma raz�n tampoco se realiza copia.
	 */
	void attachMDLCore(const MDLCore* model);

	/**
	 * Devuelve el modelo que se est� instanciando. Si no se ha
	 * establecido, devolver� NULL.
	 */
	const MDLCore* getMDLCore() const {return _model; }

	/**
	 * Establece el fotograma de la animaci�n actual. Normalmente no se
	 * utilizar�.
	 * @param frame Nuevo fotograma a establecer. Debe ser un fotograma
	 * v�lido.
	 */
	void setFrame(unsigned int frame);

	/**
	 * Devuelve el n�mero de pieles (diferentes grupos de textura) de
	 * el modelo.
	 * @todo �Alg�n sitio donde se cuente qu� son las pieles?
	 */
	unsigned int getNumSkins() const {
					assert(_model != NULL);
					return _model->getNumSkins(); }

	/**
	 * Establece la piel (grupo de textura) actual.
	 * @param skin �ndice de la piel. Debe estar entre 0 y getNumSkins - 1.
	 * En caso contrario el resultado es indefinido.
	 */
	void setSkin(unsigned int skin) {
					assert(_model != NULL);
					_nextState.skin = skin;
					_stateChanged = true; }

	/**
	 * Devuelve el n�mero de secuencias del modelo.
	 * @return N�mero de secuencias del modelo.
	 */
	unsigned int getNumSequences() const {
					assert(_model != NULL);
					return _model->getNumSequences(); }

	/**
	 * Establece la secuencia actual. Si se establece como actual
	 * la secuencia que se est� reproduciendo en este momento, la llamada
	 * no tiene ning�n efecto. <br>
	 * Si la secuencia tiene varias animaciones, se establece como peso para
	 * la primera de ellas 1.0, y 0.0 para todas las dem�s.
	 * @param index �ndice de la secuencia actual.
	 */
	void setSequenceByIndex(unsigned int index);

	/**
	 * Establece la secuencia actual utilizando su nombre. Es el
	 * nombre establecido por el creador del .QC utilizado para
	 * empaquetar el modelo. No hay una forma sencilla de obtener esos
	 * nombres (habr�a que recorrerlas todas). Este m�todo se implementa
	 * para que sea utilizado por c�digo que est� fuertemente relacionado
	 * con un modelo determinado y conoce los nombres de las animaciones.
	 * <p>
	 * Si se establece como actual la secuencia que se est� reproduciendo
	 * en este momento, la llamada no tiene ning�n efecto. <br>
	 * Si la secuencia tiene varias animaciones, se establece como peso para
	 * la primera de ellas 1.0, y 0.0 para todas las dem�s.
	 * @param name Nombre de la secuencia. Si no existe esta secuencia el
	 * resultado es indeterminado.
	 * @return 
	 *		- Cierto: se encontr� la secuencia y se estableci�.
	 *		- Falso: la secuencia no se encontr�, ni se estableci�. El
	 * estado del objeto no habr� cambiado.
	 * @warning Si el objeto est� en un estado erroneo (no se ha establecido
	 * modelo, por ejemplo), el resultado de este m�todo ser� indeterminado.
	 * Es decir, el valor devuelto a falso s�lo se utiliza si la secuencia
	 * no se encontr�, pero no como un valor de aviso de errores. La
	 * comprobaci�n de problemas m�s b�sicos no se realiza, salvo en la
	 * compilaci�n en modo depuraci�n (donde se usa assert), para no penalizar
	 * en eficiencia. Por tanto, un uso de este m�todo con un objeto con
	 * problemas tendr� un resultado indeterminado.
	 * @note Este m�todo es menos eficiente que
	 * MDLInstance::setSequenceByIndex.
	 */
	bool setSequence(const std::string& name);

	/**
	 * Establece la secuencia actual utilizando un identificador de
	 * secuencia estandar. Si el modelo define m�s de una secuencia para
	 * ese identificador de secuencia, se selecciona una aleatoriamente en
	 * funci�n de los pesos que estableciera el QC usado para empaquetar
	 * el modelo.
	 * <p>
	 * Si se establece como actual la secuencia que se est� reproduciendo
	 * en este momento, la llamada no tiene ning�n efecto. <br>
	 * Si la secuencia tiene varias animaciones, se establece como peso para
	 * la primera de ellas 1.0, y 0.0 para todas las dem�s.
	 * @param seq Identificador de secuencia estandar.
	 * @return
	 *		- Cierto:  se encontr� la secuencia y se estableci�.
	 *		- Falso: la secuencia no se encontr�, ni se estableci�. El
	 * estado del objeto no habr� cambiado.
	 * @warning Si el objeto est� en un estado erroneo (no se ha establecido
	 * modelo, por ejemplo), el resultado de este m�todo ser� indeterminado.
	 * Es decir, el valor devuelto a falso s�lo se utiliza si la secuencia
	 * no se encontr�, pero no como un valor de aviso de errores. La
	 * comprobaci�n de problemas m�s b�sicos no se realiza, salvo en la
	 * compilaci�n en modo depuraci�n (donde se usa assert), para no penalizar
	 * en eficiencia. Por tanto, un uso de este m�todo con un objeto con
	 * problemas tendr� un resultado indeterminado.
	 * @note Este m�todo es menos eficiente que
	 * MDLInstance::setSequenceByIndex.
	 * @note La elecci�n aleatoria de la secuencia se realiza usando la
	 * funci�n rand() de la librer�a est�ndar de C. Es responsabilidad del
	 * usuario haber establecido la semilla aleatoria llamando previamente
	 * a srand.
	 * @warning Este m�todo depende de una constante de compilaci�n definida
	 * en el fichero .cpp de nombre MAX_SEQUENCES_SAME_STANDAR_MOVEMENT cuyo
	 * valor actual es 5. Si hay m�s de 5 secuencias del mismo tipo de
	 * movimiento est�ndar, s�lo se considerar�n las 5 primeras.
	 */
	bool setSequence(HalfLifeMDL::actionTags_t seq);

	/**
	 * Devuelve el �ndice de la secuencia del modelo con un nombre dado.
	 * @return �ndice de la secuencia si existe. Si no existe, se devuelve
	 * el n�mero de secuencias (pues es un �ndice incorrecto).
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
	 * Este m�todo puede usarse para obtener el �ndice de una secuencia
	 * y poder utilizar as� setSequenceByIndex en lugar de setSequence
	 * que es m�s lento.
	 * @param name Nombre de la secuencia.
	 * @return �ndice de la secuencia, y getNumSequences() si la secuencia
	 * no existe.
	 */
	unsigned int getSequence(const std::string& name);

	/**
	 * Devuelve cierto o falso indicando si existe una secuencia en el
	 * modelo con un identificador est�ndar de secuencia.
	 * 
	 * @param seq Identificador de secuencia estandar buscada.
	 * @return Cierto si existe al menos una secuencia con ese identificador.
	 */
	bool existSequence(HalfLifeMDL::actionTags_t seq);

	/**
	 * Devuelve el n�mero de animaciones de la secuencia establecida
	 * actualmente. <p>
	 * Una secuencia puede crearse a partir de varias animaciones, que se
	 * mezclan al vuelo para determinar la posici�n del modelo. Esto permite
	 * modificar el comportamiento de una secuencia modificando los pesos
	 * asignados a cada una de las animaciones que la forman. <br>
	 * Este m�todo proporciona el n�mero de animaciones que se mezclan
	 * para construir la postura del modelo en la secuencia actual. Para
	 * dar un peso a cada animaci�n se utiliza setAnimationWeight.
	 */
	unsigned int getNumAnimations() const {
					assert(_model != NULL);
					return _model->getSequence(_nextState.sequence)->
														getNumAnimations(); }

	/**
	 * Establece el peso de una animaci�n de la secuencia establecida
	 * actualmente. <p>
	 * Una secuencia puede crearse a partir de varias animaciones, que se
	 * mezclan al vuelo para determinar la posici�n del modelo. Esto permite
	 * modificar el comportamiento de una secuencia modificando los pesos
	 * asignados a cada una de las animaciones que la forman. <br>
	 * Este m�todo se utiliza para establecer el peso que se utilizar�
	 * con cada una de las animaciones de la secuencia actual. La suma de
	 * pesos no necesariamente debe sumar uno. El objeto se encarga de
	 * calcular la suma de todos los pesos de las animaciones de la secuencia
	 * y utilizar la proporci�n de cada peso con el resto para mezclar todas
	 * las animaciones.
	 * @param animation �ndice de la animaci�n cuyo peso se proporciona. La
	 * primera animaci�n tiene el �ndice 0. El n�mero de animaciones de la
	 * secuencia se obtiene con getNumAnimations. Este campo no debe superar
	 * el n�mero total de animaciones. En caso contrario el resultado es
	 * indeterminado (no se comprueba el rango, salvo en modo depuraci�n 
	 * con assert).
	 * @param weight Peso asignado a la animaci�n indicada en el par�metro
	 * anterior. La suma de todos los pesos de las animaciones de la secuencia
	 * actual no necesariamente deben sumar 1. Se utiliza la suma de todos
	 * ellos para mezclar las animaciones en funci�n de los pesos
	 * individuales.<br>
	 * Aunque no se impone la obligaci�n de que todos los pesos deban sumar
	 * uno, s� es necesario que alguna animaci�n tenga un peso diferente de
	 * 0. De no ser as�, el resultado queda indeterminado.
	 */
	void setAnimationWeight(unsigned int animation, MDLBasicType weight) {
					assert(_animationWeights);
					assert(animation < getNumAnimations());
					_animationWeights[animation] = weight; }

	/**
	 * Devuelve el peso asociado a una animaci�n. Es la operaci�n inversa a
	 * setAnimationWeight.
	 * @param animation �ndice de la animaci�n cuyo peso se desea conocer.
	 * @return Peso de la animaci�n solicitada.
	 * @see setAnimationWeight
	 */
	MDLBasicType getAnimationWeight(unsigned int animation) const {
					assert(_animationWeights);
					assert(animation < getNumAnimations());
					return _animationWeights[animation]; }


	/**
	 * Devuelve la secuencia actual. Es la secuencia que se utilizar� para
	 * dibujar la pr�xima vez que se llame a update.
	 * @return Secuencia actual.
	 */
	const MDLSequence* getSequence() const {
					assert(_model != NULL);
					return _model->getSequence(_nextState.sequence); }

	/**
	 * Devuelve el n�mero de partes en las que est� dividido el modelo.
	 * @return N�mero de partes del modelo.
	 */
	unsigned int getNumBodyParts() const {
					assert(_model != NULL);
					return _model->getNumBodyParts(); }

	/**
	 * Devuelve el n�mero de submodelos (recubrimientos) de una parte del
	 * cuerpo.
	 * @param bodyPart Parte del cuerpo cuyo n�mero de recubrimientos se
	 * desea conocer.
	 * @return N�mero de recubrimientos de la parte del cuerpo solicitada.
	 */
	unsigned int getNumSubmodels(unsigned int bodyPart) const {
					assert(_model != NULL);
					assert(bodyPart < getNumBodyParts());
					return (_model->getBodyPart(bodyPart)->getNumModels()); }

	/**
	 * Establece el recubrimiento utilizado para una determinada parte
	 * del cuerpo.
	 * @param bodyPart Identificador (�ndice) de la parte del cuerpo.
	 * @param submodel �ndice del recubrimiento a establecer.
	 */
	void setSubmodel(unsigned int bodyPart, unsigned int submodel) {
					assert(submodel < getNumSubmodels(bodyPart));
					_nextState.selectedModels[bodyPart] = submodel;
					_stateChanged = true; _submodelChanged = true; }

	/**
	 * Devuelve el submodelo (recubrimiento) establecido actualmente
	 * en una parte del modelo. Devuelve el valor que se utilizar� la
	 * pr�xima vez que se llame a update.
	 * @param bodyPart Parte del cuerpo cuyo recubrimiento se desea conocer.
	 * @return �ndice del recubrimiento seleccionado para la parte del cuerpo
	 * pasada como par�metro.
	 */
	unsigned int getSubmodel(unsigned int bodyPart) const {
					assert(bodyPart < getNumBodyParts());
					return _nextState.selectedModels[bodyPart]; }

	/**
	 * Devuelve el tipo de movimiento absoluto (respecto al mundo) del
	 * modelo cuando se reproduce la secuencia actual. En funci�n del
	 * valor devuelto, tendr� o no sentido llamar, por ejemplo, a
	 * getLinearMovement.
	 * @return Tipo de movimiento absoluto del modelo en el mundo.
	 */
	MDLSequence::MotionType getMotionType() const { 
					return getSequence()->getMotionType(); }

	/**
	 * Devuelve el desplazamiento lineal en un segundo del modelo. S�lo
	 * tiene sentido si el desplazamiento es lineal (MT_LINEAR).
	 * @return Desplazamiento lineal por segundo del modelo.
	 */
	MDLPoint getLinearMovement() const {
					return getSequence()->getLinearMovement(); }

	/**
	 * Devuelve el n�mero de puntos de uni�n del modelo. Un punto de uni�n
	 * especifica un sistema de coordenadas respecto a la posici�n del
	 * modelo que depende de su postura (secuencia y fotograma actual).
	 * Se supone que ese sistema de coordenadas es coherente con el
	 * modelo y especificado por el dise�ador, para que se puedan
	 * dibujar objetos usando ese nuevo sistema de coordenadas 
	 * (tradicionalmente armas) y el resultado quede coherente.
	 * @return N�mero de puntos de uni�n del modelo.
	 */
	unsigned int getNumAttachments() const {
					assert(_model != NULL);
					return _model->getNumAttachments(); }

	/**
	 * Devuelve el sistema de coordenadas creado por un punto de uni�n
	 * en la secuencia y fotograma actual del modelo. <p>
	 * @param attachment N�mero de punto de uni�n cuya informaci�n
	 * se desea obtener.
	 * @param x Par�metro de salida con la direcci�n del eje X del
	 * nuevo sistema de referencia. En realidad devuelve el vector
	 * libre <em>i</em> del sistema, que ser� unitario.
	 * @param y Par�metro de salida con la direcci�n del eje Y del
	 * nuevo sistema de referencia. En realidad devuelve el vector
	 * libre <em>j</em> del sistema, que ser� unitario.
	 * @param z Par�metro de salida con la direcci�n del eje Z del
	 * nuevo sistema de referencia. En realidad devuelve el vector
	 * libre <em>k</em> del sistema, que ser� unitario.
	 * @param trans Par�metro de salida con la traslaci�n del nuevo
	 * sistema de referencia respecto al sistema del modelo completo.
	 */
	void getAttachment(unsigned int attachment, MDLPoint& x, MDLPoint& y,
												MDLPoint& z, MDLPoint& trans);

	// Controladores, boca (por comodidad), blendings...

	//---------------------------------------------------------------
	//              M�todos de acceso a la geometr�a
	//---------------------------------------------------------------

	/**
	 * Avanza el frame actual en tantos <em>segundos</em> como se solicite.
	 * Esta funci�n es la que hace de puente entre las funciones de
	 * actualizaci�n del estado del modelo y las usadas para obtener
	 * la geometr�a.
	 * @param time Tiempo de la aplicaci�n en <b>segundos</b>. El valor
	 * concreto recibido no es importante. Lo es la diferencia entre los
	 * valores en las llamadas consecutivas. Se usa para avanzar la animaci�n
	 * actual en funci�n del tiempo transcurrido desde la �ltima llamada.
	 * @note Este m�todo puede provocar la llamada a los capturadores de
	 * eventos del modelo actual en funci�n de la secuencia que se est�
	 * reproduciendo y los eventos que pueda generar.
	 */
	void update(float time);

	/**
	 * Devuelve el Bounding Box alineado con los ejes de la secuencia activa
	 * actualmente. Este bounding box es constante en toda la secuencia,
	 * aglutinando a todos los bounding box de cada frame. <br>
	 * Para m�s informaci�n, vease HalfLifeMDL::seqdesc_t::bbmin.
	 * @param bbmin Par�metro de salida con la esquina inferior del bounding
	 * box alineado con los ejes.
	 * @param bbmax Par�metro de salida con la esquina superior del bounding
	 * box alineado con los ejes.
	 * @see HalfLifeMDL::seqdesc_t::bbmin
	 * @todo Si en alg�n momento se arregla
	 * HalfLifeMDL::MDLCore::getBoundingBox estar�a bien que aqu� se
	 * devolviera ese bounding box si no hay secuencia seleccionada... claro
	 * que habr�a que admitir que no hubiera secuencia seleccionada :-)
	 */
	void getBoundingBox(MDLPoint& bbmin, MDLPoint& bbmax) const {
					assert(_model);
					assert(_currentState.sequence < 
								_model->getNumSequences());
					_model->getSequence(_currentState.sequence)->
								getBoundingBox(bbmin, bbmax); }


	/**
	 * Devuelve el n�mero de mallas del modelo.<p>
	 * Se calcula de forma sofisticada a partir del n�mero de mallas
	 * de cada una de las partes del cuerpo del modelo original para
	 * abstraer todas las divisiones del modelo durante la fase de
	 * dibujado.
	 * @return N�mero de mallas del modelo.
	 * @warning El valor devuelto por este m�todo puede variar a lo
	 * largo de la ejecuci�n. En concreto, existe posibilidad de
	 * variaci�n si se modifica el submodelo activo para cualquier
	 * parte del cuerpo.
	 * @todo Enlace a vease el m�todo de cambio de parte del cuerpo.
	 */
	unsigned int getNumMeshes() const {
					assert(_model);
					return _numMeshes; }

	/**
	 * Establece como activa la malla de �ndice indicado. Las llamadas
	 * que se realicen a este m�todo pueden ser costosas en tiempo. Est�
	 * optimizada para que sea llamada con mallas consecutivas, es decir
	 * en un recorrido de las mallas desde la 0 hasta la �ltima el
	 * establecimiento de la siguiente malla ser� m�s r�pido que en el
	 * mismo recorrido en sentido inverso, o en cualquier otro orden.
	 * @param mesh �ndice de la malla a establecer.
	 * @return El tipo de la malla establecida.
	 * @see MDLMesh::MeshType
	 */
	MDLMesh::MeshType selectMesh(unsigned int mesh);

	/**
	 * Devuelve la textura de la malla seleccionada actualmente (establecida
	 * con MDLInstance::selectMesh). Para acelerar la ejecuci�n del m�todo, no
	 * se comprueba que hay malla seleccionada (salvo en modo depuraci�n con
	 * assert), por lo que no se devuelve NULL ante error y el resultado ser�
	 * indefinido si no hay malla seleccionada.
	 * @return Textura de la malla seleccionada.
	 */
	const MDLTexture* getTexture() const;

	/**
	 * Devuelve el �ndice de la textura de la malla seleccionada actualmente
	 * (establecida con MDLInstance::selectMesh). Para acelerar la ejecuci�n
	 * del m�todo, no se comprueba que hay malla seleccionada (salvo en modo
	 * depuraci�n con assert), por lo que no se devuelve NULL ante error y el
	 * resultado ser� indefinido si no hay malla seleccionada.
	 * @return �ndice de la teextura de la malla seleccionada.
	 */
	const unsigned int getTextureIndex() const;

	/**
	 * Devuelve el n�mero de v�rtices de la malla seleccionada.
	 * Para acelerar la ejecuci�n  del m�todo, no se comprueba que hay
	 * malla seleccionada (salvo en modo depuraci�n con assert), por lo
	 * que no se devuelve 0 ante error y el resultado ser� indefinido si
	 * no hay malla seleccionada.
	 * @return N�mero de v�rtices de la malla seleccionada.
	 */
	unsigned int getNumVertices() const;

	/**
	 * Devuelve el tipo de la malla actual.
	 * @return Tipo de la malla actual.
	 * @see MDLMesh::MeshType
	 */
	MDLMesh::MeshType getMeshType() const;

	/**
	 * Devuelve el puntero a un array con los v�rtices de la malla
	 * seleccionada. Estos v�rtices ser�n referenciados posteriormente por
	 * el array devuelto por MDLInstance::getVertexIndices.<p>
	 * Como el array devuelto es <em>lineal</em> (s�lo tiene una
	 * dimensi�n), los v�rtices se organizan todos seguidos:<p>
	 * <center><code>[ x0, y0, z0, x1, y1, z1, x2, y2, z2, ... ]
	 * </code></center><p>
	 * El array NO debe liberarse, pues es gestionado por el objeto.
	 * Como norma general, su contenido se pierde entre llamadas
	 * a MDLInstance::selectMesh.<p>
	 * En realidad siempre se devuelve el mismo puntero, que apunta a un
	 * buffer interno del objeto, pero el contenido va variando con la
	 * malla seleccionada. No obstante, no se deber�a aprovechar esta
	 * caracter�stica, y los usuarios deber�an considerar que el puntero
	 * devuelto puede cambiar en cualquier momento.
	 * @return Puntero a un array con los v�rtices de la malla.
	 */
	const MDLBasicType* getVertices() const {
					assert (_vertices); return _vertices; }

	/**
	 * Devuelve el puntero a un array con las normales de la malla
	 * seleccionada. Estas normales ser�n referenciadas posteriormente por
	 * el array devuelto por MDLInstance::getVertexIndices.<p>
	 * Como el array devuelto es <em>lineal</em> (s�lo tiene una
	 * dimensi�n), las normales se organizan todas seguidas: <p>
	 * <center><code>[ x0, y0, z0, x1, y1, z1, x2, y2, z2, ... ]
	 * </code></center><p>
	 * El array NO debe liberarse, pues es gestionado por el objeto.
	 * Como norma general, su contenido se pierde entre llamadas
	 * a MDLInstance::selectMesh.<p>
	 * En realidad siempre se devuelve el mismo puntero, que apunta a un
	 * buffer interno del objeto, pero el contenido va variando con la
	 * malla seleccionada. No obstante, no se deber�a aprovechar esta
	 * caracter�stica, y los usuarios deber�an considerar que el puntero
	 * devuelto puede cambiar en cualquier momento.
	 * @return Puntero a un array con las normales de la malla.
	 */
	const MDLBasicType* getNormals() const {
					assert (_normals); return _normals; }

	/**
	 * Devuelve el puntero a un array con las coordenadas de textura de
	 * la malla seleccionada. Estas coordenadas de textura ser�n
	 * posteriormente referenciadas por el array devuelto por
	 * MDLInstance::getVertexIndices. <p>
	 * Como el array devuelto es <em>lineal</em> (s�lo tiene una dimensi�n),
	 * las coordenadas de textura se organizan todas seguidas: <p>
	 * <center><code>[ u0, v0, u1, v1, u2, v2, ... ]</code></center><p>
	 * El array NO debe liberarse, pues es gestionado por el objeto. Como
	 * norma general, su contenido se pierde entre llamadas a
	 * MDLInstance::selectMesh.<p> No obstante, si no se modifica la
	 * configuraci�n del modelo en lo que se refiere a los recubrimientos
	 * usados para cada parte del cuerpo, siempre se devolver� el mismo
	 * puntero (y contenido) cuando se llame a selectMesh(i) con el mismo
	 * �ndice.
	 * @return Puntero a un array con las coordenadas de textura de la malla.
	 */
	const MDLBasicType* getTextureCoords();

	/**
	 * Devuelve un puntero a un array con la matriz de adyacencia de los
	 * v�rtices devueltos por MDLInstance::getVertices, las normales de
	 * MDLInstance::getNormals y las coordenadas de textura de
	 * MDLInstance::getTextureCoords para crear los tri�ngulos de la malla.<p>
	 * Cada elemento del array es un �ndice que indica la posici�n en los
	 * arrays anteriores donde est�n las coordenadas, normal y coordenadas
	 * de textura del v�rtice al que se refiere el elemento. La construcci�n
	 * de los tri�ngulos de la malla a partir de la matriz de adyacencia
	 * depende del valor devuelto por MDLInstance::getMeshType. Se puede
	 * encontrar informaci�n sobre esto en MDLMesh::MeshType.
	 * @return Puntero a un array con la informaci�n de adyacencia de los
	 * v�rtices para formar los tri�ngulos. <p>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 * @note Es posible que una malla no referencie a todos los v�rtices
	 * contenidos en la matriz devuelta por MDLInstance::getVertices.
	 */
	const unsigned int* getVertexIndices();

	/**
	 * Devuelve el n�mero de v�rtices contenidos en el array devuelto por
	 * MDLInstance::getVertexIndices. En funci�n del tipo de la malla
	 * actual (informaci�n devuelta por MDLInstance::getMeshType), el
	 * n�mero de tri�ngulos de la malla ser� el n�mero devuelto por
	 * esta funci�n entre tres, o menos dos. Se puede encontrar m�s
	 * informaci�n sobre esto en MDLMesh::MeshType.
	 * @return N�mero de elementos v�lido en el puntero devuelto por
	 * MDLInstance::getVertexIndices.
	 * @note Aunque esta funci�n no tiene c�lculos intensos, s� posee
	 * varias indirecciones. Para una implementaci�n �ptima, es mejor
	 * asignar el valor devuelto a una variable que se utilizar� como
	 * condici�n del bucle, en lugar de llamar a esta funci�n cont�nuamente.
	 */
	unsigned int getNumVertexIndices();

	/**
	 * Devuelve la posici�n de un hueso en el fotograma actual. Este m�todo
	 * s�lo tiene utilidad en depuraci�n, para poder dibujar los huesos
	 * adem�s del modelo. <p>
	 * Como ejemplo, el c�digo para mostrar los huesos de un modelo
	 * tendr�a el siguiente esqueleto:
	 * <table border="0" cellspacing="1" cellpadding="1" align="center">
	 * <tr><td>
	 * <tt><pre>
	 * const MDLCore* core = instancia.getMDLCore();
	 * for (unsigned bone = 0; bone < core->getNumBones(); bone++) {
	 *	// Se permiten varios huesos padre, no se puede asegurar que
	 *	// s�lo lo sea el hueso 0. Hay que comprobar.
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
	 * @param bone Hueso cuya posici�n se desea conocer.
	 * @param bonePos Par�metro de salida con la posici�n del hueso
	 * solicitado.
	 */
	void getBonePos(unsigned int bone, MDLPoint& bonePos);

protected:

	/**
	 * Libera toda la memoria auxiliar necesaria para controlar la
	 * din�mica del modelo. Controla que los punteros sean NULL.
	 */
	void free();

	/**
	 * Calcula en MDLInstance::_vertices la transformaci�n de la malla
	 * seleccionada actual, indicada por MDLInstance::_currentBodyPart y 
	 * MDLInstance::_currentSubmodelMesh.<br>
	 * Es llamado por selectMesh, y utiliza las matrices de transformaci�n
	 * de los huesos calculados en update.
	 */
	void updateVertices();

	/**
	 * Calcula en MDLInstance::_normals la transformaci�n de las normales
	 * de la malla seleccionada actualmente, indicada por
	 * MDLInstance::_currentBodyPart y MDLInstance::_currentSubmodelMesh.<br>
	 * Es llamado por selectMesh, y utiliza las matrices de transformaci�n
	 * de los huesos calculados en update.
	 */
	void updateNormals();

	/**
	 * Calcula las matrices de transformaci�n de los v�rtices asociados
	 * a cada hueso en el array MDLInstance::_boneMatrices. Es llamado
	 * por MDLInstance::update, y utiliza la informaci�n del modelo y el
	 * fotograma actual.
	 */
	void updateMatrices();

	/**
	 * Comprueba si con el �ltimo avance del fotograma actual se ha
	 * atravesado alg�n evento de la secuencia que haya que generar.
	 * Es un m�todo auxiliar usado en update(). Si hay alg�n evento
	 * nuevo, se avisa a los oyentes.
	 */
	void checkEvents();

	/**
	 * Informaci�n est�tica del modelo. De este objeto se obtiene la
	 * informaci�n sobre los huesos, pieles, etc. que no cambian a lo
	 * largo del tiempo.
	 */
	const MDLCore* _model;

	/**
	 * Fotograma actual de la secuencia. Se almacena como flotante para
	 * mantener la velocidad de reproducci�n de la animaci�n lo m�s
	 * correcta posible utilizando interpolaci�n entre fotogramas.
	 */
	float _frame;

	/**
	 * Array con el peso asociado a cada animaci�n de la secuencia actual.
	 * @todo Enlace al m�todo donde se cuente con m�s detalle qu� es esto.
	 * Se pide memoria para este array al asociar el modelo est�tico.
	 * @todo Informaci�n sobre d�nde se crea.
	 */
	MDLBasicType* _animationWeights;

	/**
	 * Instante del �ltimo momento en el que se llam� a update.
	 */
	float _lastTime;

	/**
	 * Bandera que indica si _lastTime posee o no un valor v�lido.
	 * Ser� falso antes de llamar a update por primera vez.
	 */
	bool _validLastTime;

	/**
	 * Clase que almacena todo el estado din�mico de un modelo, como
	 * por ejemplo la secuencia seleccionada actualmente, la piel, etc.
	 * La intenci�n de tenerlo todo en una clase en lugar de como
	 * atributos de MDLInstance (al fin y al cabo es una estructura
	 * interna a una clase) es porque MDLInstance almacena dos atributos de
	 * esta estructura, uno con los datos utilizados por los m�todos que
	 * proporcionan la geometr�a del fotograma actual, y otro con la
	 * informaci�n que se usar� en el pr�ximo fotograma. Esto hace que la
	 * clase pueda ser utilizada por dos hebras simult�neamente, la que se
	 * encarga del dibujado, y la que se encarga de configurar el personaje.
	 * <p>
	 * Como es una clase interna, los atributos de dejan como p�blicos para
	 * no complicar la implementaci�n con m�todos de acceso y modificaci�n.
	 * No se ha implementado como una estructura porque tiene algunos m�todos
	 * �tiles (un constructor, por ejemplo), y, aunque sea posible, no
	 * parece apropiado definir una estructura con m�todos.
	 */
	class ModelState {
	public:
		/**
		 * Submodelo seleccionado para cada parte del cuerpo. Es un puntero
		 * para no fijar un m�ximo n�mero de partes del modelo ni desperdiciar
		 * memoria, aunque esto obliga a controlar su gesti�n. Antes de su
		 * uso debe llamarse a ModelState::setNumberModels. NO debe pedirse
		 * memoria para �l ni liberarse a mano. Para eso existen los m�todos
		 * correspondientes.
		 */
		unsigned int* selectedModels;

		/**
		 * �ndice de la secuencia actual.
		 */
		unsigned int sequence;

		/**
		 * Piel (grupo de texturas) seleccionada actualmente.
		 */
		unsigned int skin;

		/**
		 * Valor de los controladores. La posici�n i�sima del vector guarda
		 * la informaci�n del controlador de la posici�n i del array de
		 * controladores del modelo instanciado, no la del controlador
		 * de identificador i.<p>
		 * El valor se guarda restringido a 0..255, considerando a 0 el menor
		 * valor posible y a 255 el mayor, seg�n el rango establecido para el
		 * controlador en el fichero qc usado para crear el modelo.
		 * @todo Plantearse si esto deber�a ser unsigned char... o cambiar
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
		 * tantas partes del modelo como se indique en el par�metro.
		 * @param numBodyParts N�mero de partes del cuerpo de las que se
		 * tendr� que almacenar el recubrimiento seleccionado.
		 */
		void setNumberModels(unsigned int numBodyParts);

		/**
		 * Libera la memoria usada por punteros del objeto.
		 */
		void free();

		/**
		 * Operador de asignaci�n. Se utiliza copia profunda, con los punteros.
		 */
		ModelState& operator=(const ModelState& source);

	protected:

		/**
		 * Tama�o del array apuntado por selectedModels.
		 */
		unsigned int numModels;

	private:

		/**
		 * Constructor copia. Se implementa privado para prohibir la copia
		 * de objetos si no es con el operador de asignaci�n. El paso de
		 * objetos de esta clase por valor como par�metro estar� prohibido.
		 */
		ModelState(const ModelState& source) {}

	}; // class ModelState

	/**
	 * Estado del modelo actual, utilizado por los m�todos que proporcionan
	 * informaci�n sobre la <b>geometr�a</b>. Se copia de _nextState si es
	 * necesario al llamar a los m�todos de actualizaci�n de la geometr�a. <p>
	 * Se pide la memoria necesaria para los punteros de la estructura al
	 * asociar el modelo est�tico (MDLCore) al objeto.
	 */
	ModelState _currentState;

	/**
	 * Estado del modelo que se est� "gestando" para el pr�ximo fotograma.
	 * Se utiliza en todos los m�todos de consulta y modificaci�n del estado
	 * del objeto. Sus modificaciones no se llevar�n a cabo en la geometr�a
	 * hasta que no se llame a alguno de los m�todos de actualizaci�n, como
	 * MDLInstance::update.<p>
	 * Se pide la memoria necesaria para los punteros de la estructura al
	 * asociar el modelo est�tico (MDLCore) al objeto.
	 */
	ModelState _nextState;

	/**
	 * Bandera que se activa si el estado del siguiente fotograma ha cambiado
	 * respecto al actual. Se modifica en todos los m�todos que establecen el
	 * estado del objeto, y se desactiva en los m�todos de actualizaci�n,
	 * como MDLInstance::update.
	 */
	bool _stateChanged;

	/**
	 * Bandera que se activa si del estado ha cambiado la informaci�n sobre
	 * qu� modelo (recubrimiento) se est� utilizando en alguna parte del 
	 * cuerpo. Se modifica en MDLInstance::setSubmodel, y se desactiva en
	 * los m�todos de actualizaci�n.
	 */
	bool _submodelChanged;

	/**
	 * N�mero de mallas del modelo. Es el valor devuelto por
	 * MDLInstance::getNumMeshes. Se calcula en los m�todos de actualizaci�n
	 * de la geometr�a.
	 */
	unsigned int _numMeshes;

	/**
	 * N�mero de malla seleccionada actualmente. Es el n�mero "creado" en esta
	 * clase; la malla a la que pertenece en el MDLCore correspondiente se
	 * calcula a partir del n�mero de mallas del submodelo seleccionado de
	 * todas las partes del cuerpo.
	 */
	unsigned int _selectedMesh;

	/**
	 * Array con los v�rtices de la malla seleccionada actualmente ya
	 * transformados seg�n la posici�n del modelo. Se pide memoria para
	 * �l al asociar el modelo est�tico (MDLCore) al objeto, de tama�o
	 * suficiente para almacenar los v�rtices de la malla m�s grande del
	 * modelo. Como los v�rtices se almacenan en realidad en los
	 * recubrimientos (MDLSubmodel, que son los que tienen las mallas),
	 * el tama�o se calcula a partir de ellos, no de las mallas.
	 */
	MDLBasicType* _vertices;

	/**
	 * Array con las normales de la malla seleccionada actualmente ya
	 * transformados seg�n la postura del modelo. Se pide memoria para
	 * �l al asociar el modelo est�tico (MDLCore) al objeto, de tama�o
	 * suficiente para almacenar las normales de la malla m�s grande del
	 * modelo. Como las normales se almacenan en realidad en los
	 * recubrimientos (MDLSubmodel, que son los que tienen las mallas),
	 * el tama�o se calcula a partir de ellos, no de las mallas.
	 */
	MDLBasicType* _normals;

	/**
	 * Matrices de transformaci�n de cada hueso para el fotograma actual.
	 * Se calculan cuando se llama a MDLInstance::update. Se pide memoria para
	 * este array al asociar el modelo est�tico (MDLCore) al objeto.<br>
	 * La posici�n i del array almacena la matriz de transformaci�n que debe
	 * aplicarse a todos los v�rtices que est�n asociados con el hueso i en
	 * el modelo.
	 * <p>
	 * Se actualiza en MDLInstance::updateMatrices llamado por
	 * MDLInstance::update.
	 */
	MDLMatrix* _boneMatrices;

	/**
	 * Array de cuateriones din�mico. En realidad deber�a ser una variable
	 * local de updateMatrices, pero mantenerlo fuera ahorra tiempo en
	 * ejecuci�n.<p>
	 * MDLInstance::updateMatrices necesita un array de cuaterniones para
	 * calcular las matrices de transformaci�n de cada hueso. El tama�o
	 * de ese array depende del n�mero de huesos del modelo. Una
	 * alternativa ser�a crear una constante MAX_NUM_BONES o algo similar,
	 * y declarar el array est�tico como variable local en
	 * MDLInstance::updateMatrices. Pero es un poco restringida, por lo que
	 * una soluci�n m�s razonable es declarar un puntero y pedir memoria
	 * din�mica para el array. El problema es que pedir y liberar memoria
	 * cada vez que se llama a MDLInstance::updateMatrices para los
	 * cuaterniones es un derroche en tiempo. La soluci�n adoptada
	 * es tener el puntero como atributo, pedir memoria para �l al establecer
	 * el modelo (momento en el que se conoce el n�mero de huesos), y
	 * liberarlo de forma controlada cuando el modelo deje de utilizarse.
	 * MDLInstance::updateMatrices tendr� siempre disponible el espacio justo
	 * y no tendr� que preocuparse por �l. La desventaja es que se utiliza
	 * continuamente este espacio de memoria, incluso aunque no se llame
	 * nunca a MDLInstance::updateMatrices.
	 * @see MDLInstance::updateMatrices
	 * @see MDLInstance::_boneMatrices
	 */
	MDLQuaternion* _boneQuaternions;

	/**
	 * Array de vectores din�mico. En realidad deber�a ser una variable
	 * local de MDLInstance::updateMatrices, pero mantenerlo fuera ahorra
	 * tiempo de ejecuci�n.
	 * Vease MDLInstance::_boneQuaternions para informaci�n adicional.
	 *
	 * @see MDLInstance::_boneQuaternions
	 * @see MDLInstance::updateMatrices
	 * @see MDLInstance::_boneMatrices
	 */
	MDLPoint* _bonePositions;

	/**
	 * Bandera que indica si las matrices de transformaci�n de los huesos han
	 * cambiado desde la �ltima vez que se llam� a selectMesh. <p>
	 * Es una bandera que se activa en update() y se desactiva en selectMesh.
	 * Es necesaria porque, si es posible, selectMesh no recalcula la
	 * proyecci�n de los v�rtices del modelo respecto a los huesos (esto
	 * ocurre si la nueva malla a establecer tiene los mismos v�rtices que
	 * la antigua). Este ahorro de transformaci�n no debe aplicarse si las
	 * matrices de transformaci�n han sido modificadas (en update), y esa
	 * informaci�n se almacena en esta bandera.
	 */
	bool _boneMatricesChanged;

	/**
	 * Tama�o del array _vertices y _normals. El n�mero de v�rtices o
	 * normales que pueden contener ser� este valor entre tres. No es el
	 * n�mero de v�rtices de la malla actual, sino el n�mero de v�rtices
	 * que puede almacenar MDLInstance::_vertices o MDLInstance::_normals
	 */
	unsigned int _tamVerticesArray;

	/**
	 * Parte del cuerpo a la que pertenece la malla que est� seleccionada
	 * actualmente.
	 */
	unsigned int _currentBodyPart;

	/**
	 * �ndice de la malla del submodelo seleccionada actualmente.
	 * La malla seleccionada actualmente pertenece a un submodelo
	 * (HalfLifeMDL::MDLSubmodel), y �ste a una parte del cuerpo
	 * (HalfLifeMDL::MDLBodyPart). En cada momento, s�lo un submodelo
	 * de cada parte del cuerpo se utiliza en la representaci�n del modelo,
	 * y esa informaci�n se guarda en MDLInstance::_currentSelectedSubmdl.<br>
	 * Cada submodelo tiene a su vez varias mallas. Este campo guarda el
	 * �ndice de la malla seleccionada.
	 */
	unsigned int _currentSubmodelMesh;

	/**
	 * N�mero del evento de la secuencia actual que ser� el pr�ximo que se
	 * genere. Se utiliza para optimizar la b�squeda de la generaci�n de
	 * eventos. Est� asociado con _frame.
	 */
	unsigned int _nextEvent;

	/**
	 * N�mero de fotograma donde salta el siguiente evento de la secuencia
	 * que se generar�. Si la secuencia no tiene eventos, tomar� un valor
	 * por encima del n�mero de fotogramas de la secuencia (UINT_MAX).
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
