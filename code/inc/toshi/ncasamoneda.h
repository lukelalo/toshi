#ifndef N_CASAMONEDA_H
#define N_CASAMONEDA_H
//------------------------------------------------------------------------------
/**
    @class classname

    @brief a brief description of the class

    a detailed description of the class

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_EDIFICIO_H
#include "toshi/nedificio.h"
#endif

#undef N_DEFINES
#define N_DEFINES nCasaMoneda
#include "kernel/ndefdllclass.h"

#define DISTANCIA_A_CASAMONEDA    1.6


class nCasaMoneda : public nEdificio
{
	public:

		/// constructor
		nCasaMoneda();
		/// destructor
		virtual ~nCasaMoneda();

		/// persistency
		virtual bool SaveCmds( nPersistServer *persistServer );
		/// pointer to nKernelServer
		static nKernelServer *kernelServer;

		// Posiciona el edificio
		virtual void SetPos( float x, float y );
		// Crea el edificio y lo pinta
		virtual void Pintar();
		// Selecciona la casa de la moneda
		virtual void Seleccionar();
		// Actualiza el estado
		virtual void Tick( float dt );
		// Rango de seleccion
		bool EstaEnRango( vector3 posicion );

	private:

		nString  ruta;
		float    posicion_x;
		float    posicion_y;
		int      CanalSeleccion;
		float    valorSeleccion;
};

#endif