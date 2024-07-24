#ifndef N_FONT_H
#define N_FONT_H
//------------------------------------------------------------------------------
#include <map>

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif


#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_TEXTUREARRAY_H
#include "gfx/ntexturearray.h"
#endif

#include "TextureFont/TextureFont.h"

//------------------------------------------------------------------------------
/**
    @class nFont

    @brief Nebula Text Font Class
  
    @author Juan Antonio Recio García, Pedro Pablo Gómez Martín
*/
class N_PUBLIC nFont : public nRoot {

public:

	nFont();

	~nFont();

	bool SaveCmds(nPersistServer* ps);
	
	static nKernelServer* kernelServer;	//Puntero al kernelServer

	//--------------------------------------------------------------------//
	/**
	 * Lee la información de la fuente contenida en una textura de un fichero
	 * e inicializa el objeto con ella. Además, se carga la textura en el motor 
	 * gráfico.
	 * Si se llama repetidamente a este método, en cada llamada se liberarán los
	 * recursos cargados anteriormente y se cargará el nuevo fichero.
	 *
	 * @param fileName nombre del fichero que contiene la información
	 * @return true si no hubo errores, false en cualquier otro caso.
	 */
	bool load(const char* filename);


	/**
	 * Devuelve cierto si el carácter pasado como parámetro está disponible
	 * en la fuente actual.
	 * Si el objeto no está inicializado el resultado será impredecible.
	 *
	 * @param character Carácter que se consulta.
	 * @return Cierto si el carácter está disponible.
	 */
	bool isAvailable(unsigned short character) const {
		return fontData->isAvailable(character);
	}

	/**
	 * Devuelve el ancho de la textura de la fuente.
	 * Si el objeto no está inicializado el resultado será impredecible.
	 *
	 * @return Ancho de la textura de la fuente.
	 */
	unsigned int getTextureWidth() const {
		return fontData->getTextureWidth();
	}

	/**
	 * Devuelve el alto de la textura de la fuente.
	 * Si el objeto no está inicializado el resultado será impredecible.
	 *
	 * @return Alto de la textura de la fuente.
	 */
	unsigned int getTextureHeight() const {
		return fontData->getTextureHeight();
	}

	/**
	 * Devuelve el número de píxeles que asciende de la línea base el símbolo
	 * más alto (normalmente la letra f).
	 * Si el objeto no está inicializado el resultado será impredecible.
	 *
	 * @return Número de píxeles máximo de ascenso.
	 */
	unsigned int getMaxAscent() const {
		return fontData->getMaxAscent();
	}

	/**
	 * Devuelve el número de píxeles que desciende de la línea base el símbolo
	 * que más baja (normalmente la letra g):
	 * Si el objeto no está inicializado el resultado será impredecible.
	 *
	 * @return Número de píxeles máximo de descenso.
	 */
	unsigned int getMaxDescent() const {
		return fontData->getMaxDescent();
	}

	/**
	 * Devuelve el número de píxeles que hay que dejar entre línea y línea.
	 * Si el objeto no está inicializado el resultado será impredecible.
	 *
	 * @return Espacio (en píxeles) entre líneas.
	 */
	unsigned int getPixelsBetweenLines() const {
		return fontData->getPixelsBetweenLines();
	}

	/**
	 * Devuelve la información referente a un símbolo.
	 *
	 * @param character Carácter cuya información se desea obtener.
	 * @return Información referente al símbolo solicitado. Si el símbolo no
	 * está disponible el resultado será impredecible.
	 *
	 * @todo Aquí hay un conflicto de internacionalización... se supone que
	 * las clases que controlan las fuentes de la textura tratan los
	 * caracteres como unsigned short para curarse en salud. Sin embargo,
	 * desde aquí estamos recibiendo un unsigned char. Se podría poner
	 * unsigned short directamente, y así que el problema de la
	 * internacionalización sea culpa de otros (justo quien esté encima de
	 * nosotros ;-) )... pero el problema está en que encima de nosotros
	 * seguramente tengamos a alguien que realmente guarda las cadenas
	 * en simples char*.<br>
	 * Esto parece inocente... pero en realidad al llamar a este supuesto
	 * método que recibiría un unsigned short pasándole un char la conversión
	 * de tipos no funciona demasiado bien... El problema está en que, por
	 * ejemplo, el carácter á (ASCII 225) es (en el tipo char) el -31... que
	 * al pasarlo a unsigned short... no se queda como 225 sino como 65505.
	 * Total, que para no complicarnos la vida hacemos esa conversión en
	 * dos etapas. Esta función recibe un unsigned char. Seguramente nos
	 * pasen un char, pero esa conversión es válida (el -31 de la 'á' se
	 * pasa al 225), y luego al llamar al controlador de fuente que recibe
	 * un unsigned short el 225 sigue siendo un 225 porque ya no se hace
	 * extensión de signo no deseada.
	 */
	const CTextureFont::tGlyph *getGlyph(unsigned char character) const {
		return fontData->getGlyph(character);
	}

	/**
	 * Devuelve la información de un simbolo utilizando coordenadas
	 * normalizadas independientes del tamaño de la textura subyacente.
	 *
	 * @param character Carácter cuya información se desea obtener.
	 * @return Información referente al símbolo solicitado. Si el símbolo no
	 * está disponible, el resultado será impredecible.
	 * @todo Vease el asociado a getGlyph.
	 */
	const CTextureFont::tGlyphRel *getGlyphRel(unsigned char character) const {
		return fontData->getGlyphRel(character);
	}

	/**
	 * Devuelve el tamaño de un texto dado en pixeles.
	 *
	 * @param text Texto del que se desea conocer el tamaño.
	 * @param tamX Parametro de salida que contendra el ancho del texto
	 * completo.
	 * @param tamY Parámetro de salida que contendrá el alto del texto
	 * completo.
	 */
	void getTextSize(const unsigned char *text, int &tamX, int &tamY) const;

	nTextureArray* getTexture() const {
		return texture;
	}

protected:

	CTextureFont* fontData;
	nTextureArray* texture;

	void release();

};
//------------------------------------------------------------------------------
#endif