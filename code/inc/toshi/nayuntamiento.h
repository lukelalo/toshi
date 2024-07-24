#ifndef N_AYUNTAMIENTO_H
#define N_AYUNTAMIENTO_H
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
#define N_DEFINES nAyuntamiento
#include "kernel/ndefdllclass.h"

#define DISTANCIA_AL_AYUNTAMIENTO    1.2


class nAyuntamiento : public nEdificio
{
	public:

		/// constructor general
		nAyuntamiento();
		/// destructor
		virtual ~nAyuntamiento();

		/// persistency
		virtual bool SaveCmds( nPersistServer *persistServer );
		/// pointer to nKernelServer
		static nKernelServer *kernelServer;

		// Identifica al edificio
		virtual void SetId( int id );
		// Obtiene la identificación del edificio
		int GetId();
		// Posiciona al edificio
		virtual void SetPos( float x, float y );
		// Actualiza el estado
		virtual void Tick( float dt );
		// Crea el edificio y pintarlo
		virtual void Pintar();
		// Selecciona el ayuntamiento
		virtual void Seleccionar();
		// Rango de selección
		bool EstaEnRango( vector3 posicion );

	private:

		nString  ruta;
		int      id_ayuntamiento;
		float    posicion_x;
		float    posicion_y;
		int      CanalSeleccion;
		float    valorSeleccion;
};

#endif