#define N_IMPLEMENTS nMDLRepository

#include "MLoader/nMDLRepository.h"
#include "HalfLifeMDL/MDLModelRepository.h"

using namespace HalfLifeMDL;

nNebulaScriptClass(nMDLRepository, "nroot");
//------------------------------------------------------------------------------
/**
 * Constructor, únicamente creamos las dos variables privadas de la clase
 * en las que almacenaremos el personaje y el arma que nos proporcione
 * la clase que funciona como almacén de modelos (MDLModelRepository).
 */
nMDLRepository::nMDLRepository(){
}
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
nMDLRepository::~nMDLRepository(){	
}
//------------------------------------------------------------------------------
/**
 * Devolvemos el personaje.
 * @param fichero Fichero del que vamos a cargar el modelo.
 * @return Modelo cargado.
 */
MDLCore* nMDLRepository::getModelo(const char* fichero){
	MDLCore *modelo;

	modelo = MDLModelRepository::loadModel(fichero);
	if (modelo)
		modelo->freeTextures();
	return modelo;
}
