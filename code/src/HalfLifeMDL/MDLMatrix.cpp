//---------------------------------------------------------------------------
// MDLMatrix.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLMatrix.cpp
 * Contiene la implementaci�n de la clase que almacena una matriz de
 * transformaci�n.
 * @see HalfLifeMDL::MDLMatrix
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 */

#include "MDLMatrix.h"

namespace HalfLifeMDL {

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
MDLMatrix& MDLMatrix::multiply(const MDLMatrix& op2, MDLMatrix& result) {

	// Estiramos los bucles a mano. F�sicamente las matrices son de
	// [3][4], aunque "l�gicamente" son de [4][4], por eso se pueden
	// multiplicar. Se considera que la �ltima fila es (0 0 0 1)

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

