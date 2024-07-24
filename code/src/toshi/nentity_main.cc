#define N_IMPLEMENTS nEntity
//------------------------------------------------------------------------------
//  (C) 2002	ling
//------------------------------------------------------------------------------
#include "collide/ncollideserver.h"
#include "collide/ncollideobject.h"
#include "gfx/nchannelserver.h"
#include "node/n3dnode.h"
#include "toshi/nworld.h"
#include "toshi/nentity.h"
#include "toshi/ncampesino.h"
#include "toshi/nmapa.h"

static int const NUM_MAX_COLISIONES = 500;

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
    // Constants
	velocidadAndar(0.775f),
	velocidadCorrer(2.0f),
	ejecutarScript(false),
	nombreEntidad(""),
    // Physics
    velocityVector(),
    positionVector(),
	siguienteVector(),
	destinoVector(),
    // Visual
    transformMatrix(),
	scaleVector(1, 1, 1),
	rotateVector(0, 0, 0),
    // Collide
    collideClass(),
    collideShape(),
    collideObject(NULL),
	contactNormal(0, 0, 0)
{
    refChannelServer = "/sys/servers/channel";
	refScriptServer = "/sys/servers/script";

	sprintf(touchMethod, "");
	sprintf(touchedEntity, "");
	mapa=NULL;
	//personaje=NULL;
}

//------------------------------------------------------------------------------
/**
*/
nEntity::~nEntity()
{
	collideObject->Remove();
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
    nRoot::Initialize();
}

void nEntity::SetPosition(float x, float y)
{
    positionVector.x = x;
    positionVector.z = y;
	destinoVector.set(x,0,y);
	siguienteVector.set(x,0,y);
	//velocityVector.set(0,0,0);
}

void nEntity::Rxyz(float x, float y, float z)
{
    rotateVector.x = n_deg2rad(x);
	rotateVector.y = n_deg2rad(y);
    rotateVector.z = n_deg2rad(z);
}

void nEntity::Txyz(float x, float y, float z)
{
    positionVector.x = x;
	positionVector.y = y;
    positionVector.z = z;
}

void nEntity::Sxyz(float x, float y, float z)
{
    scaleVector.x = x;
	scaleVector.y = y;
    scaleVector.z = z;
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

    /*nChannelServer* chan_server = refChannelServer.get();
    chan_server->SetChannel1f(refChannelServer->GenChannel("travel"), curTravel / (360.0f * PI * entityHeight));// / (2.0f * PI * 2.0f));
    chan_server->SetChannel1f(refChannelServer->GenChannel("vel_x"), velocityVector.x);
    chan_server->SetChannel1f(refChannelServer->GenChannel("vel_y"), velocityVector.y);
    chan_server->SetChannel1f(refChannelServer->GenChannel("vel_z"), velocityVector.z);
    chan_server->SetChannel1f(refChannelServer->GenChannel("engine"), fabsf(curSpeed/maxForwardSpeed));
	*/
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

bool nEntity::Collide()
{
    if (NULL == collideObject)
        return false;

	if (!this->GetVisible())
		return false;

    nCollideReport** report;
    int num_colls = collideObject->GetCollissions(report);
    if (0 == num_colls)
        return false;

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

		if (!other->GetVisible())
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
			//char path[200];
			//sprintf(path,"puts \"entidad %s \"",touchedEntity);
			//refScriptServer->Run(path, result);
		}
		else
			processCollision(other, contact_normal);
    }
	sprintf(touchedEntity, "");
	return true;
}

/**
Función llamada desde el script, cuando se quiere procesar una colisión
con la forma por defecto. Coge los datos de la última colisión detectada.
*/

void nEntity::processCollision()
{
	n_assert(touchedEntity[0] != '\0');

	nEntity *other = (nEntity*)kernelServer->Lookup(touchedEntity);
	processCollision(other, &contactNormal);
}


/*
Procesa la colisión de la entidad con otra.
*/
void nEntity::processCollision( nEntity *other, const vector3 *contact_normal )
{
    // We don't do height
    //vector3 rel_velocity = velocityVector - other->velocityVector;
    //rel_velocity.y = 0;

    // Coefficient of restitution, pretty low coz they're tanks, not powerballs
    // Inverse mass used here seem to cancel out... ?
    //float e = 0.15f;
    //float impulse_numerator = -(1.0f + e) * (rel_velocity % *contact_normal);
    //float impulse_denominator = inverseMass;
    //vector3 impulse = *contact_normal * (impulse_numerator/impulse_denominator);
	quaternion aux;
	vector3    posicionContrario        = other->positionVector;
	vector3    velocidadContrario       = other->velocityVector;
	vector3    direccionContrario       = posicionContrario-positionVector;
	vector3    velocidadEntidad         = velocityVector;
	float      anguloDestino            = 0.0f;
	float      anguloVelocidadContrario = 0.0f;
	float      angulo                   = 0.0f;
	angulo                   = CalcularAngulo( velocidadEntidad );
	anguloDestino            = CalcularAngulo( direccionContrario );
	anguloVelocidadContrario = CalcularAngulo( velocidadContrario );
	float anguloi  = n_rad2deg( angulo );
	float angulod  = n_rad2deg( anguloDestino );
	float angulovc = n_rad2deg( anguloVelocidadContrario );
	aux.set( velocidadEntidad.x, 0.0f, velocidadEntidad.z, 0.0f );

	if ( strcmp( this->GetClass()->GetName(), "ncampesino" ) == 0        &&
		 ( strcmp( other->GetClass()->GetName(), "ncampesino" ) == 0  ||
		   strcmp( other->GetClass()->GetName(), "npersonaje" ) == 0  ||
			strcmp( other->GetClass()->GetName(), "nkunoichi" ) == 0  ||
			strcmp( other->GetClass()->GetName(), "nladrona" ) == 0  ||
			strcmp( other->GetClass()->GetName(), "nsamurai" ) == 0  ||
			strcmp( other->GetClass()->GetName(), "nshogun" ) == 0  ||
			strcmp( other->GetClass()->GetName(), "nmercader" ) == 0  ||
			strcmp( other->GetClass()->GetName(), "nmaestro" ) == 0  ||
			strcmp( other->GetClass()->GetName(), "ningeniero" ) == 0  ||
			strcmp( other->GetClass()->GetName(), "nshugenja" ) == 0
		   ))
	{
		nCampesino* campesino = (nCampesino*) this;
		if ( !campesino->EstaContratado() )
		{
			campesino->Colisionar();
			if ( campesino->GetNumColisiones() > NUM_MAX_COLISIONES )
			{
				//n_printf( "SE LE PARA AL CAMPESINO %d POR COLISIONAR CON %s\n", campesino->GetId(), other->GetName() );
				this->destinoVector = this->positionVector;
			}
		}
	}

	//n_printf("Angulo inicial:%f %f AnguloDestino:%f %f AnguloVelocidadContrario:%f %f\n",angulo,anguloi+180,anguloDestino,angulod+180,anguloVelocidadContrario,angulovc+180);
	if ( anguloi < angulod )
	{
		//n_printf( "Angulo de velocidad de entidad %f menor que direccion contrario %f\n", anguloi, angulod );
		anguloi += 360;
	}

	if ( anguloi < angulovc )
	{
		//n_printf( "Angulo de velocidad de entidad %f menor que angulo de velocidad contrario %f\n", anguloi, angulovc );
		anguloi += 360;
	}

	if ( anguloi - angulovc < 5 )
	{
		//n_printf( "Angulo de velocidad de entidad %f menor que angulo de velocidad contrario + 2º %f\n", anguloi, angulovc );
		this->destinoVector = this->positionVector - direccionContrario*2;
	}
	else if ( anguloi - angulod < 90 )
	{
		//n_printf( "Angulo de velocidad de entidad %f menor que direccion contrario + 90º %f\n", anguloi, angulod );
		aux.set_rotate_y( n_deg2rad( 5.0f ) );
		//n_printf("Menos fuerte izquierda %f %f %f\n",(anguloi-angulod),anguloi, angulod);
	}
	else if ( anguloi - angulod < 180 )
	{
		//n_printf( "Angulo de velocidad de entidad %f menor que direccion contrario + 180º %f\n", anguloi, angulod );
		aux.set_rotate_y( n_deg2rad( 2.0f ) );
		//n_printf("Mas fuerte izquierda %f %f %f\n",(anguloi-angulod),anguloi, angulod);
	}
	else if ( anguloi - angulod > 270 )
	{
		//n_printf( "Angulo de velocidad de entidad %f mayor que direccion contrario + 270º %f\n", anguloi, angulod );
		aux.set_rotate_y( n_deg2rad( -5.0f ) );
		//n_printf("Menos fuerte derecha %f %f %f\n",(anguloi-angulod),anguloi, angulod);
	}
	else  // 180 > anguloi - angulod < 270
	{
		//n_printf( "Angulo de velocidad de entidad %f y direccion contrario %f\n", anguloi, angulod );
		aux.set_rotate_y( n_deg2rad( -2.0f ) );
		//n_printf("Mas fuerte derecha %f %f %f\n",(anguloi-angulod),anguloi, angulod);
	}

	velocidadEntidad = aux.rotate( velocidadEntidad );
	velocityVector.norm();
	velocityVector   = velocidadEntidad * velocidadAndar;
	velocityVector.y = 0.0f;
	//vector3 vector_velocidad=velocityVector;
	//vector_velocidad.norm();
	//vector3 siguienteVector=positionVector + normal_contacto*2;
    // :HACK: Just to ensure penetration goes away
    //positionVector += *contact_normal * -0.1f;
	//n_printf("Colisionaa!!! %f %f\n",contact_normal->x,contact_normal->z);
	/**/
	//forceVector=*contact_normal*-1.0f;
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
    /*if (0.0f != cmdTurn)
    {*/
        /* For command heading
        float delta = n_min(dt * turnRate, fabsf(heading_diff));

        // Other direction
        if (0.0f > heading_diff)
            delta = -delta;

        // Reverse direction if difference is more than PI
        if (PI < fabsf(heading_diff))
            delta = -delta;
        */
        /*float delta = dt * cmdTurn;

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

        curRoll += delta;*/
    //}
	if (this->GetVisible())
		//this->positionVector.y=refWorld->GetHeight(vector3(positionVector.x-1.0f+ANCHO_MAPA/2,0.0f,positionVector.z-1.0f+ALTO_MAPA/2));
		this->positionVector.y=refWorld->GetHeight(positionVector);
	else
	{
		/*if (this->positionVector.y>-1)
			this->positionVector.y=-20;*/
	}
}

/**
    @brief Compute forces acting upon entity.

    In proper physics sims, this would include constraints but since this is
    me doing a game, they are random numbers I found on the desk.
*/
void nEntity::ComputeForces()
{
    // Apply gravity
    // forceVector += world->GetGravity() / inverseMass;

    // Apply other nice things like constraints

    // Apply acceleration
    /*float speed_delta = cmdSpeed - curSpeed;
	if (cmdSpeed==0.0f) {
		speed_delta=0.0f;
	}
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
        curAccel = 0.0f;*/

    // Be nice to calculate terrain's normal force here

    // Assuming ground contact, apply contact friction
    // Friction calculations below should be split into longitudal and
    // lateral calculations with respect to orientation.
    /*float speed = velocityVector % curHeadingVector;
    forceVector += curHeadingVector * (-1.0f * entityMass * speed);
    vector3 lateral_velocity = velocityVector - (curHeadingVector * speed);
    forceVector += lateral_velocity * (-1.5f * entityMass);
	*/
	/*switch (this->estado) {
		case QUIETO:
			forceVector.set(0.0f,0.0f,0.0f);
			break;
		case BUSQUEDA:
			forceVector=destinoVector-positionVector;
			break;
		case EVITAR_OBSTACULO:
			forceVector=positionVector-destinoVector;
			break;
	}
	*/
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
}

/**
    Apply additional constraints or effects after pseudo-physics calculations.
*/
void nEntity::Update()
{
    // Update transform matrix, which is applied to both the visual and
    // collision objects

    transformMatrix.ident();
	transformMatrix.scale(scaleVector);
	transformMatrix.rotate_x(rotateVector.x);
	transformMatrix.rotate_y(rotateVector.y);
	transformMatrix.rotate_z(rotateVector.z);
    transformMatrix.translate(positionVector);
    // First argument is timestamp, which is not used here (but should be)
	if (collideObject)
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
	//char modelo_path[N_MAXPATH];
    n_assert(visnode_path);
	visNode = visnode_path;
	refVisNode = visNode.Get();
	/*sprintf(modelo_path,"%s/mdl",this->GetVisNode());
	o_modelo=(nMLoader *) kernelServer->Lookup(modelo_path);*/
}

void nEntity::Tick(float dt) {
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

bool nEntity::EstaEnRango(vector3 posicion)
{
    vector3 distancia;
	distancia=posicion-this->GetPosition();
	return distancia.len()<DISTANCIA_DE_RANGO;
}


bool nEntity::EstaEnRangoDeAccion(vector3 posicion)
{
    vector3 distancia;
	distancia=posicion-this->GetPosition();
	return distancia.len()<DISTANCIA_DE_ACCIONES;
}

float nEntity::CalcularAngulo(vector3 dir) {
	float angulo=0.0f;
	dir.norm();
	if (dir.x<0) {
		if (dir.z<0) {
			angulo=-asin(dir.x)+PI/2;
		}
		else {
			angulo=asin(dir.x)-PI/2;
		}
	}
	else {
		if (dir.z<0) {
			angulo=-asin(dir.x)+PI/2;
		}
		else {
			angulo=asin(dir.x)-PI/2;
		}
	}
	return angulo;
}