#ifndef N_DISTRITO_H
#define N_DISTRITO_H
//------------------------------------------------------------------------------
/**
    @class classname

    @brief a brief description of the class

    a detailed description of the class

    (C) 2001 RadonLabs GmbH
*/

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#undef N_DEFINES
#define N_DEFINES nDistrito
#include "kernel/ndefdllclass.h"

#define RADIO_DISTRITO 4

#include <list>

using namespace std;

//------------------------------------------------------------------------------
class nAyuntamiento;
class nPlano;

class nDistrito : public nRoot
{
	public:

		/// constructor
		nDistrito();
		/// destructor
		virtual ~nDistrito();
		
		// Agrega una construcción al distrito
		void AgregarConstruccion( nPlano* &plano );
		// Elimina una construcción del distrito
		void EliminarConstruccion( int construccion );
		// Agrega el ayuntamiento al distrito
		void AgregarAyuntamiento( int id_ayuntamiento );

		// Devuelve una posición válida dentro del distrito para construir
		vector2 BuscarPosicionConstruccion();

		// Devuelve el número de construcciones que tiene el distrito
		int NumeroConstrucciones();

		// Devuelve cierto si una posición está dentro del distrito
		bool DentroDelDistrito( float x, float y );

		// Tratan la seleccion de posición del distrito (a la hora de construir);
		void MostrarSeleccion();
		void OcultarSeleccion();

		// Devuelve cierto si existe un edificio con el mismo nombre
		bool ExisteEdificio (string p_nombre);

		// Devuelve el plano de un edificio con el identificador
		nPlano *GetPlano (int p_id_plano);

		// Obtener la lista de planos de la casa de planos
		//list<nPlano*> GetListaPlanos();

		// Obtener la lista de planos de la casa de planos
		nArray<nPlano*> GetListaPlanos();

		// Devuelve un puntero al edificio que está en la posición pasada por parámetro en la lista
		nPlano* GetEdificio( int posicion );

		/// persistency
		virtual bool SaveCmds( nPersistServer* persistServer );
		// Identificador del edificio
		static nKernelServer* kernelServer;

	private:

		nString			ayuntamiento;
		//list<nPlano*>	planos_construidos;
		nArray<nPlano*> lista_planos;
};

//------------------------------------------------------------------------------
#endif