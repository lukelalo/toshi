#ifndef N_PARTIDA_H
#define N_PARTIDA_H
//------------------------------------------------------------------------------
/**
    @class nPartida

    @brief Contiene la partida del juego

*/

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#ifndef N_PERSONAJE_H
#include "toshi/npersonaje.h"
#endif

#undef N_DEFINES
#define N_DEFINES nPartida
#include "kernel/ndefdllclass.h"

/**************************************************************************************************
 * Clase que define una partida de TOSHI
 **************************************************************************************************/
#define MAX_TIEMPO_TURNO				60.0f
#define MIN_TIEMPO_TURNO				15.0f

class nJugador;
class nCampesino;
class nGame;
class nEntity;
class nPersonaje;

class nPartida : public nRoot
{
	enum Puntuacion
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
	};

	struct T_Personaje
	{
		nPersonaje *personaje;
		float		tiempo_jugado;
		int			jugador;
		bool		libre;
		bool		libreAnterior;
	};

public:

	nPartida();
	~nPartida();
    /// persistency
    virtual bool SaveCmds(nPersistServer* ps);

	// Comienza una partida en un equipo local, sin red
	void ComenzarPartidaLocal(int num_jugadores);

	// Comienza una partida por red
	void ComenzarPartidaRed(bool servidor);

	// Libera un personaje para que lo coja cualquier otro jugador
	void LiberarPersonaje(int jugador);

	// Indica si un jugador puede liberar un personaje
	bool PuedeLiberarPersonaje(int jugador);

	// Bloquea un personaje para que no lo coja nadie más
	void BloqueaPersonaje(nPersonaje::TPersonaje, int jugador);

	// Devuelve si una partida está activa
	inline bool GetActiva();

	// Devuelve el jugador principal
	nJugador* GetJugadorPrincipal();

	// Devuelve un jugador disponible para la IA
	nJugador* AsignarJugadorIA();

	// Devuelve si el personaje seleccionado se encuentra libre
	bool PersonajeLibre(nPersonaje::TPersonaje);

	// Genera un puntero al juego
	void SetJuego(nGame *p_juego);

	// Devuelve el personaje del jugador mas cercano a la posición de entrada
	nEntity *JugadorMasCercano(float posx, float posy, int id_jugador);

	// Devuelve el personaje del jugador o el campesino mas cercano a la posición de entrada
	nEntity *PersonajeMasCercano(float posx, float posy, int id_jugador);

	// Devuelve el campesino más cercano a la posición de entrada
	nEntity *CampesinoMasCercano(float posx, float posy, int id_jugador);

	// Devuelve el campesino muerto más cercano
	nEntity *CampesinoMuertoMasCercano( float posx, float posy, int id_jugador );
	// Devuelve el personaje del jugador que tenga más edificios construidos
	nJugador *JugadorMejor( int id_jugador );
	// Puntero a la lista de campesinos
	nArray<nCampesino *> *GetCampesinos();
	// Devuelve el ayuntamiento más cercano
	nEntity *AyuntamientoMasCercano( float posx, float posy, int id_jugador );

	// Devuelve un edificio del jugador con mas edificios
	nEntity *EdificioMejorJugador(int id_jugador);

	// Devuelve el ayuntamiento del jugador con mas planos
	nEntity *JugadorConMasPlanos(int id_jugador);

	// Devuelve cierto si están todos los personajes menos uno ocupados
	bool TodosLosPersonajesOcupados();

	// Puntero a la lista de jugadores
	nArray<nJugador*>* GetJugadores();

	// Puntero a la lista de personajes
	nArray<nPartida::T_Personaje*>* GetPersonajes();

	// Devuelve un puntero al jugador que posee al personaje pasado por parámetro
	nJugador* GetJugadorConPersonaje( nPersonaje::TPersonaje );

	// Puntúa a los jugadores con el mayor número de dinero
	void PuntuarPorDinero();

	// Puntúa a los jugadores con el menor tiempo de personaje consumido
	void PuntuarPorTiempo();

	// Puntúa a los jugadores que tengan al menos un edificio construido de cada distintivo
	void PuntuarPorDistintivo();

	// Devuelve si un jugador es jugador principal
	bool EsJugadorPrincipal(int jugador);

	// Devuelve los puntos por acciones del jugador
	int GetPuntosPorAccion(int jugador);

	// Devuelve los puntos por construcciones del jugador
	int GetPuntosPorConstruccion(int jugador);

	// Devuelve el número de construcciones del jugador
	int GetNumConstrucciones(int jugador);

	// Devuelve los puntos por distintivo del jugador
	int GetPuntosPorDistintivo(int jugador);

	// Devuelve los puntos por dinero del jugador
	int GetPuntosPorOro(int jugador);

	// Devuelve los puntos por tiempo del jugador
	int GetPuntosPorTiempo(int jugador);

	// Devuelve el total del tiempo del jugador
	int GetPuntosTotales(int jugador);

	// Asigna los nodos visuales iniciales a los personajes
	void AsignarNodosVisuales();

	// Actualiza la clase partida con el tiempo
	void Tick(float t);

	// Resetea la partida
	void Reset();

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:

	nAutoRef<nScriptServer>  refScriptServer;
	nArray<nJugador *> *jugadores;
	nArray<nCampesino *> *campesinos;
	nArray<nPartida::T_Personaje *> *personajes;
	nGame *juego;
	float TiempoTurno;
	float TiempoAnterior;
	float TiempoTurnoPago;
	float TiempoParaTerminar;
	float TiempoTurnoSiguiente;
	int numero_jugadores;
	int siguienteJugador;
	bool es_servidor;
	bool activa;
	bool TodosOcultos;
	bool FinPartida;

	void RedibujaSeleccionPersonajes(bool redibujarTodos);
	void OcultaTodosLosPersonajes();
	void MostrarPersonajesActivos();
	float CalcularAngulo(vector3 p_dir);

}; // class nPartida

inline bool nPartida::GetActiva()
{
	return this->activa;
}

#endif