//---------------------------------------------------------------------------
// MDLModelRepository.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLModelRepository.cpp
 * Contiene la definición (implementación) de la clase que almacena la base de
 * datos de todos los modelos cargados, es decir de todos los objetos
 * HalfLifeMDL::MDLCore.<p>
 * @see HalfLifeMDL::MDLModelRepository
 * @author Marco Antonio Gómez Martín
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
 * @return Objeto representando el modelo. Si hay algún
 * error en la lectura se devuelve NULL.
 */
MDLCore	*MDLModelRepository::loadModel(const std::string &fich) {
	return getSingleton().loadModelPrivate(fich);
}

/**
 * Carga un modelo a partir del nombre del fichero. Si el
 * modelo ya ha sido previamente cargado, no se vuelve a
 * acceder al disco, sino que se devuelve la copia que
 * ya existe en memoria. Esta función es la equivalente
 * a la función estática loadModel.
 * @param fich Nombre del fichero.
 * @return Objeto representando el modelo. Si hay algún
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
		// Hubo algún problema. Liberamos el objeto y acabamos
		// con error.
		delete mdl;
		return NULL;
	}

	_models[std::string(fich.c_str())] = mdl;
	return mdl;

}

/**
 * Constructor de la clase. Sólo se puede crear un objeto de
 * la clase (patrón Singleton), por lo que más de una llamada
 * a él fallará (con assert). Además, dada la solución mixta
 * que hemos adoptado en esta implementación (Singleton y
 * métodos estáticos), el usuario únicamente utilizará
 * éstos últimos, por lo que no puede llamar al constructor,
 * ya que de hecho, se llama al principio de la aplicación
 * para crear el objeto estático del patrón Singleton. El
 * método se ha dejado público para poder tener ese objeto
 * estático.
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

