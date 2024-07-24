#ifndef N_JUGADOR_H
#define N_JUGADOR_H
//------------------------------------------------------------------------------
/**
    @class classname

    @brief a brief description of the class

    a detailed description of the class

    (C) 2001 RadonLabs GmbH
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

#ifndef N_PLANO_H
#include "toshi/nplano.h"
#endif

#ifndef N_PERSONAJE_H
#include "toshi/npersonaje.h"
#endif

#ifndef N_DISTRITO_H
#include "toshi/ndistrito.h"
#endif

#undef N_DEFINES
#define N_DEFINES nJugador
#include "kernel/ndefdllclass.h"
#define N_MAXPLANOS						8
#define N_MAXTIPODEPUNTOS               11
#define TIEMPO_PERMANENCIA_SELECCION	1.0f
#define MAX_TIEMPO_PAGO  				20.0f
#define TIEMPO_MUESTRA_INFORMACION		5.0f

#include <cstdlib>

using namespace std;

//------------------------------------------------------------------------------
class nPersonaje;
class nPlano;
class nDistrito;
class nCampesino;
class nPartida;

typedef enum _Acciones
{
	NINGUNA,
	IR_A,
	IR_A_ENTIDAD,
	IR_A_AYUNTAMIENTO,
	COGER_DINERO,
	RECAUDAR,
	COGER_PLANO,
	INTERCAMBIAR_PLANOS,
	SEGUIR_A,
	SEGUIR_A_JUGADOR,
    DESTRUIR_EDIFICIO,
	CONSTRUIR_EDIFICIO,
	CONTRATAR,
	PRIMERA,
	SEGUNDA,
	ESPERAR,
	LIBERAR_PERSONAJE,
	CAMBIAR_PERSONAJE,
	METER_PERSONAJE,
	NACER,
	MORIR
} Acciones;

typedef enum _Clase_Destino
{
	C_CASAPLANOS,
	C_CASAMONEDA,
	C_AYUNTAMIENTO,
	C_CONSTRUCCION,
	C_CAMPESINO,
	C_PERSONAJE,
	C_KUNOICHI,
	C_LADRONA,
	C_SHUGENJA,
	C_SHOGUN,
	C_MAESTRO,
	C_MERCADER,
	C_INGENIERO,
	C_SAMURAI,
	C_NINGUNA
} Clase_Destino;

typedef struct _TAccionNet
{
	short					accion;
	float					posicion_x;
	float					posicion_y;
	float					posicion_z;
	Clase_Destino			clase_destino;
	int						id_destino;
	short					jugador;
	short					personaje;
	float					tiempo;
	int						numAccion;
} TAccionNet;

typedef struct _PlayerStateNet 
{
	TAccionNet					accion;
	short						plano;
	short						num_jugador;
} PlayerStateNet;

typedef enum _Puntuacion
{
	CONTRATOS,
	DESPIDOS,
	RESURRECCIONES,
	ROBOS,
	ASESINATOS,
	CONSTRUCCIONES,
	DESTRUCCIONES,
	DISTINTIVOS,
	DINERO,
	TIEMPO
} Puntuacion;

typedef struct _TAccion
{
	Acciones				accion;
	vector3					posicion;
	nEntity*				destino;
	nJugador*				jugador;
	nPersonaje::TPersonaje  nombre_personaje;
	float					tiempo;
} TAccion;

typedef struct _TAccionesPorTurno
{
	int VisitasACasaMoneda;
	int VisitasACasaPlanos;
	int VisitasAAyuntamiento;
	int Robos;
	int Asesinatos;
	int Resurrecciones;
	int Construcciones;
	int Intercambios;
	int Recaudaciones;
	int Liberaciones;
} TAccionesPorTurno;

class nJugador : public nRoot
{
	public:

		typedef enum _Puntuacion
		{
			CONTRATOS,
			DESPIDOS,
			RESURRECCIONES,
			ROBOS,
			ASESINATOS,
			CONSTRUCCIONES,
			DESTRUCCIONES,
			DISTINTIVOS,
			DINERO,
			TIEMPO,
			TOTAL
		} Puntuacion;

		typedef struct _Puntos
		{
			Puntuacion  categoria;
			int         puntos;
		} TPuntos;

		// constructor
		nJugador();
		// destructor
		virtual ~nJugador();
		// persistency
		virtual bool SaveCmds( nPersistServer* persistServer );
		// Identificador del jugador (del 0 al 7)
		void SetId( int id );
		int GetId();
		// Nombre del jugador
		void SetNombre( const char* nombre_jugador );
		const char* GetNombre() const;
		// Cambia el personaje activo
		void CambiarPersonaje( nPersonaje::TPersonaje nombre_personaje );
		// Se ejecuta al finalizar el tiempo de turno. Libera el personaje y se convierte en un campesino
		void LiberarPersonaje( bool fin_de_turno );
		// Oro del jugador
		void SetOro( int oro );
		int GetOro();
		// Tiempo de acción del jugador
		void SetTiempo( float tiempo );
		float GetTiempo();
		float GetTiempoEfectivo();
		// Número de campesinos contratados por el jugador
		void SetCampesinosContratados( int campesinos );
		int GetCampesinosContratados();
		// Número de campesinos liberados por el jugador
		void SetCampesinosLiberados( int campesinos );
		int GetCampesinosLiberados();
		// Número de campesinos resucitados por el jugador
		void SetCampesinosResucitados( int campesinos );
		int GetCampesinosResucitados();
		// Contratar un campesino
		void ContratarCampesino( nCampesino* &campesino );
		// Liberar un campesino
		void LiberarCampesino( nCampesino* &campesino );
		// Número de jugadores robados por el jugador
		void SetPersonajesRobados( int personajes );
		int GetPersonajesRobados();
		// Número de jugadores asesinados por el jugador
		void SetPersonajesAsesinados( int personajes );
		int GetPersonajesAsesinados();
		// Número de edificios derribados por el jugador
		void SetEdificiosDerribados( int edificios );
		int GetEdificiosDerribados();
		// Derribar un edificio de un jugador
		void DerribarEdificio( const char* nombre_edificio );
		// Puntos del jugador
		void SetPuntos( int tipo, int cuantos );
		nArray<TPuntos> GetPuntos();
		// Devuelve cierto si un jugador tiene espacio para coger planos
		bool PuedeCogerPlanos();
		// Coge un plano y lo muestra en pantalla
		void CogerPlano( nPlano* &plano );
		// Liberar un plano
		void LiberarPlano( int pos_plano );
		// Manejador de los eventos del ratón (para el hud)
		bool ManejadorRaton( float x, float y );
		// Devuelve el puntero que tiene que tener el ratón si está encima de un elemento del HUD
		int PunteroRaton( float x, float y , bool p_ayuda );
		// Ejecuta la acción principal sobre una entidad
		void AccionPrincipal( nEntity* entidad );
		// Ejecuta la acción secundaria sobre una entidad
		void AccionSecundaria( nEntity* entidad );
		// Ejecuta lo necesario al seleccionar una posicion
		void SeleccionarPosicion( vector3 posicion );
		// Agrega una acción a la lista de acciones pendientes
		void AgregarAccion( TAccion* accion );
		// Devuelve cierto si la acción en la cima de la pila ha concluido
		bool AccionTerminada( float dt );
		// Vacía la pila de acciones
		void VaciarPilaAcciones();
		// Borra la acción en la cima de la pila y ejecuta la siguiente acción
		void EjecutarSiguienteAccion();
		// Activa/Desactiva el modo Construccion
		void SetModoConstruccion( bool activo );
		bool GetModoConstruccion();
		// Muestra/Oculta el HUD (para el modo Construccion)
		void CambiarVisibilidadHUD( bool visibilidad );
		// Indica si un punto se encuentra dentro del distrito del jugador
		bool PosicionDentroDeDistrito( vector3 posicion );
		// Muestra la construccion que queremos realizar en una posición válida
		void MostrarConstruccionValida( vector3 posicion );
		// Muestra la construccion que queremos realizar en una posición no válida
		void MostrarConstruccionNoValida( vector3 posicion );
		// Construye el edificio en la posición dada
		void ConstruirEdificio( vector3 posicion );
		// Actualiza el estado del jugador
		void Tick( float t );
		// Comprueba si el jugador puede contratar un campesino
		bool PuedeContratarCampesino( nCampesino* &campesino );
		// Asigna un personaje al jugador
		void SetPersonaje( nPersonaje* personaje_jugador );
		// Asigna el siguiente personaje a un jugador
		void SetSiguientePersonaje( nPersonaje* personaje_jugador );
		// Devuelve un puntero al personaje
		nPersonaje* GetPersonaje();
		// Devuelve un puntero al siguiente personaje
		nPersonaje* GetSiguientePersonaje();
		// Devuelve un puntero al personaje
		nPersonaje::TPersonaje GetPersonajeAnterior();
		// Coger dinero
		void CogerDinero();
		// Recaudar dinero
		void Recaudar();
		// Intercambiar planos
		void IntercambiarPlanos();
		// Intercambiar planos con otro jugador
		void IntercambiarPlanosConJugador( nJugador* &propietario );
		// Obtiene el distrito
		nDistrito* GetDistrito();
		// Asigna la partida al jugador
		void SetPartida( nPartida* refpartida );
		// Obtiene la partida
		nPartida* GetPartida();
		// Jugador con acciones pendientes
		bool JugadorInactivo();
		// Jugador que se muestra en pantalla;
		bool GetJugadorPrincipal();
		void SetJugadorPrincipal( bool es_principal );
		// Se le paga a un campesino su renta por estar contratado
		int PagarACampesino( nCampesino* &campesino );
		// Se comprueba si el jugador tiene construido un edificio determinado
		bool LoTieneConstruido( const char* nombre_edificio );
		// pointer to nKernelServer
		static nKernelServer* kernelServer;
		// Roba al personaje que tenga mas cercano (incluyendo campesinos);
		void IA_RobarPersonajeMasCercano();
		// Va a la casa de planos a coger un plano
		void IA_CogerPlano();
		// Va a la casa de la moneda a coger dinero
		void IA_CogerDinero();
		// Cambia el personaje
		void IA_CambiarPersonaje( nPersonaje::TPersonaje personaje );
		// Libera el personaje
		bool IA_LiberarPersonaje();
		// Mata al jugador que tenga mas cercano
		void IA_MatarJugadorMasCercano();
		// Construye un plano en su distrito
		void IA_ConstruirPlano();
		// Intercambia planos con el jugador que tenga más planos (trampa jijiji)
		void IA_IntercambiarPlanos();
		// Intercambia planos con otro jugador
		void IA_IntercambiarPlanosConJugador();
		// Destruye un edificio del jugador que tenga más edificios construidos (mas trampa jijiji)
		void IA_DestruirEdificio();
		// Destruye un edificio del jugador que tenga más edificios construidos (mas trampa jijiji)
		void IA_Ir_A_Ayuntamiento();
		// Recauda impuestos del ayuntamiento
		void IA_Recaudar();
		// Roba al personaje que tenga más edificios construidos
		void IA_RobarPersonajeMejor();
		// Mata al jugador que tenga más edificios construidos
		void IA_MatarJugadorMejor();
		// Recauda impuestos en un ayuntamiento ajeno
		void IA_RecaudarEnAyuntamientoMasCercano();
		// Resucita al campesino más cercano
		void IA_Resucitar();
		// Contrata al campesino más cercano
		void IA_Contratar();
		// Espera un tiempo
		void Esperar( float tiempo );
		// Devuelve el número de planos que tiene el jugador
		int GetNumPlanos();
		// Asigna puntos según la categoría
		void Puntuar( int categoria, nJugador* propietario, int valor_destruccion );
		// Bloquea al jugador para que no pueda hacer nada en el juego
		void Bloquear();
		// Comprueba si el jugador está bloqueado
		bool EstaBloqueado();
		// Desbloquea al jugador para que pueda hacer cosas en el juego
		void Desbloquear();
		// Devuelve el número de puntos obtenidos por acciones
		int PuntosPorAccion();
		// Devuelve el número de puntos obtenidos por construcción
		int PuntosPorConstruccion();
		// Devuelve el número de puntos obtenidos por distintivos diferentes
		int PuntosPorDistintivo();
		// Devuelve el número de puntos obtenidos por dinero
		int PuntosPorDinero();
		// Devuelve el número de puntos obtenidos por tiempo empleado
		int PuntosPorTiempo();
		// Devuelve un puntero al array de planos
		nArray<nPlano*> GetPlanos();
		// Muestra una información puntual y resetea el contador de información mostrada
		void MostrarInformacion(const char *p_informacion);
		// Oculta la línea de información mostrada
		void OcultarInformacion();
		// Muestrea el tiempo y si llega a 0 oculta la línea de información
		void TratarMuestraInformacion(float dt);
		// Total de coste de planos que posee el jugador
		int TotalCostePlanos();
		// Total de valor de planos que posee el jugador
		int TotalValorPlanos();
		// Total de edificios especiales del jugador
		int TotalPlanosEspeciales();
		// Total de tipos distintos de planos del jugador
		int TotalPlanosDistintos();

		/* ACCIONES DE UN JUGADOR */

		// Agrega la acción de ir a un sitio
		void AccionIrA(vector3 posicion);
		// Agrega la acción de cambiar de personaje
		void AccionCambiarPersonaje(nPersonaje::TPersonaje personaje);
		// Agrega la acción de liberar un personaje
		void AccionLiberarPersonaje();
		// Agrega la acción de ir al ayuntamiento
		void AccionIrAAyuntamiento();
		// Agrega la acción de coger dinero
		void AccionCogerDinero(nEntity *destino);
		// Agrega la acción de coger un plano
		void AccionCogerPlano(nEntity *destino);
		// Agrega la acción de recaudar dinero en un ayuntamiento
		void AccionRecaudar(nEntity *destino);
		// Agrega la acción de contratar a un campesino
		void AccionContratar(nEntity *destino);
		// Agrega la acción de esperar un tiempo
		void AccionEsperar(float tiempo);
		// Agrega la acción de intercambiar planos
		void AccionIntercambiarPlanos(nEntity *destino);
		// Agrega la acción especial
		void AccionEspecial(nEntity *destino);
		// Agrega la acción de destruir edificio
		void AccionDestruirEdificio(nEntity *edificio);
		// Agrega la acción de construir edificio
		void AccionConstruirEdificio(nEntity *edificio);
		// Agrega la acción de morir
		void AccionMorir();

		/**/

		// RED
		PlayerStateNet getPlayerStateNet();
		void setPlayerStateNet(PlayerStateNet state);


	private:

		PlayerStateNet			 AccionActual;
		nString                  Nombre;
		nPersonaje::TPersonaje	 PersonajeActivo;
		nPersonaje::TPersonaje	 PersonajeAnterior;
		nString				 	 EdificioAConstruir;
		nAutoRef<nScriptServer>  refScriptServer;
		nAutoRef<nChannelServer> refChannelServer;
		nArray<nPlano*>			 planos;
		nArray<TPuntos>	         Puntos;
		nArray<int>              aytos_recaudados;
		nStack<TAccion*>		 pila_acciones;
		nDistrito*				 distrito;
		nPersonaje*				 personaje;
		nPersonaje*				 personajeOculto;
		nPersonaje*				 siguientePersonaje;
		nPartida*				 partida;
		float					 Tiempo;
		float					 TiempoEfectivo;
		float					 TiempoSeleccion;
		float					 TiempoAnterior;
		float					 TiempoInformacion;
		int						 timeChannel;
		int						 CanalTiempoTurno;
		int                      CanalTiempoPago;
		int						 Oro;
		int						 PersonajesRobados;
		int						 PersonajesAsesinados;
		int						 EdificiosDerribados;
		int						 CampesinosContratados;
		int						 CampesinosLiberados;
		int						 CampesinosResucitados;
		int						 plano_a_construir;
		int						 IdJugador;
		TAccionesPorTurno        AccionesPorTurno;
		bool					 ModoConstruccion;
		bool					 TienePersonaje;
		bool                     JugadorPrincipal;
		bool					 JugadorActivo;
		bool                     JugadorBloqueado;
		bool					 JugadorMuerto;
		bool					 BarraBloqueada;

		TAccion *AccionVacia();
		bool	SeleccionarPersonaje( float x, float y );
		void	TratarMarcaSeleccionPosicion( float dt );
		void	TratarTiempo( float t );
		void	TratarMarcadorTiempoTurno();
		void	TratarSonidoAmbiente();
		void	MostrarSeleccion( vector3 posicion );
		void	OcultarSeleccion();
		void	GuardarAccionActual(TAccion *accion);
		void	SacarPersonaje();
		void	MeterPersonaje();
		void	SonidoPersonaje(Acciones p_accion);
};

//------------------------------------------------------------------------------
#endif