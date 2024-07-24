#ifndef N_SHOGUN_H
#define N_SHOGUN_H
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

#ifndef N_PERSONAJE_H
#include "toshi/npersonaje.h"
#endif

#undef N_DEFINES
#define N_DEFINES nShogun
#include "kernel/ndefdllclass.h"

class nCampesino;
class nJugador;

//------------------------------------------------------------------------------
class nShogun : public nPersonaje
{
	public:
	
		// constructor por defecto
		nShogun();
		// constructor general
		nShogun( nJugador* jugador_shogun );
		// destructor
		virtual ~nShogun();
		// persistency
		virtual bool SaveCmds( nPersistServer* persistServer );
		// Jugador que controla al personaje
		nJugador* GetJugador();
		// Contratar un campesino
		void ContratarCampesino( nCampesino* &campesino );
		// Liberar un campesino
		void LiberarCampesino( nCampesino* &campesino );
		// Resucitar un campesino
		void ResucitarCampesino( nCampesino* &campesino );
		// Destruir un edificio
		void DestruirEdificio( nConstruccion* &construccion );
		// Matar al personaje apuntado
		void Matar( nEntity* &personaje_a_matar );
		// Robar al personaje apuntado
		int Robar( nEntity* &personaje_a_robar );
		// pointer to nKernelServer
		static nKernelServer* kernelServer;

	private:

		nJugador*  jugador;
};

//------------------------------------------------------------------------------
#endif