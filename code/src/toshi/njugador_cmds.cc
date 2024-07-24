#define N_IMPLEMENTS nJugador
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/njugador.h"
#include "kernel/npersistserver.h"

static void n_setid( void *slf, nCmd *cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nclassname

    @superclass
    name of the super class (super klaas, weiter so!)

    @classinfo
    A detailed description of what the class does (written for script programmers!)
*/
void n_initcmds( nClass *clazz )
{
    clazz->BeginCmds();
	clazz->AddCmd( "v_setid_i", 'SID ', n_setid );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/

static void n_setid( void *slf, nCmd *cmd )
{
	nJugador *self = (nJugador *) slf;
	self->SetId( cmd->In()->GetI() );
}

bool nJugador::SaveCmds( nPersistServer *ps )
{
	if ( nRoot::SaveCmds( ps ) )
    {
        //nCmd* cmd;
        return true;
    }

    return false;
}

