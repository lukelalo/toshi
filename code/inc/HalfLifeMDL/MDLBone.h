//---------------------------------------------------------------------------
// MDLBone.h
//---------------------------------------------------------------------------

/**
 * @file MDLBone.h
 * Contiene la declaración de la clase que almacena la información sobre
 * un hueso del modelo de Half Life.
 * @see HalfLifeMDL::MDLBone
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */

#ifndef __HalfLifeMDL_MDLBone
#define __HalfLifeMDL_MDLBone

#include <string>

#include "MDLBasic.h"

namespace HalfLifeMDL {

/**
 * Clase que almacena la información sobre un hueso (articulación) del
 * modelo. Cada hueso tendrá un padre (salvo el raíz), que se utiliza
 * para colocarle. Se crea así una jerarquía de huesos (o articulaciones)
 * sobre la que se coloca la piel.<p>
 * Ésta es una clase "de alto nivel" a la que el usuario del módulo tiene 
 * acceso. Está directamente relacionada con la estructura HalfLifeMDL::bone_t
 * utilizada en el fichero MDL.
 * @warning Aunque no es lo habitual, algunos modelos del juego original de
 * Half Life tienen <b>dos huesos raíz</b>. Ejemplos de esto son
 * <code>pow_hard.mdl</code> o <code>pow_triple.mdl</code>. En realidad no
 * son modelos de personajes, sino de dos o tres objetos que están en el
 * mismo modelo pero que se desplazan de forma más o menos independiente
 * en la animación.
 * @see HalfLifeMDL::bone_t
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 * @todo Enlace a la página general de la "arquitectura".
 */
class MDLBone {
public:
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
	MDLBone(const bone_t* infoBone, const MDLBone* parent);

	/**
	 * Devuelve el nombre del hueso.
	 * @return Nombre del hueso especificado por el diseñador al realizar
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
	 * escalado sólo debe usarse para los valores del grado de libertad
	 * obtenidos de las animaciones.
	 * @param dof Grado de libertad cuyo valor se desea obtener.
	 * @return Valor por defecto para ese grado de libertad.
	 * @todo Lo suyo sería quitar esto y que pack() haga las multiplicaciones
	 * oportunas.
	 */
	float getScale(DoF_t dof) const { return _scale[dof]; }

	/**
	 * Devuelve el hueso padre.
	 * @return Hueso padre. Si se devuelve NULL, este hueso es el raíz del
	 * modelo, que no tiene padre.
	 */
	const MDLBone* getParent() const { return _parent; }

	/**
	 * Devuelve el índice en el modelo al que pertenece este hueso del
	 * hueso padre. Si este hueso es el raíz, se devuelve -1.
	 * @return Índice del hueso padre en el vector de huesos del
	 * modelo. Si no existe hueso padre, se devuelve -1.
	 */
	int getParentIndex() const { return _parentIndex;} 

protected:
	/**
	 * Nombre del hueso dado por el diseñador al crear el modelo.
	 */
	std::string _name;

	/**
	 * Hueso padre del que "cuelga" éste. Se utiliza para crear la
	 * jerarquía del esqueleto. Si el valor es NULL, éste será el hueso
	 * padre.
	 */
	const MDLBone* _parent;

	/**
	 * Índice del hueso padre en el vector de huesos del modelo. Si no
	 * existe hueso padre, será -1.
	 */
	int _parentIndex;

	/**
	 * Valores por defecto de los 6 grados de libertad del hueso.
	 * Los grados de libertad son:
	 *
	 *	- 0: Desplazamiento X.
	 *	- 1: Desplazamiento Y.
	 *	- 2: Desplazamiento Z.
	 *	- 3: Rotación sobre el eje X.
	 *	- 4: Rotación sobre el eje Y.
	 *	- 5: Rotación sobre el eje Z.
	 *
	 * El enumerado HalfLifeMDL::DoF_t (Degree Of Freedom) contiene las
	 * constantes pertinentes para acceder con nombres simbólicos a los
	 * elementos de este vector.<p>
	 * Si un hueso no tiene animación en una secuencia, se usan los valores de
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
	 *	- 3: Rotación sobre el eje X.
	 *	- 4: Rotación sobre el eje Y.
	 *	- 5: Rotación sobre el eje Z.
	 *
	 * El enumerado HalfLifeMDL::DoF_t (Degree Of Freedom) contiene las
	 * constantes pertinentes para acceder con nombres simbólicos a los
	 * elementos de este vector.<p>
	 * Este escalado sólo se usa para los valores obtenidos con las animaciones,
	 * pero no para los valores por defecto (que se supone que están ya escalados).
	 */
	float _scale[6];

}; // class MDLBone

} // namespace HalfLifeMDL

#endif
