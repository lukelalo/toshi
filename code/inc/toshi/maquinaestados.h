#ifndef N_MAQUINAESTADOS_H
#define N_MAQUINAESTADOS_H

#undef N_DEFINES
#define N_DEFINES nMaquinaEstados
#include "kernel/ndefdllclass.h"


class nMaquinaEstados
{
	public:

		enum TAlfabetoEntrada
		{
			MUERTO,
			CONTRATADO,
			LIBERADO,
			FIN_TRAYECTO,
			FIN_ESPERA,
			RESUCITADO
		};

		enum TEstado
		{
			PARAR,
			MORIR,
			SEGUIR,
			DEAMBULAR
		};

		bool siguienteEstado( TAlfabetoEntrada in );
		TEstado GetEstado();
		void SetEstado( TEstado estado );

    private:

		TEstado  _estadoActual;
};

#endif