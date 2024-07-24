#define N_IMPLEMENTS nFont
//------------------------------------------------------------------------------
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "text/nFont.h"
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
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    //clazz->AddCmd("v_xxx_v", 'XXXX', n_xxx);
    clazz->EndCmds();
}


//------------------------------------------------------------------------------

/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nFont::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        //nCmd* cmd = ps->GetCmd(this, 'BLDR');
        //ps->PutCmd(cmd);

        return true;
    }
    return false;
}

