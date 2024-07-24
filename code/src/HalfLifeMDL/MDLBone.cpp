//---------------------------------------------------------------------------
// MDLBone.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLBone.cpp
 * Contiene la definición (implementación) de la clase MDLBone que almacena
 * la información sobre un hueso (articulación) del modelo. Cada hueso tendrá
 * un padre (salvo el raíz), que se utiliza para colocarle. Se crea así una
 * jerarquía de huesos (o articulaciones) sobre la que se coloca la piel.
 * <p>
 * Ésta es una clase "de alto nivel" a la que el usuario del módulo tiene 
 * acceso. Está directamente relacionada con la estructura HalfLifeMDL::bone_t
 * utilizada en el fichero MDL.
 * @see HalfLifeMDL::bone_t
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 * @todo Enlace a la página general de la "arquitectura".
 */

#include "MDLBone.h"

namespace HalfLifeMDL {

/**
 * Constructor de los objetos de la clase.
 * @param infoBone Estructura con la información general del hueso tal
 * y como se ha leído del fichero. Este puntero debe ser
 * liberado por el usuario (se hace copia).
 * @param parent Hueso padre. Aunque esa información también está en
 * el parámetro infoBone, aquí se recibe un puntero al hueso, en lugar
 * del índice recibido en la estructura. El hueso padre NO es liberado
 * en el constructor. Si el hueso actual es el raíz (sin padre), este
 * parámetro será NULL.
 * @todo Meter los controladores.
 */
MDLBone::MDLBone(const bone_t* infoBone, const MDLBone* parent) {

	_name = infoBone->name;
	_parentIndex = infoBone->parent;

	_parent = parent;

	for (int c = 0; c < 6; c++) {
		_defaultValue[c] = infoBone->value[c];
		_scale[c] = infoBone->scale[c];
	}

} // constructor

} // namespace HalfLifeMDL

