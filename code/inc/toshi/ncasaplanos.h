#ifndef N_CASAPLANOS_H
#define N_CASAPLANOS_H
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

#ifndef N_NPLANO_H
#include "toshi/nplano.h"
#endif

#undef N_DEFINES
#define N_DEFINES nCasaPlanos
#include "kernel/ndefdllclass.h"

#include "file/nnpkfileserver.h"

#include <list>
#include <string>

using namespace std;

#define DISTANCIA_A_CASAPLANOS    1.2


class nJugador;
class nPlano;

class nCasaPlanos : public nEdificio
{
	public:

		/// constructor general
		nCasaPlanos();
		/// destructor
		virtual ~nCasaPlanos();

		/// persistency
		virtual bool SaveCmds( nPersistServer *persistServer );
		// Posición del edificio
		/// pointer to nKernelServer
		static nKernelServer *kernelServer;

		virtual void SetPos( float x, float y );
		// Actualiza el estado
		virtual void Tick( float dt );
		// Crear el edificio y pintarlo
		virtual void Pintar();
		// Seleccionar la casa de planos
		virtual void Seleccionar();
		// Pone la ruta del fichero de planos en la casa de planos
		virtual void SetRutaFichero( const char *ruta );
		// Cargar fichero de texto que contiene los planos
		virtual void CargaFicheroPlanos();
		// Obtener el número de planos que existe en la casa de planos
		virtual inline int NumeroDePlanos();
		// Obtener el último plano de la casa de planos
		virtual bool GetPlano( nPlano* &plano );
		// Obtener la lista de planos de la casa de planos
		list<nPlano *> GetListaPlanos();
		// Inserta en la última posición de la lista de planos
		void InsertaPlano( nPlano* &plano );
		// Lee todas las posiciones de la lista de planos
		void LeePlanos();
		// Rango de seleccion
		bool EstaEnRango( vector3 posicion );

	private:

		float                    posicion_x;
		float                    posicion_y;
		list<nPlano *>           lista_planos;
		nString                  ruta_fichero;
		nAutoRef<nNpkFileServer> refFileServer;
		int                      CanalSeleccion;
		float                    valorSeleccion;

		// Convierte un valor string en uno TipoPlano
		nPlano::TPlano TipoDePlano( string tipo_plano );
};


inline int nCasaPlanos::NumeroDePlanos()
{
	return lista_planos.size();
}

#endif