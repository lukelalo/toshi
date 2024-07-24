//---------------------------------------------------------------------------
// MDLSubmodel.h
//---------------------------------------------------------------------------

/**
 * @file MDLSubmodel.h
 * Contiene la declaración de la clase que almacena la información sobre
 * el recubrimiento de una parte del modelo de Half Life.
 * @see HalfLifeMDL::MDLSubmodel
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLSubmodel
#define __HalfLifeMDL_MDLSubmodel

#include <string>

#include "MDLBasic.h"
#include "MDLPoint.h"
#include "MDLMesh.h"

namespace HalfLifeMDL {

/**
 * Clase que almacena la información sobre un recubrimiento de una parte de un
 * modelo de Half Life. Cada parte del cuerpo puede tener más de un
 * recubrimiento, aunque sólo se utilizará uno simultáneamente. Puede obtener
 * más información sobre esto en MDLBodyPart.
 * <p>
 * El constructor de esta clase es complicado y necesita algunos métodos y
 * tipos auxiliares. La complicación radica en que en el modelo contenido en
 * el fichero cada malla tiene una "matriz de adyacencia" para vértices
 * y otra para normales. El submodelo tiene una lista de vértices, otra de
 * normales, y para saber cuales usar en una determinada malla hay que
 * pedirle los índices de los vértices y de las normales, y luego acceder
 * a los arrays del submodelo, a índices, hipotéticamente, diferentes.
 * Además, las coordenadas de textura también van de forma independiente.
 * Cada vértice especifica una coordenada u y v independiente: no existe
 * un array de coordenadas <u,v> en el submodelo que luego todas las
 * mallas referencian. Desde el punto de vista del usuario de la librería
 * eso puede ser una molestia. Por ejemplo, no se pueden utilizar
 * directamente los arrays de vértices de OpenGL. <p>
 * Por tanto, esta clase reorganiza toda la información de vértices,
 * normales y coordenadas de textura para que las mallas sólo tengan una
 * matriz de adyacencia y los submodelos contengan un array de vértices y
 * uno de normales con las mismas dimensiones, y otro con las coordenadas
 * <code>u</code> y <code>v</code>, de tamaño doble.
 * <p>
 * Ésta es una clase "de alto nivel" a la que el usuario del módulo tiene
 * acceso. Está directamente relacionada con la estructura
 * HalfLifeMDL::model_t utilizada en el fichero MDL. Se le ha cambiado el
 * nombre por "Submodel" porque model puede llevar a confusión con el modelo
 * completo.
 * 
 * @see HalfLifeMDL::model_t
 * @see HalfLifeMDL::MDLBodyPart
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 * @todo Enlace a la página general de la "arquitectura".
 */
class MDLSubmodel {
public:

	/**
	 * Constructor de los objetos de la clase.
	 * @param infoModel Estructura con la información del [sub]modelo
	 * tal y como se ha leído del fichero. Este puntero debe ser
	 * liberado por el usuario (se hace copia).
	 * @param offset Desplazamiento de todos los punteros contenidos
	 * en infoModel. Los punteros que contiene están referidos al
	 * origen del fichero, y hay que sumarle el valor de este parámetro
	 * para obtener la posición en memoria.
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
	MDLSubmodel(const model_t* infoModel, unsigned int offset,
							bool connectedTriangles,  bool clockwise,
							const MDLSkins* skins);

	/**
	 * Destructor. Libera la memoria utilizada.
	 */
	~MDLSubmodel();

	/**
	 * Devuelve el nombre del modelo.
	 * @return Nombre del modelo.
	 */
	const std::string& getName() const { return _name; }

	/**
	 * Devuelve el número de vértices del modelo.
	 * @return Número de vértices.
	 */
	unsigned int getNumVertices() const { return _numVertices; }

	/**
	 * Devuelve las coordenadas del vértice solicitado del modelo.
	 * Para obtener el hueso que proporciona el eje de coordenadas
	 * del vértice debe usarse MDLSubmodel::getBone.
	 * @param v Índice del vértice solicitado.
	 * @return Coordenadas del vértice solicitado.<br>
 	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLPoint* getVertex(unsigned int v) const {
				assert(v < _numVertices);
				assert(_vertices);
				return &_vertices[v]; }

	/**
	 * Devuelve el array de vértices del modelo.<p>
	 * Para obtener el hueso que proporciona el eje de coordenadas del
	 * vértice debe usarse MDLSubmodel::getBones.
	 * @return Puntero a la primera posición del array de vértices.<br>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLPoint* getVertices() const {
				assert(_vertices);
				return _vertices; }

	/**
	 * Devuelve el índice del hueso al que está asociado el vértice o
	 * la normal del parámetro.
	 * @param i Índice del vértice o normal cuyo hueso se solicita.
	 * @return Índice del hueso dentro del modelo al que pertenece este
	 * submodelo respecto a cuyo sistema de coordenadas está descrito
	 * el vértice v y su normal. Las coordenadas pueden obtenerse con
	 * MDLSubmodel::getVertex o MDLSubmodel::getNormal
	 */
	const unsigned int getBone(unsigned int i) const {
				assert(i < _numVertices);
				assert(_bones);
				return _bones[i]; }

	/**
	 * Devuelve el array de los índices de los huesos a los que están
	 * asociados los vértices y normales del modelo. La posición i se
	 * refiere al vértice i-ésimo, es decir al vértice proporcionado por
	 * MDLSubmodel::getVertex con i como parámetro, o la posición
	 * i-ésima del vector devuelto por getVertices. <p>
	 * Como las normales comparten el hueso de sus vértices, todo lo
	 * anterior es válido para la normal i-éxima.
	 * @return Puntero a la primera posición del array de huesos de los
	 * vértices y normales.<br>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const unsigned int* getBones() const {
				assert(_bones);
				return _bones; }

	/**
	 * Devuelve las coordenadas de la normal solicitada del modelo.
	 * Para obtener el hueso que proporciona el eje de coordenadas
	 * de la normal debe usarse MDLSubmodel::getBone.
	 * @param n Índice de la normal solicitada.
	 * @return Coordenadas de la normal solicitado.<br>
 	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLPoint* getNormal(unsigned int n) const {
				assert(n < _numVertices);
				assert(_normals);
				return &_normals[n]; }

	/**
	 * Devuelve el array de normales del modelo.<p>
	 * Para obtener el hueso que proporciona el eje de coordenadas de la
	 * normal debe usarse MDLSubmodel::getBones.
	 * @return Puntero a la primera posición del array de normales.<br>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLPoint* getNormals() const {
				assert(_normals);
				return _normals; }

	/**
	 * Devuelve un array con información sobre repetición de vértices.
	 * Al unificarse los vértices, las normales y las coordenadas de
	 * textura en arrays "sincronizados" por el índice, en la lista de
	 * vértices aparecen elementos repetidos. Eso supone realizar más 
	 * transformaciones de las necesarias. <p>
	 * Para evitarlo, este método devuelve un array que almacena información
	 * sobre repetición de vértices dentro del array devuelto por
	 * getVertices(), que puede ser usada para ahorrar transformaciones. <p>
	 * En concreto, si el elemento iésimo del array devuelto vale 0,
	 * entonces la posición i del array de vértices contendrá la primera
	 * aparición del vértice. Si la posición iésima vale u, el vértice
	 * i será el mismo que el vértice i - u, y el hueso iésimo será el
	 * mismo que el i - u. <p>
	 * La información en las posiciones iésimas de los vértices y de los
	 * huesos está correctamente inicializada, de modo que el uso del array
	 * devuelto por este método en realidad es opcional.
	 * @return Array con información sobre repetición de vértices.
	 */
	const int* getRepeatedVertices() const {
				assert (_repeatedVertices);
				return _repeatedVertices; }

	/**
	 * Devuelve un array con información sobre repetición de normales.
	 * Al unificarse los vértices, las normales y las coordenadas de
	 * textura en arrays "sincronizados" por el índice, en la lista de
	 * normales aparecen elementos repetidos. Eso supone realizar más 
	 * transformaciones de las necesarias. <p>
	 * Para evitarlo, este método devuelve un array que almacena información
	 * sobre repetición de normales dentro del array devuelto por
	 * getNormals(), que puede ser usada para ahorrar transformaciones. <p>
	 * En concreto, si el elemento iésimo del array devuelto vale 0,
	 * entonces la posición i del array de normales contendrá la primera
	 * aparición de la normal. Si la posición iésima vale u, la normal
	 * i será la misma que la i - u, y el hueso iésimo será el
	 * mismo que el i - u. <p>
	 * La información en las posiciones iésimas de las normales y de los
	 * huesos está correctamente inicializada, de modo que el uso del array
	 * devuelto por este método en realidad es opcional.
	 * @return Array con información sobre repetición de normales.
	 */
	const int* getRepeatedNormals() const {
				assert (_repeatedNormals);
				return _repeatedNormals; }

	/**
	 * Devuelve un array de dos posiciones con las coordenadas de textura
	 * <code>u</code> y <code>v</code> del vértice de índice solicitada.
	 * @param i Índice del vértice cuyas coordenadas de textura se desean
	 * conocer.
	 * @return Puntero a un array de dos elementos con las coordenadas
	 * <code>u</code> y <code>v</code>. <br>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLBasicType* getTextureCoords(unsigned int i) const {
				assert(i < _numVertices);
				assert(_texCoord);
				return &(_texCoord[2*i]); }

	/**
	 * Devuelve un array con las coordenadas de textura de todos los vértices
	 * del modelo. <p> 
	 * Las coordenadas se almacenan por vértice, usandose dos posiciones
	 * del array por vértice. La estructura es:<p>
	 * <center><tt>[u0 v0 u1 v1 u2 v2 ... uN vN]</tt></center>
	 * <p>
	 * Por lo tanto, en general, las posiciones 2*i y 2*i + 1 del array
	 * almacenan las coordenadas de textura del vértice iésimo.
	 * El tamaño del array devuelto es getNumvertices() * 2.
	 * @return Puntero a un array de coordenadas de textura. <br>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLBasicType* getTextureCoords() const {
				assert(_texCoord);
				return _texCoord; }

	/**
	 * Devuelve el número de mallas del submodelo.
	 * @return Número de mallas del submodelo.
	 */
	const unsigned int getNumMeshes() const { return _numMeshes; }

	/**
	 * Devuelve la malla solicitada.
	 * @param mesh Índice de la malla que se desea obtener.
	 * @return Malla solicitada.<br>
 	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLMesh* getMesh(unsigned int mesh) const {
				assert(mesh < _numMeshes);
				assert(_meshes);
				return &_meshes[mesh]; }

#ifndef NDEBUG
	/**
	 * Devuelve la diferencia entre el número de transformaciones
	 * necesarias por cada fotograma con la unificación de las matrices
	 * de adyacencia y sin ella. En concreto, es la resta el doble del
	 * número de vértices "unificados" (el doble porque hay el mismo
	 * número de vértices que de normales) menos el número de vértices
	 * y de normales que había en el submodelo según se guardaba en el MDL.<p>
	 * Este método sólo tiene utilidad para estadísticas por curiosidad.
	 * De hecho sólo existe en la compilación en modo depuración.
	 */
	int getIncrTransformations() const { return _incrTransformations; }
#endif

protected:

	/**
	 * Tipo auxiliar para fusionar las matrices de adyacencia de las mallas.
	 * Contiene toda la información de un vértice de una malla. Se usa
	 * un array de elementos con esta estructura para buscar repetidos y
	 * reorganizar las mallas.
	 */
	typedef struct {

		/**
		 * Hueso cuyo sistema de coordenadas se usa para especificar la
		 * posición del vértice y la orientación de la normal.
		 */
		unsigned int bone;

		/**
		 * Coordenadas del vértice en el sistema del hueso vertexBone.
		 */
		MDLPoint vertex;

		/**
		 * Coordenadas de la normal del vértice en el sistema del hueso
		 * normalBone.
		 */
		MDLPoint normal;

		/**
		 * Coordenada u de la textura.
		 */
		MDLBasicType uCoord;

		/**
		 * Coordenada u de la textura sin normalizar. La normalización se
		 * realiza dividiendo este valor por el ancho en píxeles de la
		 * textura. Se almacena este valor para utilizarlo en las
		 * comparaciones. El campo uCoord se calcula a partir de uCoordS
		 * y el ancho de la textura, por lo que al comparar y ordenar dos
		 * variables de este tipo sólo se utiliza el campo uCoordS y no
		 * uCoord.
		 */
		MDLshort uCoordS;

		/**
		 * Coordenada v de la textura.
		 */
		MDLBasicType vCoord;

		/**
		 * Coordenada v de la textura sin normalizar. La normalización se
		 * realiza dividiendo este valor por el alto en píxeles de la
		 * textura. Se almacena este valor para utilizarlo en las
		 * comparaciones. El campo vCoord se calcula a partir de vCoordS
		 * y el alto de la textura, por lo que al comparar y ordenar dos
		 * variables de este tipo sólo se utiliza el campo vCoordS y no
		 * vCoord.
		 */
		MDLshort vCoordS;

	} FullVertexInfo;

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
	static unsigned int getNumVertices(const mesh_t* meshes, 
										unsigned int numMeshes,
										unsigned int offset);

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
	static unsigned int sortAndClean(FullVertexInfo* vertices,
												unsigned int size);

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
	static unsigned int search(FullVertexInfo* vertices, unsigned int size,
								unsigned int bone,
								const vec3_t vertex, 
								const vec3_t normal,
//								MDLBasicType u, MDLBasicType v);
								MDLshort u, MDLshort v);

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
	static int compareFullVertexInfo(const void* v1, const void* v2);

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
	static unsigned int numberMeshesNeeded(const MDLshort* mesh,
											bool connectedTriangles);

	/**
	 * Nombre del submodelo.
	 */
	std::string _name;

	/**
	 * Número de vértices del submodelo.
	 */
	unsigned int _numVertices;

	/**
	 * Vértices del modelo. Cada vértice tiene las coordenadas proporcionadas
	 * en función de un hueso del modelo. El hueso al que se refiere se
	 * almacena en MDLSubmodel::_bones.
	 */
	MDLPoint* _vertices;

	/**
	 * Normales de los vértices del modelo. Cada normal tiene las coordenadas
	 * especificadas en función del hueso al que está asociado el vértice.
	 * Ese hueso se almacena en MDLSubmodel::_bones. La normal del vértice
	 * especificado en _vertices[i] se almacena en _normals[i].
	 */
	MDLPoint* _normals;

	/**
	 * Huesos respecto a los que se proporcionan las coordenadas de cada
	 * vértice del array MDLSubmodel::_vertices y su normal del array
	 * MDLSubmodel::_normals. Es el índice del hueso dentro de los huesos
	 * del modelo completo al que pertenece el submodelo.
	 */
	unsigned int* _bones;

	/**
	 * Coordenadas de textura de los vértices del modelo. Las coordenadas
	 * se almacenan seguidas, colocandose primero la coordenada u del
	 * vértice 0, luego la coordenad v de ese mismo vértice, posteriormente
	 * las coordenadas u,v del vértice 1, etcétera. Por tanto, el tamaño
	 * de este array será _numVertices * 2.<br>
	 * En general, las posiciones 2*i y 2*i + 1 del array, almacenan las
	 * coordenadas de textura del vértice _vertices[i].
	 */
	MDLBasicType* _texCoord;

	/**
	 * Información sobre repetición de vértices. Al unificarse los vértices,
	 * las normales y las coordenadas de textura en arrays "sincronizados"
	 * por el índice, en la lista de vértices aparecen elementos repetidos.
	 * Eso supone realizar más transformaciones de las necesarias. Para
	 * evitarlo, este array almacena información sobre repetición de vértices
	 * dentro del array _vertices, que puede ser usada para ahorrar
	 * transformaciones. <p>
	 * En concreto, si _repeatedVertices[i] vale 0, entonces la posición
	 * i del array _vertices contendrá la primera aparición del vértice. Si
	 * _repeatedVertices[i] vale u, el elemento _vertices[i] contendrá el
	 * mismo valor que _vertices[i-u], y _bones[i] será igual que _bones[i-u].
	 * La información en las posiciones iésimas de _vertices y _bones está
	 * correctamente inicializada, de modo que el uso del array
	 * _repeatedVertices en realidad es opcional.
	 */
	int* _repeatedVertices;

	/**
	 * Información sobre repetición de normales. Al unificarse los vértices,
	 * las normales y las coordenadas de textura en arrays "sincronizados"
	 * por el índice, en la lista de normales aparecen elementos repetidos.
	 * Eso supone realizar más transformaciones de las necesarias. Para
	 * evitarlo, este array almacena información sobre repetición de normales
	 * dentro del array _normals, que puede ser usada para ahorrar
	 * transformaciones. <p>
	 * En concreto, si _repeatedNormals[i] vale 0, entonces la posición
	 * i del array _normals contendrá la primera aparición de la normal. Si
	 * _repeatedNormals[i] vale u, el elemento _normals[i] contendrá el
	 * mismo valor que _normals[i-u], y _bones[i] será igual que _bones[i-u].
	 * La información en las posiciones iésimas de _normals y _bones está
	 * correctamente inicializada, de modo que el uso del array
	 * _repeatedNormals en realidad es opcional.
	 */
	int* _repeatedNormals;

	/**
	 * Puntero a las mallas del modelo.
	 */
	MDLMesh *_meshes;

	/**
	 * Numero de mallas del submodelo.
	 */
	unsigned int _numMeshes;

#ifndef NDEBUG
	/**
	 * Diferencia entre el número de transformaciones necesarias por cada
	 * fotograma con la unificación de las matrices de adyacencia y sin
	 * ella. En concreto, es la resta el doble del número de vértices
	 * "unificados" (el doble porque hay el mismo número de vértices
	 * que de normales) menos el número de vértices y de normales que
	 * había en el submodelo según se guardaba en el MDL. <p>
	 * Este campo sólo tiene utilidad para estadísticas por curiosidad.
	 * De hecho sólo existe en la compilación en modo depuración.
	 */
	int _incrTransformations;
#endif

private:
	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementación es vacía y no debe usarse.
	 */
	MDLSubmodel(const MDLSubmodel& source) {
		assert(!"Constructor copia MDLSubmodel no disponible"); }

	/**
	 * Operador de asignación privado para evitar problemas en los
	 * delete. Su implementación es vacía y no debe usarse.
	 */
	MDLSubmodel& operator=(const MDLSubmodel &source) {
		assert(!"Operador asignación en MDLSubmodel no disponible");
		return *this;}


}; // class MDLSubmodel

} // namespace HalfLifeMDL

#endif
