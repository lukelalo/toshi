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
  
    @author Juan Antonio Recio Garc�a, Pedro Pablo G�mez Mart�n
*/
class N_PUBLIC nFont : public nRoot {

public:

	nFont();

	~nFont();

	bool SaveCmds(nPersistServer* ps);
	
	static nKernelServer* kernelServer;	//Puntero al kernelServer

	//--------------------------------------------------------------------//
	/**
	 * Lee la informaci�n de la fuente contenida en una textura de un fichero
	 * e inicializa el objeto con ella. Adem�s, se carga la textura en el motor 
	 * gr�fico.
	 * Si se llama repetidamente a este m�todo, en cada llamada se liberar�n los
	 * recursos cargados anteriormente y se cargar� el nuevo fichero.
	 *
	 * @param fileName nombre del fichero que contiene la informaci�n
	 * @return true si no hubo errores, false en cualquier otro caso.
	 */
	bool load(const char* filename);


	/**
	 * Devuelve cierto si el car�cter pasado como par�metro est� disponible
	 * en la fuente actual.
	 * Si el objeto no est� inicializado el resultado ser� impredecible.
	 *
	 * @param character Car�cter que se consulta.
	 * @return Cierto si el car�cter est� disponible.
	 */
	bool isAvailable(unsigned short character) const {
		return fontData->isAvailable(character);
	}

	/**
	 * Devuelve el ancho de la textura de la fuente.
	 * Si el objeto no est� inicializado el resultado ser� impredecible.
	 *
	 * @return Ancho de la textura de la fuente.
	 */
	unsigned int getTextureWidth() const {
		return fontData->getTextureWidth();
	}

	/**
	 * Devuelve el alto de la textura de la fuente.
	 * Si el objeto no est� inicializado el resultado ser� impredecible.
	 *
	 * @return Alto de la textura de la fuente.
	 */
	unsigned int getTextureHeight() const {
		return fontData->getTextureHeight();
	}

	/**
	 * Devuelve el n�mero de p�xeles que asciende de la l�nea base el s�mbolo
	 * m�s alto (normalmente la letra f).
	 * Si el objeto no est� inicializado el resultado ser� impredecible.
	 *
	 * @return N�mero de p�xeles m�ximo de ascenso.
	 */
	unsigned int getMaxAscent() const {
		return fontData->getMaxAscent();
	}

	/**
	 * Devuelve el n�mero de p�xeles que desciende de la l�nea base el s�mbolo
	 * que m�s baja (normalmente la letra g):
	 * Si el objeto no est� inicializado el resultado ser� impredecible.
	 *
	 * @return N�mero de p�xeles m�ximo de descenso.
	 */
	unsigned int getMaxDescent() const {
		return fontData->getMaxDescent();
	}

	/**
	 * Devuelve el n�mero de p�xeles que hay que dejar entre l�nea y l�nea.
	 * Si el objeto no est� inicializado el resultado ser� impredecible.
	 *
	 * @return Espacio (en p�xeles) entre l�neas.
	 */
	unsigned int getPixelsBetweenLines() const {
		return fontData->getPixelsBetweenLines();
	}

	/**
	 * Devuelve la informaci�n referente a un s�mbolo.
	 *
	 * @param character Car�cter cuya informaci�n se desea obtener.
	 * @return Informaci�n referente al s�mbolo solicitado. Si el s�mbolo no
	 * est� disponible el resultado ser� impredecible.
	 *
	 * @todo Aqu� hay un conflicto de internacionalizaci�n... se supone que
	 * las clases que controlan las fuentes de la textura tratan los
	 * caracteres como unsigned short para curarse en salud. Sin embargo,
	 * desde aqu� estamos recibiendo un unsigned char. Se podr�a poner
	 * unsigned short directamente, y as� que el problema de la
	 * internacionalizaci�n sea culpa de otros (justo quien est� encima de
	 * nosotros ;-) )... pero el problema est� en que encima de nosotros
	 * seguramente tengamos a alguien que realmente guarda las cadenas
	 * en simples char*.<br>
	 * Esto parece inocente... pero en realidad al llamar a este supuesto
	 * m�todo que recibir�a un unsigned short pas�ndole un char la conversi�n
	 * de tipos no funciona demasiado bien... El problema est� en que, por
	 * ejemplo, el car�cter � (ASCII 225) es (en el tipo char) el -31... que
	 * al pasarlo a unsigned short... no se queda como 225 sino como 65505.
	 * Total, que para no complicarnos la vida hacemos esa conversi�n en
	 * dos etapas. Esta funci�n recibe un unsigned char. Seguramente nos
	 * pasen un char, pero esa conversi�n es v�lida (el -31 de la '�' se
	 * pasa al 225), y luego al llamar al controlador de fuente que recibe
	 * un unsigned short el 225 sigue siendo un 225 porque ya no se hace
	 * extensi�n de signo no deseada.
	 */
	const CTextureFont::tGlyph *getGlyph(unsigned char character) const {
		return fontData->getGlyph(character);
	}

	/**
	 * Devuelve la informaci�n de un simbolo utilizando coordenadas
	 * normalizadas independientes del tama�o de la textura subyacente.
	 *
	 * @param character Car�cter cuya informaci�n se desea obtener.
	 * @return Informaci�n referente al s�mbolo solicitado. Si el s�mbolo no
	 * est� disponible, el resultado ser� impredecible.
	 * @todo Vease el asociado a getGlyph.
	 */
	const CTextureFont::tGlyphRel *getGlyphRel(unsigned char character) const {
		return fontData->getGlyphRel(character);
	}

	/**
	 * Devuelve el tama�o de un texto dado en pixeles.
	 *
	 * @param text Texto del que se desea conocer el tama�o.
	 * @param tamX Parametro de salida que contendra el ancho del texto
	 * completo.
	 * @param tamY Par�metro de salida que contendr� el alto del texto
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