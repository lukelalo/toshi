#ifndef N_MENU_H
#define N_MENU_H
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
#define N_DEFINES nMenu
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------

class nScriptServer;
class nTexArrayNode;
class n3DNode;
class nOpcion;
class nMenu : public nRoot
{
	struct T_Trigger
	{
		float tiempo;
		nString accion;
	};
	public:
	
		// constructor por defecto
		nMenu();
		// destructor
		virtual ~nMenu();
		// persistency

		// EVENTOS DEL MENÚ

		// Al mostrar un menú
		void Mostrar();
		// Al esconder el menú
		void Ocultar();
		// Al hacer click en alguna parte del menu
		void Click (float x, float y);
		// Al Seleccionar el siguiente elemento del menú
		void Siguiente ();
		// Al Seleccionar el anterior elemento del menú
		void Anterior ();
		// Al seleccionar una opción
		void Seleccionar ();
		// Al mover el raton por el menú
		void MoverRaton(float x, float y);

		// Devuelve true si el menú está activo
		bool GetActivo();

		// Actualiza la imagen de fondo del menú
		void SetImagenFondo(const char *imagen);

		// TRATAMIENTO DE OPCIONES

		// Almacena el nodo raíz del menú
		void SetNodoRaiz(const char *nodo);

		// Genera el objeto sprite del fondo
		void GenerarSprite();

		// Crea una nueva opción del menú
		int AgregarOpcion(const char *accion);
		// Modifica la imagen principal de la opción
		void ModificarImagenPrincipalOpcion(int num_opcion,const char *imagen);
		// Modifica la imagen secundaria de la opción (al pasar el ratón por encima)
		void ModificarImagenOverOpcion(int num_opcion,const char *imagen);
		// Modifica la imagen al seleccionar la opción
		void ModificarImagenOpcionSeleccionada(int num_opcion,const char *imagen);
		// Modifica la acción que se ejecuta cuando se llama a la opción
		void ModificarAccionOpcion(int num_opcion,const char *accion);
		// Modifica la posición de la opción en la pantalla
		void ModificarPosicionOpcion(int num_opcion,float posx,float posy);
		// Modifica las dimensiones de la opción
		void ModificarDimensionesOpcion(int num_opcion,float ancho,float alto);

		// EVENTOS TEMPORALES

		// Agrega un evento temporal al menú
		void AgregarTrigger(float tiempo,const char *accion);		

		// Actualiza el tiempo del menú
		void Tick(float t);

		virtual bool SaveCmds( nPersistServer* persistServer );
		// pointer to nKernelServer
		static nKernelServer* kernelServer;

	private:
		nAutoRef<nScriptServer> scriptServer;
		nString imagenFondo;
		nString nodoRaiz;
		nArray <nOpcion *> *listaOpciones;
		nArray <T_Trigger *> *listaTriggers;
		float TiempoInicial;
		float TiempoAnterior;
		int numero_opciones;
		int opcion_seleccionada;
		bool activo;

		// Ejecuta un trigger si ha pasado su tiempo de ejecución
		void EjecutarTrigger(int trigger, float tiempo);
};

//------------------------------------------------------------------------------
#endif