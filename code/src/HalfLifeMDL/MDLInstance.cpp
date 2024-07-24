//---------------------------------------------------------------------------
// MDLInstance.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLInstance.cpp
 * Contiene la definición (implementación) de la clase que almacena la 
 * información dinámica de un modelo de Half Life (HalfLifeMDL::MDLInstance).
 * Esta clase tiene métodos para establecer el modelo, la animación
 * actual, etc., y para obtener la información necesaria para el
 * renderizado del modelo.<p>
 * @see HalfLifeMDL::MDLCore
 * @see HalfLifeMDL::MDLInstance
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */

#include <limits.h>

#include <iostream>

#include "MDLInstance.h"
#include "MDLBoneAnimation.h"

/**
 * Constante que especifica el máximo número de secuencias de un modelo
 * que pueden tener asociada el mismo identificador de secuencia estándar.
 * <p>
 * En realidad esta restricción no se impone sobre los ficheros leídos,
 * sino sobre el método getSequence(HalfLifeMDL::actionTags_t). Si hay
 * más de MAX_SEQUENCES_SAME_STANDAR_MOVEMENT secuencias del mismo
 * tipo, ese método sólo considerará a las primeras.
 */
#define MAX_SEQUENCES_SAME_STANDAR_MOVEMENT 5
// Si se cambia este valor, debería modificarse la documentación de
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
} // Constructor sin parámetros

//---------------------------------------------------------------------------

/**
 * Constructor a partir de un MDLCore.
 * @param model Modelo que se instancia en el objeto. Debe
 * estar inicializado correctamente. El objeto no es liberado por
 * el destructor de la clase, para permitir compartir el mismo
 * modelo en varias instancias y que no se produzcan problemas en
 * los destructores. Por la misma razón tampoco se realiza copia.
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
 * aunque sí la memoria utilizada internamente.
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
 * los destructores. Por la misma razón tampoco se realiza copia.
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

	// Recorremos el modelo para ver qué recubrimiento tiene más vértices
	// y crear matrices de tamaño suficiente para que entren.
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

		} // for que recorría todos los modelos.
	} // for que recorría todas las partes del cuerpo

	// En maxVertices tenemos el número de vértices que tiene el recubrimiento
	// más grande. Pedimos memoria para el array donde guardaremos los
	// vértices transformados.
	// Los vértices están todos seguidos en un array de coordenadas, con
	// tres coordenadas por vértice: [x0. y0, z0, x1, y1, z1, ...]
	// Ocurre lo mismo con las normales.
	_vertices = new MDLBasicType[maxVertices*3];
	_tamVerticesArray = maxVertices;

	_normals = new MDLBasicType[maxVertices*3];

	_boneMatrices = new MDLMatrix[model->getNumBones()];
	_boneQuaternions = new MDLQuaternion[model->getNumBones()];
	_bonePositions = new MDLPoint[model->getNumBones()];

	// Miramos qué secuencia tiene más animaciones para crear el array
	// donde se guardarán los pesos.
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

	// Llamamos a update para que se actualice la información del número
	// de mallas.
	// Para que update no haga cosas que no queremos, utilizamos algún truco.
	// Establecemos como animación actual una diferente a la "anterior" para
	// forzar a comenzar por el fotograma cero. También indicamos que
	// el submodelo ha cambiado, para que cuente cuantas mallas tenemos.
	_stateChanged = true;
	_submodelChanged = true;	// Forzamos el cálculo del número de mallas.
	_currentState.sequence = 1;
	_nextState.sequence = 0;	// Se establece la secuencia 0 por defecto.
	update(0);
	
	// El parámetro pasado a update no es válido. Es un valor dummy
	// para que update haga su trabajo. Anulamos el registro de ese tiempo
	// incorrecto.
	_validLastTime = false;

} // attachMDLCore

//---------------------------------------------------------------------------

/**
 * Establece la secuencia actual. Si se establece como actual
 * la secuencia que se está reproduciendo en este momento, la llamada
 * no tiene ningún efecto. <br>
 * Si la secuencia tiene varias animaciones, se establece como peso para
 * la primera de ellas 1.0, y 0.0 para todas las demás.
 * @param index Índice de la secuencia actual.
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
			// números aleatorios.
			setSequenceByIndex(indexSequences[0]);
			break;
		default:
			// Nos toca elegir una. Seleccionamos un número aleatorio entre
			// 0 y el peso total menos uno, y luego miramos a qué secuencia
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
 * modelo con un identificador estándar de secuencia.
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
 * Devuelve el sistema de coordenadas creado por un punto de unión
 * en la secuencia y fotograma actual del modelo.
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
void MDLInstance::getAttachment(unsigned int attachment, MDLPoint& x,
								MDLPoint& y, MDLPoint& z, MDLPoint& trans) {
	assert(_model != NULL);
	assert(attachment < _model->getNumAttachments());

	const MDLAttachment* at;
	unsigned int bone;

	at = _model->getAttachment(attachment);
	bone = at->getIndexBone();

	// La información de los vectores directores la sacamos directamente de
	// la matriz de transformación del hueso del que depende el punto
	// de unión.
	x.set(_boneMatrices[bone][0][0],
			_boneMatrices[bone][1][0], _boneMatrices[bone][2][0]);
	y.set(_boneMatrices[bone][0][1],
			_boneMatrices[bone][1][1], _boneMatrices[bone][2][1]);
	z.set(_boneMatrices[bone][0][2],
			_boneMatrices[bone][1][2], _boneMatrices[bone][2][2]);

	// El desplazamiento es la posición del punto de control respecto
	// al hueso del que depende.
	trans = _boneMatrices[bone] * at->getOffset();

} // getAttachment

//---------------------------------------------------------------------------

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
void MDLInstance::update(float time) {

	assert(_model);
	if (_stateChanged) {
		// Las cosas han cambiado desde la última vez que nos llamaron.
		// Para hacerlo más seguro ante hebras, hacemos una copia del
		// estado original y trabajamos sobre él.
		ModelState state;
		state = _nextState;

		// Analizamos cual ha sido el cambio para actualizar los datos
		// utilizados en la parte del control de la geometría.
		if (_submodelChanged) {
			// Hay que mirar cuantas mallas tenemos ahora. Su número
			// será la suma del número de mallas de los recubrimientos
			// (submodelos) de todas las partes del modelo.
			_numMeshes = 0;
			for (int i = 0; i < getNumBodyParts(); i++) {
				const MDLSubmodel* submodel;
				submodel = _model->getBodyPart(i)->getModel(
												state.selectedModels[i]);
				_numMeshes += submodel->getNumMeshes();
			}
		} // _submodelChanged

		// Miramos si ha cambiado la secuencia. Si es así, establecemos
		// el fotograma 0 de la nueva secuencia, y empezamos a contar el
		// tiempo de nuevo. Es decir, tras un cambio de secuencia, siempre
		// se mostrará el fotograma 0 la próxima vez que se llame a update.
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
		// decir el utilizado por los métodos que proporcionan información
		// de la geometría.
		_currentState = state;
		_stateChanged = false;
	}

	if (_validLastTime) {
		const MDLSequence* seq;
		seq = _model->getSequence(_currentState.sequence);
		unsigned int lastFrame;
		// Guardamos en lastFrame el último frame empezando a contar por 0.
		lastFrame = seq->getNumFrames() - 1;

		// Sólo nos complicamos la vida si la secuencia tiene más de un fotograma
		// o si no hemos llegado al final.
		if ( (lastFrame > 0) && (seq->isCycle() || _frame < lastFrame)) {
			// Definitivamente tenemos que avanzar.
			_frame += (time - _lastTime) * seq->getFPS();
			checkEvents();	// Si nos hemos salido de la animación, checkEvents
							// se dará cuenta y no tendrá problemas.
// Código necesario si checkEvents() puede ocasionar un cambio automático de
// secuencia actual. Ver el comentario en checkEvents().
// seq = _model->getSequence(_currentState.sequence);
			// Miramos si no nos hemos salido de la animación, para tomar las
			// medidas oportunas.
			if (!seq->isCycle()) {
				if (_frame >= lastFrame) {
					// Nos hemos pasado. Nos quedamos en el último fotograma.
					_frame = lastFrame;
					// TODO: Si finalmente siempre que una animación que no 
					// es un ciclo generará un evento al acabar, debería ir
					// aquí.
				}
			} // if que miraba si la secuencia era un ciclo.
			else {
				// La secuencia es un ciclo. Tenemos que analizar el fotograma
				// final, por si nos hemos salido para empezar de nuevo.
				// Para evitar tener problemas en ordenadores lentos en los
				// que podríamos haber dado más de una vuelta a la animación
				// entre las dos llamadas a update consecutivas, no utilizamos
				// un if y una resta.
				// En su lugar, calculamos "la vuelta en la que estamos", es
				// decir cogemos el fotograma final y lo dividimos por el
				// número de fotogramas de la secuencia, pasandolo a entero.
				// Si el nuevo fotograma está dentro del margen de fotogramas
				// del ciclo, el resultado será 0, y será mayor que 0 en
				// caso contrario.
				// El número de vueltas lo multiplicamos por el número de
				// fotogramas de la secuencia, y obtendríamos así el
				// número de fotogramas que tenemos que ir para atrás para
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

	// Obtenemos las nuevas matrices de transformación de todos los huesos
	updateMatrices();

	// Forzamos a que, pase lo que pase, en la próxima llamada a selectMesh
	// se proyecten los vértices.
	_boneMatricesChanged = true;

	// Almacenamos el instante actual para comparar la próxima vez que nos
	// llamen.
	_lastTime = time;

} // update

//---------------------------------------------------------------------------

/**
 * Establece como activa la malla de índice indicado. Las llamadas
 * que se realicen a este método pueden ser costosas en tiempo. Está
 * optimizada para que sea llamada con mallas consecutivas, es decir
 * en un recorrido de las mallas desde la 0 hasta la última el
 * establecimiento de la siguiente malla será más rápido que en el
 * mismo recorrido en sentido inverso, o en cualquier otro orden.
 * @param mesh Índice de la malla a establecer.
 */
MDLMesh::MeshType MDLInstance::selectMesh(unsigned int mesh) {

	assert(_model);
	assert(mesh < _numMeshes);

	// Primero miramos los casos especiales para los que el método está
	// "optimizado". Esos casos especiales son cuando la malla es la 0
	// (será la primera malla del modelo seleccionado de la primera
	// parte del cuerpo), o cuando la malla pedida sea la siguiente a
	// la actual (utilizamos los atributos para saber qué malla es, y
	// saltar a la siguiente parte del cuerpo si es necesario).
	if (mesh == 0) {
		// La malla es la primera de todas.
		// TODO: podría mejorarse esto viendo si la primera malla
		// tiene los mismos vértices que la actual, y luego usar
		// _boneMatricesChanged para asegurarse de que no hay que
		// actualizar los vértices transformados de todas formas.
		_currentBodyPart = 0;
		_currentSubmodelMesh = 0;
		_selectedMesh = 0;
		updateVertices();
		updateNormals();
	}
	else if ((mesh - 1) == _selectedMesh) {
		// Se ha pedido la siguiente malla. Miramos si incrementar en
		// uno el número de malla supone un cambio en la parte del cuerpo.

		unsigned int selSubmdl;
		const MDLSubmodel *submodel;

		// Metemos en selSubmdl el número del recubrimiento
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
			// optimización :-D). No tenemos que calcular la proyección de
			// los vértices.
			_currentSubmodelMesh++;
			_selectedMesh = mesh;
			// En realidad si las matrices de transformación han cambiado
			// (se ha llamado a update) sí habrá que calcular de nuevo las
			// proyecciones. Esto será poco probable s
			if (_boneMatricesChanged) {
				updateVertices();
				updateNormals();
			}
		}
	} // if (mesh + 1 == selectedMesh)
	else {
		// Caso de no optimización. Tenemos que recorrer las partes del cuerpo
		// para ver a cual pertenece la malla que nos han solicitado, y
		// luego calcular la proyección de los vértices.
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
	  // optimización

	// Bajamos la bandera. Esto está aquí por claridad. Una forma más
	// óptima sería hacerlo en updateVertices y así nos evitaríamos este
	// establecmiento a falso en algunos casos.
	// Eso es debido a que desde aquí siempre llamamos a updateVertices si
	// _boneMatricesChanged era cierto, aunque hay veces que llamamos a
	// updateVertices cuando _boneMatricesChanged es falso (pero no siempre,
	// y justo por eso optimizaríamos un poquitín).
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

	// Metemos en selSubmdl el número del recubrimiento
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
const unsigned int* MDLInstance::getVertexIndices() {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;

	assert(_model);

	// Metemos en selSubmdl el número del recubrimiento
	// seleccionado en la parte del cuerpo al que pertenece la malla
	// mapeada por la malla _selectedMesh.
	selSubmdl = _currentState.selectedModels[_currentBodyPart];

	// Metemos en submodel el submodelo selectedSubmodel de la parte
	// del cuerpo al que pertenece la malla mapeada por la malla
	// _selectedMesh.
	submodel = _model->getBodyPart(_currentBodyPart)->getModel(selSubmdl);

	// Devolvemos los vértices del submodelo.
	return submodel->getMesh(_currentSubmodelMesh)->getVertices();

} // getVertexIndices

//---------------------------------------------------------------------------

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
const MDLBasicType* MDLInstance::getTextureCoords() {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;

	assert(_model);

	// Metemos en selSubmdl el número del recubrimiento
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
 * con MDLInstance::selectMesh). Para acelerar la ejecución del método, no
 * se comprueba que hay malla seleccionada (salvo en modo depuración con
 * assert), por lo que no se devuelve NULL ante error y el resultado será
 * indefinido si no hay malla seleccionada.
 * @return Textura de la malla seleccionada.
 */
const MDLTexture* MDLInstance::getTexture() const {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;
	unsigned int texture;

	assert(_model);

	// Metemos en selSubmdl el número del recubrimiento
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
 * Devuelve el índice de la textura de la malla seleccionada actualmente
 * (establecida con MDLInstance::selectMesh). Para acelerar la ejecución
 * del método, no se comprueba que hay malla seleccionada (salvo en modo
 * depuración con assert), por lo que no se devuelve NULL ante error y el
 * resultado será indefinido si no hay malla seleccionada.
 * @return Índice de la teextura de la malla seleccionada.
 */
const unsigned int MDLInstance::getTextureIndex() const {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;
	unsigned int texture;

	assert(_model);

	// Metemos en selSubmdl el número del recubrimiento
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
 * Devuelve el número de vértices de la malla seleccionada.
 * Para acelerar la ejecución  del método, no se comprueba que hay
 * malla seleccionada (salvo en modo depuración con assert), por lo
 * que no se devuelve 0 ante error y el resultado será indefinido si
 * no hay malla seleccionada.
 * @return Número de vértices de la malla seleccionada.
 */
unsigned int MDLInstance::getNumVertices() const {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;

	assert(_model);

	// Metemos en selSubmdl el número del recubrimiento
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
unsigned int MDLInstance::getNumVertexIndices() {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;

	assert(_model);

	// Metemos en selSubmdl el número del recubrimiento
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
 * Devuelve la posición de un hueso en el fotograma actual. Este método
 * sólo tiene utilidad en depuración, para poder dibujar los huesos
 * además del modelo.
 * @param bone Hueso cuya posición se desea conocer.
 * @param bonePos Parámetro de salida con la posición del hueso
 * solicitado.
 */
void MDLInstance::getBonePos(unsigned int bone, MDLPoint& bonePos) {

	assert(_model);
	assert(bone < _model->getNumBones());

	// Hay que dar la posición del punto (0,0,0) respecto al hueso
	// <bone>. Sería como multiplicar ese vértice por su matriz de
	// transformación, pero aprovechamos que el punto es el (0,0,0)
	// para no perder el tiempo. Basta con dar la parte del
	// desplazamiento de la matriz de transformación.

	bonePos.x() = _boneMatrices[bone][0][3];
	bonePos.y() = _boneMatrices[bone][1][3];
	bonePos.z() = _boneMatrices[bone][2][3];

} // getBonePos

//---------------------------------------------------------------------------

/**
 * Libera toda la memoria auxiliar necesaria para controlar la
 * dinámica del modelo. Controla que los punteros sean NULL.
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
 * Calcula en MDLInstance::_vertices la transformación de la malla
 * seleccionada actual, indicada por MDLInstance::_currentBodyPart y 
 * MDLInstance::_currentSubmodelMesh.<br>
 * Es llamado por selectMesh, y utiliza las matrices de transformación
 * de los huesos calculados en update.
 */
void MDLInstance::updateVertices() {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;
	const MDLPoint* vertices;
	const unsigned int* bones;
	const int* repeated;

	// Metemos en selSubmdl el número del recubrimiento
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
			// Hay que realizar la transformación.
			// Multiplicamos cada vértice por la matriz de transformación
			// del hueso al que está asociado el vértice, y guardamos el
			// resultado en la matriz de vértices transformados.
			unsigned int boneCurrentVertex = *bones;
			// Estamos actualizando la posición 0 del vértice (coordenada X)
			*vertexCoord = vertices->dotProduct(
								_boneMatrices[boneCurrentVertex][0]) +
								_boneMatrices[boneCurrentVertex][0][3];
			vertexCoord++;	// Pasamos a la siguiente coordenada "de salida".
			// Estamos actualizando la posición 1 del vértice (coordenada X)
			*vertexCoord = vertices->dotProduct(
								_boneMatrices[boneCurrentVertex][1]) +
								_boneMatrices[boneCurrentVertex][1][3];
			vertexCoord++;	// Pasamos a la siguiente coordenada "de salida".
			// Estamos actualizando la posición 2 del vértice (coordenada X)
			*vertexCoord = vertices->dotProduct(
								_boneMatrices[boneCurrentVertex][2]) +
								_boneMatrices[boneCurrentVertex][2][3];
			*vertexCoord++;
		}
		else {
			// Ya hemos manejado este vértice antes. Nos aprovechamos de
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
		// Preparamos los punteros a la información del submodelo para
		// apuntar al siguiente vértice.
		vertices++;
		bones++;
	}

} // updateVertices

//---------------------------------------------------------------------------

/**
 * Calcula en MDLInstance::_normals la transformación de las normales
 * de la malla seleccionada actualmente, indicada por
 * MDLInstance::_currentBodyPart y MDLInstance::_currentSubmodelMesh.<br>
 * Es llamado por selectMesh, y utiliza las matrices de transformación
 * de los huesos calculados en update.
 */
void MDLInstance::updateNormals() {

	unsigned int selSubmdl;
	const MDLSubmodel *submodel;
	const MDLPoint* normals;
	const unsigned int* bones;
	const int* repeated;

	// Metemos en selSubmdl el número del recubrimiento
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
			// Multiplicamos cada normal por la matriz de transformación
			// del hueso al que está asociado la normal, y guardamos el
			// resultado en la matriz de normales transformados.
			// En realidad no hay que usar la matriz de transformación
			// completa, pues estamos transformando vectores libres.
			// Simplemente rotamos, no usamos la traslación.
			unsigned int boneCurrentNormal = *bones;
			// Estamos actualizando la posición 0 de la normal (coordenada X)
			*normalCoord = normals->dotProduct(
								_boneMatrices[boneCurrentNormal][0]);
			normalCoord++;	// Pasamos a la siguiente coordenada "de salida".
			// Estamos actualizando la posición 1 de la normal (coordenada X)
			*normalCoord = normals->dotProduct(
								_boneMatrices[boneCurrentNormal][1]);
			normalCoord++;	// Pasamos a la siguiente coordenada "de salida".
			// Estamos actualizando la posición 2 de la normal (coordenada X)
			*normalCoord = normals->dotProduct(
								_boneMatrices[boneCurrentNormal][2]);
			normalCoord++;
		}
		else {
			// Ya hemos manejado este vértice antes. Nos aprovechamos de
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
		// Preparamos los punteros a la información del submodelo para
		// apuntar a la siguiente normal.
		normals++;
		bones++;
	}

} // updateNormals

//---------------------------------------------------------------------------

/**
 * Calcula las matrices de transformación de los vértices asociados
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

	// TODO: si en algún momento se quiere permitir mostrar la posición por
	// defecto (habría que ampliar el API, o algo), sería sencillo ampliar
	// todo esto a partir del código comentado.

	// if (hay animación seleccionada)
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
	// Lo primero que hay que hacer es obtener el cuaternión almacenando
	// la rotación de cada hueso respecto al padre, y su posición
	// (desplazamiento), mezclando todas las animaciones de la secuencia.
	MDLBasicType acum;
	acum = 0;
	for (unsigned int indexAnim = 0; indexAnim < getNumAnimations(); 
														indexAnim++) {
		float fraction;
		acum += _animationWeights[indexAnim];
		if (_animationWeights[indexAnim] == 0.0) {
			// No hay nada que hacer con la animación actual, porque tiene
			// peso nulo. Pasamos a la siguiente vuelta del bucle, saltandonos
			// toda la información de esta animación.
			// Esta comprobación nos evita de paso la división por cero si
			// acum aún es 0. Es decir, si la secuencia tiene 4 animaciones, y
			// las dos primeras tienen peso 0, acum valdrá 0 al principio, y
			// no deberíamos dividir por acum. No es necesario hacer esa
			// comprobación explícita, porque para que acum valga cero
			// también debe valer 0 _animationWeights, de ahí que baste con
			// comprobar eso. Bueno... más o menos, pero tú me entiendes :-D
			anims += _model->getNumBones();
			continue;
		}
		fraction = _animationWeights[indexAnim] / acum;
//std::cout << indexAnim << ": " << fraction << "(" << _animationWeights[indexAnim] << ");";
		for (boneIndex = 0; boneIndex < _model->getNumBones(); 
											boneIndex++, bone++) {
			// bone apunta al puntero al hueso boneIndex del modelo
			// Almacenamos en _boneQuaternions[boneIndex] el cuaternión del
			// hueso boneIndex (*bone) respecto a su hueso padre, y en
			// _bonePositions[boneIndex] la posición.

			(*anims)->CalcBonePosition(intFrame, fractionFrame,
									_bonePositions[boneIndex],
									0, 0, 0, fraction);
			(*anims)->CalcBoneQuaternion(intFrame, fractionFrame,
									_boneQuaternions[boneIndex],
									0, 0, 0, fraction);
			anims++;
		} // for de cálculo de cuaterniones y desplazamientos de los huesos.
	} // for que recorría todas las animaciones de la secuencia actual.
//std::cout << std::endl;
	// En _boneQuaternions tenemos los cuaterniones de los huesos y en
	// _bonePositions los desplazamientos respecto al padre. Tenemos que
	// calcular las matrices de cada uno. Recorremos todos los huesos
	// calculando la matriz respecto al padre, y luego la multiplicamos
	// por la matriz del padre. Se supone que los huesos están en
	// preorden en el modelo.
	bone = _model->getBones();
	for (boneIndex = 0; boneIndex < _model->getNumBones(); boneIndex++, bone++) {
		int parentIndex;
		// Establecemos la matriz de rotación a partir del cuaternión.
		_boneQuaternions[boneIndex].toMatrix(_boneMatrices[boneIndex]);

		// Establecemos la parte del desplazamiento.
		_boneMatrices[boneIndex][0][3] = _bonePositions[boneIndex][0];
		_boneMatrices[boneIndex][1][3] = _bonePositions[boneIndex][1];
		_boneMatrices[boneIndex][2][3] = _bonePositions[boneIndex][2];

		// En _boneMatrices[boneIndex] tenemos la matriz de transformación
		// del hueso respecto al hueso padre. Queremos la matriz absoluta,
		// a si es que la multiplicamos por la matriz del padre. Para que
		// esto funcione, la matriz del padre ya debe haberse calculado,
		// es decir, los huesos deben estar en preorden.
		parentIndex = (*bone)->getParentIndex();
		if (parentIndex != -1) {	// No somos el hueso padre
			assert (parentIndex < (int) boneIndex ||
						!"Los huesos del modelo no están en preorden.");
			_boneMatrices[boneIndex] = _boneMatrices[parentIndex] * 
												_boneMatrices[boneIndex];
		}

	} // for de cálculo de las matrices de los huesos.

} // updateMatrices

//---------------------------------------------------------------------------

/**
 * Comprueba si con el último avance del fotograma actual se ha
 * atravesado algún evento de la secuencia que haya que generar.
 * Es un método auxiliar usado en update(). Si hay algún evento
 * nuevo, se avisa a los oyentes.
 */
void MDLInstance::checkEvents() {

	while (_frameNextEvent < _frame) {
		// Hay que lanzar el siguiente evento.

/*
		Si se quiere que el modelo realice "transiciones" entre dos 
		secuencias (por ejemplo de idle a walk), se puede crear una
		secuencia mezcla de ambas animaciones, o mezclar las dos
		secuencias o algo así. Según Daniel Sanchez Crespo, esa
		alternativa da buen resultado y no queda mal porque la mezcla
		de los huesos es razonable.
		Otra alternativa es pedir a los diseñadores que hagan las
		transiciones, teniendo un fotograma "clave" que se utiliza
		como intermedio entre animaciones.
		Así, tendríamos una animación de inicio de andar, que, nada
		más acabar, debe automáticamente forzar el comienzo de
		la animación de andar. Del mismo modo, cuando el usuario
		deja de andar tenemos que enlazar el último fotograma de
		esa secuencia con la secuencia de dejar de andar.
		Parece que el formato MDL tiene información para guardar
		ese tipo de cosas (qué secuencia debe seguir a una dada),
		pero no hemos encontrado el modo de establecer eso en el
		fichero .qc de compilación de MDLs.
		La alternativa para implementar esto es utilizar eventos
		en las animaciones, y que el gestor de eventos modifique
		la secuencia convenientemente.
		El problema es que para que el enlace quede fluido, ese
		cambio debe tener en cuenta el fotograma actual _real_
		(no el fotograma que ha generado el salto del evento),
		además debería tener en cuenta la parte flotante de dicho
		frame para que la interpolación no haga crujir el enlace...

		La alternativa que nos planteamos fue que ese trabajo lo
		hiciera el MDLInstance directamente. Nos inventamos un
		evento especial predefinido (cuyo identificador establece
		la constante de precompilación CHANGE_SEQUENCE_EVENT) y
		si se encuentra ese evento, desde aquí saltamos a la secuencia
		establecida en el parámetro del evento.

		Realmente todo esto fueron solo pruebas rápidas para ver si
		el enlace era posible. Implementar eso correctamente requeriría
		un poco más de esfuerzo. Por ejemplo, sería razonable que
		antes de realizar el cambio se preguntara si se desea realizar.
		Así la animación de andar tendría el enlace con la animación
		de parar de andar, y se preguntaría contínuamente si se quiere
		pasar a ella o no. Incluso podría permitirse "sobreescribir" a
		qué secuencia pasar, para poder empezar a correr o algo así.

		Este cambio también implica a la clase que controle a esta por
		encima, que es la que debe encargarse de desplazar el modelo
		en el entorno en aquellas animaciones que lo tengan. Si desde
		aquí cambiamos la secuencia de modo "silencioso", podríamos
		probocar un cambio en la velocidad de desplazamiento de la que
		"arriba" no necesariamente serían conscientes. La creación del
		evento anterior permitiría notificar el cambio.

		Si se quieren utilizar los identificadores de animaciones
		predefinidos (ACT_WALK, y cosas así) en los MDL con transiciones,
		hay que desarrollar un "mapeo" entre identificadores no utilizados
		y animaciones con transiciones.
		En realidad existe un "problema" adicional. Es posible que lo
		lógico sería asociar el identificador ACT_WALK a la secuencia de
		inicio de andar, y asociar algún otro identificador dummy al 
		verdadero ciclo. Esto tiene el problema de que en algún momento
		se pasará a una secuencia diferente a ACT_WALK. Si desde arriba
		están estableciendo continuamente la secuencia de andar (porque
		se sigue detectando la pulsación de tecla de andar del usuario
		o cualquier cosa por el estilo), confiando que desde aquí si
		nos piden establecer una secuencia que es la activa no vamos
		a hacer nada, el asunto dejará de funcionar, porque realmente
		cuando la transición termine, cambiaremos de animación, nos
		volverán a pedir que pongamos la de andar (que resulta ser sólo
		el inicio del andar), y el resultado será que el personaje
		andará con un "ciclo de la secuencia de inicio de andar".

		Por último, sin la existencia del "aviso hacia arriba" de la llegada
		del evento de cambio de secuencia, para volver a idle cuando se
		está andando se necesitarían dos animaciones: el ciclo de andar
		sin evento de cambio, y el ciclo de andar con él, que pasa a la
		secuencia de fin de andar que enlaza a su vez con idle. Cuando
		el usuario decide dejar de andar, habría que establecer la
		animación de ciclo de andar con evento, pero "silenciosamente",
		es decir, sin que cambie el fotograma actual (en realidad las
		dos secuencias son la misma). Para eso, se necesitaría otro
		método de setSameSequence o algo por el estilo.


		Total. Aquí lo que hay es el código que se metió para probar los
		enlaces entre secuencias con el evento predefinido. Si se
		decide implementar esto, habría que revisarlo, quizá meter
		el aviso del cambio de secuencia comentado antes, y vigilar
		la actualización de los eventos, para que los eventos de la
		nueva secuencia se llamen correctamente (ahora mismo está sin
		probar nada).
*/

/*
		// Lo analizamos (el evento).
		
		if (getSequence()->getEvent(_nextEvent)->getEventCode()
										== CHANGE_SEQUENCE_EVENT) {
			// El evento que se ha producido es el evento especial de
			// cambio de secuencia que realizamos "de estrangis"

			// std::cout << "Cambio de animación automática a "
						 << getSequence()->getEvent(_nextEvent)->getOptions()
						 << " ";
			float restar = getSequence()->getEvent(_nextEvent)->getNumFrame();
			char aux[201];
			// sprintf(aux, "_frameNextEvent = %d, _frame = %f\n", 
			//			(int)_frameNextEvent, _frame);
			// std::cout << aux;
			// sprintf(aux, "\nAnimacion %s, fotograma %f, pasamos a "
						"animación %s, fotograma %f por evento en "
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
//						Implementación de ModelState
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
 * tantas partes del modelo como se indique en el parámetro.
 * @param numBodyParts Número de partes del cuerpo de las que se
 * tendrá que almacenar el recubrimiento seleccionado.
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
 * Operador de asignación. Se utiliza copia profunda, con los punteros.
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

