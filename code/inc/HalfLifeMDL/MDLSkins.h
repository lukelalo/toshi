//---------------------------------------------------------------------------
// MDLSkins.h
//---------------------------------------------------------------------------

/**
 * @file MDLSkins.h
 * Contiene la declaraci�n de la clase que almacena la informaci�n sobre
 * las pieles (versiones de texturas) del modelo de Half Life.
 * @see HalfLifeMDL::MDLSkins
 * @author Pedro Pablo G�mez Mart�n
 * @date Diciembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLSkins
#define __HalfLifeMDL_MDLSkins

#include <string>
#include "MDLBasic.h"
#include "MDLTexture.h"

namespace HalfLifeMDL {

/**
 * Clase que almacena la informaci�n sobre las pieles (versiones de las 
 * texturas) de un modelo de Half Life.
 * <p>
 * Una piel es un cojunto de texturas. Un modelo puede almacenar varias
 * pieles, y en cada momento se utilizar� una. <p>
 * Todas las pieles del modelo tienen el mismo n�mero de texturas. Diferentes
 * pieles pueden compartir texturas. Cuando una malla referencia una textura,
 * en realidad referencia un �ndice dentro de la piel actual. Por ejemplo,
 * imaginemos un modelo con dos pieles, una con el traje limpio, y otra con
 * �l manchado. Adem�s, tiene tres texturas, una para el traje sucio,
 * una para el traje limpio, y la �ltima con la cara.<br>
 * Las dos pieles referenciar�n dos texturas:
 *	- Piel 0: textura del traje limpio, y textura de la cara.
 *	- Piel 1: textura del traje sucio, y textura de la cara.
 * Las mallas referencian posiciones de las pieles. Por tanto las mallas del
 * cuerpo usar�n la textura 0, y las mallas de la cabeza usar�n la textura 1.
 * Si la piel seleccionada es la 0, para el cuerpo se utilizar� la textura 0
 * de la piel 0, es decir la textura del traje limpio. Si la piel seleccionada
 * es la 1, se utilizar� la textura 0 de la piel 1, es decir la textura del
 * traje sucio. Para la cabeza se usar� en ambos casos la textura de la cara.
 * <br>
 * Todas las texturas asociadas a un mismo �ndice en las pieles deben tener
 * las mismas dimensiones. En el ejemplo anterior, la textura del traje limpio
 * y del traje sucio deben tener las mismas dimensiones, pues ambas son la
 * textura n�mero cero en diferentes pieles. Esto se debe al modo en el que
 * se almacenan las coordenadas de textura en el fichero.
 * <p>
 * Los objetos de esta clase almacenan la informaci�n est�tica de las pieles
 * de un modelo de Half Life. Es decir cuantas pieles hay, cuantas texturas
 * por piel, y qu� texturas hay en cada piel.
 * <p>
 * �sta es una clase "de alto nivel" a la que el usuario del m�dulo tiene 
 * acceso. Est� directamente relacionada con los campos header_t::numskinref,
 * header_t::numskinfamilies y header_t::skinindex.
 * @author Pedro Pablo G�mez Mart�n
 * @date Diciembre, 2003.
 * @todo Enlace a la p�gina general de la "arquitectura".
 */
class MDLSkins {
public:

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
	MDLSkins(unsigned int numSkins, unsigned int numTexturesPerSkin,
				MDLushort* skins, const MDLTexture** textures);

	/**
	 * Destructor. Libera la memoria del objeto.
	 */
	~MDLSkins();

	/**
	 * Devuelve el n�mero de pieles.
	 * @return N�mero de pieles.
	 */
	unsigned int getNumSkins() const
				{ return _numSkins; }

	/**
	 * Devuelve el n�mero de texturas que hay en cada piel.
	 */
	unsigned int getNumTexturesPerSkin() const
				{ return _numTexturesPerSkin; }

	/**
	 * Devuelve el �ndice dentro del modelo de la textura de una piel
	 * y un �ndice de textura en la piel.
	 * @param skin �ndice de la piel cuya textura se desea obtener.
	 * @param texture �ndice de la textura que se quiere obtener.
	 * @return �ndice dentro del modelo de la textura solicitada.
	 */
	unsigned int getTextureIndex(unsigned int skin, 
									unsigned int texture) const {
				assert(skin < _numSkins);
				assert(texture < _numTexturesPerSkin);
				return _skins[skin*_numTexturesPerSkin + texture]; }

	/**
	 * Devuelve la textura del modelo que corresponde a una piel y a un �ndice
	 * de textura dentro de esa piel.
	 * @param skin �ndice de la piel cuya textura se desea obtener.
	 * @param texture �ndice de la textura que se quiere obtener.
	 * @return Textura solicitada..
	 */
	const MDLTexture* getTexture(unsigned int skin,
									unsigned int texture) const {
				return _textures[getTextureIndex(skin, texture)]; }

protected:

	/**
	 * N�mero de pieles del modelo.
	 */
	unsigned int _numSkins;

	/**
	 * N�mero de texturas de cada piel.
	 */
	unsigned int _numTexturesPerSkin;

	/**
	 * Informaci�n de las pieles.
	 * @todo Una vez que se sepa realmente el orden, decir qu� va primero.
	 */
	unsigned int* _skins;

	/**
	 * Texturas del modelo.
	 */
	const MDLTexture** _textures;

private:
	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLSkins(const MDLSkins& source) {
		assert(!"Constructor copia MDLSkins no disponible"); }

	/**
	 * Operador de asignaci�n privado para evitar problemas en los
	 * delete. Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLSkins& operator=(const MDLSkins &source) {
		assert(!"Operador asignaci�n en MDLSkins no disponible");
		return *this;}

}; // class MDLSkins

} // namespace HalfLifeMDL

#endif
