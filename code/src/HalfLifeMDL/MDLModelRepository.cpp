//---------------------------------------------------------------------------
// MDLModelRepository.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLModelRepository.cpp
 * Contiene la definici�n (implementaci�n) de la clase que almacena la base de
 * datos de todos los modelos cargados, es decir de todos los objetos
 * HalfLifeMDL::MDLCore.<p>
 * @see HalfLifeMDL::MDLModelRepository
 * @author Marco Antonio G�mez Mart�n
 * @date Noviembre, 2003.
 */

#include "MDLModelRepository.h"

#include <assert.h>
#include <memory>

#include "MDLReader.h"

using namespace std;

namespace HalfLifeMDL {

/**
 * Carga un modelo a partir del nombre del fichero. Si el
 * modelo ya ha sido previamente cargado, no se vuelve a
 * acceder al disco, sino que se devuelve la copia que
 * ya existe en memoria.
 * @param fich Nombre del fichero.
 * @return Objeto representando el modelo. Si hay alg�n
 * error en la lectura se devuelve NULL.
 */
MDLCore	*MDLModelRepository::loadModel(const std::string &fich) {
	return getSingleton().loadModelPrivate(fich);
}

/**
 * Carga un modelo a partir del nombre del fichero. Si el
 * modelo ya ha sido previamente cargado, no se vuelve a
 * acceder al disco, sino que se devuelve la copia que
 * ya existe en memoria. Esta funci�n es la equivalente
 * a la funci�n est�tica loadModel.
 * @param fich Nombre del fichero.
 * @return Objeto representando el modelo. Si hay alg�n
 * error en la lectura se devuelve NULL.
 */
MDLCore	*MDLModelRepository::loadModelPrivate(const string &fich) {

	map<string, MDLCore *>::iterator it;
	it = _models.find(fich);
	if (it != _models.end())
		return (*it).second;

	// El modelo no se ha cargado previamente, lo hacemos.
	MDLCore		*mdl;
	mdl = MDLReader::readMDL(fich);
	if (!mdl)
		return NULL;

	// Empaquetamos el modelo.
	if (!mdl->pack()) {
		// Hubo alg�n problema. Liberamos el objeto y acabamos
		// con error.
		delete mdl;
		return NULL;
	}

	_models[std::string(fich.c_str())] = mdl;
	return mdl;

}

/**
 * Constructor de la clase. S�lo se puede crear un objeto de
 * la clase (patr�n Singleton), por lo que m�s de una llamada
 * a �l fallar� (con assert). Adem�s, dada la soluci�n mixta
 * que hemos adoptado en esta implementaci�n (Singleton y
 * m�todos est�ticos), el usuario �nicamente utilizar�
 * �stos �ltimos, por lo que no puede llamar al constructor,
 * ya que de hecho, se llama al principio de la aplicaci�n
 * para crear el objeto est�tico del patr�n Singleton. El
 * m�todo se ha dejado p�blico para poder tener ese objeto
 * est�tico.
 */
MDLModelRepository::MDLModelRepository() {

	assert(!_singletonPtr && "A second MDLModelRepository shouldn't be created");
	_singletonPtr = this;
}

/**
 * Destructor de la clase. Elimina todas los objetos
 * HalfLifeMDL::MDLCore que existan en la base de datos.
 */
MDLModelRepository::~MDLModelRepository() {

	assert(_singletonPtr);
	_singletonPtr = NULL;

	map<string, MDLCore *>::iterator it = _models.begin();

	while (it != _models.end()) {
		delete (*it).second;
		++it;
	}
	_models.clear();
}



MDLModelRepository	*MDLModelRepository::_singletonPtr= NULL;
MDLModelRepository	MDLModelRepository::_singleton;

} // namespace HalfLifeMDL

