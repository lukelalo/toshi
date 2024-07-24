//---------------------------------------------------------------------------
// MDLPoint.h
//---------------------------------------------------------------------------

/**
 * @file MDLPoint.h
 * Contiene la declaración de la clase que almacena la información sobre
 * un punto (vértice) o una dirección (normal) de un modelo de Half Life.
 * @see HalfLifeMDL::MDLPoint
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLPoint
#define __HalfLifeMDL_MDLPoint

#include <assert.h>
#include <math.h>
#include "MDLBasicType.h"

#include "MDLDefinesDLLClass.h"

#ifdef _WIN32
#pragma warning( disable : 4244)	// Warning de conversión de double a float
#endif

namespace HalfLifeMDL {

/**
 * Clase que almacena la información sobre un punto u orientación (vértice o
 * normal) de un modelo de Half Life..
 * <p>
 * Ésta es una clase "de alto nivel" a la que el usuario del módulo tiene 
 * acceso. Está directamente relacionada con la estructura
 * HalfLifeMDL::vec3_t utilizada en el fichero MDL.
 * @see HalfLifeMDL::vec3_t
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 * @todo Enlace a la página general de la "arquitectura".
 */
class MDL_PUBLIC MDLPoint {
public:

	/**
	 * Constructor a partir de las coordenadas de los parámetros.
	 * param x Coordenada x del punto. Valor 0 por defecto.
	 * param y Coordenada y del punto. Valor 0 por defecto.
	 * param z Coordenada z del punto. Valor 0 por defecto.
	 */
	MDLPoint(MDLBasicType x = 0, MDLBasicType y = 0, MDLBasicType z = 0) {
					_coords[0] = x;
					_coords[1] = y;
					_coords[2] = z;	}

	/**
	 * Devuelve el valor de la coordenada X
	 * @return Coordenada x.
	 */
	MDLBasicType x() const { return _coords[0]; }

	/**
	 * Devuelve una referencia a la coordenada X.
	 * @return Coordenadad x.
	 */
	MDLBasicType& x() { return _coords[0]; }

	/**
	 * Devuelve el valor de la coordenada Y
	 * @return Coordenadad y.
	 */
	MDLBasicType y() const { return _coords[1]; }

	/**
	 * Devuelve una referencia a la coordenada Y.
	 * @return Coordenadad y.
	 */
	MDLBasicType& y() { return _coords[1]; }

	/**
	 * Devuelve el valor de la coordenada Z.
	 * @return Coordenadad z.
	 */
	MDLBasicType z() const { return _coords[2]; }

	/**
	 * Devuelve una referencia a la coordenada Z.
	 * @return Coordenadad z.
	 */
	MDLBasicType& z() { return _coords[2]; }

	/**
	 * Devuelve la dimensión del punto solicitada. Se supone
	 * que 0 es la coordenada X, 1 la Y, y 2 la Z.
	 * @return Valor de la coordenada solicitada.
	 */
	MDLBasicType& operator[](int v) {
					assert((v >= 0) && (v <= 2));
					return _coords[v]; }

	/**
	 * Devuelve la dimensión del punto solicitada. Se supone
	 * que 0 es la coordenada X, 1 la Y, y 2 la Z.
	 * @return Valor de la coordenada solicitada.
	 */
	MDLBasicType operator[](int v) const {
					assert((v >= 0) && (v <= 2));
					return _coords[v]; }

	/**
	 * Establece las tres coordenadas del punto.
	 * @param x Nueva coordenada x a establecer.
	 * @param y Nueva coordenada y a establecer.
	 * @param z Nueva coordenada z a establecer.
	 */
	void set(MDLBasicType x, MDLBasicType y, MDLBasicType z) {
					_coords[0] = x; _coords[1] = y; _coords[2] = z; }

	/**
	 * Devuelve el producto escalar del punto actual y otro, considerados
	 * ambos como vectores.
	 * @param v2 Segundo vértice para el producto escalar.
	 * @return this·v2  (producto escalar).
	 */
	MDLBasicType dotProduct(const MDLPoint& v2) const {
					return (_coords[0] * v2._coords[0] + 
							_coords[1] * v2._coords[1] + 
							_coords[2] * v2._coords[2]); }

	/**
	 * Devuelve el producto escalar del punto actual y otro proporcionado
	 * en un array en lugar de en otro MDLPoint, considerados ambos como
	 * vectores.
	 * @param v2 Segundo vector para el producto escalar. Es un array
	 * que debe tener al menos tres elementos (coordenadas x,y,z).
	 * @return this·v2  (producto escalar).
	 */
	MDLBasicType dotProduct(const MDLBasicType *v2) const {
					return (_coords[0] * v2[0] + 
							_coords[1] * v2[1] + 
							_coords[2] * v2[2]); }

	/**
	 * Devuelve la suma de dos objetos de esta clase, considerados como
	 * vectores.
	 * @param v2 Segundo vértice a sumar.
	 * @return this + v2 (suma de vectores).
	 */
	MDLPoint operator+(const MDLPoint& v2) const {
					MDLPoint result;
					result._coords[0] = _coords[0] + v2._coords[0];
					result._coords[1] = _coords[1] + v2._coords[1];
					result._coords[2] = _coords[2] + v2._coords[2];
					return result; }

	/**
	 * Devuelve la suma del punto actual y otro proporcionado en un array
	 * en lugar de en otro MDLPoint.
	 * @param v2 Segundo vector para la suma. Es un array que debe tener al
	 * menos tres elementos (coordenadas x,y,z).
	 */
	MDLPoint operator+(const MDLBasicType *v2) const {
					MDLPoint result;
					result._coords[0] = _coords[0] + v2[0];
					result._coords[1] = _coords[0] + v2[1];
					result._coords[2] = _coords[0] + v2[2];
					return result; }

	/**
	 * Devuelve el vector multiplicado por un escalar.
	 * @param value Valor escalar por el que multiplicar el punto.
	 * @return El punto resultado de multiplicar el actual por el
	 * escalar pasado como parámetro.
	 */
	MDLPoint operator*(float value) const {
					MDLPoint result;
					result._coords[0] = _coords[0] * value;
					result._coords[1] = _coords[1] * value;
					result._coords[2] = _coords[2] * value;
					return result; }

	/**
	 * Devuelve el módulo del punto (más bien del vector) almacenado en el
	 * objeto.
	 * @return Módulo del objeto.
	 */
	MDLBasicType module() const {
					return sqrt(
						_coords[0] * _coords[0]  + 
						_coords[1] * _coords[1]  +
						_coords[2] * _coords[2]); }

	bool operator==(const MDLPoint& v2) const {
					return ( (_coords[0] == v2._coords[0]) && 
							(_coords[1] == v2._coords[1]) && 
							(_coords[2] == v2._coords[2]));  }

protected:

	/**
	 * Coordenadas del punto.
	 */
	MDLBasicType _coords[3];

}; // class MDLPoint

} // namespace HalfLifeMDL

#endif
