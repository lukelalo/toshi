//---------------------------------------------------------------------------
// MDLModelRepository.h
//---------------------------------------------------------------------------

/**
 * @file MDLModelRepository.h
 * Contiene la declaración de la clase que almacena la base de datos
 * de todos los modelos cargados, es decir de todos los objetos
 * HalfLifeMDL::MDLCore.<p>
 * @see HalfLifeMDL::MDLModelRepository
 * @author Marco Antonio Gómez Martín
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLModelRepository
#define __HalfLifeMDL_MDLModelRepository

#ifdef _WIN32
#pragma warning( disable : 4786 )
#endif

#include <string>

#include <map>
#include "MDLCore.h"

namespace HalfLifeMDL {

/**
 * Clase que almacena una base de datos con todos los modelos
 * cargados del Half Life. De esta forma, cuando la aplicación
 * quiere cargar un fichero, no se tiene que preocupar de mirar
 * si ya se ha hecho y está en memoria o no.<p>
 * Solo existirá una base de datos en toda la aplicación. Por esta
 * razón, hemos creado métodos estáticos para acceder a ella. Sin embargo,
 * internamente esas funciones estáticas llaman a los métodos de
 * objeto del Singleton. Se ha optado por esta alternativa "mixta"
 * (en vez de las puras de todo estático o Singleton), por comodidad
 * a la hora de utilizarlo, y aún así, tener un método destructor,
 * que elimine todos los MDLCore cargados al final de la aplicación.
 * @author Marco Antonio Gómez Martín
 * @date Noviembre, 2003
 */
class MDL_PUBLIC MDLModelRepository {
public:

	/**
	 * Carga un modelo a partir del nombre del fichero. Si el
	 * modelo ya ha sido previamente cargado, no se vuelve a
	 * acceder al disco, sino que se devuelve la copia que
	 * ya existe en memoria.
	 * @param fich Nombre del fichero.
	 * @return Objeto representando el modelo. Si hay algún
	 * error en la lectura se devuelve NULL.
	 */
	static MDLCore	*loadModel(const std::string &fich);

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
	MDLModelRepository();

	/**
	 * Destructor de la clase. Elimina todas los objetos
	 * HalfLifeMDL::MDLCore que existan en la base de datos.
	 */
	~MDLModelRepository();


protected:

	/**
	 * Tabla hash que relaciona el nombre de un fichero en disco,
	 * con el modelo MDLCore cargado. Se va rellenando cuando se
	 * van cargando los ficheros.
	 */
	std::map<std::string, MDLCore *>	_models;

	/**
	 * Puntero al único objeto creado de la clase (patrón Singleton).
	 */
	static MDLModelRepository	*_singletonPtr;

	/**
	 * Único objeto creado de la clase, al principio de la aplicación.
	 * Es apuntado por el puntero _singletonPtr.
	 */
	static MDLModelRepository	_singleton;

	/**
	 * Función para el acceso al único objeto creado de la clase.
	 * @return Objeto de la clase.
	 */
	static MDLModelRepository	&getSingleton() {return _singleton;}

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
	MDLCore	*loadModelPrivate(const std::string &fich);
};

} // namespace HalfLifeMDL

#endif
