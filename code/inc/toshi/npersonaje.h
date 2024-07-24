#ifndef N_PERSONAJE_H
#define N_PERSONAJE_H
//------------------------------------------------------------------------------
/**
    @class classname

    @brief a brief description of the class

    a detailed description of the class

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_ENTITY_H
#include "toshi/nentity.h"
#endif

#undef N_DEFINES
#define N_DEFINES nPersonaje
#include "kernel/ndefdllclass.h"

#define DISTANCIA_DE_ACCION 0.5
#define DISTANCIA_DE_ALCANCE 0.45
#define DISTANCIA_DE_SEGUIMIENTO 1.5

#include <list>

using namespace std;

//------------------------------------------------------------------------------
class nCollideObject;
class nCollideShape;
class nWorld;
class nConstruccion;
class nJugador;
class nPersonaje : public nEntity
{
	public:

		struct nodo
		{
			int posx;
			int posy;
		};

		enum TAnim
		{
			ANIM_QUIETO,
			ANIM_ANDAR,
			ANIM_CORRER,
			ANIM_COGER,
			ANIM_MORIR,
			ANIM_ACCION,
			ANIM_RESUCITAR,
			ANIM_CAMBIAR_PERSONAJE,
			ANIM_LIBERAR_PERSONAJE,
			ANIM_CONSTRUIR,
			ANIM_RESUCITAR_A,
			ANIM_CAMBIAR_PLANOS,
			ANIM_DESTRUIR
		};

		enum TPersonaje
		{
			PERSONAJE_KUNOICHI,
			PERSONAJE_LADRONA,
			PERSONAJE_SHOGUN,
			PERSONAJE_SHUGENJA,
			PERSONAJE_MERCADER,
			PERSONAJE_MAESTRO,
			PERSONAJE_INGENIERO,
			PERSONAJE_SAMURAI,
			PERSONAJE_CAMPESINO
		};

		enum Comportamiento
		{
			QUIETO,
			BUSQUEDA,
			BUSQUEDA_EDIFICIO,
			SEGUIMIENTO,
			SEGUIMIENTO_JUGADOR,
			EVITAR_OBSTACULO,
			MUERTO
		};

		// constructor
		nPersonaje();
		// destructor
		virtual ~nPersonaje();
		// persistency
		virtual bool SaveCmds( nPersistServer* persistServer );
		// Posición del personaje
		virtual void SetPosicion( float pos_x, float pos_y );
		// Tiempo de acción del personaje
		virtual void SetTiempo( float tiempo );
		float GetTiempo();
		// Tiempo de pago del personaje al campesino
		virtual void SetTiempoPago( float tiempo );
		float GetTiempoPago();
		// Nombre del personaje
		virtual void SetNombre( const char* nombre_personaje );
		const char* GetNombre() const;
		// Número de campesinos contratados por el personaje
		virtual void SetCampesinosContratados( int campesinos );
		int GetCampesinosContratados();
		// Contratar un campesino
		virtual void ContratarCampesino( int id_campesino );
		// Liberar un campesino
		virtual void LiberarCampesino( int id_campesino );
		// Resucitar un personaje
		virtual void ResucitarCampesino( int id_campesino );
		// Destruir un edificio
		virtual void DestruirEdificio( nConstruccion* &construccion );
		// Matar a un personaje
		virtual void Matar( nEntity* &personaje_a_matar );
		// Robar a un personaje
		virtual int Robar( nEntity* &personaje_a_robar );
		// Cambia la animación actual del personaje
		void SetAnimacion( TAnim animacionNueva);
		TAnim GetAnimacion();

		// Devuelve el tipo de personaje
		TPersonaje GetTipoPersonaje();

		Comportamiento GetComportamiento();
		// Cambia el nodo de visibilidad
		void SetVisNode(const char* visnode_path);
		// Mueve el personaje a una posición
		void IrA(bool actualiza, float x, float z);
		// Mueve el personaje a una posición sin A*
		void IrARapido(bool actualiza, float x, float z);
		// Mueve el personaje a una posición cercana a un edificio
		void IrAEdificio(bool actualiza, nEntity *entidad);
		// Sigue a una entidad
		void SeguirA(bool actualiza, nEntity *entidad);
		// Sigue a unn jugador
		void SeguirAJugador(bool actualiza, nJugador *jugador);
		// Sigue a una entidad desde mayor rango
		void SeguirDeLejos(bool actualiza, nEntity *entidad);
		// Devuelve si la animación del personaje ha acabado
		bool AnimacionTerminada();
		// Obtiene la lista de campesinos contratados por el personaje
		nArray<int> GetListaCampesinosContratados();
		// POSICIONAMIENTO
		virtual bool EstaEnRango(vector3 posicion);
		virtual bool EstaEnRangoDeAccion(vector3 posicion);
		virtual bool EstaEnRangoDeSeguimientoLejano(vector3 posicion);
		virtual bool EstaEnRangoDePersecucion(vector3 posicion);

		// Actualiza las animaciones de un personaje
		void ActualizarAnimaciones();
		// Called in a loop
		void Tick(float dt);
		// pointer to nKernelServer
		static nKernelServer* kernelServer;
		// Paran y activan al personaje
		void Correr();
		void Mover();
		void Parar();
		void OcultarSombra();
		void MostrarSombra();
		// Se prugunta por si el campesino está contratado por el personaje
		bool CampesinoMio( int id_campesino );
		// Asigna un jugador al personaje
		nJugador* GetJugador();
		// Asigna un jugador al personaje
		void SetJugador( nJugador* jugador );
		// Asigna un TRUE al atributo Muerto y pone las animaciones
		void Morir();
		// Asigna un FALSE al atributo Muerto solamente
		void Revivir();
		// Asigna un FALSE al atributo Muerto y pone las animaciones
		void Nacer();
		// Comprueba si el personaje está muerto
		bool EstaMuerto();

	protected:

		TPersonaje TipoPersonaje;

	private:

		void CalcularRutaAEstrella(float x, float z);
		void SiguienteNodo(float x, float z);
		void OrientarSombra();
		Comportamiento estado;
		nArray<int> lista_campesinos;
		nArray<int> lista_animaciones;
		nodo camino[ANCHO_MAPA*2];
		nEntity *entidadDestino;
		nMLoader *o_modelo;
		nJugador* manager;
		nJugador* jugadorDestino;
		nString	Nombre;
		float	Tiempo;
		float   TiempoPago;
		float	PosicionX;
		float	PosicionY;
		TAnim	Animacion;
		int		NumSecuencias;
		int		CampesinosContratados;
		int		nodoActual;
		int		CanalSonido;
		bool    Muerto;
};

//------------------------------------------------------------------------------
#endif