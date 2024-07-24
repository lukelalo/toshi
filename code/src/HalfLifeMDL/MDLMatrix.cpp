//---------------------------------------------------------------------------
// MDLMatrix.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLMatrix.cpp
 * Contiene la implementación de la clase que almacena una matriz de
 * transformación.
 * @see HalfLifeMDL::MDLMatrix
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#include "MDLMatrix.h"

namespace HalfLifeMDL {

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
MDLMatrix& MDLMatrix::multiply(const MDLMatrix& op2, MDLMatrix& result) {

	// Estiramos los bucles a mano. Físicamente las matrices son de
	// [3][4], aunque "lógicamente" son de [4][4], por eso se pueden
	// multiplicar. Se considera que la última fila es (0 0 0 1)

	result[0][0] = _data[0][0] * op2._data[0][0] +
					_data[0][1] * op2._data[1][0] +
					_data[0][2] * op2._data[2][0];
	result[0][1] = _data[0][0] * op2._data[0][1] +
					_data[0][1] * op2._data[1][1] +
					_data[0][2] * op2._data[2][1];
	result[0][2] = _data[0][0] * op2._data[0][2] + 
					_data[0][1] * op2._data[1][2] +
					_data[0][2] * op2._data[2][2];
	result[0][3] = _data[0][0] * op2._data[0][3] + 
					_data[0][1] * op2._data[1][3] +
					_data[0][2] * op2._data[2][3] + _data[0][3];
	result[1][0] = _data[1][0] * op2._data[0][0] + 
					_data[1][1] * op2._data[1][0] +
					_data[1][2] * op2._data[2][0];
	result[1][1] = _data[1][0] * op2._data[0][1] + 
					_data[1][1] * op2._data[1][1] +
					_data[1][2] * op2._data[2][1];
	result[1][2] = _data[1][0] * op2._data[0][2] +
					_data[1][1] * op2._data[1][2] +
					_data[1][2] * op2._data[2][2];
	result[1][3] = _data[1][0] * op2._data[0][3] + 
					_data[1][1] * op2._data[1][3] +
					_data[1][2] * op2._data[2][3] + _data[1][3];
	result[2][0] = _data[2][0] * op2._data[0][0] + 
					_data[2][1] * op2._data[1][0] +
					_data[2][2] * op2._data[2][0];
	result[2][1] = _data[2][0] * op2._data[0][1] +
					_data[2][1] * op2._data[1][1] +
					_data[2][2] * op2._data[2][1];
	result[2][2] = _data[2][0] * op2._data[0][2] +
					_data[2][1] * op2._data[1][2] +
					_data[2][2] * op2._data[2][2];
	result[2][3] = _data[2][0] * op2._data[0][3] + 
					_data[2][1] * op2._data[1][3] +
					_data[2][2] * op2._data[2][3] + _data[2][3];

	return result;

} // multiply

} // namespace HalfLifeMDL

