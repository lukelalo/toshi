#define N_IMPLEMENTS nZipFileServer
//------------------------------------------------------------------------------
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "file/nzipfileserver.h"
#include "kernel/npersistserver.h"

static void n_setrootdir(void* slf, nCmd* cmd);
static void n_getrootdir(void* slf, nCmd* cmd);


//------------------------------------------------------------------------------
/**
    @scriptclass
    nzipfileserver
    
    @superclass
    nfileserver2

    @classinfo
    server for zip files and directories 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setrootdir_s", 'SROO', n_setrootdir);
    clazz->AddCmd("s_getrootdir_v", 'GROO', n_getrootdir);
    clazz->EndCmds();
}


//------------------------------------------------------------------------------
/**
    @cmd
    setrootdir

    @input
	s path

    @output
    v

    @info
    sets the directory in which to look for zip-files
*/
static
void
n_setrootdir(void* slf, nCmd* cmd)
{
    nZipFileServer* self = (nZipFileServer*) slf;
    self->SetRootDirectory(cmd->In()->GetS());
}
//------------------------------------------------------------------------------
/**
    @cmd
    getrootdir

    @input
	v

    @output
	s path

    @info
	siehe setrootdir
*/
static
void
n_getrootdir(void* slf, nCmd* cmd)
{
    nZipFileServer* self = (nZipFileServer*) slf;
    cmd->Out()->SetS(self->GetRootDirectory());
}
