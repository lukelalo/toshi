//---------------------------------------------------------------------------
// MDLSkins.h
//---------------------------------------------------------------------------

/**
 * @file MDLSkins.h
 * Contiene la declaración de la clase que almacena la información sobre
 * las pieles (versiones de texturas) del modelo de Half Life.
 * @see HalfLifeMDL::MDLSkins
 * @author Pedro Pablo Gómez Martín
 * @date Diciembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLSkins
#define __HalfLifeMDL_MDLSkins

#include <string>
#include "MDLBasic.h"
#include "MDLTexture.h"

namespace HalfLifeMDL {

/**
 * Clase que almacena la información sobre las pieles (versiones de las 
 * texturas) de un modelo de Half Life.
 * <p>
 * Una piel es un cojunto de texturas. Un modelo puede almacenar varias
 * pieles, y en cada momento se utilizará una. <p>
 * Todas las pieles del modelo tienen el mismo número de texturas. Diferentes
 * pieles pueden compartir texturas. Cuando una malla referencia una textura,
 * en realidad referencia un índice dentro de la piel actual. Por ejemplo,
 * imaginemos un modelo con dos pieles, una con el traje limpio, y otra con
 * él manchado. Además, tiene tres texturas, una para el traje sucio,
 * una para el traje limpio, y la última con la cara.<br>
 * Las dos pieles referenciarán dos texturas:
 *	- Piel 0: textura del traje limpio, y textura de la cara.
 *	- Piel 1: textura del traje sucio, y textura de la cara.
 * Las mallas referencian posiciones de las pieles. Por tanto las mallas del
 * cuerpo usarán la textura 0, y las mallas de la cabeza usarán la textura 1.
 * Si la piel seleccionada es la 0, para el cuerpo se utilizará la textura 0
 * de la piel 0, es decir la textura del traje limpio. Si la piel seleccionada
 * es la 1, se utilizará la textura 0 de la piel 1, es decir la textura del
 * traje sucio. Para la cabeza se usará en ambos casos la textura de la cara.
 * <br>
 * Todas las texturas asociadas a un mismo índice en las pieles deben tener
 * las mismas dimensiones. En el ejemplo anterior, la textura del traje limpio
 * y del traje sucio deben tener las mismas dimensiones, pues ambas son la
 * textura número cero en diferentes pieles. Esto se debe al modo en el que
 * se almacenan las coordenadas de textura en el fichero.
 * <p>
 * Los objetos de esta clase almacenan la información estática de las pieles
 * de un modelo de Half Life. Es decir cuantas pieles hay, cuantas texturas
 * por piel, y qué texturas hay en cada piel.
 * <p>
 * Ésta es una clase "de alto nivel" a la que el usuario del módulo tiene 
 * acceso. Está directamente relacionada con los campos header_t::numskinref,
 * header_t::numskinfamilies y header_t::skinindex.
 * @author Pedro Pablo Gómez Martín
 * @date Diciembre, 2003.
 * @todo Enlace a la página general de la "arquitectura".
 */
class MDLSkins {
public:

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
	MDLSkins(unsigned int numSkins, unsigned int numTexturesPerSkin,
				MDLushort* skins, const MDLTexture** textures);

	/**
	 * Destructor. Libera la memoria del objeto.
	 */
	~MDLSkins();

	/**
	 * Devuelve el número de pieles.
	 * @return Número de pieles.
	 */
	unsigned int getNumSkins() const
				{ return _numSkins; }

	/**
	 * Devuelve el número de texturas que hay en cada piel.
	 */
	unsigned int getNumTexturesPerSkin() const
				{ return _numTexturesPerSkin; }

	/**
	 * Devuelve el índice dentro del modelo de la textura de una piel
	 * y un índice de textura en la piel.
	 * @param skin Índice de la piel cuya textura se desea obtener.
	 * @param texture Índice de la textura que se quiere obtener.
	 * @return Índice dentro del modelo de la textura solicitada.
	 */
	unsigned int getTextureIndex(unsigned int skin, 
									unsigned int texture) const {
				assert(skin < _numSkins);
				assert(texture < _numTexturesPerSkin);
				return _skins[skin*_numTexturesPerSkin + texture]; }

	/**
	 * Devuelve la textura del modelo que corresponde a una piel y a un índice
	 * de textura dentro de esa piel.
	 * @param skin Índice de la piel cuya textura se desea obtener.
	 * @param texture Índice de la textura que se quiere obtener.
	 * @return Textura solicitada..
	 */
	const MDLTexture* getTexture(unsigned int skin,
									unsigned int texture) const {
				return _textures[getTextureIndex(skin, texture)]; }

protected:

	/**
	 * Número de pieles del modelo.
	 */
	unsigned int _numSkins;

	/**
	 * Número de texturas de cada piel.
	 */
	unsigned int _numTexturesPerSkin;

	/**
	 * Información de las pieles.
	 * @todo Una vez que se sepa realmente el orden, decir qué va primero.
	 */
	unsigned int* _skins;

	/**
	 * Texturas del modelo.
	 */
	const MDLTexture** _textures;

private:
	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementación es vacía y no debe usarse.
	 */
	MDLSkins(const MDLSkins& source) {
		assert(!"Constructor copia MDLSkins no disponible"); }

	/**
	 * Operador de asignación privado para evitar problemas en los
	 * delete. Su implementación es vacía y no debe usarse.
	 */
	MDLSkins& operator=(const MDLSkins &source) {
		assert(!"Operador asignación en MDLSkins no disponible");
		return *this;}

}; // class MDLSkins

} // namespace HalfLifeMDL

#endif
