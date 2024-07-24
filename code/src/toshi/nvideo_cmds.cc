#define N_IMPLEMENTS nVideo
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nvideo.h"
#include "kernel/npersistserver.h"

static void n_mostrar(void* slf, nCmd* cmd);
static void n_ocultar(void* slf, nCmd* cmd);
static void n_setvideo(void *slf, nCmd *cmd);
static void n_setdestructorvideo(void *slf, nCmd *cmd);
static void n_setaccionfinal(void *slf, nCmd *cmd);
static void n_agregartrigger(void *slf, nCmd *cmd);

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
	clazz->AddCmd("v_mostrar_v", 'MOST', n_mostrar);
    clazz->AddCmd("v_ocultar_v", 'OCUL', n_ocultar);
    clazz->AddCmd("v_setvideo_s", 'SVID', n_setvideo);
    clazz->AddCmd("v_setdestructorvideo_s", 'SDVI', n_setdestructorvideo);
	clazz->AddCmd("v_setaccionfinal_s",'SACF',n_setaccionfinal);
	clazz->AddCmd("v_agregartrigger_fs",'AGTR',n_agregartrigger);	
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool nVideo::SaveCmds( nPersistServer* ps )
{
	if ( nRoot::SaveCmds( ps ) )
    {
        //nCmd* cmd;
        return true;
    }

    return false;
}

static void n_mostrar(void* slf, nCmd* cmd) {
    nVideo* self = (nVideo*)slf;
    self->Mostrar();
}

static void n_ocultar(void* slf, nCmd* cmd) {
    nVideo* self = (nVideo*)slf;
    self->Ocultar();
}

static void n_setvideo(void* slf, nCmd* cmd) {
    nVideo* self = (nVideo*)slf;
    self->SetVideo(cmd->In()->GetS());
}

static void n_setdestructorvideo(void* slf, nCmd* cmd) {
    nVideo* self = (nVideo*)slf;
    self->SetDestructorVideo(cmd->In()->GetS());
}

static void n_setaccionfinal(void* slf, nCmd* cmd) {
    nVideo* self = (nVideo*)slf;
    self->SetAccionFinal(cmd->In()->GetS());
}

static void n_agregartrigger(void* slf, nCmd* cmd) {
    nVideo* self = (nVideo*)slf;
	float tiempo=cmd->In()->GetF();
	const char *accion=cmd->In()->GetS();
    self->AgregarTrigger(tiempo,accion);
}