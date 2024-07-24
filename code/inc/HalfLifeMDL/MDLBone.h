//---------------------------------------------------------------------------
// MDLBone.h
//---------------------------------------------------------------------------

/**
 * @file MDLBone.h
 * Contiene la declaraci�n de la clase que almacena la informaci�n sobre
 * un hueso del modelo de Half Life.
 * @see HalfLifeMDL::MDLBone
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */

#ifndef __HalfLifeMDL_MDLBone
#define __HalfLifeMDL_MDLBone

#include <string>

#include "MDLBasic.h"

namespace HalfLifeMDL {

/**
 * Clase que almacena la informaci�n sobre un hueso (articulaci�n) del
 * modelo. Cada hueso tendr� un padre (salvo el ra�z), que se utiliza
 * para colocarle. Se crea as� una jerarqu�a de huesos (o articulaciones)
 * sobre la que se coloca la piel.<p>
 * �sta es una clase "de alto nivel" a la que el usuario del m�dulo tiene 
 * acceso. Est� directamente relacionada con la estructura HalfLifeMDL::bone_t
 * utilizada en el fichero MDL.
 * @warning Aunque no es lo habitual, algunos modelos del juego original de
 * Half Life tienen <b>dos huesos ra�z</b>. Ejemplos de esto son
 * <code>pow_hard.mdl</code> o <code>pow_triple.mdl</code>. En realidad no
 * son modelos de personajes, sino de dos o tres objetos que est�n en el
 * mismo modelo pero que se desplazan de forma m�s o menos independiente
 * en la animaci�n.
 * @see HalfLifeMDL::bone_t
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 * @todo Enlace a la p�gina general de la "arquitectura".
 */
class MDLBone {
public:
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
	MDLBone(const bone_t* infoBone, const MDLBone* parent);

	/**
	 * Devuelve el nombre del hueso.
	 * @return Nombre del hueso especificado por el dise�ador al realizar
	 * el modelo.
	 */
	const std::string& getName() const { return _name; }

	/**
	 * Devuelve el valor por defecto del grado de libertad especificado.
	 * @param dof Grado de libertad cuyo valor se desea obtener.
	 * @return Valor por defecto para ese grado de libertad.
	 */
	float getDefaultValue(DoF_t dof) const { return _defaultValue[dof]; }

	/**
	 * Devuelve el escalado del grado de libertad especificado. Este
	 * escalado s�lo debe usarse para los valores del grado de libertad
	 * obtenidos de las animaciones.
	 * @param dof Grado de libertad cuyo valor se desea obtener.
	 * @return Valor por defecto para ese grado de libertad.
	 * @todo Lo suyo ser�a quitar esto y que pack() haga las multiplicaciones
	 * oportunas.
	 */
	float getScale(DoF_t dof) const { return _scale[dof]; }

	/**
	 * Devuelve el hueso padre.
	 * @return Hueso padre. Si se devuelve NULL, este hueso es el ra�z del
	 * modelo, que no tiene padre.
	 */
	const MDLBone* getParent() const { return _parent; }

	/**
	 * Devuelve el �ndice en el modelo al que pertenece este hueso del
	 * hueso padre. Si este hueso es el ra�z, se devuelve -1.
	 * @return �ndice del hueso padre en el vector de huesos del
	 * modelo. Si no existe hueso padre, se devuelve -1.
	 */
	int getParentIndex() const { return _parentIndex;} 

protected:
	/**
	 * Nombre del hueso dado por el dise�ador al crear el modelo.
	 */
	std::string _name;

	/**
	 * Hueso padre del que "cuelga" �ste. Se utiliza para crear la
	 * jerarqu�a del esqueleto. Si el valor es NULL, �ste ser� el hueso
	 * padre.
	 */
	const MDLBone* _parent;

	/**
	 * �ndice del hueso padre en el vector de huesos del modelo. Si no
	 * existe hueso padre, ser� -1.
	 */
	int _parentIndex;

	/**
	 * Valores por defecto de los 6 grados de libertad del hueso.
	 * Los grados de libertad son:
	 *
	 *	- 0: Desplazamiento X.
	 *	- 1: Desplazamiento Y.
	 *	- 2: Desplazamiento Z.
	 *	- 3: Rotaci�n sobre el eje X.
	 *	- 4: Rotaci�n sobre el eje Y.
	 *	- 5: Rotaci�n sobre el eje Z.
	 *
	 * El enumerado HalfLifeMDL::DoF_t (Degree Of Freedom) contiene las
	 * constantes pertinentes para acceder con nombres simb�licos a los
	 * elementos de este vector.<p>
	 * Si un hueso no tiene animaci�n en una secuencia, se usan los valores de
	 * este vector para colocarlo con respecto al hueso padre.
	 */
	float _defaultValue[6];

	/**
	 * Escalado de cada grado de libertad.
	 * Los grados de libertad son:
	 *
	 *	- 0: Desplazamiento X.
	 *	- 1: Desplazamiento Y.
	 *	- 2: Desplazamiento Z.
	 *	- 3: Rotaci�n sobre el eje X.
	 *	- 4: Rotaci�n sobre el eje Y.
	 *	- 5: Rotaci�n sobre el eje Z.
	 *
	 * El enumerado HalfLifeMDL::DoF_t (Degree Of Freedom) contiene las
	 * constantes pertinentes para acceder con nombres simb�licos a los
	 * elementos de este vector.<p>
	 * Este escalado s�lo se usa para los valores obtenidos con las animaciones,
	 * pero no para los valores por defecto (que se supone que est�n ya escalados).
	 */
	float _scale[6];

}; // class MDLBone

} // namespace HalfLifeMDL

#endif
