#define N_IMPLEMENTS nGame
//------------------------------------------------------------------------------
//	Interfaz TCL de la clase nGame
//------------------------------------------------------------------------------
#include "laberintosultan/ngame.h"
#include "kernel/npersistserver.h"

static void n_run(void* slf, nCmd* cmd);
static void n_stop(void* slf, nCmd* cmd);
static void n_init(void *slf, nCmd *cmd);
static void n_setia(void *slf, nCmd *cmd);
static void n_getia(void *slf, nCmd *cmd);
static void n_setscene(void *slf, nCmd *cmd);
static void n_setworld(void *slf, nCmd *cmd);
static void n_setcamera(void *slf, nCmd *cmd);

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
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
	clazz->AddCmd("v_init_s", 'INIT', n_init);
    clazz->AddCmd("v_run_v", 'RUNN', n_run);
    clazz->AddCmd("v_stop_v", 'STOP', n_stop);
	clazz->AddCmd("v_setia_s",'SIAP',n_setia);
	clazz->AddCmd("v_getia_s",'GIAP',n_getia);	
	clazz->AddCmd("v_setscene_s",'SSCN',n_setscene);	
	clazz->AddCmd("v_setworld_s",'SWRL',n_setworld);
	clazz->AddCmd("v_setcamera_s",'SCMR',n_setcamera);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    init

    @input
    s (game script path)

    @output
    v

    @info
	Ejecuta un script que se supone es el 'constructor' del juego.
*/
static void n_init(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
    self->Init(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    run

    @input
    s (game script path)

    @output
    v

    @info
	Comienza el bucle principal. Debe llamarse después de crear todos los
	servidores.
	Además, se puede llamar antes al metodo init, para establecer
	las propiedades del juego.
*/
static void n_run(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
    self->Run();
}

//------------------------------------------------------------------------------
/**
    @cmd
    stop

    @input
    v

    @output
    v

    @info
	Hace parar al bucle principal.
*/
static void n_stop(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
    self->Stop();
}

bool
nGame::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        // nCmd* cmd = ps->GetCmd(this, 'RUNN');
        // fs->PutCmd(cmd);

        return true;
    }
    return false;
}

static void n_setia(void *slf,nCmd *cmd) {
	nGame *self=(nGame*)slf;
	self->SetIA(cmd->In()->GetS());
}

static void n_getia(void *slf,nCmd *cmd) {
	nGame *self=(nGame*)slf;
	cmd->Out()->SetS(self->GetIA());
}

static void n_setscene(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	self->SetScene(cmd->In()->GetS());
}

static void n_setworld(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	self->SetWorld(cmd->In()->GetS());
}
static void n_setcamera(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	self->SetCamera(cmd->In()->GetS());
}