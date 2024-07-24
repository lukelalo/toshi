//---------------------------------------------------------------------------
// MDLBodyPart.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLBodyPart.cpp
 * Contiene la definici�n (implementaci�n) de la clase que almacena la
 * informaci�n sobre un evento de una secuencia del modelo de Half Life.
 * @see HalfLifeMDL::MDLEvent
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 */

#include "MDLBodyPart.h"
#include "MDLBasic.h"

namespace HalfLifeMDL {

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
MDLBodyPart::MDLBodyPart(const bodyparts_t* infoBodyPart, unsigned int offset,
					bool connectedTriangles,  bool clockwise,
					const MDLSkins* skins) :
			_name(infoBodyPart->name), _numModels(infoBodyPart->nummodels) {

	// Creamos los submodelos.
	assert(_numModels != 0 && "���Parte del cuerpo sin recubrimientos???");

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
 * Devuelve la diferencia entre el n�mero de transformaciones
 * necesarias por cada fotograma con la unificaci�n de las matrices
 * de adyacencia y sin ella. En concreto, es la resta el doble del
 * n�mero de v�rtices "unificados" (el doble porque hay el mismo
 * n�mero de v�rtices que de normales) menos el n�mero de v�rtices
 * y de normales que hab�a en el submodelo seg�n se guardaba en el MDL.<p>
 * Este m�todo s�lo tiene utilidad para estad�sticas por curiosidad.
 * De hecho s�lo existe en la compilaci�n en modo depuraci�n.
 * En realidad, se devuelve la suma de los incrementos en todas los
 * posibles recubrimientos de la parte del cuerpo. En la pr�ctica, s�lo
 * se utilizar� uno cada vez, por lo que no se incrementa en ese n�mero
 * el n�mero de transformaciones por fotogramas si hay m�s de un
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

