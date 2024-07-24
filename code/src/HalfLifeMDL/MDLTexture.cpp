//---------------------------------------------------------------------------
// MDLTexture.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLTexture.cpp
 * Contiene la definici�n (implementaci�n) de la clase MDLTexture que almacena
 * la informaci�n sobre una textura de un modelo de Half Life.
 * <p>
 * �sta es una clase "de alto nivel" a la que el usuario del m�dulo tiene 
 * acceso. Est� directamente relacionada con la estructura
 * HalfLifeMDL::texture_t utilizada en el fichero MDL.
 * @see HalfLifeMDL::texture_t
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 * @todo Enlace a la p�gina general de la "arquitectura".
 */

#include <assert.h>
#include <memory>

#include "MDLTexture.h"

namespace HalfLifeMDL {

/**
 * Constructor de los objetos de la clase.
 * @param infoTexture Estructura con la informaci�n general de la textura.
 * @param Informaci�n con la imagen y paleta de la textura, tal y como se
 * almacena en el fichero MDL. Se hace copia de su contenido; es decir la
 * liberaci�n del puntero pasado como par�metro es responsabilidad del
 * usuario.
 * @warning Si el segundo par�metro no tiene suficiente memoria
 * inicializada para contener a la imagen de la textura indicada por
 * el primer par�metro, el resultado ser� indefinido.
 */
MDLTexture::MDLTexture(texture_t* infoTexture, MDLuchar* rawTexture) {

	assert(infoTexture != NULL);
	assert(rawTexture != NULL);

	_height = infoTexture->height;
	_width = infoTexture->width;

	_name = infoTexture->name;

	// Pedimos memoria para la parte de datos. Aunque el objeto
	// almacena dos punteros, el bloque de datos ser� �nico.
	_image = new MDLuchar[_height*_width + 3*256];

	_palette = _image + _height*_width;

	memcpy(_image, rawTexture, _height*_width + 3*256);

} // Constructor

//---------------------------------------------------------------------------

/**
 * Destructor.
 */
MDLTexture::~MDLTexture() {
	if (_image != NULL)
		delete[] _image;

	// El puntero a _image contiene el bloque total, con la paleta y
	// la imagen, por lo que se libera _palette autom�ticamente.

} // Destructor

//---------------------------------------------------------------------------

/**
 * Libera la memoria de la imagen de la textura. Los m�todos que
 * proporcionan la informaci�n de la imagen dejar�n de funcionar.
 * Los m�todos para obtener sus propiedades (ancho y alto) seguir�n
 * funcionando. Este m�todo es �til para liberar memoria si la textura
 * es enviada a alguna librer�a gr�fica que mantiene por s� misma
 * la informaci�n de las im�genes.
 */
void MDLTexture::freeImage() {
	if (_image != NULL) {
		delete[] _image;
		// El puntero a _image contiene el bloque total, con la imagen
		// y la paleta, por lo que no es necesario liberar _palette.
		_image = NULL;
		_palette = NULL;
	}

} // freeImage

//---------------------------------------------------------------------------

/**
 * Devuelve la imagen en formato RGB en un array que debe haber sido
 * inicializado por el usuario. El array debe poder albergar toda
 * la imagen, por lo que necesita un tama�o de altura*anchura*3.
 * Cada elemento del vector ser� un flotante, donde un 1 indica la mayor
 * intensidad de la componente del color, y un 0 la m�nima. Los p�xeles
 * se proporcionan de izquierda a derecha, y de abajo a arriba.<p>
 * Este m�todo no debe ser llamado tras una invocaci�n a freeImage().
 * @param Array inicializado por el usuario de tama�o suficiente, donde
 * colocar los p�xeles de la imagen.
 * @return Cierto si todo fue bien (no se ha llamado a
 * MDLTexture::freeImage).
 */
bool MDLTexture::getRGBImage(float* image) const {

	unsigned int limite = _height * _width;
	MDLuchar* pixel = _image;

	if (_image == NULL)
		return false;

	for (int c = 0; c < limite; ++c, ++pixel) {
		*image = (float)_palette[*pixel * 3] / 255;
		++image;
		*image = (float)_palette[*pixel * 3 + 1] / 255;
		++image;
		*image = (float)_palette[*pixel * 3 + 2] / 255;
		++image;
	}

	return true;

} // getRGB(unsigned char)

//---------------------------------------------------------------------------

/**
 * Devuelve la imagen en formato RGB en un array que debe haber sido
 * inicializado por el usuario. El array debe poder albergar toda
 * la imagen, por lo que necesita un tama�o de altura*anchura*3.
 * Cada elemento del vector ser� un unsigned char. Los p�xeles
 * se proporcionan de izquierda a derecha, y de abajo a arriba.<p>
 * Este m�todo no debe ser llamado tras una invocaci�n a freeImage().
 * @param Array inicializado por el usuario de tama�o suficiente, donde
 * colocar los p�xeles de la imagen.
 * @return Cierto si todo fue bien (no se ha llamado a
 * MDLTexture::freeImage).
 */
bool MDLTexture::getRGBImage(unsigned char* image) const {

	unsigned int limite = _height * _width;
	MDLuchar* pixel = _image;

	if (_image == NULL)
		return false;

	for (int c = 0; c < limite; ++c, ++pixel) {
		*image = _palette[*pixel * 3] << (sizeof(unsigned char) - 1) * 8;
		++image;
		*image = _palette[*pixel * 3 + 1] << (sizeof(unsigned char) - 1) * 8;
		++image;
		*image = _palette[*pixel * 3 + 2] << (sizeof(unsigned char) - 1) * 8;
		++image;
	}

	return true;

} // getRGB(unsigned char)

//---------------------------------------------------------------------------

/**
 * Devuelve el tama�o que tendr�a que tener la imagen para que
 * sus dimensiones fueran potencias de dos, y se acercara
 * lo m�s posible al tama�o original (por exceso).<p>
 * @param  width Anchura de la imagen, es decir potencia de dos
 * m�s cercana a la anchura original (mayor o igual a ella).
 * @param height Altura de la imagen, es decir potencia de dos
 * m�s cercana a la altura original (mayor o igual a ella).
 */
void MDLTexture::getSizePowerOfTwo(unsigned int &width, 
								   unsigned int &height) const {

	for (width = 1; width < _width; width <<= 1);

	for (height = 1; height < _height; height <<= 1);
	
} // getSizePowerOfTwo

//---------------------------------------------------------------------------

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
bool MDLTexture::getPowerOfTwoRGBImage(float *image) const {

	if (isPowerOfTwo(_width) && isPowerOfTwo(_height))
		return getRGBImage(image);

	if (_image == NULL)
		return false;

	int	i, j;
	unsigned int	alturasalida, anchurasalida;
	int	*row1, *row2, *col1, *col2;
	float *out = image;
	unsigned char	*pix1, *pix2, *pix3, *pix4;

	getSizePowerOfTwo(anchurasalida, alturasalida);

	row1 = new int[alturasalida];
	row2 = new int[alturasalida];
	col1 = new int[anchurasalida];
	col2 = new int[anchurasalida];

	// Rellenamos las matrices rowX y colX
	for (i = 0; i < anchurasalida; i++) {
		col1[i] = (int) ((i + 0.25) * (_width / (float)anchurasalida));
		col2[i] = (int) ((i + 0.75) * (_width / (float)anchurasalida));
	}
	for (i = 0; i < alturasalida; i++) {
		row1[i] = (int) ((i + 0.25) * (_height / (float)alturasalida)) *
																	_width;
		row2[i] = (int) ((i + 0.75) * (_height / (float)alturasalida)) * 
																	_width;
	}

	// Escalado y conversi�n a RGB
	for (i = 0; i < alturasalida; i++)
		for (j = 0; j < anchurasalida; j++, out += 3) {
			pix1 = &_palette[_image[row1[i] + col1[j]] * 3];
			pix2 = &_palette[_image[row1[i] + col2[j]] * 3];
			pix3 = &_palette[_image[row2[i] + col1[j]] * 3];
			pix4 = &_palette[_image[row2[i] + col2[j]] * 3];

			out[0] = (float)((pix1[0] + pix2[0] + pix3[0] + pix4[0]) >> 2) 
																		/ 255;
			out[1] = (float)((pix1[1] + pix2[1] + pix3[1] + pix4[1]) >> 2)
																		/ 255;
			out[2] = (float)((pix1[2] + pix2[2] + pix3[2] + pix4[2]) >> 2)
																		/ 255;
		}

	delete(row1); delete(row2);
	delete(col1); delete(col2);
	return true;

} // getPowerOfTwoRGBImage(float *image)

//---------------------------------------------------------------------------

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
bool MDLTexture::getPowerOfTwoRGBImage(unsigned char *image) const {

	if (isPowerOfTwo(_width) && isPowerOfTwo(_height))
		return getRGBImage(image);

	if (_image == NULL)
		return false;

	int	i, j;
	unsigned int	alturasalida, anchurasalida;
	int	*row1, *row2, *col1, *col2;
	unsigned char	*out = image;
	unsigned char	*pix1, *pix2, *pix3, *pix4;

	getSizePowerOfTwo(anchurasalida, alturasalida);

	row1 = new int[alturasalida];
	row2 = new int[alturasalida];
	col1 = new int[anchurasalida];
	col2 = new int[anchurasalida];

	// Rellenamos las matrices rowX y colX
	for (i = 0; i < anchurasalida; i++) {
		col1[i] = (int) ((i + 0.25) * (_width / (float)anchurasalida));
		col2[i] = (int) ((i + 0.75) * (_width / (float)anchurasalida));
	}
	for (i = 0; i < alturasalida; i++) {
		row1[i] = (int) ((i + 0.25) * (_height / (float)alturasalida)) *
																	_width;
		row2[i] = (int) ((i + 0.75) * (_height / (float)alturasalida)) * 
																	_width;
	}

	// Escalado y conversi�n a RGB
	for (i = 0; i < alturasalida; i++)
		for (j = 0; j < anchurasalida; j++, out += 3) {
			pix1 = &_palette[_image[row1[i] + col1[j]] * 3];
			pix2 = &_palette[_image[row1[i] + col2[j]] * 3];
			pix3 = &_palette[_image[row2[i] + col1[j]] * 3];
			pix4 = &_palette[_image[row2[i] + col2[j]] * 3];

			out[0] = ((int)pix1[0] + (int)pix2[0] +
								(int)pix3[0] + (int)pix4[0]) >> 2;
			out[1] = ((int)pix1[1] + (int)pix2[1] +
								(int)pix3[1] + (int)pix4[1]) >> 2;
			out[2] = ((int)pix1[2] + (int)pix2[2] +
								(int)pix3[2] + (int)pix4[2]) >> 2;
		}

	delete(row1); delete(row2);
	delete(col1); delete(col2);
	return true;

} // getPowerOfTwoRGBImage(unsigned char * image)

} // namespace HalfLifeMDL

