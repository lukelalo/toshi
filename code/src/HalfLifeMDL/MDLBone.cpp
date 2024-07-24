//---------------------------------------------------------------------------
// MDLBone.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLBone.cpp
 * Contiene la definici�n (implementaci�n) de la clase MDLBone que almacena
 * la informaci�n sobre un hueso (articulaci�n) del modelo. Cada hueso tendr�
 * un padre (salvo el ra�z), que se utiliza para colocarle. Se crea as� una
 * jerarqu�a de huesos (o articulaciones) sobre la que se coloca la piel.
 * <p>
 * �sta es una clase "de alto nivel" a la que el usuario del m�dulo tiene 
 * acceso. Est� directamente relacionada con la estructura HalfLifeMDL::bone_t
 * utilizada en el fichero MDL.
 * @see HalfLifeMDL::bone_t
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 * @todo Enlace a la p�gina general de la "arquitectura".
 */

#include "MDLBone.h"

namespace HalfLifeMDL {

/**
 * Constructor de los objetos de la clase.
 * @param infoBone Estructura con la informaci�n general del hueso tal
 * y como se ha le�do del fichero. Este puntero debe ser
 * liberado por el usuario (se hace copia).
 * @param parent Hueso padre. Aunque esa informaci�n tambi�n est� en
 * el par�metro infoBone, aqu� se recibe un puntero al hueso, en lugar
 * del �ndice recibido en la estructura. El hueso padre NO es liberado
 * en el constructor. Si el hueso actual es el ra�z (sin padre), este
 * par�metro ser� NULL.
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

