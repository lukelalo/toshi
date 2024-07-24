#define N_IMPLEMENTS nCasaPlanos
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------

#include "toshi/ncasaplanos.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nMapa

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

bool nCasaPlanos::SaveCmds( nPersistServer *ps )
{
	if ( nEdificio::SaveCmds( ps ) )
    {
        //nCmd* cmd;
        return true;
    }
    return false;
}