#ifndef N_MDLREPOSITORY_H
#define N_MDLREPOSITORY_H
//----------------------------------------------------------------------------------------------
/**
 *   @file nMDLRepository
 *
 *   @brief Esta clase se encarga gestionar los modelos que se cargan usando
 *	 la clase MDLMModelRepository, que ir� guardando aquellos que se han cargado alguna vez.
 *	 De este modo, cuando se solicita un determinado modelo y ya se encuentra almacenado, no 
 *	 se volver� a cargar sino que lo devuelve directamente. Si es la primera vez que se solicita,
 *	 lo cargar� y lo almacenar� para poder devolverlo directamente en futuras ocasiones.
 *	
 *   @author Felicidad Ramos Manjavacas, Marco Antonio G�mez Mart�n
 *	 @date Marzo 2004.
 */
//----------------------------------------------------------------------------------------------
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#include "kernel/ndefdllclass.h"
#include "HalfLifeMDL/MDLCore.h"

//------------------------------------------------------------------------------
/**
Servidor para cargar modelos de ficheros MDL. Debe estar colgado
de "sys/servers/mdlRepository", para que las instancias puedan acceder
a este servidor.<p>

@note Utiliza la librer�a HalfLifeMDL para la carga de los ficheros.

@author Felicidad Ramos Manjavacas, Marco Antonio G�mez Mart�n
@date Marzo 2004.
*/
class N_PUBLIC nMDLRepository : public nRoot
{
public:
    
	nMDLRepository();

	~nMDLRepository();

 	virtual bool SaveCmds(nPersistServer* persistServer);	//Persistencia

	/**
	Puntero al kernel server
	*/
	static nKernelServer* kernelServer;

	/**
	M�todo que carga un modelo del fichero, y devuelve la
	informaci�n cargada.
	*/
	HalfLifeMDL::MDLCore* getModelo(const char* fichero);

};
//------------------------------------------------------------------------------
#endif
