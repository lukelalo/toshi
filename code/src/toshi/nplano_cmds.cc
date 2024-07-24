#define N_IMPLEMENTS nPlano
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "node/n3dnode.h"
#include "toshi/nplano.h"
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
void n_initcmds( nClass *clazz )
{
    clazz->BeginCmds();
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool nPlano::SaveCmds( nPersistServer *ps )
{
	if ( n3DNode::SaveCmds( ps ) )
    {
        //nCmd* cmd;
        return true;
    }

    return false;
}