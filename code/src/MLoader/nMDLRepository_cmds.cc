#define N_IMPLEMENTS nMDLRepository

/**
 *    @scriptclass
 *    nMDLRepository
 *    
 *    @superclass
 *    nRoot
 *
 *    @classinfo
 *    Clase que gestiona las peticiones de modelos a cargar.
 */
//------------------------------------------------------------------------------
#include "MLoader/nMDLRepository.h"
#include "kernel/npersistserver.h"

void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure.
*/
bool
nMDLRepository::SaveCmds(nPersistServer* ps)
{
	return nRoot::SaveCmds(ps);
}
