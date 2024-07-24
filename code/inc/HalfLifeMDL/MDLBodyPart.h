//---------------------------------------------------------------------------
// MDLBodyPart.h
//---------------------------------------------------------------------------

/**
 * @file MDLBodyPart.h
 * Contiene la declaración de la clase que almacena la información sobre
 * una parte del modelo de Half Life.
 * @see HalfLifeMDL::MDLBodyPart
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLBodyPart
#define __HalfLifeMDL_MDLBodyPart

#include <string>
#include <assert.h>

#include "MDLSkins.h"
#include "MDLSubmodel.h"

namespace HalfLifeMDL {

/**
 * Clase que almacena la información sobre una parte del modelo de Half
 * Life.
 * <p>
 * Cada modelo guarda los huesos y las animaciones que se les aplican.
 * Luego tiene mallas sobre esos huesos. Es posible asociar varias mallas
 * a los mismos huesos y reutilizar las animaciones, para tener modelos
 * diferentes, o versiones retocadas del mismo (por ejemplo con o sin casco).
 * <p>
 * En el ejemplo del casco anterior habría que tener información completa
 * sobre dos mallas que son prácticamente iguales (sólo cambian en la parte de
 * la cabeza por el casco). Para evitarlo, el modelo se divide en partes
 * ("partes del cuerpo") independientes, recubriendo cada una de ellas un
 * conjunto de huesos diferente. Y cada una de estas partes puede
 * proporcionar varios recubrimientos posibles. De ese modo, en el ejemplo
 * anterior, el modelo tendría dos partes, la cabeza y el resto, y sólo sería
 * la parte de la cabeza la que proporcionaría dos alternativas (con y sin
 * casco), reutilizandose en ambos casos la maya del resto del cuerpo.
 * <p>
 * Normalmente sólo habrá una parte del cuerpo, con un único posible
 * recubrimiento. Ejemplos de ficheros MDL del juego Half Life donde esto no
 * ocurre son barney.mdl (una sóla parte del cuerpo, pero con dos modelos) y
 * el modelo del jugador, con dos partes, la cabeza (con y sin casco), y
 * el resto del cuerpo.
 * <p>
 * Ésta es una clase "de alto nivel" a la que el usuario del módulo tiene
 * acceso. Está directamente relacionada con la estructura
 * HalfLifeMDL::bodyparts_t utilizada en el fichero MDL.
 * 
 * @warning Las partes del cuerpo en los ficheros MDL es completamente
 * diferente a las partes de los modelos de Quake III. En este último
 * las partes se utilizaban para facilitar la mezcla de animaciones. Aquí
 * las animaciones no cambian (están especificadas en los huesos), lo que
 * cambia son las mallas que se obtienen a partir de esos huesos.
 *
 * @see HalfLifeMDL::bodyparts_t
 * @see HalfLifeMDL::MDLSubmodel
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 * @todo Enlace a la página general de la "arquitectura".
 */
class MDLBodyPart {
public:

	/**
	 * Constructor de los objetos de la clase.
	 * @param infoBodyPart Estructura con la información de la parte del
	 * modelo tal y como se ha leído del fichero. Este puntero debe ser
	 * liberado por el usuario (se hace copia).
	 * @param offset Desplazamiento de todos los punteros contenidos
	 * en infoBodyPart. Los punteros que contiene están referidos al
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
	MDLBodyPart(const bodyparts_t* infoBodyPart, unsigned int offset,
					bool connectedTriangles, bool clockwise,
					const MDLSkins* skins);

	/**
	 * Destructor. Libera la memoria solicitada.
	 */
	~MDLBodyPart();

	/**
	 * Devuelve el nombre de la parte del cuerpo.
	 * @return Nombre de la parte del cuerpo.
	 */
	const std::string& getName() const { return _name; }

	/**
	 * Devuelve el número de recubrimientos de la parte del cuerpo.
	 * @return Número de recubrimientos.
	 */
	const unsigned int getNumModels() const { return _numModels; }

	/**
	 * Devuelve el recubrimiento solicitado.
	 * @param index Índice del recubrimiento solicitado.
	 * @return Recubrimiento.<br>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLSubmodel* getModel(unsigned int index) const {
			assert(index < _numModels);
			assert(_models);
			return _models[index]; }

#ifndef NDEBUG
	/**
	 * Devuelve la diferencia entre el número de transformaciones
	 * necesarias por cada fotograma con la unificación de las matrices
	 * de adyacencia y sin ella. En concreto, es la resta el doble del
	 * número de vértices "unificados" (el doble porque hay el mismo
	 * número de vértices que de normales) menos el número de vértices
	 * y de normales que había en el submodelo según se guardaba en el MDL.<p>
	 * Este método sólo tiene utilidad para estadísticas por curiosidad.
	 * De hecho sólo existe en la compilación en modo depuración. <p>
	 * En realidad, se devuelve la suma de los incrementos en todas los
	 * posibles recubrimientos de la parte del cuerpo. En la práctica, sólo
	 * se utilizará uno cada vez, por lo que no se incrementa en ese número
	 * el número de transformaciones por fotogramas si hay más de un
	 * recubrimiento.
	 */
	int getIncrTransformations();
#endif

protected:

	/**
	 * Nombre de la parte del cuerpo.
	 */
	std::string _name;

	/**
	 * Número de recubrimientos de esta parte del cuerpo.
	 */
	unsigned int _numModels;

	/**
	 * Recubrimientos de esta parte del cuerpo.
	 */
	MDLSubmodel** _models;

private:

	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementación es vacía y no debe usarse.
	 */
	MDLBodyPart(const MDLBodyPart& source) {
		assert(!"Constructor copia MDLBodyPart no disponible"); }

	/**
	 * Operador de asignación privado para evitar problemas en los
	 * delete. Su implementación es vacía y no debe usarse.
	 */
	MDLBodyPart& operator=(const MDLBodyPart &source) {
		assert(!"Operador asignación en MDLBodyPart no disponible");
		return *this;}

}; // class MDLBodyPart

} // namespace HalfLifeMDL

#endif
