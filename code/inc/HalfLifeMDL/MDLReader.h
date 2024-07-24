//---------------------------------------------------------------------------
// MDLReader.h
//---------------------------------------------------------------------------

/**
 * @file MDLReader.h
 * Contiene la declaración de la clase lectora de MDLs. <p>
 * En realidad el lector es sencillo debido a que la mayor parte
 * del trabajo se realiza en MDLCore.
 * @see HalfLifeMDL::MDLReader
 * @see HalfLifeMDL::MDLCore
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */

#ifndef __HalfLifeMDL_MDLReader
#define __HalfLifeMDL_MDLReader

#include <string>
#include "MDLCore.h"

namespace HalfLifeMDL {

/**
 * Clase con métodos estáticos para leer modelos del Half Life y
 * obtener objetos que almacenan su información estática.<p>
 * Por información estática se entiende el número de animaciones,
 * huesos, texturas, etc. que tiene el modelo, y su información
 * asociada. Estos objetos son la base para poder crear posteriormente
 * objetos que almacenan la información dinámica de un modelo (fotograma
 * actual, animación en ejecución, etc.). De ese modo se pueden crear
 * varias "instancias" del mismo modelo.<p>
 * En realidad el lector es sencillo debido a que la mayor parte
 * del trabajo se realiza en MDLCore.
 * @see HalfLifeMDL::MDLInstance
 * @see HalfLifeMDL::MDLCore
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
class MDL_PUBLIC MDLReader {
public:

	/**
	 * Lee un modelo a partir del nombre del fichero especificado.
	 * Se utiliza el "convenio de nombres" usado por Half Life para
	 * buscar información externa en archivos diferentes. Más concretamente
	 * si el fichero especificado (supongamos de nombre
	 * <code>model.mdl</code>) no tiene texturas, automáticamente se buscarán
	 * en el fichero <code>modelT.mdl</code>.<p>
	 * De forma similar, si el modelo tiene secuencias de animación externas
	 * al fichero de la geometría, se buscarán en los ficheros
	 * <code>model01.mdl</code>, <code>model02.mdl</code>, etc, usando tantos
	 * archivos adicionales como grupos de secuencia externa se especifiquen
	 * en el fichero original.
	 * @param fileName Nombre del fichero de donde obtener el modelo.
	 * @return Objeto representando el modelo. Si hay algún problema en la
	 * lectura se devuelve NULL.
	 * @warning Si el fichero base indica que no tiene texturas, y al
	 * intentar leer el fichero <code>modelT.mdl</code> se encuentra
	 * cualquier error (lectura incorrecta o fichero inexistente), se
	 * supondrá que el modelo no utiliza texturas y no se avisará de
	 * ningún error. Si el modelo realmente necesita esas texturas y el
	 * problema fue de lectura, tal vez aparezcan problemas posteriormente
	 * (según cómo utilice el modelo la aplicación).<br>
	 * Sin embargo si se produce algún error al intentar leer un fichero
	 * con secuencias externas, sí se notificará error.
	 */
	static MDLCore* readMDL(const std::string &fileName);

protected:

	/**
	 * Lee el contenido del fichero de nombre especificado y devuelve
	 * su contenido completo. Para eso, obtiene el tamaño del fichero
	 * pide memoria (con new char[tamaño]), y lee todo el fichero en
	 * ese array, cuyo puntero devuelve.
	 * @param fileName Nombre del fichero a leer.
	 * @return Puntero a la zona de memoria donde se ha leído el fichero.
	 * Es responsabilidad del usuario liberar la memoria (con delete[]).
	 * Si el fichero no puede ser leído, devuelve NULL.
	 */
	static char* readFile(const std::string& fileName);

}; // class MDLReader

} // namespace HalfLifeMDL

#endif
