#define N_IMPLEMENTS nIACONAESTRELLA

#include "toshi/niaconaestrella.h"
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
bool nIACONAESTRELLA::SaveCmds( nPersistServer *ps )
{

	if ( !nIA::SaveCmds( ps ) )
		return false;

	return true;
}
