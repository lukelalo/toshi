#define N_IMPLEMENTS nVideo
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nvideo.h"
#include "toshi/nopcion.h"
#include "kernel/nscriptserver.h"

nNebulaScriptClass( nVideo, "nroot" );

// Constructor por defecto
nVideo::nVideo() :
    scriptServer(kernelServer, this)
{
    scriptServer = "/sys/servers/script";
	TiempoInicial	=	0.0f;
	TiempoAnterior	=	0.0f;
	accionVideo		=	"";
	accionFinal		=	"";
	destructorVideo	=	"";
	activo			=	false;
}

// Destructor
nVideo::~nVideo()
{
	for ( int i=0;i<listaTriggers.Size();i++)
	{
		n_free ( listaTriggers.At(i) );
	}
	listaTriggers.Clear();
}

void nVideo::Mostrar() 
{
	this->TiempoInicial=this->TiempoAnterior;
	const char* result;
	scriptServer->Run(accionVideo.Get(), result);
	this->activo=true;
}

void nVideo::Ocultar() 
{
	this->TiempoInicial		=	0.0f;
	const char* result;
	scriptServer->Run(destructorVideo.Get(), result);	
	scriptServer->Run(accionFinal.Get(), result);	
	this->activo=false;
	this->listaTriggers.Clear();
}


void nVideo::SetVideo(const char *accion) 
{
	this->accionVideo=accion;
}


void nVideo::SetAccionFinal(const char *accion) 
{
	this->accionFinal=accion;	
}

void nVideo::SetDestructorVideo(const char *accion) 
{
	this->destructorVideo=accion;	
}

void nVideo::AgregarTrigger(float tiempo, const char *accion)
{
	T_Trigger *trigger=new T_Trigger;
	trigger->accion=accion;
	trigger->tiempo=tiempo;
	listaTriggers.PushBack(trigger);
}

void nVideo::Tick(float t) 
{
	float TiempoActual=t;
	if (this->activo) {
		for (int i=0;i<this->listaTriggers.Size();i++) 
		{
			if (this->listaTriggers.At(i)!=NULL)
				EjecutarTrigger(i,TiempoActual-TiempoInicial);
		}	
	}
	TiempoAnterior=TiempoActual;
}

void nVideo::EjecutarTrigger(int trigger,float tiempo) 
{
	T_Trigger *trigger_actual=listaTriggers.At(trigger);
	if (trigger_actual->tiempo<=tiempo) 
	{
		// TODO:Ejecutamos la acción del trigger
		const char* result;
		scriptServer->Run(trigger_actual->accion.Get(), result);			
		// Borramos el trigger
		n_free(listaTriggers.At(trigger));
		listaTriggers.At(trigger)=NULL;
	}
}