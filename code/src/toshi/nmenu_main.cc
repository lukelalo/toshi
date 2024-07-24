#define N_IMPLEMENTS nMenu
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nmenu.h"
#include "toshi/nopcion.h"
#include "kernel/nscriptserver.h"
#include "node/n3dnode.h"
#include "node/ntexarraynode.h"
#include "kernel/ntimeserver.h"

nNebulaScriptClass( nMenu, "nroot" );

// Constructor por defecto
nMenu::nMenu() :
    scriptServer(kernelServer, this)
{
    scriptServer		=	"/sys/servers/script";
	TiempoInicial		=	0.0f;
	TiempoAnterior		=	0.0f;
	opcion_seleccionada	=	-1;
	numero_opciones		=	0;
	imagenFondo			=	"";
	nodoRaiz			=	"";
	activo				=	false;
	listaOpciones		=	new nArray<nOpcion *>;
	listaTriggers		=   new nArray<T_Trigger *>;
	listaOpciones->Clear();
	listaTriggers->Clear();
}

// Destructor
nMenu::~nMenu()
{
	for ( int i=0;i<listaOpciones->Size();i++)
	{
		n_free ( listaOpciones->At(i) );
	}
	for ( int i=0;i<listaTriggers->Size();i++)
	{
		n_free ( listaTriggers->At(i) );
	}
	listaOpciones->Clear();
	listaTriggers->Clear();
	n_free( listaOpciones );
	n_free( listaTriggers );
	listaOpciones=NULL;
	listaTriggers=NULL;
    scriptServer		=	"";
	TiempoInicial		=	0.0f;
	TiempoAnterior		=	0.0f;
	opcion_seleccionada	=	-1;
	numero_opciones		=	0;
	imagenFondo			=	"";
	nodoRaiz			=	"";
}

void nMenu::Mostrar() 
{
	this->TiempoInicial=this->TiempoAnterior;
	n3DNode *fondo=(n3DNode *) kernelServer->Lookup(this->nodoRaiz.Get());
	fondo->SetActive(true);
	for (int i=0;i<this->listaOpciones->Size();i++) 
	{
		this->listaOpciones->At(i)->Mostrar();
	}
	this->activo=true;
}

void nMenu::Ocultar() 
{
	this->TiempoInicial		=	0.0f;
	n3DNode *fondo=(n3DNode *) kernelServer->Lookup(this->nodoRaiz.Get());
	fondo->SetActive(false);
	for (int i=0;i<this->listaOpciones->Size();i++) 
	{
		this->listaOpciones->At(i)->Ocultar();
	}
	this->activo=false;

	listaTriggers->Clear();
}

void nMenu::Siguiente() 
{
	this->opcion_seleccionada++;
	this->opcion_seleccionada%=numero_opciones;

	listaOpciones->At(this->opcion_seleccionada)->onSelect();
}

void nMenu::Anterior()
{
	this->opcion_seleccionada--;
	if (this->opcion_seleccionada<0)
		this->opcion_seleccionada=numero_opciones-1;

	listaOpciones->At(this->opcion_seleccionada)->onSelect();
}

void nMenu::MoverRaton(float x, float y) 
{
	for (int i=0;i<listaOpciones->Size();i++) 
	{
		if (listaOpciones->At(i)!=NULL) 
		{
			if (listaOpciones->At(i)->CheckMousePosition(x,y))
				listaOpciones->At(i)->onMouseOver();
			else
				listaOpciones->At(i)->onMouseOut();
		}
	}
}

void nMenu::Seleccionar() 
{
	listaOpciones->At(this->opcion_seleccionada)->onClick();
}

void nMenu::Click(float x, float y) 
{
	for (int i=0;i<listaOpciones->Size();i++) 
	{
		if (listaOpciones->At(i)!=NULL) 
		{
			if (listaOpciones->At(i)->CheckMousePosition(x,y)) 
			{
				listaOpciones->At(i)->onClick();
				break;
			}
		}
	}
}

bool nMenu::GetActivo()
{
	return this->activo;
}

void nMenu::SetImagenFondo(const char *imagen) 
{
	char buf[N_MAXPATH];
	this->imagenFondo=imagen;
	sprintf(buf,"%s/tex",this->nodoRaiz.Get());
	nTexArrayNode *fondo = (nTexArrayNode *) kernelServer->Lookup(buf);
	fondo->SetTexture(0,imagen,NULL);
	fondo->SetHighQuality(0,true);
	fondo->Preload();
}

void nMenu::SetNodoRaiz(const char *nodo) 
{
	this->nodoRaiz=nodo;
	char buf[N_MAXPATH];
	n3DNode *imagen=(n3DNode *) kernelServer->New("n3dnode",this->nodoRaiz.Get());
	imagen->SetActive(false);
	imagen->Preload();
	sprintf(buf,"%s/tex",this->nodoRaiz.Get());
	nTexArrayNode *tex = (nTexArrayNode *) kernelServer->New("ntexarraynode",buf);
}

void nMenu::GenerarSprite() 
{
	char buf[N_MAXPATH];
	sprintf(buf,"%s/sprite",this->nodoRaiz.Get());
	nCSprite *sprite=(nCSprite *) kernelServer->New("nCSprite",buf);
	sprite->setAutoScale(false);
	sprite->setOrder(50);
	sprite->setScreenPositionf(0,0);
	sprite->setScreenSizef(1.0f,1.0f);
	sprite->Preload();
	for (int i=0; i<this->listaOpciones->Size();i++) 
	{
		this->listaOpciones->At(i)->GenerarSprite();
	}
}


int nMenu::AgregarOpcion(const char *accion) 
{
	nOpcion *opcion=new nOpcion;
	
	int pos=listaOpciones->Size();

	char buf[N_MAXPATH];
	sprintf(buf,"%s/opcion%d",this->nodoRaiz.Get(),pos);
	opcion->SetNodo(buf);
	opcion->SetAccion(accion);

	listaOpciones->PushBack(opcion);
	return pos;
}

void nMenu::AgregarTrigger(float tiempo, const char *accion)
{
	T_Trigger *trigger=new T_Trigger;
	trigger->accion=accion;
	trigger->tiempo=tiempo+(float)kernelServer->ts->GetTime();
	listaTriggers->PushBack(trigger);
}

void nMenu::ModificarAccionOpcion(int num_opcion, const char *accion)
{
	nOpcion* opcion=listaOpciones->At(num_opcion);
	opcion->SetAccion(accion);
}

void nMenu::ModificarPosicionOpcion(int num_opcion, float posx, float posy)
{
	nOpcion* opcion=listaOpciones->At(num_opcion);
	opcion->SetPosicion(posx,posy);
}

void nMenu::ModificarDimensionesOpcion(int num_opcion, float ancho, float alto)
{
	nOpcion* opcion=listaOpciones->At(num_opcion);
	opcion->SetDimensiones(ancho,alto);
}

void nMenu::ModificarImagenPrincipalOpcion(int num_opcion, const char *imagen)
{
	nOpcion* opcion=listaOpciones->At(num_opcion);
	opcion->SetImagenPrincipal(imagen);
}

void nMenu::ModificarImagenOverOpcion(int num_opcion, const char *imagen)
{
	nOpcion* opcion=listaOpciones->At(num_opcion);
	opcion->SetImagenOver(imagen);
}

void nMenu::ModificarImagenOpcionSeleccionada(int num_opcion, const char *imagen)
{
	nOpcion* opcion=listaOpciones->At(num_opcion);
	opcion->SetImagenSeleccion(imagen);
}


void nMenu::Tick(float t) 
{
	float TiempoActual=t;
	if (this->activo) {
		for (int i=0;i<this->listaTriggers->Size();i++) 
		{
			if (this->listaTriggers->At(i)!=NULL)
			{
				// n_printf("Miramos Trigger: Tiempo Actual=%f, TiempoInicial=%f\n",TiempoActual, TiempoInicial);
				EjecutarTrigger(i,TiempoActual);
			}
		}
	}
	TiempoAnterior=TiempoActual;
}

void nMenu::EjecutarTrigger(int trigger,float tiempo) 
{
	T_Trigger *trigger_actual=listaTriggers->At(trigger);
	if (trigger_actual->tiempo<=tiempo) 
	{
		// n_printf("Ejecutamos el trigger %s en el tiempo %f\n",trigger_actual->accion.Get(),tiempo);
		// Ejecutamos la acción del trigger
		const char* result;
		scriptServer->Run(trigger_actual->accion.Get(), result);		
		// Borramos el trigger
		n_free(listaTriggers->At(trigger));
		listaTriggers->At(trigger)=NULL;
	}
}