//---------------------------------------------------------------------------
// MDLTexture.h
//---------------------------------------------------------------------------

/**
 * @file MDLTexture.h
 * Contiene la declaraci�n de la clase que almacena la informaci�n sobre
 * una textura del modelo de Half Life.
 * @see HalfLifeMDL::MDLTexture
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */

#ifndef __HalfLifeMDL_MDLTexture
#define __HalfLifeMDL_MDLTexture

#include <string>
#include "MDLBasic.h"
#include "MDLDefinesDLLClass.h"

namespace HalfLifeMDL {

/**
 * Clase que almacena la informaci�n sobre una textura de un modelo de
 * Half Life.
 * <p>
 * A parte de almacenar la imagen, tambi�n almacena un <EM>tag</EM>. Es un
 * atributo del tipo de datos MDLTexture::Tag, que el objeto no utiliza para
 * nada, pero facilita la vida al usuario de los objetos de la clase.
 * <p>
 * La idea es que el usuario de esta librer�a establezca al tipo
 * MDLTexture::Tab como un alias al tipo que le interese, y que lo utilizar�
 * para conocer esta textura. Por ejemplo, se puede guardar un entero para
 * almacenar el identificador de la textura una vez enviada a OpenGL, un
 * puntero a una superficie de Direct3D, o cualquier otra cosa relativa al
 * motor gr�fico con el que se est� pintando el modelo.<br>
 * Existen m�todos para establecer y acceder al tag de la textura.
 * <p>
 * �sta es una clase "de alto nivel" a la que el usuario del m�dulo tiene 
 * acceso. Est� directamente relacionada con la estructura
 * HalfLifeMDL::texture_t utilizada en el fichero MDL.
 * @see HalfLifeMDL::texture_t
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 * @todo Enlace a la p�gina general de la "arquitectura".
 * @todo A�n falta almacenar la parte referente a HalfLifeMDL::texture_t::flags
 * @todo Meter un <EM>tag</EM> para que el usuario pueda meter un identificador
 * a la librer�a gr�fica o algo por el estilo.
 */
class MDL_PUBLIC MDLTexture {
public:

	/**
	 * Tipo de datos del <EM>tag</EM> de la textura. El tipo real del que
	 * �ste es alias lo deber�a modificar el usuario seg�n sus intereses.
	 */
	typedef void *Tag;

	/**
	 * Constructor de los objetos de la clase.
	 * @param infoTexture Estructura con la informaci�n general de la textura.
	 * @param rawTexture Informaci�n con la imagen y paleta de la textura, tal
	 * y como se almacena en el fichero MDL. Se hace copia de su contenido;
	 * es decir la liberaci�n del puntero pasado como par�metro es 
	 * responsabilidad del usuario.
	 * @warning Si el segundo par�metro no tiene suficiente memoria
	 * inicializada para contener a la imagen de la textura indicada por
	 * el primer par�metro, el resultado ser� indefinido.
	 */
	MDLTexture(texture_t* infoTexture, MDLuchar* rawTexture);

	/**
	 * Destructor.
	 */
	~MDLTexture();

	/**
	 * Libera la memoria de la imagen de la textura. Los m�todos que
	 * proporcionan la informaci�n de la imagen dejar�n de funcionar.
	 * Los m�todos para obtener sus propiedades (ancho y alto) seguir�n
	 * funcionando. Este m�todo es �til para liberar memoria si la textura
	 * es enviada a alguna librer�a gr�fica que mantiene por s� misma
	 * la informaci�n de las im�genes.
	 */
	void freeImage();

	/**
	 * Devuelve el nombre de la textura. T�picamente es el nombre del fichero
	 * BMP a partir del que se ley� la imagen que se empaquet� en el archivo
	 * MDL del que se ha le�do.
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
	 * la imagen, por lo que necesita un tama�o de altura*anchura*3.
	 * Cada elemento del vector ser� un flotante, donde un 1 indica la mayor
	 * intensidad de la componente del color, y un 0 la m�nima. Los p�xeles
	 * se proporcionan de izquierda a derecha, y de abajo a arriba.<p>
	 * Este m�todo no debe ser llamado tras una invocaci�n a freeImage().
	 * @param image Array inicializado por el usuario de tama�o suficiente,
	 * donde colocar los p�xeles de la imagen.
	 * @return Cierto si todo fue bien (no se ha llamado a
	 * MDLTexture::freeImage).
	 */
	bool getRGBImage(float* image) const;

	/**
	 * Devuelve la imagen en formato RGB en un array que debe haber sido
	 * inicializado por el usuario. El array debe poder albergar toda
	 * la imagen, por lo que necesita un tama�o de altura*anchura*3.
	 * Cada elemento del vector ser� un unsigned char. Los p�xeles
	 * se proporcionan de izquierda a derecha, y de abajo a arriba.<p>
	 * Este m�todo no debe ser llamado tras una invocaci�n a freeImage().
	 * @param image Array inicializado por el usuario de tama�o suficiente,
	 * donde colocar los p�xeles de la imagen.
	 * @return Cierto si todo fue bien (no se ha llamado a
	 * MDLTexture::freeImage).
	 */
	bool getRGBImage(unsigned char* image) const;

	/**
	 * Devuelve el tama�o que tendr�a que tener la imagen para que
	 * sus dimensiones fueran potencias de dos, y se acercara
	 * lo m�s posible al tama�o original (por exceso).<p>
	 * @param  width Anchura de la imagen, es decir potencia de dos
	 * m�s cercana a la anchura original (mayor o igual a ella).
	 * @param height Altura de la imagen, es decir potencia de dos
	 * m�s cercana a la altura original (mayor o igual a ella).
	 */
	void getSizePowerOfTwo(unsigned int &width, unsigned int &height) const;

	/**
	 * Devuelve la imagen con las dimensiones potencia de dos en formato RGB 
	 * en un array que debe haber sido inicializado por el usuario. El array 
	 * debe poder albergar toda la imagen, por lo que necesita un tama�o 
	 * de altura*anchura*3, donde la altura y la anchura han sido obtenidas
	 * llamando a getSizePowerOfTwo.
	 * Cada elemento del vector ser� un flotante, donde un 1 indica la mayor
	 * intensidad de la componente del color, y un 0 la m�nima. Los p�xeles
	 * se proporcionan de izquierda a derecha, y de abajo a arriba.<p>
	 * Este m�todo no debe ser llamado tras una invocaci�n a freeImage().
	 * @param image Array inicializado por el usuario de tama�o suficiente,
	 * donde colocar los p�xeles de la imagen.
	 * @return Cierto si todo fue bien (no se ha llamado a
	 * MDLTexture::freeImage).
	 */
	bool getPowerOfTwoRGBImage(float *image) const;

	/**
	 * Devuelve la imagen con las dimensiones potencia de dos en formato RGB 
	 * en un array que debe haber sido inicializado por el usuario. El array 
	 * debe poder albergar toda la imagen, por lo que necesita un tama�o 
	 * de altura*anchura*3, donde la altura y la anchura han sido obtenidas
	 * llamando a getSizePowerOfTwo.
	 * Cada elemento del vector ser� un unsigned char. Los p�xeles
	 * se proporcionan de izquierda a derecha, y de abajo a arriba.<p>
	 * Este m�todo no debe ser llamado tras una invocaci�n a freeImage().
	 * @param image Array inicializado por el usuario de tama�o suficiente,
	 * donde colocar los p�xeles de la imagen.
	 * @return Cierto si todo fue bien (no se ha llamado a
	 * MDLTexture::freeImage).
	 */
	bool getPowerOfTwoRGBImage(unsigned char *image) const;

	/**
	 * Devuelve la etiqueta del objeto. Es un valor que no se utiliza desde
	 * el objeto, y su �nico objetivo es que el usuario del objeto pueda
	 * mantener un dato que le d� una pista r�pida de qu� textura es esta
	 * dentro del motor usado para pintar el modelo.
	 * @return Etiqueta del objeto.
	 */
	Tag getTag() const { return _tag; }

	/**
	 * Establece la etiqueta del objeto. Es un valor que no se utiliza desde
	 * el objeto, y su �nico objetivo es que el usuario del objeto pueda
	 * mantener un dato que le d� una pista r�pida de qu� textura es esta
	 * dentro del motor usado para pintar el modelo.
	 * @param tag Nueva etiqueta a establecer.
	 */
	void setTag(Tag tag) { _tag = tag; }

protected:

	/**
	 * Nombre de la textura. Normalmente es el nombre del fichero del que
	 * se ley� la textura para empaquetarla en el fichero MDL.
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
	 * P�xeles de la imagen indexados con la paleta. Hay un byte por cada
	 * pixel de la imagen, organizandola de izquierda a derecha y de abajo
	 * a arriba.
	 */
	MDLuchar* _image;

	/**
	 * Paleta de la textura. Est� organizada en bloques de tres bytes, cada
	 * uno indicando una componente del color RGB en 8 bits sin signo. El
	 * primer bloque se refiere a las componentes del color 0, el segundo
	 * a las del color 1, etc.
	 */
	MDLuchar* _palette;

	/**
	 * Etiqueta del objeto. Es un valor que no se utiliza desde el objeto, y
	 * su �nico objetivo es que el usuario del objeto pueda mantener un dato
	 * que le d� una pista r�pida de qu� textura es esta dentro del motor
	 * usado para pintar el modelo.
	 */
	Tag _tag;


	/**
	 * Funci�n para averiguar si un n�mero es potencia de dos o no.<p>
	 * Se utiliza debido a que las texturas manejadas por los motores
	 * gr�ficos suelen permitir �nicamente im�genes potencia de dos.
	 * @param value Entero del que se quiere averiguar si es o no
	 * potencia de dos.
	 * @return Devuelve cierto si el valor es potencia de dos.
	 */
	static bool isPowerOfTwo (int value) {
		return (value & -value) == value; }

private:
	/**
	 * Constructor copia privado para evitar problemas en los delete.
	 * Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLTexture(const MDLTexture& source) {
		assert(!"Constructor copia MDLTexture no disponible"); }

	/**
	 * Operador de asignaci�n privado para evitar problemas en los
	 * delete. Su implementaci�n es vac�a y no debe usarse.
	 */
	MDLTexture& operator=(const MDLTexture &source) {
		assert(!"Operador asignaci�n en MDLTexture no disponible");
		return *this;}

}; // class MDLTexture

} // namespace HalfLifeMDL

#endif
