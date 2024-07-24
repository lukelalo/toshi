#ifndef N_ENTITY_H
#define N_ENTITY_H
//------------------------------------------------------------------------------
/**
    @class nEntity

    @brief a brief description of the class

    a detailed description of the class

    (C)	2002	ling
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#include "kernel/nscriptserver.h"

#undef N_DEFINES
#define N_DEFINES nEntity
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class n3DNode;
class nChannelServer;
class nCollideObject;
class nCollideShape;
class nWorld;

class nEntity : public nRoot
{
public:
    enum Command
    {
        CMD_FORWARD,
        CMD_BACKWARD,
        CMD_LEFT,
        CMD_RIGHT,
		CMD_JUMP
    };

public:
    /// constructor
    nEntity();
    /// destructor
    virtual ~nEntity();
    virtual void Initialize();
    /// persistency
    virtual bool SaveCmds(nPersistServer* ps);

    /// Command entity to do something
    void SetCommand(Command cmd, bool set);

    // For mucking around with
    void ApplyForce(const vector3& force);

    // Prototype definitions methods
    nWorld* GetWorld();
    // VisNode
    const char* GetVisNode();
    void SetVisNode(const char* visnode_path);

    // Collide
	
    void SetCollideClass(const char* coll_class);
    const char* GetCollideClass() const;
    void SetCollideShape(const char* coll_shape);
    const char* GetCollideShape() const;
 
    /// Set position of entity
    void SetPosition(float x, float y);
    const vector3& GetPosition() const;

    float GetCurHeading() const;
    const vector3& GetCurHeadingVector() const;

    /// Forward speed, not velocity, different things boyo
    float GetCurSpeed() const;
    float GetCurRoll() const;

    // Constants
    float GetHeight() const;

    /// Called during attach loop. Devuelve NULL si la entidad
	// no es visible.
    n3DNode* Attach(void);

	inline void SetVisible(bool set);
	inline bool GetVisible() const;

    // Called in a loop
	void Tick(float dt);

    void Collide();

	// Establece el metodo de TCL a llamar cuando hay colisión.
	void SetTouchMethod(const char *m);

	// Devuelve el método de TCL que se llamará cuando hay colisión
	const char *GetTouchMethod() {return touchMethod;}

	vector3 GetContactNormal() {return contactNormal;}

	const char *GetTouchedEntity() {return touchedEntity;}

	/**
	Función llamada desde el script, cuando se quiere procesar una colisión
	con la forma por defecto. Coge los datos de la última colisión detectada.
	*/
	
	void processCollision();



	inline float GetEntityHeight();
	inline void SetEntityHeight(float newEntityHeight);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

protected:
    // Called in a loop
    void Trigger(float dt);
    /// Compute forces acting on this entity
    void ComputeForces();
    /// Integrate aceleration, velocity and position
    void Integrate(float dt);
    /// Post arrange entity depending on new physical parameters
    void Update();

	/*
	Procesa la colisión de la entidad con otra.
	*/
	void processCollision(const nEntity *other, const vector3 *contact_normal);

private:

    nRef<nWorld> refWorld;
    nAutoRef<nChannelServer> refChannelServer;
	nAutoRef<nScriptServer> refScriptServer;
    nAutoRef<n3DNode> refVisNode;

	nString visNode;
	bool visible;

    // Properties
    float entityMass;
    float inverseMass;
    float entityHeight;

    // Constants
    float maxAcceleration;
    float maxDeceleration;
    float maxReverseAcceleration;
    float maxForwardSpeed;
    float maxReverseSpeed;
    float turnRate;
	float jumpRate;
	float potJump;
    float rollRate;

    // Dynamic
    float curSpeed;
    float curHeading;
    vector3 curHeadingVector;
    float curRoll;

    float curAccel;
    float curTravel;

    // Commands
    float cmdSpeed;
    float cmdTurn;
    float cmdRoll;
	float cmdJump;

	// Script a ejecutar cuando se detecta colisión
	char touchMethod[N_MAXPATH];

    vector3 forceVector;
    vector3 velocityVector;

    /// Position of entity
    vector3 positionVector;

    matrix44 transformMatrix;

    nString collideClass;
    nString collideShape;
    /// Reference to collide object
    nCollideObject* collideObject;

	// Cuando se están procesando las colisiones, se almacena en
	// estas variables información sobre la colisión, para poder ser
	// consultadas desde TCL.
	char touchedEntity[N_MAXPATH];
	vector3 contactNormal;

};

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nEntity::GetPosition() const
{
    return positionVector;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nEntity::GetCurHeading() const
{
    return curHeading;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nEntity::GetCurHeadingVector() const
{
    return curHeadingVector;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nEntity::GetCurSpeed() const
{
    return curSpeed;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nEntity::GetCurRoll() const
{
    return curRoll;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nEntity::GetHeight() const
{
    return entityHeight;
}


inline void nEntity::SetVisible(bool set) {
	visible = set;
}

inline bool nEntity::GetVisible() const {
	return visible;
}

inline float nEntity::GetEntityHeight() {
	return entityHeight;
}

inline void nEntity::SetEntityHeight(float newEntityHeight) {
	entityHeight = newEntityHeight;
}

//------------------------------------------------------------------------------
#endif
