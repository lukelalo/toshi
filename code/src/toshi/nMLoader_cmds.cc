#define N_IMPLEMENTS nMLoader
//------------------------------------------------------------------------------
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
/**
 *    @scriptclass
 *    nMLoader
 *    
 *    @superclass
 *    n3dnode
 *
 *    @classinfo
 *    Esta clase genera las estructuras que usa el motor Nebula para pintar modelos en 3D.
 */
//------------------------------------------------------------------------------
#include <String>
#include "toshi/nMLoader.h"
#include "kernel/npersistserver.h"

static void n_setfile(void *slf, nCmd *cmd);
static void n_setAnimation(void *slf, nCmd *cmd);
static void n_getAnimation(void *slf, nCmd *cmd);
static void n_getNumAttaches(void *slf, nCmd *cmd);
static void n_getNumCurrentAttach(void *slf, nCmd *cmd);
static void n_getCurrentAttach(void *slf, nCmd *cmd);
static void n_setAttach(void *slf, nCmd *cmd);
static void n_detach(void *slf, nCmd *cmd);
  
void
n_initcmds(nClass* clazz) 
{
    clazz->BeginCmds();
	clazz->AddCmd("v_setFileName_s", 'SFLN', n_setfile);
	clazz->AddCmd("v_setAnimation_i", 'SANI', n_setAnimation);
	clazz->AddCmd("s_getAnimation_v", 'GANI', n_getAnimation);
	clazz->AddCmd("i_getNumAttaches_v", 'GNAT', n_getNumAttaches);
	clazz->AddCmd("i_getNumCurrentAttach_v", 'GNAA', n_getNumCurrentAttach);
	clazz->AddCmd("s_getCurrentAttach_v", 'GSCA', n_getCurrentAttach);
	clazz->AddCmd("b_attach_is", 'ATCH', n_setAttach);
	clazz->AddCmd("v_detach_v", 'DTCH', n_detach);
    clazz->EndCmds();
}
//------------------------------------------------------------------------------
/**
 Método para invocar desde el escript de TCL que carga un fichero determinado
*/
static void n_setfile(void *slf, nCmd *cmd) {
	nMLoader *self = (nMLoader*)slf;

	self->SetFileName((char*)cmd->In()->GetS());
}

/**
 * Metodo para invocar desde el script tcl que crea los nodos al principio
 * al método de la clase nMLoader que establece la animación del muñeco.
 */
static void n_setAnimation(void *slf, nCmd *cmd) {
	nMLoader *self = (nMLoader*)slf;
	self->SetAnimation((int)cmd->In()->GetI());
}

static void n_getAnimation(void *slf, nCmd *cmd) {
	nMLoader *self = (nMLoader*)slf;
	const char *nombre = self->GetAnimation();
	cmd->Out()->SetS(nombre);
}

//------------------------------------------------------------------------------

static void n_getNumAttaches(void *slf, nCmd *cmd) {
    nMLoader* self = (nMLoader*) slf;
    cmd->Out()->SetI(self->getNumAttachTotales());
}

static void n_getNumCurrentAttach(void *slf, nCmd *cmd) {
    nMLoader* self = (nMLoader*) slf;
    cmd->Out()->SetI(self->getNumeroAttach());
}

static void n_getCurrentAttach(void *slf, nCmd *cmd) {
    nMLoader* self = (nMLoader*) slf;
    cmd->Out()->SetS(self->getNombreAttach().c_str());
}

static void n_setAttach(void *slf, nCmd *cmd) {
    nMLoader* self = (nMLoader*) slf;

	int n = cmd->In()->GetI();
	const char * nombre = cmd->In()->GetS();
    cmd->Out()->SetB(self->AttachModel(n, nombre));
}

static void n_detach(void *slf, nCmd *cmd) {
    nMLoader* self = (nMLoader*) slf;
    self->DetachModel();
}




//------------------------------------------------------------------------------
/**
 * @param  ps          writes the nCmd object contents out to a file.
 * @return             success or failure.
 */
bool
nMLoader::SaveCmds(nPersistServer* ps)
{
    if (nVisNode::SaveCmds(ps))
    {
        nCmd* cmd = ps->GetCmd(this, 'BLDR');
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
//------------------------------------------------------------------------------