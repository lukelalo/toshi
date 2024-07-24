//---------------------------------------------------------------------------

#ifndef __TextureFont_H
#define __TextureFont_H

#include <assert.h>

//---------------------------------------------------------------------------

#define CTEXTURE_FONT_COMPILE_SAVE_FILE


/**
 * Clase que almacena la informaci�n sobre la posici�n de los diferentes
 * glyphs (s�mbolos) de una fuente almacenada en una textura. Tiene un m�todo
 * para inicializar la informaci�n a partir de un fichero en disco, y otro
 * para guardarlo.<br>
 * Esta clase <b>no</b> almacena la imagen, �nicamente la posici�n y tama�o
 * de cada s�mbolo. Cada s�mbolo se referencia utilizando enteros cortos sin
 * signo (16 bits). En la pr�ctica la mayor�a de las veces se utilizar�n
 * simplemente los 8 bits bajos (c�digo ASCII tradicional, con la codificaci�n
 * regional concreta).
 * <p>
 * El uso habitual consistir� en inicializar un objeto de esta clase con la
 * informaci�n de una fuente utilizando setFont(<nombreDelFichero>). Ese
 * fichero se habr� generado con una herramienta que automatice su
 * creaci�n.
 * <p>
 * Luego el "motor de texto" utilizar� la informaci�n proporcionada por el
 * objeto para construir los textos que se necesiten. Los m�todos
 * interesantes son:
 * <ul>
 *   <li><code>isAvailable</code>: indica si un determinado s�mbolo (entero
 * corto sin signo) est� disponible en la fuente actual.
 *   <li><code>getPixelsBetweenLines</code>: devuelve el n�mero de p�xeles
 * (seg�n las dimensiones de la textura) que hay que dejar de separaci�n entre
 * fuentes para que haya espacio suficiente y nose produzcan solapamientos. Es
 * posible obtener ese n�mero de p�xeles de forma normalizada (entre 0 y 1)
 * de acuerdo al alto de la textura dividiendo el valor por el entero devuelto
 * por <code>getTextureHeight</code>.
 *   <li><code>getGlyph</code>: devuelve la informaci�n de un s�mbolo pasado
 * como par�metro. El s�mbolo debe existir.
 *   <li><code>getGlyphRel</code>: semejante a la anterior.
 * </ul>
 * Tanto <code>getGlyph</code> como <code>getGlyphRel</code> proporcionan
 * informaci�n semejante. La diferencia principal es que <code>getGlyph</code>
 * devuelve la posici�n y tama�o del rect�ngulo del s�mbolo en la textura
 * usando a los p�xeles en la imagen como unidad de medida. Por su parte
 * <code>getGlyphRel</code> devuelve informaci�n normalizada, apta para
 * utilizar directamente como coordenadas de textura. Ambas funciones son
 * igual de r�pidas (la informaci�n est� precalculada).
 * <p>
 * Ambos m�todos devuelven, adem�s, dos enteros con dimensiones especificadas
 * en p�xeles en ambos casos. El campo <code>forward</code> especifica cuantos
 * p�xeles hay que avanzar la posici�n actual una vez escrito el s�mbolo. Por
 * su parte, el campo <code>back</code> especifica cuanto hay que avanzar
 * la posici�n actual <b>antes</b> de escribirlo. La utilidad principal de
 * estos dos campos est� en las letras cursiva (por ejemplo), en las que una
 * letra tiende a solaparse con la anterior en cierta medida.
 * <p><p>
 * El formato del fichero est� pensado para que pueda "pegarse" a cualquier
 * otro en su parte final. Se ha comprobado que los cargadores de im�genes
 * no parecen tener problemas si se a�ade informaci�n adicional al final de
 * los ficheros. El formato est� pensado para que la informaci�n se pegue por
 * lo tanto al final de un fichero "huesped" (t�picamente un fichero de imagen)
 * para guardar de forma conjunta la informaci�n.
 * <p>
 * Para lograrlo, en lugar de tener cabecera tiene "coda", con un puntero
 * a la posici�n donde empieza realmente en el fichero la informaci�n que
 * nos interesa aqu�. Esto permite que, en realidad, la informaci�n pueda
 * tambi�n guardarse de forma independiente en un fichero separado a la
 * propia imagen.
 */
class CTextureFont {

public:

	/**
	 * Estructura que almacena la informaci�n de un s�mbolo (letra, d�gito...)
	 */
	struct tGlyph {
		/**
		 * Car�cter (en principio ASCII, pero no nos atamos las manos y
		 * guardamos un short) del s�mbolo cuya informaci�n guardamos.
		 */
		unsigned short character;

		/**
		 * Posici�n X (en p�xeles) de la esquina superior izquierda del
		 * rect�ngulo que mantiene el s�mbolo en la textura. La posici�n (0,0)
		 * de la textura es la esquina superior izquierda.
		 */
		unsigned int x;

		/**
		 * Posici�n Y (en p�xeles) de la esquina superior izquierda del
		 * rect�ngulo que mantiene el s�mbolo en la textura. La posici�n (0,0)
		 * de la textura es la esquina superior izquierda.
		 */
		unsigned int y;

		/**
		 * Ancho del rect�ngulo con el s�mbolo, en p�xeles.
		 */
		unsigned int width;

		/**
		 * Alto del rect�ngulo con el s�mbolo en p�xeles.
		 */
		unsigned int height;

		/**
		 * N�mero de p�xeles que hay que desplazar la posici�n actual una vez
		 * escrito el s�mbolo actual, antes de escribir el siguiente.
		 */
		int forward;

		/**
		 * N�mero de p�xeles que hay que desplazar la posici�n actual
		 * ANTES de escribir el s�mbolo actual.
		 */
		int back;
	};

	/**
	 * Estructura que almacena informaci�n general de una fuente almacenada en
	 * una textura.
	 */
	struct tFont {

		/**
		 * Ancho de la textura en p�xeles.
		 */
		unsigned int width;

		/**
		 * Alto de la textura en p�xeles.
		 */
		unsigned int height;

		/**
		 * N�mero de p�xeles como mucho que ascienden las letras desde su
		 * l�nea base (normalmente la de la letra f)
		 */
		unsigned int maxAscent;

		/**
		 * N�mero de p�xeles como mucho que descienden las letras desde su
		 * l�nea base (normalmente la de la letra g).
		 */
		unsigned int maxDescent;

		/**
		 * Espacio indicado por la fuente para pasar de una l�nea a otra.
		 */
		unsigned int baseLineToBaseLine;

		/**
		 * N�mero de s�mbolos almacenados en la textura.
		 */
		unsigned int numGlyphs;
	};

	/**
	 * Estructura que almacena la informaci�n de un s�mbolo (letra, d�gito...)
	 * con las posiciones y tama�os normalizados para hacerlos independientes
	 * del tama�o de la textura.
	 */
	struct tGlyphRel {

		/**
		 * Posici�n X (entre 0 y 1) de la esquina superior izquierda del
		 * rect�ngulo que mantiene el s�mbolo en la textura. La posici�n (0,0)
		 * de la textura es la esquina superior izquierda.
		 */
		float x1;

		/**
		 * Posici�n Y (entre 0 y 1) de la esquina superior izquierda del
		 * rect�ngulo que mantiene el s�mbolo en la textura. La posici�n (0,0)
		 * de la textura es la esquina superior izquierda.
		 */
		float y1;

		/**
		 * Posici�n X (entre 0 y 1) de la esquina inferior derecha del
		 * rect�ngulo que mantiene el s�mbolo en la textura. La posici�n (0,0)
		 * de la textura es la esquina superior izquierda.
		 */
		float x2;

		/**
		 * Posici�n Y (entre 0 y 1) de la esquina inferior derecha del
		 * rect�ngulo que mantiene el s�mbolo en la textura. La posici�n (0,0)
		 * de la textura es la esquina superior izquierda.
		 */
		float y2;

		/**
		 * N�mero de p�xeles que hay que desplazar la posici�n actual una vez
		 * escrito el s�mbolo actual, antes de escribir el siguiente.
		 */
		int forward;

		/**
		 * N�mero de p�xeles que hay que desplazar la posici�n actual
		 * ANTES de escribir el s�mbolo actual.
		 */
		int back;
	};

	/**
	 * Crea un objeto vac�o. Para poder hacer uso de �l habr� que llamar a
	 * alguno de los constructores retardados setTexture.
	 */
	CTextureFont();

	/**
	 * Destructor. Libera los recursos.
	 */
	~CTextureFont();

	/**
	 * Lee la informaci�n de la fuente contenida en una textura de un fichero
	 * e inicializa el objeto con ella.
	 *
	 * @param fileName nombre del fichero que contiene la informaci�n.
	 * @return Cierto si todo fue bien. Falso en caso contrario. En ese caso,
	 * si el objeto hab�a sido inicializado previamente, se perder� la
	 * informaci�n anterior.
	 */
	bool setFont(const char *fileName);

#ifdef CTEXTURE_FONT_COMPILE_SAVE_FILE
	/**
	 * Inicializa el objeto a partir de la informaci�n de la fuente contenida
	 * en las estructuras pasadas como par�metros. Este constructor s�lo
	 * tiene utilidad en la herramienta de generaci�n de texturas.
	 *
	 * @param font Informaci�n general sobre la fuente.
	 * @param glyphs Array con la informaci�n de cada s�mbolo. Debe contener
	 * al menos el n�mero de s�mbolos especificados por el campo num_glyphs del
	 * par�metro font.
	 * @return Cierto si todo fue bien. Falso en caso contrario. En ese caso,
	 * si el objeto hab�a sido inicializado previamente, se perder� la
	 * informaci�n anterior.
	 */
	bool setFont(const tFont *font, const tGlyph *glyphs);

	/**
	 * Almacena la informaci�n de la fuente actual en el fichero cuyo nombre
	 * se pasa como par�metro. Si el fichero ya existe, se a�ade la
	 * informaci�n al final de �l.
	 *
	 * @return Cierto si todo fue bien. Falso si se ha producido alg�n error,
	 * bien porque la fuente no est� inicializada correctamente, o porque
	 * se haya producido alg�n problema con el fichero. En ese �ltimo caso
	 * el fichero puede haber quedado en cualquier estado.
	 */
	bool saveFile(const char* fileName);

#endif

	/**
	 * Devuelve cierto si el car�cter pasado como par�metro est� disponible
	 * en la fuente actual.
	 * Si el objeto no est� inicializado el resultado ser� impredecible.
	 *
	 * @param character Car�cter que se consulta.
	 * @return Cierto si el car�cter est� disponible.
	 */
	bool isAvailable(unsigned short character) {
		return ((character >= _firstChar) &&
				(character <= _lastChar) &&
				(_glyphsInd[character - _firstChar] != NULL));
	}

	/**
	 * Devuelve el ancho de la textura de la fuente.
	 * Si el objeto no est� inicializado el resultado ser� impredecible.
	 *
	 * @return Ancho de la textura de la fuente.
	 */
	unsigned int getTextureWidth() {
			assert(_initialized);
			return _fontInfo.width;
	}

	/**
	 * Devuelve el alto de la textura de la fuente.
	 * Si el objeto no est� inicializado el resultado ser� impredecible.
	 *
	 * @return Alto de la textura de la fuente.
	 */
	unsigned int getTextureHeight() {
			assert(_initialized);
			return _fontInfo.height;
	}

	/**
	 * Devuelve el n�mero de p�xeles que asciende de la l�nea base el s�mbolo
	 * m�s alto (normalmente la letra f).
	 * Si el objeto no est� inicializado el resultado ser� impredecible.
	 *
	 * @return N�mero de p�xeles m�ximo de ascenso.
	 */
	unsigned int getMaxAscent() {
			assert(_initialized);
			return _fontInfo.maxAscent;
	}

	/**
	 * Devuelve el n�mero de p�xeles que desciende de la l�nea base el s�mbolo
	 * que m�s baja (normalmente la letra g):
	 * Si el objeto no est� inicializado el resultado ser� impredecible.
	 *
	 * @return N�mero de p�xeles m�ximo de descenso.
	 */
	unsigned int getMaxDescent() {
			assert(_initialized);
			return _fontInfo.maxDescent;
	}

	/**
	 * Devuelve el n�mero de p�xeles que hay que dejar entre l�nea y l�nea.
	 * Si el objeto no est� inicializado el resultado ser� impredecible.
	 *
	 * @return Espacio (en p�xeles) entre l�neas.
	 */
	unsigned int getPixelsBetweenLines() {
			assert(_initialized);
			return _fontInfo.baseLineToBaseLine;
	}

	/**
	 * Devuelve la informaci�n referente a un s�mbolo.
	 *
	 * @param character Car�cter cuya informaci�n se desea obtener.
	 * @return Informaci�n referente al s�mbolo solicitado. Si el s�mbolo no
	 * est� disponible el resultado ser� impredecible.
	 */
	const tGlyph *getGlyph(unsigned short character) {
			assert(_initialized);
			return _glyphsInd[character - _firstChar];
	}

	/**
	 * Devuelve la informaci�n de un simbolo utilizando coordenadas
	 * normalizadas independientes del tama�o de la textura subyacente.
	 *
	 * @param character Car�cter cuya informaci�n se desea obtener.
	 * @return Informaci�n referente al s�mbolo solicitado. Si el s�mbolo no
	 * est� disponible, el resultado ser� impredecible.
	 */
	const tGlyphRel *getGlyphRel(unsigned short character) {
			assert(_initialized);
			return _glyphsRelInd[character - _firstChar];
	}

protected:

	/**
	 * Estructura con los campos de la "coda" del fichero.
	 */
	struct tFileCoda {

		/**
		 * N�mero m�gico del fichero. Ser� FeTX (Fuente En TeXtura).
		 */
		char magic[4];

		/**
		 * N�mero dummy. Se guardar� el valor 0x12345678 en little endian.
		 * Se utiliza para averiguar si la arquitectura del sistema es
		 * big endian o little endian y adaptarse a ello.
		 */
		unsigned int little;	// 0x12345678

		/**
		 * Posici�n en el fichero donde se encuentra el inicio de la
		 * informaci�n de la fuente.
		 */
		unsigned int position;

	};

	/**
	 * Limpia el objeto, liberando la memoria.
	 */
	void release();

#ifndef NDEBUG
	/**
	 * Atributo que indica si la fuente ha sido inicializada o no. S�lo est�
	 * disponible en la compilaci�n en modo depuraci�n.
	 */
	bool _initialized;
#endif

	/**
	 * Informaci�n general de la fuente.
	 */
	tFont _fontInfo;

	/**
	 * Array din�mico con la informaci�n de cada s�mbolo con la informaci�n
	 * medida en p�xeles
	 */
	tGlyph* _glyphs;

	/**
	 * Array din�mico con la informaci�n de cada s�mbolo con la informaci�n
	 * normalizada para hacerla independiente del tama�o de la textura.
	 */
	tGlyphRel* _glyphsRel;

	/**
	 * Array din�mico con punteros a la informaci�n de cada s�mbolo. Sirve para
	 * acelerar la b�squeda sin utilizar una tabla hash, bajo riesgo de perder
	 * un poco de memoria.
	 */
	tGlyph** _glyphsInd;

	/**
	 * Array din�mico con punteros a la informaci�n de cada s�mbolo con
	 * coordenadas normalizadas. Sirve para acelerar la b�squeda sin utilizar
	 * una tabla hash, bajo riesgo de perder un poco de memoria.
	 */
	tGlyphRel** _glyphsRelInd;

	/**
	 * Valor entero del primer s�mbolo que se almacena dentro de _glyphsInd
	 * y _glyphsRelInd.
	 */
	unsigned short _firstChar;

	/**
	 * Valor entero del �ltimo s�mbolo que se almacena dentro de _glyphsInd
	 * y _glyphsRelInd.
	 */
	unsigned short _lastChar;

}; // class CTextureFont

#endif
