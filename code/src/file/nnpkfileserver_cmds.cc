#define N_IMPLEMENTS nNpkFileServer
//------------------------------------------------------------------------------
//  nnpkfileserver_cmds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "file/nnpkfileserver.h"

static void n_parsefile(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nnpkfileserver
    
    @superclass
    nfileserver2

    @classinfo
    Transparent filesystem access into npk package files.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("i_parsefile_s", 'PRSF', n_parsefile);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    parsefile
    @input
    s (AbsFileName)
    @output
    b (Success)
    @info
    Parse an npk file.
*/
static
void
n_parsefile(void* slf, nCmd* cmd)
{
    nNpkFileServer* self = (nNpkFileServer*) slf;
    cmd->Out()->SetI(self->ParseFile(cmd->In()->GetS()));
}

