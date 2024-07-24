#define N_IMPLEMENTS nGame
//------------------------------------------------------------------------------
//	Interfaz TCL de la clase nGame
//------------------------------------------------------------------------------
#include "toshi/ngame.h"
#include "kernel/npersistserver.h"
#include "toshi/nentity.h"

static void n_run(void* slf, nCmd* cmd);
static void n_stop(void* slf, nCmd* cmd);
static void n_init(void *slf, nCmd *cmd);
static void n_setia(void *slf, nCmd *cmd);
static void n_getia(void *slf, nCmd *cmd);
static void n_setnet(void *slf, nCmd *cmd);
static void n_getnet(void *slf, nCmd *cmd);
static void n_setmenu(void *slf, nCmd *cmd);
static void n_getmenu(void *slf, nCmd *cmd);
static void n_setvideo(void *slf, nCmd *cmd);
static void n_getvideo(void *slf, nCmd *cmd);
static void n_setscene(void *slf, nCmd *cmd);
static void n_setworld(void *slf, nCmd *cmd);
static void n_setcamera(void *slf, nCmd *cmd);
static void n_getposmouseworld(void *slf, nCmd *cmd);
static void n_getentmouseworld(void *slf, nCmd *cmd);
static void n_manejadorbotonprincipal(void *slf, nCmd *cmd);
static void n_manejadorbotonsecundario(void *slf, nCmd *cmd);
static void n_mostrarrender(void *slf, nCmd *cmd);
static void n_getposmouse(void *slf, nCmd *cmd);
static void n_comenzarpartidalocal(void *slf, nCmd *cmd);
static void n_comenzarpartidared(void *slf, nCmd *cmd);
static void n_update(void *slf, nCmd *cmd);
static void n_mute(void *slf, nCmd *cmd);
static void n_pause(void *slf, nCmd *cmd);
static void n_playsong(void *slf, nCmd *cmd);
static void n_pausesong(void *slf, nCmd *cmd);
static void n_pauseallsounds(void *slf, nCmd *cmd);
static void n_stopsong(void *slf, nCmd *cmd);
static void n_stopallsounds(void *slf, nCmd *cmd);
static void n_playsound(void *slf, nCmd *cmd);
static void n_setmusicvolume(void *slf, nCmd *cmd);
static void n_setsoundvolume(void *slf, nCmd *cmd);
static void n_esjugadorprincipal(void *slf, nCmd *cmd);
static void n_getpuntosporaccion(void *slf, nCmd *cmd);
static void n_getpuntosporconstruccion(void *slf, nCmd *cmd);
static void n_getnumconstrucciones(void *slf, nCmd *cmd);
static void n_getpuntospordistintivo(void *slf, nCmd *cmd);
static void n_getpuntospororo(void *slf, nCmd *cmd);
static void n_getpuntosportiempo(void *slf, nCmd *cmd);
static void n_getpuntostotales(void *slf, nCmd *cmd);
static void n_setiniciado(void *slf,nCmd *cmd);
static void n_mostrarayuda(void *slf,nCmd *cmd);

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
	clazz->AddCmd("s_getia_v",'GIAP',n_getia);	
	clazz->AddCmd("v_setnet_s",'SNET',n_setnet);
	clazz->AddCmd("s_getnet_v",'GNET',n_getnet);		
	clazz->AddCmd("v_setmenu_s",'SMNU',n_setmenu);
	clazz->AddCmd("s_getmenu_v",'GMNU',n_getmenu);	
	clazz->AddCmd("v_setvideo_s",'SVDO',n_setvideo);
	clazz->AddCmd("s_getvideo_v",'GVDO',n_getvideo);	
	clazz->AddCmd("v_setscene_s",'SSCN',n_setscene);	
	clazz->AddCmd("v_setworld_s",'SWRL',n_setworld);
	clazz->AddCmd("v_setcamera_s",'SCMR',n_setcamera);
	clazz->AddCmd("ff_getposmouseworld_v",'GPMW',n_getposmouseworld);
	clazz->AddCmd("s_getentmouseworld_v",'GEMW',n_getentmouseworld);
	clazz->AddCmd("v_manejadorbotonprincipal_v",'MBPR',n_manejadorbotonprincipal);
	clazz->AddCmd("v_manejadorbotonsecundario_v",'MBSE',n_manejadorbotonsecundario);
	clazz->AddCmd("v_mostrarrender_b",'MREN',n_mostrarrender);
	clazz->AddCmd("ff_getposmouse_v",'GPM ',n_getposmouse);
	clazz->AddCmd("v_comenzarpartidalocal_i",'CMPL',n_comenzarpartidalocal);
	clazz->AddCmd("v_comenzarpartidared_b",'CMPR',n_comenzarpartidared);
	clazz->AddCmd("v_update_v",'UPDT',n_update);
	clazz->AddCmd("v_mute_b",'MUTE',n_mute);
	clazz->AddCmd("v_pause_b",'PAUS',n_pause);
	clazz->AddCmd("v_playsong_is",'PLSG',n_playsong);
	clazz->AddCmd("v_playsound_is",'PLSD',n_playsound);
	clazz->AddCmd("v_setmusicvolume_if",'SMVO',n_setmusicvolume);
	clazz->AddCmd("v_setsoundvolume_if",'SSVO',n_setsoundvolume);
	clazz->AddCmd("v_stopsong_i",'STSN',n_stopsong);
	clazz->AddCmd("v_stopallsounds_v",'STAS',n_stopallsounds);
	clazz->AddCmd("v_pausesong_ib",'PSSG',n_pausesong);
	clazz->AddCmd("v_pauseallsounds_b",'PASN',n_pauseallsounds);
	clazz->AddCmd("b_esjugadorprincipal_i",'EJPR',n_esjugadorprincipal);
	clazz->AddCmd("i_getpuntosporaccion_i",'GPPA',n_getpuntosporaccion);
	clazz->AddCmd("i_getpuntosporconstruccion_i",'GPPC',n_getpuntosporconstruccion);
	clazz->AddCmd("i_getnumconstrucciones_i",'GNCR',n_getnumconstrucciones);
	clazz->AddCmd("i_getpuntospordistintivo_i",'GPPD',n_getpuntospordistintivo);
	clazz->AddCmd("i_getpuntospororo_i",'GPPO',n_getpuntospororo);
	clazz->AddCmd("i_getpuntosportiempo_i",'GPPT',n_getpuntosportiempo);
	clazz->AddCmd("i_getpuntostotales_i",'GPT ',n_getpuntostotales);
	clazz->AddCmd("v_setiniciado_b",'SINI',n_setiniciado);
	clazz->AddCmd("v_mostrarayuda_b",'MOAY',n_mostrarayuda);
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

static void n_setiniciado(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
    self->SetIniciado(cmd->In()->GetB());
}

static void n_mostrarayuda(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
    self->MostrarAyuda(cmd->In()->GetB());
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

static void n_update(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
    self->Update();
}

static void n_mute(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
	self->Mute(cmd->In()->GetB());
}

static void n_pause(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
	self->Pause(cmd->In()->GetB());
}

static void n_playsong(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
	int canal=cmd->In()->GetI();
	const char *cancion=cmd->In()->GetS();
    self->PlaySong(canal,cancion);
}

static void n_pausesong(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
	int canal=cmd->In()->GetI();
	bool pausa=cmd->In()->GetB();
	self->PauseSong(canal,pausa);
}

static void n_pauseallsounds(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
	bool v_pausa=cmd->In()->GetB();
	self->PauseAllSounds(v_pausa);
}


static void n_stopsong(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
	self->StopSong(cmd->In()->GetI());
}

static void n_stopallsounds(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
	self->StopAllSounds();
}

static void n_playsound(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
	int canal=cmd->In()->GetI();
	const char *sonido=cmd->In()->GetS();
	self->PlaySound(canal,sonido);
}

static void n_setmusicvolume(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
	int canal=cmd->In()->GetI();
	float volumen=cmd->In()->GetF();
	self->SetMusicVolume(canal, volumen);
}
static void n_setsoundvolume(void* slf, nCmd* cmd) {
    nGame* self = (nGame*)slf;
	int canal=cmd->In()->GetI();
	float volumen=cmd->In()->GetF();
	self->SetSoundVolume(canal, volumen);
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

static void n_setnet(void *slf, nCmd *cmd) {
	nGame *self = (nGame*)slf;
	self->SetNet(cmd->In()->GetS());
}
static void n_getnet(void *slf, nCmd *cmd) {
	nGame *self = (nGame*)slf;
	cmd->Out()->SetS(self->GetNet());
}

static void n_setmenu(void *slf, nCmd *cmd) {
	nGame *self = (nGame*)slf;
	self->SetMenu(cmd->In()->GetS());
}
static void n_getmenu(void *slf, nCmd *cmd) {
	nGame *self = (nGame*)slf;
	cmd->Out()->SetS(self->GetMenu());
}

static void n_setvideo(void *slf, nCmd *cmd) {
	nGame *self = (nGame*)slf;
	self->SetVideo(cmd->In()->GetS());
}
static void n_getvideo(void *slf, nCmd *cmd) {
	nGame *self = (nGame*)slf;
	cmd->Out()->SetS(self->GetVideo());
}

static void n_getposmouse(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;

	vector3 posmouse = self->GetPosMouse();

	cmd->Out()->SetF((float)posmouse.x);
	cmd->Out()->SetF((float)posmouse.y);
}

static void n_getposmouseworld(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;

	vector3 posworld = self->GetPosMouseWorld();

	cmd->Out()->SetF((float)posworld.x);
	cmd->Out()->SetF((float)posworld.z);
}

static void n_manejadorbotonprincipal(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	self->ManejadorBotonPrincipal();
}

static void n_manejadorbotonsecundario(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	self->ManejadorBotonSecundario();
}

static void n_mostrarrender(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	self->MostrarRender(cmd->In()->GetB());
}

static void n_getentmouseworld(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	char buf[N_MAXPATH];
	self->GetEntMouseWorld()->GetFullName(buf,N_MAXPATH);
	cmd->Out()->SetS(buf);
}

static void n_comenzarpartidalocal(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	self->ComenzarPartidaLocal(cmd->In()->GetI());
}

static void n_comenzarpartidared(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	self->ComenzarPartidaRed(cmd->In()->GetB());
}

static void n_esjugadorprincipal(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	cmd->Out()->SetB(self->EsJugadorPrincipal(cmd->In()->GetI()));
}

static void n_getpuntosporaccion(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	cmd->Out()->SetI(self->GetPuntosPorAccion(cmd->In()->GetI()));
}

static void n_getpuntosporconstruccion(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	cmd->Out()->SetI(self->GetPuntosPorConstruccion(cmd->In()->GetI()));
}

static void n_getnumconstrucciones(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	cmd->Out()->SetI(self->GetNumConstrucciones(cmd->In()->GetI()));
}

static void n_getpuntospordistintivo(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	cmd->Out()->SetI(self->GetPuntosPorDistintivo(cmd->In()->GetI()));
}

static void n_getpuntospororo(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	cmd->Out()->SetI(self->GetPuntosPorOro(cmd->In()->GetI()));
}

static void n_getpuntosportiempo(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	cmd->Out()->SetI(self->GetPuntosPorTiempo(cmd->In()->GetI()));
}

static void n_getpuntostotales(void *slf, nCmd *cmd) {
	nGame*self=(nGame*)slf;
	cmd->Out()->SetI(self->GetPuntosTotales(cmd->In()->GetI()));
}