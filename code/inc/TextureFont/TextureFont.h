//---------------------------------------------------------------------------

#ifndef __TextureFont_H
#define __TextureFont_H

#include <assert.h>

//---------------------------------------------------------------------------

#define CTEXTURE_FONT_COMPILE_SAVE_FILE


/**
 * Clase que almacena la información sobre la posición de los diferentes
 * glyphs (símbolos) de una fuente almacenada en una textura. Tiene un método
 * para inicializar la información a partir de un fichero en disco, y otro
 * para guardarlo.<br>
 * Esta clase <b>no</b> almacena la imagen, únicamente la posición y tamaño
 * de cada símbolo. Cada símbolo se referencia utilizando enteros cortos sin
 * signo (16 bits). En la práctica la mayoría de las veces se utilizarán
 * simplemente los 8 bits bajos (código ASCII tradicional, con la codificación
 * regional concreta).
 * <p>
 * El uso habitual consistirá en inicializar un objeto de esta clase con la
 * información de una fuente utilizando setFont(<nombreDelFichero>). Ese
 * fichero se habrá generado con una herramienta que automatice su
 * creación.
 * <p>
 * Luego el "motor de texto" utilizará la información proporcionada por el
 * objeto para construir los textos que se necesiten. Los métodos
 * interesantes son:
 * <ul>
 *   <li><code>isAvailable</code>: indica si un determinado símbolo (entero
 * corto sin signo) está disponible en la fuente actual.
 *   <li><code>getPixelsBetweenLines</code>: devuelve el número de píxeles
 * (según las dimensiones de la textura) que hay que dejar de separación entre
 * fuentes para que haya espacio suficiente y nose produzcan solapamientos. Es
 * posible obtener ese número de píxeles de forma normalizada (entre 0 y 1)
 * de acuerdo al alto de la textura dividiendo el valor por el entero devuelto
 * por <code>getTextureHeight</code>.
 *   <li><code>getGlyph</code>: devuelve la información de un símbolo pasado
 * como parámetro. El símbolo debe existir.
 *   <li><code>getGlyphRel</code>: semejante a la anterior.
 * </ul>
 * Tanto <code>getGlyph</code> como <code>getGlyphRel</code> proporcionan
 * información semejante. La diferencia principal es que <code>getGlyph</code>
 * devuelve la posición y tamaño del rectángulo del símbolo en la textura
 * usando a los píxeles en la imagen como unidad de medida. Por su parte
 * <code>getGlyphRel</code> devuelve información normalizada, apta para
 * utilizar directamente como coordenadas de textura. Ambas funciones son
 * igual de rápidas (la información está precalculada).
 * <p>
 * Ambos métodos devuelven, además, dos enteros con dimensiones especificadas
 * en píxeles en ambos casos. El campo <code>forward</code> especifica cuantos
 * píxeles hay que avanzar la posición actual una vez escrito el símbolo. Por
 * su parte, el campo <code>back</code> especifica cuanto hay que avanzar
 * la posición actual <b>antes</b> de escribirlo. La utilidad principal de
 * estos dos campos está en las letras cursiva (por ejemplo), en las que una
 * letra tiende a solaparse con la anterior en cierta medida.
 * <p><p>
 * El formato del fichero está pensado para que pueda "pegarse" a cualquier
 * otro en su parte final. Se ha comprobado que los cargadores de imágenes
 * no parecen tener problemas si se añade información adicional al final de
 * los ficheros. El formato está pensado para que la información se pegue por
 * lo tanto al final de un fichero "huesped" (típicamente un fichero de imagen)
 * para guardar de forma conjunta la información.
 * <p>
 * Para lograrlo, en lugar de tener cabecera tiene "coda", con un puntero
 * a la posición donde empieza realmente en el fichero la información que
 * nos interesa aquí. Esto permite que, en realidad, la información pueda
 * también guardarse de forma independiente en un fichero separado a la
 * propia imagen.
 */
class CTextureFont {

public:

	/**
	 * Estructura que almacena la información de un símbolo (letra, dígito...)
	 */
	struct tGlyph {
		/**
		 * Carácter (en principio ASCII, pero no nos atamos las manos y
		 * guardamos un short) del símbolo cuya información guardamos.
		 */
		unsigned short character;

		/**
		 * Posición X (en píxeles) de la esquina superior izquierda del
		 * rectángulo que mantiene el símbolo en la textura. La posición (0,0)
		 * de la textura es la esquina superior izquierda.
		 */
		unsigned int x;

		/**
		 * Posición Y (en píxeles) de la esquina superior izquierda del
		 * rectángulo que mantiene el símbolo en la textura. La posición (0,0)
		 * de la textura es la esquina superior izquierda.
		 */
		unsigned int y;

		/**
		 * Ancho del rectángulo con el símbolo, en píxeles.
		 */
		unsigned int width;

		/**
		 * Alto del rectángulo con el símbolo en píxeles.
		 */
		unsigned int height;

		/**
		 * Número de píxeles que hay que desplazar la posición actual una vez
		 * escrito el símbolo actual, antes de escribir el siguiente.
		 */
		int forward;

		/**
		 * Número de píxeles que hay que desplazar la posición actual
		 * ANTES de escribir el símbolo actual.
		 */
		int back;
	};

	/**
	 * Estructura que almacena información general de una fuente almacenada en
	 * una textura.
	 */
	struct tFont {

		/**
		 * Ancho de la textura en píxeles.
		 */
		unsigned int width;

		/**
		 * Alto de la textura en píxeles.
		 */
		unsigned int height;

		/**
		 * Número de píxeles como mucho que ascienden las letras desde su
		 * línea base (normalmente la de la letra f)
		 */
		unsigned int maxAscent;

		/**
		 * Número de píxeles como mucho que descienden las letras desde su
		 * línea base (normalmente la de la letra g).
		 */
		unsigned int maxDescent;

		/**
		 * Espacio indicado por la fuente para pasar de una línea a otra.
		 */
		unsigned int baseLineToBaseLine;

		/**
		 * Número de símbolos almacenados en la textura.
		 */
		unsigned int numGlyphs;
	};

	/**
	 * Estructura que almacena la información de un símbolo (letra, dígito...)
	 * con las posiciones y tamaños normalizados para hacerlos independientes
	 * del tamaño de la textura.
	 */
	struct tGlyphRel {

		/**
		 * Posición X (entre 0 y 1) de la esquina superior izquierda del
		 * rectángulo que mantiene el símbolo en la textura. La posición (0,0)
		 * de la textura es la esquina superior izquierda.
		 */
		float x1;

		/**
		 * Posición Y (entre 0 y 1) de la esquina superior izquierda del
		 * rectángulo que mantiene el símbolo en la textura. La posición (0,0)
		 * de la textura es la esquina superior izquierda.
		 */
		float y1;

		/**
		 * Posición X (entre 0 y 1) de la esquina inferior derecha del
		 * rectángulo que mantiene el símbolo en la textura. La posición (0,0)
		 * de la textura es la esquina superior izquierda.
		 */
		float x2;

		/**
		 * Posición Y (entre 0 y 1) de la esquina inferior derecha del
		 * rectángulo que mantiene el símbolo en la textura. La posición (0,0)
		 * de la textura es la esquina superior izquierda.
		 */
		float y2;

		/**
		 * Número de píxeles que hay que desplazar la posición actual una vez
		 * escrito el símbolo actual, antes de escribir el siguiente.
		 */
		int forward;

		/**
		 * Número de píxeles que hay que desplazar la posición actual
		 * ANTES de escribir el símbolo actual.
		 */
		int back;
	};

	/**
	 * Crea un objeto vacío. Para poder hacer uso de él habrá que llamar a
	 * alguno de los constructores retardados setTexture.
	 */
	CTextureFont();

	/**
	 * Destructor. Libera los recursos.
	 */
	~CTextureFont();

	/**
	 * Lee la información de la fuente contenida en una textura de un fichero
	 * e inicializa el objeto con ella.
	 *
	 * @param fileName nombre del fichero que contiene la información.
	 * @return Cierto si todo fue bien. Falso en caso contrario. En ese caso,
	 * si el objeto había sido inicializado previamente, se perderá la
	 * información anterior.
	 */
	bool setFont(const char *fileName);

#ifdef CTEXTURE_FONT_COMPILE_SAVE_FILE
	/**
	 * Inicializa el objeto a partir de la información de la fuente contenida
	 * en las estructuras pasadas como parámetros. Este constructor sólo
	 * tiene utilidad en la herramienta de generación de texturas.
	 *
	 * @param font Información general sobre la fuente.
	 * @param glyphs Array con la información de cada símbolo. Debe contener
	 * al menos el número de símbolos especificados por el campo num_glyphs del
	 * parámetro font.
	 * @return Cierto si todo fue bien. Falso en caso contrario. En ese caso,
	 * si el objeto había sido inicializado previamente, se perderá la
	 * información anterior.
	 */
	bool setFont(const tFont *font, const tGlyph *glyphs);

	/**
	 * Almacena la información de la fuente actual en el fichero cuyo nombre
	 * se pasa como parámetro. Si el fichero ya existe, se añade la
	 * información al final de él.
	 *
	 * @return Cierto si todo fue bien. Falso si se ha producido algún error,
	 * bien porque la fuente no esté inicializada correctamente, o porque
	 * se haya producido algún problema con el fichero. En ese último caso
	 * el fichero puede haber quedado en cualquier estado.
	 */
	bool saveFile(const char* fileName);

#endif

	/**
	 * Devuelve cierto si el carácter pasado como parámetro está disponible
	 * en la fuente actual.
	 * Si el objeto no está inicializado el resultado será impredecible.
	 *
	 * @param character Carácter que se consulta.
	 * @return Cierto si el carácter está disponible.
	 */
	bool isAvailable(unsigned short character) {
		return ((character >= _firstChar) &&
				(character <= _lastChar) &&
				(_glyphsInd[character - _firstChar] != NULL));
	}

	/**
	 * Devuelve el ancho de la textura de la fuente.
	 * Si el objeto no está inicializado el resultado será impredecible.
	 *
	 * @return Ancho de la textura de la fuente.
	 */
	unsigned int getTextureWidth() {
			assert(_initialized);
			return _fontInfo.width;
	}

	/**
	 * Devuelve el alto de la textura de la fuente.
	 * Si el objeto no está inicializado el resultado será impredecible.
	 *
	 * @return Alto de la textura de la fuente.
	 */
	unsigned int getTextureHeight() {
			assert(_initialized);
			return _fontInfo.height;
	}

	/**
	 * Devuelve el número de píxeles que asciende de la línea base el símbolo
	 * más alto (normalmente la letra f).
	 * Si el objeto no está inicializado el resultado será impredecible.
	 *
	 * @return Número de píxeles máximo de ascenso.
	 */
	unsigned int getMaxAscent() {
			assert(_initialized);
			return _fontInfo.maxAscent;
	}

	/**
	 * Devuelve el número de píxeles que desciende de la línea base el símbolo
	 * que más baja (normalmente la letra g):
	 * Si el objeto no está inicializado el resultado será impredecible.
	 *
	 * @return Número de píxeles máximo de descenso.
	 */
	unsigned int getMaxDescent() {
			assert(_initialized);
			return _fontInfo.maxDescent;
	}

	/**
	 * Devuelve el número de píxeles que hay que dejar entre línea y línea.
	 * Si el objeto no está inicializado el resultado será impredecible.
	 *
	 * @return Espacio (en píxeles) entre líneas.
	 */
	unsigned int getPixelsBetweenLines() {
			assert(_initialized);
			return _fontInfo.baseLineToBaseLine;
	}

	/**
	 * Devuelve la información referente a un símbolo.
	 *
	 * @param character Carácter cuya información se desea obtener.
	 * @return Información referente al símbolo solicitado. Si el símbolo no
	 * está disponible el resultado será impredecible.
	 */
	const tGlyph *getGlyph(unsigned short character) {
			assert(_initialized);
			return _glyphsInd[character - _firstChar];
	}

	/**
	 * Devuelve la información de un simbolo utilizando coordenadas
	 * normalizadas independientes del tamaño de la textura subyacente.
	 *
	 * @param character Carácter cuya información se desea obtener.
	 * @return Información referente al símbolo solicitado. Si el símbolo no
	 * está disponible, el resultado será impredecible.
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
		 * Número mágico del fichero. Será FeTX (Fuente En TeXtura).
		 */
		char magic[4];

		/**
		 * Número dummy. Se guardará el valor 0x12345678 en little endian.
		 * Se utiliza para averiguar si la arquitectura del sistema es
		 * big endian o little endian y adaptarse a ello.
		 */
		unsigned int little;	// 0x12345678

		/**
		 * Posición en el fichero donde se encuentra el inicio de la
		 * información de la fuente.
		 */
		unsigned int position;

	};

	/**
	 * Limpia el objeto, liberando la memoria.
	 */
	void release();

#ifndef NDEBUG
	/**
	 * Atributo que indica si la fuente ha sido inicializada o no. Sólo está
	 * disponible en la compilación en modo depuración.
	 */
	bool _initialized;
#endif

	/**
	 * Información general de la fuente.
	 */
	tFont _fontInfo;

	/**
	 * Array dinámico con la información de cada símbolo con la información
	 * medida en píxeles
	 */
	tGlyph* _glyphs;

	/**
	 * Array dinámico con la información de cada símbolo con la información
	 * normalizada para hacerla independiente del tamaño de la textura.
	 */
	tGlyphRel* _glyphsRel;

	/**
	 * Array dinámico con punteros a la información de cada símbolo. Sirve para
	 * acelerar la búsqueda sin utilizar una tabla hash, bajo riesgo de perder
	 * un poco de memoria.
	 */
	tGlyph** _glyphsInd;

	/**
	 * Array dinámico con punteros a la información de cada símbolo con
	 * coordenadas normalizadas. Sirve para acelerar la búsqueda sin utilizar
	 * una tabla hash, bajo riesgo de perder un poco de memoria.
	 */
	tGlyphRel** _glyphsRelInd;

	/**
	 * Valor entero del primer símbolo que se almacena dentro de _glyphsInd
	 * y _glyphsRelInd.
	 */
	unsigned short _firstChar;

	/**
	 * Valor entero del último símbolo que se almacena dentro de _glyphsInd
	 * y _glyphsRelInd.
	 */
	unsigned short _lastChar;

}; // class CTextureFont

#endif
