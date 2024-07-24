#define N_IMPLEMENTS nEntity
//------------------------------------------------------------------------------
//  (C) 2002	ling
//------------------------------------------------------------------------------
#include "laberintosultan/nentity.h"
#include "kernel/npersistserver.h"

static void n_getvisnode(void* slf, nCmd* cmd);
static void n_setvisnode(void* slf, nCmd* cmd);
static void n_setcollclass(void* slf, nCmd* cmd);
static void n_getcollclass(void* slf, nCmd* cmd);
static void n_setcollshape(void* slf, nCmd* cmd);
static void n_getcollshape(void* slf, nCmd* cmd);
static void n_setposition(void* slf, nCmd* cmd);
static void n_getposition(void *slf, nCmd *cmd);
static void n_setcommand(void *slf, nCmd *cmd);
static void n_settouchmethod(void *slf, nCmd *cmd);
static void n_gettouchmethod(void *slf, nCmd *cmd);
static void n_getcontactnormal(void *slf, nCmd *cmd);
static void n_gettouchedentity(void *slf, nCmd *cmd);
static void n_setvisible(void *slf, nCmd *cmd);
static void n_getvisible(void *slf, nCmd *cmd);
static void n_processcollision(void *slf, nCmd *cmd);
static void n_getentityheight(void *slf, nCmd *cmd);
static void n_setentityheight(void *slf, nCmd *cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nentity
    
    @superclass
    nroot

    @classinfo
    A detailed description of what the class does (written for script programmers!) 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setvisnode_s", 'SVIS', n_setvisnode);
    clazz->AddCmd("s_getvisnode_v", 'GVIS', n_getvisnode);
	clazz->AddCmd("v_setvisible_b", 'SVIB', n_setvisible);
	clazz->AddCmd("b_getvisible_v", 'GVIB', n_getvisible);
    clazz->AddCmd("v_setcollclass_s", 'SCLC', n_setcollclass);
    clazz->AddCmd("s_getcollclass_v", 'GCLC', n_getcollclass);
    clazz->AddCmd("v_setcollshape_s", 'SCSH', n_setcollshape);
    clazz->AddCmd("s_getcollshape_v", 'GCSH', n_getcollshape);
    clazz->AddCmd("v_setposition_ff", 'SPOS', n_setposition);
	clazz->AddCmd("ff_getposition_v", 'GPOS', n_getposition);
	clazz->AddCmd("v_settouchmethod_s", 'STMT', n_settouchmethod);
	clazz->AddCmd("s_gettouchmethod_v", 'GTMT', n_gettouchmethod);
	clazz->AddCmd("v_setcommand_sb", 'SCMD', n_setcommand);
	clazz->AddCmd("fff_getcontactnormal_v", 'GCNM', n_getcontactnormal);
	clazz->AddCmd("s_gettouchedentity_v", 'GTEN', n_gettouchedentity);
	clazz->AddCmd("v_processcollision_v", 'PCOL', n_processcollision);
	clazz->AddCmd("f_getentityheight_v", 'GENH', n_getentityheight);
	clazz->AddCmd("v_setentityheight_f", 'SENH', n_setentityheight);
    clazz->EndCmds();
}
//------------------------------------------------------------------------------

static void n_gettouchmethod(void *slf, nCmd *cmd) {
	nEntity *self = (nEntity*)slf;
	cmd->Out()->SetS(self->GetTouchMethod());
}
//------------------------------------------------------------------------------
static void n_settouchmethod(void *slf, nCmd *cmd) {
	nEntity *self = (nEntity*)slf;
	self->SetTouchMethod(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvisnode

    @input
    v

    @output
    s(nVisNode)

    @info
    Gets the full path of the associated visual node.
*/
static
void
n_getvisnode(void* slf, nCmd* cmd)
{
    nEntity* self = (nEntity*) slf;
    cmd->Out()->SetS(self->GetVisNode());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvisnode

    @input
    s(nVisNode)

    @output
    v

    @info
    Sets visual node of the entity.
*/
static
void
n_setvisnode(void* slf, nCmd* cmd)
{
    nEntity* self = (nEntity*) slf;
    self->SetVisNode(cmd->In()->GetS());
}


static void n_getvisible(void *slf, nCmd *cmd) {
	nEntity *self = (nEntity*) slf;
	cmd->Out()->SetB(self->GetVisible());
}

static void n_setvisible(void *slf, nCmd *cmd) {
	nEntity *self = (nEntity*) slf;
	self->SetVisible(cmd->In()->GetB());
}
//------------------------------------------------------------------------------
/**
    @cmd
    setposition

    @input
    f(x position), f(y position)

    @output
    v

    @info
    Sets the position of the entity on the terrain
*/
static
void
n_setposition(void* slf, nCmd* cmd)
{
    nEntity* self = (nEntity*) slf;
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    self->SetPosition(x, y);
}


static void n_getposition(void *slf, nCmd *cmd) {
	nEntity *self = (nEntity*) slf;
	vector3 pos;
	pos = self->GetPosition();

	cmd->Out()->SetF(pos.x);
	cmd->Out()->SetF(pos.z);
}


/**
    @cmd
    setcollclass

    @input
    s (CollideClass)

    @output
    v

    @info
    Sets the collide class.
*/
static
void
n_setcollclass(void* slf, nCmd* cmd)
{
    nEntity* self = (nEntity*) slf;
    self->SetCollideClass(cmd->In()->GetS());
}


static void n_getcollclass(void *slf, nCmd *cmd) {
	nEntity *self = (nEntity*) slf;
	cmd->Out()->SetS(self->GetCollideClass());
}
//------------------------------------------------------------------------------
/**
    @cmd
    setcolleshape

    @input
    s (CollideShapeFilename)

    @output
    v

    @info
    Sets the collide shape.
*/
static
void
n_setcollshape(void* slf, nCmd* cmd)
{
    nEntity* self = (nEntity*) slf;
    self->SetCollideShape(cmd->In()->GetS());
}

static void n_getcollshape(void *slf, nCmd *cmd) {
	nEntity *self = (nEntity*) slf;
	cmd->Out()->SetS(self->GetCollideShape());
}


//------------------------------------------------------------------------------
/**
	@cmd
	setcommand

	@input
	s(forward, backward, left o right), b(Activar / desactivar)

    @output
	v

	@info
	Establece el movimiento que se desea de la entidad
*/
static void n_setcommand(void *slf, nCmd *cmd) {

	nEntity *self = (nEntity*)slf;

	const char *com = cmd->In()->GetS();
	bool set = cmd->In()->GetB();

	if (strcmp("forward", com) == 0) 
		self->SetCommand(nEntity::CMD_FORWARD, set);
	else if (strcmp("backward", com) == 0) 
		self->SetCommand(nEntity::CMD_BACKWARD, set);
	else if (strcmp("left", com) == 0) 
		self->SetCommand(nEntity::CMD_LEFT, set);
	else if (strcmp("right", com) == 0) 
		self->SetCommand(nEntity::CMD_RIGHT, set);
	else if (strcmp("jump", com) == 0) 
		self->SetCommand(nEntity::CMD_JUMP, set);
}
//------------------------------------------------------------------------------
static void n_getentityheight(void *slf, nCmd *cmd) {
	nEntity *self = (nEntity*)slf;
	cmd->Out()->SetF(self->GetEntityHeight());
}
//------------------------------------------------------------------------------
static void n_setentityheight(void *slf, nCmd *cmd) {
	nEntity *self = (nEntity*)slf;
	self->SetEntityHeight(cmd->In()->GetF());
}
//------------------------------------------------------------------------------

static void n_getcontactnormal(void *slf, nCmd *cmd) {

	nEntity *self = (nEntity*)slf;

	vector3 cn = self->GetContactNormal();

	cmd->Out()->SetF(cn.x);
	cmd->Out()->SetF(cn.y);
	cmd->Out()->SetF(cn.z);
}
//------------------------------------------------------------------------------
void n_gettouchedentity(void *slf, nCmd *cmd) {

	nEntity *self = (nEntity*)slf;

	cmd->Out()->SetS(self->GetTouchedEntity());
}
//------------------------------------------------------------------------------
void n_processcollision(void *slf, nCmd *cmd) {

	nEntity *self = (nEntity*)slf;

	self->processCollision();
}


//------------------------------------------------------------------------------
/**
    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nEntity::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nCmd* cmd;

        // setvisnode
        cmd = ps->GetCmd(this, 'SVIS');
        if (NULL != cmd)
        {
            cmd->In()->SetS(this->GetVisNode());
            ps->PutCmd(cmd);
        }

		// setvisible
		cmd = ps->GetCmd(this, 'SVIB');
		if (NULL != cmd) {
			cmd->In()->SetB(this->GetVisible());
			ps->PutCmd(cmd);
		}

        // setposition
        cmd = ps->GetCmd(this, 'SPOS');
        if (NULL != cmd)
        {
            cmd->In()->SetF(this->positionVector.x);
            cmd->In()->SetF(this->positionVector.y);
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
