#define N_IMPLEMENTS nCSprite

#include "CSprite/nCSprite.h"
#include "kernel/npersistserver.h"


static void n_loadsprite(void* slf, nCmd* cmd);

static void n_setposition(void *slf, nCmd* cmd);
static void n_setpositionf(void *slf, nCmd *cmd);
static void n_getposition(void *slf, nCmd *cmd);
static void n_getpositionf(void *slf, nCmd *cmd);
static void n_setsize(void *slf, nCmd *cmd);
static void n_setsizef(void *slf, nCmd *cmd);
static void n_getsize(void *slf, nCmd *cmd);
static void n_getsizef(void *slf, nCmd *cmd);
static void n_setsourceposition(void *slf, nCmd *cmd);
static void n_setsourcepositionf(void *slf, nCmd *cmd);
static void n_getsourceposition(void*slf, nCmd *cmd);
static void n_getsourcepositionf(void*slf, nCmd *cmd);
static void n_setsourcesize(void *slf, nCmd *cmd);
static void n_setsourcesizef(void *slf, nCmd *cmd);
static void n_getsourcesize(void *slf, nCmd *cmd);
static void n_getsourcesizef(void *slf, nCmd *cmd);
static void n_getimagesize(void *slf, nCmd *cmd);
static void n_setautoscale(void *slf, nCmd *cmd);
static void n_getautoscale(void *slf, nCmd *cmd);
static void n_setvisible(void *slf, nCmd *cmd);
static void n_getvisible(void *slf, nCmd *cmd);
static void n_setorder(void *slf, nCmd *cmd);
static void n_getorder(void *slf, nCmd *cmd);

//------------------------------------------------------------------------------

void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_loadsprite_s", 'BLDR', n_loadsprite);
	clazz->AddCmd("v_setposition_ii", 'SPI ', n_setposition);
	clazz->AddCmd("v_setpositionf_ff", 'SPF ', n_setpositionf);
	clazz->AddCmd("ii_getposition_v", 'GPII', n_getposition);
	clazz->AddCmd("ff_getpositionf_v", 'GPFF', n_getpositionf);
	clazz->AddCmd("v_setsize_ii", 'SSI ', n_setsize);
	clazz->AddCmd("v_setsizef_ff", 'SSF ', n_setsizef);
	clazz->AddCmd("ii_getsize_v", 'GSII', n_getsize);
	clazz->AddCmd("ff_getsizef_v", 'GSFF', n_getsizef);
	clazz->AddCmd("v_setsourceposition_ii", 'SSPI', n_setsourceposition);
	clazz->AddCmd("v_setsourcepositionf_ff", 'SSPF', n_setsourcepositionf);
	clazz->AddCmd("ii_getsourceposition_v", 'GSPI', n_getsourceposition);
	clazz->AddCmd("ff_getsourcepositionf_v", 'GSPF', n_getsourcepositionf);
	clazz->AddCmd("v_setsourcesize_ii", 'SSSI', n_setsourcesize);
	clazz->AddCmd("v_setsourcesizef_ff", 'SSSF', n_setsourcesizef);
	clazz->AddCmd("ii_getsourcesize_v", 'GSSI', n_getsourcesize);
	clazz->AddCmd("ff_getsourcesizef_v", 'GSSF', n_getsourcesizef);
	clazz->AddCmd("ii_getimagesize_v", 'GISZ', n_getimagesize);
	clazz->AddCmd("v_setautoscale_b", 'SAES', n_setautoscale);
	clazz->AddCmd("b_getautoscale_v", 'GAES', n_getautoscale);
	clazz->AddCmd("v_setvisible_b", 'SVIS', n_setvisible);
	clazz->AddCmd("b_getvisible_v", 'GVIS', n_getvisible);
	clazz->AddCmd("v_setorder_i", 'SORD', n_setorder);
	clazz->AddCmd("i_getorder_v", 'GORD', n_getorder);

	clazz->EndCmds();
}

//------------------------------------------------------------------------------

static
void
n_loadsprite(void* slf, nCmd* cmd)
{
    nCSprite* self = (nCSprite*) slf;
    self->createSprite(cmd->In()->GetS());
}
//------------------------------------------------------------------------------
/**
	@cmd setposition

	@input
	i Posicion x en píxeles de la pantalla
	i Posicion y en píxeles de la pantalla

	@output
	v

  @info
  Situa el sprite en la posición indicada en los parámetros.
*/
static void n_setposition(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	int x = cmd->In()->GetI();
	int y = cmd->In()->GetI();
	self->setScreenPosition(x, y);
}

/**
	@cmd setpositionf

	@input
	f Posicion x en coordenadas relativas (entre 0 y 1) de la pantalla
	f Posicion y en coordenadas relativas (entre 0 y 1) de la pantalla

	@output
	v

  @info
  Situa el sprite en la posición indicada en los parámetros.
*/
static void n_setpositionf(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	float x = cmd->In()->GetF();
	float y = cmd->In()->GetF();
	self->setScreenPositionf(x, y);
}

static void n_getposition(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	int x, y;
	self->getPosition(x, y);
	cmd->Out()->SetI(x);
	cmd->Out()->SetI(y);
}

static void n_getpositionf(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	float x, y;
	self->getPositionf(x, y);
	cmd->Out()->SetF(x);
	cmd->Out()->SetF(y);
}

static void n_setsize(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	int x = cmd->In()->GetI();
	int y = cmd->In()->GetI();
	self->setScreenSize(x, y);
}

static void n_setsizef(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	float x = cmd->In()->GetF();
	float y = cmd->In()->GetF();
	self->setScreenSizef(x, y);
}

static void n_getsize(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	int x, y;
	self->getScreenSize(x, y);
	cmd->Out()->SetI(x);
	cmd->Out()->SetI(y);
}

static void n_getsizef(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	float x, y;
	self->getScreenSizef(x, y);
	cmd->Out()->SetF(x);
	cmd->Out()->SetF(y);
}

static void n_setautoscale(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*)slf;
	self->setAutoScale(cmd->In()->GetB());
}

static void n_getautoscale(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*)slf;
	cmd->Out()->SetB(self->getAutoScale());
}

static void n_setvisible(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*)slf;
	self->setVisible(cmd->In()->GetB());
}

static void n_getvisible(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*)slf;
	cmd->Out()->SetB(self->getVisible());
}

static void n_setsourceposition(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	int x = cmd->In()->GetI();
	int y = cmd->In()->GetI();
	self->setSourcePosition(x, y);
}

static void n_setsourcepositionf(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	float x = cmd->In()->GetF();
	float y = cmd->In()->GetF();
	self->setSourcePositionf(x, y);
}

static void n_getsourceposition(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	int x, y;
	self->getSourcePosition(x, y);
	cmd->Out()->SetI(x);
	cmd->Out()->SetI(y);
}

static void n_getsourcepositionf(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	float x, y;
	self->getSourcePositionf(x, y);
	cmd->Out()->SetF(x);
	cmd->Out()->SetF(y);
}

static void n_setsourcesize(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	int x = cmd->In()->GetI();
	int y = cmd->In()->GetI();
	self->setSourceSize(x, y);
}

static void n_setsourcesizef(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	float x = cmd->In()->GetF();
	float y = cmd->In()->GetF();
	self->setSourceSizef(x, y);
}

static void n_getsourcesize(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	int x, y;
	self->getSourceSize(x, y);
	cmd->Out()->SetI(x);
	cmd->Out()->SetI(y);
}

static void n_getsourcesizef(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	float x, y;
	self->getSourceSizef(x, y);
	cmd->Out()->SetF(x);
	cmd->Out()->SetF(y);
}

static void n_getimagesize(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;
	int x, y;
	self->getImageSize(x, y);
	cmd->Out()->SetI(x);
	cmd->Out()->SetI(y);
}

static void n_getorder(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;

	cmd->Out()->SetI(self->getOrder());
}

static void n_setorder(void *slf, nCmd *cmd) {
	nCSprite *self = (nCSprite*) slf;

	self->setOrder(cmd->In()->GetI());
}

//------------------------------------------------------------------------------

/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nCSprite::SaveCmds(nPersistServer* ps)
{
	n_assert("nCSprite::SaveCmds no implementada.");
    if (nVisNode::SaveCmds(ps))
    {
        nCmd* cmd = ps->GetCmd(this, 'BLDR');
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}

