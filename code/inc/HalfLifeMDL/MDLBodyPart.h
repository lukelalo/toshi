//---------------------------------------------------------------------------
// MDLBodyPart.h
//---------------------------------------------------------------------------

/**
 * @file MDLBodyPart.h
 * Contiene la declaraci�n de la clase que almacena la informaci�n sobre
 * una parte del modelo de Half Life.
 * @see HalfLifeMDL::MDLBodyPart
 * @author Pedro Pablo G�mez Mart�n
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
 * Clase que almacena la informaci�n sobre una parte del modelo de Half
 * Life.
 * <p>
 * Cada modelo guarda los huesos y las animaciones que se les aplican.
 * Luego tiene mallas sobre esos huesos. Es posible asociar varias mallas
 * a los mismos huesos y reutilizar las animaciones, para tener modelos
 * diferentes, o versiones retocadas del mismo (por ejemplo con o sin casco).
 * <p>
 * En el ejemplo del casco anterior habr�a que tener informaci�n completa
 * sobre dos mallas que son pr�cticamente iguales (s�lo cambian en la parte de
 * la cabeza por el casco). Para evitarlo, el modelo se divide en partes
 * ("partes del cuerpo") independientes, recubriendo cada una de ellas un
 * conjunto de huesos diferente. Y cada una de estas partes puede
 * proporcionar varios recubrimientos posibles. De ese modo, en el ejemplo
 * anterior, el modelo tendr�a dos partes, la cabeza y el resto, y s�lo ser�a
 * la parte de la cabeza la que proporcionar�a dos alternativas (con y sin
 * casco), reutilizandose en ambos casos la maya del resto del cuerpo.
 * <p>
 * Normalmente s�lo habr� una parte del cuerpo, con un �nico posible
 * recubrimiento. Ejemplos de ficheros MDL del juego Half Life donde esto no
 * ocurre son barney.mdl (una s�la parte del cuerpo, pero con dos modelos) y
 * el modelo del jugador, con dos partes, la cabeza (con y sin casco), y
 * el resto del cuerpo.
 * <p>
 * �sta es una clase "de alto nivel" a la que el usuario del m�dulo tiene
 * acceso. Est� directamente relacionada con la estructura
 * HalfLifeMDL::bodyparts_t utilizada en el fichero MDL.
 * 
 * @warning Las partes del cuerpo en los ficheros MDL es completamente
 * diferente a las partes de los modelos de Quake III. En este �ltimo
 * las partes se utilizaban para facilitar la mezcla de animaciones. Aqu�
 * las animaciones no cambian (est�n especificadas en los huesos), lo que
 * cambia son las mallas que se obtienen a partir de esos huesos.
 *
 * @see HalfLifeMDL::bodyparts_t
 * @see HalfLifeMDL::MDLSubmodel
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 * @todo Enlace a la p�gina general de la "arquitectura".
 */
class MDLBodyPart {
public:

	/**
	 * Constructor de los objetos de la clase.
	 * @param infoBodyPart Estructura con la informaci�n de la parte del
	 * modelo tal y como se ha le�do del fichero. Este puntero debe ser
	 * liberado por el usuario (se hace copia).
	 * @param offset Desplazamiento de todos los punteros contenidos
	 * en infoBodyPart. Los punteros que contiene est�n referidos al
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
	 * Devuelve el n�mero de recubrimientos de la parte del cuerpo.
	 * @return N�mero de recubrimientos.
	 */
	const unsigned int getNumModels() const { return _numModels; }

	/**
	 * Devuelve el recubrimiento solicitado.
	 * @param index �ndice del recubrimiento solicitado.
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
	 * Devuelve la diferencia entre el n�mero de transformaciones
	 * necesarias por cada fotograma con la unificaci�n de las matrices
	 * de adyacencia y sin ella. En concreto, es la resta el doble del
	 * n�mero de v�rtices "unificados" (el doble porque hay el mismo
	 * n�mero de v�rtices que de normales) menos el n�mero de v�rtices
	 * y de normales que hab�a en el submodelo seg�n se guardaba en el MDL.<p>
	 * Este m�todo s�lo tiene utilidad para estad�sticas por curiosidad.
	 * De hecho s�lo existe en la compilaci�n en modo depuraci�n. <p>
	 * En realidad, se devuelve la suma de los incrementos en todas los
	 * posibles recubrimientos de la parte del cuerpo. En la pr�ctica, s�lo
	 * se utilizar� uno cada vez, por lo que no se incrementa en ese n�mero
	 * el n�mero de transformaciones por fotogramas si hay m�s de un
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
	 * N�mero de recubrimientos de esta parte del cuerpo.
	 */
	unsigned int _numModels;

	/**
	 * Recubrimientos de esta parte del cuerpo.
	 */
	MDLSubmodel** _models;

private:

	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLBodyPart(const MDLBodyPart& source) {
		assert(!"Constructor copia MDLBodyPart no disponible"); }

	/**
	 * Operador de asignaci�n privado para evitar problemas en los
	 * delete. Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLBodyPart& operator=(const MDLBodyPart &source) {
		assert(!"Operador asignaci�n en MDLBodyPart no disponible");
		return *this;}

}; // class MDLBodyPart

} // namespace HalfLifeMDL

#endif
