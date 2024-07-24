#define N_IMPLEMENTS nMenu
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nMenu.h"
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

static void n_mostrar(void* slf, nCmd* cmd);
static void n_ocultar(void* slf, nCmd* cmd);
static void n_setimagenfondo(void *slf, nCmd *cmd);
static void n_agregaropcion(void *slf, nCmd *cmd);
static void n_agregartrigger(void *slf, nCmd *cmd);
static void n_modificaraccionopcion(void *slf, nCmd *cmd);
static void n_modificarposicionopcion(void *slf, nCmd *cmd);
static void n_modificardimensionesopcion(void *slf, nCmd *cmd);
static void n_modificarimagenprincipalopcion(void *slf, nCmd *cmd);
static void n_modificarimagenoveropcion(void *slf, nCmd *cmd);
static void n_modificarimagenseleccionopcion(void *slf, nCmd *cmd);
static void n_setnodo(void *slf, nCmd *cmd);
static void n_generarsprite(void *slf, nCmd *cmd);

void n_initcmds( nClass* clazz )
{
    clazz->BeginCmds();
	clazz->AddCmd("v_mostrar_v", 'MOST', n_mostrar);
    clazz->AddCmd("v_ocultar_v", 'OCUL', n_ocultar);
    clazz->AddCmd("v_setimagenfondo_s", 'SIFO', n_setimagenfondo);
	clazz->AddCmd("i_agregaropcion_s",'AOPC',n_agregaropcion);
	clazz->AddCmd("v_agregartrigger_fs",'AGTR',n_agregartrigger);	
	clazz->AddCmd("v_modificaraccionopcion_is",'MAOP',n_modificaraccionopcion);
	clazz->AddCmd("v_modificarposicionopcion_iff",'MPOP',n_modificarposicionopcion);
	clazz->AddCmd("v_modificardimensionesopcion_iff",'MDOP',n_modificardimensionesopcion);
	clazz->AddCmd("v_modificarimagenprincipalopcion_is",'MIPO',n_modificarimagenprincipalopcion);
	clazz->AddCmd("v_modificarimagenoveropcion_is",'MIOO',n_modificarimagenoveropcion);
	clazz->AddCmd("v_modificarimagenseleccionopcion_is",'MISO',n_modificarimagenseleccionopcion);
	clazz->AddCmd("v_setnodo_s",'SNOD',n_setnodo);
	clazz->AddCmd("v_generarsprite_v",'GNSP',n_generarsprite);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool nMenu::SaveCmds( nPersistServer* ps )
{
	if ( nRoot::SaveCmds( ps ) )
    {
        //nCmd* cmd;
        return true;
    }

    return false;
}

static void n_mostrar(void* slf, nCmd* cmd) 
{
	nMenu* self = (nMenu*)slf;
    self->Mostrar();
}

static void n_ocultar(void* slf, nCmd* cmd) 
{
	nMenu* self = (nMenu*)slf;
    self->Ocultar();
}

static void n_setimagenfondo(void *slf, nCmd *cmd) 
{
	nMenu* self = (nMenu*)slf;
    self->SetImagenFondo(cmd->In()->GetS());
}

static void n_agregaropcion(void *slf, nCmd *cmd) 
{
	nMenu* self = (nMenu*)slf;
	cmd->Out()->SetI(self->AgregarOpcion(cmd->In()->GetS()));
}

static void n_agregartrigger(void *slf, nCmd *cmd) 
{
	nMenu* self = (nMenu*)slf;
	float tiempo=cmd->In()->GetF();
	const char *accion=cmd->In()->GetS();
    self->AgregarTrigger(tiempo,accion);
}

static void n_modificaraccionopcion(void *slf, nCmd *cmd) 
{
	nMenu* self = (nMenu*)slf;
	int posicion=cmd->In()->GetI();
	const char *accion=cmd->In()->GetS();
	self->ModificarAccionOpcion(posicion,accion);
}

static void n_modificarposicionopcion(void *slf, nCmd *cmd)
{
	nMenu* self = (nMenu*)slf;
	int posicion=cmd->In()->GetI();
	float posx=cmd->In()->GetF();
	float posy=cmd->In()->GetF();
	self->ModificarPosicionOpcion(posicion, posx, posy);
}

static void n_modificardimensionesopcion(void *slf, nCmd *cmd)
{
	nMenu* self = (nMenu*)slf;
	int posicion=cmd->In()->GetI();
	float ancho=cmd->In()->GetF();
	float alto=cmd->In()->GetF();
	self->ModificarDimensionesOpcion(posicion,ancho,alto);
}

static void n_modificarimagenprincipalopcion(void *slf, nCmd *cmd)
{
	nMenu* self = (nMenu*)slf;
	int posicion=cmd->In()->GetI();
	const char *imagen=cmd->In()->GetS();
	self->ModificarImagenPrincipalOpcion(posicion,imagen);
}

static void n_modificarimagenoveropcion(void *slf, nCmd *cmd)
{
	nMenu* self = (nMenu*)slf;
	int posicion=cmd->In()->GetI();
	const char *imagen=cmd->In()->GetS();
	self->ModificarImagenOverOpcion(posicion,imagen);
}

static void n_modificarimagenseleccionopcion(void *slf, nCmd *cmd)
{
	nMenu* self = (nMenu*)slf;
	int posicion=cmd->In()->GetI();
	const char *imagen=cmd->In()->GetS();
	self->ModificarImagenOpcionSeleccionada(posicion,imagen);
}


static void n_setnodo(void *slf, nCmd *cmd)
{
	nMenu* self = (nMenu*)slf;
	self->SetNodoRaiz(cmd->In()->GetS());
}

static void n_generarsprite(void *slf, nCmd *cmd)
{
	nMenu* self = (nMenu*)slf;
	self->GenerarSprite();
}