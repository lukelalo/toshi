//---------------------------------------------------------------------------
// MDLSkins.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLSkins.cpp
 * Contiene la definici�n (implementaci�n) de la clase que almacena la
 * informaci�n sobre las pieles (versiones de texturas) del modelo de Half
 * Life.
 * @see HalfLifeMDL::MDLSkins
 * @author Pedro Pablo G�mez Mart�n
 * @date Diciembre, 2003.
 */

#include <assert.h>
#include <memory>

#include "MDLSkins.h"
#include "MDLBasicType.h"

namespace HalfLifeMDL {

/**
 * Constructor. Crea un objeto de la clase a partir de la informaci�n
 * proporcionada.
 * @param numSkins N�mero de pieles del modelo.
 * @param numTexturesPerSkin N�mero de texturas en cada piel.
 * @param skins Informaci�n de las pieles tal y como se almacenan en
 * el fichero. Se leer�n numSkins*numTexturesPerSkins valores. Se hace
 * copia del contenido, por lo que el puntero puede ser liberado por
 * el cliente.
 * @param textures Array con las texturas del modelo ya inicializadas.
 * No se hace copia, por lo que las texturas no deben liberarse mientras
 * no se haya destruido el objeto. Como es l�gico, las texturas no se
 * destruyen en el destructor de este objeto. <br>
 * Las texturas deben estar en el array en el mismo orden que en
 * el fichero.
 * @note Todas las texturas colocadas en la posici�n i de todas las pieles
 * deben tener las mismas dimensiones. En el c�digo del constructor se
 * comprueba este requisito s�lo si la compilaci�n se ha realizado en modo
 * depuraci�n, y el error se genera con assert.
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
									// la conversi�n de tipos autom�ticamente.

	// Analizamos las pieles para asegurarnos de que las texturas que aparecen
	// en el mismo �ndice en todas las pieles tienen las mismas dimensiones.
	// Esta comprobaci�n s�lo la hacemos en modo depuraci�n.
#ifndef NDEBUG
	for (t = 0; t < numTexturesPerSkin; t++) {
		unsigned int w, h;
		w = getTexture(0, t)->getWidth();
		h = getTexture(0, t)->getHeight();
		for (s = 1; s < _numSkins; s++) {
			if ((w != getTexture(s, t)->getWidth()) ||
				(h != getTexture(s, t)->getHeight())) 
				assert(!"Las texturas en el mismo �ndice de diferentes pieles"
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

