#ifndef N_IAJUGADOR_H
#define N_IAJUGADOR_H
//------------------------------------------------------------------------------
/**
    @class nIAJugador

    @brief Clase que representa un controlador de inteligencia artificial.

	Es llamado por el bucle principal (nGame), a intervalos constantes.
	Permite enganchar un script, que se ejecuta cada vez. Para comportamientos
	específicos programados en C++, habrá que heredar de esta clase.

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

#include "kernel/nscriptserver.h"

#undef N_DEFINES
#define N_DEFINES nIAJugador
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nJugador;
class nPersonaje;
class nGame;
class nPartida;
class N_PUBLIC nIAJugador : public nRoot
{
	enum T_Accion 
	{
		ACCION_COGER_PERSONAJE,
		ACCION_ROBAR_PERSONAJE_MAS_CERCANO,
		ACCION_COGER_PLANO,
		ACCION_LIBERAR_PERSONAJE,
		ACCION_MATAR_JUGADOR_MAS_CERCANO,
		ACCION_MATAR_JUGADOR_MEJOR,
		ACCION_ROBAR_PERSONAJE_MEJOR,
		ACCION_RESUCITAR,
		ACCION_RECAUDAR_FUERA,
		ACCION_CONSTRUIR_PLANO,
		ACCION_COGER_DINERO,
		ACCION_INTERCAMBIAR_PLANOS,
		ACCION_INTERCAMBIAR_PLANOS_CON_JUGADOR,
		ACCION_DESTRUIR_EDIFICIO,
		ACCION_BORRAR_PERSONAJE,
		ACCION_IR_A_AYUNTAMIENTO,
		ACCION_RECAUDAR,
		ACCION_CONTRATAR
	};

	enum T_Comportamiento
	{
		NEUTRAL,
		CONSTRUCTIVO,
		DESTRUCTIVO
	};

	public:

		/// constructor
		nIAJugador();
		/// destructor
		virtual ~nIAJugador();
		/// persistency
		virtual bool SaveCmds(nPersistServer* ps);

		// Trigger. Realiza la IA. Esta clase básica, lo que hace es
		// llamar al script que tiene asociado (si tiene alguno...)
		virtual void Trigger();

		// Función para saber si es rentable ir a la casa de la moneda
		bool InteresaDinero();
		// Función para saber si es rentable ir a la casa de planos
		bool InteresaPlano();
		// Función para saber si es rentable ir a cambiar planos
		bool PuedeCambiarPlanos();
		// Función para saber si se puede recaudar en el ayuntamiento
		bool PuedeRecaudar();
		// Función para saber si se puede recaudar en otro ayuntamiento
		bool PuedeRecaudarFuera();
		// Función para saber si se puede construir un edificio
		bool PuedeConstruir();
		// Función para saber si se puede destruir un edificio
		bool PuedeDestruir();
		// Función para saber si se puede resucitar a un campesino
		bool PuedeResucitar();
		// Cambia el comportamiento del jugador IA
		void SetComportamiento( T_Comportamiento estado );
		// Obtiene el comportamiento del jugador IA
		T_Comportamiento GetComportamiento();

		/// pointer to nKernelServer
		static nKernelServer* kernelServer;

	private:

		nAutoRef<nScriptServer> refScriptServer;
		nAutoRef<nGame>			refGame;
		nArray<T_Accion>		acciones_jugador;
		nPersonaje::TPersonaje	personajeSeleccionado;
		nPartida				*partida;
		nJugador				*jugador;
		T_Comportamiento        comportamiento;

		// Encola las acciones a realizar del jugador
		void EncolarAccion(T_Accion accion);
		// Ejecuta la acción siguiente que tenga en la cola
		void EjecutarSiguienteAccion();
		// Elige el personaje que va a coger en el siguiente turno
		bool EscogerPersonaje();
		// Escoge un personaje al azar de los que queden libres
		bool PersonajeAlAzar();
};

#endif
