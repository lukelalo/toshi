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

//#ifndef N_MAPA_H
//#include "toshi/nmapa.h"
//#endif

#ifndef N_SOUNDNODE_H
#include "audio/nsoundnode.h"
#endif

#ifndef N_MLOADER_H
#include "toshi/nMLoader.h"
#endif

#include "kernel/nscriptserver.h"
#include "gfx/nchannelserver.h"
#include "toshi/nmapa.h"

#undef N_DEFINES
#define N_DEFINES nEntity
#include "kernel/ndefdllclass.h"

#define DISTANCIA_DE_ACCIONES 1.5
#define DISTANCIA_DE_RANGO 1.4

//------------------------------------------------------------------------------
class n3DNode;
class nVisNode;
class nSoundNode;
class nChannelServer;
class nCollideObject;
class nCollideShape;
class nWorld;
class nMapa;
class nMLoader;
//class nPersonaje;

class nEntity : public nRoot
{
public:
    /// constructor
    nEntity();
    /// destructor
    virtual ~nEntity();
    virtual void Initialize();
    /// persistency
    virtual bool SaveCmds(nPersistServer* ps);

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
	const vector3& GetVelocity() const;
    void Rxyz(float x, float y, float z);
    void Txyz(float x, float y, float z);
    void Sxyz(float x, float y, float z);

    // Constants
    float GetHeight() const;

    /// Called during attach loop. Devuelve NULL si la entidad
	// no es visible.
    n3DNode* Attach(void);

	inline void SetVisible(bool set);
	inline bool GetVisible() const;

	// POSICIONAMIENTO
	virtual bool EstaEnRango(vector3 posicion);
	virtual bool EstaEnRangoDeAccion(vector3 posicion);

    // Called in a loop
	virtual void Tick(float dt);

    bool Collide();

	// Establece el metodo de TCL a llamar cuando hay colisión.
	void SetTouchMethod(const char *m);

	// Devuelve el método de TCL que se llamará cuando hay colisión
	const char *GetTouchMethod() {return touchMethod;}

	vector3 GetContactNormal() {return contactNormal;}

	vector3& GetVelocityVector() {return velocityVector;}

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

	float CalcularAngulo(vector3 direccion);

	/*
	Procesa la colisión de la entidad con otra.
	*/
	void processCollision( nEntity *other, const vector3 *contact_normal);

    nRef<nWorld> refWorld;
    nAutoRef<nChannelServer> refChannelServer;
	nAutoRef<nScriptServer> refScriptServer;
    nAutoRef<n3DNode> refVisNode;

	nString visNode;
	bool visible;
	bool ejecutarScript;
	nString nombreEntidad;
	nMapa* mapa;
	//nPersonaje* personaje;

    // Constants
    float velocidadAndar;
    float velocidadCorrer;

	// Script a ejecutar cuando se detecta colisión
	char touchMethod[N_MAXPATH];

    vector3 velocityVector;

    /// Position of entity
    vector3 positionVector;
	vector3 destinoVector;
	vector3 siguienteVector;
	vector3 scaleVector;
	vector3 rotateVector;

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
inline const vector3& nEntity::GetPosition() const
{
    return positionVector;
}

inline const vector3& nEntity::GetVelocity() const
{
    return velocityVector;
}

//------------------------------------------------------------------------------
/**
*/
inline float nEntity::GetHeight() const
{
    return this->positionVector.y;
}

inline void nEntity::SetVisible(bool set) 
{
	visible = set;
}

inline bool nEntity::GetVisible() const 
{
	return visible;
}

inline float nEntity::GetEntityHeight() {
	return this->positionVector.y;
}

inline void nEntity::SetEntityHeight(float newEntityHeight) 
{
	this->positionVector.y=newEntityHeight;
}

//------------------------------------------------------------------------------
#endif
