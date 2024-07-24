#define N_IMPLEMENTS nEntity
//------------------------------------------------------------------------------
//  (C) 2002	ling
//------------------------------------------------------------------------------
#include "collide/ncollideserver.h"
#include "collide/ncollideobject.h"
#include "gfx/nchannelserver.h"
#include "node/n3dnode.h"
#include "laberintosultan/nworld.h"
#include "laberintosultan/nentity.h"

nNebulaScriptClass(nEntity, "nroot");

//------------------------------------------------------------------------------
/**
*/
nEntity::nEntity() :
    refWorld(this),
    refChannelServer(kernelServer, this),
    refVisNode(kernelServer, this),
	refScriptServer(kernelServer, this),
	visible(true),
    // Properties
    entityMass(1.0f),
    inverseMass(1.0f/entityMass),
    entityHeight(0.0f),
    // Constants
    maxAcceleration(80.0f),
    maxDeceleration(50.0f),
    maxForwardSpeed(60.0f),
    maxReverseSpeed(20.0f),
    turnRate(2.0f),
	jumpRate(800.0f),
	potJump(50.0f),
    rollRate(1.0f),
    // Current
    curSpeed(0.0f),
    curHeading(0.0f),
    curHeadingVector(),
    curRoll(0.0f),
    curAccel(0.0f),
    curTravel(0.0f),
    // Command
    cmdSpeed(0.0f),
    cmdTurn(0.0f),
	cmdJump(0.0f),
    cmdRoll(0.0f),
    // Physics
    forceVector(),
    velocityVector(),
    positionVector(),
    // Visual
    transformMatrix()
	,
    // Collide
    collideClass(),
    collideShape(),
    collideObject(NULL),
	contactNormal(0, 0, 0)
{
    refChannelServer = "/sys/servers/channel";
	refScriptServer = "/sys/servers/script";

    curHeadingVector.x = -n_sin(curHeading);
    curHeadingVector.y = 0;
    curHeadingVector.z = n_cos(curHeading);

	sprintf(touchMethod, "");
	sprintf(touchedEntity, "");
	
}

//------------------------------------------------------------------------------
/**
*/
nEntity::~nEntity()
{
}

/**
    Find nWorld parent object somewhere up hierarchy and link self into
    simulation loop.
*/
void nEntity::Initialize()
{
    nRoot* parent = this;
    while (parent = parent->GetParent())
    {
        if (true == parent->IsA(kernelServer->FindClass("nworld")))
        {
            this->refWorld = (nWorld*)parent;
        }
    }

	nWorld* world = (nWorld*)this->GetParent();
	positionVector.y=world->GetHeight(positionVector);

    nRoot::Initialize();
}

/**
*/
void nEntity::SetCommand(Command cmd, bool set)
{
	nWorld* world = (nWorld*)this->GetParent();
    switch (cmd)
    {
        default:
            break;
        case CMD_JUMP:
            cmdJump = (set && positionVector.y<world->GetHeight(positionVector)+0.1f) ? jumpRate : 0.0f;
            break;
        case CMD_FORWARD:
            cmdSpeed = (set) ? this->maxForwardSpeed : 0.0f;
            break;
        case CMD_BACKWARD:
            cmdSpeed = (true == set) ? -this->maxReverseSpeed : 0.0f;
            break;
        case CMD_LEFT:
            cmdTurn = (true == set) ? turnRate : 0.0f;
            break;
        case CMD_RIGHT:
            cmdTurn = (true == set) ? -turnRate : 0.0f;
            break;
    }

}

/**
*/
void nEntity::ApplyForce(const vector3& force)
{
    forceVector += force;
}

/**
    @brief Sets the position of the entity on the map.

    Automatically aligns height to world.
*/
void nEntity::SetPosition(float x, float y)
{
    positionVector.x = x;
    positionVector.z = y;
}

/**
    @brief Called during an attach loop.
*/
n3DNode* nEntity::Attach(void)
{
    if (false == refVisNode.isvalid())
        return NULL;

	if (!visible)
		return NULL;

    nChannelServer* chan_server = refChannelServer.get();
    chan_server->SetChannel1f(refChannelServer->GenChannel("travel"), curTravel / (360.0f * PI * entityHeight));// / (2.0f * PI * 2.0f));
    chan_server->SetChannel1f(refChannelServer->GenChannel("vel_x"), velocityVector.x);
    chan_server->SetChannel1f(refChannelServer->GenChannel("vel_y"), velocityVector.y);
    chan_server->SetChannel1f(refChannelServer->GenChannel("vel_z"), velocityVector.z);
    chan_server->SetChannel1f(refChannelServer->GenChannel("engine"), fabsf(curSpeed/maxForwardSpeed));

    // Load transform into visible node prototype and pass it up for sacrifice
    refVisNode->M(transformMatrix);
    return refVisNode.get();
}

//------------------------------------------------------------------------------
// Simulation loop stuff

/**
    @brief Resolve collisions.
    Imparts a force on opposite object.
*/

void nEntity::Collide()
{
    if (NULL == collideObject)
        return;

    nCollideReport** report;
    int num_colls = collideObject->GetCollissions(report);
    if (0 == num_colls)
        return;

    for (int i = 0; i < num_colls; ++i)
    {
        nEntity* other = (nEntity*)report[i]->co2->GetClientData();
        vector3* contact_normal = &report[i]->co1_normal;
        if (this == other)
        {
            other = (nEntity*)report[i]->co1->GetClientData();
            contact_normal = &report[i]->co2_normal;
        }

        if (NULL == other)
            continue;

		// Si hay script para procesar la colisión, lo llamamos;
		// si no, el comportamiento por defecto. Se comprueba
		// si hay procedimiento de colisión, por si en una colisión,
		// el script lo elimina.
		if (touchMethod[0] != '\0') {//strlen(touchMethod) > 0) {
			nRoot *currDir = kernelServer->GetCwd();
			kernelServer->SetCwd(this);
			contactNormal = *contact_normal;
			other->GetFullName(touchedEntity, N_MAXPATH);
			const char* result;
			refScriptServer->Run(touchMethod, result);
			kernelServer->SetCwd(currDir);
		} else
			processCollision(other, contact_normal);
    }

	sprintf(touchedEntity, "");
}

/**
Función llamada desde el script, cuando se quiere procesar una colisión
con la forma por defecto. Coge los datos de la última colisión detectada.
*/

void nEntity::processCollision() {
	n_assert(touchedEntity[0] != '\0');

	nEntity *other = (nEntity*)kernelServer->Lookup(touchedEntity);
	processCollision(other, &contactNormal);
}


/*
Procesa la colisión de la entidad con otra.
*/

void nEntity::processCollision(const nEntity *other, const vector3 *contact_normal) {

    // We don't do height
    vector3 rel_velocity = velocityVector - other->velocityVector;
    rel_velocity.y = 0;

    // Coefficient of restitution, pretty low coz they're tanks, not powerballs
    // Inverse mass used here seem to cancel out... ?
    float e = 0.15f;
    float impulse_numerator = -(1.0f + e) * (rel_velocity % *contact_normal);
    float impulse_denominator = inverseMass;
    vector3 impulse = *contact_normal * (impulse_numerator/impulse_denominator);
    velocityVector += impulse * inverseMass;

    // :HACK: Just to ensure penetration goes away
    positionVector += *contact_normal * -0.1f;
    // :NOTE: If I was cunning, I would use collideObject->LineCheck() to
    // pick out the contact point and shift the entity in proportion to
    // its mass in that direction, something like the below.
    // COLLTYPE_CONTACT or COLLTYPE_EXACT
    // line3 test(transformMatrix.pos_component(), constant * *contact_normal;
    //nCollideReport line_report;
    //bool result = collideObject->GetShape()->LineCheck(COLLTYPE_CONTACT, transformMatrix,
    //                                                   test, line_report);
    // :NOTE2: If I was more cunning, I'd hack a routine to get the outermost
    // contact point and translate the objects appropriately.
    // :NOTE3: Alternatively, maintain a list of already collided objects
    // and don't apply more impulses to them.
}

/**
    @brief Process commands and such
*/
void nEntity::Trigger(float dt)
{
    // Process heading, +ve is anti-clockwise
    /* For command heading
    float heading_diff = cmd_heading - curHeading;
    if (float(PI/360.0f) < fabsf(heading_diff))
    */
    if (0.0f != cmdTurn)
    {
        /* For command heading
        float delta = n_min(dt * turnRate, fabsf(heading_diff));

        // Other direction
        if (0.0f > heading_diff)
            delta = -delta;

        // Reverse direction if difference is more than PI
        if (PI < fabsf(heading_diff))
            delta = -delta;
        */
        float delta = dt * cmdTurn;

        curHeading = fmodf(curHeading + delta, 2.0f * PI);
        if (0.0f > curHeading)
            curHeading += 2.0f * PI;

        curHeadingVector.x = n_sin(curHeading);
        curHeadingVector.y = 0;
        curHeadingVector.z = n_cos(curHeading);

        // Roll into turn for style points,
        // remember to always roll into turn
        cmdRoll = -cmdTurn / 3.0f;
        if (0.0f > cmdSpeed)
            cmdRoll = -cmdRoll;
    }
    else
        cmdRoll = 0.0f;

    // Process roll
    float roll_delta = cmdRoll - curRoll;
    if (0.0f != roll_delta)
    {
        float delta = n_min(dt * rollRate, fabsf(roll_delta));

        // Other direction
        if (0.0f > roll_delta)
            delta = -delta;

        curRoll += delta;
    }
}

/**
    @brief Compute forces acting upon entity.

    In proper physics sims, this would include constraints but since this is
    me doing a game, they are random numbers I found on the desk.
*/
void nEntity::ComputeForces()
{
	//forceVector=curHeadingVector;

    // Apply gravity
	nWorld* world = (nWorld*)this->GetParent();
    forceVector.y += world->GetGravity() / inverseMass;

    // Apply other nice things like constraints
	if (positionVector.y<=world->GetHeight(positionVector)) {
		positionVector.y=world->GetHeight(positionVector);
		forceVector.y=-forceVector.y*0.2f; //Pierde energía al rebotar en el suelo
	}

	if (positionVector.y>world->GetHeight(positionVector)-1) {
		forceVector.y+=cmdJump;
		cmdJump=(cmdJump>0?cmdJump-potJump:0.0f); //Para que no tengamos un salto infinito
	}

    // Apply acceleration
    float speed_delta = cmdSpeed - curSpeed;
    if (0.0f != speed_delta)
    {
        float accel;
        
        // Accelerating
        if (fabsf(cmdSpeed) > fabsf(curSpeed))
            accel = maxAcceleration;
        // Decelerating
        else
            accel = maxDeceleration;

        accel = n_min(accel, fabsf(speed_delta));

        // Reversing
        if (0.0f > speed_delta)
            accel = -accel;

        curAccel = accel;

        // Apply acceleration force
        forceVector += curHeadingVector * (2.0f * accel * entityMass);
    }
    else
        curAccel = 0.0f;

    // Be nice to calculate terrain's normal force here

    // Assuming ground contact, apply contact friction
    // Friction calculations below should be split into longitudal and
    // lateral calculations with respect to orientation.
    float speed = velocityVector % curHeadingVector;
    forceVector += curHeadingVector * (-1.0f * entityMass * speed);
    vector3 lateral_velocity = velocityVector - (curHeadingVector * speed);
    forceVector += lateral_velocity * (-1.5f * entityMass);

    // Apply drag, in opposite direction heading
    // where aerodynamic drag is cubed function of speed
}

/**
    @brief Integrate physics bits.

    Using Euler, how embarassing.  Hopefully won't go unstable because
    rotational dynamics are not provided for.
*/
void nEntity::Integrate(float dt)
{
    // Update current position with velocity
    positionVector += velocityVector * dt;

    // Update velocity with F = ma, integrated
    velocityVector += (forceVector * inverseMass) * dt;
    //velocityVector.y = 0;

    // Zero force
    forceVector.set(0, 0, 0);
}

/**
    Apply additional constraints or effects after pseudo-physics calculations.
*/
void nEntity::Update()
{
    // Get current speed, dot product of velocity and heading vectors
    curSpeed = velocityVector % curHeadingVector;
    // curTravel += std::max(curSpeed, curAccel);
    curTravel += curSpeed;

    // Update transform matrix, which is applied to both the visual and
    // collision objects
    transformMatrix.ident();

    // Apply roll transform hack, apologies in advance
    transformMatrix.translate(0.0f, entityHeight, 0.0f);
    transformMatrix.rotate_z(curRoll);
    transformMatrix.translate(0.0f, -entityHeight, 0.0f);

    transformMatrix.rotate_y(curHeading);
    transformMatrix.translate(0.0f, entityHeight, 0.0f);
    transformMatrix.translate(positionVector);

    // First argument is timestamp, which is not used here (but should be)
    collideObject->Transform(0.0f, transformMatrix);
}

//---------------------------------------------------------------------------
// Prototype definition methods

/**
    @brief Get the associated visual node path name.
    :TODO: Not threadsafe and a wee hackish.
*/
const char* nEntity::GetVisNode()
{
    static char buffer[256];
    this->refVisNode->GetFullName(buffer, 256);
    return buffer;
}

/**
    @brief Set the associated visual node for presentation.
*/
void nEntity::SetVisNode(const char* visnode_path)
{
    n_assert(visnode_path);
	visNode = visnode_path;
	refVisNode = visNode.Get();
}

void nEntity::Tick(float dt) {

    Trigger(dt);
    ComputeForces();
    Integrate(dt);
	//this->SetPosition(positionVector.x,positionVector.z);
    // Sort of a fake here, arrange entity
    Update();
}


void nEntity::SetCollideClass(const char* coll_class)
{
    n_assert(NULL != coll_class && "Invalid collide class string");
    collideClass = coll_class;

    if (true == refWorld.isvalid())
    {
        if (NULL == collideObject)
            collideObject = refWorld->NewCollideObject(this);
        nCollideServer* coll_server = refWorld->GetCollideServer();
        collideObject->SetCollClass(coll_server->QueryCollClass(coll_class));
    }
}


/**
    @brief Initialise collide using entity name for collide shape handle.
    Sets the prototype collision class and shape.  The shape is stored under
    the name of the prototype to ensure lack of sharing.  This is not so
    flexible but cross that bridge when it becomes an issue.

    The existance of collideClass and collideShape variables are pretty much
    unnecessary since they are only used once during entity type creation,
    after that they are just duplicating information found in collideObject.
*/
void nEntity::SetCollideShape(const char* coll_shape)
{
    n_assert(NULL != coll_shape && "Invalid collide shape string");

	// Si ya teníamos una colisión, y nos piden que carguemos la misma,
	// asumimos que es porque la malla de colisión ha cambiado.

	bool reload = false;
	if (collideShape == coll_shape)
		reload = true;

    collideShape = coll_shape;

    if (true == refWorld.isvalid())
    {
        if (NULL == collideObject)
            collideObject = refWorld->NewCollideObject(this);
        nCollideShape* shape = refWorld->NewCollideShape(coll_shape, coll_shape, reload);
        collideObject->SetShape(shape);
    }
}

const char* nEntity::GetCollideClass() const
{
    return collideClass.Get();
}

const char* nEntity::GetCollideShape() const
{
    return collideShape.Get();
}


// Establece el metodo de TCL a llamar.
void nEntity::SetTouchMethod(const char *m) {
	n_assert((int)strlen(m) < N_MAXPATH);

	sprintf(touchMethod, "%s", m);
}
