//---------------------------------------------------------------------------
// MDLQuaternion.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLQuaternion.cpp
 * Contiene la implementación de la clase que almacena la información sobre
 * un cuaternión.
 * @see HalfLifeMDL::MDLQuaternion
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#include <math.h>

#include "MDLQuaternion.h"


#ifdef _WIN32
// Warning "Conversion from 'double' to 'float', possible loss of data"
#pragma warning (disable : 4244) 
#endif

namespace HalfLifeMDL {

/**
 * Valor constante de Pi con 20 decimales
 */
#define	Q_PI	3.14159265358979323846

/**
 * Establece el cuaternión a partir de tres ángulos.
 * El código usado para generar el cuaternión se ha extraído del visor
 * de MDL de referencia. No me he preocupado mucho de mirar qué hace
 * exactamente el código, por lo que no puedo detallar qué hace el método
 * O:-). Sospecho que tiene que ver con lo que en el comentario inicial
 * se llaman ángulos Euler.
 * @param x Rotación en el eje x
 * @param y Rotación en el eje y
 * @param z Rotación en el eje z.
 */
void MDLQuaternion::setAngles(MDLBasicType x,
								MDLBasicType y, MDLBasicType z) {
	MDLBasicType angle;
	MDLBasicType sr, sp, sy, cr, cp, cy;

	// FIXME: rescale the inputs to 1/2 angle
	angle = z * 0.5;
	sy = sin(angle);
	cy = cos(angle);
	angle = y * 0.5;
	sp = sin(angle);
	cp = cos(angle);
	angle = x * 0.5;
	sr = sin(angle);
	cr = cos(angle);

	_coords[0] = sr*cp*cy-cr*sp*sy; // X
	_coords[1] = cr*sp*cy+sr*cp*sy; // Y
	_coords[2] = cr*cp*sy-sr*sp*cy; // Z
	_coords[3] = cr*cp*cy+sr*sp*sy; // W

} // setAngles

//---------------------------------------------------------------------------

/**
 * Interpolación esférica entre el cuaternión actual y el pasado como
 * parámetro (Spherical linear interpolation).
 * Puede considerarse una versión optimizada de slerp(MDLQuaternion,
 * MDLBasicType), porque el resultado lo deposita en un parámetro de
 * salida, ahorrandose una copia. La desventaja es que es más
 * incómodo de usar.
 * @param t Peso de la interpolación. Un 1.0 da todo el peso al
 * cuaternión del parámetro, y ninguno al del objeto.
 * @param result Cuaternión donde depositar el resultado.
 * @return El cuaternión resultado de la interpolación (pasado como
 * último parámetro).
 */
MDLQuaternion& MDLQuaternion::slerp(const MDLQuaternion& op2,
									MDLBasicType t, MDLQuaternion& result) {

	int i;
	MDLBasicType omega, cosom, sinom, sclp, sclq;
	MDLQuaternion q;

	q = op2;

	// decide if one of the quaternions is backwards
	MDLBasicType a;
	MDLBasicType b;
	a = 0;
	b = 0;
	for (i = 0; i < 4; i++) {
		a += (_coords[i] - q[i]) * (_coords[i] - q[i]);
		b += (_coords[i] + q[i]) * (_coords[i] + q[i]);
	}
	if (a > b) {
		for (i = 0; i < 4; i++) {
			q[i] = -q[i];
		}
	}

	cosom = _coords[0]*q[0] + _coords[1]*q[1] +
			_coords[2]*q[2] + _coords[3]*q[3];

	if ((1.0 + cosom) > 0.00000001) {
		if ((1.0 - cosom) > 0.00000001) {
			omega = acos( cosom );
			sinom = sin( omega );
			sclp = sin( (1.0 - t)*omega) / sinom;
			sclq = sin( t*omega ) / sinom;
		}
		else {
			sclp = 1.0 - t;
			sclq = t;
		}
		for (i = 0; i < 4; i++) {
			result[i] = sclp * _coords[i] + sclq * q[i];
		}
	}
	else {
		result[0] = -_coords[1];
		result[1] = _coords[0];
		result[2] = -_coords[3];
		result[3] = _coords[2];
		sclp = sin( (1.0 - t) * 0.5 * Q_PI);
		sclq = sin( t * 0.5 * Q_PI);
		for (i = 0; i < 3; i++) {
			result[i] = sclp * _coords[i] + sclq * result[i];
		}
	}

	return result;

} // slerp

//---------------------------------------------------------------------------

/**
 * Devuelve la matriz de rotación asociada al cuaternión actual.
 * Se puede considerar más eficiente que toMatrix() porque el resultado
 * se almacena en un parámetro de salida, evitandose la copia.
 * @param result Matriz resultado. Sólo se modifica la parte relativa
 * a la matriz de rotación. La parte de la traslación no se modifica.
 * @return La matriz recibida en el parámetro.
 */
MDLMatrix& MDLQuaternion::toMatrix(MDLMatrix& result) {

	result[0][0] = 1.0 - 2.0 * _coords[1] * _coords[1] -
						2.0 * _coords[2] * _coords[2];
	result[1][0] = 2.0 * _coords[0] * _coords[1] +
						2.0 * _coords[3] * _coords[2];
	result[2][0] = 2.0 * _coords[0] * _coords[2] -
						2.0 * _coords[3] * _coords[1];

	result[0][1] = 2.0 * _coords[0] * _coords[1] -
						2.0 * _coords[3] * _coords[2];
	result[1][1] = 1.0 - 2.0 * _coords[0] * _coords[0] -
						2.0 * _coords[2] * _coords[2];
	result[2][1] = 2.0 * _coords[1] * _coords[2] +
						2.0 * _coords[3] * _coords[0];

	result[0][2] = 2.0 * _coords[0] * _coords[2] +
						2.0 * _coords[3] * _coords[1];
	result[1][2] = 2.0 * _coords[1] * _coords[2] -
						2.0 * _coords[3] * _coords[0];
	result[2][2] = 1.0 - 2.0 * _coords[0] * _coords[0] -
						2.0 * _coords[1] * _coords[1];

	return result;

} // toMatrix

} // namespace HalfLifeMDL
