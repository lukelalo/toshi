//---------------------------------------------------------------------------
// MDLReader.h
//---------------------------------------------------------------------------

/**
 * @file MDLReader.h
 * Contiene la declaraci�n de la clase lectora de MDLs. <p>
 * En realidad el lector es sencillo debido a que la mayor parte
 * del trabajo se realiza en MDLCore.
 * @see HalfLifeMDL::MDLReader
 * @see HalfLifeMDL::MDLCore
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */

#ifndef __HalfLifeMDL_MDLReader
#define __HalfLifeMDL_MDLReader

#include <string>
#include "MDLCore.h"

namespace HalfLifeMDL {

/**
 * Clase con m�todos est�ticos para leer modelos del Half Life y
 * obtener objetos que almacenan su informaci�n est�tica.<p>
 * Por informaci�n est�tica se entiende el n�mero de animaciones,
 * huesos, texturas, etc. que tiene el modelo, y su informaci�n
 * asociada. Estos objetos son la base para poder crear posteriormente
 * objetos que almacenan la informaci�n din�mica de un modelo (fotograma
 * actual, animaci�n en ejecuci�n, etc.). De ese modo se pueden crear
 * varias "instancias" del mismo modelo.<p>
 * En realidad el lector es sencillo debido a que la mayor parte
 * del trabajo se realiza en MDLCore.
 * @see HalfLifeMDL::MDLInstance
 * @see HalfLifeMDL::MDLCore
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
class MDL_PUBLIC MDLReader {
public:

	/**
	 * Lee un modelo a partir del nombre del fichero especificado.
	 * Se utiliza el "convenio de nombres" usado por Half Life para
	 * buscar informaci�n externa en archivos diferentes. M�s concretamente
	 * si el fichero especificado (supongamos de nombre
	 * <code>model.mdl</code>) no tiene texturas, autom�ticamente se buscar�n
	 * en el fichero <code>modelT.mdl</code>.<p>
	 * De forma similar, si el modelo tiene secuencias de animaci�n externas
	 * al fichero de la geometr�a, se buscar�n en los ficheros
	 * <code>model01.mdl</code>, <code>model02.mdl</code>, etc, usando tantos
	 * archivos adicionales como grupos de secuencia externa se especifiquen
	 * en el fichero original.
	 * @param fileName Nombre del fichero de donde obtener el modelo.
	 * @return Objeto representando el modelo. Si hay alg�n problema en la
	 * lectura se devuelve NULL.
	 * @warning Si el fichero base indica que no tiene texturas, y al
	 * intentar leer el fichero <code>modelT.mdl</code> se encuentra
	 * cualquier error (lectura incorrecta o fichero inexistente), se
	 * supondr� que el modelo no utiliza texturas y no se avisar� de
	 * ning�n error. Si el modelo realmente necesita esas texturas y el
	 * problema fue de lectura, tal vez aparezcan problemas posteriormente
	 * (seg�n c�mo utilice el modelo la aplicaci�n).<br>
	 * Sin embargo si se produce alg�n error al intentar leer un fichero
	 * con secuencias externas, s� se notificar� error.
	 */
	static MDLCore* readMDL(const std::string &fileName);

protected:

	/**
	 * Lee el contenido del fichero de nombre especificado y devuelve
	 * su contenido completo. Para eso, obtiene el tama�o del fichero
	 * pide memoria (con new char[tama�o]), y lee todo el fichero en
	 * ese array, cuyo puntero devuelve.
	 * @param fileName Nombre del fichero a leer.
	 * @return Puntero a la zona de memoria donde se ha le�do el fichero.
	 * Es responsabilidad del usuario liberar la memoria (con delete[]).
	 * Si el fichero no puede ser le�do, devuelve NULL.
	 */
	static char* readFile(const std::string& fileName);

}; // class MDLReader

} // namespace HalfLifeMDL

#endif
