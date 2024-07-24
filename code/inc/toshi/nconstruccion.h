#ifndef N_CONSTRUCCION_H
#define N_CONSTRUCCION_H
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
#define N_DEFINES nConstruccion
#include "kernel/ndefdllclass.h"

#define DISTANCIA_A_CONSTRUCCION 1.2f
#define TIEMPO_CONSTRUCCION		 5.0f

class nJugador;

//------------------------------------------------------------------------------
class nConstruccion : public nEdificio
{
	public:
    
		enum TEstadoConstruccion
		{
			INICIAL,
			EN_CONSTRUCCION,
			CONSTRUIDO,
			EN_DESTRUCCION,
			DESTRUIDO
		};

		/// constructor
		nConstruccion();
		/// destructor
		virtual ~nConstruccion();
		/// persistency
		virtual bool SaveCmds(nPersistServer* persistServer);
		// Posición del edificio
		virtual void SetPos(float x, float y);
		// Crear el edificio y pintarlo
		virtual void Pintar();
		// Seleccionar la construccion
		virtual void Seleccionar();
		// Construir el edificio
		virtual void Construir( nJugador* jugador );
		// Modifica la Altura del Edificio
		virtual void SetValorConstruccion( float valor );
		// Modifica el valor del edificio
		virtual void SetValor(int valor);
		int GetValor();
		// Devuelve cierto si el edificio está construido
		bool EstaConstruido();
		// Destruir el edificio
		virtual void Destruir();
		// Cambia el Shader del edificio
		virtual void CambiarShader(const char * shader);
		// Actualiza el estado
		virtual void Tick(float dt);
		// Rango de seleccion
		bool EstaEnRango(vector3 posicion);
		// Asigna identificador a la construcción
		void SetId( int id );
		// Obtiene el identificador de la construcción
		int GetId();
		// Asigna jugador a la construción
		void SetManager( nJugador* jugador );
		// Obtiene el jugador que posee la construcción
		nJugador* GetManager();
		// Obtiene el nombre de la construccion
		const char *GetNombre();
		void SetNombre(const char *p_nombre);
		// Obtiene el estado de la construcción
		TEstadoConstruccion GetEstado();
		/// pointer to nKernelServer
		static nKernelServer* kernelServer;

	protected:

		TEstadoConstruccion  EstadoConstruccion;

	private:

		nJugador* manager;
		nString ruta;
		nString nombre;
		TEstadoConstruccion estado;
		float posicion_x;
		float posicion_y;
		float valorSeleccion;
		float valorConstruccion;
		int CanalConstruccion;
		int CanalSeleccion;
		int ValorEdificio;
		int id_construccion;	
};
//------------------------------------------------------------------------------
#endif
