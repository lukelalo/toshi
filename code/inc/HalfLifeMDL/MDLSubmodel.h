//---------------------------------------------------------------------------
// MDLSubmodel.h
//---------------------------------------------------------------------------

/**
 * @file MDLSubmodel.h
 * Contiene la declaraci�n de la clase que almacena la informaci�n sobre
 * el recubrimiento de una parte del modelo de Half Life.
 * @see HalfLifeMDL::MDLSubmodel
 * @author Pedro Pablo G�mez Mart�n
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
 * Clase que almacena la informaci�n sobre un recubrimiento de una parte de un
 * modelo de Half Life. Cada parte del cuerpo puede tener m�s de un
 * recubrimiento, aunque s�lo se utilizar� uno simult�neamente. Puede obtener
 * m�s informaci�n sobre esto en MDLBodyPart.
 * <p>
 * El constructor de esta clase es complicado y necesita algunos m�todos y
 * tipos auxiliares. La complicaci�n radica en que en el modelo contenido en
 * el fichero cada malla tiene una "matriz de adyacencia" para v�rtices
 * y otra para normales. El submodelo tiene una lista de v�rtices, otra de
 * normales, y para saber cuales usar en una determinada malla hay que
 * pedirle los �ndices de los v�rtices y de las normales, y luego acceder
 * a los arrays del submodelo, a �ndices, hipot�ticamente, diferentes.
 * Adem�s, las coordenadas de textura tambi�n van de forma independiente.
 * Cada v�rtice especifica una coordenada u y v independiente: no existe
 * un array de coordenadas <u,v> en el submodelo que luego todas las
 * mallas referencian. Desde el punto de vista del usuario de la librer�a
 * eso puede ser una molestia. Por ejemplo, no se pueden utilizar
 * directamente los arrays de v�rtices de OpenGL. <p>
 * Por tanto, esta clase reorganiza toda la informaci�n de v�rtices,
 * normales y coordenadas de textura para que las mallas s�lo tengan una
 * matriz de adyacencia y los submodelos contengan un array de v�rtices y
 * uno de normales con las mismas dimensiones, y otro con las coordenadas
 * <code>u</code> y <code>v</code>, de tama�o doble.
 * <p>
 * �sta es una clase "de alto nivel" a la que el usuario del m�dulo tiene
 * acceso. Est� directamente relacionada con la estructura
 * HalfLifeMDL::model_t utilizada en el fichero MDL. Se le ha cambiado el
 * nombre por "Submodel" porque model puede llevar a confusi�n con el modelo
 * completo.
 * 
 * @see HalfLifeMDL::model_t
 * @see HalfLifeMDL::MDLBodyPart
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 * @todo Enlace a la p�gina general de la "arquitectura".
 */
class MDLSubmodel {
public:

	/**
	 * Constructor de los objetos de la clase.
	 * @param infoModel Estructura con la informaci�n del [sub]modelo
	 * tal y como se ha le�do del fichero. Este puntero debe ser
	 * liberado por el usuario (se hace copia).
	 * @param offset Desplazamiento de todos los punteros contenidos
	 * en infoModel. Los punteros que contiene est�n referidos al
	 * origen del fichero, y hay que sumarle el valor de este par�metro
	 * para obtener la posici�n en memoria.
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
	 * Devuelve el n�mero de v�rtices del modelo.
	 * @return N�mero de v�rtices.
	 */
	unsigned int getNumVertices() const { return _numVertices; }

	/**
	 * Devuelve las coordenadas del v�rtice solicitado del modelo.
	 * Para obtener el hueso que proporciona el eje de coordenadas
	 * del v�rtice debe usarse MDLSubmodel::getBone.
	 * @param v �ndice del v�rtice solicitado.
	 * @return Coordenadas del v�rtice solicitado.<br>
 	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLPoint* getVertex(unsigned int v) const {
				assert(v < _numVertices);
				assert(_vertices);
				return &_vertices[v]; }

	/**
	 * Devuelve el array de v�rtices del modelo.<p>
	 * Para obtener el hueso que proporciona el eje de coordenadas del
	 * v�rtice debe usarse MDLSubmodel::getBones.
	 * @return Puntero a la primera posici�n del array de v�rtices.<br>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLPoint* getVertices() const {
				assert(_vertices);
				return _vertices; }

	/**
	 * Devuelve el �ndice del hueso al que est� asociado el v�rtice o
	 * la normal del par�metro.
	 * @param i �ndice del v�rtice o normal cuyo hueso se solicita.
	 * @return �ndice del hueso dentro del modelo al que pertenece este
	 * submodelo respecto a cuyo sistema de coordenadas est� descrito
	 * el v�rtice v y su normal. Las coordenadas pueden obtenerse con
	 * MDLSubmodel::getVertex o MDLSubmodel::getNormal
	 */
	const unsigned int getBone(unsigned int i) const {
				assert(i < _numVertices);
				assert(_bones);
				return _bones[i]; }

	/**
	 * Devuelve el array de los �ndices de los huesos a los que est�n
	 * asociados los v�rtices y normales del modelo. La posici�n i se
	 * refiere al v�rtice i-�simo, es decir al v�rtice proporcionado por
	 * MDLSubmodel::getVertex con i como par�metro, o la posici�n
	 * i-�sima del vector devuelto por getVertices. <p>
	 * Como las normales comparten el hueso de sus v�rtices, todo lo
	 * anterior es v�lido para la normal i-�xima.
	 * @return Puntero a la primera posici�n del array de huesos de los
	 * v�rtices y normales.<br>
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
	 * @param n �ndice de la normal solicitada.
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
	 * @return Puntero a la primera posici�n del array de normales.<br>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLPoint* getNormals() const {
				assert(_normals);
				return _normals; }

	/**
	 * Devuelve un array con informaci�n sobre repetici�n de v�rtices.
	 * Al unificarse los v�rtices, las normales y las coordenadas de
	 * textura en arrays "sincronizados" por el �ndice, en la lista de
	 * v�rtices aparecen elementos repetidos. Eso supone realizar m�s 
	 * transformaciones de las necesarias. <p>
	 * Para evitarlo, este m�todo devuelve un array que almacena informaci�n
	 * sobre repetici�n de v�rtices dentro del array devuelto por
	 * getVertices(), que puede ser usada para ahorrar transformaciones. <p>
	 * En concreto, si el elemento i�simo del array devuelto vale 0,
	 * entonces la posici�n i del array de v�rtices contendr� la primera
	 * aparici�n del v�rtice. Si la posici�n i�sima vale u, el v�rtice
	 * i ser� el mismo que el v�rtice i - u, y el hueso i�simo ser� el
	 * mismo que el i - u. <p>
	 * La informaci�n en las posiciones i�simas de los v�rtices y de los
	 * huesos est� correctamente inicializada, de modo que el uso del array
	 * devuelto por este m�todo en realidad es opcional.
	 * @return Array con informaci�n sobre repetici�n de v�rtices.
	 */
	const int* getRepeatedVertices() const {
				assert (_repeatedVertices);
				return _repeatedVertices; }

	/**
	 * Devuelve un array con informaci�n sobre repetici�n de normales.
	 * Al unificarse los v�rtices, las normales y las coordenadas de
	 * textura en arrays "sincronizados" por el �ndice, en la lista de
	 * normales aparecen elementos repetidos. Eso supone realizar m�s 
	 * transformaciones de las necesarias. <p>
	 * Para evitarlo, este m�todo devuelve un array que almacena informaci�n
	 * sobre repetici�n de normales dentro del array devuelto por
	 * getNormals(), que puede ser usada para ahorrar transformaciones. <p>
	 * En concreto, si el elemento i�simo del array devuelto vale 0,
	 * entonces la posici�n i del array de normales contendr� la primera
	 * aparici�n de la normal. Si la posici�n i�sima vale u, la normal
	 * i ser� la misma que la i - u, y el hueso i�simo ser� el
	 * mismo que el i - u. <p>
	 * La informaci�n en las posiciones i�simas de las normales y de los
	 * huesos est� correctamente inicializada, de modo que el uso del array
	 * devuelto por este m�todo en realidad es opcional.
	 * @return Array con informaci�n sobre repetici�n de normales.
	 */
	const int* getRepeatedNormals() const {
				assert (_repeatedNormals);
				return _repeatedNormals; }

	/**
	 * Devuelve un array de dos posiciones con las coordenadas de textura
	 * <code>u</code> y <code>v</code> del v�rtice de �ndice solicitada.
	 * @param i �ndice del v�rtice cuyas coordenadas de textura se desean
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
	 * Devuelve un array con las coordenadas de textura de todos los v�rtices
	 * del modelo. <p> 
	 * Las coordenadas se almacenan por v�rtice, usandose dos posiciones
	 * del array por v�rtice. La estructura es:<p>
	 * <center><tt>[u0 v0 u1 v1 u2 v2 ... uN vN]</tt></center>
	 * <p>
	 * Por lo tanto, en general, las posiciones 2*i y 2*i + 1 del array
	 * almacenan las coordenadas de textura del v�rtice i�simo.
	 * El tama�o del array devuelto es getNumvertices() * 2.
	 * @return Puntero a un array de coordenadas de textura. <br>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLBasicType* getTextureCoords() const {
				assert(_texCoord);
				return _texCoord; }

	/**
	 * Devuelve el n�mero de mallas del submodelo.
	 * @return N�mero de mallas del submodelo.
	 */
	const unsigned int getNumMeshes() const { return _numMeshes; }

	/**
	 * Devuelve la malla solicitada.
	 * @param mesh �ndice de la malla que se desea obtener.
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
	 * Devuelve la diferencia entre el n�mero de transformaciones
	 * necesarias por cada fotograma con la unificaci�n de las matrices
	 * de adyacencia y sin ella. En concreto, es la resta el doble del
	 * n�mero de v�rtices "unificados" (el doble porque hay el mismo
	 * n�mero de v�rtices que de normales) menos el n�mero de v�rtices
	 * y de normales que hab�a en el submodelo seg�n se guardaba en el MDL.<p>
	 * Este m�todo s�lo tiene utilidad para estad�sticas por curiosidad.
	 * De hecho s�lo existe en la compilaci�n en modo depuraci�n.
	 */
	int getIncrTransformations() const { return _incrTransformations; }
#endif

protected:

	/**
	 * Tipo auxiliar para fusionar las matrices de adyacencia de las mallas.
	 * Contiene toda la informaci�n de un v�rtice de una malla. Se usa
	 * un array de elementos con esta estructura para buscar repetidos y
	 * reorganizar las mallas.
	 */
	typedef struct {

		/**
		 * Hueso cuyo sistema de coordenadas se usa para especificar la
		 * posici�n del v�rtice y la orientaci�n de la normal.
		 */
		unsigned int bone;

		/**
		 * Coordenadas del v�rtice en el sistema del hueso vertexBone.
		 */
		MDLPoint vertex;

		/**
		 * Coordenadas de la normal del v�rtice en el sistema del hueso
		 * normalBone.
		 */
		MDLPoint normal;

		/**
		 * Coordenada u de la textura.
		 */
		MDLBasicType uCoord;

		/**
		 * Coordenada u de la textura sin normalizar. La normalizaci�n se
		 * realiza dividiendo este valor por el ancho en p�xeles de la
		 * textura. Se almacena este valor para utilizarlo en las
		 * comparaciones. El campo uCoord se calcula a partir de uCoordS
		 * y el ancho de la textura, por lo que al comparar y ordenar dos
		 * variables de este tipo s�lo se utiliza el campo uCoordS y no
		 * uCoord.
		 */
		MDLshort uCoordS;

		/**
		 * Coordenada v de la textura.
		 */
		MDLBasicType vCoord;

		/**
		 * Coordenada v de la textura sin normalizar. La normalizaci�n se
		 * realiza dividiendo este valor por el alto en p�xeles de la
		 * textura. Se almacena este valor para utilizarlo en las
		 * comparaciones. El campo vCoord se calcula a partir de vCoordS
		 * y el alto de la textura, por lo que al comparar y ordenar dos
		 * variables de este tipo s�lo se utiliza el campo vCoordS y no
		 * vCoord.
		 */
		MDLshort vCoordS;

	} FullVertexInfo;

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
	static unsigned int getNumVertices(const mesh_t* meshes, 
										unsigned int numMeshes,
										unsigned int offset);

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
	static unsigned int sortAndClean(FullVertexInfo* vertices,
												unsigned int size);

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
	static unsigned int search(FullVertexInfo* vertices, unsigned int size,
								unsigned int bone,
								const vec3_t vertex, 
								const vec3_t normal,
//								MDLBasicType u, MDLBasicType v);
								MDLshort u, MDLshort v);

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
	static int compareFullVertexInfo(const void* v1, const void* v2);

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
	static unsigned int numberMeshesNeeded(const MDLshort* mesh,
											bool connectedTriangles);

	/**
	 * Nombre del submodelo.
	 */
	std::string _name;

	/**
	 * N�mero de v�rtices del submodelo.
	 */
	unsigned int _numVertices;

	/**
	 * V�rtices del modelo. Cada v�rtice tiene las coordenadas proporcionadas
	 * en funci�n de un hueso del modelo. El hueso al que se refiere se
	 * almacena en MDLSubmodel::_bones.
	 */
	MDLPoint* _vertices;

	/**
	 * Normales de los v�rtices del modelo. Cada normal tiene las coordenadas
	 * especificadas en funci�n del hueso al que est� asociado el v�rtice.
	 * Ese hueso se almacena en MDLSubmodel::_bones. La normal del v�rtice
	 * especificado en _vertices[i] se almacena en _normals[i].
	 */
	MDLPoint* _normals;

	/**
	 * Huesos respecto a los que se proporcionan las coordenadas de cada
	 * v�rtice del array MDLSubmodel::_vertices y su normal del array
	 * MDLSubmodel::_normals. Es el �ndice del hueso dentro de los huesos
	 * del modelo completo al que pertenece el submodelo.
	 */
	unsigned int* _bones;

	/**
	 * Coordenadas de textura de los v�rtices del modelo. Las coordenadas
	 * se almacenan seguidas, colocandose primero la coordenada u del
	 * v�rtice 0, luego la coordenad v de ese mismo v�rtice, posteriormente
	 * las coordenadas u,v del v�rtice 1, etc�tera. Por tanto, el tama�o
	 * de este array ser� _numVertices * 2.<br>
	 * En general, las posiciones 2*i y 2*i + 1 del array, almacenan las
	 * coordenadas de textura del v�rtice _vertices[i].
	 */
	MDLBasicType* _texCoord;

	/**
	 * Informaci�n sobre repetici�n de v�rtices. Al unificarse los v�rtices,
	 * las normales y las coordenadas de textura en arrays "sincronizados"
	 * por el �ndice, en la lista de v�rtices aparecen elementos repetidos.
	 * Eso supone realizar m�s transformaciones de las necesarias. Para
	 * evitarlo, este array almacena informaci�n sobre repetici�n de v�rtices
	 * dentro del array _vertices, que puede ser usada para ahorrar
	 * transformaciones. <p>
	 * En concreto, si _repeatedVertices[i] vale 0, entonces la posici�n
	 * i del array _vertices contendr� la primera aparici�n del v�rtice. Si
	 * _repeatedVertices[i] vale u, el elemento _vertices[i] contendr� el
	 * mismo valor que _vertices[i-u], y _bones[i] ser� igual que _bones[i-u].
	 * La informaci�n en las posiciones i�simas de _vertices y _bones est�
	 * correctamente inicializada, de modo que el uso del array
	 * _repeatedVertices en realidad es opcional.
	 */
	int* _repeatedVertices;

	/**
	 * Informaci�n sobre repetici�n de normales. Al unificarse los v�rtices,
	 * las normales y las coordenadas de textura en arrays "sincronizados"
	 * por el �ndice, en la lista de normales aparecen elementos repetidos.
	 * Eso supone realizar m�s transformaciones de las necesarias. Para
	 * evitarlo, este array almacena informaci�n sobre repetici�n de normales
	 * dentro del array _normals, que puede ser usada para ahorrar
	 * transformaciones. <p>
	 * En concreto, si _repeatedNormals[i] vale 0, entonces la posici�n
	 * i del array _normals contendr� la primera aparici�n de la normal. Si
	 * _repeatedNormals[i] vale u, el elemento _normals[i] contendr� el
	 * mismo valor que _normals[i-u], y _bones[i] ser� igual que _bones[i-u].
	 * La informaci�n en las posiciones i�simas de _normals y _bones est�
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
	 * Diferencia entre el n�mero de transformaciones necesarias por cada
	 * fotograma con la unificaci�n de las matrices de adyacencia y sin
	 * ella. En concreto, es la resta el doble del n�mero de v�rtices
	 * "unificados" (el doble porque hay el mismo n�mero de v�rtices
	 * que de normales) menos el n�mero de v�rtices y de normales que
	 * hab�a en el submodelo seg�n se guardaba en el MDL. <p>
	 * Este campo s�lo tiene utilidad para estad�sticas por curiosidad.
	 * De hecho s�lo existe en la compilaci�n en modo depuraci�n.
	 */
	int _incrTransformations;
#endif

private:
	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLSubmodel(const MDLSubmodel& source) {
		assert(!"Constructor copia MDLSubmodel no disponible"); }

	/**
	 * Operador de asignaci�n privado para evitar problemas en los
	 * delete. Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLSubmodel& operator=(const MDLSubmodel &source) {
		assert(!"Operador asignaci�n en MDLSubmodel no disponible");
		return *this;}


}; // class MDLSubmodel

} // namespace HalfLifeMDL

#endif
