#define N_IMPLEMENTS nLabel

#include "text/nLabel.h"
#include "kernel/npersistserver.h"

static void n_loadfont(void* slf, nCmd* cmd);
static void n_settext(void*slf, nCmd* cmd);
static void n_setposition(void *slf, nCmd *cmd);
static void n_setpositionf(void *slf, nCmd *cmd);
static void n_setalign(void *slf, nCmd *cmd);
static void n_getcolor(void *slf, nCmd *cmd);
static void n_setcolor(void *slf, nCmd *cmd);
static void n_setorder(void *slf, nCmd *cmd);
static void n_getorder(void *slf, nCmd *cmd);

//------------------------------------------------------------------------------
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_loadfont_s", 'VLFS', n_loadfont);
    clazz->AddCmd("v_settext_s",  'VSTS', n_settext);
	clazz->AddCmd("v_setposition_ii", 'SPOS', n_setposition);
	clazz->AddCmd("v_setpositionf_ff", 'SPOF', n_setpositionf);
	clazz->AddCmd("v_setalign_s", 'SALI', n_setalign);
	clazz->AddCmd("v_setcolor_fff", 'SCOL', n_setcolor);
	clazz->AddCmd("fff_getcolor_v", 'GCOL', n_getcolor);
	clazz->AddCmd("v_setorder_i", 'SORD', n_setorder);
	clazz->AddCmd("i_getorder_v", 'GORD', n_getorder);

    clazz->EndCmds();
}

static void n_setcolor(void *slf, nCmd *cmd) {
	float r, g, b;
	nLabel *self = (nLabel*)slf;

	r = cmd->In()->GetF();
	g = cmd->In()->GetF();
	b = cmd->In()->GetF();

	self->setColor(r, g, b);
}

static void n_getcolor(void *slf, nCmd *cmd) {
	float r, g, b;
	nLabel *self = (nLabel*)slf;

	self->getColor(r, g, b);

	cmd->Out()->SetF(r);
	cmd->Out()->SetF(g);
	cmd->Out()->SetF(b);
}

static void n_setalign(void *slf, nCmd *cmd) {
	nLabel::Align type;
	nLabel *self = (nLabel*)slf;

	const char *com = cmd->In()->GetS();

	if (strcmp("left", com) == 0)
		type = nLabel::Align::A_LEFT;
	else if (strcmp("center", com) == 0)
		type = nLabel::Align::A_CENTER;
	else if (strcmp("right", com) == 0)
		type = nLabel::Align::A_RIGHT;
	else
		return;

	self->setAlign(type);
}

//------------------------------------------------------------------------------

static
void
n_loadfont(void* slf, nCmd* cmd)
{
    nLabel* self = (nLabel*) slf;
    self->loadFont((char*)cmd->In()->GetS()); //Llamamos a la funcion en C
}

//------------------------------------------------------------------------------

static
void
n_settext(void* slf, nCmd* cmd)
{
    nLabel* self = (nLabel*) slf;
    self->setText((char*)cmd->In()->GetS()); //Llamamos a la funcion en C
}

//------------------------------------------------------------------------------
static void n_setposition(void *slf, nCmd *cmd) {
	nLabel *self = (nLabel*)slf;
	int x, y;
	x = cmd->In()->GetI();
	y = cmd->In()->GetI();
	self->setScreenPosition(x, y);
}

static void n_setpositionf(void *slf, nCmd *cmd) {
	nLabel *self = (nLabel*)slf;
	float x, y;
	x = cmd->In()->GetF();
	y = cmd->In()->GetF();
	self->setScreenPositionf(x, y);
}

static void n_getorder(void *slf, nCmd *cmd) {
	nLabel *self = (nLabel*) slf;

	cmd->Out()->SetI(self->getOrder());
}

static void n_setorder(void *slf, nCmd *cmd) {
	nLabel *self = (nLabel*) slf;

	self->setOrder(cmd->In()->GetI());
}


//------------------------------------------------------------------------------

bool
nLabel::SaveCmds(nPersistServer* ps)
{
    if (nVisNode::SaveCmds(ps))
    {
        nCmd* cmd = ps->GetCmd(this, 'VLFS');
        ps->PutCmd(cmd);
		cmd = ps->GetCmd(this,'VSFS');
		ps->PutCmd(cmd);
        return true;
    }
    return false;
}
