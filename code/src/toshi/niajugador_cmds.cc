#define N_IMPLEMENTS nIAJugador

#include "toshi/niajugador.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nworld

    @superclass
    nroot

    @classinfo
    A detailed description of what the class does (written for script programmers!)
*/
void n_initcmds( nClass *clazz )
{
    clazz->BeginCmds();
	clazz->EndCmds();
}



//------------------------------------------------------------------------------
/**
    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool nIAJugador::SaveCmds( nPersistServer *ps )
{

	if ( !nRoot::SaveCmds( ps ) )
		return false;

	return true;
}
