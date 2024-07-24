#ifndef N_CAMPESINO_H
#define N_CAMPESINO_H
//------------------------------------------------------------------------------
/**
    @class classname

    @brief a brief description of the class

    a detailed description of the class

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_PERSONAJE_H
#include "toshi/npersonaje.h"
#endif

#ifndef N_MAQUINAESTADOS_H
#include "toshi/maquinaestados.h"
#endif

#undef N_DEFINES
#define N_DEFINES nCampesino
#include "kernel/ndefdllclass.h"


class nMaquinaEstados;

class nCampesino : public nPersonaje
{
    public:

		// constructor
		nCampesino();
        // destructor
        virtual ~nCampesino();

        // persistency
        virtual bool SaveCmds( nPersistServer *persistServer );
        // pointer to nKernelServer
        static nKernelServer *kernelServer;

		// Identificativo del campesino
		void SetId( const int id_campesino );
		const int GetId() const;
		// Oro del campesino
		void SetOro( int oro );
		int GetOro();
		// �Est� contratado el campesino?
		bool EstaContratado();
		// Campesino contratado
		void Contratar( nEntity *personaje );
		// Campesino liberado
		void Liberar();
		// Campesino robado
		void Robar();
		// Lo que cuesta contratar al campesino
		int GetPrecio();
		// Lo que cuesta mantener al campesino
		int GetSueldo();
		// Aumenta el n�mero de colisiones del campesino con otros campesinos
		void Colisionar();
		// Devuelve el n�mero de colisiones del campesino
		int GetNumColisiones();
		// Campesino muerto
		void Matar();
		// �Est� muerto el campesino?
		bool EstaMuerto();
		// Campesino resucitado
		void Resucitar();
		// �Ha alcanzado el campesino el destino?
		bool FinDeTrayecto();
		// �Ha terminado el campesino el reposo?
		bool FinDeEspera();
		// Desencadena la acci�n asociada al estado del campesino
		void UpdateEstado( float dt );
		// Called in a loop
		void Tick( float dt );
		// El personaje que contrat� al campesino
		nEntity *GetManager();

    private:

		nEntity		 	 *manager;
		nMaquinaEstados  maqEstados;
		float			 reposo;
		float			 reposoActual;
		int	  			 Id;
		int				 Oro;
		int				 nodoActual;
		int				 Precio;
		int              Sueldo;
		int				 num_colisiones;
		bool			 Muerto;
		bool			 Contratado;

		void actualizarMaquinaEstados();
		void ejecutarComportamiento( float dt );
};

#endif