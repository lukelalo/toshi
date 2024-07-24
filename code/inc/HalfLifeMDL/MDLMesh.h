//---------------------------------------------------------------------------
// MDLMesh.h
//---------------------------------------------------------------------------

/**
 * @file MDLMesh.h
 * Contiene la declaración de la clase que almacena la información sobre
 * una malla de un modelo de Half Life.
 * @see HalfLifeMDL::MDLMesh
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLMesh
#define __HalfLifeMDL_MDLMesh

#include <string>
#include <assert.h>

#include "MDLBasicType.h"
#include "MDLSkins.h"

namespace HalfLifeMDL {

class MDLSubmodel;

/**
 * Clase que almacena la información sobre una malla del modelo de Half Life.
 * Un objeto de esta clase no tiene ningún sentido tratado de forma aislada.
 * La información que contiene se refiere a los datos almacenados por el
 * submodelo que lo contiene (HalfLifeMDL::MDLSubmodel) en el caso de los
 * vértices y las normales, y a los datos del modelo completo al que pertenece
 * ese submodelo en el caso de las texturas.
 * <p>
 * Ésta es una clase "de alto nivel" a la que el usuario del módulo tiene
 * acceso. Está directamente relacionada con la estructura
 * HalfLifeMDL::mesh_t utilizada en el fichero MDL, aunque no siempre la
 * relación es uno a uno. Una estructura mesh_t contiene toda la información
 * de una malla, que puede estar formada por varios bloques de triángulos,
 * unos representados en modo FAN (todos comparten un vértices), y otros
 * en modo STRIP (el triángulo n comparte dos vértices con el n-1). Las
 * mallas contenidas en esta clase sólo contienen uno de esos bloques si
 * el usuario del módulo ha solicitado empaquetar el modelo manteniendo
 * los bloques (vease HalfLifeMDL::MDLCore::pack), o a toda la malla en
 * caso contrario (y se almacenarán triángulos individuales, sin información
 * de compartición de vértices). Hay más información sobre esto en el tipo
 * HalfLifeMDL::MDLMesh::MeshType.
 * <p>
 * Otra diferencia entre esta clase y mesh_t es que en mesh_t hay una matriz
 * de adyacencia para vértices y otra para normales´. Los elementos del tipo
 * model_t contienen uno o varios mesh_t. Cada model_t guarda un array de
 * vértices y de normales que no tienen por qué tener igual tamaño. Las mallas
 * (mesh_t) luego tienen un array de índices al array de vértices y otro de
 * índices al de normales. Ambas matrices de adyacencia tienen el mismo número
 * de elementos. Por su parte, cada mesh_t tiene las coordenadas de
 * textura de sus vértices.<br>
 * El trabajo conjunto de MDLSubmodel (representante de alto nivel de
 * model_t) y de MDLMesh (representante de mesh_t) sustituye esas dos matrices
 * de adyacencia por una sola. MDLSubmodel sigue almacenando un array de
 * vértices y otro de normales, pero ahora ambos tienen el mismo tamaño. Cuando
 * una malla (MDLMesh) en su matriz de adyacencia referencia al vértice i,
 * indica que se utiliza el vértice iésimo del MDLSubmodel que la contiene,
 * y también la normal iésima del mismo MDLSubmodel. <br>
 * Por último, también se han <em>elevado</em> las coordenadas de textura
 * a MDLSubmodel, en lugar de mantenerlas en MDLMesh. Los submodelos tienen
 * por lo tanto otro array de corodenadas de textura, con el mismo tamaño
 * que el de vértices y normales, y que es accedido por las mallas de la
 * misma forma.
 * 
 * @see HalfLifeMDL::mesh_t
 * @see HalfLifeMDL::MDLBodyPart
 * @todo Meter enlace a la información de las pieles de textura...
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 * @todo Enlace a la página general de la "arquitectura".
 */
class MDL_PUBLIC MDLMesh {
public:

	/**
	 * La clase MDLSubmodel se hace clase amiga de MDLMesh porque es
	 * MDLSubmodel quien contiene toda la información necesaria para
	 * inicializar los campos de MDLMesh. Como en realidad la información
	 * la va consiguiendo poco a poco, es más cómodo hacer a MDLSubmodel
	 * clase amiga que meter los parámetros en el constructor.
	 */
	friend MDLSubmodel;

	/**
	 * Enumerado que define constantes para especificar el modo en
	 * el que los triángulos de la malla se almacenan.
	 */
	typedef enum {
		/** Se almacenan triángulos individuales que no comparten vértices */
		MT_Triangles,
		/**
		 * La malla almacena triángulos en modo STRIP, de modo que un
		 * triángulo comparte dos vértices con el anterior, y otros dos
		 * con el siguiente.<br>
		 * En concreto, una vez pasados los dos primeros
		 * vértices, cada vértice adicional define un triángulo más. El
		 * triángulo n-ésimo con n impar, estará formado por los vértices
		 * n, n+1 y n+2 de la lista de vértices. Para n impar, estará formado
		 * por los vértices n+1, n y n+2. <br>
		 * Si hay NV vértices, se dibujarán NV-2 triángulos.<p>
		 * @note Este modo de empaquetado es el mismo que el esperado por
		 * OpenGL ante glBegin(GL_TRIANGLE_STRIP).
		 */
		MT_Strip,
		/**
		 * La malla almacena triángulos en modo FAN, de modo que todos
		 * los triángulos comparten un mismo vértice, y luego los consecutivos
		 * comparten otro más.<br>
		 * En concreto, una vez pasados los dos primeros vértices, cada
		 * vértice adicional define un triángulo más. El triángulo n-esimo
		 * se crea con los vértices 1, n+1 y n+2.<br>
		 * Si hay NV vértices, se dibujarán NV-2 triángulos.<p>
		 * @note Este modo de empaquetado es el mismo que el esperado por
		 * OpenGL ante glBegin(GL_TRIANGLE_FAN).
		 */
		MT_Fan
	} MeshType;

	/**
	 * Constructor por defecto. Pone a NULL todos los parámetros. El único
	 * modo de inicializar mallas es a través de los modos estáticos que
	 * posee la clase.
	 */
	MDLMesh() : _vertices(NULL) {}

	/**
	 * Devuelve el número de vértices de la malla. Es el número de elementos
	 * contenidos en el array devuelto por MDLMesh::getVertices.
	 */
	unsigned int getNumVertices() const { return _numVertices; }

	/**
	 * Devuelve los índices de los vértices de la malla. Los índices
	 * se refieren a los vértices, normales y coordenadas de textura
	 * almacenados por el submodelo al que pertenece la malla.
	 * @return Índices de los vértices de la malla.<br>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const unsigned int* getVertices() const {
			assert(_vertices);
			return _vertices; }

	/**
	 * Devuelve el índice del vértice solicitado de la malla. El índice
	 * se refiere a los vértices, normales y coordenadas de textura
	 * almacenados por el submodelo al que pertenece la malla.
	 * @param i Vértice solicitado.
	 * @return Vértice solicitado de la malla.
	 */
	unsigned int getVertice(unsigned int i) const {
			assert(_vertices);
			assert(i < _numVertices);
			return _vertices[i]; }

	/**
	 * Devuelve la textura de la piel utilizada por la malla.
	 * No es directamente el índice de la textura, sino el de la textura de
	 * la piel. El índice de la textura, o la textura directamente, se
	 * obtiene usando la información sobre las pieles del modelo.
	 * @return Textura de la piel de la malla.
	 * @see MDLSkins
	 */
	unsigned int getSkinTexture() const { return _skinTexture; }

	/**
	 * Devuelve el tipo de la malla (el modo en el que los triángulos que
	 * contiene son creados).
	 * @return Tipo de la malla.
	 * @see MDLMesh::MeshType
	 */
	MeshType getMeshType() const { assert(_vertices); return _type; }

protected:

	/**
	 * Tipo de la malla. Determina el modo en el que los vértices de los
	 * triángulos que contien han sido almacenados (empaquetados o no).
	 */
	MeshType _type;

	/**
	 * Número de vértices de la malla. El número de triángulos que contiene
	 * variará según el valor de MDLMesh::_type.
	 */
	unsigned int _numVertices;

	/**
	 * Vértices de la malla. Se almacena el índice del vértice dentro de
	 * los vértices del submodelo al que pertenece la malla. El mismo
	 * índice se utiliza para acceder a la normal del vértice y a sus
	 * coordenadas de textura, que son almacenadas por el submodelo
	 * al que pertenece la malla. El tamaño de este array se almacena
	 * en _numVertices.
	 * 
	 * @see HalfLifeMDL::MDLSubmodel::_vertices
	 * @see HalfLifeMDL::MDLSubmodel::_normals
	 * @see HalfLifeMDL::MDLSubmodel::_texCoord
	 * @see HalfLifeMDL::MDLMesh
	 */
	unsigned int* _vertices;

	/**
	 * Índice a la textura usada por la malla.
	 * No es directamente el índice de la textura, sino el de la textura de
	 * la piel. El índice de la textura se obtiene consultando la posición
	 * indicada por este campo en el array de índices a texturas de la
	 * piel actual.
	 * @todo En algún sitio habría que hablar de las pieles en general.
	 * Enchufar el enlace aquí. Quizá habría que cambiar el comentario
	 * según el modo en el que se implementen las pieles. :-m
	 */
	unsigned int _skinTexture;

private:

	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementación es vacía y no debe usarse.
	 */
	MDLMesh(const MDLMesh& source) {
		assert(!"Constructor copia MDLMesh no disponible"); }

	/**
	 * Operador de asignación privado para evitar problemas en los
	 * delete. Su implementación es vacía y no debe usarse.
	 */
	MDLMesh& operator=(const MDLMesh &source) {
		assert(!"Operador asignación en MDLMesh no disponible");
		return *this;}


}; // class MDLMesh

} // namespace HalfLifeMDL

#endif
