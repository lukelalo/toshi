//---------------------------------------------------------------------------
// MDLReader.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLReader.cpp
 * Contiene la definici�n (implementaci�n) de la clase lectora de MDLs.<p>
 * En realidad el lector es sencillo debido a que la mayor parte
 * del trabajo se realiza en MDLCore.
 * @see HalfLifeMDL::MDLReader
 * @see HalfLifeMDL::MDLCore
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */

#include <fstream>

#include "MDLReader.h"

namespace HalfLifeMDL {

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
MDLCore* MDLReader::readMDL(const std::string &fileName) {

	MDLCore *model;
	char *buffer;

	// Leemos el fichero con la geometr�a.
	buffer = readFile(fileName);

	if (buffer == NULL)
		// Error en la lectura. Terminamos.
		return NULL;

	// Creamos un modelo nuevo.
	model = new MDLCore;

	// Le damos al modelo el contenido del fichero para que se
	// inicialice.
	if (!model->setMainFile(buffer)) {
		delete[] buffer;
		delete model;
		return NULL;
	}

	// Miramos si las texturas est�n en un fichero externo.
	if (model->getNumTextures() == 0) {
		// No hab�a texturas. Construimos la cadena "modelT.mdl"
		// que ser� el nombre del fichero donde se supone que
		// est�n las texturas.
		int dotPosition;
		std::string newName;
		dotPosition = fileName.find_last_of(".");
		if (dotPosition == std::string::npos)
			newName = fileName + "T";
		else
			newName = fileName.substr(0, dotPosition) + "T" +
						fileName.substr(dotPosition);

		// Leemos el nuevo fichero.
		buffer = readFile(newName);
		if (buffer) {
			// Si el fichero no se pudo leer, se supone que
			// no existe porque el modelo no tiene texturas.
			// Por tanto, s�lo hacemos la asociaci�n si
			// el fichero se ley� bien. Si no fue as�, no
			// nos preocupamos m�s.
			if (!model->setTextureFile(buffer)) {
				delete[] buffer;
				delete model;
				return NULL;
			}
		}
	} // if (model->getNumTextures() == 0)

	// Miramos si hay ficheros externos con secuencias de
	// animaci�n.
	if (model->getNumSeqGroups() > 1) {
		int dotPosition;
		dotPosition = fileName.find_last_of(".");
		for (int i = 1; i < model->getNumSeqGroups(); i++) {
			std::string newName;
			std::string twoDigits;
			twoDigits = (char)((int)'0' + (i/10)%10);
			twoDigits += (char)((int)'0' + (i%10));
			if (dotPosition == std::string::npos)
				newName = fileName + twoDigits;
			else
				newName = fileName.substr(0, dotPosition) + twoDigits +
							fileName.substr(dotPosition);
			buffer = readFile(newName);
			if (!buffer) {
				// No se ha podido leer. Este error no lo dejamos pasar.
				delete[] buffer;
				delete[] model;
				return NULL;
			}
			if (!model->setSeqGroupFile(buffer, i)) {
				delete[] buffer;
				delete model;
				return NULL;
			}
		} // for
	} // if (model->getNumSeqGroups() > 1)

	return model;

} // readMDL

//---------------------------------------------------------------------------

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
char* MDLReader::readFile(const std::string& fileName) {

	std::ifstream f;
	long fileSize;
	char* buffer;

	// Abrimos el fichero en modo binario.
	f.open(fileName.c_str(), std::ios::in | std::ios::binary);
	if (!f.is_open())
		return NULL;

	// Calculamos el tama�o del fichero.
	f.seekg(0, std::ios::end);
	fileSize = f.tellg();
	f.seekg(0, std::ios::beg);

	// Pedimos memoria para todo el archivo.
	buffer = new char[fileSize];
	if (!buffer) {
		f.close();
		return NULL;
	}

	// Lo leemos entero.
	f.read(buffer, fileSize);

	// Si hubo alg�n problema de lectura, acabamos con error.
	if (!f) {
		f.close();
		delete[] buffer;
		return NULL;
	}

	f.close();

	return buffer;

} // readFile

} // namespace HalfLifeMDL