//---------------------------------------------------------------------------
// MDLMatrix.h
//---------------------------------------------------------------------------

/**
 * @file MDLMatrix.h
 * Contiene la declaraci�n de la clase que almacena una matriz de
 * transformaci�n.
 * @see HalfLifeMDL::MDLMatrix
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLMatrix
#define __HalfLifeMDL_MDLMatrix

#include <assert.h>
#include "MDLBasicType.h"
#include "MDLPoint.h"

namespace HalfLifeMDL {

/**
 * Tipo que facilita la implementaci�n de la clase MDLMatrix (especialmente
 * la sobreescritura del operador de cast de tipos).
 * @see HalfLifeMDL::MDLMatrix
 */
typedef MDLBasicType matrix_t[3][4];

/**
 * Clase que almacena la informaci�n sobre una matriz de transformaci�n.
 * Es una clase interna, que no tendr� que ser utilizada en ning�n momento
 * por los usuarios del namespace. Se usa para almacenar las matrices
 * de transformaci�n que hay que aplicar a los v�rtices en funci�n de la 
 * posici�n y horientaci�n del hueso del que dependen. Se calculan a partir
 * de la posici�n de los huesos especificados por el fotograma de la secuencia
 * actual. <p>
 * El tipo b�sico para cada elemento de la matriz es
 * HalfLifeMDL::MDLBasicType, que puede ser redefinido sin que eso conlleve
 * problemas de compilaci�n. Naturalmente no deber�a utilizarse un tipo
 * demasiado restringido para �l si se desean obtener resultados razonables.
 * <p>
 * La matriz no es de 4x4, sino de 3x4. La informaci�n sobre la fila de la
 * proyecci�n se obvia, y se supone que est� a cero, salvo el �ltimo elemento
 * que ser� uno. Las tres primeras columnas contienen la matriz de
 * rotaci�n; la �ltima contiene la traslaci�n originada por la matriz.
 * <p>
 * Gracias a la sobreescritura del cast de tipos, es posible acceder a los
 * datos de la matriz con la notaci�n tradicional de C, sin necesidad de
 * solicitarle los datos a la clase a trav�s de un m�todo:
 *
 * @include HalfLifeMDL/castDeTipos.cpp
 *
 * Esta funcionalidad tiene, sin embargo, una pega: no puede utilizarse para
 * matrices constantes, pues la conversi�n de tipos genera una matriz cuyos
 * valores pueden modificarse. Se podr�a haber implementado una segunda
 * conversi�n de tipos que devolviera una matriz constante, pero eso ocasiona
 * errores en la compilaci�n por llamada al operador[] ambiguo cuando la
 * matriz no es constante. Por tanto, a pesar de todo se ha implementado el
 * m�todo getValue(int, int) que es v�lido tambi�n para matrices constantes.
 *
 * @include HalfLifeMDL/castDeTiposConst.cpp
 *
 * @see HalfLifeMDL::MDLBasicType
 * @see HalfLifeMDL::MDLPoint
 * @see HalfLifeMDL::MDLQuaternion
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 * @todo Enlace a la p�gina general de la "arquitectura".
 */
class MDLMatrix {
public:

	/**
	 * Operador suma. Devuelve la suma de la matriz actual y la pasada como
	 * par�metro.
	 * @param operand Segundo operando de la suma (el primero es la matriz
	 * actual).
	 * @return Suma de ambas matrices.
	 */
	MDLMatrix operator+(const MDLMatrix& operand) {
				MDLMatrix result;
				add(operand, result);
				return result; }

	/**
	 * Suma dos matrices. Puede considerarse m�s �ptima que la implementada
	 * por el operador suma porque deposita el resultado en un par�metro de
	 * salida, lo que evita la copia posterior. La desventaja es que su uso
	 * es m�s inc�modo.
	 * @param operand Segundo operando de la suma (el primero es la matriz
	 * actual).
	 * @param result Matriz donde se despositar� el resultado.
	 * @return Devuelve la matriz resultado pasada como segundo par�metro.
	 */
	MDLMatrix& add(const MDLMatrix& operand, MDLMatrix& result) {
				for (int y = 0; y < 3; y++)
					for (int x = 0; x < 4; x++)
						result[y][x] = _data[y][x] + operand.getValue(y,x);
				return result; }
	
	/**
	 * Cast de tipos que permite acceder a los datos de la matriz con
	 * la notaci�n habitual en C/C++.
	 * A continuaci�n se muestra un ejemplo de lo que se logra con
	 * esta implementaci�n.
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
	 * @return La posici�n [x][y] de la matriz contenida por el objeto.
	 */
	MDLBasicType getValue(int x, int y) const { return _data[x][y]; }

	/**
	 * Multiplica dos matrices. Supone que la �ltima fila de las dos matrices
	 * es [0 0 0 1].
	 * @param op2 Multiplicador del producto de matrices.
	 * @return Matriz resultado de multiplicar la actual por la del
	 * par�metro.
	 */
	MDLMatrix operator*(const MDLMatrix& op2) {
				MDLMatrix result;
				multiply(op2, result);
				return result; }

	/**
	 * Multiplica dos matrices. Puede considerarse m�s �ptima que la
	 * implementada por el operador multiplicaci�n porque deposita el
	 * resultado en un par�metro de salida, lo que evita la copia posterior.
	 * La desventaja es que su uso es m�s inc�modo.
	 * @param op2 Segundo operando de la multiplicacion (el primero es
	 * la matriz actual).
	 * @param result Matriz donde se despositar� el resultado.
	 * @return Devuelve la matriz resultado pasada como segundo par�metro.
	 */
	MDLMatrix& multiply(const MDLMatrix& op2, MDLMatrix& result);

	/**
	 * Multiplica la matriz actual por un punto o v�rtice, devolviendo el
	 * vector transformado.
	 * @param op2 Punto (v�rtice) a transformar.
	 * @return V�rtice transformado mediante la matriz de transformaci�n
	 * actual.
	 */
	MDLPoint operator*(const MDLPoint& op2) {
				MDLPoint result;
				multiply(op2, result);
				return result; }

	/**
	 * Multiplica la matriz actual por un punto o v�rtice, devolviendo el
	 * vector transformado. Puede considerarse m�s �ptima que la implementada
	 * por el operador multiplicaci�n porque desposita el resultado en un 
	 * par�metro de salida, lo que evita la copia posterior. La desventaja
	 * es que su uso es m�s inc�modo.
	 * @param op2 Punto (v�rtice) a transformar.
	 * @param result Punto / v�rtice donde despositar el resultado.
	 * @return Devuelve el v�rtice pasado como segundo par�metro.
	 */
	MDLPoint& multiply(const MDLPoint& op2, MDLPoint& result) {
				result[0] = op2.dotProduct(_data[0]) + _data[0][3];
				result[1] = op2.dotProduct(_data[1]) + _data[1][3];
				result[2] = op2.dotProduct(_data[2]) + _data[2][3];
				return result; }

	// Faltan los constructores y dem�s, los cuaterniones...

protected:

	/**
	 * Datos de la matriz.
	 */
	matrix_t _data;

}; // class MDLMatrix

} // namespace HalfLifeMDL

#endif
