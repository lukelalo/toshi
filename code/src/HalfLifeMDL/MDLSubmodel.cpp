//---------------------------------------------------------------------------
// MDLSubmodel.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLSubmodel.cpp
 * Contiene la definición (implementación) de la clase que almacena la
 * información sobre un evento de una secuencia del modelo de Half Life.
 * @see HalfLifeMDL::MDLEvent
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#include "MDLBodyPart.h"

namespace HalfLifeMDL {

/**
 * Constructor de los objetos de la clase.
 * @param infoModel Estructura con la información del [sub]modelo
 * tal y como se ha leído del fichero. Este puntero debe ser
 * liberado por el usuario (se hace copia).
 * @param offset Desplazamiento de todos los punteros contenidos
 * en infoModel. Los punteros que contiene están referidos al
 * origen del fichero, y hay que sumarle el valor de este parámetro
 * para obtener la posición en memoria
 * @param connectedTriangles Si se desea que las mallas del modelo
 * mantengan los triángulos conectados, ya sea en modo FAN o en modo
 * STRIP, se debe pasar cierto en este parámetro. Si el motor no es
 * capaz de dibujar triángulos compactos, se pasa falso, de modo que
 * los triángulos se almacenan de forma separada. Vea
 * HalfLifeMDL::MDLMesh para más información.
 * @param clockwise Especifica el orden de los vértices que crean la
 * cara delantera en las mallas del modelo que se creará. Si se desea
 * que los triángulos devueltos tengan su cara frontal con los vértices
 * en el sentido de las agujas del reloj hay que especificar cierto en
 * este campo, y falso en caso contrario. <br>
 * Si se solicitan las caras con los vértices en el sentido contrario a
 * las agujas del reloj, y manteniendo los triángulos conectados, las
 * mallas en modo STRIP tendrán un primer triángulo "corrupto", en el
 * que el primer vértice y el tercero serán el mismo.
 * @param skins Información sobre las pieles del modelo. Debe haber sido
 * ya creada. El puntero se utiliza durante la fase de construcción, pero
 * no se almacena posteriormente, y tampoco se hace copia.
 */
MDLSubmodel::MDLSubmodel(const model_t* infoModel, unsigned int offset,
							bool connectedTriangles, bool clockwise,
							const MDLSkins* skins) :
		_name(infoModel->name), _numVertices(infoModel->numverts),
		_vertices(NULL), _bones(NULL),
		_normals(NULL), _texCoord(NULL), _repeatedVertices(NULL),
		_repeatedNormals(NULL) {

	// Para construir las matrices de adyacencia tal y como las queremos
	// hay que recolocar el array de vértices y normales que nos da el MDL
	// para este submodelo dentro de infoModel. Además hay que crear un
	// array para cada coordenada de textura.
	// Lo que hacemos es recorrer todas las mallas (inicialmente sin
	// preocuparnos de si están en strip o fan, pues queremos la información
	// de los vértices que usan, no cómo están conectados), y meteremos
	// todos la información de cada vértice en un array. Nos las arreglaremos
	// para quitar los repetidos de ese array, y luego crearemos el array
	// de vértices, el array de normales y el de coordenadas de textura
	// a partir de él. El siguiente paso es construir las mallas para que
	// utilicen esos arrays unificados, de modo que sólo tengan una matriz
	// de adyacencia cada una, no una para vértices, otra para normales
	// y las coordenadas de textura a su aire.

	// Concretando más, para que esto no sea demasiado ineficiente, lo
	// que se hará será crear un array dinámico de FullVertexInfo. Cada
	// posición guarda la información completa de un vértice del submodelo,
	// esté en la malla que esté. El tamaño de ese array será el número de
	// vértices referenciados en las mallas del submodelo.

	// Inicialmente, se meten todos los vértices de todas las mallas, sin
	// preocuparse por los repetidos. El siguiente paso es realizar una
	// ordenación de todos esos vértices. En realidad la información de los
	// vértices no tiene un orden claro; da igual cual se utilice, con tal de
	// que sea un orden completo, y que los vértices con igual información
	// aparezcan seguidos en el array.
	// Después se eliminan los repetidos. Es posible hacerlo más o menos
	// rápidamente con una sola pasada sobre el vector, pues ya está ordenado.

	// Una vez que se tienen los vértices útiles creados, pueden construirse
	// los arrays de vértices, normales y coordenadas de textura del
	// submodelo, manteniendo la información separada en lugar de usando la
	// estructura FullVertexInfo.

	// El último paso es crear las mallas. La información de cada una debe
	// ser de nuevo analizada, viendo a qué vértice, normal y coordenadas
	// de textura referencian. Con todos esos datos, se construye un
	// FullVertexInfo que se busca dentro del array auxiliar que se creó
	// antes. Esa búsqueda será binaria, aprovechando el orden artificial
	// creado. La posición en el array donde se encuentre el vértice con
	// la información coincidente será el referenciado por la matriz de
	// adyacencia de la malla.

	// El código de este método es bastante largo. No estoy a favor de los
	// métodos largos, por lo que, incialmente, la idea era tener otros
	// dos o tres métodos auxiliares, a parte de los que ya utiliza. Sin
	// embargo, en el momento de implementar esos métodos, la cantidad de
	// información que necesitaban era bastante alta, llegando incluso a
	// necesitar nueve parámetros de variada naturaleza. Para no romper
	// con la norma, era necesario documentar cada uno de ellos, cuyo
	// cometido era muy específico, al igual que cada uno de los métodos
	// auxiliares. Finalmente he decidido no usar esos métodos auxiliares
	// a costa de alargar el código de éste. Los comentarios sobre lo que
	// se va haciendo en cada momento deberían facilitar la comprensión.

	// NOTA: durante algún tiempo, el código de este método ocasionaba un
	// extraño problema: los modelos se mostraban correctamente si el programa
	// estaba compilado en modo depuración, pero no si estaba compilado en
	// modo liberación.
	// Averiguar el problema fue un poco conflictivo, no pudiendo depurar
	// en modo release, y habiendo tantos lugares donde podían producirse
	// problemas. Finalmente se descubrió el problema. Por alguna razón,
	// al buscar dentro del array de FullVertexInfo (una vez eliminado los
	// repetidos) los vértices de cada malla, en modo depuración siempre se
	// encontraban los vértices (de hecho hay un assert que detiene la
	// ejecución si eso no ocurría). Sin embargo, en modo release, algunos
	// vértices no se encontraban. Como el assert sólo es en modo depuración,
	// no saltaba ;-). Resultó que el problema siempre estaba en las
	// coordenadas de textura. En realidad el vértice buscado estaba... salvo
	// por un pequeño error de redondeo. De hecho, increíblemente, si se
	// escribían todos los vértices del array de FullVertexInfo una vez
	// limpio, había diferencias entre el resultado en modo depuración y
	// en modo release. Las diferencias siempre eran en coordenadas de
	// textura. Por ejemplo, en modo debug se mostraba 0.526859 y en
	// modo release 0.526860, o 0.384298 y 0.384297 por poner otro ejemplo.
	// Incomprensible, francamente.
	// La realidad es que las coordenadas de textura se calculan, no se
	// leen directamente en flotante del fichero. Se calculan a partir del
	// short que almacena el fichero, normalizandolo dividiendolo por el
	// ancho/alto de la textura en píxeles. No entiendo por qué en modo 
	// depuración y en modo release el resultado de esa división resultaba
	// ser diferente :-m En cualquier caso, lo que ocasionaba el error era
	// que al comparar las coordenadas del array limpio, con las coordenadas
	// de cada uno de los vértices (que se vuelven a calcular haciendo la
	// división anterior), resultaban ser diferentes. Supongo que el problema
	// de que fueran diferentes ambas divisiones es por algún problema de
	// conversión de tipos en la división... aunque no lo tengo del todo claro
	// En cierto modo ¡¡estoy diciendo que x / y != x / y, con x short e y
	// flotante!!. Para no complicarme la vida y tener dolores de cabeza con
	// la coma flotante y el IEEE ;-) lo que he hecho ha sido comparar
	// directamente los short en lugar de los flotantes con la división. Eso
	// ha supuesto ampliar FullVertexInfo para que guarde ese short, pero
	// también la versión calculada de las coordenadas de textura para poder
	// asignarlas cuando se construyan los vértices finales cómodamente.
	// el código está lleno de trozos comentados con la versión antigua
	// cuando se utilizaban las coordenadas de textura reales en lugar de las
	// versiones sin normalizar.

	FullVertexInfo* verticesInfo;
	FullVertexInfo* writingVertex;
	unsigned int maxNumVertices;
	unsigned int contMesh;
	mesh_t* meshes;
	vec3_t *verts;
	MDLuchar *vBones;
	vec3_t *norms;
	MDLuchar *nBones;
	MDLMesh* currentObjMesh;

	if (infoModel->nummesh == 0) {
		// El submodelo está vacío. Seguramente se utilice para "quitar"
		// la parte del cuerpo que supuestamente recubre, como por
		// ejemplo si la parte del cuerpo es una mochila, que el modelo
		// aparezca sin ella. En ese caso no tenemos que hacer nada más.
		// Esto ocurre en el modelo del juego original terror.mdl
		_numMeshes = 0;
		return;
	}

	// Metemos en meshes el puntero a la primera malla del submodelo
	// especificada en el fichero.
	meshes = (mesh_t*)((MDLchar*)infoModel->meshindex + offset);

	// Obtenemos el número de vértices que se referencian en todas las mallas.
	maxNumVertices = getNumVertices(meshes, infoModel->nummesh, offset);

	// Creamos el array auxiliar.
	verticesInfo = new FullVertexInfo[maxNumVertices];

	// Hacemos que verts apunte a los vértices del submodelo especificados
	// en el fichero, que bones apunte a los huesos a los que se refieren,
	// y lo mismo para las normales.
	verts = (vec3_t*)((MDLchar*)infoModel->vertindex + offset);
	vBones = (MDLuchar*)((MDLchar*)infoModel->vertinfoindex + offset);
	norms = (vec3_t*)((MDLchar*)infoModel->normindex + offset);
	nBones = (MDLuchar*)((MDLchar*)infoModel->norminfoindex + offset);

	writingVertex = verticesInfo;
	// Empezamos a recorrer todas las mallas del modelo.
	// meshes ya apunta a la primera malla. En cada vuelta del bucle
	// avanzaremos meshes para que pase a la siguiente. Vamos metiendo
	// en writingVertex la información completa de cada vértice.
	for (contMesh = 0; contMesh < infoModel->nummesh; contMesh++, meshes++) {
		MDLshort* tris;
		int numVert;
		MDLBasicType uFactor, vFactor;

		// Asumimos que las texturas asociadas a la posición <meshes->skinref>
		// de todas las pieles tienen el mismo tamaño. Deben tenerlo porque
		// las coordenadas de textura se reciben en un entero corto, y para
		// obtener las coordenadas reales (entre 0 y 1) hay que dividirlo
		// por el tamaño de la textura. Si para pieles diferentes el tamaño
		// de la textura es diferente, las coordenadas de textura resultantes
		// cambiarían, y sería un caos... Vamos, que en algún caso podría
		// estar hecho aposta, ahora que lo pienso, pero me parece bastante
		// retorcido.
		if (skins->getNumSkins() != 0) {
			uFactor = 1.0 / 
						skins->getTexture(0, meshes->skinref)->getWidth();
			vFactor = 1.0 /
						skins->getTexture(0, meshes->skinref)->getHeight();
		}
		else if (AllowModelsWithoutTextures == 1) {
			uFactor = 1.0;
			vFactor = 1.0;
		} else {
			// Lo único que podemos hacer es esperar la excepción...
			// (esto es un poco ridículo, pero bueno).
			uFactor = 0.0;
			vFactor = 0.0;
		}

		// Hacemos que tris apunte al comienzo de la información de la
		// malla mesh.
		tris = (MDLshort*)((MDLchar*)meshes->triindex + offset);
		// Recorremos todos los vértices.
		numVert = *tris;
		while (numVert != 0) {
			if (numVert < 0)
				numVert = -numVert;
			tris++;
			for (unsigned int c = 0; c < numVert; c++) {
				// El valor apuntado por tris es el índice del vértice.
				writingVertex->bone = vBones[*tris];
				writingVertex->vertex.set(verts[*tris][0],verts[*tris][1],
											verts[*tris][2]);
				tris++;
				// El valor apuntado por tris es el índice de la normal.
				assert(writingVertex->bone == nBones[*tris]);
				writingVertex->normal.set(norms[*tris][0], norms[*tris][1],
											norms[*tris][2]);
				tris++;
				// El valor apuntado por tris es la coordenada u.
				writingVertex->uCoord = uFactor * *tris;
				writingVertex->uCoordS = *tris;
				tris++;
				// El valor apuntado por tris es la coordenada v.
				writingVertex->vCoord = vFactor * *tris;
				writingVertex->vCoordS = *tris;
				tris++;
				writingVertex++;
			}
			numVert = *tris;
		} // while que recorría todos los vértices.
	
	} // for que recorría todas las mallas

	// Ya tenemos en verticesInfo todos los vértices de las mallas. Ahora los
	// ordenamos y eliminamos los repetidos.
	maxNumVertices = sortAndClean(verticesInfo, maxNumVertices);
	assert (maxNumVertices > 0);


#ifndef NDEBUG
	_incrTransformations = maxNumVertices * 2 - 
							(infoModel->numverts  + infoModel->numnorms);
#endif

	// Ya tenemos reducida la matriz de vértices. Ahora hay que construir las
	// matrices que mantendrá el submodelo, y luego las matrices de adyacencia.

	_numVertices = maxNumVertices;
	_vertices = new MDLPoint[_numVertices];
	_normals = new MDLPoint[_numVertices];
	_bones = new unsigned int[_numVertices];
	_texCoord = new MDLBasicType[_numVertices*2];
	_repeatedVertices = new int[_numVertices];
	_repeatedNormals = new int[_numVertices];

	// Preparamos la entrada en el bucle. Éste necesita que siempre
	// exista un elemento anterior, por eso el 0 lo metemos
	// manualmente.
	_bones[0] = verticesInfo[0].bone;
	_vertices[0] = verticesInfo[0].vertex;
	_normals[0] = verticesInfo[0].normal;
	_texCoord[0] = verticesInfo[0].uCoord;
	_texCoord[1] = verticesInfo[0].vCoord;
	_repeatedVertices[0] = _repeatedNormals[0] = 0;
	for (unsigned int cont = 1; cont < _numVertices; cont++) {
		_bones[cont] = verticesInfo[cont].bone;
		_vertices[cont] = verticesInfo[cont].vertex;
		_normals[cont] = verticesInfo[cont].normal;
		_texCoord[2*cont] = verticesInfo[cont].uCoord;
		_texCoord[2*cont + 1] = verticesInfo[cont].vCoord;
		if ((verticesInfo[cont].bone == verticesInfo[cont-1].bone) &&
			(verticesInfo[cont].vertex == verticesInfo[cont-1].vertex))
			_repeatedVertices[cont] = 1;
		else
			_repeatedVertices[cont] = 0;
		// No miramos más hacia atrás de uno en las normales. En los
		// vértices no hace falta porque estamos ordenados por vértice
		// :-) De todas formas es de esperar que las normales tiendan
		// también sólo a coincidir de forma contínua.
		if ((verticesInfo[cont].bone == verticesInfo[cont-1].bone) &&
			(verticesInfo[cont].normal == verticesInfo[cont-1].normal))
			_repeatedNormals[cont] = 1;
		else
			_repeatedNormals[cont] = 0;
	}

	// Ahora viene lo divertido... nos toca construir las mallas.
	// Recorremos todas las mallas mesh_t para contar cuantas mallas
	// MDLMesh vamos a tener que construir, según tengamos o no que
	// mantener la conectividad de triángulos.
	_numMeshes = 0;
	meshes = (mesh_t*)((MDLchar*)infoModel->meshindex + offset);
	for (contMesh = 0; contMesh < infoModel->nummesh; ++contMesh, ++meshes)
		_numMeshes += numberMeshesNeeded(
							(MDLshort*)((MDLchar*)meshes->triindex + offset),
							connectedTriangles);

	assert(_numMeshes > 0 && "¡¡Número de mallas de un submodelo es 0!!");

	// Pedimos memoria para todas las mallas.
	_meshes = new MDLMesh[_numMeshes];

	// Y ahora las inicializamos.
	meshes = (mesh_t*)((MDLchar*)infoModel->meshindex + offset);
	currentObjMesh = _meshes;

	for (contMesh = 0; contMesh < infoModel->nummesh; ++contMesh, ++meshes) {
		// meshes apunta a una malla que tenemos que construir. Una mesh_t
		// puede originar uno o varios MDLSubmodel, dependiendo de si se
		// va a mantener la información sobre la conectividad o no.
		int numVert;
		unsigned int cont;
		MDLshort* mesh;
		unsigned int texture;

		texture = meshes->skinref;

		// Asumimos que las texturas asociadas a la posición meshes->skinref
		// de todas las pieles tienen el mismo tamaño. Deben tenerlo porque
		// las coordenadas de textura se reciben en un entero corto, y para
		// obtener las coordenadas reales (entre 0 y 1) hay que dividirlo
		// por el tamaño de la textura. Si para pieles diferentes el tamaño
		// de la textura es diferente, las coordenadas de textura resultantes
		// cambiarían, y sería un caos... Vamos, que en algún caso podría
		// estar hecho aposta, ahora que lo pienso, pero me parece bastante
		// retorcido.

		MDLBasicType uFactor, vFactor;
		if (skins->getNumSkins() != 0) {
			uFactor = 1.0 / skins->getTexture(0, texture)->getWidth();
			vFactor = 1.0 / skins->getTexture(0, texture)->getHeight();
		}
		else if (AllowModelsWithoutTextures == 1) {
			uFactor = 1.0;
			vFactor = 1.0;
		} else {
			// Lo único que podemos hacer es esperar la excepción...
			uFactor = 0.0;
			vFactor = 0.0;
		}

		// Hacemos que mesh apunte a la información de los triángulos de
		// la malla apuntada por meshes
		mesh = (MDLshort*)((MDLchar*)meshes->triindex + offset);
		// Empezamos a crear mallas.
		if (connectedTriangles) {
			// Debemos mantener la organización por bloques, con mallas
			// de tipo MT_Fan y MT_Strip.
			numVert = *mesh;
			// Recorremos toda la información 
			while (numVert != 0) {
				// Hacemos que mesh apunte a la información del primer vértice
				mesh++;
				// Creamos el array para los vértices en el objeto malla que
				// nos toca rellenar.
				if (numVert < 0) {
					currentObjMesh->_type = MDLMesh::MT_Fan;
					numVert = -numVert;
				}
				else
					currentObjMesh->_type = MDLMesh::MT_Strip;

				currentObjMesh->_numVertices = numVert;
				if (!clockwise && 
					(currentObjMesh->_type == MDLMesh::MT_Strip)) {
					// Necesitamos uno mas para meter un triángulo corrupto
					// que nos de la vuelta al sentido de los triángulos.
					currentObjMesh->_vertices = new unsigned int[numVert + 1];
					currentObjMesh->_numVertices++;
				}
				else
					currentObjMesh->_vertices = new unsigned int[numVert];
				currentObjMesh->_skinTexture = texture;
				// Rellenamos la matriz de adyacencia usando los índices de
				// verInfo.
				if (clockwise)
					// Tenemos que dar las mallas tal y como nos las da a
					// nosotros el fichero. No hay que preocuparse más.
					for (cont = 0; cont < numVert; cont++) {
						unsigned int vertice;
						vertice = search(verticesInfo, maxNumVertices,
											vBones[*mesh],
											verts[*mesh], norms[*(mesh+1)],
											*(mesh + 2),
											*(mesh + 3));
/*											uFactor * *(mesh + 2),
											vFactor * *(mesh + 3));*/
						currentObjMesh->_vertices[cont] = vertice;
						mesh += 4;
					} // for que recorría todos los vértices de la nueva malla
				else {
					// Tenemos que dar los triángulos en orden inverso,
					// con la cara frontal teniendo los vértices en sentido
					// antihorario. Nos toca pensar un poco más.
					if (currentObjMesh->_type == MDLMesh::MT_Fan) {
						// El primer vértice es el mismo. Los demás van
						// justo al revés para poder mantener la relación.
						// Lo hacemos dando "dos pasadas".
						for (cont = 0; cont < numVert; cont++) {
							unsigned int vertice;
							vertice = search(verticesInfo, maxNumVertices,
												vBones[*mesh],
												verts[*mesh], norms[*(mesh+1)],
												*(mesh + 2),
												*(mesh + 3));
/*												uFactor * *(mesh + 2),
												vFactor * *(mesh + 3));*/
							currentObjMesh->_vertices[cont] = vertice;
							mesh += 4;
						}
						unsigned int le, r, aux;
						le = 1; r = numVert - 1;
						while (le < r) {
							aux = currentObjMesh->_vertices[le];
							currentObjMesh->_vertices[le] = 
											currentObjMesh->_vertices[r];
							currentObjMesh->_vertices[r] = aux;
							le++;
							r--;
						}
					} // if (currentObjMesh->_type == MDLMesh::MT_Fan)
					else {
						// Hay que meter una entrada "dummy" para crear
						// un triángulo corrupto con los vértices 1-0-1
						// Eso supone meter el vértice 1, y luego todos los
						// demás como normalmente.
						unsigned int vertice;
						vertice = search(verticesInfo, maxNumVertices,
											vBones[*(mesh+4)],
											verts[*(mesh+4)], norms[*(mesh+5)],
											*(mesh + 6),
											*(mesh + 7));
/*											uFactor * *(mesh + 6),
											vFactor * *(mesh + 7));*/
						currentObjMesh->_vertices[0] = vertice;
						for (cont = 1; cont <= numVert; cont++) {
							unsigned int vertice;
							vertice = search(verticesInfo, maxNumVertices,
												vBones[*mesh],
												verts[*mesh], norms[*(mesh+1)],
												*(mesh + 2),
												*(mesh + 3));
/*												uFactor * *(mesh + 2),
												vFactor * *(mesh + 3));*/
							currentObjMesh->_vertices[cont] = vertice;
							mesh += 4;
						} // for que recorría todos los vértices de la nueva malla
					} // if-else (currentObjMesh->_type == MDLMesh::MT_Fan)
				} // if-else (clockwise)
				numVert = *mesh;
				currentObjMesh++;
			} // while (numVert != 0)
		} // if (connectedTriangles)
		else {
			// Tenemos que "desenrollar" todos los triángulos. Sólo
			// utilizaremos un objeto malla para cada mesh_t.
			currentObjMesh->_type = MDLMesh::MT_Triangles;
			// Lo primero es saber cuantos vértices tiene la malla para
			// poder pedir la memoria suficiente.
			const MDLshort* auxMesh = mesh;
			unsigned int numTriangles = 0;
			unsigned int commonVertex1, commonVertex2;
			unsigned int* currentNewVertex;
			/*	Modo "manual" de contar el número de triángulos.
				No hace falta hacerlo así, porque mesh_t dispone del
				campo numtris.
			while (*auxMesh != 0) {
				if (*auxMesh < 0) {
					numTriangles += -*auxMesh - 2;
					auxMesh += (-*auxMesh) * 4;
				}
				else {
					numTriangles += *auxMesh - 2;
					auxMesh += (*auxMesh) * 4;
				}
				auxMesh++;
			} // while que contaba el número de triángulos.
			*/
			numTriangles = meshes->numtris;

			currentObjMesh->_numVertices = 3 * numTriangles;
			currentObjMesh->_vertices = new unsigned int [3 * numTriangles];
			currentObjMesh->_skinTexture = texture;

			// Recorremos toda la malla metiendo los vértices desenrollados.
			numVert = *mesh;
			currentNewVertex = currentObjMesh->_vertices;
			while (numVert != 0) {
				// Hacemos que mesh apunte a la información del primer
				// vértice.
				mesh++;
				if (numVert < 0) {
					// Los vértices que vamos a leer están en modo fan
					numVert = -numVert;
					commonVertex1 = search(verticesInfo, maxNumVertices,
										vBones[*mesh],
										verts[*mesh], norms[*(mesh+1)],
										*(mesh + 2),
										*(mesh + 3));
/*										uFactor * *(mesh + 2),
										vFactor * *(mesh + 3));*/
					mesh += 4; // Avanzamos al siguiente vértice.
					for (cont = 0; cont < numVert - 2; cont++) {
						// El triángulo cont está formado por los vértices
						// 0, n y n + 1 si clockwise es cierto, y por los
						// vértices 0, n+1, n si clockwise es falso. En
						// commonVertex1 tendremos siempre el vértice 0,
						// y mesh apuntará al vértice n.
						*currentNewVertex = commonVertex1;
						currentNewVertex++;
						if (clockwise) {
							*currentNewVertex = search(verticesInfo, 
												maxNumVertices,
												vBones[*mesh],
												verts[*mesh], norms[*(mesh+1)],
												*(mesh + 2),
												*(mesh + 3));
/*												uFactor * *(mesh + 2),
												vFactor * *(mesh + 3));*/
							currentNewVertex++;
						}
						else
							commonVertex2 =  search(verticesInfo, 
												maxNumVertices,
												vBones[*mesh],
												verts[*mesh], norms[*(mesh+1)],
												*(mesh + 2),
												*(mesh + 3));
/*												uFactor * *(mesh + 2),
												vFactor * *(mesh + 3));*/
						mesh += 4;
						*currentNewVertex = search(verticesInfo, 
											maxNumVertices,
											vBones[*mesh],
											verts[*mesh], norms[*(mesh+1)],
											*(mesh + 2),
											*(mesh + 3));
/*											uFactor * *(mesh + 2),
											vFactor * *(mesh + 3));*/
						currentNewVertex++;
						if (!clockwise) {
							*currentNewVertex = commonVertex2;
							currentNewVertex++;
						}
						// Al final, mesh ha avanzado sólo una vez, y
						// currentNewVertex tres (ha guardado 3 nuevos
						// vértices).
					} // for sobre los vértices de este bloque en modo fan
					// mesh apunta al vértice n + 1 (al último del último
					// triángulo). Tenemos que saltarnoslo.
					// Otra forma de verlo es que antes de entrar en el
					// for sólo incrementamos mesh una vez, y hay que
					// incrementarlo numVert vértices, y lo hemos hecho
					// numVert - 1 veces.
					mesh += 4;
				} // if (numVert < 0)
				else {
					// Los vértices que vamos a leer están en modo strip.
					bool evenTriangle = true;
					commonVertex1 = search(verticesInfo, maxNumVertices,
										vBones[*mesh],
										verts[*mesh], norms[*(mesh+1)],
										*(mesh + 2),
										*(mesh + 3));
/*										uFactor * *(mesh + 2),
										vFactor * *(mesh + 3));*/
					mesh += 4; // Avanzamos al siguiente vértice.
					commonVertex2 = search(verticesInfo, maxNumVertices,
										vBones[*mesh],
										verts[*mesh], norms[*(mesh+1)],
										*(mesh + 2),
										*(mesh + 3));
/*										uFactor * *(mesh + 2),
										vFactor * *(mesh + 3));*/
					mesh += 4; // Avanzamos al siguiente vértice.
					if (!clockwise) {
						// Hay que invertir los triángulos. Por el uso
						// que vamos a ir haciendo de commonVertex1 y
						// commonVertex2, es suficiente con intercambiarlos
						// y considerar que estamos en un triángulo
						// del tipo contrario.
						unsigned int aux;
						aux = commonVertex1;
						commonVertex1 = commonVertex2;
						commonVertex2 = aux;
						evenTriangle = !evenTriangle;
					}
					for (cont = 0; cont < numVert - 2; cont++) {
						// El triángulo cont está formado por los vértices
						// cont, cont + 1 y cont + 2 si cont es par, y
						// cont + 1, cont y cont + 2 si es impar en el
						// caso de countwise (sentido de las agujas del
						// reloj). En el sentido antihorario, sería el
						// triángulo cont + 1, cont y cont + 2 si es
						// par, y cont, cont + 1 y cont + 2 si es impar.
						// De ahí el cambio que se realiza antes de entrar
						// en este bucle si estamos en sentido antihorario.
						// En cualquier caso, nos las arreglaremos para
						// que el triángulo siempre sea
						// (commonVertex1, commonVertex2, *mesh).
						*currentNewVertex = commonVertex1;
						currentNewVertex++;
						*currentNewVertex = commonVertex2;
						currentNewVertex++;
						*currentNewVertex =  search(verticesInfo,
										maxNumVertices, vBones[*mesh],
										verts[*mesh], norms[*(mesh+1)],
										*(mesh + 2),
										*(mesh + 3));
/*										uFactor * *(mesh + 2),
										vFactor * *(mesh + 3));*/
						// Preparamos la siguiente vuelta.
						if (evenTriangle)
							commonVertex1 = *currentNewVertex;
						else
							commonVertex2 = *currentNewVertex;
						currentNewVertex++;
						mesh += 4;
						evenTriangle = !evenTriangle;
					} // for sobre los vértices de este bloque en modo strip

				} // if-else (numVert < 0)
				numVert = *mesh;
			} // while (numVert != 0)
			currentObjMesh++;
		} // if-else (connectedTriangles)

	} // for que recorría todos los mesh_t del modelo.

	// ¡¡Listo!! Liberamos el array auxiliar antes de terminar.
	delete[] verticesInfo;

} // constructor

//---------------------------------------------------------------------------

/**
 * Destructor. Libera la memoria utilizada.
 */
MDLSubmodel::~MDLSubmodel() {

	if (_vertices != NULL)			delete[] _vertices;
	if (_bones != NULL)				delete[] _bones;
	if (_normals != NULL)			delete[] _normals;
	if (_texCoord != NULL)			delete[] _texCoord;
	if (_repeatedVertices != NULL)	delete[] _repeatedVertices;
	if (_repeatedNormals != NULL)	delete[] _repeatedNormals;

} // Destructor

//---------------------------------------------------------------------------

/**
 * Recorre las mallas proporcionadas y da el número de vértices que
 * contienen, sin desenrollar ni preocuparse de repeticiones. Simplemente
 * las recorre y devuelve el número de entradas con información sobre
 * un vértice. <p>
 * Este método es auxiliar, y se usa para facilitar la unificación de
 * los arrays de vértices, normales y coordenadas de textura del
 * submodelo.
 * @param meshes Puntero al array de mallas del modelo.
 * @param numMeshes Número de mallas en el array anterior.
 * @param offset Desplazamiento de todos los punteros contenidos
 * en meshes. Los punteros que contiene están referidos al
 * origen del fichero, y hay que sumarle el valor de este parámetro
 * para obtener la posición en memoria.
 */
unsigned int MDLSubmodel::getNumVertices(const mesh_t* meshes, 
											unsigned int numMeshes,
											unsigned int offset) {
	unsigned int result, count;
	int i;
	MDLshort* mesh;
	result = 0;

	// Aunque mesh_t::numtris tuviera efectivamente el número de
	// triángulos, no podríamos usarlo porque queremos el número de
	// vértices usados en las listas, y como cada malla puede tener
	// varias secuencias de triángulos no podemos saber cuantos
	// vértices se usan para definir los numtris triángulos de la malla
	// (al usar el STRIP o FAN).

	for (count = 0; count < numMeshes; count++) {
		mesh = (MDLshort*)((MDLchar*) meshes[count].triindex + offset);
		i = *mesh;
		while (i != 0) {
			mesh++;
			if (i < 0) i = -i;
			result += i;
			mesh += i * 4;
			i = *mesh;
		}
	}

	return result;

} // getNumVertices

//---------------------------------------------------------------------------

/**
 * Recibe un array de FullVertexInfo desordenado, lo ordena y elimina
 * los repetidos. <p>
 * Es un método auxiliar, y se usa para facilitar la unificación de
 * los arrays de vértices, normales y coordenadas de textura del
 * submodelo. <p>
 * La ordenación resultante será primero por el hueso al que se refiere
 * el vértice y la normal, luego por el vértice, después por la normal,
 * y finalmente por las coordenadas de textura.
 * @param vertices Array con la información de vértices a ordenar y
 * limpiar.
 * @param size Número de vértices en el array.
 * @return Número de elementos útiles en el array final, una vez
 * quitados los repetidos.
 */
unsigned int MDLSubmodel::sortAndClean(FullVertexInfo* vertices,
									 unsigned int size) {

	// Para ordenar usamos la función qsort de la librería de
	// C.

	qsort(vertices, size, sizeof(FullVertexInfo), compareFullVertexInfo);

	// Ya está ordenado. Ahora hay que quitar los repetidos.
	FullVertexInfo *write, *read;
	unsigned int c, result;
	write = vertices;
	read = vertices;
	c = result = 0;
	while (c < size) {
		// Avanzamos el de lectura saltandonos todos los repetidos.
		while ((c < size) && (compareFullVertexInfo(read, write) == 0)) {
			// Son iguales. Avanzamos al siguiente.
			read++;
			c++;
		}
		// O read ha llegado al final, o read y write apuntan a vectores
		// con datos diferentes. Avanzamos el de escritura
		write++;
		result++;
		// Copiamos el recién leído diferente en la nueva posición
		// de escritura. Sólo lo hacemos si efectivamente son posiciones
		// diferentes (si ya hemos encontrado alguno igual que hemos quitado)
		if ((c < size) && (write != read))
			*write = *read;
	} // while general

	return result;

} // sortAndClean

//---------------------------------------------------------------------------

/**
 * Busca en el array de vértices auxliar que se suponen ordenados un
 * vértice concreto. Como el array está ordenado, utiliza búsqueda
 * binaria, mediante la función bsearch de la librería de C y el
 * método compareFullVertexInfo. <p>
 * Este es un método auxiliar, y se usa para facilitar la unificación de
 * los arrays de vértices, normales y coordenadas de textura del
 * submodelo. <p>
 * Debido a su uso específico, puede asegurarse que el vértice buscado
 * siempre estará en el array (en modo depuración esto se comprueba
 * con assert). Se devuelve la posición donde está el elemento
 * encontrado.
 * @param vertices Array de vértices ordenado donde buscar.
 * @param size Número de elementos en el array anterior.
 * @param bone Índice del hueso al que se refiere el vértice y la normal
 * a buscar.
 * @param vertex Coordenadas del vértice.
 * @param normal Coordenadas de la normal.
 * @param u Coordenada u de textura.
 * @param v Coordenada v de textura.
 * @return Posición del vértice con esas características dentro del
 * array de vértices.
 */
unsigned int MDLSubmodel::search(FullVertexInfo* vertices, unsigned int size,
									unsigned int bone,
									const vec3_t vertex, 
									const vec3_t normal,
//									MDLBasicType u, MDLBasicType v) {
									MDLshort u, MDLshort v) {
	assert(vertices);
	FullVertexInfo f, *r;
	f.bone = bone;
	f.vertex.set(vertex[0], vertex[1], vertex[2]);
	f.normal.set(normal[0], normal[1], normal[2]);
	f.uCoordS = u; f.vCoordS = v;
	r = (FullVertexInfo*) bsearch(&f, vertices, size, sizeof(FullVertexInfo),
					compareFullVertexInfo);
	assert (r != NULL);

	return (r - vertices);

} // search

//---------------------------------------------------------------------------

/**
 * Macro auxiliar para la implementación más cómoda de
 * MDLSubmodel::compareFullVertexInfo. De hecho, su ámbito es local a esa
 * función.
 */
#define comparaCampo(c) if (elem1->##c < elem2->##c) return -1; \
						else if (elem1->##c > elem2->##c) return 1;

/**
 * Función que compara dos variables de tipo FullVertexInfo. Se usa
 * para ordenar el vector auxiliar con qsort (Quick Sort de la librería
 * de C), y buscar sobre él con bsearch (búsqueda binaria de la libreria
 * de C).
 * @param v1 Puntero al primer FullVertexInfo a comparar.
 * @param v2 Puntero al segundo FullVertexInfo a comparar.
 * @return 
 *	- < 0 si elem1 es menor que elem2
 *	- = 0 si elem1 es igual a eleme2
 *	- > 0 si elem1 es mayor que elem2
 */
int MDLSubmodel::compareFullVertexInfo(const void* v1, const void* v2) {

	FullVertexInfo* elem1 = (FullVertexInfo*) v1;
	FullVertexInfo* elem2 = (FullVertexInfo*) v2;

	// Primero ordenamos por el hueso al que se refiere el vértice
	// (y la normal). Lo hacemos así para que, con un poco de suerte,
	// luego la caché se porte un poco mejor con las matrices de
	// transformación de los huesos... aunque no sé si valdrá o no para
	// algo :-)
	comparaCampo(bone);
	comparaCampo(vertex[0]);
	comparaCampo(vertex[1]);
	comparaCampo(vertex[2]);
	comparaCampo(normal[0]);
	comparaCampo(normal[1]);
	comparaCampo(normal[2]);
	comparaCampo(uCoordS);
	comparaCampo(vCoordS);

	// Si no hemos salido hasta aquí es porque todos los campos son
	// iguales...
	return 0;

#undef comparaCampo

} // comareFullVertexInfo

//---------------------------------------------------------------------------

/**
 * Recibe un puntero con el formato de HalfLifeMDL::mesh_t::triindex y
 * devuelve el número de mallas necesarias para almacenar su información
 * en función del formato de mallas que se quiera generar. Es un método
 * auxiliar usado para la construcción de las mallas del submodelo.<p>
 * Más concretamente, una malla contenida en un objeto MDLMesh contiene
 * una ristra de triángulos, ya sean triángulos independientes, o ristras
 * (en modo fan o strip). La información que llega en el primer parámetro
 * es una malla tal y como la almacena el formato de fichero de modelo de
 * Half Life. Una de estas mallas tiene (o puede tener) varios bloques de
 * triángulos, cada uno formando una ristra (en modo fan o strip). En
 * general, se necesitarán varios objetos MDLMesh para almacenar
 * toda la información contenida en una malla del fichero MDL. Este
 * método devuelve su número exacto.
 * @param mesh Malla de triángulos en el formato de
 * HalfLifeMDL::mesh_t::triindex.
 * @param connectedTriangles a cierto si se quiere mantener la información
 * de las ristras de triángulos. A falso en caso contrario.<br>
 * Si se pasa falso en este parámetro siempre se devolverá un número no
 * superior a 1, pues todos los triángulos se "desenrollarían" para
 * devolverlos en una sola malla de triángulos independientes.
 * @return Número de objetos MDLMesh necesarios para almacenar la
 * información de la malla del segundo parámetro, usando o no ristras
 * de triángulos según lo especificado por el segundo parámetro.
 */
unsigned int MDLSubmodel::numberMeshesNeeded(const MDLshort* mesh,
										 bool connectedTriangles) {
	unsigned int cont = 0;
	if (!connectedTriangles)
		return ((*mesh != 0) ? 1 : 0);

	while (*mesh != 0) {
		cont++;
		if (*mesh < 0)
			mesh += (-*mesh) * 4;
		else
			mesh += (*mesh) * 4;
		++mesh;
	}

	return cont;

} // neededMeshesNumber

} // namespace HalfLifeMDL

