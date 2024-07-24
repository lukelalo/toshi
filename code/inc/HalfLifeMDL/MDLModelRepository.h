//---------------------------------------------------------------------------
// MDLModelRepository.h
//---------------------------------------------------------------------------

/**
 * @file MDLModelRepository.h
 * Contiene la declaraci�n de la clase que almacena la base de datos
 * de todos los modelos cargados, es decir de todos los objetos
 * HalfLifeMDL::MDLCore.<p>
 * @see HalfLifeMDL::MDLModelRepository
 * @author Marco Antonio G�mez Mart�n
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
 * cargados del Half Life. De esta forma, cuando la aplicaci�n
 * quiere cargar un fichero, no se tiene que preocupar de mirar
 * si ya se ha hecho y est� en memoria o no.<p>
 * Solo existir� una base de datos en toda la aplicaci�n. Por esta
 * raz�n, hemos creado m�todos est�ticos para acceder a ella. Sin embargo,
 * internamente esas funciones est�ticas llaman a los m�todos de
 * objeto del Singleton. Se ha optado por esta alternativa "mixta"
 * (en vez de las puras de todo est�tico o Singleton), por comodidad
 * a la hora de utilizarlo, y a�n as�, tener un m�todo destructor,
 * que elimine todos los MDLCore cargados al final de la aplicaci�n.
 * @author Marco Antonio G�mez Mart�n
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
	 * @return Objeto representando el modelo. Si hay alg�n
	 * error en la lectura se devuelve NULL.
	 */
	static MDLCore	*loadModel(const std::string &fich);

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
	 * Puntero al �nico objeto creado de la clase (patr�n Singleton).
	 */
	static MDLModelRepository	*_singletonPtr;

	/**
	 * �nico objeto creado de la clase, al principio de la aplicaci�n.
	 * Es apuntado por el puntero _singletonPtr.
	 */
	static MDLModelRepository	_singleton;

	/**
	 * Funci�n para el acceso al �nico objeto creado de la clase.
	 * @return Objeto de la clase.
	 */
	static MDLModelRepository	&getSingleton() {return _singleton;}

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
	MDLCore	*loadModelPrivate(const std::string &fich);
};

} // namespace HalfLifeMDL

#endif
