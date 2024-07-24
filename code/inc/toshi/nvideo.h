#ifndef N_VIDEO_H
#define N_VIDEO_H
//------------------------------------------------------------------------------
/**
    @class classname

    @brief a brief description of the class

    a detailed description of the class

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_CSPRITE_H
#include "CSprite/ncsprite.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#undef N_DEFINES
#define N_DEFINES nVideo
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------

class nScriptServer;
class nCSprite;
class nOpcion;
class nVideo : public nRoot
{
	struct T_Trigger
	{
		float tiempo;
		nString accion;
	};
	public:
	
		// constructor por defecto
		nVideo();
		// destructor
		virtual ~nVideo();
		// persistency

		// EVENTOS DEL V�DEO

		// Al mostrar un v�deo
		void Mostrar();
		// Al esconder el v�deo
		void Ocultar();

		// Actualiza el v�deo que se va a ejecutar
		void SetVideo(const char *accion);

		// Actualiza la acci�n que se va a ejecutar para destruir el video
		void SetDestructorVideo(const char *accion);

		// Actualiza la acci�n que se va a ejecutar despu�s del video
		void SetAccionFinal(const char *accion);

		// EVENTOS TEMPORALES

		// Agrega un evento temporal al men�
		void AgregarTrigger(float tiempo,const char *accion);		

		// Actualiza el tiempo del men�
		void Tick(float t);

		virtual bool SaveCmds( nPersistServer* persistServer );
		// pointer to nKernelServer
		static nKernelServer* kernelServer;

	private:
		nAutoRef<nScriptServer> scriptServer;
		nString accionVideo;
		nString accionFinal;
		nString destructorVideo;
		nArray<T_Trigger *> listaTriggers;
		float TiempoInicial;
		float TiempoAnterior;
		bool activo;

		// Ejecuta un trigger si ha pasado su tiempo de ejecuci�n
		void EjecutarTrigger(int trigger, float tiempo);
};

//------------------------------------------------------------------------------
#endif