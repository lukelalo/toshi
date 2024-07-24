#define N_IMPLEMENTS nPartida
//------------------------------------------------------------------------------
//	Interfaz TCL de la clase nGame
//------------------------------------------------------------------------------
#include "toshi/npartida.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    ngame

    @superclass
    nroot

    @classinfo
    Overarching game object which is instantiated and takes care of running
    the entire game.
*/
void n_initcmds( nClass *clazz )
{
    clazz->BeginCmds();
    clazz->EndCmds();
}

//------------------------------------------------------------------------------

bool nPartida::SaveCmds( nPersistServer *ps )
{
    if ( nRoot::SaveCmds( ps ) )
        return true;
    return false;
}