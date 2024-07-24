//---------------------------------------------------------------------------
// MDLSkins.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLSkins.cpp
 * Contiene la definición (implementación) de la clase que almacena la
 * información sobre las pieles (versiones de texturas) del modelo de Half
 * Life.
 * @see HalfLifeMDL::MDLSkins
 * @author Pedro Pablo Gómez Martín
 * @date Diciembre, 2003.
 */

#include <assert.h>
#include <memory>

#include "MDLSkins.h"
#include "MDLBasicType.h"

namespace HalfLifeMDL {

/**
 * Constructor. Crea un objeto de la clase a partir de la información
 * proporcionada.
 * @param numSkins Número de pieles del modelo.
 * @param numTexturesPerSkin Número de texturas en cada piel.
 * @param skins Información de las pieles tal y como se almacenan en
 * el fichero. Se leerán numSkins*numTexturesPerSkins valores. Se hace
 * copia del contenido, por lo que el puntero puede ser liberado por
 * el cliente.
 * @param textures Array con las texturas del modelo ya inicializadas.
 * No se hace copia, por lo que las texturas no deben liberarse mientras
 * no se haya destruido el objeto. Como es lógico, las texturas no se
 * destruyen en el destructor de este objeto. <br>
 * Las texturas deben estar en el array en el mismo orden que en
 * el fichero.
 * @note Todas las texturas colocadas en la posición i de todas las pieles
 * deben tener las mismas dimensiones. En el código del constructor se
 * comprueba este requisito sólo si la compilación se ha realizado en modo
 * depuración, y el error se genera con assert.
 */
MDLSkins::MDLSkins(unsigned int numSkins, unsigned int numTexturesPerSkin,
				   MDLushort* skins, const MDLTexture** textures) {

	unsigned int t, s, i;

#ifndef NDEBUG
	if (AllowModelsWithoutTextures == 0) {
		assert(skins);
		assert(textures);
	}
#endif

	_textures = textures;
	if (numSkins * numTexturesPerSkin > 0)
		_skins = new unsigned int[numSkins*numTexturesPerSkin];
	_numSkins = numSkins;
	_numTexturesPerSkin = numTexturesPerSkin;

	for (s = 0, i = 0; s < _numSkins; s++)
		for (t = 0; t < _numTexturesPerSkin; t++, i++)
			_skins[i] = skins[i];	// No usamos memcpy para que se haga
									// la conversión de tipos automáticamente.

	// Analizamos las pieles para asegurarnos de que las texturas que aparecen
	// en el mismo índice en todas las pieles tienen las mismas dimensiones.
	// Esta comprobación sólo la hacemos en modo depuración.
#ifndef NDEBUG
	for (t = 0; t < numTexturesPerSkin; t++) {
		unsigned int w, h;
		w = getTexture(0, t)->getWidth();
		h = getTexture(0, t)->getHeight();
		for (s = 1; s < _numSkins; s++) {
			if ((w != getTexture(s, t)->getWidth()) ||
				(h != getTexture(s, t)->getHeight())) 
				assert(!"Las texturas en el mismo índice de diferentes pieles"
						" deben tener iguales coordenadas.");
		}
	}
#endif

} // constructor

//---------------------------------------------------------------------------

/**
 * Destructor. Libera la memoria del objeto.
 */
MDLSkins::~MDLSkins() {

	if (_skins != NULL)
		delete[] _skins;

} // destructor

} // namespace HalfLifeMDL

