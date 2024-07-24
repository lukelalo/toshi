//---------------------------------------------------------------------------
// MDLMatrix.h
//---------------------------------------------------------------------------

/**
 * @file MDLMatrix.h
 * Contiene la declaración de la clase que almacena una matriz de
 * transformación.
 * @see HalfLifeMDL::MDLMatrix
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLMatrix
#define __HalfLifeMDL_MDLMatrix

#include <assert.h>
#include "MDLBasicType.h"
#include "MDLPoint.h"

namespace HalfLifeMDL {

/**
 * Tipo que facilita la implementación de la clase MDLMatrix (especialmente
 * la sobreescritura del operador de cast de tipos).
 * @see HalfLifeMDL::MDLMatrix
 */
typedef MDLBasicType matrix_t[3][4];

/**
 * Clase que almacena la información sobre una matriz de transformación.
 * Es una clase interna, que no tendrá que ser utilizada en ningún momento
 * por los usuarios del namespace. Se usa para almacenar las matrices
 * de transformación que hay que aplicar a los vértices en función de la 
 * posición y horientación del hueso del que dependen. Se calculan a partir
 * de la posición de los huesos especificados por el fotograma de la secuencia
 * actual. <p>
 * El tipo básico para cada elemento de la matriz es
 * HalfLifeMDL::MDLBasicType, que puede ser redefinido sin que eso conlleve
 * problemas de compilación. Naturalmente no debería utilizarse un tipo
 * demasiado restringido para él si se desean obtener resultados razonables.
 * <p>
 * La matriz no es de 4x4, sino de 3x4. La información sobre la fila de la
 * proyección se obvia, y se supone que está a cero, salvo el último elemento
 * que será uno. Las tres primeras columnas contienen la matriz de
 * rotación; la última contiene la traslación originada por la matriz.
 * <p>
 * Gracias a la sobreescritura del cast de tipos, es posible acceder a los
 * datos de la matriz con la notación tradicional de C, sin necesidad de
 * solicitarle los datos a la clase a través de un método:
 *
 * @include HalfLifeMDL/castDeTipos.cpp
 *
 * Esta funcionalidad tiene, sin embargo, una pega: no puede utilizarse para
 * matrices constantes, pues la conversión de tipos genera una matriz cuyos
 * valores pueden modificarse. Se podría haber implementado una segunda
 * conversión de tipos que devolviera una matriz constante, pero eso ocasiona
 * errores en la compilación por llamada al operador[] ambiguo cuando la
 * matriz no es constante. Por tanto, a pesar de todo se ha implementado el
 * método getValue(int, int) que es válido también para matrices constantes.
 *
 * @include HalfLifeMDL/castDeTiposConst.cpp
 *
 * @see HalfLifeMDL::MDLBasicType
 * @see HalfLifeMDL::MDLPoint
 * @see HalfLifeMDL::MDLQuaternion
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 * @todo Enlace a la página general de la "arquitectura".
 */
class MDLMatrix {
public:

	/**
	 * Operador suma. Devuelve la suma de la matriz actual y la pasada como
	 * parámetro.
	 * @param operand Segundo operando de la suma (el primero es la matriz
	 * actual).
	 * @return Suma de ambas matrices.
	 */
	MDLMatrix operator+(const MDLMatrix& operand) {
				MDLMatrix result;
				add(operand, result);
				return result; }

	/**
	 * Suma dos matrices. Puede considerarse más óptima que la implementada
	 * por el operador suma porque deposita el resultado en un parámetro de
	 * salida, lo que evita la copia posterior. La desventaja es que su uso
	 * es más incómodo.
	 * @param operand Segundo operando de la suma (el primero es la matriz
	 * actual).
	 * @param result Matriz donde se despositará el resultado.
	 * @return Devuelve la matriz resultado pasada como segundo parámetro.
	 */
	MDLMatrix& add(const MDLMatrix& operand, MDLMatrix& result) {
				for (int y = 0; y < 3; y++)
					for (int x = 0; x < 4; x++)
						result[y][x] = _data[y][x] + operand.getValue(y,x);
				return result; }
	
	/**
	 * Cast de tipos que permite acceder a los datos de la matriz con
	 * la notación habitual en C/C++.
	 * A continuación se muestra un ejemplo de lo que se logra con
	 * esta implementación.
	 * @include HalfLifeMDL/castDeTipos.cpp
	 * @return Matriz guardada por el objeto.
	 */
	operator matrix_t&() { return _data; }

	/**
	 * Proporciona acceso a los valores del array para matrices constantes.
	 * Para matrices no constantes, es equivalente a
	 * <code>matriz[x][y]</code>.
	 * @param x Primera coordenada.
	 * @param y Segunda coordenada.
	 * @return La posición [x][y] de la matriz contenida por el objeto.
	 */
	MDLBasicType getValue(int x, int y) const { return _data[x][y]; }

	/**
	 * Multiplica dos matrices. Supone que la última fila de las dos matrices
	 * es [0 0 0 1].
	 * @param op2 Multiplicador del producto de matrices.
	 * @return Matriz resultado de multiplicar la actual por la del
	 * parámetro.
	 */
	MDLMatrix operator*(const MDLMatrix& op2) {
				MDLMatrix result;
				multiply(op2, result);
				return result; }

	/**
	 * Multiplica dos matrices. Puede considerarse más óptima que la
	 * implementada por el operador multiplicación porque deposita el
	 * resultado en un parámetro de salida, lo que evita la copia posterior.
	 * La desventaja es que su uso es más incómodo.
	 * @param op2 Segundo operando de la multiplicacion (el primero es
	 * la matriz actual).
	 * @param result Matriz donde se despositará el resultado.
	 * @return Devuelve la matriz resultado pasada como segundo parámetro.
	 */
	MDLMatrix& multiply(const MDLMatrix& op2, MDLMatrix& result);

	/**
	 * Multiplica la matriz actual por un punto o vértice, devolviendo el
	 * vector transformado.
	 * @param op2 Punto (vértice) a transformar.
	 * @return Vértice transformado mediante la matriz de transformación
	 * actual.
	 */
	MDLPoint operator*(const MDLPoint& op2) {
				MDLPoint result;
				multiply(op2, result);
				return result; }

	/**
	 * Multiplica la matriz actual por un punto o vértice, devolviendo el
	 * vector transformado. Puede considerarse más óptima que la implementada
	 * por el operador multiplicación porque desposita el resultado en un 
	 * parámetro de salida, lo que evita la copia posterior. La desventaja
	 * es que su uso es más incómodo.
	 * @param op2 Punto (vértice) a transformar.
	 * @param result Punto / vértice donde despositar el resultado.
	 * @return Devuelve el vértice pasado como segundo parámetro.
	 */
	MDLPoint& multiply(const MDLPoint& op2, MDLPoint& result) {
				result[0] = op2.dotProduct(_data[0]) + _data[0][3];
				result[1] = op2.dotProduct(_data[1]) + _data[1][3];
				result[2] = op2.dotProduct(_data[2]) + _data[2][3];
				return result; }

	// Faltan los constructores y demás, los cuaterniones...

protected:

	/**
	 * Datos de la matriz.
	 */
	matrix_t _data;

}; // class MDLMatrix

} // namespace HalfLifeMDL

#endif
