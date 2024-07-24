#define N_IMPLEMENTS nOpcion
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nOpcion.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nclassname
    
    @superclass
    name of the super class (super klaas, weiter so!)

    @classinfo
    A detailed description of what the class does (written for script programmers!) 
*/
void n_initcmds( nClass* clazz )
{
    clazz->BeginCmds();
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool nOpcion::SaveCmds( nPersistServer* ps )
{
	if ( nRoot::SaveCmds( ps ) )
    {
        //nCmd* cmd;
        return true;
    }

    return false;
}