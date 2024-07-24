#ifndef N_OPCION_H
#define N_OPCION_H
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

#undef N_DEFINES
#define N_DEFINES nOpcion
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------

/*------------------------------------------------------------------------------

Clase nOpcion
Opción de un menú con sus eventos

------------------------------------------------------------------------------*/
class nScriptServer;
class nTexArrayNode;
class nCSprite;
class n3DNode;
class nOpcion : public nRoot
{
	public:
		// constructor por defecto
		nOpcion();
		// destructor
		virtual ~nOpcion();

		// EVENTOS DE LA OPCIÓN

		// Al pasar por encima de la opción
		void onMouseOver();
		// Al salir de encima de la opción
		void onMouseOut();
		// Al seleccionar una opción
		void onSelect();
		// Al ejecutar la opción
		void onClick();
		// Modifica la imagen principal de la opción

		// Mostrar una opción
		void Mostrar();
		// Ocultar la opción
		void Ocultar();

		void SetNodo(const char *nodo);

		void GenerarSprite();

		// Modifica la imagen principal de la opción
		void SetImagenPrincipal(const char *imagen);
		// Modifica la imagen secundaria de la opción (al pasar el ratón por encima)
		void SetImagenOver(const char *imagen);
		// Modifica la imagen al seleccionar la opción
		void SetImagenSeleccion(const char *imagen);
		// Modifica la acción que se ejecuta cuando se llama a la opción
		void SetAccion(const char *accion);
		// Modifica la posición de la opción en la pantalla
		void SetPosicion(float posx,float posy);
		// Modifica las dimensiones de la opción
		void SetDimensiones(float ancho,float alto);

		// FUNCIONES DE COMPROBACIÓN DE POSICIÓN
		// Al pasar por encima de la opcion devuelve true
		bool CheckMousePosition(float posx, float posy);
		// Al encontrarse seleccionada devuelve true
		bool CheckSelected();

		virtual bool SaveCmds( nPersistServer* persistServer );

		// pointer to nKernelServer
		static nKernelServer* kernelServer;

	private:
		nAutoRef<nScriptServer> scriptServer;
		nString	nodoSprite;
		nString imagenOpcion;
		nString imagenOver;
		nString imagenSeleccionado;
		nString accionAEjecutar;
		float posx;
		float posy;
		float ancho;
		float alto;
		bool mouseOver;
		bool selected;
		bool creadoSprite;
};
#endif