//---------------------------------------------------------------------------
// MDLInstance.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLInstance.cpp
 * Contiene la definici�n (implementaci�n) de la clase que almacena la 
 * informaci�n din�mica de un modelo de Half Life (HalfLifeMDL::MDLInstance).
 * Esta clase tiene m�todos para establecer el modelo, la animaci�n
 * actual, etc., y para obtener la informaci�n necesaria para el
 * renderizado del modelo.<p>
 * @see HalfLifeMDL::MDLCore
 * @see HalfLifeMDL::MDLInstance
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */

#include <limits.h>

#include <iostream>

#include "MDLInstance.h"
#include "MDLBoneAnimation.h"

/**
 * Constante que especifica el m�ximo n�mero de secuencias de un modelo
 * que pueden tener asociada el mismo identificador de secuencia est�ndar.
 * <p>
 * En realidad esta restricci�n no se impone sobre los ficheros le�dos,
 * sino sobre el m�todo getSequence(HalfLifeMDL::actionTags_t). Si hay
 * m�s de MAX_SEQUENCES_SAME_STANDAR_MOVEMENT secuencias del mismo
 * tipo, ese m�todo s�lo considerar� a las primeras.
 */
#define MAX_SEQUENCES_SAME_STANDAR_MOVEMENT 5
// Si se cambia este valor, deber�a modificarse la documentaci�n de
// getSequence(HalfLifeMDL::actionTags_t)

// Ver comentario en checkEvents
//#define CHANGE_SEQUENCE_EVENT 3000

namespace HalfLifeMDL {

/**
 * Constructor por defecto. Requiere una llamada posterior a
 * attachMDLCore.
 */
MDLInstance::MDLInstance() : _model(NULL), _vertices(NULL), _normals(NULL),
							 _boneMatrices(NULL), _boneQuaternions(NULL),
							 _bonePositions(NULL), _validLastTime(false),
							_animationWeights(NULL) {
} // Constructor sin par�metros

//---------------------------------------------------------------------------

/**
 * Constructor a partir de un MDLCore.
 * @param model Modelo que se instancia en el objeto. Debe
 * estar inicializado correctamente. El objeto no es liberado por
 * el destructor de la clase, para permitir compartir el mismo
 * modelo en varias instancias y que no se produzcan problemas en
 * los destructores. Por la misma raz�n tampoco se realiza copia.
 */
MDLInstance::MDLInstance(const MDLCore* model) : _model(NULL),
							_vertices(NULL), _normals(NULL), 
							_boneMatrices(NULL),  _boneQuaternions(NULL),
							_bonePositions(NULL), _validLastTime(false),
							_animationWeights(NULL) {
	attachMDLCore(model);
} // constructor(MDLCore* )

//---------------------------------------------------------------------------

/**
 * Destructor. No libera el espacio del modelo que se instancia,
 * aunque s� la memoria utilizada internamente.
 */
MDLInstance::~MDLInstance() {

	free();

} // destructor

//---------------------------------------------------------------------------

/**
 * Establece el modelo que se instancia con el objeto actual.
 * @param model Modelo ya inicializado. El objeto no es liberado por
 * el destructor de la clase, para permitir compartir el mismo
 * modelo en varias instancias y que no se produzcan problemas en
 * los destructores. Por la misma raz�n tampoco se realiza copia.
 */
void MDLInstance::attachMDLCore(const MDLCore* model) {

	unsigned int maxVertices, maxAnimations;
	free();
	if (model == NULL) {
		_model = NULL;
		return;
	}

	// Pedimos memoria para guardar el submodelo de cada parte
	// del cuerpo.
	assert(model->getNumBodyParts() > 0);
	_currentState.setNumberModels(model->getNumBodyParts());
	_nextState.setNumberModels(model->getNumBodyParts());
	// Establecemos como actual el recubrimiento 0 para todas
	// las partes del cuerpo.
	for (unsigned int bp = 0; bp < model->getNumBodyParts(); bp++)
		_nextState.selectedModels[bp] = 0;

	// Recorremos el modelo para ver qu� recubrimiento tiene m�s v�rtices
	// y crear matrices de tama�o suficiente para que entren.
	maxVertices = 0;
	for (unsigned int indexBodyPart = 0;
			indexBodyPart < model->getNumBodyParts(); indexBodyPart++) {
		const MDLBodyPart *bodyPart;
		bodyPart = model->getBodyPart(indexBodyPart);
		for (unsigned int indexModel = 0;
				indexModel < bodyPart->getNumModels(); indexModel++) {
			unsigned int numVertices;
			numVertices  = bodyPart->getModel(indexModel)->getNumVertices();

			if (numVertices > maxVertices)
				maxVertices = numVertices;

		} // for que recorr�a todos los modelos.
	} // for que recorr�a todas las partes del cuerpo

	// En maxVertices tenemos el n�mero de v�rtices que tiene el recubrimiento
	// m�s grande. Pedimos memoria para el array donde guardaremos los
	// v�rtices transformados.
	// Los v�rtices est�n todos seguidos en un array de coordenadas, con
	// tres coordenadas por v�rtice: [x0. y0, z0, x1, y1, z1, ...]
	// Ocurre lo mismo con las normales.
	_vertices = new MDLBasicType[maxVertices*3];
	_tamVerticesArray = maxVertices;

	_normals = new MDLBasicType[maxVertices*3];

	_boneMatrices = new MDLMatrix[model->getNumBones()];
	_boneQuaternions = new MDLQuaternion[model->getNumBones()];
	_bonePositions = new MDLPoint[model->getNumBones()];

	// Miramos qu� secuencia tiene m�s animaciones para crear el array
	// donde se guardar�n los pesos.
	maxAnimations = 0;
	for (unsigned int indexSequence = 0; 
			indexSequence < model->getNumSequences(); indexSequence++) {
		unsigned int numAnim;
		numAnim = model->getSequence(indexSequence)->getNumAnimations();
		if (numAnim > maxAnimations)
			maxAnimations = numAnim;
	}
	assert(maxAnimations > 0);
	_animationWeights = new MDLBasicType[maxAnimations];
	_animationWeights[0] = 1.0;
	for (unsigned int c = 1; c < maxAnimations; c++)
		_animationWeights[c] = 0.0;



	_model = model;

	// Llamamos a update para que se actualice la informaci�n del n�mero
	// de mallas.
	// Para que update no haga cosas que no queremos, utilizamos alg�n truco.
	// Establecemos como animaci�n actual una diferente a la "anterior" para
	// forzar a comenzar por el fotograma cero. Tambi�n indicamos que
	// el submodelo ha cambiado, para que cuente cuantas mallas tenemos.
	_stateChanged = true;
	_submodelChanged = true;	// Forzamos el c�lculo del n�mero de mallas.
	_currentState.sequence = 1;
	_nextState.sequence = 0;	// Se establece la secuencia 0 por defecto.
	update(0);
	
	// El par�metro pasado a update no es v�lido. Es un valor dummy
	// para que update haga su trabajo. Anulamos el registro de ese tiempo
	// incorrecto.
	_validLastTime = false;

} // attachMDLCore

//---------------------------------------------------------------------------

/**
 * Establece la secuencia actual. Si se establece como actual
 * la secuencia que se est� reproduciendo en este momento, la llamada
 * no tiene ning�n efecto. <br>
 * Si la secuencia tiene varias animaciones, se establece como peso para
 * la primera de ellas 1.0, y 0.0 para todas las dem�s.
 * @param index �ndice de la secuencia actual.
 */
void MDLInstance::setSequenceByIndex(unsigned int index) {

	assert(_model != NULL);
	assert(index < getNumSequences());
	if (_nextState.sequence != index) {
		_nextState.sequence = index;
		_stateChanged = true;
		_animationWeights[0] = 1.0;
		for (unsigned int c = 1; c < getNumAnimations(); c++)
			_animationWeights[c] = 0.0;
	}

} // setSequenceByIndex

//---------------------------------------------------------------------------

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
bool MDLInstance::setSequence(const std::string& name) {

	assert(_model);

	unsigned int c;

	c = getSequence(name);

	if (c == _model->getNumSequences())
		return false;

	setSequenceByIndex(c);
	return true;

} // setSequence(std::string)

//---------------------------------------------------------------------------

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
bool MDLInstance::setSequence(HalfLifeMDL::actionTags_t seq) {

	const MDLSequence* sequences[MAX_SEQUENCES_SAME_STANDAR_MOVEMENT];
	unsigned int indexSequences[MAX_SEQUENCES_SAME_STANDAR_MOVEMENT];
	unsigned int count, numValidSequences, totalWeight;
	count = 0;
	numValidSequences = 0;
	totalWeight = 0;

	while ((count < _model->getNumSequences()) &&
		(numValidSequences < MAX_SEQUENCES_SAME_STANDAR_MOVEMENT)) {
		if (_model->getSequence(count)->getType() == seq) {
			sequences[numValidSequences] = _model->getSequence(count);
			indexSequences[numValidSequences] = count;
			++numValidSequences;
			totalWeight += _model->getSequence(count)->getWeight();
		}
		count++;
	}

	// En sequences tenemos las secuencias del tipo solicitado.
	switch(numValidSequences) {
		case 0:
			// No hay ninguna secuencia del tipo solicitado.
			// Terminamos.
			return false;
		case 1:
			// Solo hay una secuencia. No nos complicamos la vida con
			// n�meros aleatorios.
			setSequenceByIndex(indexSequences[0]);
			break;
		default:
			// Nos toca elegir una. Seleccionamos un n�mero aleatorio entre
			// 0 y el peso total menos uno, y luego miramos a qu� secuencia
			// le corresponde ese valor.
			unsigned int elegido = rand();
			unsigned int selection = ((float)elegido / RAND_MAX) * 
													(totalWeight - 1);
			count = 0;
			while(sequences[count]->getWeight() <= selection) {
				selection -= sequences[count]->getWeight();
				count++;
			}
			setSequenceByIndex(indexSequences[count]);
	}

	return true;

} // setSequence(actionTags_t)

//---------------------------------------------------------------------------

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
unsigned int MDLInstance::getSequence(const std::string& name) {

	assert(_model);

	// Recorremos todas las secuencias del modelo buscando la que tenga
	// ese nombre.
	unsigned int c = 0;
	while ((c < _model->getNumSequences()) &&
			(name != _model->getSequence(c)->getName()))
		c++;

	return c;

} // getSequence

//---------------------------------------------------------------------------

/**
 * Devuelve cierto o falso indicando si existe una secuencia en el
 * modelo con un identificador est�ndar de secuencia.
 * 
 * @param seq Identificador de secuencia estandar buscada.
 * @return Cierto si existe al menos una secuencia con ese identificador.
 */
bool MDLInstance::existSequence(HalfLifeMDL::actionTags_t seq) {

	unsigned int count = 0;

	while (count < _model->getNumSequences()) {
		if (_model->getSequence(count)->getType() == seq)
			return true;
	}

	return false;

} // existSequence

//---------------------------------------------------------------------------

/**
 * Devuelve el sistema de coordenadas creado por un punto de uni�n
 * en la secuencia y fotograma actual del modelo.
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
void MDLInstance::getAttachment(unsigned int attachment, MDLPoint& x,
								MDLPoint& y, MDLPoint& z, MDLPoint& trans) {
	assert(_model != NULL);
	assert(attachment < _model->getNumAttachments());

	const MDLAttachment* at;
	unsigned int bone;

	at = _model->getAttachment(attachment);
	bone = at->getIndexBone();

	// La informaci�n de los vectores directores la sacamos directamente de
	// la matriz de transformaci�n del hueso del que depende el punto
	// de uni�n.
	x.set(_boneMatrices[bone][0][0],
			_boneMatrices[bone][1][0], _boneMatrices[bone][2][0]);
	y.set(_boneMatrices[bone][0][1],
			_boneMatrices[bone][1][1], _boneMatrices[bone][2][1]);
	z.set(_boneMatrices[bone][0][2],
			_boneMatrices[bone][1][2], _boneMatrices[bone][2][2]);

	// El desplazamiento es la posici�n del punto de control respecto
	// al hueso del que depende.
	trans = _boneMatrices[bone] * at->getOffset();

} // getAttachment

//---------------------------------------------------------------------------

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
void MDLInstance::update(float time) {

	assert(_model);
	if (_stateChanged) {
		// Las cosas han cambiado desde la �ltima vez que nos llamaron.
		// Para hacerlo m�s seguro ante hebras, hacemos una copia del
		// estado original y trabajamos sobre �l.
		ModelState state;
		state = _nextState;

		// Analizamos cual ha sido el cambio para actualizar los datos
		// utilizados en la parte del control de la geometr�a.
		if (_submodelChanged) {
			// Hay que mirar cuantas mallas tenemos ahora. Su n�mero
			// ser� la suma del n�mero de mallas de los recubrimientos
			// (submodelos) de todas las partes del modelo.
			_numMeshes = 0;
			for (int i = 0; i < getNumBodyParts(); i++) {
				const MDLSubmodel* submodel;
				submodel = _model->getBodyPart(i)->getModel(
												state.selectedModels[i]);
				_numMeshes += submodel->getNumMeshes();
			}
		} // _submodelChanged

		// Miramos si ha cambiado la secuencia. Si es as�, establecemos
		// el fotograma 0 de la nueva secuencia, y empezamos a contar el
		// tiempo de nuevo. Es decir, tras un cambio de secuencia, siempre
		// se mostrar� el fotograma 0 la pr�xima vez que se llame a update.
		if (_currentState.sequence != state.sequence) {
			// Ha habido cambio de secuencia. La reiniciamos y anulamos
			// la cuenta de tiempo antigua.
			_frame = 0;
			_validLastTime = 0;
			_nextEvent = 0;
			if (getSequence()->getNumEvents() == 0)
				_frameNextEvent = UINT_MAX;
			else
				_frameNextEvent = getSequence()->getEvent(0)->getNumFrame();
		}

		// Actualizamos el estado que es "visible desde este lado", es
		// decir el utilizado por los m�todos que proporcionan informaci�n
		// de la geometr�a.
		_currentState = state;
		_stateChanged = false;
	}

	if (_validLastTime) {
		const MDLSequence* seq;
		seq = _model->getSequence(_currentState.sequence);
		unsigned int lastFrame;
		// Guardamos en lastFrame el �ltimo frame empezando a contar por 0.
		lastFrame = seq->getNumFrames() - 1;

		// S�lo nos complicamos la vida si la secuencia tiene m�s de un fotograma
		// o si no hemos llegado al final.
		if ( (lastFrame > 0) && (seq->isCycle() || _frame < lastFrame)) {
			// Definitivamente tenemos que avanzar.
			_frame += (time - _lastTime) * seq->getFPS();
			checkEvents();	// Si nos hemos salido de la animaci�n, checkEvents
							// se dar� cuenta y no tendr� problemas.
// C�digo necesario si checkEvents() puede ocasionar un cambio autom�tico de
// secuencia actual. Ver el comentario en checkEvents().
// seq = _model->getSequence(_currentState.sequence);
			// Miramos si no nos hemos salido de la animaci�n, para tomar las
			// medidas oportunas.
			if (!seq->isCycle()) {
				if (_frame >= lastFrame) {
					// Nos hemos pasado. Nos quedamos en el �ltimo fotograma.
					_frame = lastFrame;
					// TODO: Si finalmente siempre que una animaci�n que no 
					// es un ciclo generar� un evento al acabar, deber�a ir
					// aqu�.
				}
			} // if que miraba si la secuencia era un ciclo.
			else {
				// La secuencia es un ciclo. Tenemos que analizar el fotograma
				// final, por si nos hemos salido para empezar de nuevo.
				// Para evitar tener problemas en ordenadores lentos en los
				// que podr�amos haber dado m�s de una vuelta a la animaci�n
				// entre las dos llamadas a update consecutivas, no utilizamos
				// un if y una resta.
				// En su lugar, calculamos "la vuelta en la que estamos", es
				// decir cogemos el fotograma final y lo dividimos por el
				// n�mero de fotogramas de la secuencia, pasandolo a entero.
				// Si el nuevo fotograma est� dentro del margen de fotogramas
				// del ciclo, el resultado ser� 0, y ser� mayor que 0 en
				// caso contrario.
				// El n�mero de vueltas lo multiplicamos por el n�mero de
				// fotogramas de la secuencia, y obtendr�amos as� el
				// n�mero de fotogramas que tenemos que ir para atr�s para
				// caer al mismo fotograma pero dentro del margen de la
				// secuencia. Ese valor se lo restamos al fotograma actual
				// y arreglado.
				unsigned int toInt;
				toInt = (unsigned int) _frame / lastFrame;
				_frame -= toInt * lastFrame;
				// Afortunadamente, si hemos dado varias vueltas no queremos
				// generar los eventos de todas las vueltas que hemos dado y
				// no hemos dibujado. Si hemos dado alguna vuelta, volvemos
				// a mirar los eventos reiniciando la cuenta.
				if (toInt != 0) {
					_nextEvent = 0;
					if (getSequence()->getNumEvents() == 0)
						_frameNextEvent = UINT_MAX;
					else
						_frameNextEvent = getSequence()->getEvent(0)->
																getNumFrame();
					checkEvents();
				}
			}

			// TODO: analizar los eventos.

		}
	}
	else
		// Ya tenemos un tiempo con el que comparar.
		_validLastTime = true;

	// Obtenemos las nuevas matrices de transformaci�n de todos los huesos
	updateMatrices();

	// Forzamos a que, pase lo que pase, en la pr�xima llamada a selectMesh
	// se proyecten los v�rtices.
	_boneMatricesChanged = true;

	// Almacenamos el instante actual para comparar la pr�xima vez que nos
	// llamen.
	_lastTime = time;

} // update

//---------------------------------------------------------------------------

/**
 * Establece como activa la malla de �ndice indicado. Las llamadas
 * que se realicen a este m�todo pueden ser costosas en tiempo. Est�
 * optimizada para que sea llamada con mallas consecutivas, es decir
 * en un recorrido de las mallas desde la 0 hasta la �ltima el
 * establecimiento de la siguiente malla ser� m�s r�pido que en el
 * mismo recorrido en sentido inverso, o en cualquier otro orden.
 * @param mesh �ndice de la malla a establecer.
 */
MDLMesh::MeshType MDLInstance::selectMesh(unsigned int mesh) {

	assert(_model);
	assert(mesh < _numMeshes);

	// Primero miramos los casos especiales para los que el m�todo est�
	// "optimizado". Esos casos especiales son cuando la malla es la 0
	// (ser� la primera malla del modelo seleccionado de la primera
	// parte del cuerpo), o cuando la malla pedida sea la siguiente a
	// la actual (utilizamos los atributos para saber qu� malla es, y
	// saltar a la siguiente parte del cuerpo si es necesario).
	if (mesh == 0) {
		// La malla es la primera de todas.
		// TODO: podr�a mejorarse esto viendo si la primera malla
		// tiene los mismos v�rtices que la actual, y luego usar
		// _boneMatricesChanged para asegurarse de que no hay que
		// actualizar los v�rtices transformados de todas formas.
		_currentBodyPart = 0;
		_currentSubmodelMesh = 0;
		_selectedMesh = 0;
		updateVertices();
		updateNormals();
	}
	else if ((mesh - 1) == _selectedMesh) {
		// Se ha pedido la siguiente malla. Miramos si incrementar en
		// uno el n�mero de malla supone un cambio en la parte del cuerpo.

		unsigned int selSubmdl;
		const MDLSubmodel *submodel;

		// Metemos en selSubmdl el n�mero del recubrimiento
		// seleccionado en la parte del cuerpo al que pertenece la malla
		// mapeada por la malla _selectedMesh.
		selSubmdl = _currentState.selectedModels[_currentBodyPart];

		// Metemos en submodel el submodelo selectedSubmodel de la parte
		// del cuerpo al que pertenece la malla mapeada por la malla
		// _selectedMesh.
		submodel = _model->getBodyPart(_currentBodyPart)->getModel(selSubmdl);

		if ((_currentSubmodelMesh + 1) == submodel->getNumMeshes()) {
			// Hay un cambio de parte del cuerpo. Nos toca currar.
			_currentSubmodelMesh = 0;
			_currentBodyPart++;
			_selectedMesh = mesh;
			updateVertices();
			updateNormals();
		}
		else {
			// El avance es dentro de una malla del recubrimiento de la
			// parte del cuerpo en la que estamos (justo esta es la
			// optimizaci�n :-D). No tenemos que calcular la proyecci�n de
			// los v�rtices.
			_currentSubmodelMesh++;
			_selectedMesh = mesh;
			// En realidad si las matrices de transformaci�n han cambiado
			// (se ha llamado a update) s� habr� que calcular de nuevo las
			// proyecciones. Esto ser� poco probable s
			if (_boneMatricesChanged) {
				updateVertices();
				updateNormals();
			}
		}
	} // if (mesh + 1 == selectedMesh)
	else {
		// Caso de no optimizaci�n. Tenemos que recorrer las partes del cuerpo
		// para ver a cual pertenece la malla que nos han solicitado, y
		// luego calcular la proyecci�n de los v�rtices.
		_currentBodyPart = 0;
		_currentSubmodelMesh = 0;
		_selectedMesh = mesh;
		unsigned int selSubmdl;
		const MDLSubmodel* submodel;
		selSubmdl = _currentState.selectedModels[0];
		submodel = _model->getBodyPart(0)->getModel(selSubmdl);
		while (_selectedMesh >= submodel->getNumMeshes()) {
			_selectedMesh -= submodel->getNumMeshes();
			_currentBodyPart++;
			selSubmdl = _currentState.selectedModels[_currentBodyPart];
			submodel =
				_model->getBodyPart(_currentBodyPart)->getModel(selSubmdl);
		}
		_currentSubmodelMesh = _selectedMesh;

		updateVertices();
		updateNormals();

	} // if-else que analizaba el tipo de cambio para aplicar o no
	  // optimizaci�n

	// Bajamos la bandera. Esto est� aqu� por claridad. Una forma m�s
	// �ptima ser�a hacerlo en updateVertices y as� nos evitar�amos este
	// establecmiento a falso en algunos casos.
	// Eso es debido a que desde aqu� siempre llamamos a updateVertices si
	// _boneMatricesChanged era cierto, aunque hay veces que llamamos a
	// updateVertices cuando _boneMatricesChanged es falso (pero no siempre,
	// y justo por eso optimizar�amos un poquit�n).
	_boneMatricesChanged = false;

	return getMeshType();

} // selectMesh

//---------------------------------------------------------------------------

/**
 * Devuelve el tipo de la malla actual.
 * @return Tipo de la malla actual.
 * @see MDLMesh::MeshType
 */
MDLMesh::MeshType MDLInstance::getMeshType() const {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;

	assert(_model);

	// Metemos en selSubmdl el n�mero del recubrimiento
	// seleccionado en la parte del cuerpo al que pertenece la malla
	// mapeada por la malla _selectedMesh.
	selSubmdl = _currentState.selectedModels[_currentBodyPart];

	// Metemos en submodel el submodelo selectedSubmodel de la parte
	// del cuerpo al que pertenece la malla mapeada por la malla
	// _selectedMesh.
	submodel = _model->getBodyPart(_currentBodyPart)->getModel(selSubmdl);

	// Devolvemos el tipo del submodelo.
	return submodel->getMesh(_currentSubmodelMesh)->getMeshType();

} // getMeshType

//---------------------------------------------------------------------------

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
const unsigned int* MDLInstance::getVertexIndices() {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;

	assert(_model);

	// Metemos en selSubmdl el n�mero del recubrimiento
	// seleccionado en la parte del cuerpo al que pertenece la malla
	// mapeada por la malla _selectedMesh.
	selSubmdl = _currentState.selectedModels[_currentBodyPart];

	// Metemos en submodel el submodelo selectedSubmodel de la parte
	// del cuerpo al que pertenece la malla mapeada por la malla
	// _selectedMesh.
	submodel = _model->getBodyPart(_currentBodyPart)->getModel(selSubmdl);

	// Devolvemos los v�rtices del submodelo.
	return submodel->getMesh(_currentSubmodelMesh)->getVertices();

} // getVertexIndices

//---------------------------------------------------------------------------

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
const MDLBasicType* MDLInstance::getTextureCoords() {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;

	assert(_model);

	// Metemos en selSubmdl el n�mero del recubrimiento
	// seleccionado en la parte del cuerpo al que pertenece la malla
	// mapeada por la malla _selectedMesh.
	selSubmdl = _currentState.selectedModels[_currentBodyPart];

	// Metemos en submodel el submodelo selectedSubmodel de la parte
	// del cuerpo al que pertenece la malla mapeada por la malla
	// _selectedMesh.
	submodel = _model->getBodyPart(_currentBodyPart)->getModel(selSubmdl);

	// Devolvemos las normales del submodelo.
	return submodel->getTextureCoords();

} // getTexCoords

//---------------------------------------------------------------------------

/**
 * Devuelve la textura de la malla seleccionada actualmente (establecida
 * con MDLInstance::selectMesh). Para acelerar la ejecuci�n del m�todo, no
 * se comprueba que hay malla seleccionada (salvo en modo depuraci�n con
 * assert), por lo que no se devuelve NULL ante error y el resultado ser�
 * indefinido si no hay malla seleccionada.
 * @return Textura de la malla seleccionada.
 */
const MDLTexture* MDLInstance::getTexture() const {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;
	unsigned int texture;

	assert(_model);

	// Metemos en selSubmdl el n�mero del recubrimiento
	// seleccionado en la parte del cuerpo al que pertenece la malla
	// mapeada por la malla _selectedMesh.
	selSubmdl = _currentState.selectedModels[_currentBodyPart];

	// Metemos en submodel el submodelo selectedSubmodel de la parte
	// del cuerpo al que pertenece la malla mapeada por la malla
	// _selectedMesh.
	submodel = _model->getBodyPart(_currentBodyPart)->getModel(selSubmdl);

	texture = submodel->getMesh(_currentSubmodelMesh)->getSkinTexture();

	return _model->getSkins()->getTexture(_currentState.skin, texture);
	
} // getTexture

//---------------------------------------------------------------------------

/**
 * Devuelve el �ndice de la textura de la malla seleccionada actualmente
 * (establecida con MDLInstance::selectMesh). Para acelerar la ejecuci�n
 * del m�todo, no se comprueba que hay malla seleccionada (salvo en modo
 * depuraci�n con assert), por lo que no se devuelve NULL ante error y el
 * resultado ser� indefinido si no hay malla seleccionada.
 * @return �ndice de la teextura de la malla seleccionada.
 */
const unsigned int MDLInstance::getTextureIndex() const {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;
	unsigned int texture;

	assert(_model);

	// Metemos en selSubmdl el n�mero del recubrimiento
	// seleccionado en la parte del cuerpo al que pertenece la malla
	// mapeada por la malla _selectedMesh.
	selSubmdl = _currentState.selectedModels[_currentBodyPart];

	// Metemos en submodel el submodelo selectedSubmodel de la parte
	// del cuerpo al que pertenece la malla mapeada por la malla
	// _selectedMesh.
	submodel = _model->getBodyPart(_currentBodyPart)->getModel(selSubmdl);

	texture = submodel->getMesh(_currentSubmodelMesh)->getSkinTexture();

	return _model->getSkins()->getTextureIndex(_currentState.skin, texture);

} // getTextureIndex

//---------------------------------------------------------------------------

/**
 * Devuelve el n�mero de v�rtices de la malla seleccionada.
 * Para acelerar la ejecuci�n  del m�todo, no se comprueba que hay
 * malla seleccionada (salvo en modo depuraci�n con assert), por lo
 * que no se devuelve 0 ante error y el resultado ser� indefinido si
 * no hay malla seleccionada.
 * @return N�mero de v�rtices de la malla seleccionada.
 */
unsigned int MDLInstance::getNumVertices() const {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;

	assert(_model);

	// Metemos en selSubmdl el n�mero del recubrimiento
	// seleccionado en la parte del cuerpo al que pertenece la malla
	// mapeada por la malla _selectedMesh.
	selSubmdl = _currentState.selectedModels[_currentBodyPart];

	// Metemos en submodel el submodelo selectedSubmodel de la parte
	// del cuerpo al que pertenece la malla mapeada por la malla
	// _selectedMesh.
	submodel = _model->getBodyPart(_currentBodyPart)->getModel(selSubmdl);

	// Devolvemos el tipo del submodelo.
	return submodel->getNumVertices();

} // getNumVertices

//---------------------------------------------------------------------------

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
unsigned int MDLInstance::getNumVertexIndices() {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;

	assert(_model);

	// Metemos en selSubmdl el n�mero del recubrimiento
	// seleccionado en la parte del cuerpo al que pertenece la malla
	// mapeada por la malla _selectedMesh.
	selSubmdl = _currentState.selectedModels[_currentBodyPart];

	// Metemos en submodel el submodelo selectedSubmodel de la parte
	// del cuerpo al que pertenece la malla mapeada por la malla
	// _selectedMesh.
	submodel = _model->getBodyPart(_currentBodyPart)->getModel(selSubmdl);

	// Devolvemos el tipo del submodelo.
	return submodel->getMesh(_currentSubmodelMesh)->getNumVertices();

} // getNumVertexIndices

//---------------------------------------------------------------------------

/**
 * Devuelve la posici�n de un hueso en el fotograma actual. Este m�todo
 * s�lo tiene utilidad en depuraci�n, para poder dibujar los huesos
 * adem�s del modelo.
 * @param bone Hueso cuya posici�n se desea conocer.
 * @param bonePos Par�metro de salida con la posici�n del hueso
 * solicitado.
 */
void MDLInstance::getBonePos(unsigned int bone, MDLPoint& bonePos) {

	assert(_model);
	assert(bone < _model->getNumBones());

	// Hay que dar la posici�n del punto (0,0,0) respecto al hueso
	// <bone>. Ser�a como multiplicar ese v�rtice por su matriz de
	// transformaci�n, pero aprovechamos que el punto es el (0,0,0)
	// para no perder el tiempo. Basta con dar la parte del
	// desplazamiento de la matriz de transformaci�n.

	bonePos.x() = _boneMatrices[bone][0][3];
	bonePos.y() = _boneMatrices[bone][1][3];
	bonePos.z() = _boneMatrices[bone][2][3];

} // getBonePos

//---------------------------------------------------------------------------

/**
 * Libera toda la memoria auxiliar necesaria para controlar la
 * din�mica del modelo. Controla que los punteros sean NULL.
 */
void MDLInstance::free() {

	_currentState.free();
	_nextState.free();

	if (_vertices) {
		delete[] _vertices;
		_vertices = NULL;
	}
	if (_normals) {
		delete[] _normals;
		_normals = NULL;
	}
	if (_boneMatrices) {
		delete[] _boneMatrices;
		_boneMatrices = NULL;
	}
	if (_boneQuaternions) {
		delete[] _boneQuaternions;
		_boneQuaternions = NULL;
	}
	if (_bonePositions) {
		delete[] _bonePositions;
		_bonePositions = NULL;
	}
	if (_animationWeights) {
		delete[] _animationWeights;
		_animationWeights = NULL;
	}

} // free

//---------------------------------------------------------------------------

/**
 * Calcula en MDLInstance::_vertices la transformaci�n de la malla
 * seleccionada actual, indicada por MDLInstance::_currentBodyPart y 
 * MDLInstance::_currentSubmodelMesh.<br>
 * Es llamado por selectMesh, y utiliza las matrices de transformaci�n
 * de los huesos calculados en update.
 */
void MDLInstance::updateVertices() {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;
	const MDLPoint* vertices;
	const unsigned int* bones;
	const int* repeated;

	// Metemos en selSubmdl el n�mero del recubrimiento
	// seleccionado en la parte del cuerpo al que pertenece la malla
	// mapeada por la malla _selectedMesh.
	selSubmdl = _currentState.selectedModels[_currentBodyPart];

	// Metemos en submodel el submodelo selectedSubmodel de la parte
	// del cuerpo al que pertenece la malla mapeada por la malla
	// _selectedMesh.
	submodel = _model->getBodyPart(_currentBodyPart)->getModel(selSubmdl);

	vertices = submodel->getVertices();
	bones = submodel->getBones();
	repeated = submodel->getRepeatedVertices();
	float* vertexCoord = _vertices;
	for (int i = 0; i < submodel->getNumVertices(); i++) {
		if (repeated[i] == 0) {
			// Hay que realizar la transformaci�n.
			// Multiplicamos cada v�rtice por la matriz de transformaci�n
			// del hueso al que est� asociado el v�rtice, y guardamos el
			// resultado en la matriz de v�rtices transformados.
			unsigned int boneCurrentVertex = *bones;
			// Estamos actualizando la posici�n 0 del v�rtice (coordenada X)
			*vertexCoord = vertices->dotProduct(
								_boneMatrices[boneCurrentVertex][0]) +
								_boneMatrices[boneCurrentVertex][0][3];
			vertexCoord++;	// Pasamos a la siguiente coordenada "de salida".
			// Estamos actualizando la posici�n 1 del v�rtice (coordenada X)
			*vertexCoord = vertices->dotProduct(
								_boneMatrices[boneCurrentVertex][1]) +
								_boneMatrices[boneCurrentVertex][1][3];
			vertexCoord++;	// Pasamos a la siguiente coordenada "de salida".
			// Estamos actualizando la posici�n 2 del v�rtice (coordenada X)
			*vertexCoord = vertices->dotProduct(
								_boneMatrices[boneCurrentVertex][2]) +
								_boneMatrices[boneCurrentVertex][2][3];
			*vertexCoord++;
		}
		else {
			// Ya hemos manejado este v�rtice antes. Nos aprovechamos de
			// ello.
			unsigned int pos;
			pos = (i - repeated[i]) * 3;
			*vertexCoord = _vertices[pos++];
			vertexCoord++;
			*vertexCoord = _vertices[pos++];
			vertexCoord++;
			*vertexCoord = _vertices[pos++];
			vertexCoord++;
		}
		// Preparamos los punteros a la informaci�n del submodelo para
		// apuntar al siguiente v�rtice.
		vertices++;
		bones++;
	}

} // updateVertices

//---------------------------------------------------------------------------

/**
 * Calcula en MDLInstance::_normals la transformaci�n de las normales
 * de la malla seleccionada actualmente, indicada por
 * MDLInstance::_currentBodyPart y MDLInstance::_currentSubmodelMesh.<br>
 * Es llamado por selectMesh, y utiliza las matrices de transformaci�n
 * de los huesos calculados en update.
 */
void MDLInstance::updateNormals() {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;
	const MDLPoint* normals;
	const unsigned int* bones;
	const int* repeated;

	// Metemos en selSubmdl el n�mero del recubrimiento
	// seleccionado en la parte del cuerpo al que pertenece la malla
	// mapeada por la malla _selectedMesh.
	selSubmdl = _currentState.selectedModels[_currentBodyPart];

	// Metemos en submodel el submodelo selectedSubmodel de la parte
	// del cuerpo al que pertenece la malla mapeada por la malla
	// _selectedMesh.
	submodel = _model->getBodyPart(_currentBodyPart)->getModel(selSubmdl);

	normals = submodel->getNormals();
	bones = submodel->getBones();
	repeated = submodel->getRepeatedNormals();
	float* normalCoord = _normals;
	for (int i = 0; i < submodel->getNumVertices(); i++) {
		if (repeated[i] == 0) {
			// Multiplicamos cada normal por la matriz de transformaci�n
			// del hueso al que est� asociado la normal, y guardamos el
			// resultado en la matriz de normales transformados.
			// En realidad no hay que usar la matriz de transformaci�n
			// completa, pues estamos transformando vectores libres.
			// Simplemente rotamos, no usamos la traslaci�n.
			unsigned int boneCurrentNormal = *bones;
			// Estamos actualizando la posici�n 0 de la normal (coordenada X)
			*normalCoord = normals->dotProduct(
								_boneMatrices[boneCurrentNormal][0]);
			normalCoord++;	// Pasamos a la siguiente coordenada "de salida".
			// Estamos actualizando la posici�n 1 de la normal (coordenada X)
			*normalCoord = normals->dotProduct(
								_boneMatrices[boneCurrentNormal][1]);
			normalCoord++;	// Pasamos a la siguiente coordenada "de salida".
			// Estamos actualizando la posici�n 2 de la normal (coordenada X)
			*normalCoord = normals->dotProduct(
								_boneMatrices[boneCurrentNormal][2]);
			normalCoord++;
		}
		else {
			// Ya hemos manejado este v�rtice antes. Nos aprovechamos de
			// ello.
			unsigned int pos;
			pos = (i - repeated[i]) * 3;
			*normalCoord = _normals[pos++];
			normalCoord++;
			*normalCoord = _normals[pos++];
			normalCoord++;
			*normalCoord = _normals[pos++];
			normalCoord++;
		}
		// Preparamos los punteros a la informaci�n del submodelo para
		// apuntar a la siguiente normal.
		normals++;
		bones++;
	}

} // updateNormals

//---------------------------------------------------------------------------

/**
 * Calcula las matrices de transformaci�n de los v�rtices asociados
 * a cada hueso en el array MDLInstance::_boneMatrices.
 */
void MDLInstance::updateMatrices() {

	unsigned int boneIndex;
	const MDLBone** bone = _model->getBones();
	float fractionFrame;
	int intFrame;
	const MDLBoneAnimation** anims;

	intFrame = _frame;
	fractionFrame = _frame - intFrame;

	// TODO: si en alg�n momento se quiere permitir mostrar la posici�n por
	// defecto (habr�a que ampliar el API, o algo), ser�a sencillo ampliar
	// todo esto a partir del c�digo comentado.

	// if (hay animaci�n seleccionada)
	anims = (_model->getSequence(_currentState.sequence))->getAnimations();
	// else
	//	anims = null;

	/* if (anims == NULL) {
		for (boneIndex = 0; boneIndex < _model->getNumBones();
											boneIndex++, bone++) {
			_boneQuaternions[boneIndex].setAngles(
											(*bone)->getDefaultValue(DoF_RX),
											(*bone)->getDefaultValue(DoF_RY),
											(*bone)->getDefaultValue(DoF_RZ));

			_bonePositions[boneIndex].set((*bone)->getDefaultValue(DoF_X),
											(*bone)->getDefaultValue(DoF_Y),
											(*bone)->getDefaultValue(DoF_Z));
	}
	else */
	// Lo primero que hay que hacer es obtener el cuaterni�n almacenando
	// la rotaci�n de cada hueso respecto al padre, y su posici�n
	// (desplazamiento), mezclando todas las animaciones de la secuencia.
	MDLBasicType acum;
	acum = 0;
	for (unsigned int indexAnim = 0; indexAnim < getNumAnimations(); 
														indexAnim++) {
		float fraction;
		acum += _animationWeights[indexAnim];
		if (_animationWeights[indexAnim] == 0.0) {
			// No hay nada que hacer con la animaci�n actual, porque tiene
			// peso nulo. Pasamos a la siguiente vuelta del bucle, saltandonos
			// toda la informaci�n de esta animaci�n.
			// Esta comprobaci�n nos evita de paso la divisi�n por cero si
			// acum a�n es 0. Es decir, si la secuencia tiene 4 animaciones, y
			// las dos primeras tienen peso 0, acum valdr� 0 al principio, y
			// no deber�amos dividir por acum. No es necesario hacer esa
			// comprobaci�n expl�cita, porque para que acum valga cero
			// tambi�n debe valer 0 _animationWeights, de ah� que baste con
			// comprobar eso. Bueno... m�s o menos, pero t� me entiendes :-D
			anims += _model->getNumBones();
			continue;
		}
		fraction = _animationWeights[indexAnim] / acum;
//std::cout << indexAnim << ": " << fraction << "(" << _animationWeights[indexAnim] << ");";
		for (boneIndex = 0; boneIndex < _model->getNumBones(); 
											boneIndex++, bone++) {
			// bone apunta al puntero al hueso boneIndex del modelo
			// Almacenamos en _boneQuaternions[boneIndex] el cuaterni�n del
			// hueso boneIndex (*bone) respecto a su hueso padre, y en
			// _bonePositions[boneIndex] la posici�n.

			(*anims)->CalcBonePosition(intFrame, fractionFrame,
									_bonePositions[boneIndex],
									0, 0, 0, fraction);
			(*anims)->CalcBoneQuaternion(intFrame, fractionFrame,
									_boneQuaternions[boneIndex],
									0, 0, 0, fraction);
			anims++;
		} // for de c�lculo de cuaterniones y desplazamientos de los huesos.
	} // for que recorr�a todas las animaciones de la secuencia actual.
//std::cout << std::endl;
	// En _boneQuaternions tenemos los cuaterniones de los huesos y en
	// _bonePositions los desplazamientos respecto al padre. Tenemos que
	// calcular las matrices de cada uno. Recorremos todos los huesos
	// calculando la matriz respecto al padre, y luego la multiplicamos
	// por la matriz del padre. Se supone que los huesos est�n en
	// preorden en el modelo.
	bone = _model->getBones();
	for (boneIndex = 0; boneIndex < _model->getNumBones(); boneIndex++, bone++) {
		int parentIndex;
		// Establecemos la matriz de rotaci�n a partir del cuaterni�n.
		_boneQuaternions[boneIndex].toMatrix(_boneMatrices[boneIndex]);

		// Establecemos la parte del desplazamiento.
		_boneMatrices[boneIndex][0][3] = _bonePositions[boneIndex][0];
		_boneMatrices[boneIndex][1][3] = _bonePositions[boneIndex][1];
		_boneMatrices[boneIndex][2][3] = _bonePositions[boneIndex][2];

		// En _boneMatrices[boneIndex] tenemos la matriz de transformaci�n
		// del hueso respecto al hueso padre. Queremos la matriz absoluta,
		// a si es que la multiplicamos por la matriz del padre. Para que
		// esto funcione, la matriz del padre ya debe haberse calculado,
		// es decir, los huesos deben estar en preorden.
		parentIndex = (*bone)->getParentIndex();
		if (parentIndex != -1) {	// No somos el hueso padre
			assert (parentIndex < (int) boneIndex ||
						!"Los huesos del modelo no est�n en preorden.");
			_boneMatrices[boneIndex] = _boneMatrices[parentIndex] * 
												_boneMatrices[boneIndex];
		}

	} // for de c�lculo de las matrices de los huesos.

} // updateMatrices

//---------------------------------------------------------------------------

/**
 * Comprueba si con el �ltimo avance del fotograma actual se ha
 * atravesado alg�n evento de la secuencia que haya que generar.
 * Es un m�todo auxiliar usado en update(). Si hay alg�n evento
 * nuevo, se avisa a los oyentes.
 */
void MDLInstance::checkEvents() {

	while (_frameNextEvent < _frame) {
		// Hay que lanzar el siguiente evento.

/*
		Si se quiere que el modelo realice "transiciones" entre dos 
		secuencias (por ejemplo de idle a walk), se puede crear una
		secuencia mezcla de ambas animaciones, o mezclar las dos
		secuencias o algo as�. Seg�n Daniel Sanchez Crespo, esa
		alternativa da buen resultado y no queda mal porque la mezcla
		de los huesos es razonable.
		Otra alternativa es pedir a los dise�adores que hagan las
		transiciones, teniendo un fotograma "clave" que se utiliza
		como intermedio entre animaciones.
		As�, tendr�amos una animaci�n de inicio de andar, que, nada
		m�s acabar, debe autom�ticamente forzar el comienzo de
		la animaci�n de andar. Del mismo modo, cuando el usuario
		deja de andar tenemos que enlazar el �ltimo fotograma de
		esa secuencia con la secuencia de dejar de andar.
		Parece que el formato MDL tiene informaci�n para guardar
		ese tipo de cosas (qu� secuencia debe seguir a una dada),
		pero no hemos encontrado el modo de establecer eso en el
		fichero .qc de compilaci�n de MDLs.
		La alternativa para implementar esto es utilizar eventos
		en las animaciones, y que el gestor de eventos modifique
		la secuencia convenientemente.
		El problema es que para que el enlace quede fluido, ese
		cambio debe tener en cuenta el fotograma actual _real_
		(no el fotograma que ha generado el salto del evento),
		adem�s deber�a tener en cuenta la parte flotante de dicho
		frame para que la interpolaci�n no haga crujir el enlace...

		La alternativa que nos planteamos fue que ese trabajo lo
		hiciera el MDLInstance directamente. Nos inventamos un
		evento especial predefinido (cuyo identificador establece
		la constante de precompilaci�n CHANGE_SEQUENCE_EVENT) y
		si se encuentra ese evento, desde aqu� saltamos a la secuencia
		establecida en el par�metro del evento.

		Realmente todo esto fueron solo pruebas r�pidas para ver si
		el enlace era posible. Implementar eso correctamente requerir�a
		un poco m�s de esfuerzo. Por ejemplo, ser�a razonable que
		antes de realizar el cambio se preguntara si se desea realizar.
		As� la animaci�n de andar tendr�a el enlace con la animaci�n
		de parar de andar, y se preguntar�a cont�nuamente si se quiere
		pasar a ella o no. Incluso podr�a permitirse "sobreescribir" a
		qu� secuencia pasar, para poder empezar a correr o algo as�.

		Este cambio tambi�n implica a la clase que controle a esta por
		encima, que es la que debe encargarse de desplazar el modelo
		en el entorno en aquellas animaciones que lo tengan. Si desde
		aqu� cambiamos la secuencia de modo "silencioso", podr�amos
		probocar un cambio en la velocidad de desplazamiento de la que
		"arriba" no necesariamente ser�an conscientes. La creaci�n del
		evento anterior permitir�a notificar el cambio.

		Si se quieren utilizar los identificadores de animaciones
		predefinidos (ACT_WALK, y cosas as�) en los MDL con transiciones,
		hay que desarrollar un "mapeo" entre identificadores no utilizados
		y animaciones con transiciones.
		En realidad existe un "problema" adicional. Es posible que lo
		l�gico ser�a asociar el identificador ACT_WALK a la secuencia de
		inicio de andar, y asociar alg�n otro identificador dummy al 
		verdadero ciclo. Esto tiene el problema de que en alg�n momento
		se pasar� a una secuencia diferente a ACT_WALK. Si desde arriba
		est�n estableciendo continuamente la secuencia de andar (porque
		se sigue detectando la pulsaci�n de tecla de andar del usuario
		o cualquier cosa por el estilo), confiando que desde aqu� si
		nos piden establecer una secuencia que es la activa no vamos
		a hacer nada, el asunto dejar� de funcionar, porque realmente
		cuando la transici�n termine, cambiaremos de animaci�n, nos
		volver�n a pedir que pongamos la de andar (que resulta ser s�lo
		el inicio del andar), y el resultado ser� que el personaje
		andar� con un "ciclo de la secuencia de inicio de andar".

		Por �ltimo, sin la existencia del "aviso hacia arriba" de la llegada
		del evento de cambio de secuencia, para volver a idle cuando se
		est� andando se necesitar�an dos animaciones: el ciclo de andar
		sin evento de cambio, y el ciclo de andar con �l, que pasa a la
		secuencia de fin de andar que enlaza a su vez con idle. Cuando
		el usuario decide dejar de andar, habr�a que establecer la
		animaci�n de ciclo de andar con evento, pero "silenciosamente",
		es decir, sin que cambie el fotograma actual (en realidad las
		dos secuencias son la misma). Para eso, se necesitar�a otro
		m�todo de setSameSequence o algo por el estilo.


		Total. Aqu� lo que hay es el c�digo que se meti� para probar los
		enlaces entre secuencias con el evento predefinido. Si se
		decide implementar esto, habr�a que revisarlo, quiz� meter
		el aviso del cambio de secuencia comentado antes, y vigilar
		la actualizaci�n de los eventos, para que los eventos de la
		nueva secuencia se llamen correctamente (ahora mismo est� sin
		probar nada).
*/

/*
		// Lo analizamos (el evento).
		
		if (getSequence()->getEvent(_nextEvent)->getEventCode()
										== CHANGE_SEQUENCE_EVENT) {
			// El evento que se ha producido es el evento especial de
			// cambio de secuencia que realizamos "de estrangis"

			// std::cout << "Cambio de animaci�n autom�tica a "
						 << getSequence()->getEvent(_nextEvent)->getOptions()
						 << " ";
			float restar = getSequence()->getEvent(_nextEvent)->getNumFrame();
			char aux[201];
			// sprintf(aux, "_frameNextEvent = %d, _frame = %f\n", 
			//			(int)_frameNextEvent, _frame);
			// std::cout << aux;
			// sprintf(aux, "\nAnimacion %s, fotograma %f, pasamos a "
						"animaci�n %s, fotograma %f por evento en "
						"fotograma %d\n", getSequence()->getName().c_str(),
						_frame, getSequence()->getEvent(_nextEvent)->getOptions().c_str(),
						_frame - restar, (int)restar);
			// std::cout << aux;
			_currentState.sequence = this->getSequence(getSequence()->getEvent(_nextEvent)->getOptions());
			_nextState.sequence = _currentState.sequence;
			_frame -= restar;
			if (getSequence()->getNumEvents() == 0)
				_frameNextEvent = UINT_MAX;
			else
				_frameNextEvent = getSequence()->getEvent(0)->getNumFrame();

			break;
		}*/
		std::vector<MDLEventListener*>::iterator it;
		for (it = _eventListeners.begin(); it != _eventListeners.end(); ++it)
			(*it)->eventRaised(this, _currentState.sequence, 
								getSequence()->getEvent(_nextEvent));

		++_nextEvent;
		if (getSequence()->getNumEvents() == _nextEvent)
			_frameNextEvent = UINT_MAX;
		else
			_frameNextEvent = getSequence()->getEvent(_nextEvent)->
															getNumFrame();
	}

} // checkEvents

//---------------------------------------------------------------------------
//						Implementaci�n de ModelState
//---------------------------------------------------------------------------

/**
 * Constructor. No se pide memoria.
 */
MDLInstance::ModelState::ModelState() : selectedModels(NULL),
										numModels(0), sequence(0), skin(0) {

	for (int i = 0; i < 4; i++)
		controller[i] = 0;

} // ModelState::constructor

//---------------------------------------------------------------------------

/** 
 * Pide memoria para almacenar el modelo (recubrimiento) asignado a
 * tantas partes del modelo como se indique en el par�metro.
 * @param numBodyParts N�mero de partes del cuerpo de las que se
 * tendr� que almacenar el recubrimiento seleccionado.
 */
void MDLInstance::ModelState::setNumberModels(unsigned int numBodyParts) {

	free();
	if (numBodyParts != 0)
		selectedModels = new unsigned int[numBodyParts];
	numModels = numBodyParts;

} // ModelState::setNumberModels

//---------------------------------------------------------------------------

void MDLInstance::ModelState::free() {

	if (selectedModels != NULL) {
		delete[] selectedModels;
		selectedModels = NULL;
		numModels = 0;
	}

} // ModelState::free

//---------------------------------------------------------------------------

/**
 * Operador de asignaci�n. Se utiliza copia profunda, con los punteros.
 */
MDLInstance::ModelState& MDLInstance::ModelState::operator=(
									const MDLInstance::ModelState& source) {

	sequence = source.sequence;
	skin = source.skin;
	for (int i = 0; i < 4; i++)  controller[i] = source.controller[i];

	if (numModels != source.numModels)
		setNumberModels(source.numModels);

	for (int c = 0; c < numModels; c++)
		selectedModels[c] = source.selectedModels[c];

	return *this;

} // ModelState::operator=

} // namespace HalfLifeMDL

