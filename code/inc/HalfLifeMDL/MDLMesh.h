//---------------------------------------------------------------------------
// MDLMesh.h
//---------------------------------------------------------------------------

/**
 * @file MDLMesh.h
 * Contiene la declaraci�n de la clase que almacena la informaci�n sobre
 * una malla de un modelo de Half Life.
 * @see HalfLifeMDL::MDLMesh
 * @author Pedro Pablo G�mez Mart�n
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
 * Clase que almacena la informaci�n sobre una malla del modelo de Half Life.
 * Un objeto de esta clase no tiene ning�n sentido tratado de forma aislada.
 * La informaci�n que contiene se refiere a los datos almacenados por el
 * submodelo que lo contiene (HalfLifeMDL::MDLSubmodel) en el caso de los
 * v�rtices y las normales, y a los datos del modelo completo al que pertenece
 * ese submodelo en el caso de las texturas.
 * <p>
 * �sta es una clase "de alto nivel" a la que el usuario del m�dulo tiene
 * acceso. Est� directamente relacionada con la estructura
 * HalfLifeMDL::mesh_t utilizada en el fichero MDL, aunque no siempre la
 * relaci�n es uno a uno. Una estructura mesh_t contiene toda la informaci�n
 * de una malla, que puede estar formada por varios bloques de tri�ngulos,
 * unos representados en modo FAN (todos comparten un v�rtices), y otros
 * en modo STRIP (el tri�ngulo n comparte dos v�rtices con el n-1). Las
 * mallas contenidas en esta clase s�lo contienen uno de esos bloques si
 * el usuario del m�dulo ha solicitado empaquetar el modelo manteniendo
 * los bloques (vease HalfLifeMDL::MDLCore::pack), o a toda la malla en
 * caso contrario (y se almacenar�n tri�ngulos individuales, sin informaci�n
 * de compartici�n de v�rtices). Hay m�s informaci�n sobre esto en el tipo
 * HalfLifeMDL::MDLMesh::MeshType.
 * <p>
 * Otra diferencia entre esta clase y mesh_t es que en mesh_t hay una matriz
 * de adyacencia para v�rtices y otra para normales�. Los elementos del tipo
 * model_t contienen uno o varios mesh_t. Cada model_t guarda un array de
 * v�rtices y de normales que no tienen por qu� tener igual tama�o. Las mallas
 * (mesh_t) luego tienen un array de �ndices al array de v�rtices y otro de
 * �ndices al de normales. Ambas matrices de adyacencia tienen el mismo n�mero
 * de elementos. Por su parte, cada mesh_t tiene las coordenadas de
 * textura de sus v�rtices.<br>
 * El trabajo conjunto de MDLSubmodel (representante de alto nivel de
 * model_t) y de MDLMesh (representante de mesh_t) sustituye esas dos matrices
 * de adyacencia por una sola. MDLSubmodel sigue almacenando un array de
 * v�rtices y otro de normales, pero ahora ambos tienen el mismo tama�o. Cuando
 * una malla (MDLMesh) en su matriz de adyacencia referencia al v�rtice i,
 * indica que se utiliza el v�rtice i�simo del MDLSubmodel que la contiene,
 * y tambi�n la normal i�sima del mismo MDLSubmodel. <br>
 * Por �ltimo, tambi�n se han <em>elevado</em> las coordenadas de textura
 * a MDLSubmodel, en lugar de mantenerlas en MDLMesh. Los submodelos tienen
 * por lo tanto otro array de corodenadas de textura, con el mismo tama�o
 * que el de v�rtices y normales, y que es accedido por las mallas de la
 * misma forma.
 * 
 * @see HalfLifeMDL::mesh_t
 * @see HalfLifeMDL::MDLBodyPart
 * @todo Meter enlace a la informaci�n de las pieles de textura...
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 * @todo Enlace a la p�gina general de la "arquitectura".
 */
class MDL_PUBLIC MDLMesh {
public:

	/**
	 * La clase MDLSubmodel se hace clase amiga de MDLMesh porque es
	 * MDLSubmodel quien contiene toda la informaci�n necesaria para
	 * inicializar los campos de MDLMesh. Como en realidad la informaci�n
	 * la va consiguiendo poco a poco, es m�s c�modo hacer a MDLSubmodel
	 * clase amiga que meter los par�metros en el constructor.
	 */
	friend MDLSubmodel;

	/**
	 * Enumerado que define constantes para especificar el modo en
	 * el que los tri�ngulos de la malla se almacenan.
	 */
	typedef enum {
		/** Se almacenan tri�ngulos individuales que no comparten v�rtices */
		MT_Triangles,
		/**
		 * La malla almacena tri�ngulos en modo STRIP, de modo que un
		 * tri�ngulo comparte dos v�rtices con el anterior, y otros dos
		 * con el siguiente.<br>
		 * En concreto, una vez pasados los dos primeros
		 * v�rtices, cada v�rtice adicional define un tri�ngulo m�s. El
		 * tri�ngulo n-�simo con n impar, estar� formado por los v�rtices
		 * n, n+1 y n+2 de la lista de v�rtices. Para n impar, estar� formado
		 * por los v�rtices n+1, n y n+2. <br>
		 * Si hay NV v�rtices, se dibujar�n NV-2 tri�ngulos.<p>
		 * @note Este modo de empaquetado es el mismo que el esperado por
		 * OpenGL ante glBegin(GL_TRIANGLE_STRIP).
		 */
		MT_Strip,
		/**
		 * La malla almacena tri�ngulos en modo FAN, de modo que todos
		 * los tri�ngulos comparten un mismo v�rtice, y luego los consecutivos
		 * comparten otro m�s.<br>
		 * En concreto, una vez pasados los dos primeros v�rtices, cada
		 * v�rtice adicional define un tri�ngulo m�s. El tri�ngulo n-esimo
		 * se crea con los v�rtices 1, n+1 y n+2.<br>
		 * Si hay NV v�rtices, se dibujar�n NV-2 tri�ngulos.<p>
		 * @note Este modo de empaquetado es el mismo que el esperado por
		 * OpenGL ante glBegin(GL_TRIANGLE_FAN).
		 */
		MT_Fan
	} MeshType;

	/**
	 * Constructor por defecto. Pone a NULL todos los par�metros. El �nico
	 * modo de inicializar mallas es a trav�s de los modos est�ticos que
	 * posee la clase.
	 */
	MDLMesh() : _vertices(NULL) {}

	/**
	 * Devuelve el n�mero de v�rtices de la malla. Es el n�mero de elementos
	 * contenidos en el array devuelto por MDLMesh::getVertices.
	 */
	unsigned int getNumVertices() const { return _numVertices; }

	/**
	 * Devuelve los �ndices de los v�rtices de la malla. Los �ndices
	 * se refieren a los v�rtices, normales y coordenadas de textura
	 * almacenados por el submodelo al que pertenece la malla.
	 * @return �ndices de los v�rtices de la malla.<br>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const unsigned int* getVertices() const {
			assert(_vertices);
			return _vertices; }

	/**
	 * Devuelve el �ndice del v�rtice solicitado de la malla. El �ndice
	 * se refiere a los v�rtices, normales y coordenadas de textura
	 * almacenados por el submodelo al que pertenece la malla.
	 * @param i V�rtice solicitado.
	 * @return V�rtice solicitado de la malla.
	 */
	unsigned int getVertice(unsigned int i) const {
			assert(_vertices);
			assert(i < _numVertices);
			return _vertices[i]; }

	/**
	 * Devuelve la textura de la piel utilizada por la malla.
	 * No es directamente el �ndice de la textura, sino el de la textura de
	 * la piel. El �ndice de la textura, o la textura directamente, se
	 * obtiene usando la informaci�n sobre las pieles del modelo.
	 * @return Textura de la piel de la malla.
	 * @see MDLSkins
	 */
	unsigned int getSkinTexture() const { return _skinTexture; }

	/**
	 * Devuelve el tipo de la malla (el modo en el que los tri�ngulos que
	 * contiene son creados).
	 * @return Tipo de la malla.
	 * @see MDLMesh::MeshType
	 */
	MeshType getMeshType() const { assert(_vertices); return _type; }

protected:

	/**
	 * Tipo de la malla. Determina el modo en el que los v�rtices de los
	 * tri�ngulos que contien han sido almacenados (empaquetados o no).
	 */
	MeshType _type;

	/**
	 * N�mero de v�rtices de la malla. El n�mero de tri�ngulos que contiene
	 * variar� seg�n el valor de MDLMesh::_type.
	 */
	unsigned int _numVertices;

	/**
	 * V�rtices de la malla. Se almacena el �ndice del v�rtice dentro de
	 * los v�rtices del submodelo al que pertenece la malla. El mismo
	 * �ndice se utiliza para acceder a la normal del v�rtice y a sus
	 * coordenadas de textura, que son almacenadas por el submodelo
	 * al que pertenece la malla. El tama�o de este array se almacena
	 * en _numVertices.
	 * 
	 * @see HalfLifeMDL::MDLSubmodel::_vertices
	 * @see HalfLifeMDL::MDLSubmodel::_normals
	 * @see HalfLifeMDL::MDLSubmodel::_texCoord
	 * @see HalfLifeMDL::MDLMesh
	 */
	unsigned int* _vertices;

	/**
	 * �ndice a la textura usada por la malla.
	 * No es directamente el �ndice de la textura, sino el de la textura de
	 * la piel. El �ndice de la textura se obtiene consultando la posici�n
	 * indicada por este campo en el array de �ndices a texturas de la
	 * piel actual.
	 * @todo En alg�n sitio habr�a que hablar de las pieles en general.
	 * Enchufar el enlace aqu�. Quiz� habr�a que cambiar el comentario
	 * seg�n el modo en el que se implementen las pieles. :-m
	 */
	unsigned int _skinTexture;

private:

	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLMesh(const MDLMesh& source) {
		assert(!"Constructor copia MDLMesh no disponible"); }

	/**
	 * Operador de asignaci�n privado para evitar problemas en los
	 * delete. Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLMesh& operator=(const MDLMesh &source) {
		assert(!"Operador asignaci�n en MDLMesh no disponible");
		return *this;}


}; // class MDLMesh

} // namespace HalfLifeMDL

#endif
