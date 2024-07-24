//---------------------------------------------------------------------------
// MDLTexture.h
//---------------------------------------------------------------------------

/**
 * @file MDLTexture.h
 * Contiene la declaración de la clase que almacena la información sobre
 * una textura del modelo de Half Life.
 * @see HalfLifeMDL::MDLTexture
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */

#ifndef __HalfLifeMDL_MDLTexture
#define __HalfLifeMDL_MDLTexture

#include <string>
#include "MDLBasic.h"
#include "MDLDefinesDLLClass.h"

namespace HalfLifeMDL {

/**
 * Clase que almacena la información sobre una textura de un modelo de
 * Half Life.
 * <p>
 * A parte de almacenar la imagen, también almacena un <EM>tag</EM>. Es un
 * atributo del tipo de datos MDLTexture::Tag, que el objeto no utiliza para
 * nada, pero facilita la vida al usuario de los objetos de la clase.
 * <p>
 * La idea es que el usuario de esta librería establezca al tipo
 * MDLTexture::Tab como un alias al tipo que le interese, y que lo utilizará
 * para conocer esta textura. Por ejemplo, se puede guardar un entero para
 * almacenar el identificador de la textura una vez enviada a OpenGL, un
 * puntero a una superficie de Direct3D, o cualquier otra cosa relativa al
 * motor gráfico con el que se está pintando el modelo.<br>
 * Existen métodos para establecer y acceder al tag de la textura.
 * <p>
 * Ésta es una clase "de alto nivel" a la que el usuario del módulo tiene 
 * acceso. Está directamente relacionada con la estructura
 * HalfLifeMDL::texture_t utilizada en el fichero MDL.
 * @see HalfLifeMDL::texture_t
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 * @todo Enlace a la página general de la "arquitectura".
 * @todo Aún falta almacenar la parte referente a HalfLifeMDL::texture_t::flags
 * @todo Meter un <EM>tag</EM> para que el usuario pueda meter un identificador
 * a la librería gráfica o algo por el estilo.
 */
class MDL_PUBLIC MDLTexture {
public:

	/**
	 * Tipo de datos del <EM>tag</EM> de la textura. El tipo real del que
	 * éste es alias lo debería modificar el usuario según sus intereses.
	 */
	typedef void *Tag;

	/**
	 * Constructor de los objetos de la clase.
	 * @param infoTexture Estructura con la información general de la textura.
	 * @param rawTexture Información con la imagen y paleta de la textura, tal
	 * y como se almacena en el fichero MDL. Se hace copia de su contenido;
	 * es decir la liberación del puntero pasado como parámetro es 
	 * responsabilidad del usuario.
	 * @warning Si el segundo parámetro no tiene suficiente memoria
	 * inicializada para contener a la imagen de la textura indicada por
	 * el primer parámetro, el resultado será indefinido.
	 */
	MDLTexture(texture_t* infoTexture, MDLuchar* rawTexture);

	/**
	 * Destructor.
	 */
	~MDLTexture();

	/**
	 * Libera la memoria de la imagen de la textura. Los métodos que
	 * proporcionan la información de la imagen dejarán de funcionar.
	 * Los métodos para obtener sus propiedades (ancho y alto) seguirán
	 * funcionando. Este método es útil para liberar memoria si la textura
	 * es enviada a alguna librería gráfica que mantiene por sí misma
	 * la información de las imágenes.
	 */
	void freeImage();

	/**
	 * Devuelve el nombre de la textura. Típicamente es el nombre del fichero
	 * BMP a partir del que se leyó la imagen que se empaquetó en el archivo
	 * MDL del que se ha leído.
	 * @return Nombre de la textura.
	 */
	const std::string& getName() const { return _name; }

	/**
	 * Devuelve la altura de la textura.
	 * @return Altura de la textura.
	 */
	unsigned int getHeight() const { return _height; }

	/**
	 * Devuelve el ancho de la textura.
	 * @return Ancho de la textura.
	 */
	unsigned int getWidth() const { return _width; }

	/**
	 * Devuelve la imagen en formato RGB en un array que debe haber sido
	 * inicializado por el usuario. El array debe poder albergar toda
	 * la imagen, por lo que necesita un tamaño de altura*anchura*3.
	 * Cada elemento del vector será un flotante, donde un 1 indica la mayor
	 * intensidad de la componente del color, y un 0 la mínima. Los píxeles
	 * se proporcionan de izquierda a derecha, y de abajo a arriba.<p>
	 * Este método no debe ser llamado tras una invocación a freeImage().
	 * @param image Array inicializado por el usuario de tamaño suficiente,
	 * donde colocar los píxeles de la imagen.
	 * @return Cierto si todo fue bien (no se ha llamado a
	 * MDLTexture::freeImage).
	 */
	bool getRGBImage(float* image) const;

	/**
	 * Devuelve la imagen en formato RGB en un array que debe haber sido
	 * inicializado por el usuario. El array debe poder albergar toda
	 * la imagen, por lo que necesita un tamaño de altura*anchura*3.
	 * Cada elemento del vector será un unsigned char. Los píxeles
	 * se proporcionan de izquierda a derecha, y de abajo a arriba.<p>
	 * Este método no debe ser llamado tras una invocación a freeImage().
	 * @param image Array inicializado por el usuario de tamaño suficiente,
	 * donde colocar los píxeles de la imagen.
	 * @return Cierto si todo fue bien (no se ha llamado a
	 * MDLTexture::freeImage).
	 */
	bool getRGBImage(unsigned char* image) const;

	/**
	 * Devuelve el tamaño que tendría que tener la imagen para que
	 * sus dimensiones fueran potencias de dos, y se acercara
	 * lo más posible al tamaño original (por exceso).<p>
	 * @param  width Anchura de la imagen, es decir potencia de dos
	 * más cercana a la anchura original (mayor o igual a ella).
	 * @param height Altura de la imagen, es decir potencia de dos
	 * más cercana a la altura original (mayor o igual a ella).
	 */
	void getSizePowerOfTwo(unsigned int &width, unsigned int &height) const;

	/**
	 * Devuelve la imagen con las dimensiones potencia de dos en formato RGB 
	 * en un array que debe haber sido inicializado por el usuario. El array 
	 * debe poder albergar toda la imagen, por lo que necesita un tamaño 
	 * de altura*anchura*3, donde la altura y la anchura han sido obtenidas
	 * llamando a getSizePowerOfTwo.
	 * Cada elemento del vector será un flotante, donde un 1 indica la mayor
	 * intensidad de la componente del color, y un 0 la mínima. Los píxeles
	 * se proporcionan de izquierda a derecha, y de abajo a arriba.<p>
	 * Este método no debe ser llamado tras una invocación a freeImage().
	 * @param image Array inicializado por el usuario de tamaño suficiente,
	 * donde colocar los píxeles de la imagen.
	 * @return Cierto si todo fue bien (no se ha llamado a
	 * MDLTexture::freeImage).
	 */
	bool getPowerOfTwoRGBImage(float *image) const;

	/**
	 * Devuelve la imagen con las dimensiones potencia de dos en formato RGB 
	 * en un array que debe haber sido inicializado por el usuario. El array 
	 * debe poder albergar toda la imagen, por lo que necesita un tamaño 
	 * de altura*anchura*3, donde la altura y la anchura han sido obtenidas
	 * llamando a getSizePowerOfTwo.
	 * Cada elemento del vector será un unsigned char. Los píxeles
	 * se proporcionan de izquierda a derecha, y de abajo a arriba.<p>
	 * Este método no debe ser llamado tras una invocación a freeImage().
	 * @param image Array inicializado por el usuario de tamaño suficiente,
	 * donde colocar los píxeles de la imagen.
	 * @return Cierto si todo fue bien (no se ha llamado a
	 * MDLTexture::freeImage).
	 */
	bool getPowerOfTwoRGBImage(unsigned char *image) const;

	/**
	 * Devuelve la etiqueta del objeto. Es un valor que no se utiliza desde
	 * el objeto, y su único objetivo es que el usuario del objeto pueda
	 * mantener un dato que le dé una pista rápida de qué textura es esta
	 * dentro del motor usado para pintar el modelo.
	 * @return Etiqueta del objeto.
	 */
	Tag getTag() const { return _tag; }

	/**
	 * Establece la etiqueta del objeto. Es un valor que no se utiliza desde
	 * el objeto, y su único objetivo es que el usuario del objeto pueda
	 * mantener un dato que le dé una pista rápida de qué textura es esta
	 * dentro del motor usado para pintar el modelo.
	 * @param tag Nueva etiqueta a establecer.
	 */
	void setTag(Tag tag) { _tag = tag; }

protected:

	/**
	 * Nombre de la textura. Normalmente es el nombre del fichero del que
	 * se leyó la textura para empaquetarla en el fichero MDL.
	 */
	std::string _name;

	/**
	 * Alto de la textura.
	 */
	unsigned int _height;

	/**
	 * Ancho de la textura.
	 */
	unsigned int _width;

	/**
	 * Píxeles de la imagen indexados con la paleta. Hay un byte por cada
	 * pixel de la imagen, organizandola de izquierda a derecha y de abajo
	 * a arriba.
	 */
	MDLuchar* _image;

	/**
	 * Paleta de la textura. Está organizada en bloques de tres bytes, cada
	 * uno indicando una componente del color RGB en 8 bits sin signo. El
	 * primer bloque se refiere a las componentes del color 0, el segundo
	 * a las del color 1, etc.
	 */
	MDLuchar* _palette;

	/**
	 * Etiqueta del objeto. Es un valor que no se utiliza desde el objeto, y
	 * su único objetivo es que el usuario del objeto pueda mantener un dato
	 * que le dé una pista rápida de qué textura es esta dentro del motor
	 * usado para pintar el modelo.
	 */
	Tag _tag;


	/**
	 * Función para averiguar si un número es potencia de dos o no.<p>
	 * Se utiliza debido a que las texturas manejadas por los motores
	 * gráficos suelen permitir únicamente imágenes potencia de dos.
	 * @param value Entero del que se quiere averiguar si es o no
	 * potencia de dos.
	 * @return Devuelve cierto si el valor es potencia de dos.
	 */
	static bool isPowerOfTwo (int value) {
		return (value & -value) == value; }

private:
	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementación es vacía y no debe usarse.
	 */
	MDLTexture(const MDLTexture& source) {
		assert(!"Constructor copia MDLTexture no disponible"); }

	/**
	 * Operador de asignación privado para evitar problemas en los
	 * delete. Su implementación es vacía y no debe usarse.
	 */
	MDLTexture& operator=(const MDLTexture &source) {
		assert(!"Operador asignación en MDLTexture no disponible");
		return *this;}

}; // class MDLTexture

} // namespace HalfLifeMDL

#endif
