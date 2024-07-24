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
 * Clase que controla la c�mara actual. Es la c�mara visible desde TCL.
 * Internamente utiliza otras clases.
 * <p>
 * Externamente, la c�mara tiene varios modos de funcionamiento. El primero
 * de ellos es siguiendo a una entidad. Se coloca en una posici�n correcta,
 * siguiendo a la entidad cuando se mueve, y dem�s. No se coloca en una
 * posici�n relativa a la entidad fija (por ejemplo, siempre desde detr�s),
 * si no que va desplaz�ndose de forma circular a ella seg�n convenga.
 * Adem�s, permite modificar precisamente esa posici�n.
 * <p>
 * El segundo modo de funcionamiento es una posici�n fija, tanto en la
 * posici�n de la c�mara como en el punto al que mira. En ese caso no
 * hay posible interacci�n.
 * <p>
 * Por �ltimo, el tercer modo de funcionamiento es una transici�n de una
 * c�mara a otra.
 * <p>
 * Seg�n lo anterior, esta clase en realidad aglutina varios tipos de
 * c�maras. En lugar de implementarla con condiconales, se ha desarrollado
 * una clase abstracta (que es "interna") con un interfaz para una c�mara
 * gen�rica, y luego tres subclases. La clase nCamera hace las veces, por
 * lo tanto, de "gestor de c�mara", utilizando una de las subclases u otra
 * seg�n el estado actual. Las subclases de c�mara son por lo tanto el
 * estado actual de la c�mara, y modelizan la estrategia de selecci�n de la
 * nueva posici�n ante la siguiente actualizaci�n de la posici�n.
 * <p>
 * nCamera permite modificar la c�mara actual con llamadas relativamente
 * sencillas, encarg�ndose internamente de modificar la subclase de c�mara
 * que se va a utilizar. nCamera almacena toda la informaci�n necesaria
 * para las "subc�maras" existentes. Esa informaci�n debe ser establecida
 * externamente antes de hacer uso de un "modo de c�mara" que la necesite
 * (b�sicamente la entidad hacia la que mira una c�mara de seguimiento
 * antes de establecer esa c�mara como activa, o la posici�n y destinos
 * fijos antes de poner la c�mara fija).
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
		TRANSITION			// Hacia una posici�n origen y destino
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
	 * C�mara que se utiliza actualmente para calcular la posici�n y
	 * punto destino hacia el que se mira.
	 */
	CAbstractCamera* camaraActual;

	/**
	 * Matriz de transformaci�n actual de la c�mara.
	 */
    matrix44 transformMatrix;

	/**
	 * Posici�n actual de la c�mara.
	 */
	vector3 posActual;

	/**
	 * Posici�n hacia la que mira la c�mara actualmente.
	 */
	vector3 destinoActual;

    nAutoRef<nWorld> world;
    nAutoRef<n3DNode> cameraNode;
    nAutoRef<nEntity> targetEntity;

    nCamera::Style cameraStyle;
    vector3 targetPosition;

	// Velocidad de giro solicitada por el usuario.
	float giroDerecha;
	// distancia de la c�mara al objetivo
	float distanciaCamara;
	// altura de la c�mara
	float alturaCamara;

	vector3 posicionEstatica;
	vector3 destinoEstatico;

};

//------------------------------------------------------------------------------

/**
 * Clase abstracta que define el interfaz que deben implementar las c�maras
 * que puede utilizar nCamera.
 * @see nCamera
 */
class CAbstractCamera {
public:

	/**
	 * Calcula la nueva posici�n de la c�mara a partir del origen actual,
	 * el destino actual y el desplazamiento deseado (el solicitado por el
	 * usuario con las teclas de control de c�mara). <br>
	 * El objeto c�mara puede guardar estado, que deber� haber inicializado
	 * en el constructor, para calcular la nueva posici�n (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posici�n de la c�mara actualmente.
	 * @param destinoActual Posici�n hacia la que apunta la c�mara actualmente.
	 * @param desplazamiento Desplazamiento de la c�mara solicitado por el
	 * usuario con los dispositivos de entrada correspondientes de control de
	 * la c�mara.
	 * @return Nueva posici�n de la c�mara de acuerdo a la pol�tica de la
	 * "subc�mara".
	 */
	virtual vector3 getNuevaPos(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento,
							float &distanciaCamara,
							float &alturaCamara) = 0;

	/**
	 * Calcula la nueva posici�n hacia la que apunta la c�mara a partir del
	 * origen actual, el destino actual y el desplazamiento deseado (el
	 * solicitado por el usuario con las teclas de control de c�mara). <br>
	 * El objeto c�mara puede guardar estado, que deber� haber inicializado
	 * en el constructor, para calcular la nueva posici�n (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posici�n de la c�mara actualmente.
	 * @param destinoActual Posici�n hacia la que apunta la c�mara actualmente.
	 * @param desplazamiento Desplazamiento del destino de la c�mara solicitado
	 * por el usuario con los dispositivos de entrada correspondientes de control
	 * de la c�mara.
	 * 
	 * @return Nueva posici�n hacia la que mira la c�mara de acuerdo a la
	 * pol�tica de la "subc�mara".
	 */
	virtual vector3 getNuevoDestino(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento) = 0;

	/**
	 * Devuelve una nueva c�mara. En algunas c�maras de transici�n, la propia
	 * c�mara puede desear solicitar un cambio de c�mara para la siguiente
	 * vuelta. Si eso ocurre, este m�todo devolver� la nueva c�mara que hay
	 * que comenzar a utilziar. En caso contrario, devuelve NULL.
	 *
	 * @return Nueva c�mara a utilizar, o NULL en caso contrario.
	 * @note La implementaci�n por defecto devuelve NULL, indicando que no hay
	 * que realizar cambio de c�mara en ning�n caso.
	 */
	virtual CAbstractCamera* getNuevaCamara() { return NULL; }

}; // class CAbstractCamera


//------------------------------------------------------------------------------

/**
 * Tipo de c�mara que sigue a una entidad, permitiendo verla desde todos los
 * �ngulos seg�n se mueve, pero no dejandola alejarse m�s de un l�mite.
 * @see nCamera
 * @see CAbstractCamera
 */
class CTrackingCamera : public CAbstractCamera {
public:

	/**
	 * Construcci�n. Recibe la entidad a la que se sigue. No debe destruirse
	 * mientras se use esta c�mara. No se libera en el destructor.
	 */
	CTrackingCamera(nEntity* entidad);

	/**
	 * Calcula la nueva posici�n de la c�mara a partir del origen actual,
	 * el destino actual y el desplazamiento deseado (el solicitado por el
	 * usuario con las teclas de control de c�mara). <br>
	 * El objeto c�mara puede guardar estado, que deber� haber inicializado
	 * en el constructor, para calcular la nueva posici�n (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posici�n de la c�mara actualmente.
	 * @param destinoActual Posici�n hacia la que apunta la c�mara actualmente.
	 * @param desplazamiento Desplazamiento de la c�mara solicitado por el
	 * usuario con los dispositivos de entrada correspondientes de control de
	 * la c�mara.
	 * @return Nueva posici�n de la c�mara de acuerdo a la pol�tica de la
	 * "subc�mara".
	 */
	virtual vector3 getNuevaPos(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento,
							float &distanciaCamara,
							float &alturaCamara);

	/**
	 * Calcula la nueva posici�n hacia la que apunta la c�mara a partir del
	 * origen actual, el destino actual y el desplazamiento deseado (el
	 * solicitado por el usuario con las teclas de control de c�mara). <br>
	 * El objeto c�mara puede guardar estado, que deber� haber inicializado
	 * en el constructor, para calcular la nueva posici�n (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posici�n de la c�mara actualmente.
	 * @param destinoActual Posici�n hacia la que apunta la c�mara actualmente.
	 * @param desplazamiento Desplazamiento del destino de la c�mara solicitado
	 * por el usuario con los dispositivos de entrada correspondientes de control
	 * de la c�mara.
	 * 
	 * @return Nueva posici�n hacia la que mira la c�mara de acuerdo a la
	 * pol�tica de la "subc�mara".
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
 * Tipo de c�mara que tiene est�tico tanto el origen como el punto de mira.
 * @see nCamera
 * @see CAbstractCamera
 */
class CStaticCamera : public CAbstractCamera {
public:

	/**
	 * Construcci�n.
	 *
	 * @param origen Posici�n de la c�mara.
	 * @param destino Posici�n hacia la que mira la c�mara.
	 */
	CStaticCamera(vector3 origen, vector3 destino);

	/**
	 * Calcula la nueva posici�n de la c�mara a partir del origen actual,
	 * el destino actual y el desplazamiento deseado (el solicitado por el
	 * usuario con las teclas de control de c�mara). <br>
	 * El objeto c�mara puede guardar estado, que deber� haber inicializado
	 * en el constructor, para calcular la nueva posici�n (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posici�n de la c�mara actualmente.
	 * @param destinoActual Posici�n hacia la que apunta la c�mara actualmente.
	 * @param desplazamiento Desplazamiento de la c�mara solicitado por el
	 * usuario con los dispositivos de entrada correspondientes de control de
	 * la c�mara.
	 * @return Nueva posici�n de la c�mara de acuerdo a la pol�tica de la
	 * "subc�mara".
	 */
	virtual vector3 getNuevaPos(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento,
							float &distanciaCamara,
							float &alturaCamara);

	/**
	 * Calcula la nueva posici�n hacia la que apunta la c�mara a partir del
	 * origen actual, el destino actual y el desplazamiento deseado (el
	 * solicitado por el usuario con las teclas de control de c�mara). <br>
	 * El objeto c�mara puede guardar estado, que deber� haber inicializado
	 * en el constructor, para calcular la nueva posici�n (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posici�n de la c�mara actualmente.
	 * @param destinoActual Posici�n hacia la que apunta la c�mara actualmente.
	 * @param desplazamiento Desplazamiento del destino de la c�mara solicitado
	 * por el usuario con los dispositivos de entrada correspondientes de control
	 * de la c�mara.
	 * 
	 * @return Nueva posici�n hacia la que mira la c�mara de acuerdo a la
	 * pol�tica de la "subc�mara".
	 */
	virtual vector3 getNuevoDestino(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento);
private:

	/**
	 * Posici�n est�tica de la c�mara.
	 */
	vector3 _origen;
	
	/**
	 * Posici�n hacia la que mira la c�mara.
	 */
	vector3 _destino;

}; // class CStaticCamera

//------------------------------------------------------------------------------

/**
 * Tipo de c�mara que hace la transici�n entre un estado de la c�mara
 * inicial, y la posici�n de otra.
 * <p>
 * En el constructor recibe una c�mara que ser� la que controla la posici�n
 * y destino del punto de mira que se desea lograr. Adem�s, recibe una
 * posici�n y destino iniciales para esa c�mara. Cada vez que se le solicita
 * una nueva posici�n, esta c�mara llama a la c�mara del constructor para
 * solicitarle la posici�n hacia la que hacemos la transici�n (por si ha
 * cambiado desde la �ltima vez), y en funci�n de lo indicado y la posici�n real
 * de la c�mara, se calcula el desplazamiento y se mueve.
 * <p>
 * Es decir, tenemos dos c�maras y dos posiciones: la real, y la perseguida.
 * La c�mara pasada como par�metro se utiliza para calcular la posici�n
 * perseguida. La primera posici�n perseguida se pasa tambi�n en el
 * constructor. Esa posici�n se supone que es la posici�n inicial de la
 * c�mara perseguida, y en cada paso de simulaci�n se actualiza seg�n
 * nos indique dicha c�mara. <br>
 * Por otro lado, la c�mara usada en realidad parte de una posici�n indicada
 * en los par�metros, y utilizando la posici�n perseguida calcula la nueva
 * posici�n. Cuando se llega a la posici�n destino indicada por la c�mara
 * interna, se avisa del cambio de c�mara deseado.
 * 
 * @warning Se asume que primero se llamar� a getNuevaPos e inmediatamente
 * despu�s a getNuevoDestino. La simulaci�n de la c�mara perseguida se
 * hace de golpe en getNuevaPos, por lo que el orden de llamadas es
 * importante.
 *
 * @see nCamera
 * @see CAbstractCamera
 */
class CTransitionCamera : public CAbstractCamera {
public:

	/**
	 * Construcci�n.
	 *
	 * @param camaraPerseguida C�mara cuya posici�n (calculada cada vez)
	 * deseamos alcanzar. No se libera en el destructor. Es la c�mara
	 * devuelta por getNuevaCamara cuando se alcanza la posici�n destino
	 * deseada. Puede haber cambiado su estado interno desde que se le
	 * pas� en el constructor.
	 * @param origen Posici�n de la c�mara perseguida al principio.
	 * @param destino Posici�n hacia la que mira la c�mara perseguida al
	 * principio.
	 */
	CTransitionCamera(CAbstractCamera *camaraPerseguida,
						vector3 origen, vector3 destino);

	/**
	 * Calcula la nueva posici�n de la c�mara a partir del origen actual,
	 * el destino actual y el desplazamiento deseado (el solicitado por el
	 * usuario con las teclas de control de c�mara). <br>
	 * El objeto c�mara puede guardar estado, que deber� haber inicializado
	 * en el constructor, para calcular la nueva posici�n (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posici�n de la c�mara actualmente.
	 * @param destinoActual Posici�n hacia la que apunta la c�mara actualmente.
	 * @param desplazamiento Desplazamiento de la c�mara solicitado por el
	 * usuario con los dispositivos de entrada correspondientes de control de
	 * la c�mara.
	 * @return Nueva posici�n de la c�mara de acuerdo a la pol�tica de la
	 * "subc�mara".
	 */
	virtual vector3 getNuevaPos(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento,
							float &distanciaCamara,
							float &alturaCamara);

	/**
	 * Calcula la nueva posici�n hacia la que apunta la c�mara a partir del
	 * origen actual, el destino actual y el desplazamiento deseado (el
	 * solicitado por el usuario con las teclas de control de c�mara). <br>
	 * El objeto c�mara puede guardar estado, que deber� haber inicializado
	 * en el constructor, para calcular la nueva posici�n (por ejemplo
	 * la velocidad actual de desplazamiento, o la entidad hacia la que se
	 * apunta siempre).
	 *
	 * @param origenActual Posici�n de la c�mara actualmente.
	 * @param destinoActual Posici�n hacia la que apunta la c�mara actualmente.
	 * @param desplazamiento Desplazamiento del destino de la c�mara solicitado
	 * por el usuario con los dispositivos de entrada correspondientes de control
	 * de la c�mara.
	 * 
	 * @return Nueva posici�n hacia la que mira la c�mara de acuerdo a la
	 * pol�tica de la "subc�mara".
	 */
	virtual vector3 getNuevoDestino(const vector3 &origenActual,
							const vector3 &destinoActual, 
							const vector3 &desplazamiento);
	/**
	 * Devuelve una nueva c�mara. En algunas c�maras de transici�n, la propia
	 * c�mara puede desear solicitar un cambio de c�mara para la siguiente
	 * vuelta. Si eso ocurre, este m�todo devolver� la nueva c�mara que hay
	 * que comenzar a utilziar. En caso contrario, devuelve NULL.
	 *
	 * @return Nueva c�mara a utilizar, o NULL en caso contrario.
	 * @note La implementaci�n por defecto devuelve NULL, indicando que no hay
	 * que realizar cambio de c�mara en ning�n caso.
	 */
	virtual CAbstractCamera* getNuevaCamara();

private:

	/**
	 * C�mara perseguida.
	 */
	CAbstractCamera* _camaraPerseguida;

	/**
	 * Posici�n actual de la c�mara perseguida.
	 */
	vector3 _origen;
	
	/**
	 * Posici�n actual hacia la que mira la c�mara perseguida.
	 */
	vector3 _destino;

	/**
	 * Booleano que indica si en la �ltima actualizaci�n alcanzamos a la
	 * c�mara perseguida.
	 */
	bool _camaraAlcanzada;

	/**
	 * Nueva posici�n destino de la c�mara. Se calcula en getNuevaPos, pero
	 * se devuelve en getNuevoDestino.
	 */
	vector3 _nuevoDestino;

}; // class CTransitionCamera


#endif
