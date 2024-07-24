#ifndef N_CAMERA_H
#define N_CAMERA_H
//------------------------------------------------------------------------------
/**
    @class nCamera

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

#undef N_DEFINES
#define N_DEFINES nCamera
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class n3DNode;
class nEntity;
class nWorld;

class CAbstractCamera;

/**
 * Clase que controla la cámara actual. Es la cámara visible desde TCL.
 * Internamente utiliza otras clases.
 * <p>
 * Externamente, la cámara tiene varios modos de funcionamiento. El primero
 * de ellos es siguiendo a una entidad. Se coloca en una posición correcta,
 * siguiendo a la entidad cuando se mueve, y demás. No se coloca en una
 * posición relativa a la entidad fija (por ejemplo, siempre desde detrás),
 * si no que va desplazándose de forma circular a ella según convenga.
 * Además, permite modificar precisamente esa posición.
 * <p>
 * El segundo modo de funcionamiento es una posición fija, tanto en la
 * posición de la cámara como en el punto al que mira. En ese caso no
 * hay posible interacción.
 * <p>
 * Por último, el tercer modo de funcionamiento es una transición de una
 * cámara a otra.
 * <p>
 * Según lo anterior, esta clase en realidad aglutina varios tipos de
 * cámaras. En lugar de implementarla con condiconales, se ha desarrollado
 * una clase abstracta (que es "interna") con un interfaz para una cámara
 * genérica, y luego tres subclases. La clase nCamera hace las veces, por
 * lo tanto, de "gestor de cámara", utilizando una de las subclases u otra
 * según el estado actual. Las subclases de cámara son por lo tanto el
 * estado actual de la cámara, y modelizan la estrategia de selección de la
 * nueva posición ante la siguiente actualización de la posición.
 * <p>
 * nCamera permite modificar la cámara actual con llamadas relativamente
 * sencillas, encargándose internamente de modificar la subclase de cámara
 * que se va a utilizar. nCamera almacena toda la información necesaria
 * para las "subcámaras" existentes. Esa información debe ser establecida
 * externamente antes de hacer uso de un "modo de cámara" que la necesite
 * (básicamente la entidad hacia la que mira una cámara de seguimiento
 * antes de establecer esa cámara como activa, o la posición y destinos
 * fijos antes de poner la cámara fija).
 */
class nCamera : public nRoot
{
public:
    enum Style
    {
        STATIONARY,
        LOCKED_CHASE,
        CHASE,
		TOTAL_STATIONARY,	// Tanto origen como destino
		TRANSITION			// Hacia una posición origen y destino
							// Se usa internamente entre las transiciones
							// desde TOTAL_STATIONARY a cualquier otra.
    };

public:
    /// constructor
    nCamera();
    /// destructor
    virtual ~nCamera();
    /// persistency
    virtual bool SaveCmds(nPersistServer* ps);

    /// Get camera transform
    const matrix44& GetTransform() const;
    /// Update camera position
    void Update(double dt);

    /// Set the world
    void SetWorld(const char* world_path);
    void SetNode(const char* node_path);

    /// Set tracked entity
    void SetTarget(const char* target_path);
    /// Get tracked entity
    nEntity* GetTarget();

    /// Set camera position
    void SetPosition(float x, float y, float z);
    
    /// Set camera style angle
    void SetStyle(nCamera::Style style);
    // Set camera target position
    void SetTargetPosition(float x, float y, float z);

	void setGiroDerecha(float g) { giroDerecha = g; }
	void setDistancia(float d) { 
		distanciaCamara += d;
		if (distanciaCamara<alturaCamara+2.0f)
			distanciaCamara=alturaCamara+2.0f;
		if (distanciaCamara>8.0)
			distanciaCamara=8.0;
	}
	void setAltura(float a) { 
		alturaCamara += a;
		if (alturaCamara<1.0)
			alturaCamara=1.0;
		if (alturaCamara>3.0)
			alturaCamara=3.0;
	}


    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:

	/**
	 * Cámara que se utiliza actualmente para calcular la posición y
	 * punto destino hacia el que se mira.
	 */
	CAbstractCamera* camaraActual;

	/**
	 * Matriz de transformación actual de la cámara.
	 */
    matrix44 transformMatrix;

	/**
	 * Posición actual de la cámara.
	 */
	vector3 posActual;

	/**
	 * Posición hacia la que mira la cámara actualmente.
	 */
	vector3 destinoActual;

    nAutoRef<nWorld> world;
    nAutoRef<n3DNode> cameraNode;
    nAutoRef<nEntity> targetEntity;

    nCamera::Style cameraStyle;
    vector3 targetPosition;

	// Velocidad de giro solicitada por el usuario.
	float giroDerecha;
	// distancia de la cámara al objetivo
	float distanciaCamara;
	// altura de la cámara
	float alturaCamara;

	vector3 posicionEstatica;
	vector3 destinoEstatico;

};

//------------------------------------------------------------------------------

/**
 * Clase abstracta que define el interfaz que deben implementar las cámaras
 * que puede utilizar nCamera.
 * @see nCamera
 */
class CAbstractCamera {
public:

	/**
	 * Calcula la nueva posición de la cámara a partir del origen actual,
	 * el destino actual y el desplazamiento deseado (el solicitado por el
	 * usuario con las teclas de control de cámara). <br>
	 * El objeto cámara puede guardar estado, que deberá haber inicializado
	 * en el constructor, para calcular la nueva posición (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posición de la cámara actualmente.
	 * @param destinoActual Posición hacia la que apunta la cámara actualmente.
	 * @param desplazamiento Desplazamiento de la cámara solicitado por el
	 * usuario con los dispositivos de entrada correspondientes de control de
	 * la cámara.
	 * @return Nueva posición de la cámara de acuerdo a la política de la
	 * "subcámara".
	 */
	virtual vector3 getNuevaPos(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento,
							float &distanciaCamara,
							float &alturaCamara) = 0;

	/**
	 * Calcula la nueva posición hacia la que apunta la cámara a partir del
	 * origen actual, el destino actual y el desplazamiento deseado (el
	 * solicitado por el usuario con las teclas de control de cámara). <br>
	 * El objeto cámara puede guardar estado, que deberá haber inicializado
	 * en el constructor, para calcular la nueva posición (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posición de la cámara actualmente.
	 * @param destinoActual Posición hacia la que apunta la cámara actualmente.
	 * @param desplazamiento Desplazamiento del destino de la cámara solicitado
	 * por el usuario con los dispositivos de entrada correspondientes de control
	 * de la cámara.
	 * 
	 * @return Nueva posición hacia la que mira la cámara de acuerdo a la
	 * política de la "subcámara".
	 */
	virtual vector3 getNuevoDestino(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento) = 0;

	/**
	 * Devuelve una nueva cámara. En algunas cámaras de transición, la propia
	 * cámara puede desear solicitar un cambio de cámara para la siguiente
	 * vuelta. Si eso ocurre, este método devolverá la nueva cámara que hay
	 * que comenzar a utilziar. En caso contrario, devuelve NULL.
	 *
	 * @return Nueva cámara a utilizar, o NULL en caso contrario.
	 * @note La implementación por defecto devuelve NULL, indicando que no hay
	 * que realizar cambio de cámara en ningún caso.
	 */
	virtual CAbstractCamera* getNuevaCamara() { return NULL; }

}; // class CAbstractCamera


//------------------------------------------------------------------------------

/**
 * Tipo de cámara que sigue a una entidad, permitiendo verla desde todos los
 * ángulos según se mueve, pero no dejandola alejarse más de un límite.
 * @see nCamera
 * @see CAbstractCamera
 */
class CTrackingCamera : public CAbstractCamera {
public:

	/**
	 * Construcción. Recibe la entidad a la que se sigue. No debe destruirse
	 * mientras se use esta cámara. No se libera en el destructor.
	 */
	CTrackingCamera(nEntity* entidad);

	/**
	 * Calcula la nueva posición de la cámara a partir del origen actual,
	 * el destino actual y el desplazamiento deseado (el solicitado por el
	 * usuario con las teclas de control de cámara). <br>
	 * El objeto cámara puede guardar estado, que deberá haber inicializado
	 * en el constructor, para calcular la nueva posición (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posición de la cámara actualmente.
	 * @param destinoActual Posición hacia la que apunta la cámara actualmente.
	 * @param desplazamiento Desplazamiento de la cámara solicitado por el
	 * usuario con los dispositivos de entrada correspondientes de control de
	 * la cámara.
	 * @return Nueva posición de la cámara de acuerdo a la política de la
	 * "subcámara".
	 */
	virtual vector3 getNuevaPos(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento,
							float &distanciaCamara,
							float &alturaCamara);

	/**
	 * Calcula la nueva posición hacia la que apunta la cámara a partir del
	 * origen actual, el destino actual y el desplazamiento deseado (el
	 * solicitado por el usuario con las teclas de control de cámara). <br>
	 * El objeto cámara puede guardar estado, que deberá haber inicializado
	 * en el constructor, para calcular la nueva posición (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posición de la cámara actualmente.
	 * @param destinoActual Posición hacia la que apunta la cámara actualmente.
	 * @param desplazamiento Desplazamiento del destino de la cámara solicitado
	 * por el usuario con los dispositivos de entrada correspondientes de control
	 * de la cámara.
	 * 
	 * @return Nueva posición hacia la que mira la cámara de acuerdo a la
	 * política de la "subcámara".
	 */
	virtual vector3 getNuevoDestino(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento);
private:

	/**
	 * Entidad a la que seguimos.
	 */
	nEntity* targetEntity;

}; // class CTrackingCamera

//------------------------------------------------------------------------------

/**
 * Tipo de cámara que tiene estático tanto el origen como el punto de mira.
 * @see nCamera
 * @see CAbstractCamera
 */
class CStaticCamera : public CAbstractCamera {
public:

	/**
	 * Construcción.
	 *
	 * @param origen Posición de la cámara.
	 * @param destino Posición hacia la que mira la cámara.
	 */
	CStaticCamera(vector3 origen, vector3 destino);

	/**
	 * Calcula la nueva posición de la cámara a partir del origen actual,
	 * el destino actual y el desplazamiento deseado (el solicitado por el
	 * usuario con las teclas de control de cámara). <br>
	 * El objeto cámara puede guardar estado, que deberá haber inicializado
	 * en el constructor, para calcular la nueva posición (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posición de la cámara actualmente.
	 * @param destinoActual Posición hacia la que apunta la cámara actualmente.
	 * @param desplazamiento Desplazamiento de la cámara solicitado por el
	 * usuario con los dispositivos de entrada correspondientes de control de
	 * la cámara.
	 * @return Nueva posición de la cámara de acuerdo a la política de la
	 * "subcámara".
	 */
	virtual vector3 getNuevaPos(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento,
							float &distanciaCamara,
							float &alturaCamara);

	/**
	 * Calcula la nueva posición hacia la que apunta la cámara a partir del
	 * origen actual, el destino actual y el desplazamiento deseado (el
	 * solicitado por el usuario con las teclas de control de cámara). <br>
	 * El objeto cámara puede guardar estado, que deberá haber inicializado
	 * en el constructor, para calcular la nueva posición (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posición de la cámara actualmente.
	 * @param destinoActual Posición hacia la que apunta la cámara actualmente.
	 * @param desplazamiento Desplazamiento del destino de la cámara solicitado
	 * por el usuario con los dispositivos de entrada correspondientes de control
	 * de la cámara.
	 * 
	 * @return Nueva posición hacia la que mira la cámara de acuerdo a la
	 * política de la "subcámara".
	 */
	virtual vector3 getNuevoDestino(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento);
private:

	/**
	 * Posición estática de la cámara.
	 */
	vector3 _origen;
	
	/**
	 * Posición hacia la que mira la cámara.
	 */
	vector3 _destino;

}; // class CStaticCamera

//------------------------------------------------------------------------------

/**
 * Tipo de cámara que hace la transición entre un estado de la cámara
 * inicial, y la posición de otra.
 * <p>
 * En el constructor recibe una cámara que será la que controla la posición
 * y destino del punto de mira que se desea lograr. Además, recibe una
 * posición y destino iniciales para esa cámara. Cada vez que se le solicita
 * una nueva posición, esta cámara llama a la cámara del constructor para
 * solicitarle la posición hacia la que hacemos la transición (por si ha
 * cambiado desde la última vez), y en función de lo indicado y la posición real
 * de la cámara, se calcula el desplazamiento y se mueve.
 * <p>
 * Es decir, tenemos dos cámaras y dos posiciones: la real, y la perseguida.
 * La cámara pasada como parámetro se utiliza para calcular la posición
 * perseguida. La primera posición perseguida se pasa también en el
 * constructor. Esa posición se supone que es la posición inicial de la
 * cámara perseguida, y en cada paso de simulación se actualiza según
 * nos indique dicha cámara. <br>
 * Por otro lado, la cámara usada en realidad parte de una posición indicada
 * en los parámetros, y utilizando la posición perseguida calcula la nueva
 * posición. Cuando se llega a la posición destino indicada por la cámara
 * interna, se avisa del cambio de cámara deseado.
 * 
 * @warning Se asume que primero se llamará a getNuevaPos e inmediatamente
 * después a getNuevoDestino. La simulación de la cámara perseguida se
 * hace de golpe en getNuevaPos, por lo que el orden de llamadas es
 * importante.
 *
 * @see nCamera
 * @see CAbstractCamera
 */
class CTransitionCamera : public CAbstractCamera {
public:

	/**
	 * Construcción.
	 *
	 * @param camaraPerseguida Cámara cuya posición (calculada cada vez)
	 * deseamos alcanzar. No se libera en el destructor. Es la cámara
	 * devuelta por getNuevaCamara cuando se alcanza la posición destino
	 * deseada. Puede haber cambiado su estado interno desde que se le
	 * pasó en el constructor.
	 * @param origen Posición de la cámara perseguida al principio.
	 * @param destino Posición hacia la que mira la cámara perseguida al
	 * principio.
	 */
	CTransitionCamera(CAbstractCamera *camaraPerseguida,
						vector3 origen, vector3 destino);

	/**
	 * Calcula la nueva posición de la cámara a partir del origen actual,
	 * el destino actual y el desplazamiento deseado (el solicitado por el
	 * usuario con las teclas de control de cámara). <br>
	 * El objeto cámara puede guardar estado, que deberá haber inicializado
	 * en el constructor, para calcular la nueva posición (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posición de la cámara actualmente.
	 * @param destinoActual Posición hacia la que apunta la cámara actualmente.
	 * @param desplazamiento Desplazamiento de la cámara solicitado por el
	 * usuario con los dispositivos de entrada correspondientes de control de
	 * la cámara.
	 * @return Nueva posición de la cámara de acuerdo a la política de la
	 * "subcámara".
	 */
	virtual vector3 getNuevaPos(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento,
							float &distanciaCamara,
							float &alturaCamara);

	/**
	 * Calcula la nueva posición hacia la que apunta la cámara a partir del
	 * origen actual, el destino actual y el desplazamiento deseado (el
	 * solicitado por el usuario con las teclas de control de cámara). <br>
	 * El objeto cámara puede guardar estado, que deberá haber inicializado
	 * en el constructor, para calcular la nueva posición (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posición de la cámara actualmente.
	 * @param destinoActual Posición hacia la que apunta la cámara actualmente.
	 * @param desplazamiento Desplazamiento del destino de la cámara solicitado
	 * por el usuario con los dispositivos de entrada correspondientes de control
	 * de la cámara.
	 * 
	 * @return Nueva posición hacia la que mira la cámara de acuerdo a la
	 * política de la "subcámara".
	 */
	virtual vector3 getNuevoDestino(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento);
	/**
	 * Devuelve una nueva cámara. En algunas cámaras de transición, la propia
	 * cámara puede desear solicitar un cambio de cámara para la siguiente
	 * vuelta. Si eso ocurre, este método devolverá la nueva cámara que hay
	 * que comenzar a utilziar. En caso contrario, devuelve NULL.
	 *
	 * @return Nueva cámara a utilizar, o NULL en caso contrario.
	 * @note La implementación por defecto devuelve NULL, indicando que no hay
	 * que realizar cambio de cámara en ningún caso.
	 */
	virtual CAbstractCamera* getNuevaCamara();

private:

	/**
	 * Cámara perseguida.
	 */
	CAbstractCamera* _camaraPerseguida;

	/**
	 * Posición actual de la cámara perseguida.
	 */
	vector3 _origen;
	
	/**
	 * Posición actual hacia la que mira la cámara perseguida.
	 */
	vector3 _destino;

	/**
	 * Booleano que indica si en la última actualización alcanzamos a la
	 * cámara perseguida.
	 */
	bool _camaraAlcanzada;

	/**
	 * Nueva posición destino de la cámara. Se calcula en getNuevaPos, pero
	 * se devuelve en getNuevoDestino.
	 */
	vector3 _nuevoDestino;

}; // class CTransitionCamera


#endif
