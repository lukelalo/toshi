#define N_IMPLEMENTS nPersonaje
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/npersonaje.h"
#include "kernel/npersistserver.h"

static void n_ira( void *slf, nCmd *cmd );
static void n_setnodovis( void *slf, nCmd *cmd );
static void n_getcampesinoscontratados( void *slf, nCmd *cmd );


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
	clazz->AddCmd( "v_ira_ff", 'IRA ', n_ira );
    clazz->AddCmd( "v_setnodovis_s", 'SNVI', n_setnodovis );
	clazz->AddCmd( "i_getcampesinoscontratados_v", 'GCCO', n_getcampesinoscontratados );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool nPersonaje::SaveCmds( nPersistServer *ps )
{
	if ( nEntity::SaveCmds( ps ) )
    {
        //nCmd* cmd;
        return true;
    }

    return false;
}

static void n_ira( void *slf, nCmd *cmd )
{
    nPersonaje *self = (nPersonaje *) slf;
    float x = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->IrA( true, x, z );
}

static void n_setnodovis( void *slf, nCmd *cmd )
{
    nPersonaje *self = (nPersonaje *) slf;
    self->SetVisNode( cmd->In()->GetS() );
}

static void n_getcampesinoscontratados( void *slf, nCmd *cmd )
{
	nPersonaje *self      = (nPersonaje *) slf;
	int        campesinos = self->GetCampesinosContratados();

	cmd->Out()->SetI( campesinos );
}