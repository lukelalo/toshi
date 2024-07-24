//---------------------------------------------------------------------------
// MDLSubmodel.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLSubmodel.cpp
 * Contiene la definici�n (implementaci�n) de la clase que almacena la
 * informaci�n sobre un evento de una secuencia del modelo de Half Life.
 * @see HalfLifeMDL::MDLEvent
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 */

#include "MDLBodyPart.h"

namespace HalfLifeMDL {

/**
 * Constructor de los objetos de la clase.
 * @param infoModel Estructura con la informaci�n del [sub]modelo
 * tal y como se ha le�do del fichero. Este puntero debe ser
 * liberado por el usuario (se hace copia).
 * @param offset Desplazamiento de todos los punteros contenidos
 * en infoModel. Los punteros que contiene est�n referidos al
 * origen del fichero, y hay que sumarle el valor de este par�metro
 * para obtener la posici�n en memoria
 * @param connectedTriangles Si se desea que las mallas del modelo
 * mantengan los tri�ngulos conectados, ya sea en modo FAN o en modo
 * STRIP, se debe pasar cierto en este par�metro. Si el motor no es
 * capaz de dibujar tri�ngulos compactos, se pasa falso, de modo que
 * los tri�ngulos se almacenan de forma separada. Vea
 * HalfLifeMDL::MDLMesh para m�s informaci�n.
 * @param clockwise Especifica el orden de los v�rtices que crean la
 * cara delantera en las mallas del modelo que se crear�. Si se desea
 * que los tri�ngulos devueltos tengan su cara frontal con los v�rtices
 * en el sentido de las agujas del reloj hay que especificar cierto en
 * este campo, y falso en caso contrario. <br>
 * Si se solicitan las caras con los v�rtices en el sentido contrario a
 * las agujas del reloj, y manteniendo los tri�ngulos conectados, las
 * mallas en modo STRIP tendr�n un primer tri�ngulo "corrupto", en el
 * que el primer v�rtice y el tercero ser�n el mismo.
 * @param skins Informaci�n sobre las pieles del modelo. Debe haber sido
 * ya creada. El puntero se utiliza durante la fase de construcci�n, pero
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
	// hay que recolocar el array de v�rtices y normales que nos da el MDL
	// para este submodelo dentro de infoModel. Adem�s hay que crear un
	// array para cada coordenada de textura.
	// Lo que hacemos es recorrer todas las mallas (inicialmente sin
	// preocuparnos de si est�n en strip o fan, pues queremos la informaci�n
	// de los v�rtices que usan, no c�mo est�n conectados), y meteremos
	// todos la informaci�n de cada v�rtice en un array. Nos las arreglaremos
	// para quitar los repetidos de ese array, y luego crearemos el array
	// de v�rtices, el array de normales y el de coordenadas de textura
	// a partir de �l. El siguiente paso es construir las mallas para que
	// utilicen esos arrays unificados, de modo que s�lo tengan una matriz
	// de adyacencia cada una, no una para v�rtices, otra para normales
	// y las coordenadas de textura a su aire.

	// Concretando m�s, para que esto no sea demasiado ineficiente, lo
	// que se har� ser� crear un array din�mico de FullVertexInfo. Cada
	// posici�n guarda la informaci�n completa de un v�rtice del submodelo,
	// est� en la malla que est�. El tama�o de ese array ser� el n�mero de
	// v�rtices referenciados en las mallas del submodelo.

	// Inicialmente, se meten todos los v�rtices de todas las mallas, sin
	// preocuparse por los repetidos. El siguiente paso es realizar una
	// ordenaci�n de todos esos v�rtices. En realidad la informaci�n de los
	// v�rtices no tiene un orden claro; da igual cual se utilice, con tal de
	// que sea un orden completo, y que los v�rtices con igual informaci�n
	// aparezcan seguidos en el array.
	// Despu�s se eliminan los repetidos. Es posible hacerlo m�s o menos
	// r�pidamente con una sola pasada sobre el vector, pues ya est� ordenado.

	// Una vez que se tienen los v�rtices �tiles creados, pueden construirse
	// los arrays de v�rtices, normales y coordenadas de textura del
	// submodelo, manteniendo la informaci�n separada en lugar de usando la
	// estructura FullVertexInfo.

	// El �ltimo paso es crear las mallas. La informaci�n de cada una debe
	// ser de nuevo analizada, viendo a qu� v�rtice, normal y coordenadas
	// de textura referencian. Con todos esos datos, se construye un
	// FullVertexInfo que se busca dentro del array auxiliar que se cre�
	// antes. Esa b�squeda ser� binaria, aprovechando el orden artificial
	// creado. La posici�n en el array donde se encuentre el v�rtice con
	// la informaci�n coincidente ser� el referenciado por la matriz de
	// adyacencia de la malla.

	// El c�digo de este m�todo es bastante largo. No estoy a favor de los
	// m�todos largos, por lo que, incialmente, la idea era tener otros
	// dos o tres m�todos auxiliares, a parte de los que ya utiliza. Sin
	// embargo, en el momento de implementar esos m�todos, la cantidad de
	// informaci�n que necesitaban era bastante alta, llegando incluso a
	// necesitar nueve par�metros de variada naturaleza. Para no romper
	// con la norma, era necesario documentar cada uno de ellos, cuyo
	// cometido era muy espec�fico, al igual que cada uno de los m�todos
	// auxiliares. Finalmente he decidido no usar esos m�todos auxiliares
	// a costa de alargar el c�digo de �ste. Los comentarios sobre lo que
	// se va haciendo en cada momento deber�an facilitar la comprensi�n.

	// NOTA: durante alg�n tiempo, el c�digo de este m�todo ocasionaba un
	// extra�o problema: los modelos se mostraban correctamente si el programa
	// estaba compilado en modo depuraci�n, pero no si estaba compilado en
	// modo liberaci�n.
	// Averiguar el problema fue un poco conflictivo, no pudiendo depurar
	// en modo release, y habiendo tantos lugares donde pod�an producirse
	// problemas. Finalmente se descubri� el problema. Por alguna raz�n,
	// al buscar dentro del array de FullVertexInfo (una vez eliminado los
	// repetidos) los v�rtices de cada malla, en modo depuraci�n siempre se
	// encontraban los v�rtices (de hecho hay un assert que detiene la
	// ejecuci�n si eso no ocurr�a). Sin embargo, en modo release, algunos
	// v�rtices no se encontraban. Como el assert s�lo es en modo depuraci�n,
	// no saltaba ;-). Result� que el problema siempre estaba en las
	// coordenadas de textura. En realidad el v�rtice buscado estaba... salvo
	// por un peque�o error de redondeo. De hecho, incre�blemente, si se
	// escrib�an todos los v�rtices del array de FullVertexInfo una vez
	// limpio, hab�a diferencias entre el resultado en modo depuraci�n y
	// en modo release. Las diferencias siempre eran en coordenadas de
	// textura. Por ejemplo, en modo debug se mostraba 0.526859 y en
	// modo release 0.526860, o 0.384298 y 0.384297 por poner otro ejemplo.
	// Incomprensible, francamente.
	// La realidad es que las coordenadas de textura se calculan, no se
	// leen directamente en flotante del fichero. Se calculan a partir del
	// short que almacena el fichero, normalizandolo dividiendolo por el
	// ancho/alto de la textura en p�xeles. No entiendo por qu� en modo 
	// depuraci�n y en modo release el resultado de esa divisi�n resultaba
	// ser diferente :-m En cualquier caso, lo que ocasionaba el error era
	// que al comparar las coordenadas del array limpio, con las coordenadas
	// de cada uno de los v�rtices (que se vuelven a calcular haciendo la
	// divisi�n anterior), resultaban ser diferentes. Supongo que el problema
	// de que fueran diferentes ambas divisiones es por alg�n problema de
	// conversi�n de tipos en la divisi�n... aunque no lo tengo del todo claro
	// En cierto modo ��estoy diciendo que x / y != x / y, con x short e y
	// flotante!!. Para no complicarme la vida y tener dolores de cabeza con
	// la coma flotante y el IEEE ;-) lo que he hecho ha sido comparar
	// directamente los short en lugar de los flotantes con la divisi�n. Eso
	// ha supuesto ampliar FullVertexInfo para que guarde ese short, pero
	// tambi�n la versi�n calculada de las coordenadas de textura para poder
	// asignarlas cuando se construyan los v�rtices finales c�modamente.
	// el c�digo est� lleno de trozos comentados con la versi�n antigua
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
		// El submodelo est� vac�o. Seguramente se utilice para "quitar"
		// la parte del cuerpo que supuestamente recubre, como por
		// ejemplo si la parte del cuerpo es una mochila, que el modelo
		// aparezca sin ella. En ese caso no tenemos que hacer nada m�s.
		// Esto ocurre en el modelo del juego original terror.mdl
		_numMeshes = 0;
		return;
	}

	// Metemos en meshes el puntero a la primera malla del submodelo
	// especificada en el fichero.
	meshes = (mesh_t*)((MDLchar*)infoModel->meshindex + offset);

	// Obtenemos el n�mero de v�rtices que se referencian en todas las mallas.
	maxNumVertices = getNumVertices(meshes, infoModel->nummesh, offset);

	// Creamos el array auxiliar.
	verticesInfo = new FullVertexInfo[maxNumVertices];

	// Hacemos que verts apunte a los v�rtices del submodelo especificados
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
	// en writingVertex la informaci�n completa de cada v�rtice.
	for (contMesh = 0; contMesh < infoModel->nummesh; contMesh++, meshes++) {
		MDLshort* tris;
		int numVert;
		MDLBasicType uFactor, vFactor;

		// Asumimos que las texturas asociadas a la posici�n <meshes->skinref>
		// de todas las pieles tienen el mismo tama�o. Deben tenerlo porque
		// las coordenadas de textura se reciben en un entero corto, y para
		// obtener las coordenadas reales (entre 0 y 1) hay que dividirlo
		// por el tama�o de la textura. Si para pieles diferentes el tama�o
		// de la textura es diferente, las coordenadas de textura resultantes
		// cambiar�an, y ser�a un caos... Vamos, que en alg�n caso podr�a
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
			// Lo �nico que podemos hacer es esperar la excepci�n...
			// (esto es un poco rid�culo, pero bueno).
			uFactor = 0.0;
			vFactor = 0.0;
		}

		// Hacemos que tris apunte al comienzo de la informaci�n de la
		// malla mesh.
		tris = (MDLshort*)((MDLchar*)meshes->triindex + offset);
		// Recorremos todos los v�rtices.
		numVert = *tris;
		while (numVert != 0) {
			if (numVert < 0)
				numVert = -numVert;
			tris++;
			for (unsigned int c = 0; c < numVert; c++) {
				// El valor apuntado por tris es el �ndice del v�rtice.
				writingVertex->bone = vBones[*tris];
				writingVertex->vertex.set(verts[*tris][0],verts[*tris][1],
											verts[*tris][2]);
				tris++;
				// El valor apuntado por tris es el �ndice de la normal.
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
		} // while que recorr�a todos los v�rtices.
	
	} // for que recorr�a todas las mallas

	// Ya tenemos en verticesInfo todos los v�rtices de las mallas. Ahora los
	// ordenamos y eliminamos los repetidos.
	maxNumVertices = sortAndClean(verticesInfo, maxNumVertices);
	assert (maxNumVertices > 0);


#ifndef NDEBUG
	_incrTransformations = maxNumVertices * 2 - 
							(infoModel->numverts  + infoModel->numnorms);
#endif

	// Ya tenemos reducida la matriz de v�rtices. Ahora hay que construir las
	// matrices que mantendr� el submodelo, y luego las matrices de adyacencia.

	_numVertices = maxNumVertices;
	_vertices = new MDLPoint[_numVertices];
	_normals = new MDLPoint[_numVertices];
	_bones = new unsigned int[_numVertices];
	_texCoord = new MDLBasicType[_numVertices*2];
	_repeatedVertices = new int[_numVertices];
	_repeatedNormals = new int[_numVertices];

	// Preparamos la entrada en el bucle. �ste necesita que siempre
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
		// No miramos m�s hacia atr�s de uno en las normales. En los
		// v�rtices no hace falta porque estamos ordenados por v�rtice
		// :-) De todas formas es de esperar que las normales tiendan
		// tambi�n s�lo a coincidir de forma cont�nua.
		if ((verticesInfo[cont].bone == verticesInfo[cont-1].bone) &&
			(verticesInfo[cont].normal == verticesInfo[cont-1].normal))
			_repeatedNormals[cont] = 1;
		else
			_repeatedNormals[cont] = 0;
	}

	// Ahora viene lo divertido... nos toca construir las mallas.
	// Recorremos todas las mallas mesh_t para contar cuantas mallas
	// MDLMesh vamos a tener que construir, seg�n tengamos o no que
	// mantener la conectividad de tri�ngulos.
	_numMeshes = 0;
	meshes = (mesh_t*)((MDLchar*)infoModel->meshindex + offset);
	for (contMesh = 0; contMesh < infoModel->nummesh; ++contMesh, ++meshes)
		_numMeshes += numberMeshesNeeded(
							(MDLshort*)((MDLchar*)meshes->triindex + offset),
							connectedTriangles);

	assert(_numMeshes > 0 && "��N�mero de mallas de un submodelo es 0!!");

	// Pedimos memoria para todas las mallas.
	_meshes = new MDLMesh[_numMeshes];

	// Y ahora las inicializamos.
	meshes = (mesh_t*)((MDLchar*)infoModel->meshindex + offset);
	currentObjMesh = _meshes;

	for (contMesh = 0; contMesh < infoModel->nummesh; ++contMesh, ++meshes) {
		// meshes apunta a una malla que tenemos que construir. Una mesh_t
		// puede originar uno o varios MDLSubmodel, dependiendo de si se
		// va a mantener la informaci�n sobre la conectividad o no.
		int numVert;
		unsigned int cont;
		MDLshort* mesh;
		unsigned int texture;

		texture = meshes->skinref;

		// Asumimos que las texturas asociadas a la posici�n meshes->skinref
		// de todas las pieles tienen el mismo tama�o. Deben tenerlo porque
		// las coordenadas de textura se reciben en un entero corto, y para
		// obtener las coordenadas reales (entre 0 y 1) hay que dividirlo
		// por el tama�o de la textura. Si para pieles diferentes el tama�o
		// de la textura es diferente, las coordenadas de textura resultantes
		// cambiar�an, y ser�a un caos... Vamos, que en alg�n caso podr�a
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
			// Lo �nico que podemos hacer es esperar la excepci�n...
			uFactor = 0.0;
			vFactor = 0.0;
		}

		// Hacemos que mesh apunte a la informaci�n de los tri�ngulos de
		// la malla apuntada por meshes
		mesh = (MDLshort*)((MDLchar*)meshes->triindex + offset);
		// Empezamos a crear mallas.
		if (connectedTriangles) {
			// Debemos mantener la organizaci�n por bloques, con mallas
			// de tipo MT_Fan y MT_Strip.
			numVert = *mesh;
			// Recorremos toda la informaci�n 
			while (numVert != 0) {
				// Hacemos que mesh apunte a la informaci�n del primer v�rtice
				mesh++;
				// Creamos el array para los v�rtices en el objeto malla que
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
					// Necesitamos uno mas para meter un tri�ngulo corrupto
					// que nos de la vuelta al sentido de los tri�ngulos.
					currentObjMesh->_vertices = new unsigned int[numVert + 1];
					currentObjMesh->_numVertices++;
				}
				else
					currentObjMesh->_vertices = new unsigned int[numVert];
				currentObjMesh->_skinTexture = texture;
				// Rellenamos la matriz de adyacencia usando los �ndices de
				// verInfo.
				if (clockwise)
					// Tenemos que dar las mallas tal y como nos las da a
					// nosotros el fichero. No hay que preocuparse m�s.
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
					} // for que recorr�a todos los v�rtices de la nueva malla
				else {
					// Tenemos que dar los tri�ngulos en orden inverso,
					// con la cara frontal teniendo los v�rtices en sentido
					// antihorario. Nos toca pensar un poco m�s.
					if (currentObjMesh->_type == MDLMesh::MT_Fan) {
						// El primer v�rtice es el mismo. Los dem�s van
						// justo al rev�s para poder mantener la relaci�n.
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
						// un tri�ngulo corrupto con los v�rtices 1-0-1
						// Eso supone meter el v�rtice 1, y luego todos los
						// dem�s como normalmente.
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
						} // for que recorr�a todos los v�rtices de la nueva malla
					} // if-else (currentObjMesh->_type == MDLMesh::MT_Fan)
				} // if-else (clockwise)
				numVert = *mesh;
				currentObjMesh++;
			} // while (numVert != 0)
		} // if (connectedTriangles)
		else {
			// Tenemos que "desenrollar" todos los tri�ngulos. S�lo
			// utilizaremos un objeto malla para cada mesh_t.
			currentObjMesh->_type = MDLMesh::MT_Triangles;
			// Lo primero es saber cuantos v�rtices tiene la malla para
			// poder pedir la memoria suficiente.
			const MDLshort* auxMesh = mesh;
			unsigned int numTriangles = 0;
			unsigned int commonVertex1, commonVertex2;
			unsigned int* currentNewVertex;
			/*	Modo "manual" de contar el n�mero de tri�ngulos.
				No hace falta hacerlo as�, porque mesh_t dispone del
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
			} // while que contaba el n�mero de tri�ngulos.
			*/
			numTriangles = meshes->numtris;

			currentObjMesh->_numVertices = 3 * numTriangles;
			currentObjMesh->_vertices = new unsigned int [3 * numTriangles];
			currentObjMesh->_skinTexture = texture;

			// Recorremos toda la malla metiendo los v�rtices desenrollados.
			numVert = *mesh;
			currentNewVertex = currentObjMesh->_vertices;
			while (numVert != 0) {
				// Hacemos que mesh apunte a la informaci�n del primer
				// v�rtice.
				mesh++;
				if (numVert < 0) {
					// Los v�rtices que vamos a leer est�n en modo fan
					numVert = -numVert;
					commonVertex1 = search(verticesInfo, maxNumVertices,
										vBones[*mesh],
										verts[*mesh], norms[*(mesh+1)],
										*(mesh + 2),
										*(mesh + 3));
/*										uFactor * *(mesh + 2),
										vFactor * *(mesh + 3));*/
					mesh += 4; // Avanzamos al siguiente v�rtice.
					for (cont = 0; cont < numVert - 2; cont++) {
						// El tri�ngulo cont est� formado por los v�rtices
						// 0, n y n + 1 si clockwise es cierto, y por los
						// v�rtices 0, n+1, n si clockwise es falso. En
						// commonVertex1 tendremos siempre el v�rtice 0,
						// y mesh apuntar� al v�rtice n.
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
						// Al final, mesh ha avanzado s�lo una vez, y
						// currentNewVertex tres (ha guardado 3 nuevos
						// v�rtices).
					} // for sobre los v�rtices de este bloque en modo fan
					// mesh apunta al v�rtice n + 1 (al �ltimo del �ltimo
					// tri�ngulo). Tenemos que saltarnoslo.
					// Otra forma de verlo es que antes de entrar en el
					// for s�lo incrementamos mesh una vez, y hay que
					// incrementarlo numVert v�rtices, y lo hemos hecho
					// numVert - 1 veces.
					mesh += 4;
				} // if (numVert < 0)
				else {
					// Los v�rtices que vamos a leer est�n en modo strip.
					bool evenTriangle = true;
					commonVertex1 = search(verticesInfo, maxNumVertices,
										vBones[*mesh],
										verts[*mesh], norms[*(mesh+1)],
										*(mesh + 2),
										*(mesh + 3));
/*										uFactor * *(mesh + 2),
										vFactor * *(mesh + 3));*/
					mesh += 4; // Avanzamos al siguiente v�rtice.
					commonVertex2 = search(verticesInfo, maxNumVertices,
										vBones[*mesh],
										verts[*mesh], norms[*(mesh+1)],
										*(mesh + 2),
										*(mesh + 3));
/*										uFactor * *(mesh + 2),
										vFactor * *(mesh + 3));*/
					mesh += 4; // Avanzamos al siguiente v�rtice.
					if (!clockwise) {
						// Hay que invertir los tri�ngulos. Por el uso
						// que vamos a ir haciendo de commonVertex1 y
						// commonVertex2, es suficiente con intercambiarlos
						// y considerar que estamos en un tri�ngulo
						// del tipo contrario.
						unsigned int aux;
						aux = commonVertex1;
						commonVertex1 = commonVertex2;
						commonVertex2 = aux;
						evenTriangle = !evenTriangle;
					}
					for (cont = 0; cont < numVert - 2; cont++) {
						// El tri�ngulo cont est� formado por los v�rtices
						// cont, cont + 1 y cont + 2 si cont es par, y
						// cont + 1, cont y cont + 2 si es impar en el
						// caso de countwise (sentido de las agujas del
						// reloj). En el sentido antihorario, ser�a el
						// tri�ngulo cont + 1, cont y cont + 2 si es
						// par, y cont, cont + 1 y cont + 2 si es impar.
						// De ah� el cambio que se realiza antes de entrar
						// en este bucle si estamos en sentido antihorario.
						// En cualquier caso, nos las arreglaremos para
						// que el tri�ngulo siempre sea
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
					} // for sobre los v�rtices de este bloque en modo strip

				} // if-else (numVert < 0)
				numVert = *mesh;
			} // while (numVert != 0)
			currentObjMesh++;
		} // if-else (connectedTriangles)

	} // for que recorr�a todos los mesh_t del modelo.

	// ��Listo!! Liberamos el array auxiliar antes de terminar.
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
 * Recorre las mallas proporcionadas y da el n�mero de v�rtices que
 * contienen, sin desenrollar ni preocuparse de repeticiones. Simplemente
 * las recorre y devuelve el n�mero de entradas con informaci�n sobre
 * un v�rtice. <p>
 * Este m�todo es auxiliar, y se usa para facilitar la unificaci�n de
 * los arrays de v�rtices, normales y coordenadas de textura del
 * submodelo.
 * @param meshes Puntero al array de mallas del modelo.
 * @param numMeshes N�mero de mallas en el array anterior.
 * @param offset Desplazamiento de todos los punteros contenidos
 * en meshes. Los punteros que contiene est�n referidos al
 * origen del fichero, y hay que sumarle el valor de este par�metro
 * para obtener la posici�n en memoria.
 */
unsigned int MDLSubmodel::getNumVertices(const mesh_t* meshes, 
											unsigned int numMeshes,
											unsigned int offset) {
	unsigned int result, count;
	int i;
	MDLshort* mesh;
	result = 0;

	// Aunque mesh_t::numtris tuviera efectivamente el n�mero de
	// tri�ngulos, no podr�amos usarlo porque queremos el n�mero de
	// v�rtices usados en las listas, y como cada malla puede tener
	// varias secuencias de tri�ngulos no podemos saber cuantos
	// v�rtices se usan para definir los numtris tri�ngulos de la malla
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
 * Es un m�todo auxiliar, y se usa para facilitar la unificaci�n de
 * los arrays de v�rtices, normales y coordenadas de textura del
 * submodelo. <p>
 * La ordenaci�n resultante ser� primero por el hueso al que se refiere
 * el v�rtice y la normal, luego por el v�rtice, despu�s por la normal,
 * y finalmente por las coordenadas de textura.
 * @param vertices Array con la informaci�n de v�rtices a ordenar y
 * limpiar.
 * @param size N�mero de v�rtices en el array.
 * @return N�mero de elementos �tiles en el array final, una vez
 * quitados los repetidos.
 */
unsigned int MDLSubmodel::sortAndClean(FullVertexInfo* vertices,
									 unsigned int size) {

	// Para ordenar usamos la funci�n qsort de la librer�a de
	// C.

	qsort(vertices, size, sizeof(FullVertexInfo), compareFullVertexInfo);

	// Ya est� ordenado. Ahora hay que quitar los repetidos.
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
		// Copiamos el reci�n le�do diferente en la nueva posici�n
		// de escritura. S�lo lo hacemos si efectivamente son posiciones
		// diferentes (si ya hemos encontrado alguno igual que hemos quitado)
		if ((c < size) && (write != read))
			*write = *read;
	} // while general

	return result;

} // sortAndClean

//---------------------------------------------------------------------------

/**
 * Busca en el array de v�rtices auxliar que se suponen ordenados un
 * v�rtice concreto. Como el array est� ordenado, utiliza b�squeda
 * binaria, mediante la funci�n bsearch de la librer�a de C y el
 * m�todo compareFullVertexInfo. <p>
 * Este es un m�todo auxiliar, y se usa para facilitar la unificaci�n de
 * los arrays de v�rtices, normales y coordenadas de textura del
 * submodelo. <p>
 * Debido a su uso espec�fico, puede asegurarse que el v�rtice buscado
 * siempre estar� en el array (en modo depuraci�n esto se comprueba
 * con assert). Se devuelve la posici�n donde est� el elemento
 * encontrado.
 * @param vertices Array de v�rtices ordenado donde buscar.
 * @param size N�mero de elementos en el array anterior.
 * @param bone �ndice del hueso al que se refiere el v�rtice y la normal
 * a buscar.
 * @param vertex Coordenadas del v�rtice.
 * @param normal Coordenadas de la normal.
 * @param u Coordenada u de textura.
 * @param v Coordenada v de textura.
 * @return Posici�n del v�rtice con esas caracter�sticas dentro del
 * array de v�rtices.
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
 * Macro auxiliar para la implementaci�n m�s c�moda de
 * MDLSubmodel::compareFullVertexInfo. De hecho, su �mbito es local a esa
 * funci�n.
 */
#define comparaCampo(c) if (elem1->##c < elem2->##c) return -1; \
						else if (elem1->##c > elem2->##c) return 1;

/**
 * Funci�n que compara dos variables de tipo FullVertexInfo. Se usa
 * para ordenar el vector auxiliar con qsort (Quick Sort de la librer�a
 * de C), y buscar sobre �l con bsearch (b�squeda binaria de la libreria
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

	// Primero ordenamos por el hueso al que se refiere el v�rtice
	// (y la normal). Lo hacemos as� para que, con un poco de suerte,
	// luego la cach� se porte un poco mejor con las matrices de
	// transformaci�n de los huesos... aunque no s� si valdr� o no para
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

	// Si no hemos salido hasta aqu� es porque todos los campos son
	// iguales...
	return 0;

#undef comparaCampo

} // comareFullVertexInfo

//---------------------------------------------------------------------------

/**
 * Recibe un puntero con el formato de HalfLifeMDL::mesh_t::triindex y
 * devuelve el n�mero de mallas necesarias para almacenar su informaci�n
 * en funci�n del formato de mallas que se quiera generar. Es un m�todo
 * auxiliar usado para la construcci�n de las mallas del submodelo.<p>
 * M�s concretamente, una malla contenida en un objeto MDLMesh contiene
 * una ristra de tri�ngulos, ya sean tri�ngulos independientes, o ristras
 * (en modo fan o strip). La informaci�n que llega en el primer par�metro
 * es una malla tal y como la almacena el formato de fichero de modelo de
 * Half Life. Una de estas mallas tiene (o puede tener) varios bloques de
 * tri�ngulos, cada uno formando una ristra (en modo fan o strip). En
 * general, se necesitar�n varios objetos MDLMesh para almacenar
 * toda la informaci�n contenida en una malla del fichero MDL. Este
 * m�todo devuelve su n�mero exacto.
 * @param mesh Malla de tri�ngulos en el formato de
 * HalfLifeMDL::mesh_t::triindex.
 * @param connectedTriangles a cierto si se quiere mantener la informaci�n
 * de las ristras de tri�ngulos. A falso en caso contrario.<br>
 * Si se pasa falso en este par�metro siempre se devolver� un n�mero no
 * superior a 1, pues todos los tri�ngulos se "desenrollar�an" para
 * devolverlos en una sola malla de tri�ngulos independientes.
 * @return N�mero de objetos MDLMesh necesarios para almacenar la
 * informaci�n de la malla del segundo par�metro, usando o no ristras
 * de tri�ngulos seg�n lo especificado por el segundo par�metro.
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

