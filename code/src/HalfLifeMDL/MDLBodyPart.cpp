//---------------------------------------------------------------------------
// MDLBodyPart.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLBodyPart.cpp
 * Contiene la definición (implementación) de la clase que almacena la
 * información sobre un evento de una secuencia del modelo de Half Life.
 * @see HalfLifeMDL::MDLEvent
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#include "MDLBodyPart.h"
#include "MDLBasic.h"

namespace HalfLifeMDL {

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
MDLBodyPart::MDLBodyPart(const bodyparts_t* infoBodyPart, unsigned int offset,
					bool connectedTriangles,  bool clockwise,
					const MDLSkins* skins) :
			_name(infoBodyPart->name), _numModels(infoBodyPart->nummodels) {

	// Creamos los submodelos.
	assert(_numModels != 0 && "¿¿¿Parte del cuerpo sin recubrimientos???");

	_models = new MDLSubmodel*[_numModels];

	model_t* model = (model_t*)((MDLchar*)infoBodyPart->modelindex + offset);
	for (unsigned int c = 0; c < _numModels; ++c, ++model)
		_models[c] = new MDLSubmodel(model, offset, connectedTriangles, 
										clockwise, skins);

} // constructor

//---------------------------------------------------------------------------

/**
 * Destructor. Libera la memoria solicitada.
 */
MDLBodyPart::~MDLBodyPart() {

	if (_models != NULL) {
		for (unsigned int c = 0; c < _numModels; c++)
			delete _models[c];
		delete[] _models;
		_models = NULL;
	}

} // Destructor

//---------------------------------------------------------------------------

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
 * En realidad, se devuelve la suma de los incrementos en todas los
 * posibles recubrimientos de la parte del cuerpo. En la práctica, sólo
 * se utilizará uno cada vez, por lo que no se incrementa en ese número
 * el número de transformaciones por fotogramas si hay más de un
 * recubrimiento.
 */
int MDLBodyPart::getIncrTransformations(){ 

	int result = 0;
	if (_models != NULL) {
		for (unsigned int c = 0; c < _numModels; c++)
			result += _models[c]->getIncrTransformations();
	}
	return result;

} // getIncrTransformations

#endif

} // namespace HalfLifeMDL

